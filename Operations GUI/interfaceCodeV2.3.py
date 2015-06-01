from __future__ import division
from Tkinter import *
from datetime import datetime, date, time
from string import whitespace
from PIL import Image, ImageTk
import os, sys, glob, Tkconstants,tkFileDialog,tkMessageBox,glob,tkFont,subprocess,zmq,time

context = zmq.Context()

#  Sockets to talk to servers
interfacePortGamma = "5559"
interfaceSocketGamma = context.socket(zmq.REQ)		
interfaceSocketGamma.connect("tcp://192.168.100.2:%s" % interfacePortGamma)
interfacePortCapture = "5551"
interfaceSocketCapture = context.socket(zmq.REQ)
interfaceSocketCapture.connect("tcp://192.168.100.42:%s" % interfacePortCapture)
lidarVerificationPort = "5563"
lidarVerificationSocket = context.socket(zmq.REQ)
lidarVerificationSocket.connect("tcp://192.168.100.42:%s" % lidarVerificationPort)
ladybugVerificationPort = "5564"
ladybugVerificationSocket = context.socket(zmq.REQ)
ladybugVerificationSocket.connect("tcp://192.168.100.42:%s" % ladybugVerificationPort)
arduinoVerificationPort = "5565"
arduinoVerificationSocket = context.socket(zmq.REQ)
arduinoVerificationSocket.connect("tcp://192.168.100.42:%s" % arduinoVerificationPort)
interfacePortGps = "5552"
interfaceSocketGps = context.socket(zmq.REQ)
interfaceSocketGps.connect("tcp://192.168.100.42:%s" % interfacePortGps)
gpsVerificationPort = "5566"
gpsVerificationSocket = context.socket(zmq.REQ)
gpsVerificationSocket.connect("tcp://192.168.100.42:%s" %gpsVerificationPort)
interfacePortLiq = "5557"
interfaceSocketLiq = context.socket(zmq.REQ)
interfaceSocketLiq.connect("tcp://192.168.100.23:%s" % interfacePortLiq)
liqVerificationPort = "5567"
liqVerificationSocket = context.socket(zmq.REQ)
liqVerificationSocket.connect("tcp://192.168.100.23:%s" % liqVerificationPort)
interfacePortHs = "5556"
interfaceSocketHs = context.socket(zmq.REQ)
interfaceSocketHs.connect("tcp://192.168.100.43:%s" % interfacePortHs)
hsVerificationPort = "5555"
hsVerificationSocket = context.socket(zmq.REQ)
hsVerificationSocket.connect("tcp://192.168.100.43:%s" % hsVerificationPort)

