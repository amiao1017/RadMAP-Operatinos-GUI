import zmq
import subprocess
import glob
from datetime import datetime, date, time
import shlex
import string
from Tkinter import Tk



context = zmq.Context()

#  Sockets to talk to servers
dbSocket = context.socket(zmq.REP)
dbSocket.connect("tcp://192.168.1.100:5108")

#booleans
velodynePortStarted = False
velodyneStarboardStarted = False
ladybugPortStarted = False
ladybugStarboardStarted = False
arduinoStarted = False
GpsStarted = False


processes = [] #list of all the processes running
iterations = 0 #number of iterations through loop. used to index list of processes for verification

#States
createDirs_state = 0 #0 if directories to be saved into not set up, 1 if set. if 1, ladybugs, lidar and arduino can be started
system_state = 0 #0 if sta command not received
#1 if sta command received and Lidar/Ladybug/Arduino collecting data


while True:

    
    
    if dbSocket.poll(100) != 0: #potentially add a timeout to the socket poll
        dbCommand = dbSocket.recv()
        if dbCommand.find("STA") != -1:
			print "Command Received"
			dbSocket.send("Command Received")
	
                                                  
