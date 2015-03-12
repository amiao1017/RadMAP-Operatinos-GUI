import zmq
import sys, os, time

context = zmq.Context()
port = '5560'
gammaSocket = context.socket(zmq.PAIR)
gammaSocket.bind('tcp://127.0.0.1:%s' %port)

path = "/disks/project/misti"
curTime = time.strftime("%Y_%m_%d",time.localtime())
rootname = "Socket_Received_Data"
path = path + "/RadMAP_Gamma_Data/%s/" %curTime
print path
if not os.path.exists(path):
	os.makedirs(path)
	print "Path Made"
name = os.path.join(path,time.strftime("%Y_%m_%d_%H%M.%S_",time.localtime())+rootname+'.bin')

while True:
	if gammaSocket.poll(1) != 0:
		gammaData = gammaSocket.recv()
		print "Gammas received on Receiver Side"
		gammaSocket.send("Gammas Message Received on Sender Side")
		self.output = open(name, 'a')
		self.output.write(gammaData)
		self.output.close()