class operationsApp(Tk):

	
	def __init__(self,parent):
		Tk.__init__(self,parent)
		self.parent = parent
		self.initialize()
		self.after(5000, self.verificationMonitor)

		#define options for opening directory explorer
		self.dir_opt = options = {}
		options['initialdir'] = 'C:\\'
		options['mustexist'] = False
		options['parent'] = parent
		options['title'] = 'MISTI Operations GUI'

	def initialize(self):
		self.grid()
		self.clock = StringVar()
		self.clock.set("Session Not Started")
		self.operator = StringVar()
		self.clickedOperator = False
		self.clickedNotes = False
		self.startClicked = IntVar()
		self.stopClicked = IntVar()
		self.systemClicked = IntVar()
		self.startTime = 0
		self.elapsedTime = IntVar()
		self.setOperatorCounter= 0
		self.startList = []
		self.stopList = []
		self.startCapture = IntVar()
		self.startGps = IntVar()
		self.startNeutrons = IntVar()
		self.startHyperSpec = IntVar()
		self.stopCapture = IntVar()
		self.stopGps = IntVar()
		self.stopNeutrons = IntVar()
		self.stopHyperSpec = IntVar()
		self.verificationGammaColor = StringVar()
		self.verificationNeutronHvColor = StringVar()		
		self.verificationDbusColor = StringVar()
		self.verificationBusExtMonColor = StringVar()
		self.verificationStartMistiColor = StringVar()
		self.verificationArduinoColor = StringVar()
		self.verificationLidarColor = StringVar()
		self.verificationLadybugColor = StringVar()
		self.verificationWeatherColor = StringVar()
		self.verificationGpsColor = StringVar()
		self.verificationNeutronsColor = StringVar()
		self.verificationHyperSpecColor = StringVar()
		self.arduinoDataCheck = StringVar()
		self.ladybugDataCheck = StringVar()
		self.lidarDataCheck = StringVar()
		self.timestampedNotes = StringVar()
		self.dataOffloadNotes = StringVar()
		self.notesVar = StringVar()
		self.displayVar = StringVar()
		self.fileName = StringVar()
		self.fileOpened = False
		self.noteSaved = False
		self.multipleFiles = 0
		self.startCaptureFlag = 0
		self.startCaptureCommand = "startCapture"
		self.startHyperSpecCommand = "startHyperSpec"
		self.startHyperSpecFlag = 0
		self.startNeutronsFlag = 0
		self.startGpsFlag = 0
		self.hyperSpecStarted = 0
		self.startCaptureStarted = 0
		self.gpsStarted = 0
		self.neutronsStarted = 0
		self.hyperSpecStopped = 0
		self.startCaptureStopped = 0
		self.gpsStopped = 0
		self.neutronsStopped = 0
		self.acqStop = 0
		self.stopSelected = 0
		self.acqStarted = 0
		self.troubleshooting = 1

		self.operatorGrid = Label (self)
		self.operatorGrid.grid(column=0,row=0)

		self.operatorLabel = Label(self.operatorGrid,anchor="w",text="Operator:", font=2)
		self.operatorLabel.grid(column=0,row=0)
		labelUnderlined = tkFont.Font(self.operatorLabel,self.operatorLabel.cget("font"))
		labelUnderlined.configure(underline = True)
		self.operatorLabel.configure(font=labelUnderlined)

		self.entryOperator = Entry(self.operatorGrid, textvariable=self.operator, width=25)
		self.entryOperator.grid(column=1,row=0)
		self.entryOperator.bind("<Return>", self.setOperatorEnter)
		self.entryOperator.bind("<Button-1>", lambda number: self.entryClicked(1))
		self.entryOperator.insert(0,"Enter Operator Name Here")

		self.setOperator = Button(self.operatorGrid,text="Set Operator",activebackground="blue",activeforeground="white",relief=RAISED, command = self.setOperatorClick)
		self.setOperator.grid(column=2,row=0)

		#space = Label(self,width=5)
		#space.grid(column=3,row=0)

		self.elapsedTimeGrid = Label(self)
		self.elapsedTimeGrid.grid(column=0,row=1)

		self.displayElapsedTimeLabel = Label(self.elapsedTimeGrid, text="Time Since Start of Session:")
		self.displayElapsedTimeLabel.grid(column=0,row=0)
		labelUnderlined = tkFont.Font(self.displayElapsedTimeLabel,self.displayElapsedTimeLabel.cget("font"))
		labelUnderlined.configure(underline = True)
		self.displayElapsedTimeLabel.configure(font=labelUnderlined)

		self.displayElapsedTime = Label(self.elapsedTimeGrid, textvariable=self.elapsedTime,width=4)
		self.displayElapsedTime.grid(column=1,row=0)

		self.displayClock = Label(self.elapsedTimeGrid, textvariable=self.clock, anchor="e", bg="gray",width=16)
		self.displayClock.grid(column=2,row=0)

		self.communicationDisplayGrid = Label(self)
		self.communicationDisplayGrid.grid(column=0,row=2)

		self.communicationDisplay = Label(self.communicationDisplayGrid, anchor=S, textvariable=self.displayVar, bg="white", relief=RAISED, width = 36, height=2, wraplength=275)
		self.communicationDisplay.grid(column=0,row=0,columnspan=3)

		self.mainControlGrid = Label(self)
		self.mainControlGrid.grid(column=0,row=3)

		self.mainStart = Button(self.mainControlGrid,text="START",activebackground="blue",activeforeground="white",relief=RAISED, command = self.mainStartClick)
		self.mainStart.grid(column=0,row=0)

		self.mainStop = Button(self.mainControlGrid,text="STOP",activebackground="blue",activeforeground="white",relief=RAISED, command = self.mainStopClick)
		self.mainStop.grid(column=1,row=0)

		self.indStart = Label(self.mainControlGrid)
		self.indStart.grid(column=0,row=1)

		self.startHyperSpecCheck = Checkbutton(self.indStart,variable=self.startHyperSpec,text="Start HyperSpec", command = self.startHyperSpecClick)
		self.startHyperSpecCheck.pack()

		self.startCaptureCheck = Checkbutton(self.indStart,variable=self.startCapture,text="Start Capture", command = self.startCaptureClick)
		self.startCaptureCheck.pack()

		self.startGpsCheck = Checkbutton(self.indStart,variable=self.startGps,text="Start GPS", command = self.startGpsClick)
		self.startGpsCheck.pack()

		self.startNeutronsCheck = Checkbutton(self.indStart,variable=self.startNeutrons,text="Start Neutrons", command = self.startNeutronsClick)
		self.startNeutronsCheck.pack()

		self.indStartButton = Button(self.indStart, text="Select Start", activebackground="blue",activeforeground="white",relief=RAISED, command = self.indStartClick)
		self.indStartButton.pack()

		self.indStop = Label(self.mainControlGrid)
		self.indStop.grid(column=1,row=1)

		self.stopHyperSpecCheck = Checkbutton(self.indStop,variable=self.stopHyperSpec,text="Stop HyperSpec", command = self.stopHyperSpecClick)
		self.stopHyperSpecCheck.pack()

		self.stopCaptureCheck = Checkbutton(self.indStop,variable=self.stopCapture,text="Stop Capture", command = self.stopCaptureClick)
		self.stopCaptureCheck.pack()

		self.stopGpsCheck = Checkbutton(self.indStop,variable=self.stopGps,text="Stop GPS", command = self.stopGpsClick)
		self.stopGpsCheck.pack()

		self.stopNeutronsCheck = Checkbutton(self.indStop,variable=self.stopNeutrons,text="Stop Neutrons", command = self.stopNeutronsClick)
		self.stopNeutronsCheck.pack()		

		self.indStopButton = Button(self.indStop, text="Select Stop", activebackground="blue",activeforeground="white",relief=RAISED, command = self.indStopClick)
		self.indStopButton.pack()

		self.verificationLeds = Label(self)
		self.verificationLeds.grid(column=0,row=4)

		self.verificationLedsLeft = Label(self.verificationLeds)
		self.verificationLedsLeft.grid(column=0,row=0)

		
		#if Neutron HV is on self.verificationNeutronHvColor.set("Green")
		#else 
		#self.verificationNeutronHvColor.set("Red")
		#self.verificationNeutronHvButton = Button(self.verificationLedsLeft,text="Neutron HV",relief=RAISED,bg=self.verificationNeutronHvColor.get(),activebackground="blue",activeforeground="white", command = self.neutronHVClick)
		#self.verificationNeutronHvButton.pack()

		#if Dbus is setup self.verificationDbusColor.set("Green")
		#else 
		self.verificationDbus = Button(self.verificationLedsLeft,text="DBUS Setup",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.dBusClick)		
		self.verificationDbus.pack()

		#if Dbus Monitoring is on self.verificationDbusMonColor.set("Green")
		#else 
		self.verificationBusExtMon = Button(self.verificationLedsLeft,text="BusExtender Monitoring",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.busExtClick)
		self.verificationBusExtMon.pack()

		#if Start Misti has been started self.verificationStartMistiColor.set("Green")
		#else 
		self.verificationStartMisti = Button(self.verificationLedsLeft,text="Start MISTI",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.startMistiClick)
		self.verificationStartMisti.pack()

		#if Arduino Triggering is on self.verificationArduinoColor.set("Green")
		#else 
		self.verificationArduino = Button(self.verificationLedsLeft,text="Arduino Triggering",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.arduinoTriggeringClick)
		self.verificationArduino.pack()

		#if GPS is on self.verificationGpsColor.set("Green")
		#else 
		self.verificationGps = Button(self.verificationLedsLeft,text="NovAtel GPS",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.gpsClick)
		self.verificationGps.pack()

		self.verificationLedsRight = Label(self.verificationLeds)
		self.verificationLedsRight.grid(column=1,row=0)

		#if Gamma HV is on self.verificationGammaHvColor.set("Green")
		#else 
		self.verificationGammaButton = Button(self.verificationLedsRight,text="Gamma Ray",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.gammaClick)
		self.verificationGammaButton.pack()

		#if Neutrons is on self.verificationNeutronsColor.set("Green")
		#else 
		self.verificationNeutrons = Button(self.verificationLedsRight,text="Neutrons",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.neutronsClick)
		self.verificationNeutrons.pack()

		#if Lidar is on self.verificationLidarColor.set("Green")
		#else 
		self.verificationLidar = Button(self.verificationLedsRight,text="Lidar",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.lidarClick)
		self.verificationLidar.pack()

		#if Ladybug is on self.verificationLadybugColor.set("Green")
		#else 
		self.verificationLadybug = Button(self.verificationLedsRight,text="Ladybug",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.ladybugClick)
		self.verificationLadybug.pack()

		#if Hyper Spec is on self.verificationHyperSpecColor.set("Green")
		#else 
		self.verificationHyperSpec = Button(self.verificationLedsRight,text="HyperSpec",relief=RAISED,bg="Red",activebackground="blue",activeforeground="white", command = self.hyperSpecClick)
		self.verificationHyperSpec.pack()
		
		self.timestampedNotesGrid = Label(self,anchor="w")
		self.timestampedNotesGrid.grid(column=0,row=5)

		self.timestampedNotesLabel = Label(self.timestampedNotesGrid,text="Notes:")
		self.timestampedNotesLabel.grid(column=0,row=0)
		labelUnderlinedNotes = tkFont.Font(self.timestampedNotesLabel,self.timestampedNotesLabel.cget("font"))
		labelUnderlinedNotes.configure(underline = True)
		self.timestampedNotesLabel.configure(font=labelUnderlinedNotes)

		#self.timestampedNotes.set("Insert Notes Here")
		self.timestampedNotesEntry = Entry(self.timestampedNotesGrid, textvariable=self.timestampedNotes)
		self.timestampedNotesEntry.grid(column=1,row=0)
		self.timestampedNotesEntry.bind("<Return>", self.setTimestampedNotesEnter)
		self.timestampedNotesEntry.bind("<Button-1>", lambda number: self.entryClicked(2))
		self.timestampedNotesEntry.insert(0,"Insert Notes Here")

		self.timestampedNotesButton = Button(self.timestampedNotesGrid,text="Save Note",activebackground="blue",activeforeground="white",relief=RAISED, command = self.setTimestampedNotes)
		self.timestampedNotesButton.grid(column=2,row=0) 

		self.timestampedNotesDisplayScreen = Label(self.timestampedNotesGrid, anchor=S, textvariable=self.notesVar, bg="white", relief=RAISED, width = 36, height=15, wraplength=275)
		self.timestampedNotesDisplayScreen.grid(column=0,row=1,columnspan=3)

		self.timestampedNotesDisplayButton = Button(self.timestampedNotesGrid,text="Display Notes",activebackground="blue",activeforeground="white",relief=RAISED, command = self.timestampedNotesDisplay)
		self.timestampedNotesDisplayButton.grid(column=0,row=2, columnspan=3) 

		self.dataOffloadGrid = Label(self,anchor="w")
		self.dataOffloadGrid.grid(column=0,row=6)

		#dataOffloadLabel = Label(self.dataOffloadGrid,text="File Name:")
		#dataOffloadLabel.pack(side = LEFT)
		#labelUnderlinedNotes = tkFont.Font(dataOffloadLabel,dataOffloadLabel.cget("font"))
		#labelUnderlinedNotes.configure(underline = True)
		#dataOffloadLabel.configure(font=labelUnderlinedNotes)

		#self.timestampedNotes.set("Insert Notes Here")
		#self.dataOffloadEntry = Entry(self.dataOffloadGrid, textvariable=self.dataOffloadNotes)
		#self.dataOffloadEntry.pack(side = LEFT)
		#self.dataOffloadEntry.bind("<Return>", self.dataOffloadEnter)
		#self.dataOffloadEntry.bind("<Button-1>", lambda number: self.entryClicked(2))
		#self.dataOffloadEntry.insert(0,"Save File Name")

		self.dataOffloadButton = Button(self.dataOffloadGrid,text="Save Files",activebackground="blue",activeforeground="white",relief=RAISED, command = self.saveData)
		self.dataOffloadButton.pack(side = LEFT)	
		
		#self.closeWindowButton = Button(self.dataOffloadGrid, text="Close Window", activebackground="blue",activeforeground="white",relief=RAISED, command = self.closeWindow)
		#self.closeWindowButton.pack(side = LEFT)

		self.resetButton = Button(self.dataOffloadGrid, text="New Session",activebackground="blue",activeforeground="white",relief=RAISED, command = self.resetClicked)
		self.resetButton.pack()


	def entryClicked(self, number):
		if self.clickedOperator == False:		#Clicked can be thought of as saved. savedOpterator
			if number == 1:
				self.entryOperator.delete(0, END)
				self.clickedOperator = True
		if self.clickedNotes == False:
			if number == 2:
				self.timestampedNotesEntry.delete(0,END)
				self.clickedNotes = True
	def setOperatorClick(self):
		if self.setOperatorCounter == 0:
			self.entryOperator.config(state=DISABLED)
			self.setOperatorCounter = 1
		elif self.setOperatorCounter == 1:
			self.entryOperator.config(state=NORMAL)
			self.setOperatorCounter = 0

	def setOperatorEnter(self,event):
		if self.setOperatorCounter == 0:
			self.entryOperator.config(state=DISABLED)
			self.setOperatorCounter = 1
		elif self.setOperatorCounter == 1:
			self.entryOperator.config(state=NORMAL)
			self.setOperatorCounter = 0

	def tick(self):										#Time Since Method
		if self.systemClicked.get() == 1:
			self.currentTime = time.time()
			self.elapsedTime.set(round((self.currentTime - self.startTime),2))
			self.currentClock = datetime.now()
			clock2 = self.currentClock.strftime('%H:%M:%S')
			if clock2 != self.clock.get():
				self.clock.set(clock2)
			self.displayClock.after(200, app.tick)
		elif self.systemClicked.get() == 2:
			self.elapsedTimeGrid.grid_forget()
			elapsedTimeGrid = Label(self)
			elapsedTimeGrid.grid(column=0,row=1)
			self.displayElapsedTimeLabel = Label(elapsedTimeGrid, text="Time Since Start of Session:")
			self.displayElapsedTimeLabel.grid(column=0,row=0)
			labelUnderlined = tkFont.Font(self.displayElapsedTimeLabel,self.displayElapsedTimeLabel.cget("font"))
			labelUnderlined.configure(underline = True)
			self.displayElapsedTimeLabel.configure(font=labelUnderlined)
			self.displayElapsedTime = Label(elapsedTimeGrid, textvariable=self.elapsedTime,fg="white",bg="blue",width=4)
			self.displayElapsedTime.grid(column=1,row=0)
			self.displayClock = Label(elapsedTimeGrid, textvariable=self.clock, anchor="e", bg="orange",fg="black",width=7)
			self.displayClock.grid(column=2,row=0)
	

	def verificationCompleted(self):
		"""
		For Joey's amusement :P This verifies that all the scripts have terminated properly
		"""
		if self.troubleshooting:
			print "Verifying sensors have terminated acquisition"
		if self.hyperSpecStopped:
			hsVerificationSocket.send("Finished?")
			hsCompletedMess = hsVerificationSocket.recv()
			if hsCompletedMess == "Closed":
				self.verificationHyperSpec.configure(bg="Purple")
				self.displayVar.set("HyperSpec Acquisition:%s" %hsCompletedMess)
				if self.troubleshooting:
					print "HyperSpec Acquisition:%s" %hsCompletedMess
				self.verificationLeds.update()
			else:
				self.displayVar.set("HyperSpec Stop Unsuccessful: Message = %s" %hsCompletedMess)
				if self.troubleshooting:
					print "HyperSpec Stop Unsuccessful: Message = %s" %hsCompletedMess
				self.verificationHyperSpec.configure(bg="Pink")
				self.verificationLeds.update()
		#if self.startCaptureStopped:
		#	lidarVerificationSocket.send("Finished?")
		#	lidarCompletedMess = lidarVerificationSocket.recv()
		#	if lidarVerfMess == "Closed":	
		#		self.verificationLidar.configure(bg="Purple")
		#		self.displayVar.set("Lidar Acquisition Closed")		
		#		self.verificationLeds.update()
		#	else:
		#		self.displayVar.set("Lidar Stop Unsuccessful")
		#		self.verificationLidar.configure(bg="Pink")
		#		self.verificationLeds.update()
		#	ladbyVerificationSocket.send("Finished?")
		#	ladyCompletedMess = ladybugVerificationSocket.recv()	
		#	if ladybugCompletedMess == "Closed":
		#			self.verificationLadybug.configure(bg="Purple")	
		#			self.displayVar.set("Ladybug Acquisition Closed")
		#			self.verificationLeds.update()
		#	else:
		#		self.displayVar.set("Stop Unsuccessful")
		#		self.verificationLadybug.configure(bg="Pink")
		#		self.verificationLeds.update()
		#	arduinoVerificationSocket.send("Finished?")
		#	arduinoCompletedMess = arduinoVerificatinSocket.recv()
		#	if arduinoCompletedMess == "Closed":
		#		self.verificationArduino.configure(bg="Purple")	
		#		self.displayVar.set("Arduino Closed Successfully")
		#		self.verificationLeds.update()	
		#	else:
		#		self.displayVar.set("Stop Unsuccessful")
		#		self.verificationArduino.configure(bg="Pink")
		#		self.verificationLeds.update()
		if self.gpsStopped:
			gpsVerificationSocket.send("Finished?")
			gpsCompletedMess = gpsVerificationSocket.recv()
			if gpsCompletedMess == "Closed":	
				self.verificationGps.configure(bg="Purple")
				self.displayVar.set("GPS Acquisition Closed")				
				self.verificationLeds.update()
			else:
				self.displayVar.set("GPS Stop Unsuccessful")
				self.verificationGps.configure(bg="Pink")
				self.verificationLeds.update()
		if self.neutronsStopped:
			liqVerificationSocket.send("Finished?")
			liqCompletedMess = liqVerificationSocket.recv()
			if liqCompletedMess == "Closed":
				self.verificationNeutrons.configure(bg="Purple")
				self.displayVar.set("Neutron Acquisition Closed")
				self.verificationLeds.update()
			else:
				self.displayVar.set("Neutron Stop Unsuccessful")
				self.verificationNeutrons.configure(bg="Pink")
				self.verificationLeds.update()
				
	def verificationMonitor(self):
		if self.acqStarted and not self.stopSelected:
			if self.troubleshooting:
				print "Verification Monitor Called!"
			if self.hyperSpecStarted:
				hsVerificationSocket.send("Status?")
				hsVerfMess = hsVerificationSocket.recv()
				if self.troubleshooting:
					time2 = time.time()					
					print "%s ____ %s" %(hsVerfMess, time2)
				if not self.stopSelected:
					if hsVerfMess == "True":
						self.verificationHyperSpec.configure(bg="Green")
					else:
						self.verificationHyperSpec.configure(bg="Red")
		#	if self.startCaptureStarted:
		#		lidarVerificationSocket.send("Status?")
		#		lidarVerfMess = lidarVerificationSocket.recv()
		#		if self.troubleshooting:
		#			time2 = time.time()					
		#			print "%s ____ %s" %(lidarVerfMess, time2)
		#		if not self.stopSelected:
		#			if lidarVerfMess == "True":
		#				self.verificationLidar.configure(bg="Green")
		#			else:
		#				self.verificationLidar.configure(bg="Red")
		#
		#		ladybugVerificationSocket.send("Status?")
		#		ladybugVerfMess = ladybugVerificationSocket.recv()
		#		if self.troubleshooting:
		#			time2 = time.time()					
		#			print "%s ____ %s" %(ladybugVerfMess, time2)
		#		if not self.stopSelected:
		#			if ladybugVerfMess == "True":
		#				self.verificationLadybug.configure(bg="Green")
		#			else:
		#				self.verificationLadybug.configure(bg="Red")
		#		else:
		#			self.displayVar.set("Stop Unsuccessful")
		#			self.verificationLadybug.configure(bg="Pink")
		#			self.verificationLeds.update()
		#
		#		arduinoVerificationSocket.send("Status?")
		#		arduinoVerfMess = arduinoVerificationSocket.recv()
		#		if self.troubleshooting:
		#			time2 = time.time()					
		#			print "%s ____ %s" %(arduinoVerfMess, time2)
		#		if not self.stopSelected:
		#			if arduinoVerfMess == "True":
		#				self.verificationArduino.configure(bg="Green")
		#			else:
		#				self.verificationArduino.configure(bg="Red")
			if self.gpsStarted:
				gpsVerificationSocket.send("Status?")
				gpsVerfMess = gpsVerificationSocket.recv()
				if self.troubleshooting:
					time2 = time.time()					
					print "%s ____ %s" %(gpsVerfMess, time2)
				if not self.stopSelected:
					if gpsVerfMess == "True":
						self.verificationGps.configure(bg="Green")
					else:
						self.verificationGps.configure(bg="Red")
			if self.neutronsStarted:
				liqVerificationSocket.send("Status?")
				liqVerfMess = liqVerificationSocket.recv()
				if self.troubleshooting:
					time2 = time.time()					
					print "%s ____ %s" %(liqVerfMess, time2)
				if not self.stopSelected:
					if liqVerfMess == "True":
						self.verificationNeutrons.configure(bg="Green")
					else:
						self.verificationNeutrons.configure(bg="Red")
		if not self.stopSelected:
			self.after(5000,self.verificationMonitor)					

	def mainStartClick(self):
		if self.systemClicked.get() == 0:
			self.currentClock = datetime.now()
			if self.operator.get() == "Enter Operator Name Here":
					self.noOperator()
			else:
				#if self.noteSaved == False:
				if self.fileOpened == False:
					operatorName = self.operator.get()
					operatorName = operatorName.translate(None,whitespace)
					self.fileName = '/home/mistidb/misti/Documents/RadMAP Runs/%s%s' %(operatorName,self.currentClock.strftime('%m%d%y'))
					if glob.glob("%s.txt" %self.fileName): 
						if glob.glob(('%sv*.txt' %self.fileName)):
							fileList = glob.glob(('%sv*.txt' %self.fileName))
							fileListLength = len(fileList)
							self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Runs')
							self.fileName = ('%s/%s%sv.%s.txt' %(self.fileName[0],operatorName, self.currentClock.strftime('%m%d%y'),(fileListLength+2)))
						else:
							self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Runs')
							self.fileName = '%s/%s%sv.2.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
						self.f = open("%s" %self.fileName, "w")
						self.fileOpened = True
						self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
						self.f = open('%s' %self.fileName)
						self.notesVar.set(self.f.read())
						self.f.close()
					else:
						#self.fileName = glob.glob('/*/Documents/RadMAP Runs')
						self.fileName = '%s.txt' %self.fileName
						self.f = open("%s" %self.fileName, "w")
						self.fileOpened = True
						self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
						self.f = open('%s' %self.fileName)
						self.notesVar.set(self.f.read())
						self.f.close()
				else:
					self.f = open("%s" %self.fileName, "a")
					self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
					self.f = open('%s' %self.fileName)
					self.notesVar.set(self.f.read())
					self.f.close()

				self.systemClicked.set(1)
				self.startTime = time.time()	
				self.elapsedTimeGrid.grid_forget()
				
				self.elapsedTimeGrid = Label(self)
				self.elapsedTimeGrid.grid(column=0,row=1)
				self.displayElapsedTimeLabel = Label(self.elapsedTimeGrid, text="Time Since Start of Session:")
				self.displayElapsedTimeLabel.grid(column=0,row=0)
				
				labelUnderlined = tkFont.Font(self.displayElapsedTimeLabel,self.displayElapsedTimeLabel.cget("font"))
				labelUnderlined.configure(underline = True)
				self.displayElapsedTimeLabel.configure(font=labelUnderlined)
				self.displayElapsedTime = Label(self.elapsedTimeGrid, textvariable=self.elapsedTime,width=4)
				self.displayElapsedTime.grid(column=1,row=0)
				
				self.displayClock = Label(self.elapsedTimeGrid, textvariable=self.clock, anchor="e", bg="gray",width=7)
				self.displayClock.grid(column=2,row=0)
				
				self.tick()
				self.startList = ["STA", "startHyperSpec", "startCapture", "startGPS", "startNeutrons"]
				for command in self.startList:
					print command
					if command == "startHyperSpec":
						interfaceSocketHs.send('startHyperSpec')
						print "Sent Start HyperSpec"
						self.displayVar.set("Start HyperSpec Sent")
						self.communicationDisplay.update()
						hsMessage = interfaceSocketHs.recv()						print "%s" % hsMessage
						self.verificationHyperSpec.configure(bg="Orange")
						self.displayVar.set(hsMessage)
						self.communicationDisplay.update()
						interfaceSocketHs.send("Started?")
						hsStartMess = interfaceSocketHs.recv()
						print hsStartMess
						if hsStartMess == "HyperSpec Acquisition Starting":
							self.verificationHyperSpec.configure(bg="Yellow")
							self.displayVar.set(hsStartMess)
							self.communicationDisplay.update()
							self.hyperSpecStarted = 1
					if command == "startCapture":
						interfaceSocketCapture.send('startCapture')
						print "Sent StartCapture"
						self.displayVar.set("Sent StartCapture")
						self.communicationDisplay.update()
						captureMessage = interfaceSocketCapture.recv()
						print "%s" % captureMessage
						self.verificationArduino.configure(bg = "Orange")
						self.verificationLidar.configure(bg="Orange")
						self.verificationLadybug.configure(bg="Orange")
						self.displayVar.set(captureMessage)
						self.communicationDisplay.update()
						interfaceSocketCapture.send("Started?")
						captureStartMess = interfaceSocketCapture.recv()
						print captureStartMess
						if captureStartMess == "startCapture Acquisition Starting":
							self.verificationArduino.configure(bg = "Yellow")
							self.verificationLidar.configure(bg="Yellow")
							self.verificationLadybug.configure(bg="Yellow")
							self.displayVar.set(captureStartMess)
							self.communicationDisplay.update()
							self.startCaptureStarted = 1
					if command == "startGPS":
						interfaceSocketGps.send('startGPS')
						print "Sent Start GPS"
						self.displayVar.set("Sent Start GPS")
						self.communicationDisplay.update()
						gpsMessage = interfaceSocketGps.recv()
						print "%s" % gpsMessage
						self.verificationGps.configure(bg="Orange")
						self.displayVar.set(gpsMessage)
						self.communicationDisplay.update()
						interfaceSocketGps.send("Started?")
						gpsStartMess = interfaceSocketGps.recv()
						print gpsStartMess
						if gpsStartMess == "GPS Acquisition Starting":
							self.verificationGps.configure(bg="Yellow")
							self.displayVar.set(gpsStartMess)
							self.communicationDisplay.update()
							self.gpsStarted = 1
					if command == "startNeutrons":
						interfaceSocketLiq.send('startNeutrons')
						print "Sent Start Neutrons"
						self.displayVar.set("Sent Start Neutrons")
						self.communicationDisplay.update()
						liqMessage = interfaceSocketLiq.recv()
						print "%s" % liqMessage
						self.verificationNeutrons.configure(bg="Orange")
						self.displayVar.set(liqMessage)
						self.communicationDisplay.update()
						interfaceSocketLiq.send("Started?")
						liqStartMess = interfaceSocketLiq.recv()
						print liqStartMess
						if liqStartMess == "Neutron Acquisition Starting":
							self.verificationNeutrons.configure(bg="Yellow")
							self.displayVar.set(liqStartMess)
							self.communicationDisplay.update()
							self.neutronsStarted = 1
				self.acqStarted = 1


	def indStartClick(self):
		if self.systemClicked.get() == 0 and self.startList != []:
			self.currentClock = datetime.now()
			if self.operator.get() == "Enter Operator Name Here":
					self.noOperator()
			else:
				#if self.noteSaved == False:
				if self.fileOpened == False:
					operatorName = self.operator.get()
					operatorName = operatorName.translate(None,whitespace)
					self.fileName = '/home/mistidb/misti/Documents/RadMAP Runs/%s%s' %(operatorName,self.currentClock.strftime('%m%d%y'))
					if glob.glob("%s.txt" %self.fileName): 
						if glob.glob(('%sv*.txt' %self.fileName)):
							fileList = glob.glob(('%sv*.txt' %self.fileName))
							fileListLength = len(fileList)
							self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Runs')
							self.fileName = ('%s/%s%sv.%s.txt' %(self.fileName[0],operatorName, self.currentClock.strftime('%m%d%y'),(fileListLength+2)))
						else:
							self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Runs')
							self.fileName = '%s/%s%sv.2.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
						self.f = open("%s" %self.fileName, "w")
						self.fileOpened = True
						self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
						self.f = open('%s' %self.fileName)
						self.notesVar.set(self.f.read())
						self.f.close()
					else:
						#self.fileName = glob.glob('/*/*/Documents/RadMAP Runs')
						self.fileName = '%s.txt' %self.fileName
						self.f = open("%s" %self.fileName, "w")
						self.fileOpened = True
						self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
						self.f = open('%s' %self.fileName)
						self.notesVar.set(self.f.read())
						self.f.close()
						

				else:
					self.f = open("%s" %self.fileName, "a")
					self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
					self.f = open('%s' %self.fileName)
					self.notesVar.set(self.f.read())
					self.f.close()

				self.systemClicked.set(1)
				self.startTime = time.time()
				self.elapsedTimeGrid.grid_forget()
				self.elapsedTimeGrid = Label(self)

				self.elapsedTimeGrid.grid(column=0,row=1)
				self.displayElapsedTimeLabel = Label(self.elapsedTimeGrid, text="Time Since Start of Session:")
				self.displayElapsedTimeLabel.grid(column=0,row=0)
				
				labelUnderlined = tkFont.Font(self.displayElapsedTimeLabel,self.displayElapsedTimeLabel.cget("font"))
				labelUnderlined.configure(underline = True)
				self.displayElapsedTimeLabel.configure(font=labelUnderlined)
				self.displayElapsedTime = Label(self.elapsedTimeGrid, textvariable=self.elapsedTime,width=4)
				self.displayElapsedTime.grid(column=1,row=0)
				
				self.displayClock = Label(self.elapsedTimeGrid, textvariable=self.clock, anchor="e", bg="gray",width=7)
				self.displayClock.grid(column=2,row=0)
					
				self.tick()
				self.startList.insert(0, "STA")
				for command in self.startList:
					if "startHyperSpec" not in self.startList:
						self.verificationHyperSpec.configure(bg="Blue")
						self.verificationLeds.update()
					if "startCapture" not in self.startList:
						self.verificationArduino.configure(bg = "Blue")
						self.verificationLidar.configure(bg="Blue")
						self.verificationLadybug.configure(bg="Blue")
						self.verificationLeds.update()
					if "startGPS" not in self.startList:
						self.verificationGps.configure(bg="Blue")
					if "startNeutrons" not in self.startList:
						self.verificationNeutrons.configure(bg="Blue")
					if command == "startHyperSpec":
						if self.startCaptureFlag == 1:
							if self.hyperSpecStarted == 0:
								interfaceSocketHs.send('startHyperSpec')
								print "Sent Start HyperSpec"
								self.displayVar.set("Start HyperSpec Sent")
								self.communicationDisplay.update()
								hsMessage = interfaceSocketHs.recv()								print "%s" % hsMessage
								self.verificationHyperSpec.configure(bg="Orange")
								self.displayVar.set(hsMessage)
								self.communicationDisplay.update()
								interfaceSocketHs.send("Started?")
								hsStartMess = interfaceSocketHs.recv()
								print hsStartMess
								if hsStartMess == "HyperSpec Acquisition Starting":
									self.verificationHyperSpec.configure(bg="Yellow")
									self.displayVar.set(hsStartMess)
									self.communicationDisplay.update()
									self.hyperSpecStarted = 1
						else:
							print "HyperSpec cannot be started without StartCapture"
							self.displayVar.set("HyperSpec cannot be started without StartCapture")
							self.communicationDisplay.update()
					if command == "startCapture":
						if self.startHyperSpecFlag == 1:
							if self.hyperSpecStarted == 1:
								interfaceSocketCapture.send('startCapture')
								print "Sent StartCapture"
								self.displayVar.set("Sent StartCapture")
								self.communicationDisplay.update()
								captureMessage = interfaceSocketCapture.recv()
								print "%s" % captureMessage
								self.verificationArduino.configure(bg = "Orange")
								self.verificationLidar.configure(bg="Orange")
								self.verificationLadybug.configure(bg="Orange")
								self.displayVar.set(captureMessage)
								self.communicationDisplay.update()
								interfaceSocketCapture.send("Started?")
								captureStartMess = interfaceSocketCapture.recv()
								print captureStartMess
								if captureStartMess == "startCapture Acquisition Starting":
									self.verificationArduino.configure(bg = "Yellow")
									self.verificationLidar.configure(bg="Yellow")
									self.verificationLadybug.configure(bg="Yellow")
									self.displayVar.set(captureStartMess)
									self.communicationDisplay.update()
									self.startCaptureStarted = 1
							else:
								interfaceSocketHs.send('startHyperSpec')
								print "Sent Start HyperSpec"
								self.displayVar.set("Start HyperSpec Sent")
								self.communicationDisplay.update()
								hsMessage = interfaceSocketHs.recv()								print "%s" % hsMessage
								self.verificationHyperSpec.configure(bg="Orange")
								self.displayVar.set(hsMessage)
								self.communicationDisplay.update()
								interfaceSocketHs.send("Started?")
								hsStartMess = interfaceSocketHs.recv()
								print hsStartMess
								if hsStartMess == "HyperSpec Acquisition Starting":
									self.verificationHyperSpec.configure(bg="Yellow")
									self.displayVar.set(hsStartMess)
									self.communicationDisplay.update()
									self.hyperSpecStarted = 1
								interfaceSocketCapture.send('startCapture')
								print "Sent StartCapture"
								self.displayVar.set("Sent StartCapture")
								self.communicationDisplay.update()
								captureMessage = interfaceSocketCapture.recv()
								print "%s" % captureMessage
								self.verificationArduino.configure(bg = "Orange")
								self.verificationLidar.configure(bg="Orange")
								self.verificationLadybug.configure(bg="Orange")
								self.displayVar.set(captureMessage)
								self.communicationDisplay.update()
								interfaceSocketCapture.send("Started?")
								captureStartMess = interfaceSocketCapture.recv()
								print captureStartMess
								if captureStartMess == "startCapture Acquisition Starting":
									self.verificationArduino.configure(bg = "Yellow")
									self.verificationLidar.configure(bg="Yellow")
									self.verificationLadybug.configure(bg="Yellow")
									self.displayVar.set(captureStartMess)
									self.communicationDisplay.update()
									self.startCaptureStarted = 1
						else:
							interfaceSocketCapture.send('startCapture')
							print "Sent StartCapture"
							self.displayVar.set("Sent StartCapture")
							self.communicationDisplay.update()
							captureMessage = interfaceSocketCapture.recv()
							print "%s" % captureMessage
							self.verificationArduino.configure(bg = "Orange")
							self.verificationLidar.configure(bg="Orange")
							self.verificationLadybug.configure(bg="Orange")
							self.displayVar.set(captureMessage)
							self.communicationDisplay.update()
							interfaceSocketCapture.send("Started?")
							captureStartMess = interfaceSocketCapture.recv()
							print captureStartMess
							if captureStartMess == "startCapture Acquisition Starting":
								self.verificationArduino.configure(bg = "Yellow")
								self.verificationLidar.configure(bg="Yellow")
								self.verificationLadybug.configure(bg="Yellow")
								self.displayVar.set(captureStartMess)
								self.communicationDisplay.update()
								self.startCaptureStarted = 1							
					if command == "startGPS":
						if self.startCaptureFlag == 1:
							if self.startCaptureStarted:
								interfaceSocketGps.send('startGPS')
								print "Sent Start GPS"
								self.displayVar.set("Sent Start GPS")
								self.communicationDisplay.update()
								gpsMessage = interfaceSocketGps.recv()
								print "%s" % gpsMessage
								self.verificationGps.configure(bg="Orange")
								self.displayVar.set(gpsMessage)
								self.communicationDisplay.update()
								interfaceSocketGps.send("Started?")
								gpsStartMess = interfaceSocketGps.recv()
								print gpsStartMess
								if gpsStartMess == "GPS Acquisition Starting":
									self.verificationGps.configure(bg="Yellow")
									self.displayVar.set(gpsStartMess)
									self.communicationDisplay.update()
									self.gpsStarted = 1
							else:
								if self.startHyperSpecFlag:
									if not self.hyperSpecStarted:
										interfaceSocketHs.send('startHyperSpec')
										print "Sent Start HyperSpec"
										self.displayVar.set("Start HyperSpec Sent")
										self.communicationDisplay.update()
										hsMessage = interfaceSocketHs.recv()										print "%s" % hsMessage
										self.verificationHyperSpec.configure(bg="Orange")
										self.displayVar.set(hsMessage)
										self.communicationDisplay.update()
										interfaceSocketHs.send("Started?")
										hsStartMess = interfaceSocketHs.recv()
										print hsStartMess
										if hsStartMess == "HyperSpec Acquisition Starting":
											self.verificationHyperSpec.configure(bg="Yellow")
											self.displayVar.set(hsStartMess)
											self.communicationDisplay.update()
											self.hyperSpecStarted = 1
										interfaceSocketCapture.send('startCapture')
										print "Sent StartCapture"
										self.displayVar.set("Sent StartCapture")
										self.communicationDisplay.update()
										captureMessage = interfaceSocketCapture.recv()
										print "%s" % captureMessage
										self.verificationArduino.configure(bg = "Orange")
										self.verificationLidar.configure(bg="Orange")
										self.verificationLadybug.configure(bg="Orange")
										self.displayVar.set(captureMessage)
										self.communicationDisplay.update()
										interfaceSocketCapture.send("Started?")
										captureStartMess = interfaceSocketCapture.recv()
										print captureStartMess
										if captureStartMess == "startCapture Acquisition Starting":
											self.verificationArduino.configure(bg = "Yellow")
											self.verificationLidar.configure(bg="Yellow")
											self.verificationLadybug.configure(bg="Yellow")
											self.displayVar.set(captureStartMess)
											self.communicationDisplay.update()
											self.startCaptureStarted = 1	
										interfaceSocketGps.send('startGPS')
										print "Sent Start GPS"
										self.displayVar.set("Sent Start GPS")
										self.communicationDisplay.update()
										gpsMessage = interfaceSocketGps.recv()
										print "%s" % gpsMessage
										self.verificationGps.configure(bg="Orange")
										self.displayVar.set(gpsMessage)
										self.communicationDisplay.update()
										interfaceSocketGps.send("Started?")
										gpsStartMess = interfaceSocketGps.recv()
										print gpsStartMess
										if gpsStartMess == "GPS Acquisition Starting":
											self.verificationGps.configure(bg="Yellow")
											self.displayVar.set(gpsStartMess)
											self.communicationDisplay.update()
											self.gpsStarted = 1
								else:
									interfaceSocketCapture.send('startCapture')
									print "Sent StartCapture"
									self.displayVar.set("Sent StartCapture")
									self.communicationDisplay.update()
									captureMessage = interfaceSocketCapture.recv()
									print "%s" % captureMessage
									self.verificationArduino.configure(bg = "Orange")
									self.verificationLidar.configure(bg="Orange")
									self.verificationLadybug.configure(bg="Orange")
									self.displayVar.set(captureMessage)
									self.communicationDisplay.update()
									interfaceSocketCapture.send("Started?")
									captureStartMess = interfaceSocketCapture.recv()
									print captureStartMess
									if captureStartMess == "startCapture Acquisition Starting":
										self.verificationArduino.configure(bg = "Yellow")
										self.verificationLidar.configure(bg="Yellow")
										self.verificationLadybug.configure(bg="Yellow")
										self.displayVar.set(captureStartMess)
										self.communicationDisplay.update()
										self.startCaptureStarted = 1	
									interfaceSocketGps.send('startGPS')
									print "Sent Start GPS"
									self.displayVar.set("Sent Start GPS")
									self.communicationDisplay.update()
									gpsMessage = interfaceSocketGps.recv()
									print "%s" % gpsMessage
									self.verificationGps.configure(bg="Orange")
									self.displayVar.set(gpsMessage)
									self.communicationDisplay.update()
									interfaceSocketGps.send("Started?")
									gpsStartMess = interfaceSocketGps.recv()
									print gpsStartMess
									if gpsStartMess == "GPS Acquisition Starting":
										self.verificationGps.configure(bg="Yellow")
										self.displayVar.set(gpsStartMess)
										self.communicationDisplay.update()
										self.gpsStarted = 1	
						else:
							print "GPS cannot be started without StartCapture"
							self.displayVar.set("GPS cannot be started without StartCapture")
							self.communicationDisplay.update()
					if command == "startNeutrons":
						interfaceSocketLiq.send('startNeutrons')
						print "Sent Start Neutrons"
						self.displayVar.set("Sent Start Neutrons")
						self.communicationDisplay.update()
						liqMessage = interfaceSocketLiq.recv()
						print "%s" % liqMessage
						self.verificationNeutrons.configure(bg="Orange")
						self.displayVar.set(liqMessage)
						self.communicationDisplay.update()
						interfaceSocketLiq.send("Started?")
						liqStartMess = interfaceSocketLiq.recv()
						print liqStartMess
						if liqStartMess == "Neutron Acquisition Starting":
							self.verificationNeutrons.configure(bg="Yellow")
							self.displayVar.set(liqStartMess)
							self.communicationDisplay.update()
							self.neutronsStarted = 1
				self.acqStarted = 1
				self.startCaptureCheck.config(state=DISABLED)
				self.startGpsCheck.config(state=DISABLED)
				self.startNeutronsCheck.config(state=DISABLED)
				self.startHyperSpecCheck.config(state=DISABLED)


	def startCaptureClick(self):
		if self.startCapture.get() == 1:
			self.startList.append("startCapture")
			self.startCaptureFlag = 1
		elif self.startCapture.get() == 0:
			self.startList.remove("startCapture")
			self.startCaptureFlag = 0


	def startGpsClick(self):
		if self.startGps.get() == 1:
			self.startList.append("startGPS")
			self.startGpsFlag = 1
		elif self.startGps.get() == 0:
			self.startList.remove("startGPS")
			self.startGpsFlag = 0

	def startNeutronsClick(self):
		if self.startNeutrons.get() == 1:
			self.startList.append("startNeutrons")
			self.startNeutronsFlag = 1
		elif self.startNeutrons.get() == 0:
			self.startList.remove("startNeutrons")
			self.startNeutronsFlag = 0

	def startHyperSpecClick(self):
		if self.startHyperSpec.get() == 1:
			self.startList.append("startHyperSpec")
			self.startHyperSpecFlag = 1
		elif self.startHyperSpec.get() == 0:
			self.startList.remove("startHyperSpec")
			self.startHyperSpecFlag = 0
		

	def mainStopClick(self):
		self.stopSelected = 1
		if self.systemClicked.get() == 1:
			
			self.f = open("%s" %self.fileName, "a")
			self.f.write("%s: Session Ended \n" %self.currentClock.strftime('%H:%M:%S'))
			self.f = open('%s' %self.fileName)
			self.notesVar.set(self.f.read())
			self.f.close()

			self.systemClicked.set(2)
			self.stopTime = time.time()
			self.tick()
			if self.fileOpened == True:			#File should already be closed. Thinking unneccesary
				self.f.close()
			self.stopList = ["STO", "stopHyperSpec", "stopCapture", "stopGPS", "stopNeutrons"]

			for command in self.stopList:
				if command == "stopHyperSpec":
					interfaceSocketHs.send('stopHyperSpec')
					print "Sent Stop HyperSpec"
					self.displayVar.set("Sent Stop HyperSpec")	
					self.communicationDisplay.update()	
					hsMessage = interfaceSocketHs.recv()
					print "%s" % hsMessage
					self.verificationHyperSpec.configure(bg="Orange")
					self.displayVar.set(hsMessage)
					self.communicationDisplay.update()
					time.sleep(4)
					self.hyperSpecStopped = 1
					self.verificationCompleted()
				if command == "stopCapture":
					bugSocket.send('stopCapture')
					print "Sent StopCapture"
					self.displayVar.set("Sent StopCapture")
					self.communicationDisplay.update()
					bugMessage = interfaceSocketCapture.recv()
					print "%s" % bugMessage
					self.verificationArduino.configure(bg = "Orange")
					self.verificationLidar.configure(bg="Orange")
					self.verificationLadybug.configure(bg="Orange")
					self.displayVar.set(bugMessage)
					self.communicationDisplay.update()
					time.sleep(10)
					self.startCaptureStopped = 1
					self.verificationCompleted()
				if command == "stopGPS":
					interfaceSocketGps.send('stopGPS')
					print "Sent Stop GPS"
					self.displayVar.set("Sent Stop GPS")
					self.communicationDisplay.update()
					bugMessage = interfaceSocketGps.recv()
					print "%s" % bugMessage
					self.verificationGps.configure(bg="Orange")
					self.displayVar.set(bugMessage)
					self.communicationDisplay.update()
					time.sleep(6)
					self.gpsStopped = 1
					self.verificationCompleted()
				if command == "stopNeutrons":
					interfaceSocketLiq.send('stopNeutrons')
					print "Sent Stop Neutrons"
					self.displayVar.set("Sent Stop Neutrons")
					self.communicationDisplay.update()
					liqMessage = interfaceSocketLiq.recv()
					print "%s" % liqMessage
					self.verificationNeutrons.configure(bg="Orange")
					self.displayVar.set(liqMessage)
					self.communicationDisplay.update()
					time.sleep(90)
					self.neutronsStopped = 1
					self.verificationCompleted()
	
	def indStopClick(self):
		self.stopSelected = 1
		if self.systemClicked.get() == 1 and self.stopList != []:

			self.f = open("%s" %self.fileName, "a")
			self.f.write("%s: Session Ended \n" %self.currentClock.strftime('%H:%M:%S'))
			self.f = open('%s' %self.fileName)
			self.notesVar.set(self.f.read())
			self.f.close()

			self.systemClicked.set(2)
			self.stopTime = time.time()
			self.tick()
			if self.fileOpened == True:			#File should already be closed. Thinking unneccesary
				self.f.close()
			self.stopList.insert(0, "STO")
			
			for command in self.stopList:
				if command == "stopHyperSpec":
					if self.startHyperSpecFlag:
						interfaceSocketHs.send('stopHyperSpec')
						print "Sent Stop HyperSpec"
						self.displayVar.set("Sent Stop HyperSpec")
						self.communicationDisplay.update()
						hsMessage = interfaceSocketHs.recv()
						print "%s" % hsMessage
						self.verificationHyperSpec.configure(bg="Orange")
						self.displayVar.set(hsMessage)
						self.communicationDisplay.update()
						time.sleep(4)
						self.hyperSpecStopped = 1
						self.verificationCompleted()
				if command == "stopCapture":
					if self.startCaptureFlag:
						interfaceSocketCapture.send('stopCapture')
						print "Sent StopCapture"
						self.displayVar.set("Sent StopCapture")
						self.communicationDisplay.update()
						bugMessage = interfaceSocketCapture.recv()
						print "%s" % bugMessage
						self.verificationArduino.configure(bg = "Orange")
						self.verificationLidar.configure(bg="Orange")
						self.verificationLadybug.configure(bg="Orange")
						self.displayVar.set(bugMessage)
						self.communicationDisplay.update()
						time.sleep(10)
						self.startCaptureStopped = 1
						self.verificationCompleted()
				if command == "stopGPS":
					if self.startGpsFlag:
						interfaceSocketGps.send('stopGPS')
						print "Sent Stop GPS"
						self.displayVar.set("Sent Stop GPS")
						self.communicationDisplay.update()
						bugMessage = interfaceSocketGps.recv()
						print "%s" % bugMessage
						self.verificationGps.configure(bg="Orange")
						self.displayVar.set(bugMessage)
						self.communicationDisplay.update()
						time.sleep(6)
						self.gpsStopped = 1
						self.verificationCompleted()
				if command == "stopNeutrons":
					if self.startNeutronsFlag:
						interfaceSocketLiq.send('stopNeutrons')
						print "Sent Stop Neutrons"
						self.displayVar.set("Sent Stop Neutrons")
						self.communicationDisplay.update()
						liqMessage = interfaceSocketLiq.recv()
						print "%s" % liqMessage
						self.verificationNeutrons.configure(bg="Orange")
						self.displayVar.set("Neutron Stop Received, Ramping Down High Voltage")
						self.communicationDisplay.update()
						time.sleep(90)
						self.neutronsStopped = 1
						self.verificationCompleted()

	def stopCaptureClick(self):
		if self.stopCapture.get() == 1:
			self.stopList.append("stopCapture")
		elif self.stopCapture.get() == 0:
			self.stopList.remove("stopCapture")

	def stopWeatherClick(self):
		if self.stopWeather.get() == 1:
			self.stopList.append("stopWeather")
		elif self.stopWeather.get() == 0:
			self.stopList.remove("stopWeather")

	def stopGpsClick(self):
		if self.stopGps.get() == 1:
			self.stopList.append("stopGPS")
		elif self.stopGps.get() == 0:
			self.stopList.remove("stopGPS")

	def stopNeutronsClick(self):
		if self.stopNeutrons.get() == 1:
			self.stopList.append("stopNeutrons")
		elif self.stopNeutrons.get() == 0:
			self.stopList.remove("stopNeutrons")

	def stopHyperSpecClick(self):
		if self.stopHyperSpec.get() == 1:
			self.stopList.append("stopHyperSpec")
		elif self.stopHyperSpec.get() == 0:
			self.stopList.remove("stopHyperSpec")

	def gammaClick(self):
		result = tkMessageBox.askquestion("Turn Gamma HV On/Off", "Are You Sure?", icon="warning")
		if result == "yes":
			print "Gamma Ray HV Clicked"
		else:
			return
		
	def neutronHVClick(self):
		result = tkMessageBox.askquestion("Turn Neutron Hv On/Off", "Are You Sure?", icon="warning")
		if result == "yes":
			print "Neutron Ray HV Clicked"	
		else:
			return
	
	def dBusClick(self):
		print "DBUS Clicked"

	def busExtClick(self):
		print "BusExtender Clicked"

	def startMistiClick(self):
		print "Start MISTI Clicked"

	def gpsClick(self):
		print "GPS Clicked"

	def neutronsClick(self):
		print "Neutrons Clicked"
	
	def arduinoTriggeringClick(self):
		print "Arduino Triggering Clicked"	

	def lidarClick(self):
		print "Lidar Clicked"

	def ladybugClick(self):
		print "Ladybug Clicked"

	def hyperSpecClick(self):
		print "HyperSpec Clicked"

	def setTimestampedNotes(self):
		self.clickedNotes = False
		if self.systemClicked.get() == 0:
			self.currentClock = datetime.now()
		#if self.noteSaved == False:
		if self.operator.get() == "Enter Operator Name Here":
				self.noOperator()	
		else:			
			if self.fileOpened == False:
				operatorName = self.operator.get()
				operatorName = operatorName.translate(None,whitespace)
				self.fileName = '/home/mistidb/misti/Documents/RadMAP Runs/%s%s' %(operatorName,self.currentClock.strftime('%m%d%y'))
				if glob.glob('%s.txt' %self.fileName):
					if glob.glob(('%sv*.txt' %self.fileName)):
						fileList = glob.glob(('%sv*.txt' %self.fileName))
						fileListLength = len(fileList)
						self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Runs')
						self.fileName = ('%s/%s%sv.%s.txt' %(self.fileName[0],operatorName, self.currentClock.strftime('%m%d%y'),(fileListLength+2)))
					else:
						self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Runs')
						self.fileName = '%s/%s%sv.2.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
					self.f = open('%s' %self.fileName, "w")
					self.fileOpened = True
					self.f.write('%s: %s \n' %(self.currentClock.strftime('%H:%M:%S'), self.timestampedNotesEntry.get()))
					self.f = open('%s' %self.fileName)
					self.notesVar.set(self.f.read())
					self.f.close()
					self.timestampedNotesEntry.delete(0,END)
					self.noteSaved = True
				else:
					#self.fileName = glob.glob('/*/*/Documents/RadMAP Runs')
					self.fileName = '%s.txt' %self.fileName
					self.f = open('%s' %self.fileName, "w")
					self.fileOpened = True
					self.f.write('%s: %s \n' %(self.currentClock.strftime('%H:%M:%S'), self.timestampedNotesEntry.get()))
					self.f = open('%s' %self.fileName)
					self.notesVar.set(self.f.read())
					self.f.close()
					self.timestampedNotesEntry.delete(0,END)
					self.noteSaved = True					
			else:
				self.f = open('%s' %self.fileName, "a")
				self.f.write('%s: %s \n' %(self.currentClock.strftime('%H:%M:%S'), self.timestampedNotesEntry.get()))
				self.f = open('%s' %self.fileName)
				self.notesVar.set(self.f.read())
				self.f.close()
				self.timestampedNotesEntry.delete(0,END)
				self.noteSaved = True

		#elif self.noteSaved == True:
		#	self.timestampedNotesEntry.set("Note Already Saved: Ready for Next Entry")
		

	def setTimestampedNotesEnter(self,event):
		self.setTimestampedNotes()

	def timestampedNotesDisplay(self):
		print "Notes Display Clicked"
		self.f = open('%s' %self.fileName, "r")
		notes = self.f.read()
		print notes
		tkMessageBox.showinfo("Notes", notes)
		return
	
	def saveData(self):
		result = tkMessageBox.askquestion("Save Data", "Are You Sure?", icon="warning")
		if result == "yes":
			return
		else:
			return

	def closeWindow(self):
		bugSocket.close()
		liqSocket.close()
		hsSocket.close()
		context.destroy(0)		
		self.destroy()

	def resetClicked(self):
		result = tkMessageBox.askquestion("New Session", "Are You Sure?", icon="warning")
		if result == "yes":
			self.operatorGrid.grid_forget()
			self.elapsedTimeGrid.grid_forget()
			self.communicationDisplayGrid.grid_forget()
			self.mainControlGrid.grid_forget()
			self.resetButton.grid_forget()
			self.timestampedNotesGrid.grid_forget()
			self.dataOffloadGrid.grid_forget()
			self.initialize()
		else:
			return

	def noOperator(self):
		popUp = Toplevel()
		popUpLabel = Label(popUp, text="No Operator Entered: Please Enter Operator", width=45, font=2)
		popUpLabel.pack()
		#popUpLabelQuit = Button(popUp, text="Close", command=popUp.destroy())
		#popUpLabelQuit.pack()

	#def getNotes(self,txt,name):
	#	try:
	#		with open(name,"r") as f:
	#			txt.set(f.read())
	#	except IOError as e:
	#		print e
	#	else:
	#		self.after(1000,lambda:getNotes(self,txt,name))


if __name__ == "__main__":
	app = operationsApp(None)
	app.title('MISTI Operations GUI')
	app.tick()
	app.mainloop()


