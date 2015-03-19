#!/usr/bin/env python
# Echo client program
import socket
import time
import numpy
import os
import threading
from multiprocessing import Process
import struct

class madc_data:
    def __init__(self, host='mvme_fast', port=54321,path=os.environ['ARCHIVE'],rootname='',file_time=10.,maxFiles=None, queue=None):
        self.comm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.comm.connect((host, port))
        self.done = 0
        self.rootname = rootname
        self.path = path
        name = os.path.join(self.path,time.strftime("%Y_%m_%d_%H%M.%S_",time.localtime())+self.rootname+'.bin')
        self.output = open(name, 'w')
	self.output2 = open("/home/mistidb/misti/Desktop/%s.bin" %name, 'w')

        self.last_time = time.time()
        self.file_time=file_time
        self.maxFiles = maxFiles
        self.fileCnt = 0
        self.Qout = queue

    def getQueue(self):
        return self.Qout

    def loop(self):
        print 'Qout:',self.Qout
        token = chr(0xff)+chr(0xff)+chr(0xff)+chr(0xff)
        buf = ''
        while (not self.done):
            data = ''
            while len(data) < 4:
                data += self.comm.recv(4-len(data))
#            print 'acqd here0',len(data)
            
            if len(data) == 0:
                break
#            buflen= numpy.array(numpy.fromstring(data, dtype='>u4'), dtype='<u4')
            buflen = (struct.unpack('>L',data)[0] & 0x0fffffff)
#            print 'acqd here1:',buflen
            buf = data
            while len(buf) < buflen:
#                print 'acqd here2:',len(buf),buflen-len(buf)
                buf += self.comm.recv(buflen-len(buf))
#            print 'acqd here3',len(buf)
#            time.sleep(.1)
            if buflen > 0x5028: next   #throw out spurious DMAs
            buf1 = numpy.array(numpy.fromstring(buf, dtype='>u4'), dtype='<u4')            
            buf2 = bytearray(buf1.tostring())
            if self.Qout is not None:
                self.Qout.put([buf2,0])
#                self.Qout.put([buf1,0])
#            buf1 = numpy.array(numpy.fromstring(buf, dtype='>u4'), dtype='<u4')    
            try:
                buf1.tofile(self.output)
		#buf1.tofile(self.output2)
		self.output2.write(buf1)
            except:
                pass

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


            if time.time() - self.last_time >= self.file_time:
                self.output.close()
                self.last_time = time.time()
                self.fileCnt += 1
                if self.maxFiles is None or self.fileCnt < self.maxFiles:
                    name = os.path.join(self.path,time.strftime("%Y_%m_%d_%H%M.%S_",time.localtime())+self.rootname+'.bin')
                    self.output = open(name, 'w')
                    print 'name[%d]:%s' %(self.fileCnt,name)
                else:
                    break

    def stop(self):
        self.done = 1

    def start_thread(self,threadFlag=False):
        self.exitFlag = False
        if threadFlag:
            thread = threading.Thread(target=self.process_queue)
            thread.start()
        else:
            self.p = Process(target=self.loop)
#            print 'hist:',__name__
#            print 'hist:',dir(self.p)
#            print 'module name:%s pid:%d' %(__name__,self.p.pid())
            self.p.start()
            
if __name__ == '__main__':
    m = madc_data()
    m.loop()
    m.stop()
