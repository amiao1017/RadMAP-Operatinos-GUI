import zmq
import time
import sys

port = "5556"
context = zmq.Context()
dbSocket = context.socket(zmq.PAIR)
dbSocket.bind("tcp://127.0.0.1:%s" % port)

while True:
    #dbSocket.send("Server message to client3")
    msg = dbSocket.recv()
    print msg
    if msg == "Hello":
    	dbSocket.send("Hey there!")
    time.sleep(1)