import zmq
import subprocess
import glob
from datetime import datetime, date, time
import shlex
import string



context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://localhost:5108")

#booleans
velodynePortStarted = False
velodyneStarboardStarted = False
ladybugPortStarted = False
ladybugStarboardStarted = False
arduinoStarted = False
weatherStarted = False
GpsStarted = False


processes = [] #list of all the processes running
iterations = 0 #number of iterations through loop. used to index list of processes for verification

#States
createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set. if 1, ladybugs, lidar and arduino can be started
system_state = 0 #0 if sta command not received
#1 if sta command received and Lidar/Ladybug/Arduino collecting data


while True:
    #Verification if the different sensors are running
    # if !weatherStarted:
    #     #verify weather app
    #     if: #verification true
    #         weatherStarted = True
    # if !GpsStarted:
    #     #verify GPS started
    #     if: #verification true
    #         GpsStarted = True
    print "Verifying GPS and weather"
    
    
    if dbSocket.poll(milliseconds) != 0:
        dbCommand = dbSocket.recv()
        
        for cmd in dbCommand:
            if cmd == "STA":
                if createDirs_state == 0:
                    # createDirs = subprocess.Popen("createDirsGUI.bat", cwd=r"Path\\to\\createDirsGui", stdout = subprocess.PIPE) #set up the directories
                    # #verify createDirs
                    # date = datetime.now()
                    # filename = date.strftime("Run_%Y%m%d_%H" + "%M%S")
                    # checkFilename = glob.glob(filename)
                    # if checkFilename[0] == filename:
                    print "Creating directories"
                    dbSocket.send("Creating directories")
                    createDirs_state = 1
            if system_state == 0:
                if createDirs_state == 1: #data for lidar, ladybug, arduino can be saved in proper folders
                    if cmd == "startLidar":
                        if !velodynePortStarted: #run startVelodynePort and verify
                            # startVelodynePort = subprocess.Popen(shlex.split("start startVelodynePortGUI lidar\port 1"), cwd=r"Path\\to\\startVelodynePortGUI", stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                            # processes.append(startVelodynePort)
                            print "Starting Port Lidar"
                            dbSocket.send("Starting Port Lidar")
                            velodynePortStarted = True
                        else:
                            print "Port Lidar already running"
                            dbSocket.send("Port Lidar already running")
                        if !velodyneStarboardStarted: #run startVelodyneStarboard and verify
                            # startVelodyneStarboard = subprocess.Popen("start startVelodyneStarboardGUI lidar\starboard 1", cwd=r"Path\\to\\startVelodyneStarboardGUI", stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                            # processes.append(startVelodyneStarboard)
                            print "Starting Starboard Lidar"
                            dbSocket.send("Starting Starboard Lidar")
                            velodynePortStarted = True
                        else:
                            print "Starboard Lidar already started"
                            dbSocket.send("Starboard Lidar already started")
                    if cmd == "startLadybug":
                        if !ladybugPortStarted: #run startLadybugPort and verify
                            # startLadybugPort = subprocess.Popen("start startLadybugPortGUI ladybug\port 1", cwd=r"Path\\to\\startLadybugPortGUI", stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                            # processes.append(startLadybugPort)
                            print "Starting Port Ladybug"
                            dbSocket.send("Starboard Port Ladybug")
                            ladybugPortStarted = True
                        else:
                            print "Port Ladybug already started"
                            dbSocket.send("Port Ladybug already started")
                        if !ladybugStarboardStarted: #run startLadybugStarboad and verify
                            # startLadybugStarboard = subprocess.Popen("start startLadybugStarboardGUI ladybug\starboard 1", cwd=r"Path\\to\\startLadybugStarboardGUI", stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                            # processes.append(startLadybugStarboard)
                            print "Starting Starboard Ladybug"
                            dbSocket.send("Starting Starboard Ladybug")
                            ladybugStarboardStarted = True
                        else:
                            print "Starboard Ladybug already started"
                            dbSocket.send("Starboard Ladybug already started")
                    if cmd == "startArduino":
                        if !arduinoStarted: #run startArduino and verify
                            # startArduino = subprocess.Popen("start startArduinoGUI time_sync_files 1", cwd=r"Path\\to\\startArduinoGUI", stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                            # processes.append(startArduino)
                            print "Starting Arduino"
                            dbSocket.send("Starting Arduino")
                            arduinoStarted = True
                        else:
                            print "Arduino already started"
                            dbSocket.send("Arduino already started")
            if cmd == "startGps":
                if !GpsStarted:
                    # startGps = subprocess.Popen(shlex.split("startGPSscript"), cwd=r"Path\\to\\startGPSscript",  stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                    print "Starting GPS"
                    dbSocket.send("Starting gps")
                    GpsStarted = True
                else:
                    print "GPS already started"
                    dbSocket.send("GPS already started")
            if cmd == "startWeather":
                if !weatherStarted:
                    #startWeather = subprocess.Popen(shlex.split("startWeatherscript"), cwd=r"Path\\to\\startWeatherscript",  stdout = subprocess.PIPE,, stdin = subprocess.PIPE, stderr = subprocess.STDOUT)
                    print "Starting weather"
                    dbSocket.send("Starting weather")
                    weatherStarted = True
                else:
                    print "Weather already started"
                    dbSocket.send("weather already started")
            #if cmd == "STO":
            if system_state == 1: #input q then y
                if cmd == "stopLidar":
                    if velodynePortStarted: #stop lidars if started
                        # processes.remove(startVelodynePort)
                        # startVelodynePort.communicate(input = 'q')
                        print "Stopping Port Lidar"
                        dbSocket.send("Stopping Port Lidar")
                        velodynePortStarted = False
                    else:
                        print "Port Lidar already stopped"
                        dbSocket.send("Port Lidar already stopped")
                    if velodyneStarboardStarted:
                        # processes.remove(startVelodyneStarboard)
                        # startVelodyneStarboard.communicate(input = 'q')
                        print "Stopping Starboard Lidar"
                        dbSocket.send("Stopping Starboard Lidar")
                        velodyneStarboardStarted = False
                    else:
                        print "Starboard Lidar already stopped"
                        dbSocket.send("Starboard Lidar already stopped")
                if cmd == "stopLadybug":
                    if ladybugPortStarted: #stop ladybugs if started
                        # processes.remove(startLadybugPort)
                        # startLadybugPort.communicate(input = 'q')
                        print "Stopping Port Ladybug"
                        dbSocket.send("Stopping Port Ladybug")
                        ladybugPortStarted = False
                    else:
                        print "Port Ladybug already stopped"
                        dbSocket.send("Port Ladybug already stopped")
                    if ladybugStarboardStarted:
                        # processes.remove(startLadybugStarboard)
                        # startLadybugStarboard.communicate(input = 'q')
                        print "Stopping Starboard Ladybug"
                        dbSocket.send("Stopping Starboard Ladybug")
                        ladybugStarboardStarted = False
                    else:
                        print "Starboard Ladybug already stopped"
                        dbSocket.send("Starboard Ladybug already stopped")
                if cmd == "stopArduino":
                    if arduinoStarted: #stop arduino if started
                        # processes.remove(startArduino)
                        # startArduino.communicate(input = 'q')
                        print "Stopping Arduino"
                        dbSocket.send("Stopping Arduino")
                        arduinoStarted = False
                    else:
                        print "Arduino already stopped"
                        dbSocket.send("Arduino already stopped")
                if (!ladybugPortStarted) and (!ladybugStarboardStarted) and (!velodyneStarboardStarted) and (!velodynePortStarted) and (!arduinoStarted):
                    system_state = 0
            #stopWeather
            #stopGps
            
            if cmd == save: #data offload
                subprocess.call(#insert bug save script name here#)
                                
                                #Verification of Lidar, Ladybug, Arduino
    print "Verifying Lidar, Ladybug, and Arduino"
    # if len(processes) != 0:
    #     line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
    #     if (line.find("time_len: ") != -1): #arduino verification
    #         dbSocket.send("Arduino - " + line[line.find("time_len: "): ]
    #         if !arduinoStarted:
    #       arduinoStarted = True
    #     if (line.find("Images: ") != -1): #Ladybug Verification
    #               if (line.find("11291123") != -1): #port ladybug
    #               dbSocket.send("Port Ladybug - " + line[line.find("Images: "): line.find(" | MB")])
    #               if !velodynePortStarted:
    #               velodynePortStarted = True
    #               if (line.find("11260640") != -1): #starboard ladybug
    #               dbSocket.send("Starboard Ladybug - " + line[line.find("Images: "): line.find(" | MB")])
    #               if !velodyneStarboardStarted:
    #               velodyneStarboardStarted = True
    #               if ((line.find("Packets captured: ") != -1):
    #                   if (line.find("711024488") != -1): #starboard lidar
    #                   dbSocket.send("Starboard Lidar - " + line[line.find("Packets captured: "): ])
    #                   if !ladybugStarboardStarted:
    #                   ladybugStarboardStarted = True
    #                   if (line.find("711024572") != -1): #port lidar
    #                   dbSocket.send("Port Lidar - " + line[line.find("Packets captured: "): ])
    #                   if !ladybugPortStarted:
    #                   ladybugPortStarted = True
    #                   iterations += 1
    if velodynePortStarted and velodyneStarboardStarted and ladybugPortStarted and ladybugStarboardStarted and arduinoStarted: #if all verification true
        system_state = 1
                                                  
