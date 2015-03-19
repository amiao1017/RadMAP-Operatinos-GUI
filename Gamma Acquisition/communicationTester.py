from multiprocessing import Process, Value, Array, Pipe, Queue
import communicate

class Tester:
	self.parent_conn, self.child_conn = Pipe()
	messageRootname = "Received_Gamma_Ray_Data"
	p = Process(target=communicate(), args=(self.child_conn,self.curTime,messageRootname))
	p.start()
	self.parent_conn.send("Data")
	print self.parent_conn.recv()   # prints "Data Received"
	#p.join()




if __name__ == "__main__":
	test = Tester()

