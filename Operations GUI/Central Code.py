import zmq

context = zmq.Context()

#  Sockets to talk to servers
interfaceSocket = context.socket(zmq.REP)
interfaceSocket.connect("tcp://localhost:5104")
daqSocket = context.socket(zmq.REQ)
daqSocket.connect("tcp://localhost:5106")
mageSocket = context.socket(zmq.REQ)
mageSocket.connect("tcp://localhost:5107")
bugSocket = context.socket(zmq.REQ)
bugSocket.connect("tcp://localhost:5108")
liqSocket = context.socket(zmq.REQ)
liqSocket.connect("tcp://localhost:5109")
hsSocket = context.socket(zmq.REQ)
hsSocket.connect("tcp://localhost:5110")
	
while True:
	interfaceCommand = interfaceSocket.recv()
	daqSocket.send(interfaceCommand)
	mageSocket.send(interfaceCommand)
	bugSocket.send(interfaceCommand)
	liqSocket.send(interfaceCommand)
	hsSocket.send(interfaceCommand)
