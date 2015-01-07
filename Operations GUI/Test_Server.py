import zmq

context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://192.168.1.100:5108")

while True:

	if dbSocket.poll(1) != 0:
    	dbCommand = dbSocket.recv()