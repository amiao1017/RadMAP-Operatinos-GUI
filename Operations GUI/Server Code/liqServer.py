import zmq
import subprocess

context = zmq.Context()

VoltageOn = False
NeutronAcquisition = False

#  Sockets to talk to servers
portLiq = "5557"
dbSocket = context.socket(zmq.PAIR)
dbSocket.bind("tcp://192.168.100.23:%s" % portLiq)

while True:

    if dbSocket.poll(1) != 0:
    	dbCommand = dbSocket.recv()
        print dbCommand

    	if dbCommand == 'startNeutrons':
            print "dbCommand = %s" % dbCommand
            if VoltageOn == False:
            	rampOnVoltage = subprocess.Popen("PATH\\TO\\VoltageAHKScript")
            	VoltageOn = True
            if VoltageOn:
            	AcquireNeutrons = subprocess.Popen("PATH\\TO\\NEUTRONAHKSCRIPT")
            	NeutronAcquisition = True

    		
    	if dbCommand == 'stopNeutrons':
    	    if NeutronAcquisition: #stop HyperSpec if started
                StopNeutrons = subprocess.Popen("PATH\\TO\\NEUTRONSTOPAHKSCRIPT")
                NeutronAcquisition = False
            if NeutronAcquisition == False:
            	rampOffVoltage = subprocess.Popen("PATH\\TO\\VOLTAGEOFFAHKSCRIPT")

    #print "Verifying Processes"	
    #if len(processes) != 0:
        #line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
        #if (line.find("line") != -1) and (iterations % 2 = 0): #2i verification
             #dbSocket.send("2i -" + line[line.find("line"): ])
        #if (line.find("line") != -1) and (iterations % 2 = 1): #NIR Verification
            #dbSocket.send("NIR -" + line[line.find("line"): ])
    iterations += 1