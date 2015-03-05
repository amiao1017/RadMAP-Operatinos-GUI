import zmq
import subprocess
import shlex
import time
import sys

context = zmq.Context()
port = "5556"
dbSocket = context.socket(zmq.PAIR)
dbSocket.bind("tcp://192.168.100.43:%s" % port)

processes = [] #list of processes
iterations = 0
HyperSpecAcqStarted = False
2iVerification = False
NIRVerification = False

#  Sockets to talk to servers
#dbSocket = context.socket(zmq.REP)
#dbSocket.connect("tcp://192.168.1.100:5108")

def runProcess(processes, numprocesses):    
    iterations = 0
    while true:
        if dbSocket.poll(1) != 0:
            dbCommand = dbSocket.recv()
            print dbCommand
            if dbCommand == 'stopHyperSpec':
                if HyperSpecAcqStarted == True: #stop HyperSpec if started
                    print "Stopping HyperSpec Acquisition"
                    processes.remove(iAcquisition)
                    StopiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\hsStopScript.exe")
                    print "2i Acquisition Stopped"
                    #dbSocket.send("2i Acquisition stopped")
                    processes.remove(NIRAcquisition)
                    print "NIR Acquisition Stopped"
                    dbSocket.send("HyperSpec Acquisition Stopped")
                    HyperSpecAcqStarted = False
                else:
                    print "HyperSpec Acquisition already stopped"
        retcode = processes[iterations % numprocesses].poll() #returns None while subprocess is running
        line = processes[iterations % numprocesses].stdout.readline()
        yield line
        if(retcode is not None):
            break
        iterations += 1


while True:

    if dbSocket.poll(1) != 0:
    	dbCommand = dbSocket.recv()
        print dbCommand

    	if dbCommand == 'startHyperSpec':
            print "dbCommand = startHyperSpec"
    	    if HyperSpecAcqStarted == False: #run HyperSpecAcq and verify
                #testAcquitision = subprocess.call(shlex.split("python /home/rossebv/Desktop/RadMAP-Operatinos-GUI/Operations\ GUI/interfaceCodeV0.2.py &"))
                print "Acquisition Script Called"
                iAcquisition = runProcess("E:\\ResononAPI_2.2_Beta\\bin\\2iHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NEW_CONSOLE)
                processes.append(iAcquisition)
                print "2i process started"
                #dbSocket.send("2i process started")
                NIRAcquisition = runProcess("E:\\ResononAPI_2.2_Beta\\bin\\NIRHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=subprocess.CREATE_NEW_CONSOLE)
                processes.append(NIRAcquisition)
                print "NIR process started"
                dbSocket.send("HyperSpec Acquisition started")    
                HyperSpecAcqStarted = True
            else:
                print "HyperSpec Acquisition already running"

    		
    	if dbCommand == 'stopHyperSpec':
    	    if HyperSpecAcqStarted == True: #stop HyperSpec if started
                print "Stopping HyperSpec Acquisition"
                processes.remove(iAcquisition)
                StopiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\hsStopScript.exe")
                print "2i Acquisition Stopped"
                #dbSocket.send("2i Acquisition stopped")
                processes.remove(NIRAcquisition)
                print "NIR Acquisition Stopped"
                dbSocket.send("HyperSpec Acquisition Stopped")
                HyperSpecAcqStarted = False
            else:
                print "HyperSpec Acquisition already stopped"
        

    #print "Verifying Processes"	
    if len(processes) != 0:
        for line in runProcess(processes, len(processes))
            if len(processes) != 0:
                if (line.find("line") != -1 and (iterations % 2 = 0) and (line.find("discarded") == -1: #2i verification
                    print "2i - " + line
                    2iVerification = True
                if (line.find("line") != -1 and (iterations % 2 = 1): #NIR verification
                    print "NIR - " + line
                    NIRVerification = True
                iterations += 1
            VerificationMessage = 2iVerification and NIRVerification
            print VerificationMessage
            #dbSocket.send(VerificationMessage)
        #line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
        #if (line.find("line") != -1) and (iterations % 2 = 0): #2i verification
            #print "2i - "+ line
            #dbSocket.send("2i -" + line[line.find("line"): ])
        #if (line.find("line") != -1) and (iterations % 2 = 1): #NIR Verification
            #print "NIR - " + line
            #dbSocket.send("NIR -" + line[line.find("line"): ])
        #iterations += 1

