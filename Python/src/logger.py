from datetime import datetime
from .read_yaml import read_yaml
import sys

# ToDo update the logger code

class Logger():

	def __init__(self):
		self.config = read_yaml()
		self.write_path = self.config['dir']['experiment_dir']+'/log.txt'
		del self.config
		self.f = open(self.write_path, 'a')

	def first(self):
		self.f.write('\n--------- Starting new session: '+ str(datetime.now().time()) +' ---------\n\n')
	
	def info(self, *args):

		string = str(datetime.now().time())+': '+' '.join([str(i) for i in args])
		print(string)
		self.f.write(string+'\n')