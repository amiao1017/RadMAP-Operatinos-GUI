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

#if interfaceCommand == str:
#	daqSocket.send("str")
#	mageSocket.send("str")
#	bugSocket.send("str")
#	liqSocket.send("str")
#	hsSocket.send("str")

#if interfaceCommand == stp:
#	daqSocket.send("stp")
#	mageSocket.send("stp")
#	bugSocket.send("stp")
#	liqSocket.send("stp")
#	hsSocket.send("stp")
	
#if interfaceCommand == save:
#	daqSocket.send("save")
#	mageSocket.send("save")
#	bugSocket.send("save")
#	liqSocket.send("save")
#	hsSocket.send("save")
	
while True:
	daqSocket.send(interfaceCommand)
	mageSocket.send(interfaceCommand)
	bugSocket.send(interfaceCommand)
	liqSocket.send(interfaceCommand)
	hsSocket.send(interfaceCommand)
	