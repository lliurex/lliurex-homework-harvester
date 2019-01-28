/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>
 * Copyright (c) 2011 Jannis Pohlmann <jannis@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General 
 * Public License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */



#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libxfce4util/libxfce4util.h>

#include <src/lhh-backend.h>
#include <src/lhh-provider.h>

/* use g_access() on win32 */
#if defined(G_OS_WIN32)
#include <glib/gstdio.h>
#else
#define g_access(filename, mode) access((filename), (mode))
#endif

#include <pwd.h>


static void   lhh_provider_menu_provider_init   (ThunarxMenuProviderIface *iface);
static void   lhh_provider_finalize             (GObject                  *object);
static GList *lhh_provider_get_file_actions     (ThunarxMenuProvider      *menu_provider,
                                                 GtkWidget                *window,
                                                 GList                    *files);

static void   lhh_provider_execute              (LhhProvider              *lhh_provider,
                                                 GPid                    (*action) (const gchar *folder,
                                                                                    GList       *files,
                                                                                    GtkWidget   *window,
                                                                                    GError     **error),
                                                 GtkWidget                *window,
                                                 const gchar              *folder,
                                                 GList                    *files,
                                                 const gchar              *error_message);
static void   lhh_provider_child_watch          (GPid                      pid,
                                                 gint                      status,
                                                 gpointer                  user_data);
static void   lhh_provider_child_watch_destroy  (gpointer                  user_data);



struct _LhhProviderClass
{
  GObjectClass __parent__;
};

struct _LhhProvider
{
  GObject         __parent__;

#if !GTK_CHECK_VERSION(2,9,0)
  /* GTK+ 2.9.0 and above provide an icon-name property
   * for GtkActions, so we don't need the icon factory.
   */
  GtkIconFactory *icon_factory;
#endif

  /* child watch support for the last spawn command, which allowed us to refresh 
   * the folder contents after the command had terminated with ThunarVFS (i.e. 
   * when the archive had been created). This no longer works with GIO but 
   * we still use the watch to close the PID properly.
   */
  gint            child_watch_id;
};


static GQuark lhh_action_files_quark;
#if THUNARX_CHECK_VERSION(0,4,1)
static GQuark lhh_action_folder_quark;
#endif
static GQuark lhh_action_provider_quark;



THUNARX_DEFINE_TYPE_WITH_CODE (LhhProvider,
                               lhh_provider,
                               G_TYPE_OBJECT,
                               THUNARX_IMPLEMENT_INTERFACE (THUNARX_TYPE_MENU_PROVIDER,
                                                            lhh_provider_menu_provider_init));


static void
lhh_provider_class_init (LhhProviderClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the "tap-action-files", "tap-action-folder" and "tap-action-provider" quarks */
  lhh_action_files_quark = g_quark_from_string ("lhh-action-files");
#if THUNARX_CHECK_VERSION(0,4,1)
  lhh_action_folder_quark = g_quark_from_string ("lhh-action-folder");
#endif
  lhh_action_provider_quark = g_quark_from_string ("lhh-action-provider");

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = lhh_provider_finalize;
}


static void
lhh_provider_menu_provider_init (ThunarxMenuProviderIface *iface)
{
  iface->get_file_actions = lhh_provider_get_file_actions;

}



static void
lhh_provider_init (LhhProvider *lhh_provider)
{
	//
}

static void
lhh_provider_finalize (GObject *object)
{
  LhhProvider *lhh_provider = LHH_PROVIDER (object);
  GSource     *source;

  /* give up maintaince of any pending child watch */
  if (G_UNLIKELY (lhh_provider->child_watch_id != 0))
    {
      /* reset the callback function to g_spawn_close_pid() so the plugin can be
       * safely unloaded and the child will still not become a zombie afterwards.
       */
      source = g_main_context_find_source_by_id (NULL, lhh_provider->child_watch_id);
      g_source_set_callback (source, (GSourceFunc) g_spawn_close_pid, NULL, NULL);
    }
  

  (*G_OBJECT_CLASS (lhh_provider_parent_class)->finalize) (object);
}


static gboolean
lhh_is_parent_writable (ThunarxFileInfo *file_info)
{
  gboolean result = FALSE;
  gchar   *filename;
  gchar   *uri;

  /* determine the parent URI for the file info */
  uri = thunarx_file_info_get_parent_uri (file_info);
  if (G_LIKELY (uri != NULL))
    {
      /* determine the local filename for the URI */
      filename = g_filename_from_uri (uri, NULL, NULL);
      if (G_LIKELY (filename != NULL))
        {
          /* check if we can write to that folder */
          result = (g_access (filename, W_OK) == 0);

          /* release the filename */
          g_free (filename);
        }

      /* release the URI */
      g_free (uri);
    }

  return result;
}


