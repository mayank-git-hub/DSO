import numpy as np
import cv2
import pickle
import socket
import time

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
host='10.8.100.146'
port=9991

s.connect((host, port))

cap = cv2.VideoCapture(0)

while(True):


	# start = time.time()
	ret, frame = cap.read()
	# print('Time taken to get image: ', time.time() - start)

	frame = cv2.imencode('.png', frame)[1]

	x_as_bytes = pickle.dumps(frame)

	while True:

		if s.recv(1000) == b'Start_Sending':
			# print('Recieved: Start_sending')
			s.send(b'Sending'+bytes(str(len(x_as_bytes)), 'utf-8'))
			# print('Sent: Sending')
			break

	while True:

		if s.recv(len(b'Start_Sending_again')) == b'Start_Sending_again':
			# print('Recieved: Start_sending_again')
			s.send(b'Sending_again')
			# print('Sent: Sending_again')
			break

	s.send(x_as_bytes)

	# print(len(x_as_bytes))



	# frame = pickle.loads(x_as_bytes)

	# cv2.imshow('frame',frame)
	# if cv2.waitKey(1) & 0xFF == ord('q'):
	#	 break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()