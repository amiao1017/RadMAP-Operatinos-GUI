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
            #2iAcquisition = subprocess.Popen(shlex.split("2iHyperSpecAcquisitionV2.0.exe"), stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
            #check cwd
            #NIRAcquitision = subprocess.Popen(shlex.split("NIRHyperSpecAcquisitionV1.0.exe"), stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
            print "HyperSpec Acquisition starting"
            HyperSpecAcqStarted = True
        else:
            print "HyperSpec Acquisition already running"
		
	if dbCommand == 'STO':
		if HyperSpecAcqStarted: #stop HyperSpec if started
            #2iAcquisition.communicate(input = 'q')
            #NIRAcquisition.communicate(input = 'q')
            print "Stopping HyperSpec Acquisition"
            HyperSpecAcqStarted = False
        else:
            print "HyperSpec Acquisition already stopped"
	
    #if dbCommand == save:

		
	