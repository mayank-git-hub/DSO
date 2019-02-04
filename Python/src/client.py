import numpy as np
import pickle
import socket
import time
import matplotlib.pyplot as plt
from .socket_wrapper import socket_p
from .read_yaml import read_yaml
import pickle

class client():

	def __init__(self):

		self.config = read_yaml()
		self.current_data = {}
		for i in range(self.config['show_points']):
			self.current_data[i] = 0
		self.blit = self.config['blit']
		# host = self.config['ip']
		port = self.config['port']
		host = '127.0.0.1'
		# port = 5000
		self.socket = socket_p(port=port,host=host)
		self.live_update_initial()
		self.socket._connect()

	def live_update_initial(self, blit = False):

		self.fig = plt.figure()
		self.ax1 = self.fig.add_subplot(1, 1, 1)

		self.fig.canvas.draw()   # note that the first draw comes before setting data 
		x = np.linspace(0,50., num=1000)
		self.h1, = self.ax1.plot(x, lw=3)
		self.ax1.set_ylim([-1,1])

		if self.blit:
			self.ax1background = self.fig.canvas.copy_from_bbox(self.ax1.bbox)

	def update_now(self, values=None):

		if values is None:
			print('--------------- No Values Found -----------------')
			return
		self.h1.set_ydata(values)

		if self.blit:
			self.fig.canvas.restore_region(self.ax1background)
			self.ax1.draw_artist(self.h1)
			self.fig.canvas.blit(self.ax1.bbox)

		else:
			self.fig.canvas.draw()
			self.fig.canvas.flush_events()

		plt.pause(0.000000000001)

	def run(self):		

		self.socket.s.send(b'starting')
		print('Sent : asdf asdf asdf')
		values = self.socket.s.recv(4000)
		print('Got : ', str(values))
		values = self.socket.s.recv(4000)
		print('Got : ', str(values))
		exit(0)

		while True:
			values = b''
			while values == b'':
				values = self.socket.s.recv(4000000)
			values = pickle.loads(values)
			self.socket.s.send(b'1')
			self.update_now(values)