import zmq
import subprocess

context = zmq.Context()

#  Sockets to talk to servers
portDaq = "5553"
#dbSocket = context.socket(zmq.PAIR)
#dbSocket.bind("tcp://192.168.100.1:%s" % portDaq) FIND DAQ IP


while True:

	dbCommand = dbSocket.recv()

	if dbCommand == str:

	#based on what the start script returns may want to use subprocess.callback() and use resulting info for data verification
		subprocess.call(#insert daq start script name here#)
		
	if dbCommand == stp:
		subprocess.call(#insert daq stop script name here#)
	
	if dbCommand == save:
		subprocess.call(#insert daq save script name here#)
		
	#function to display live data