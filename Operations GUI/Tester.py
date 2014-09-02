from __future__ import division
from Tkinter import *
import os, sys, Tkconstants,tkFileDialog,glob,tkFont,subprocess,zmq


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
		self.operator = StringVar()
		self.operatorCopy = StringVar()



		#plotLabel = Label (self,anchor="s",text="Visualizations")
		#plotLabel.grid(column=0,row=1)
		#labelUnderlined = tkFont.Font(plotLabel,plotLabel.cget("font"))
		#labelUnderlined.configure(underline = True)
		#plotLabel.configure(font=labelUnderlined)

		label1 = Label(self,anchor="w",text="Operator")
		label1.grid(column=0,row=0)

		self.entryOperator = Entry(self, textvariable=self.operator)
		self.operator.set("Enter Operator Name Here")
		print self.operator.get()
		print self.entryOperator.get()
		self.entryOperator.grid(column=1,row=0)
		self.entryOperator.bind("<Return>", self.setOperatorClick)
		#self.entryOperator.insert(0,"Enter Operator Name Here")

		self.operatorCopy.set(self.operator.get())

		self.entryOperatorCopy = Entry(self, textvariable=self.operatorCopy)
		self.entryOperatorCopy.grid(column=1,row=2)
		#self.entryOperatorCopy.insert(0, self.operatorCopy)

		print self.operator.get()
		print self.entryOperator.get()
		print self.operatorCopy.get()
		print self.entryOperatorCopy.get()
	

		setOperator = Button(self,text="Set Operator",activebackground="blue",activeforeground="white",relief=RAISED, command = self.setOperatorClick)
		setOperator.grid(column=2,row=0)

	def setOperatorClick(self):
		print self.operator.get()
		print self.entryOperator.get()
		print self.operatorCopy.get()
		print self.entryOperatorCopy.get()

		#self.operator.set(self.entryOperator.get())
		#self.entryOperator.delete(0,END)
		#self.entryOperator.insert(0,operator)
		#self.operatorCopy.set(self.operator)

if __name__ == "__main__":
	app = operationsApp(None)
	app.title('MISTI Operations GUI')
	app.mainloop()