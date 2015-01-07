import zmq

context = zmq.Context()

#  Sockets to talk to servers

hsSocket = context.socket(zmq.REQ)
hsSocket.connect("tcp://192.168.100.43:5110")

while True:
	hsSocket.send("Did it work?")