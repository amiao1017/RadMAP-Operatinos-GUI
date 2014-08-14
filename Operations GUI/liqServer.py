import zmq
import subprocess

context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://localhost:5109")


while True:

	dbCommand = dbSocket.recv()

	if dbCommand == str:

	#based on what the start script returns may want to use subprocess.callback() and use resulting info for data verification
		subprocess.call(#insert liq start script name here#)
		
	if dbCommand == stp:
		subprocess.call(#insert liq stop script name here#)
	
	if dbCommand == save:
		subprocess.call(#insert liq save script name here#)
		
	#function to display live data