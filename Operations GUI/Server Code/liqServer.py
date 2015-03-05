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
            	rampOnVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Start_Script_V1.0.exe")
            	VoltageOn = True
            if VoltageOn:
                if NeutronAcquisition == False:
            	   AcquireNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Start_Acq.exe")
            	   NeutronAcquisition = True

    		
    	if dbCommand == 'stopNeutrons':
    	    if NeutronAcquisition: #stop HyperSpec if started
                StopNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Stop_Acq.exe")
                NeutronAcquisition = False
            if NeutronAcquisition == False:
                if VoltageOn == True:
            	   rampOffVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Stop_Script_V1.0.exe")
                   VoltageOn = False

    iterations += 1