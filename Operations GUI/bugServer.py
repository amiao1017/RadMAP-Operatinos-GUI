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
            while weather_state == 0:
                #start weather app
                #verify weather app
                #if verification true
                    weather_state = 1
            while GPS_state == 0:
                #start GPS
                #verify GPS started
                #if verification true
                    GPS_state = 1
            
            
			#subprocess.Popen("setMistiConfig", cwd=r"path\\to\\setMistiConfig", stdout = subprocess.PIPE)
			#verify setMistiConfig
			#subprocess.Popen("createDirs.bat", cwd=r"C:\\path\\to\\createDirs.bat", stdout = subprocess.PIPE)
			#verify createDirs
			while lidar_state == 0:
                #run startVelodynePort and verify
                #if verification case is true
                    lidar_state = 1 #if verification is true
            while lidar_state == 1:
                #run startVelodyneStarboard and verify
                #if verification case is true
                    lidar_state = 2
            while ladybug_state == 0:
                #run startLadybugPort and verify
                #if verification true
                    ladybug_state = 1
            while ladybug_state == 1:
                #run startLadybugStarboad and verify
                #if verification true
                    ladybug_state = 2
            while arduino_state == 0:
                #run startArduino and verify
                #if verification true
                    arduino_state = 1
            if (weather_state == 1) and (GPS_state == 1) and (lidar_state == 2) and (ladybug_state == 2) and (arduino_state == 1):
                system_state = 1

	if dbCommand == stp:
        if system_state == 1:
            while weather_state == 1:
                #stop weather app
                #verify stop
                #if verification true
                    weather_state = 0
            while GPS_state == 1:
                #stop GPS
                #verify GPS stopped
                #if verification true
                    GPS_state = 1
            while lidar_state == 2: #both port and starboard lidar on
                #stop starboard lidar
                #verify stoppage
                #if stopped
                    lidar_state = 1
            while lidar_state == 1: #port lidar on
                #stop port lidar
                #verify stoppage
                #if stopped
                    lidar_state = 0
            while ladybug_state == 2:
                #stop starboard ladybug
                #verify stoppage
                #if stopped
                    ladybug_state = 1
            while ladybug_state == 1:
                #stop port ladybug
                #verify stoppage
                #if stopped
                    ladybug_state = 0
            while arduino_state == 1:
                #stop arduino
                #verify stoppage
                #if stopped
                    arduino_state = 0
            if (weather_state == 0) and (GPS_state == 0) and (lidar_state == 0) and (ladybug_state == 0) and (arduino_state == 0):
                system_state = 0
    
    if system_state == 1: #the start message has been received and all the sensors are running
        #weather verification
        #GPS verification
        #ladybug/lidar/arduino verification
        
        
                

	if dbCommand == save: #data offload
		subprocess.call(#insert bug save script name here#)
		
	#function to display live data