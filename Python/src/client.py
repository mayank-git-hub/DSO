import numpy as np
import pickle
import socket
import time
import matplotlib.pyplot as plt
from .socket_wrapper import socket_p
from .read_yaml import read_yaml
import pickle
import struct

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
		self.buffer_size = 10000
		# port = 5000
		self.socket = socket_p(port=port,host=host)
		self.live_update_initial()
		self.socket._connect()

	def live_update_initial(self, blit = False):

		self.fig = plt.figure()
		self.ax1 = self.fig.add_subplot(1, 1, 1)

		self.fig.canvas.draw()   # note that the first draw comes before setting data 
		x = np.linspace(0,50., num=self.buffer_size)
		self.h1, = self.ax1.plot(x, lw=3)
		self.ax1.set_ylim([-20,20])

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

	def flo(self, val_4):
		pass


	def convert(self, values):
		double_values = []
		for i in range(self.buffer_size):
			double_values.append(struct.unpack('>f', values[4*i:4*(i+1)])[0])

		return double_values

	def run(self):		

		import time

		self.socket.s.send(b'starting')
		while True:
			values = b''
			self.socket.s.send(b'1')
			while True:
				values += self.socket.s.recv(400000000)
				if len(values)==4*self.buffer_size:
					break
			values = self.convert(values)
			self.update_now(values)