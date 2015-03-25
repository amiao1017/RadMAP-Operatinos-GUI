import zmq
import subprocess
import glob
from datetime import datetime, date, time
import shlex
import string
import time, glob
import os
import pyperclip

context = zmq.Context()

#  Sockets to talk to servers
interfaceCapturePort = "5551"
interfaceSocketCapture = context.socket(zmq.REP)
interfaceSocketCapture.bind("tcp://192.168.100.42:%s" % interfaceCapturePort)
lidarVerificationPort = "5563"
lidarVerificationSocket = context.socket(zmq.REP)
lidarVerificationSocket.bind("tcp://192.168.100.42:%s" % lidarVerificationPort)
ladybugVerificationPort = "5564"
ladybugVerificationSocket = context.socket(zmq.REP)
ladybugVerificationSocket.bind("tcp://192.168.100.42:%s" % ladybugVerificationPort)
arduinoVerificationPort = "5565"
arduinoVerificationSocket = context.socket(zmq.REP)
arduinoVerificationSocket.bind("tcp://192.168.100.42:%s" % arduinoVerificationPort)
interfaceGpsPort = "5552"
interfaceSocketGps = context.socket(zmq.REP)
interfaceSocketGps.bind("tcp://192.168.100.42:%s" % interfaceGpsPort)
gpsVerificationPort = "5566"
gpsVerificationSocket = context.socket(zmq.REP)
gpsVerificationSocket.bind("tcp://192.168.100.42:%s" % gpsVerificationPort)

#booleans

gpsStarted = False
gpsVerification = False
gpsAcquisitionStopped = False
velodynePortVerification = False
velodyneStarboardVerification = False
velodyneVerification = False
ladybugPortVerification = False
ladybugStarboardVerification = False
ladybugVerification = False
arduinoVerification = False
startCaptureStarted = False
startCaptureAcquisitionStopped = False


processes = [] #list of all the processes running
iterations = 0 #number of iterations through loop. used to index list of processes for verification

#States
createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set. if 1, ladybugs, lidar and arduino can be started
system_state = 0 #0 if sta command not received
#1 if sta command received and Lidar/Ladybug/Arduino collecting data


while True:
    
	if interfaceSocketCapture.poll(100) != 0: #potentially add a timeout to the socket poll
		dbCommand = interfaceSocketCapture.recv()
		
		
		if dbCommand == "startCapture":
                        interfaceSocketCapture.send("startCapture Received - Wait 5 Seconds")
			if startCaptureStarted == False:
				print "Command Received - %s" % dbCommand
				startCapture = subprocess.Popen("startCapture.bat", cwd=r'E:', creationflags=subprocess.CREATE_NEW_CONSOLE)
				date = datetime.now()
				time.sleep(1)
				startArduino = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\Server Code\\startArduino.exe")
        		else:
				print "startCapture already started"

		if dbCommand == "stopCapture":
			if startCaptureStarted == True:
				print "Command Received - %s" % dbCommand
				interfaceSocketCapture.send("stopCapture Command Received")
				stopCapture = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations Gui\\Server Code\\StopCaptureScript.exe")
				startCaptureStarted = False
				time.sleep(10)
				stopCapture = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations Gui\\Server Code\\StopCaptureVerificationScript.exe")
                                stopped = "%s" % pyperclip.paste()
                                #if stopped.find("True") != -1:
                                        
                                        
			else:
				print "startCapture already stopped"

                if dbCommand == "Started?":
                        time.sleep(4)
                        interfaceSocketCapture.send("startCapture Acquisition Starting")
			startCaptureStarted = True

				
	if interfaceSocketGps.poll(100) != 0: #potentially add a timeout to the socket poll
		dbCommand = interfaceSocketGps.recv()
		
		if dbCommand == "startGPS":
                        interfaceSocketGps.send("startGPS Received - Wait 20 Seconds")
			if gpsStarted == False:
				gpsAcquisitionStarted = False
				print "Command Received - %s" % dbCommand
				searchName = date.strftime("Run_%Y%m%d_%H%M%S")
				#fileName = glob.glob('%s' %searchName)
				#fileName = fileName
				logfile_path = 'E:\\' + searchName + '\gps\logfile.gps' #set the path for the logfile to be saved with gps data collection
				#logfile_path = date.strftime("\gps_%Y%m%d_%H%M%S_logfile.gps")
				pyperclip.copy(logfile_path)
				startGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPS_Start_Script.exe")
				
				
			else:
				print "GPS already started"

		if dbCommand == "stopGPS":
			if gpsStarted == True:
				print "Command Received - %s" % dbCommand
				interfaceSocketGps.send("stopGPS Command Received")
				startGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPS_Stop_Script.exe")
				gpsStarted = False
                                time.sleep(4)
				checkGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPSCheck.exe")
				stopped = "%s" % pyperclip.paste()
                                if stopped.find("True") != -1:
                                        gpsAcquisitionStopped = True
                                        
                                        
			else:
				print "GPS already stopped"

		if dbCommand == "Started?":
                        time.sleep(20)
			interfaceSocketGps.send("GPS Acquisition Starting")
			verifyGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPSVerification.exe")
			gpsStarted = True
	
	
	if gpsStarted == True:
		gpsString = "%s" % pyperclip.paste()
		if gpsString.find("True") != -1:
			gpsVerification = True
		else:
			gpsVerification = False
		#print "%s" % gpsVerification
	if gpsVerificationSocket.poll(100) != 0:
		gpsVerificationSocket.recv()
		if not gpsAcquisitionStopped:
			gpsVerificationSocket.send("%s" % gpsVerification)
		else:
			gpsVerificationSocket.send("Closed")


