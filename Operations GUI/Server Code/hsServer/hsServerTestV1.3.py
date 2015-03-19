import zmq
import subprocess
import shlex
import time
import sys
<<<<<<< HEAD
import time
=======

>>>>>>> f7329802ef73952ba897b8e75a0a9c63f1136ebe
context = zmq.Context()
port = "5556"
dbSocket = context.socket(zmq.PAIR)
dbSocket.bind("tcp://192.168.100.43:%s" % port)

processes = [] #list of processes
iterations = 0
HyperSpecAcqStarted = False

#  Sockets to talk to servers
#dbSocket = context.socket(zmq.REP)
#dbSocket.connect("tcp://192.168.1.100:5108")

while True:

    if dbSocket.poll(1) != 0:
    	dbCommand = dbSocket.recv()
        print dbCommand

    	if dbCommand == 'startHyperSpec':
            if HyperSpecAcqStarted == False:
                print "dbCommand = startHyperSpec"
        	    #if HyperSpecAcqStarted == False: #run HyperSpecAcq and verify
                    #testAcquitision = subprocess.call(shlex.split("python /home/rossebv/Desktop/RadMAP-Operatinos-GUI/Operations\ GUI/interfaceCodeV0.2.py &"))
                print "Acquisition Script Called"
<<<<<<< HEAD
                NIRAcquisition = subprocess.Popen('E:\\ResononAPI_2.2_Beta\\bin\\NIRHyperSpecAcquisitionV2.6.exe', cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
                processes.append(NIRAcquisition)
                print "NIR process starting"
                iAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\2iHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
                processes.append(iAcquisition)
                print "2i process started"
                HyperSpecAcqStarted = True
                time.sleep(30)
=======
                iAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\2iHyperSpecAcquisitionV2.6.exe", cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
                processes.append(iAcquisition)
                print "2i process started"
                NIRAcquisition = subprocess.Popen('E:\\ResononAPI_2.2_Beta\\bin\\NIRHyperSpecAcquisitionV2.6.exe', cwd=r'E:\\HS_Data\\', creationflags=subprocess.CREATE_NEW_CONSOLE, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
                processes.append(NIRAcquisition)
                print "NIR process starting"
                HyperSpecAcqStarted = True
>>>>>>> f7329802ef73952ba897b8e75a0a9c63f1136ebe
            else:
                print "HyperSpec Acquisition already running"
    		
        if dbCommand == 'stopHyperSpec':
            if HyperSpecAcqStarted: #stop HyperSpec if started
                print "Stopping HyperSpec Acquisition"
                processes.remove(iAcquisition)
                StopiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\2iStopScript.exe")
                print "2i Acquisition Stopped"
                processes.remove(NIRAcquisition)
                StopiAcquisition = subprocess.Popen("E:\\ResononAPI_2.2_Beta\\bin\\NIRStopScript.exe")
                print "NIR Acquisition Stopped"
                HyperSpecAcqStarted = False
            else:
                print "HyperSpec Acquisition already stopped"
        

<<<<<<< HEAD
    #print "Verifying Processes"	
    if len(processes) != 0:
        print "In Verification"
        line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
        if (line.find("line") != -1) and (iterations % 2 == 0): #2i verification
            #dbSocket.send("2i -" + line[line.find("line"): ])
            print "2i -" + line[line.find("line"): ]
        if (line.find("line") != -1) and (iterations % 2 == 1): #NIR Verification
            #dbSocket.send("NIR -" + line[line.find("line"): ])
            print "NIR -" + line[line.find("line"): ]
        yield line
=======
    print "Verifying Processes"	
    if len(processes) != 0:
        line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
        if (line.find("line") != -1) and (iterations % 2 = 0): #2i verification
            dbSocket.send("2i -" + line[line.find("line"): ])
            print "2i -" + line[line.find("line"): ]
        if (line.find("line") != -1) and (iterations % 2 = 1): #NIR Verification
            dbSocket.send("NIR -" + line[line.find("line"): ])
            print "NIR -" + line[line.find("line"): ]
>>>>>>> f7329802ef73952ba897b8e75a0a9c63f1136ebe
    iterations += 1
	
