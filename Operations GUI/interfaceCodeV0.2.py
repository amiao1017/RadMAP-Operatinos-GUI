from __future__ import division
from Tkinter import *
from datetime import datetime, date, time
from string import whitespace
from PIL import Image, ImageTk
import os, sys, glob, Tkconstants,tkFileDialog,tkMessageBox,glob,tkFont,subprocess,zmq,time

context = zmq.Context()

#  Sockets to talk to servers
portDaq = "5553"
#daqSocket = context.socket(zmq.PAIR)
#daqSocket.bind("tcp://192.168.100.1:%s" % portDaq)
portMage = "5554"
#mageSocket = context.socket(zmq.PAIR)
#mageSocket.bind("tcp://192.168.100.1:%s" % portMage)
portBug = "5555"
bugSocket = context.socket(zmq.PAIR)
bugSocket.connect("tcp://192.168.100.42:%s" % portBug)
portLiq = "5557"
#liqSocket = context.socket(zmq.PAIR)
#liqSocket.bind("tcp://192.168.100.1:%s" % portLiq)
portHs = "5556"
hsSocket = context.socket(zmq.PAIR)
hsSocket.connect("tcp://192.168.100.43:%s" % portHs)

class operationsApp(Tk):

	
	def __init__(self,parent):
		Tk.__init__(self,parent)
		self.parent = parent
		self.initialize()

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
		self.startWeather = IntVar()
		self.startGps = IntVar()
		self.startNeutrons = IntVar()
		self.startHyperSpec = IntVar()
		self.stopCapture = IntVar()
		self.stopWeather = IntVar()
		self.stopGps = IntVar()
		self.stopNeutrons = IntVar()
		self.stopHyperSpec = IntVar()
		self.verificationGammaHvColor = StringVar()
		self.verificationNeutronHvColor = StringVar()		
		self.verificationDbusColor = StringVar()
		self.verificationDbusMonColor = StringVar()
		self.verificationStartMistiColor = StringVar()
		self.verificationArduinoColor = StringVar()
		self.verificationLidarColor = StringVar()
		self.verificationLadybugColor = StringVar()
		self.verificationWeatherColor = StringVar()
		self.verificationGpsColor = StringVar()
		self.verificationNeutronsColor = StringVar()
		self.verificationHyperSpecColor = StringVar()
		self.weatherDataCheck = StringVar()
		self.arduinoDataCheck = StringVar()
		self.ladybugDataCheck = StringVar()
		self.lidarDataCheck = StringVar()
		self.timestampedNotes = StringVar()
		self.dataOffloadNotes = StringVar()
		self.notesVar = StringVar()
		self.fileName = StringVar()
		self.fileOpened = False
		self.noteSaved = False
		self.multipleFiles = 0

		self.operatorGrid = Label (self)
		self.operatorGrid.grid(column=0,row=0)

		operatorLabel = Label(self.operatorGrid,anchor="w",text="Operator:", font=2)
		operatorLabel.grid(column=0,row=0)
		labelUnderlined = tkFont.Font(operatorLabel,operatorLabel.cget("font"))
		labelUnderlined.configure(underline = True)
		operatorLabel.configure(font=labelUnderlined)

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
		self.elapsedTimeGrid.grid(column=1,row=0)

		self.displayElapsedTimeLabel = Label(self.elapsedTimeGrid, text="Time Since Start of Session:")
		self.displayElapsedTimeLabel.grid(column=0,row=0)
		labelUnderlined = tkFont.Font(self.displayElapsedTimeLabel,self.displayElapsedTimeLabel.cget("font"))
		labelUnderlined.configure(underline = True)
		self.displayElapsedTimeLabel.configure(font=labelUnderlined)

		self.displayElapsedTime = Label(self.elapsedTimeGrid, textvariable=self.elapsedTime,width=4)
		self.displayElapsedTime.grid(column=1,row=0)

		self.displayClock = Label(self.elapsedTimeGrid, textvariable=self.clock, anchor="e", bg="gray",width=16)
		self.displayClock.grid(column=2,row=0)

		self.startGrid = Label(self)
		self.startGrid.grid(column=0,row=1)

		mainStart = Button(self.startGrid,text="START",activebackground="blue",activeforeground="white",relief=RAISED, command = self.mainStartClick)
		mainStart.grid(column=0,row=1)

		mainStop = Button(self.startGrid,text="STOP",activebackground="blue",activeforeground="white",relief=RAISED, command = self.mainStopClick)
		mainStop.grid(column=1,row=1)

		self.indStart = Label(self.startGrid,anchor="w")
		self.indStart.grid(column=0,row=2)

		self.startCaptureCheck = Checkbutton(self.indStart,variable=self.startCapture,text="Start Capture", command = self.startCaptureClick)
		self.startCaptureCheck.pack()

		self.startWeatherCheck = Checkbutton(self.indStart,variable=self.startWeather,text="Start Weather", command = self.startWeatherClick)
		self.startWeatherCheck.pack()

		self.startGpsCheck = Checkbutton(self.indStart,variable=self.startGps,text="Start GPS", command = self.startGpsClick)
		self.startGpsCheck.pack()

		self.startNeutronsCheck = Checkbutton(self.indStart,variable=self.startNeutrons,text="Start Neutrons", command = self.startNeutronsClick)
		self.startNeutronsCheck.pack()		

		self.startHyperSpecCheck = Checkbutton(self.indStart,variable=self.startHyperSpec,text="Start HyperSpec", command = self.startHyperSpecClick)
		self.startHyperSpecCheck.pack()

		indStartButton = Button(self.indStart, text="Select Start", command = self.indStartClick)
		indStartButton.pack()

		indStop = Label(self.startGrid)
		indStop.grid(column=1,row=2)

		self.stopCaptureCheck = Checkbutton(indStop,variable=self.stopCapture,text="Stop Capture", command = self.stopCaptureClick)
		self.stopCaptureCheck.pack()

		self.stopWeatherCheck = Checkbutton(indStop,variable=self.stopWeather,text="Stop Weather", command = self.stopWeatherClick)
		self.stopWeatherCheck.pack()

		self.stopGpsCheck = Checkbutton(indStop,variable=self.stopGps,text="Stop GPS", command = self.stopGpsClick)
		self.stopGpsCheck.pack()

		self.stopNeutronsCheck = Checkbutton(indStop,variable=self.stopNeutrons,text="Stop Neutrons", command = self.stopNeutronsClick)
		self.stopNeutronsCheck.pack()		

		self.stopHyperSpecCheck = Checkbutton(indStop,variable=self.stopHyperSpec,text="Stop HyperSpec", command = self.stopHyperSpecClick)
		self.stopHyperSpecCheck.pack()

		indStopButton = Button(indStop, text="Select Stop", command = self.indStopClick)
		indStopButton.pack()

		verificationLeds = Label(self.startGrid)
		verificationLeds.grid(column=2,row=2)

		#if Gamma HV is on self.verificationGammaHvColor.set("Green")
		#else 
		self.verificationGammaHvColor.set("Red")
		verificationGammaHvButton = Button(verificationLeds,text="Gamma Ray HV",relief=RAISED,bg=self.verificationGammaHvColor.get(),activebackground="blue",activeforeground="white", command = self.gammaHVClick)
		verificationGammaHvButton.pack()

		#if Neutron HV is on self.verificationNeutronHvColor.set("Green")
		#else 
		self.verificationNeutronHvColor.set("Red")
		verificationNeutronHvButton = Button(verificationLeds,text="Neutron HV",relief=RAISED,bg=self.verificationNeutronHvColor.get(),activebackground="blue",activeforeground="white", command = self.neutronHVClick)
		verificationNeutronHvButton.pack()

		#if Dbus is setup self.verificationDbusColor.set("Green")
		#else 
		self.verificationDbusColor.set("Red")
		verificationDbus = Label(verificationLeds, text="DBUS Setup",relief=RAISED,bg=self.verificationDbusColor.get())
		verificationDbus.pack()

		#if Dbus Monitoring is on self.verificationDbusMonColor.set("Green")
		#else 
		self.verificationDbusMonColor.set("Red")
		verificationDbusMon = Label(verificationLeds, text="DBUS Monitoring",relief=RAISED,bg=self.verificationDbusMonColor.get())
		verificationDbus.pack()

		#if Start Misti has been started self.verificationStartMistiColor.set("Green")
		#else 
		self.verificationStartMistiColor.set("Red")
		verificationStartMisti = Label(verificationLeds, text="Start MISTI",relief=RAISED,bg=self.verificationStartMistiColor.get())
		verificationStartMisti.pack()

		#if Arduino Triggering is on self.verificationArduinoColor.set("Green")
		#else 
		self.verificationArduinoColor.set("Red")
		verificationArduino = Label(verificationLeds, text="Arduino Triggering",relief=RAISED,bg=self.verificationArduinoColor.get())
		verificationArduino.pack()

		#if Lidar is on self.verificationLidarColor.set("Green")
		#else 
		self.verificationLidarColor.set("Red")
		verificationLidar = Label(verificationLeds, text="Lidar",relief=RAISED,bg=self.verificationLidarColor.get())
		verificationLidar.pack()

		#if Ladybug is on self.verificationLadybugColor.set("Green")
		#else 
		self.verificationLadybugColor.set("Red")
		verificationLadybug = Label(verificationLeds, text="Ladybug",relief=RAISED,bg=self.verificationLadybugColor.get())
		verificationLadybug.pack()

		#if Weather is on self.verificationWeatherColor.set("Green")
		#else 
		self.verificationWeatherColor.set("Red")
		verificationWeather = Label(verificationLeds, text="Weather",relief=RAISED,bg=self.verificationWeatherColor.get())
		verificationWeather.pack()

		#if GPS is on self.verificationGpsColor.set("Green")
		#else 
		self.verificationGpsColor.set("Red")
		verificationGps = Label(verificationLeds, text="NovAtel GPS",relief=RAISED,bg=self.verificationGpsColor.get())
		verificationGps.pack()

		#if Neutrons is on self.verificationNeutronsColor.set("Green")
		#else 
		self.verificationNeutronsColor.set("Red")
		verificationNeutrons = Label(verificationLeds, text="Neutrons Acquisistion",relief=RAISED,bg=self.verificationNeutronsColor.get())
		verificationNeutrons.pack()

		#if Hyper Spec is on self.verificationHyperSpecColor.set("Green")
		#else 
		self.verificationHyperSpecColor.set("Red")
		verificationHyperSpec = Label(verificationLeds, text="Hyper Spec",relief=RAISED,bg=self.verificationHyperSpecColor.get())
		verificationHyperSpec.pack()

		self.dataCheckGrid = Label(self)
		self.dataCheckGrid.grid(column=0,row=2)

		weatherDataCheckGrid = Label(self.dataCheckGrid)
		weatherDataCheckGrid.grid(column=0,row=0)

		weatherDataCheckLabel = Label(weatherDataCheckGrid,anchor="w",text="Weather:")
		weatherDataCheckLabel.grid(column=0,row=0)
		labelUnderlinedWeather = tkFont.Font(weatherDataCheckLabel,weatherDataCheckLabel.cget("font"))
		labelUnderlinedWeather.configure(underline = True)
		weatherDataCheckLabel.configure(font=labelUnderlinedWeather)

		self.weatherDataCheck.set("Session Not Started")	
		self.weatherDataCheckEntry = Label(weatherDataCheckGrid, textvariable=self.weatherDataCheck)
		self.weatherDataCheckEntry.grid(column=1,row=0)

		arduinoDataCheckGrid = Label(self.dataCheckGrid)
		arduinoDataCheckGrid.grid(column=1,row=0)
	
		arduinoDataCheckLabel = Label(arduinoDataCheckGrid,text="Arduino Triggering:")
		arduinoDataCheckLabel.grid(column=0,row=0)
		labelUnderlinedArduino = tkFont.Font(arduinoDataCheckLabel,arduinoDataCheckLabel.cget("font"))
		labelUnderlinedArduino.configure(underline = True)
		arduinoDataCheckLabel.configure(font=labelUnderlinedArduino)

		self.arduinoDataCheck.set("Session Not Started")
		self.arduinoDataCheckEntry = Label(arduinoDataCheckGrid, textvariable=self.arduinoDataCheck)
		self.arduinoDataCheckEntry.grid(column=1,row=0)

		ladybugDataCheckGrid = Label(self.dataCheckGrid)
		ladybugDataCheckGrid.grid(column=0,row=1)

		ladybugDataCheckLabel = Label(ladybugDataCheckGrid,text="Ladybugs:")
		ladybugDataCheckLabel.grid(column=0,row=0)
		labelUnderlinedLadybug = tkFont.Font(ladybugDataCheckLabel,ladybugDataCheckLabel.cget("font"))
		labelUnderlinedLadybug.configure(underline = True)
		ladybugDataCheckLabel.configure(font=labelUnderlinedLadybug)

		self.ladybugDataCheck.set("Session Not Started")
		self.ladybugDataCheckEntry = Label(ladybugDataCheckGrid, textvariable=self.ladybugDataCheck)
		self.ladybugDataCheckEntry.grid(column=1,row=0)

		lidarDataCheckGrid = Label(self.dataCheckGrid)
		lidarDataCheckGrid.grid(column=1,row=1)

		lidarDataCheckLabel = Label(lidarDataCheckGrid,text="Lidars:")
		lidarDataCheckLabel.grid(column=0,row=0)
		labelUnderlinedLidar = tkFont.Font(lidarDataCheckLabel,lidarDataCheckLabel.cget("font"))
		labelUnderlinedLidar.configure(underline = True)
		lidarDataCheckLabel.configure(font=labelUnderlinedLidar)

		self.lidarDataCheck.set("Session Not Started")
		self.lidarDataCheckEntry = Label(lidarDataCheckGrid, textvariable=self.lidarDataCheck)
		self.lidarDataCheckEntry.grid(column=1,row=0)

		#self.mapPhoto = ImageTk.PhotoImage(file="/home/rossebv/Downloads/BerkeleyCA.gif")

		#self.mapLabel = Label(self, image=self.mapPhoto)
		#self.mapLabel.image = self.mapPhoto
		#self.mapLabel.grid(column=1,row=1)

		#self.waterfallPhoto = ImageTk.PhotoImage(file="/home/rossebv/Downloads/waterfallExample.jpg")

		#self.waterfallLabel = Label(self, image=self.waterfallPhoto)
		#self.waterfallLabel.image = self.waterfallPhoto
		#self.waterfallLabel.grid(column=1,row=3)

		self.timestampedNotesGrid = Label(self,anchor="w")
		self.timestampedNotesGrid.grid(column=0,row=3)

		timestampedNotesLabel = Label(self.timestampedNotesGrid,text="Notes:")
		timestampedNotesLabel.grid(column=0,row=0)
		labelUnderlinedNotes = tkFont.Font(timestampedNotesLabel,timestampedNotesLabel.cget("font"))
		labelUnderlinedNotes.configure(underline = True)
		timestampedNotesLabel.configure(font=labelUnderlinedNotes)

		#self.timestampedNotes.set("Insert Notes Here")
		self.timestampedNotesEntry = Entry(self.timestampedNotesGrid, textvariable=self.timestampedNotes)
		self.timestampedNotesEntry.grid(column=1,row=0)
		self.timestampedNotesEntry.bind("<Return>", self.setTimestampedNotesEnter)
		self.timestampedNotesEntry.bind("<Button-1>", lambda number: self.entryClicked(2))
		self.timestampedNotesEntry.insert(0,"Insert Notes Here")

		self.timestampedNotesButton = Button(self.timestampedNotesGrid,text="Save Note",activebackground="blue",activeforeground="white",relief=RAISED, command = self.setTimestampedNotes)
		self.timestampedNotesButton.grid(column=2,row=0) 

		self.timestampedNotesDisplay = Label(self.timestampedNotesGrid, anchor=S, textvariable=self.notesVar, bg="white", relief=RAISED, width = 36, height=15, wraplength=275)
		self.timestampedNotesDisplay.grid(column=0,row=1,columnspan=3)

		self.timestampedNotesDisplayButton = Button(self.timestampedNotesGrid,text="Display Notes",activebackground="blue",activeforeground="white",relief=RAISED, command = self.timestampedNotesDisplay)
		self.timestampedNotesDisplayButton.grid(column=0,row=2, columnspan=3) 

		self.dataOffloadGrid = Label(self,anchor="w")
		self.dataOffloadGrid.grid(column=0,row=4)

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

		#self.resetLabel = Label(self)
		#self.resetLabel.grid(column=1, row=4)	

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
			elapsedTimeGrid = Label(self)
			elapsedTimeGrid.grid(column=1,row=0)
			self.displayElapsedTimeLabel = Label(elapsedTimeGrid, text="Time Since Start of Session:")
			self.displayElapsedTimeLabel.grid(column=0,row=0)
			labelUnderlined = tkFont.Font(self.displayElapsedTimeLabel,self.displayElapsedTimeLabel.cget("font"))
			labelUnderlined.configure(underline = True)
			self.displayElapsedTimeLabel.configure(font=labelUnderlined)
			self.displayElapsedTime = Label(elapsedTimeGrid, textvariable=self.elapsedTime,fg="white",bg="blue",width=4)
			self.displayElapsedTime.grid(column=1,row=0)
			self.displayClock = Label(elapsedTimeGrid, textvariable=self.clock, anchor="e", bg="orange",fg="black",width=7)
			self.displayClock.grid(column=2,row=0)

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
					self.fileName = '/home/mistidb/misti/Documents/RadMAP Tests/%s%s' %(operatorName,self.currentClock.strftime('%m%d%y'))
					if glob.glob("%s.txt" %self.fileName): 
						if glob.glob(('%sv*.txt' %self.fileName)):
							fileList = glob.glob(('%sv*.txt' %self.fileName))
							fileListLength = len(fileList)
							self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Tests')
							self.fileName = ('%s/%s%sv.%s.txt' %(self.fileName[0],operatorName, self.currentClock.strftime('%m%d%y'),(fileListLength+2)))
						else:
							self.fileName = glob.glob('/home/mistidb/misti/Documents/RadMAP Tests')
							self.fileName = '%s/%s%sv.2.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
						self.f = open("%s" %self.fileName, "w")
						self.fileOpened = True
						self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
						self.f = open('%s' %self.fileName)
						self.notesVar.set(self.f.read())
						self.f.close()
					else:
						#self.fileName = glob.glob('/*/Documents/RadMAP Tests')
						self.fileName = '%s.txt' %(self.fileName)
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

				self.elapsedTimeGrid.grid(column=1,row=0)
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
				self.startList = ["STA", "startArduino", "startLidar", "startLadybug", "startWeather", "startGps", "startNeutrons", "startHyperSpec", "The End"]
				for command in self.startList: 
					print "%s" % command
					if command == "startLidar":
						try:
							bugSocket.send('startLidar')
							if bugSocket.poll(100) != 0:
								bugMessage = bugSocket.recv()
								print "%s" % bugMessage
						except ZMQError:
							print "Socket Send Failed"
					if command == "startHyperSpec":
						print "inside if statement"
						try:
							hsSocket.send('startHyperSpec')
							print "Sent HyperSpec Signal"
							if hsSocket.poll(100) != 0:
								hsMessage = hsSocket.recv()
								print "%s" % hsMessage
						except ZMQError:
							print "Socket Send Failed"

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
					self.fileName = '/*/*/Documents/RadMAP Tests/%s%s' %(operatorName,self.currentClock.strftime('%m%d%y'))
					if glob.glob("%s.txt" %self.fileName): 
						if glob.glob(('%sv*.txt' %self.fileName)):
							fileList = glob.glob(('%sv*.txt' %self.fileName))
							fileListLength = len(fileList)
							self.fileName = glob.glob('/*/*/Documents/RadMAP Tests')
							self.fileName = ('%s/%s%sv.%s.txt' %(self.fileName[0],operatorName, self.currentClock.strftime('%m%d%y'),(fileListLength+2)))
						else:
							self.fileName = glob.glob('/*/*/Documents/RadMAP Tests')
							self.fileName = '%s/%s%sv.2.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
						self.f = open("%s" %self.fileName, "w")
						self.fileOpened = True
						self.f.write("%s: Session Started \n" %self.currentClock.strftime('%H:%M:%S'))
						self.f = open('%s' %self.fileName)
						self.notesVar.set(self.f.read())
						self.f.close()
					else:
						self.fileName = glob.glob('/*/*/Documents/RadMAP Tests')
						self.fileName = '%s/%s%s.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
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

				self.elapsedTimeGrid.grid(column=1,row=0)
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
					if command == "startLidar":
						try:
							bugSocket.send('startLidar')
							if bugSocket.poll(100) != 0:
								bugMessage = bugSocket.recv()
								print "%s" % bugMessage
						except ZMQError:
							print "Socket Send Failed"
				for command in self.startList:
					if command == "startHyperSpec":
						try:
							hsSocket.send('startHyperSpec')
							if hsSocket.poll(100) != 0:
								hsMessage = hsSocket.recv()
								print "%s" % hsMessage
						except ZMQError:
							print "Socket Send Failed"
				self.startCaptureCheck.config(state=DISABLED)
				self.startWeatherCheck.config(state=DISABLED)
				self.startGpsCheck.config(state=DISABLED)
				self.startNeutronsCheck.config(state=DISABLED)
				self.startHyperSpecCheck.config(state=DISABLED)


	def startCaptureClick(self):
		if self.startCapture.get() == 1:
			self.startList.append("startArduino")
			self.startList.append("startLidar")
			self.startList.append("startLadybug")
		elif self.startCapture.get() == 0:
			self.startList.remove("startArduino")
			self.startList.remove("startLidar")
			self.startList.remove("startLadybug")

	def startWeatherClick(self):
		if self.startWeather.get() == 1:
			self.startList.append("startWeather")
		elif self.startWeather.get() == 0:
			self.startList.remove("startWeather")	

	def startGpsClick(self):
		if self.startGps.get() == 1:
			self.startList.append("startGps")
		elif self.startGps.get() == 0:
			self.startList.remove("startGps")

	def startNeutronsClick(self):
		if self.startNeutrons.get() == 1:
			self.startList.append("startNeutrons")
		elif self.startNeutrons.get() == 0:
			self.startList.remove("startNeutrons")

	def startHyperSpecClick(self):
		if self.startHyperSpec.get() == 1:
			self.startList.append("startHyperSpec")
		elif self.startHyperSpec.get() == 0:
			self.startList.remove("startHyperSpec")

	def mainStopClick(self):
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
			self.stopList = ["STO", "stopArduino", "stopLidar", "stopLadybug", "stopWeather", "stopGps", "stopNeutrons", "stopHyperSpec"]
			
			#booleans to check if item is off if not kill

				#subprocess.Popen(shlex.split("Stop Dbus"))
				#daqSocket.send(self.startList)
				#mageSocket.send(self.startList)
				#subprocess.Popen(shlex.split("Stop Dbus Mon"))
				#daqSocket.send("dBusMon")
				#mageSocket.send("dBusMon")
				#subprocess.Popen(shlex.split("Stop MISTI"))
				#daqSocket.send("startMISTI")
				#mageSocket.send("startMISTI")
				#subprocess.Popen(shlex.split("Run.py call"))
				#bugSocket.send(self.startList)
				#hsSocket.send(self.startList)
				#liqSocket.send(self.startList)	
	
	def indStopClick(self):
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
			#subprocess.Popen(shlex.split("Set up Dbus call"))
			#daqSocket.send(self.startList)
			#mageSocket.send(self.startList)
			#subprocess.Popen(shlex.split("Set up Dbus Monitor call"))
			#daqSocket.send("dBusMon")
			#mageSocket.send("dBusMon")
			#subprocess.Popen(shlex.split("Satrt MISTI call"))
			#daqSocket.send("startMISTI")
			#mageSocket.send("startMISTI")
			#subprocess.Popen(shlex.split("Run.py call"))
			#bugSocket.send(startList.self)
			#hsSocket.send(self.startList)
			#liqSocket.send(self.startList)	

	def stopCaptureClick(self):
		if self.stopLidar.get() == 1:
			self.stopList.append("stopCapture")
		elif self.stopLidar.get() == 0:
			self.stopList.remove("stopCapture")

	def stopWeatherClick(self):
		if self.stopWeather.get() == 1:
			self.stopList.append("stopWeather")
		elif self.stopWeather.get() == 0:
			self.stopList.remove("stopWeather")

	def stopGpsClick(self):
		if self.stopGps.get() == 1:
			self.stopList.append("stopGps")
		elif self.stopGps.get() == 0:
			self.stopList.remove("stopGps")

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

	def gammaHVClick(self):
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

	def setTimestampedNotes(self):
		self.clickedNotes = False
		if self.systemClicked.get() == 0:
			self.currentClock = datetime.now()
		#if self.noteSaved == False:
		if self.fileOpened == False:
			if self.operator.get() == "Enter Operator Name Here":
				self.noOperator()
			else:
				operatorName = self.operator.get()
				operatorName = operatorName.translate(None,whitespace)
				self.fileName = '/*/*/Documents/RadMAP Tests/%s%s' %(operatorName,self.currentClock.strftime('%m%d%y'))
				if glob.glob('%s.txt' %self.fileName):
					if glob.glob(('%sv*.txt' %self.fileName)):
						fileList = glob.glob(('%sv*.txt' %self.fileName))
						fileListLength = len(fileList)
						self.fileName = glob.glob('/*/*/Documents/RadMAP Tests')
						self.fileName = ('%s/%s%sv.%s.txt' %(self.fileName[0],operatorName, self.currentClock.strftime('%m%d%y'),(fileListLength+2)))
					else:
						self.fileName = glob.glob('/*/*/Documents/RadMAP Tests')
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
					self.fileName = glob.glob('/*/*/Documents/RadMAP Tests')
					self.fileName = '%s/%s%s.txt' %(self.fileName[0], operatorName, self.currentClock.strftime('%m%d%y'))
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
		return
	
	def saveData(self):
		result = tkMessageBox.askquestion("Save Data", "Are You Sure?", icon="warning")
		if result == "yes":
			return
		else:
			return

	def resetClicked(self):
		result = tkMessageBox.askquestion("New Session", "Are You Sure?", icon="warning")
		if result == "yes":
			self.operatorGrid.grid_forget()
			self.elapsedTimeGrid.grid_forget()
			self.startGrid.grid_forget()
			self.resetButton.grid_forget()
			self.dataCheckGrid.grid_forget()
			self.timestampedNotesGrid.grid_forget()
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


		

		#self.indSelectStart = Menubutton(self,activebackground="blue",activeforeground="white",relief=RAISED,text="Start Individual Sensor")
		#self.indSelectStart.menu = Menu (self.indSelectStart, tearoff = 0)
		#self.indSelectStart["menu"] = self.indSelectStart.menu
		#self.indSelectStart.lidarMenu = Menu (self.indSelectStart, tearoff = 0)
		#self.indSelectStart.ladybugMenu = Menu (self.indSelectStart, tearoff = 0)
		#self.indSelectStart.menu.add_cascade(label="Select Lidar",activebackground="blue",activeforeground="white",menu=self.indSelectStart.lidarMenu)
		#self.indSelectStart.menu.add_cascade(label="Select Ladybug",activebackground="blue",activeforeground="white",menu=self.indSelectStart.ladybugMenu)
		#self.indSelectStart.lidarMenu.add_command(label="Port Lidar",activebackground="blue",activeforeground="white", command=self.startPortLidar)
		#self.indSelectStart.lidarMenu.add_command(label="Starboard Lidar",activebackground="blue",activeforeground="white", command=self.startStarboardLidar)
		#self.indSelectStart.ladybugMenu.add_command(label="Port Ladybug",activebackground="blue",activeforeground="white", command=self.startPortLadybug)
		#self.indSelectStart.ladybugMenu.add_command(label="Starboard Ladybug",activebackground="blue",activeforeground="white", command=self.startStarboardLadybug)
		#self.indSelectStart.menu.add_command(label="NovAtel GPS",activebackground="blue",activeforeground="white", command=self.startGps)
		#self.indSelectStart.menu.add_command(label="Weather",activebackground="blue",activeforeground="white", command=self.startWeather)
		#self.indSelectStart.menu.add_command(label="Plot Position",activebackground="blue",activeforeground="white", command=self.aimDetectorPlotPosition)
		#self.aimDetector.plotsMenu.add_command(label="Plot Time",activebackground="blue",activeforeground="white", command=self.aimDetectorPlotTime)
		#self.aimDetector.plotsMenu.add_command(label="Plot Spectrum",activebackground="blue",activeforeground="white", command=self.aimDetectorPlotSpectrum)
		#self.aimDetector.plotsMenu.add_command(label="Plot Spectrum Detectors",activebackground="blue",activeforeground="white", command=self.aimDetectorPlotSpectrumDetectors)
		#self.aimDetector.plotsMenu.add_command(label="Plot Waterfall",activebackground="blue",activeforeground="white", command=self.aimDetectorPlotWaterfall)
		#self.aimDetector.plotsMenu.add_command(label="Plot Plus",activebackground="blue",activeforeground="white", command=self.aimDetectorPlotPlus)
		#self.aimDetector.grid(column=0,row=2)

		#self.labelVariableText2 = StringVar()
		#self.labelVariableColor2 = StringVar()
		#self.labelVariableColor2.set("red")
		#label2 = Label(self,textvariable=self.labelVariableText2,relief=RAISED,bg=self.labelVariableColor2.get())
		#label2.grid(column=1,row=2,columnspan=2,sticky='EW')

