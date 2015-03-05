import zmq
import subprocess
import time
import pyperclip

context = zmq.Context()


HVStarted = False
NeutronStarted = False
HVVerification = False
NeutronVerification = False

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
            if HVStarted == False:
            	rampOnVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\HV_Start_Script_V2.0.exe")
            	HVStarted = True
            	time.sleep(80)
            if HVStarted:
                if NeutronStarted == False:
            	   AcquireNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\Neutron_Start_AcqV2.0.exe")
            	   NeutronStarted = True

    		
    	if dbCommand == 'stopNeutrons':
            dbSocket.send("Stop Neutron Signal Received")
    	    if NeutronStarted:
                StopNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\Neutron_Stop_AcqV2.0.exe")
                NeutronStarted = False
                time.sleep(10)
            if NeutronStarted == False:
                if HVStarted == True:
            	   rampOffVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\HV_Stop_Script_V2.0.exe")
            	   time.sleep(80)
                   HVStarted = False
                   
    if HVStarted == True or NeutronStarted == True:
        VerifyVoltage = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\HV_Verification_ScriptV1.0.exe")
        clipboard = pyperclip.paste()
        if clipboard.find('False') != -1:
            HVVerification = False
        if clipboard.find('True') != -1:
            HVVerification = True
        VerifyNeutrons = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\Neutron_Verification_Script_V1.0.exe")
        clipboard = pyperclip.paste()
        if clipboard.find('False') != -1:
            NeutronVerification = False
        if clipboard.find('True') != -1:
            NeutronVerification = True
        verificationMessage = HVVerification and NeutronVerification
        dbSocket.send(verificationMessage)
        
