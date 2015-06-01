import ahk
import time
script = ahk.Script()
script.variable("text", str) #set AHK variable that can be accessed with Python
ahk.execute("SetTitleMatchMode 2") 
currLines = []
prevLines = []

while (True):
	#gpsStatus = ahk.execute("WinActivate, NovAtel")
	#if gpsStatus == True: #if GPS Window exists
	#	print "Exists!"
	#	messageStatus = ahk.execute("WinActivate, CONFIG - ASCII")
	#	if messageStatus == True: #if GPS ASCII coordinate window exists
	ahk.execute("WinGetText, text, CONFIG - ASCII")
	curr_text = script.text
	#print curr_text
	lines = curr_text.split('\n')
	for line in lines:
		if line not in prevLines:
			currLines.append(line)
			if line.find("#BESTPOS") != -1: #if line has GPS coordinates, splice the string
				gpsCoords = line[line.find(';'):line.find("WGS")]
				gpsCoords = gpsCoords[gpsCoords.find("SINGLE") + 7:]
				gpsCoords = gpsCoords[:gpsCoords.rfind(',')]
				gpsCoords = gpsCoords[:gpsCoords.rfind(',')]
				gpsCoords = gpsCoords[:gpsCoords.rfind(',')]
				gpsCoords = gpsCoords[gpsCoords.find(',')+1:]
				gpsCoords = gpsCoords[gpsCoords.find(',')+1:]				
				print gpsCoords #GPS coordinates in form "lat, lon"
			#if messageStatus == False:
	if curr_text == "":
		print "CONFIG Window not open. Cannot read GPS Coordinates"
	prevLines = currLines
	currLine = []
	time.sleep(1)
	#if gpsStatus == False:
	#	print "GPS application not started. Cannot read GPS Coordinates"
	#	break
