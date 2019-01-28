import urllib
import gi
gi.require_version("Caja","2.0")
from gi.repository import Caja,GObject
import pwd
import getpass
import grp
import subprocess
import os
import os.path

import locale
import gettext

#locale.textdomain("lliurex-homework-harvester")
#gettext.textdomain("lliurex-homework-harvester")
#_=locale.gettext

def get_translation(line):
	
	try:
	
		trans={}
		trans["Set this folder to receive homeworks"]={}
		trans["Set this folder to receive homeworks"]["es"]="Marcar esta carpeta para recibir trabajos"
		trans["Set this folder to receive homeworks"]["qcv"]="Establix esta carpeta per a rebre els treballs"
		trans["Send file to teacher"]={}
		trans["Send file to teacher"]["es"]="Enviar fichero a profesor/a"
		trans["Send file to teacher"]["qcv"]="Envia el fitxer al professor o professor/a"
		
		lng=os.getenv("LANG")
		
		
		
		if lng.find("ca")==0 or lng.find("qcv")==0:
			try:
				ret_line=trans[line]["qcv"]
				return ret_line
			except:
				return line
				
		if lng.find("es")==0:
			try:
				ret_line=trans[line]["es"]
				return ret_line
			except:
				return line
		else:
			return line
		
	except:
		return line
	
	
#def get_translation


_=get_translation





class LliurexHomeworkExtension(GObject.GObject,Caja.MenuProvider):
	
	def __init__(self):
		
		self.file_list=[]
		
	#def init
		
	def get_groups(self,user):
		
		groups=[]
		for g in grp.getgrall():
			if user in g.gr_mem:
				groups.append(g.gr_name)
		return groups
		
		
	#def get_groups
	
	def check_ownership(self,user,path):
		
		info=os.stat(path)
		
		path_user_owner=info[4]
		path_group_owner=info[5]
		
		info=pwd.getpwnam(user)
		
		user_uid=info[2]
		group_uid=info[3]
		
		if path_user_owner==user_uid or path_group_owner==group_uid:
			return True
		else:
			return False
		
	#def check_ownership
	
	
	def teachers_cb(self,menu,path):
		
		command=[]
		command.append("/usr/bin/lliurex-homework-harvester")
		command.append(path)
		subprocess.Popen(command,stdout=subprocess.PIPE)		

	#def teachers_cb
		
	def students_cb(self,menu,files):
		
		command=[]
		command.append("/usr/bin/lliurex-homework-sender")
		for file in files:
			command.append(file)
		subprocess.Popen(command,stdout=subprocess.PIPE)
		
	#def students_cb

	def get_file_items(self, window, files):
		
		file_list=[]
		for file in files:
			filename = urllib.unquote(file.get_uri()[7:])
			file_list.append(str(filename))
	
		user=getpass.getuser()
		group_list=self.get_groups(user)
	
		if "teachers" in group_list:

			#one item only
			if len(file_list)>1 or len(file_list)==0:
				return
			#folders only
			if not os.path.isdir(file_list[0]):
				return
			
			if not self.check_ownership(user,file_list[0]):
				return

			
			item = Caja.MenuItem(name='Caja::set_harverst_folder',
						 label=_('Set this folder to receive homeworks')+"...",
						 tip=_('Set this folder to receive homeworks'),
						 icon='go-jump')
			item.connect('activate', self.teachers_cb, file_list[0])
			
			
			return item,				
			
		elif "students" in group_list:
			#STUDENT SIDE
			#files only
			
			for item in file_list:
				if not os.path.isfile(item):
					return
			
			
			item = Caja.MenuItem(name='Caja::send_homework',
						 label=_('Send file to teacher')+"...",
						 tip=_('Send file to teacher'),
						 icon='go-jump')
			item.connect('activate', self.students_cb, file_list)
			return item,
			
		
			
		else:
			return



