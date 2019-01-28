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


#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo.h>
#include <src/lhh-backend.h>




static GPid lhh_backend_run (const gchar *action,
						const gchar *folder,
						GList *files,
						GtkWidget *window,
						GError     **error);
						



static GPid
lhh_backend_run (const gchar *action,
			const gchar *folder,
			GList *files,
			GtkWidget *window,
			GError     **error)
{
	GdkScreen 	*screen;
	gchar 		**argv;
	GList		*lp;
	GPid			pid=-1;
	gint			n;
	gchar		*uri;
	
	argv = g_new0 (gchar *, 4 + g_list_length (files));
	if (strcmp (action, "send")==0)
	{
		//printf("send\n");
		argv = g_new0 (gchar *, 4 + g_list_length (files));
		argv[0] = g_strdup("/usr/bin/lliurex-homework-sender");
		 for (lp = files, n = 1; lp != NULL; lp = lp->next, ++n)
		{
			uri = thunarx_file_info_get_uri (THUNARX_FILE_INFO (lp->data));
			argv[n] = g_filename_from_uri (uri, NULL, NULL);
			g_free (uri);
		}
	}
	else
	{
		//printf("receive\n");
		argv = g_new0 (gchar *, 4 + g_list_length (files));
		argv[0] = g_strdup("/usr/bin/lliurex-homework-harvester");
		 for (lp = files, n = 1; lp != NULL; lp = lp->next, ++n)
		{
			uri = thunarx_file_info_get_uri (THUNARX_FILE_INFO (lp->data));
			argv[n] = g_filename_from_uri (uri, NULL, NULL);
			g_free (uri);
			break;
		}
	}
	
	
	
	screen = gtk_widget_get_screen (window);

         if (!gdk_spawn_on_screen (screen, folder, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, error))
            pid = -1;

    
         g_strfreev (argv);
        


  return pid;
	
	
}

GPid
lhh_backend_send_file (const gchar *folder,
                            GList       *files,
                            GtkWidget   *window,
                            GError     **error)
{
  /* run the action, the mime infos will be freed by the _run() method */
  return lhh_backend_run ("send", folder, files, window, error);
}


GPid
lhh_backend_receive_files(const gchar *folder,
					GList 		*files,
					GtkWidget  	*window,
					GError		**error)
{

  return lhh_backend_run ("receive", folder, files, window, error);
}
