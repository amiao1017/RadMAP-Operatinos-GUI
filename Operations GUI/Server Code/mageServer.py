import zmq
import subprocess

context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://192.168.1.100:5554")


while True:

	dbCommand = dbSocket.recv()

	if dbCommand == str:

	#based on what the start script returns may want to use subprocess.callback() and use resulting info for data verification
		subprocess.call(#insert mage start script name here#)
		
	if dbCommand == stp:
		subprocess.call(#insert mage stop script name here#)
	
	if dbCommand == save:
		subprocess.call(#insert mage save script name here#)
		
	#function to display live data
	