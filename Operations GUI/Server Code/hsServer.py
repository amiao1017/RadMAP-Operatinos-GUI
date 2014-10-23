import zmq
import subprocess

context = zmq.Context()
HyperSpecAcqStarted = False

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://192.168.1.100:5108")

while True:

	dbCommand = dbSocket.recv()

	if dbCommand == 'STA':
		if HyperSpecAcqStarted == False: #run HyperSpecAcq and verify
            hyperSpecAcquisition = subprocess.Popen(shlex.split("2iHyperSpecAcquisitionV1.2.exe"), stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
            #check cwd
            HyperSpecAcqStarted = True
        else:
            print "HyperSpec Acquisition already running"
		
	if dbCommand == 'STO':
		if HyperSpecAcqStarted: #stop lidars if started
            startVelodynePort.communicate(input = 'q')
            print "Stopping HyperSpec Acquisition"
            HyperSpecAcqStarted = False
        else:
            print "HyperSpec Acquisition already stopped"
	
	if dbCommand == save:
		subprocess.call(#insert hs save script name here#)
		
	