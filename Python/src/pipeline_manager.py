import yaml
import os
from .read_yaml import read_yaml
from.logger import Logger
from .client import client
from .server import server

log = Logger()

class PipelineManager():

	def __init__(self):
		self.config_file = read_yaml()

		for i in self.config_file['dir']:
			if not os.path.exists(self.config_file['dir'][i]):
				os.mkdir(self.config_file['dir'][i])		

	def server(self):

		server_obj = server()
		server_obj.run()
		# pass

	def client(self):

		client_obj = client()
		client_obj.run()

		# ToDO
		pass