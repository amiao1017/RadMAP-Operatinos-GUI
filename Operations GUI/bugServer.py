import zmq
import subprocess

context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://localhost:5108")


createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set
weather_state = 0 #0 if weather not started, 1 if started
GPS_state = 0 #0 if Novatel GPS not started, 1 if started
ladybug_state = 0 #0 if ladybug not started, 1 if only port started, 2 if port and starboard started
lidar_state = 0 #0 if lidar not started, 1 if only port started, 2 if port and starboard started
arduino_state = 0 #0 if startArduino not started, 1 if started
system_state = 0 #0 if weather
                 #1 if ladybug, lidar, and arduino not started, but system ready to start capturing data
                 #2 if all sensors started and capturing data



while True:
    #Verification if the different sensors are running
    if weather_state == 0:
    #verify weather app
    if: #verification true
    weather_state = 1
    else: #start weather app
    #verify weather app
    #if verification true
    weather_state = 1
    if GPS_state == 0:
    #verify GPS started
    if: #verification true
    GPS_state = 1
    else: #start GPS
    #verify GPS started
    #if verification true
    GPS_state = 1
    
    
    

    if createDirs_state == 1 and system_state == 1:
    #verify data for Lidars, Ladybugs, and Arduino
    #socket send data/string/verification message

    if dbSocket.poll(milliseconds) != 0:
        dbCommand = dbSocket.recv()

        if dbCommand == sta:
            if createDirs_state == 0:
                createDirs = subprocess.Popen("createDirsGUI.bat", cwd=r"", stdout = subprocess.PIPE) #set up the directories
                #verify createDirs
                createDirs_state = 1
            if system_state == 0: 
                if weather_state == 0:
                    #verify weather app
                    if: #verification true
                    weather_state = 1
                    else: #start weather app
                        #verify weather app
                        #if verification true
                        weather_state = 1
                if GPS_state == 0:
                    #verify GPS started
                    if: #verification true
                    GPS_state = 1
                    else: #start GPS
                        #verify GPS started
                        #if verification true
                        GPS_state = 1
                if createDirs_state == 1: #effectively runs the startCapture.bat file
                    #run startVelodynePort and verify
                    #run startVelodyneStarboard and verify
                    #run startLadybugPort and verify
                    #run startLadybugStarboad and verify
                    #run startArduino and verify
                    #if all verification true
                        system_state = 1
                
        if dbCommand == stp:
            if system_state == 1:
#                if weather_state == 1:
#                    #stop weather app
#                    #verify stop
#                    #if verification true
#                        weather_state = 0
#                if GPS_state == 1:
#                    #stop GPS
#                    #verify GPS stopped
#                    #if verification true
#                        GPS_state = 1
                if lidar_state == 2: #both port and starboard lidar on
                    #stop starboard lidar
                    #verify stoppage
                    #if stopped
                        lidar_state = 1
                if lidar_state == 1: #port lidar on
                    #stop port lidar
                    #verify stoppage
                    #if stopped
                        lidar_state = 0
                if ladybug_state == 2:
                    #stop starboard ladybug
                    #verify stoppage
                    #if stopped
                        ladybug_state = 1
                if ladybug_state == 1:
                    #stop port ladybug
                    #verify stoppage
                    #if stopped
                        ladybug_state = 0
                if arduino_state == 1:
                    #stop arduino
                    #verify stoppage
                    #if stopped
                        arduino_state = 0
                if (weather_state == 0) and (GPS_state == 0) and (lidar_state == 0) and (ladybug_state == 0) and (arduino_state == 0):
                    system_state = 0
            
                    

        if dbCommand == save: #data offload
            subprocess.call(#insert bug save script name here#)


	#function to display live data