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
port = "5551"
dbSocket = context.socket(zmq.PAIR)
dbSocket.bind("tcp://192.168.100.42:%s" % port)

#booleans
velodynePortStarted = False
velodyneStarboardStarted = False
ladybugPortStarted = False
ladybugStarboardStarted = False
arduinoStarted = False
GpsStarted = False
startCaptureStarted = False

GpsVerification = False



processes = [] #list of all the processes running
iterations = 0 #number of iterations through loop. used to index list of processes for verification

#States
createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set. if 1, ladybugs, lidar and arduino can be started
system_state = 0 #0 if sta command not received
#1 if sta command received and Lidar/Ladybug/Arduino collecting data


while True:
    
	if dbSocket.poll(100) != 0: #potentially add a timeout to the socket poll
		dbCommand = dbSocket.recv()
		
		
		if dbCommand == "startCapture":
			if startCaptureStarted == False:
				print "Command Received - %s" % dbCommand
				dbSocket.send("startCapture Command Received")
				date = datetime.now()
				currTime = date.strftime("%Y%m%d%H%M%S")
				home = "E:\\startCapture_stdout\\"
				startCaptureFolderName = "stdOut_2i_%s" %currTime
				startCaptureFileName = "%s.txt" %startCaptureFolderName
				startCapturePath = home + startCaptureFolderName
				if not os.path.exists(startCapturePath):
					os.makedirs(startCapturePath)
				startCaptureFile = open("%s\\%s" %(startCapturePath, startCaptureFileName), 'a+')
				startCapture = subprocess.Popen("startCapture.bat", cwd=r'E:', creationflags=subprocess.CREATE_NEW_CONSOLE, stdout = startCaptureFile)
				date = datetime.now()
				time.sleep(1)
				startArduino = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\Server Code\\startArduino.exe")
				startCaptureStarted = True
			else:
				print "startCapture already started"
		if dbCommand == "startGPS":
			if GpsStarted == False:
				print "Command Received - %s" % dbCommand
				dbSocket.send("startGPS Command Received")
				searchName = date.strftime("Run_%Y%m%d_%H%M%S")
				#fileName = glob.glob('%s' %searchName)
				#fileName = fileName
				logfile_path = 'E:\\' + searchName + '\gps\logfile.gps' #set the path for the logfile to be saved with gps data collection
				#logfile_path = date.strftime("\gps_%Y%m%d_%H%M%S_logfile.gps")
				pyperclip.copy(logfile_path)
				startGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPS_Start_Script.exe")
				timeout = time.time() + 20
				while not GpsStarted: #make sure GPS started correctly
					checkGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPSCheck.exe")
					GpsString = "%s" % pyperclip.paste()
					if GpsString.find("True") != -1:
						GpsStarted = True
					if time.time() > timeout:
						break
				if GpsStart:		
					verifyGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPSVerification.exe")
				else:
					#send false + fail message?
			else:
				print "GPS already started"

		if dbCommand == "stopCapture":
			if startCaptureStarted == True:
				print "Command Received - %s" % dbCommand
				dbSocket.send("stopCapture Command Received")
				stopCapture = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations Gui\\Server Code\\StopCaptureScript.exe")
				startCaptureFile.close()
				startCaptureStarted = False
				#time.sleep(10)
			else:
				print "startCapture already stopped"

		if dbCommand == "stopGPS":
			if GpsStarted == True:
				print "Command Received - %s" % dbCommand
				dbSocket.send("stopGPS Command Received")
				startGps = subprocess.Popen("C:\\Users\\misti\\Desktop\\RadMAP-Operatinos-GUI\\Operations GUI\\AutoHotKey Scripts\\GPS_Stop_Script.exe")
				GpsStarted = False
				#time.sleep(4)
			else:
				print "GPS already stopped"
	
	if GpsStarted == True:
		GpsString = "%s" % pyperclip.paste()
		if GpsString.find("True") != -1:
			GpsVerification = True
		else:
			GpsVerification = False
		print "%s" % GpsVerification
		#dbSocket.send("GPS %s" % GpsVerification)


