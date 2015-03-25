import zmq
import subprocess
import time
import pyperclip

context = zmq.Context()


hvStarted = False
neutronStarted = False
hvVerification = False
neutronVerification = False
neutronStopped = False
hvStopped = False
acquisitionStopped = False

#  Sockets to talk to servers
interfacePort = "5557"
interfaceSocket = context.socket(zmq.REP)
interfaceSocket.bind("tcp://192.168.100.23:%s" % interfacePort)
liqVerificationPort = "5567"
liqVerificationSocket = context.socket(zmq.REP)
liqVerificationSocket.bind("tcp://192.168.100.23:%s" %liqVerificationPort)

while True:

    if interfaceSocket.poll(100) != 0:
    	dbCommand = interfaceSocket.recv()
        print dbCommand

    	if dbCommand == 'startNeutrons':
            print "dbCommand = %s" % dbCommand
            interfaceSocket.send("Start Neutron Signal Received - Wait 90 Seconds")
            if hvStarted == False:
            	rampOnVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Start_Script_V2.0.exe")
            	hvStarted = True
            	time.sleep(80)
            if neutronStarted == False:
               acquireNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Start_AcqV2.0.exe")
               neutronStarted = True
            acquisitionStopped = False

    		
    	if dbCommand == 'stopNeutrons':
            interfaceSocket.send("Stop Neutron Signal Received")
    	    if neutronStarted:
                stopNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Stop_AcqV2.0.exe")
                neutronStarted = False
                time.sleep(10)
            if hvStarted == True:
                rampOffVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Stop_Script_V2.0.exe")
                time.sleep(80)
                hvStarted = False
            neutronStopCheck = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Stop_Verification.exe")
            stopped = "%s" % pyperclip.paste()
            if (stopped.find('True') != -1):
                neutronStopped = True
            hvStopCheck = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Stop_Verification.exe")
            stopped = "%s" % pyperclip.paste()
            if (stopped.find('True') != -1):
                hvStopped = True
            acquisitionStopped = hvStopped and neutronStopped

        if dbCommand == "Started?":
            interfaceSocket.send("Neutron Acquisition Starting")
            

               
    if hvStarted == True or neutronStarted == True:
        verifyVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\HV_Verification_ScriptV1.0.exe")
        clipboard = pyperclip.paste()
        if clipboard.find('False') != -1:
            hvVerification = False
        if clipboard.find('True') != -1:
            hvVerification = True
        verifyNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\AutoHotKey Scripts\\Neutron_Verification_Script_V1.0.exe")
        clipboard = pyperclip.paste()
        if clipboard.find('False') != -1:
            neutronVerification = False
        if clipboard.find('True') != -1:
            neutronVerification = True
        verificationMessage = hvVerification and neutronVerification
    if liqVerificationSocket.poll(100) != 0:
        if not acquisitionStopped:
            liqVerificationMessage = "%s" % verificationMessage
            liqVerificationSocket.send(liqVerificationMessage)
        else:
            liqVerificationSocket.send("Closed")
        
