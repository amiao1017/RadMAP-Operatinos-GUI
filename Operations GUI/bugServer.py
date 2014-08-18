import zmq
import subprocess

context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://localhost:5108")

setMistiConfig_state = 0 #0 if MISTI_CONFIG not set, 1 if set
createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set
ladybug_state = 0 #0 if ladybug not started, 1 if only port started, 2 if port and starboard started
lidar_state = 0 #0 if lidar not started, 1 if only port started, 2 if port and starboard started
arduino_state = 0 #0 if startArduino not started, 1 if started
weather_state = 0 #0 if weather not started, 1 if started
GPS_state = 0 #0 if Novatel GPS not started, 1 if started
system_state = 0 #0 if ladybug, lidar, arduino all not started, 1 if ladybug, lidar, arduino, weather, GPS all started
				 #need to check if we also need to turn off weather and GPS



while True:

	dbCommand = dbSocket.recv()

	if dbCommand == sta:
		if system_state == 0:
			#subprocess.Popen("setMistiConfig", cwd=r"path\\to\\setMistiConfig", stdout = subprocess.PIPE)
			#verify setMistiConfig
			#subprocess.Popen("createDirs.bat", cwd=r"C:\\path\\to\\createDirs.bat", stdout = subprocess.PIPE)
			#verify createDirs
			while lidar_state = 0:
					#run startVelodynePort and verify
					lidar_state = 1 #if verification is true


			#starboard lidar and verification
			#




	#based on what the start script returns may want to use subprocess.callback() and use resulting info for data verification
		subprocess.call(#insert bug start script name here#)
		
	if dbCommand == stp:
		subprocess.call(#insert bug stop script name here#)
	
	if dbCommand == save: #data offload
		subprocess.call(#insert bug save script name here#)
		
	#function to display live data