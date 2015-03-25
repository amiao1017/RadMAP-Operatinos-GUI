import zmq
import subprocess
from datetime import datetime, date, time
import shlex
import time
import sys, os
import string
import pyperclip

context = zmq.Context()
processes = [] #list of processes
HyperSpecAcqStarted = False
acquisitionStopped = False
iiVerification = False
nirVerification = False
hyperSpecVerification = False
stopped = False

iiLinePrev = None
nirLinePrev = None

interfacePort = "5556"
interfaceSocket = context.socket(zmq.REP)
interfaceSocket.bind("tcp://192.168.100.43:%s" % interfacePort)
verificationPort = "5555"
hsVerificationSocket = context.socket(zmq.REP)
hsVerificationSocket.bind("tcp://192.168.100.43:%s" % verificationPort)

while True:
    if interfaceSocket.poll(100) != 0:
    	dbCommand = interfaceSocket.recv()
        print dbCommand

    	if dbCommand == 'startHyperSpec':
            acquisitionStopped = False
            print "dbCommand = startHyperSpec"
            interfaceSocket.send("startHyperSpec Received")
    	    if HyperSpecAcqStarted == False: #run HyperSpecAcq and verify
                print "Acquisition Script Called"
                date = datetime.now()
                currTime = date.strftime("%Y%m%d%H%M%S")
                home = "E:\\HS_stdout\\"
                iiFolderName = "stdOut_2i_%s" %currTime
                iiFileName = "%s.txt" %iiFolderName
                iiPath = home + iiFolderName
                nirFolderName = "stdOut_NIR_%s" %currTime
                nirFileName = "%s.txt" %nirFolderName
                nirPath = home +nirFolderName
                if not os.path.exists(iiPath):
                    os.makedirs(iiPath)
                if not os.path.exists(nirPath):
                    os.makedirs(nirPath)    
                iiFile = open("%s\\%s" %(iiPath, iiFileName), 'a+')
                nirFile = open("%s\\%s" %(nirPath, nirFileName), 'a+')
                iiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\2iHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdout = iiFile)
                processes.append(iiAcquisition)
                print "2i process started"
                NIRAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\NIRHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdout = nirFile)
                processes.append(NIRAcquisition)
                print "NIR process started"
            else:
                print "HyperSpec Acquisition already running"
    		
    	if dbCommand == 'stopHyperSpec':
    	    if HyperSpecAcqStarted == True: #stop HyperSpec if started
                print "Stopping HyperSpec Acquisition"
                interfaceSocket.send("HyperSpec Acquisition Stopping")
                processes.remove(iiAcquisition)
                StopiiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\hsStopScript.exe")
                iiFile.close()
                print "2i Acquisition Stopped"
                processes.remove(NIRAcquisition)
                nirFile.close()
                print "NIR Acquisition Stopped"
                HyperSpecAcqStarted = False
                time.sleep(3)
                stopVerification = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\hsStopVerificationScript.exe")
                stopped = "%s" % pyperclip.paste()
                print stopped
                if (stopped.find('True') != -1):
                    acquisitionStopped = True
                    print "acquisitionStopped % s" % acquisitionStopped
            else:
                print "HyperSpec Acquisition already stopped"

        if dbCommand == 'Started?':
            time.sleep(9)
            interfaceSocket.send("HyperSpec Acquisition Starting")
            HyperSpecAcqStarted = True


    #print "Verifying Processes"	
    if len(processes) != 0:
        iiLocation = iiFile.tell()
        iiLine = iiFile.readline()
        if not iiLine:
            time.sleep(1)
            iiFile.seek(iiLocation)
        else:
            #if ((iiLine.find("Line ") != -1) or (iiLine.find("Complete") != -1) or (iiLine.find("Data") != -1) or (iiLine.find("Done") != -1)) and (iiLine != iiLinePrev): #2i verification
            if (iiLine != iiLinePrev):              
                iiVerification = True
                if (iiLine.find("Garbage") != -1):
                    iiVerification = False
            else:
                iiVerification = False
        print "iiLine - %s" % iiLine  
        #print "iiLinePrev - %s" % iiLinePrev
        if not iiLine.strip():
            iiLinePrev = iiLine
        nirLocation = nirFile.tell()
        nirLine = nirFile.readline()
        if not nirLine:
            time.sleep(1)
            nirFile.seek(iiLocation)
        else:
            #if ((nirLine.find("Line ") != -1) or (nirLine.find("Complete") != -1) or (nirLine.find("Datacube") != -1) or (nirLine.find("Done") != -1)) and (nirLine != nirLinePrev): #NIR Verification
            if (nirLine != nirLinePrev):
                nirVerification = True
                if (nirLine.find("Garbage") != -1):
                    nirVerification = False
            else:
                nirVerification = False
        print "nirLine - %s" % nirLine  
        #print "nirLinePrev - %s" % nirLinePrev
        if not nirLine.strip():
            nirLinePrev = nirLine
    if len(processes) == 0:
        iiVerification = False
        nirVerification = False
    HyperSpecVerification = iiVerification and nirVerification
    #print "iiVerification - %s" % iiVerification 
    #print "nirVerification - %s" % nirVerification
    time1 = time.time()
    #print "HyperSpecVerification - %s @ %s" % (HyperSpecVerification, time1)
    if hsVerificationSocket.poll(100) != 0:
        msg = hsVerificationSocket.recv()
        print "message - %s" % msg
        if not acquisitionStopped:
            hsVerificationMessage = "%s" % HyperSpecVerification
            hsVerificationSocket.send(hsVerificationMessage)
        else:
            hsVerificationSocket.send("Closed")
