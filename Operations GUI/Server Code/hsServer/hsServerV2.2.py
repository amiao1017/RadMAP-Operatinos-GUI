import zmq
import subprocess
from datetime import datetime, date, time
import shlex
import time
import sys, os
import string

context = zmq.Context()
port = "5556"
dbSocket = context.socket(zmq.PAIR)
dbSocket.bind("tcp://192.168.100.43:%s" % port)

processes = [] #list of processes
iterations = 0
HyperSpecAcqStarted = False

iiVerification = False
nirVerification = False
HyperSpecVerification = False

iiLinePrev = None
nirLinePrev = None

#  Sockets to talk to servers
#dbSocket = context.socket(zmq.REP)
#dbSocket.connect("tcp://192.168.1.100:5108")

while True:

    if dbSocket.poll(1) != 0:
    	dbCommand = dbSocket.recv()
        print dbCommand

    	if dbCommand == 'startHyperSpec':
            print "dbCommand = startHyperSpec"
    	    if HyperSpecAcqStarted == False: #run HyperSpecAcq and verify
                #testAcquitision = subprocess.call(shlex.split("python /home/rossebv/Desktop/RadMAP-Operatinos-GUI/Operations\ GUI/interfaceCodeV0.2.py &"))
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
                #iiFile.close()
                print "2i process started"
                #dbSocket.send("2i process started")
                NIRAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\NIRHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdout = nirFile)
                processes.append(NIRAcquisition)
                #nirFile.close()
                print "NIR process started"
                dbSocket.send("HyperSpec Acquisition started")    
                HyperSpecAcqStarted = True
            else:
                print "HyperSpec Acquisition already running"
    		
    	if dbCommand == 'stopHyperSpec':
    	    if HyperSpecAcqStarted == True: #stop HyperSpec if started
                print "Stopping HyperSpec Acquisition"
                processes.remove(iiAcquisition)
                StopiiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\hsStopScript.exe")
                iiFile.close()
                print "2i Acquisition Stopped"
                #dbSocket.send("2i Acquisition stopped")
                processes.remove(NIRAcquisition)
                nirFile.close()
                print "NIR Acquisition Stopped"
                dbSocket.send("HyperSpec Acquisition Stopped")
                HyperSpecAcqStarted = False
            else:
                print "HyperSpec Acquisition already stopped"
        

    #print "Verifying Processes"	
    if len(processes) != 0:
        iiLocation = iiFile.tell()
        iiLine = iiFile.readline()
        if not iiLine:
            time.sleep(1)
            iiFile.seek(iiLocation)
        else:
            #print iiLine
            #if ((iiLine.find("Line ") != -1) or (iiLine.find("Complete") != -1) or (iiLine.find("Datacube") != -1) or (iiLine.find("Done") != -1)) and (iiLine != iiLinePrev): #2i verification
            if (iiLine != iiLinePrev):              
                iiVerification = True
                #iiNumber = iiLine[5:]
                #if iiNumber == '1000':
                    #iiLine = iiFile.readline()
                    #if (iiLine.find("Recording") != -1):
                        #iiVerification = True
                    #elif (iiLine == ""):
                        #iiVerification = False
                if (iiLine.find("Garbage") != -1):
                    iiVerification = False
            else:
                iiVerification = False
        print "iiLine - %s" % iiLine  
        print "iiLinePrev - %s" % iiLinePrev
        if not iiLine.strip():
            iiLinePrev = iiLine
        nirLocation = nirFile.tell()
        nirLine = nirFile.readline()
        if not nirLine:
            time.sleep(1)
            nirFile.seek(iiLocation)
        else:
            #print iiLine
            #if ((nirLine.find("Line ") != -1) or (nirLine.find("Complete") != -1) or (nirLine.find("Datacube") != -1) or (nirLine.find("Done") != -1)) and (nirLine != nirLinePrev): #NIR Verification
            if (nirLine != nirLinePrev):
                nirVerification = True
                if (nirLine.find("Garbage") != -1):
                    nirVerification = False
            else:
                nirVerification = False
        print "nirLine - %s" % nirLine  
        print "nirLinePrev - %s" % nirLinePrev
        if not nirLine.strip():
            nirLinePrev = nirLine
    if len(processes) == 0:
        iiVerification = False
        nirVerification = False
    HyperSpecVerification = iiVerification and nirVerification
    print "iiVerification - %s" % iiVerification 
    print "nirVerification - %s" % nirVerification
    print "HyperSpecVerification - %s" % HyperSpecVerification
    #dbSocket.send(HyperSpecVerification)
    iterations += 1
	
