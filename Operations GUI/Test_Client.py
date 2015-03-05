import zmq
import time
import sys

port = "5556"
context = zmq.Context()
hsSocket = context.socket(zmq.PAIR)
hsSocket.connect("tcp://127.0.0.1:%s" % port)

while True:
    #msg = hsSocket.recv()
    #print msg
    #hsSocket.send("client message to server1")
    hsSocket.send("Hello")
    if hsSocket.poll(1) != 0:
    	msg = hsSocket.recv()
    	print msg
    time.sleep(1)