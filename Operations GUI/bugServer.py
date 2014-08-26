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
gpsStarted = False


#States
createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set. if 1, ladybugs, lidar and arduino can be started
system_state = 0 #0 if sta command not received
                 #1 if sta command received and all sensors started collecting data



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
    
    
    

    if system_state == 1:
    #verify data for Lidars, Ladybugs, and Arduino
    dbSocket.send("Port Lidar -" + lidarVerificationString(startVelodynePort), NOBLOCK)
    dbSocket.send("Starboard Lidar -" + lidarVerificationString(startVelodyneStarboard), NOBLOCK)
    dbSocket.send("Port Ladybug -" + ladybugVerificationString(startLadybugPort), NOBLOCK)
    dbSocket.send("Starboard Ladybug -" + ladybugVerificationString(startLadybugStarboad), NOBLOCK)
    dbSocket.send("Arduino -" + arduinoVerificationString(startArduino), NOBLOCK)
    #socket send data/string/verification message

    if dbSocket.poll(milliseconds) != 0:
        dbCommand = dbSocket.recv()

        if dbCommand == sta: #start all sensors on bug
            weatherProcess = subprocess.Popen("weatherStartScript", cwd=r"Path\\to\\weatherStartScript", stdout = subprocess.PIPE) #start weather data
            GPSProcess = subprocess.Popen("GPSStartScript", cwd=r"Path\\to\\GPSStartScript", stdout = subprocess.PIPE)
            if createDirs_state == 0:
                createDirs = subprocess.Popen("createDirsGUI.bat", cwd=r"Path\\to\\createDirsGui", stdout = subprocess.PIPE) #set up the directories
                #verify createDirs
                date = datetime.now()
                checkFilename = glob.glob(filename)
                if checkFilename[0] == filename:
                    createDirs_state = 1
            if system_state == 0: 
                if createDirs_state == 1: #effectively runs the startCapture.bat file
                    if !velodynePortStarted: #run startVelodynePort and verify
                        startVelodynePort = subprocess.Popen(shlex.split("start startVelodynePortGUI lidar\port 1"), cwd=r"Path\\to\\startVelodynePortGUI", stdout = subprocess.PIPE)
                    if !velodyneStarboardStarted: #run startVelodyneStarboard and verify
                        startVelodyneStarboard = subprocess.Popen("start startVelodyneStarboardGUI lidar\starboard 1", cwd=r"Path\\to\\startVelodyneStarboardGUI", stdout = subprocess.PIPE)
                    if !ladybugPortStarted: #run startLadybugPort and verify
                        startLadybugPort = subprocess.Popen("start startLadybugPort ladybug\port 1", cwd=r"Path\\to\\startLadybugPortGUI", stdout = subprocess.PIPE)
                    if !ladybugStarboardStarted: #run startLadybugStarboad and verify
                        startLadybugStarboad = subprocess.Popen("start startLadybugStarboard ladybug\starboard 1", cwd=r"Path\\to\\startLadybugStarboardGUI", stdout = subprocess.PIPE)
                    if !arduinoStarted: #run startArduino and verify
                        startArduino = subprocess.Popen("start startArduino time_sync_files 1", cwd=r"Path\\to\\startArduinoGUI", stdout = subprocess.PIPE)
                    if velodynePortStarted and velodyneStarboardStarted and ladybugPortStarted and ladybugStarboardStarted and arduinoStarted: #if all verification true
                        system_state = 1
                
        if dbCommand == stp:
            if system_state == 1:

        if dbCommand == save: #data offload
            subprocess.call(#insert bug save script name here#)

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