static void
lhh_send_file (GtkAction *action,
                    GtkWidget *window)
{
  LhhProvider *lhh_provider;
  GList       *files;
  gchar       *dirname;
  gchar       *uri;

  /* determine the files associated with the action */
  files = g_object_get_qdata (G_OBJECT (action), lhh_action_files_quark);
  if (G_UNLIKELY (files == NULL))
    return;

  /* determine the provider associated with the action */
  lhh_provider = g_object_get_qdata (G_OBJECT (action), lhh_action_provider_quark);
  if (G_UNLIKELY (lhh_provider == NULL))
    return;

  /* determine the parent URI of the first selected file */
  uri = thunarx_file_info_get_parent_uri (files->data);
  if (G_UNLIKELY (uri == NULL))
    return;

  /* determine the directory of the first selected file */
  dirname = g_filename_from_uri (uri, NULL, NULL);
  g_free (uri);

  /* verify that we were able to determine a local path */
  if (G_UNLIKELY (dirname == NULL))
    return;

  /* execute the action */
  lhh_provider_execute (lhh_provider, lhh_backend_send_file, window, dirname, files, _("Failed to create archive"));

  /* cleanup */
  g_free (dirname);
}

static void
lhh_receive_files (GtkAction *action,
                    GtkWidget *window)
{
  LhhProvider *lhh_provider;
  GList       *files;
  gchar       *dirname;
  gchar       *uri;

  /* determine the files associated with the action */
  files = g_object_get_qdata (G_OBJECT (action), lhh_action_files_quark);
  if (G_UNLIKELY (files == NULL))
    return;

  /* determine the provider associated with the action */
  lhh_provider = g_object_get_qdata (G_OBJECT (action), lhh_action_provider_quark);
  if (G_UNLIKELY (lhh_provider == NULL))
    return;

  /* determine the parent URI of the first selected file */
  uri = thunarx_file_info_get_parent_uri (files->data);
  if (G_UNLIKELY (uri == NULL))
    return;

  /* determine the directory of the first selected file */
  dirname = g_filename_from_uri (uri, NULL, NULL);
  g_free (uri);

  /* verify that we were able to determine a local path */
  if (G_UNLIKELY (dirname == NULL))
    return;

  /* execute the action */
  lhh_provider_execute (lhh_provider, lhh_backend_receive_files, window, dirname, files, _("Failed to create archive"));

  /* cleanup */
  g_free (dirname);
}



