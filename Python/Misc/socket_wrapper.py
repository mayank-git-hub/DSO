import socket
import pickle
import sys
ports = [10000,10001]

class socket_p():

	def __init__(self, port,host=''):


		self.s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		self.host=host
		self.port=port
		

	def _connect(self):
		try:

			self.s.connect((self.host,self.port))
		except:
			print()

	def _bind(self):

		self.s.bind((self.host,self.port))

	def _listen(self):

		self.s.listen(10)

	def _send(self, frame):

		self.s.send(frame)

	def _accept(self):

		self.conn, self.address =  self.s.accept()

	def get_data(self):

		#Expecting not greater than 16 MP Image

		x_as_bytes = b''

		while True:
			x_as_bytes += self.conn.recv(1024*1024*3*16)

			if len(x_as_bytes) == 921764:
				break

		print(len(x_as_bytes))

		return pickle.loads(x_as_bytes)

	def get_string(self):

		return self.conn.recv(1000)