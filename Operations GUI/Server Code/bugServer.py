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

    if dbSocket.poll(milliseconds) != 0:
        dbCommand = dbSocket.recv()

        for cmd in dbCommand:
            if cmd == "STA":
                if createDirs_state == 0:
                    createDirs = subprocess.Popen("createDirsGUI.bat", cwd=r"Path\\to\\createDirsGui", stdout = subprocess.PIPE) #set up the directories
                    #verify createDirs
                    date = datetime.now()
                    filename = date.strftime("Run_%Y%m%d_%H" + "%M%S")
                    checkFilename = glob.glob(filename)
                    if checkFilename[0] == filename:
                        createDirs_state = 1
            if system_state == 0: 
                if createDirs_state == 1: #data for lidar, ladybug, arduino can be saved in proper folders
                    if cmd == "startLidar":
                        if !velodynePortStarted: #run startVelodynePort and verify
                            startVelodynePort = subprocess.Popen(shlex.split("start startVelodynePortGUI lidar\port 1"), cwd=r"Path\\to\\startVelodynePortGUI", stdin = subprocess.PIPE, stdout = subprocess.PIPE)
                            processes.append(startVelodynePort)
                        if !velodyneStarboardStarted: #run startVelodyneStarboard and verify
                            startVelodyneStarboard = subprocess.Popen("start startVelodyneStarboardGUI lidar\starboard 1", cwd=r"Path\\to\\startVelodyneStarboardGUI", stdin = subprocess.PIPE, stdout = subprocess.PIPE)
                            processes.append(startVelodyneStarboard)
                    if cmd == "startLadybug":
                        if !ladybugPortStarted: #run startLadybugPort and verify
                            startLadybugPort = subprocess.Popen("start startLadybugPortGUI ladybug\port 1", cwd=r"Path\\to\\startLadybugPortGUI", stdin = subprocess.PIPE, stdout = subprocess.PIPE)
                            processes.append(startLadybugPort)
                        if !ladybugStarboardStarted: #run startLadybugStarboad and verify
                            startLadybugStarboard = subprocess.Popen("start startLadybugStarboardGUI ladybug\starboard 1", cwd=r"Path\\to\\startLadybugStarboardGUI", stdin = subprocess.PIPE, stdout = subprocess.PIPE)
                            processes.append(startLadybugStarboard)
                    if cmd == "startArduino":
                        if !arduinoStarted: #run startArduino and verify
                            startArduino = subprocess.Popen("start startArduinoGUI time_sync_files 1", cwd=r"Path\\to\\startArduinoGUI", stdin = subprocess.PIPE, stdout = subprocess.PIPE)
                            processes.append(startArduino)
            if cmd == "startGps":
                startGps = subprocess.Popen(shlex.split("startGPSscript"), cwd=r"Path\\to\\startGPSscript",  stdin = subprocess.PIPE, stdout = subprocess.PIPE)
            if cmd == "startWeather":
                startWeather = subprocess.Popen(shlex.split("startWeatherscript"), cwd=r"Path\\to\\startWeatherscript",  stdin = subprocess.PIPE,, stdout = subprocess.PIPE)
                
        if dbCommand == stp:
            if system_state == 1:
                #input q then y
                #stopArduino
                #stopLidar
                #stopLadybug
                #stopWeather
                #stopGps

        if dbCommand == save: #data offload
            subprocess.call(#insert bug save script name here#)

    #Verification if the different sensors are running
    if !weatherStarted == 0:
        #verify weather app
        if: #verification true
            weatherStarted = 1
    if !Gps_started:
        #verify GPS started
        if: #verification true
            Gps_started = True
    
    #Verification of Lidar, Ladybug, Arduino
    if len(processes) != 0:
        line = processes[iterations % len(processes)].stdout.readline() #NEED TIMEOUT TO PREVENT BLOCKING
        if (line.find("time_len: ") != -1): #arduino verification
            dbSocket.send("Arduino - " + line[line.find("time_len: "): ]
            if !arduinoStarted:
                arduinoStarted = True
        if (line.find("Images: ") != -1): #Ladybug Verification
            if (line.find("11291123") != -1): #port ladybug
                dbSocket.send("Port Ladybug - " + line[line.find("Images: "): line.find(" | MB")])
                if !velodynePortStarted:
                    velodynePortStarted = True
            if (line.find("11260640") != -1): #starboard ladybug
                dbSocket.send("Starboard Ladybug - " + line[line.find("Images: "): line.find(" | MB")])
                if !velodyneStarboardStarted:
                    velodyneStarboardStarted = True
        if ((line.find("Packets captured: ") != -1):
            if (line.find("711024488") != -1): #starboard lidar
                dbSocket.send("Starboard Lidar - " + line[line.find("Packets captured: "): ])
                if !ladybugStarboardStarted:
                    ladybugStarboardStarted = True
            if (line.find("711024572") != -1): #port lidar
                dbSocket.send("Port Lidar - " + line[line.find("Packets captured: "): ])
                if !ladybugPortStarted:
                    ladybugPortStarted = True
        iterations += 1
        if velodynePortStarted and velodyneStarboardStarted and ladybugPortStarted and ladybugStarboardStarted and arduinoStarted: #if all verification true
            system_state = 1

#function to generate outputs of processes line by line. Returns a boolean True if verified or breaks and returns false because process is not running
def verifyProcess(process, verification_string): #process is a subprocess. verification_string is a string that shows that the process has started collecting data correctly
    verified = False #boolean if verified
    while (!verified): #while we do not know if the process started properly
        line = process.stdout.readline()
        if (line.find(verification_string) != (-1)): #check output of process with verification_string
            verified = True
        if (process.poll() is not None): #if process.poll() is not None then the process is not running
            break
    return verified

#function to generate lidar verification string. Returns a string of the packet size
def lidarVerificationString(process):
    return_string = ''
    line = process.stdout.readline()
    if (line.find("Packets captured: ") != -1):
        return_string = line[line.find("Packets captured: "): ]
    return return_string

#function to generate ladybug verification string. Returns a string of the images
def ladybugVerificationString(process):
    return_string = ''
    line = process.stdout.readline()
    if (line.find("Images: ") != -1):
        return_string = line[line.find("Images: "): line.find(" | MB")]
    return return_string

#function to generate arduino verification string. Returns a string of the time_len
def arduinoVerificationString(process):
    return_string = ''
    line = process.stdout.readline()
    if (line.find("time_len: ") != -1):
        return_string = line[line.find("time_len: "): ]
    return return_string