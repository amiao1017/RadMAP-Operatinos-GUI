#!/usr/bin/env python
# Echo client program
import socket
import time
import numpy
import os
import threading
from multiprocessing import Process, Value, Array, Pipe, Queue
import struct

#context = zmq.Context()
#port = '5560'
#gammaSocket = context.socket(zmq.PAIR)
#gammaSocket.bind('tcp://127.0.0.1: %s' %port)

troubleshooting = 0

class madc_data:
	def __init__(self, host='mvme_fast', port=54321,path='/disks/project',rootname='',file_time=10.,maxFiles=None, queue=None):
		self.comm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.comm.connect((host, port))
		self.done = Value('i', 0)
		self.curTime = time.strftime("%Y_%m_%d",time.localtime())
		self.rootname1 = "Gamma_Ray_Data"
		self.rootname2 = "Timestamp_Data"
		self.path = path + "/misti/RadMAP_Gamma_Data/%s/" %self.curTime
		if troubleshooting:		
			print self.path
		pathBoolean = os.path.exists(self.path)
		if troubleshooting:		
			print "%s" %pathBoolean
		if not os.path.exists(self.path):
			os.makedirs(self.path)
			if troubleshooting:
				print "Path Made"
		name = os.path.join(self.path,time.strftime("%Y_%m_%d_%H%M.%S_",time.localtime())+self.rootname1+'.bin')
		timestampFile = os.path.join(self.path,time.strftime("%Y_%m_%d_%H%M.%S_",time.localtime())+self.rootname2+'.txt')
		self.output = open(name, 'a')
		self.timestampFile = open(timestampFile, 'a')
	
		self.last_time = time.time()
		self.file_time=file_time
		self.maxFiles = maxFiles
		self.fileCnt = 0
		self.Qout = queue
		self.stopThread = Value('i', 0)

	def getQueue(self):
	        return self.Qout

	def loop(self):
		if troubleshooting:
			print 'Qout:',self.Qout
		token = chr(0xff)+chr(0xff)+chr(0xff)+chr(0xff)
		buf = ''
		count = 0
		#started = 0
		while (not self.done.value):
			if troubleshooting:
				print "In While Loop; self.done.value = %s" %self.done.value
			data = ''
			while len(data) < 4:
				if self.done.value == 1:
					data = ''
					break
				dataLength = len(data)
				#if count < 5 or self.done.value:
				if troubleshooting:				
					print "In Data Loop; len(Data) = %s; self.done.value = %s" %(dataLength, self.done.value) 
				data += self.comm.recv(4-len(data))
				#print data
				count = count + 1
			if troubleshooting:
			    	print 'acqd here0',len(data)
		    
			if len(data) == 0:
			        break
	#            buflen= numpy.array(numpy.fromstring(data, dtype='>u4'), dtype='<u4')
			#if started:		 
			buflen = (struct.unpack('>L',data)[0] & 0x0fffffff)
			if troubleshooting:			
				print 'acqd here1:',buflen
			buf = data
			if troubleshooting:
				print "buf = data but before WHILE loop"
			while len(buf) < buflen:
				if troubleshooting:				
					print "In WHILE loop"
				print 'acqd here2:',len(buf),buflen,buflen-len(buf)
				buf += self.comm.recv(buflen-len(buf))
				if troubleshooting:				
					print "Past Recv call"
				#if not len(buf) = buflen:
					#buf += self.comm.recv(buflen-len(buf))
				#if self.done.value == 1:
					#break
			if troubleshooting:
				print 'acqd here3',len(buf)
	#            time.sleep(.1)
		 	if buflen > 0x5028: next   #throw out spurious DMAs
			buf1 = numpy.array(numpy.fromstring(buf, dtype='>u4'), dtype='<u4')            
			buf2 = bytearray(buf1.tostring())
			if self.Qout is not None:
				self.Qout.put([buf2,0])
	#               self.Qout.put([buf1,0])
	#		buf1 = numpy.array(numpy.fromstring(buf, dtype='>u4'), dtype='<u4')    
		
			#try:
				#if started:
				#gammaSocket.send(buf1)
				#print "Gammas Sent"
				#if gammaSocket.poll(1) != 0:
					#recvCommand = gammaSocket.recv()
					#print recvCommand
			#except:
				#pass
		
			try:
				#if started:
				#if count == 1:
				#buf1.tofile(self.output)
				#buf1.tofile(self.output2)
				self.output.write(buf1)
				self.currentTime = time.time()
				self.timestampFile.write("%s \n" %self.currentTime)				
				#else:	
				#	self.output.append(buf1)
				print "File Written @ %s" %self.currentTime
			except:
				pass

		#if not started:
		#	started = 1

	#            if pos >=0:
	#            print 'acqd here1',pos,len(buf)

	#                buf1 = numpy.array(numpy.fromstring(buf[:pos+4], dtype='>u4'), dtype='<u4')
	#                if self.Qout is not None:
	#                    self.Qout.put([buf1,0])
	#                try:
	#                    buf1.tofile(self.output)
	#                except:
	#                    pass
	#                buf = buf[pos+4:]                    
		                           
		   
	#            if self.Qout is not None:
	#                print 'loop here0',len(data)
	#                self.Qout.put([data,0])
	#                time.sleep(.000001)
	#                print 'loop here1',len(data),self.Qout.qsize(),type(data)


	#            try:
	#                data.tofile(self.output)
	#            except:
	#                pass


		    #if time.time() - self.last_time >= self.file_time:
		if self.done.value:
			print "End of Loop Reached"            
			self.output.close()
			self.timestampFile.close()
			print "Output Closed"
			self.stopThread.value = 1
		#        self.last_time = time.time()
		 #       self.fileCnt += 1
		  #      if self.maxFiles is None or self.fileCnt < self.maxFiles:
		   #         name = os.path.join(self.path,time.strftime("%Y_%m_%d_%H%M.%S_",time.localtime())+self.rootname1+'.bin')
		    #        self.output = open(name, 'w')
		     #       print 'name[%d]:%s' %(self.fileCnt,name)
		#else:
		#	break
	def stop_thread(self):
		while not self.stopThread.value:
			print "Waiting for stopThead, stopThread =", self.stopThread.value, "done.value =", self.done.value
			time.sleep(1)
		self.p.terminate()
		
	def stop(self):
		self.done.value = 1
		print "Stop called; self.done.value = %s" %self.done.value

	def start_thread(self,threadFlag=False):
		print "Start Thread Called"
		self.exitFlag = False
		if threadFlag:
			thread = threading.Thread(target=self.process_queue)
			thread.start()
			print "Thread Started, threadFlag = True"
		else:
			self.p = Process(target=self.loop)
	#            print 'hist:',__name__
	#            print 'hist:',dir(self.p)
	#            print 'module name:%s pid:%d' %(__name__,self.p.pid())
			self.p.start()
			print "Started target process = loop"
            
if __name__ == '__main__':
	m = madc_data()
	m.loop()
	m.stop()
