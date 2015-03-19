import zmq
import subprocess
import time

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
            dbSocket.send("Start Neutron Signal Received")
            if VoltageOn == False:
            	rampOnVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Start_Script_V1.0.exe")
            	VoltageOn = True
            	time.sleep(80)
            if VoltageOn:
                if NeutronAcquisition == False:
            	   AcquireNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Start_Acq.exe")
            	   NeutronAcquisition = True

    		
    	if dbCommand == 'stopNeutrons':
            dbSocket.send("Stop Neutron Signal Received")
    	    if NeutronAcquisition:
                StopNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Stop_Acq.exe")
                NeutronAcquisition = False
                time.sleep(10)
            if NeutronAcquisition == False:
                if VoltageOn == True:
            	   rampOffVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Stop_Script_V1.0.exe")
            	   time.sleep(80)
                   VoltageOn = False
