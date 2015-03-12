import multiprocessing
import os, time, zmq, sys

class communication:
	def __init__(self,conn, curTime='', messageRootname=''):
		self.data = ''		
		self.communicate(conn)
		path = "/disks/project/misti/RadMAP_Gamma_Data/%s/" %curTime
		name = os.path.join(path,time.strftime("%Y_%m_%d_%H%M.%S",time.localtime())+messageRootname+'.bin')
		self.output = open(name, 'a')
		self.output.write(self.data)
		self.output.close()


	def communicate(conn):
		self.data = conn.recv()
		message = "Data Received"
		print message	
    		conn.send(message)
    		conn.close()

if __name__ == "__main__":
	communicate = communication(conn,curTime,messageRootname)
