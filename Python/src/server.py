import numpy as np
from .read_yaml import read_yaml 
from .socket_wrapper import socket_p
from .logger import Logger
import pickle
import time
# from time import time

log = Logger()

class server():

	def __init__(self):

		self.k=0.
		self.config = read_yaml()
		
		self.buffer = np.zeros([self.config['buffer_size']])
		self.time_stamp = np.zeros([self.config['buffer_size']])
		self.x = np.linspace(0,50.*self.config['buffer_size']/self.config['show_points'], num=self.config['buffer_size'])

		# if self.config['trigger']['edge'] == 'rising':
		# 	if self.config['trigger']['mode'] == 'normal':
		# 		self.trigger = self.normal_rising() 
		# 		# ToDO make function normal rising
		# 	elif self.config['trigger']['mode'] == 'auto':
		# 		self.trigger = self.auto_rising() 
		# 		# ToDO make function auto rising
		# elif self.config['trigger']['edge'] == 'falling':
		# 	if self.config['trigger']['mode'] == 'normal':
		# 		self.trigger = self.normal_falling() 
		# 		# ToDO make function normal falling
		# 	elif self.config['trigger']['mode'] == 'auto':
		# 		self.trigger = self.auto_falling()
		# 		# ToDO make function auto falling

		self.show = np.zeros([self.config['show_points']])
		self.prev_ns = time.time()#clock_gettime_ns()

		port = self.config['port']
		host = '127.0.0.1'
		
		self.socket = socket_p(port=port,host=host)
		self.socket._bind()
		log.info('Binded')
		self.socket._listen()
		log.info('Listen')
		self.socket._accept()
		log.info('Accept')

	# ToDo Change buffer_size and show_points

	def process(self):

		# Given the buffer process and return show_points

		return self.buffer[0:self.config['show_points']]

	# def trigger(self):



	def get(self):

		# Get the data from the RPI board here
		# curr_ns = time.time() #clock_gettime_ns()
		# self.time_stamp[self.current_index] = curr_ns - self.prev_ns
		# self.prev_ns = curr_ns
		self.buffer[self.current_index] = np.sin(self.x[self.current_index]/3.+ self.k)#
		
		#return 

	def run(self):

		send = True
		self.current_index = 0
		while True:
			if send == False and self.socket.conn.recv(10) == b'1':
				send = True
			elif send:
				self.get()
				self.current_index += 1
				if self.current_index%100000 == 0:
					print(self.current_index)
				if self.current_index%self.config['buffer_size'] == 0:

					self.k+=0.11
					self.socket.conn.send(pickle.dumps(self.process()))
					self.current_index = 0
					print(np.mean(self.time_stamp))
					send = False
			else:
				continue