static GList*
lhh_provider_get_file_actions (ThunarxMenuProvider *menu_provider,
                               GtkWidget           *window,
                               GList               *files)
{
  gchar              *scheme;
  LhhProvider        *lhh_provider = LHH_PROVIDER (menu_provider);
  GtkAction          *action;
  GClosure           *closure;
  gboolean            all_archives = TRUE;
  gboolean            can_write = TRUE;
  GList              *actions = NULL;
  GList              *lp;
  gint                n_files = 0;
  uid_t uid;
  gboolean is_dir;
  gboolean is_writable;
  struct passwd *pw;

  /* check all supplied files */
  for (lp = files; lp != NULL; lp = lp->next, ++n_files)
    {
      /* check if the file is a local file */
      scheme = thunarx_file_info_get_uri_scheme (lp->data);
      is_dir=thunarx_file_info_is_directory(lp->data);
      
       
      /* unable to handle non-local files */
      if (G_UNLIKELY (strcmp (scheme, "file")))
        {
          g_free (scheme);
          return NULL;
        }
      g_free (scheme);

      
      /* check if we can write to the parent folder */
      if (can_write && !lhh_is_parent_writable (lp->data))
        can_write = FALSE;

      is_writable=lhh_is_parent_writable(lp->data);

    }

      
	uid = geteuid ();
        pw = getpwuid (uid);
	if (pw)
	{
	
		if(pw->pw_uid < 1003)
		{
			return NULL;
		}
		else
		{
			
	
				    if(!is_dir){
					
				     if (is_writable)
				     {
					    
				    action = g_object_new (GTK_TYPE_ACTION,
							     "label", _("Send file to teacher"),
							     "name", "Lhh::send-file",
				#if !GTK_CHECK_VERSION(2,9,0)
							     "stock-id", "go-jump",
				#else
							     "icon-name", "tap-extract-to",
				#endif
							     "tooltip", dngettext (GETTEXT_PACKAGE,
										   "Send file to teacher",
										   "Send file to teacher",
										   n_files),
							     NULL);
					    
					closure = g_cclosure_new_object (G_CALLBACK (lhh_send_file), G_OBJECT (window));
					 g_object_set_qdata_full (G_OBJECT (action), lhh_action_files_quark,
							       thunarx_file_info_list_copy (files),
							       (GDestroyNotify) thunarx_file_info_list_free);
				      g_object_set_qdata_full (G_OBJECT (action), lhh_action_provider_quark,
							       g_object_ref (G_OBJECT (lhh_provider)),
							       (GDestroyNotify) g_object_unref);
				       
				      
				      g_signal_connect_closure (G_OBJECT (action), "activate", closure, TRUE);
				      actions = g_list_append (actions, action);	
					}
				    }
				    else
				    {

					if (is_writable)
					{
					
					    action = g_object_new (GTK_TYPE_ACTION,
							     "label", _("Set this folder to receive homeworks"),
							     "name", "Lhh::receive-file",
				#if !GTK_CHECK_VERSION(2,9,0)
							     "stock-id", "go-jump",
				#else
							     "icon-name", "tap-extract-to",
				#endif
							     "tooltip", dngettext (GETTEXT_PACKAGE,
										   "Set this folder to receive homeworks",
										   "Set this folder to receive homeworks",
										   n_files),
							     NULL);
					    
					closure = g_cclosure_new_object (G_CALLBACK (lhh_receive_files), G_OBJECT (window));
						
					g_object_set_qdata_full (G_OBJECT (action), lhh_action_files_quark,
							       thunarx_file_info_list_copy (files),
							       (GDestroyNotify) thunarx_file_info_list_free);
				      g_object_set_qdata_full (G_OBJECT (action), lhh_action_provider_quark,
							       g_object_ref (G_OBJECT (lhh_provider)),
							       (GDestroyNotify) g_object_unref);
				       
				      
				      g_signal_connect_closure (G_OBJECT (action), "activate", closure, TRUE);
				      actions = g_list_append (actions, action);						

						
						
					}
				    }
				     		
			
		}
		
	}
	
  return actions;
}


static void
lhh_provider_execute (LhhProvider *lhh_provider,
                      GPid       (*action) (const gchar *folder,
                                            GList       *files,
                                            GtkWidget   *window,
                                            GError     **error),
                      GtkWidget   *window,
                      const gchar *folder,
                      GList       *files,
                      const gchar *error_message)
{
  GtkWidget *dialog;
  GSource   *source;
  GError    *error = NULL;
  GPid       pid;

  /* try to execute the action */
  pid = (*action) (folder, files, window, &error);
  if (G_LIKELY (pid >= 0))
    {
      /* check if we already have a child watch */
      if (G_UNLIKELY (lhh_provider->child_watch_id != 0))
        {
          /* reset the callback function to g_spawn_close_pid() so the plugin can be
           * safely unloaded and the child will still not become a zombie afterwards.
           */
          source = g_main_context_find_source_by_id (NULL, lhh_provider->child_watch_id);
          g_source_set_callback (source, (GSourceFunc) g_spawn_close_pid, NULL, NULL);
        }

      /* schedule the new child watch */
      lhh_provider->child_watch_id = g_child_watch_add_full (G_PRIORITY_LOW, pid, lhh_provider_child_watch,
                                                             lhh_provider, lhh_provider_child_watch_destroy);
    }
  else if (error != NULL)
    {
      /* display an error dialog */
      dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                       GTK_DIALOG_DESTROY_WITH_PARENT
                                       | GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_CLOSE,
                                       "%s.", error_message);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_error_free (error);
    }
}



static void
lhh_provider_child_watch (GPid     pid,
                          gint     status,
                          gpointer user_data)
{
  GDK_THREADS_ENTER ();

  /* need to cleanup */
  g_spawn_close_pid (pid);

  GDK_THREADS_LEAVE ();
}



static void
lhh_provider_child_watch_destroy (gpointer user_data)
{
  LhhProvider *lhh_provider = LHH_PROVIDER (user_data);

  /* reset child watch id */
  lhh_provider->child_watch_id = 0;
}



