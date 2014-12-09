import zmq
import subprocess

context = zmq.Context()

processes = [] #list of processes
iterations = 0
HyperSpecAcqStarted = False

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://192.168.1.100:5108")

while True:

    if dbSocket.poll(1000) != 0:
    	dbCommand = dbSocket.recv()

    	if dbCommand == 'STA':
    		if HyperSpecAcqStarted == False: #run HyperSpecAcq and verify
                #NEED PATH TO .exe FILES ON HS SERVER
                2iAcquisition = subprocess.Popen(shlex.split("2iHyperSpecAcquisitionV2.0.exe"), stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                processes.append(2iAcquisition)
                NIRAcquisition = subprocess.Popen(shlex.split("NIRHyperSpecAcquisitionV1.0.exe"), stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                processes.append(NIRAcquisition)
                print "HyperSpec Acquisition starting"
                HyperSpecAcqStarted = True
            else:
                print "HyperSpec Acquisition already running"
    		
    	if dbCommand == 'STO':
    		if HyperSpecAcqStarted: #stop HyperSpec if started
                2iAcquisition.communicate(input = 'q')
                processes.remove(2iAcquisition)
                NIRAcquisition.communicate(input = 'q')
                processes.remove(NIRAcquisition)
                print "Stopping HyperSpec Acquisition"
                HyperSpecAcqStarted = False
            else:
                print "HyperSpec Acquisition already stopped"
    	
    if len(processes) != 0:
        line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
        if (line.find("line") != -1) and (iterations % 2 = 0): #2i verification
             dbSocket.send("2i -" + line[line.find("line"): ])
        if (line.find("line") != -1) and (iterations % 2 = 1): #NIR Verification
            dbSocket.send("NIR -" + line[line.find("line"): ])
		iterations += 1
	