import subprocess
import shlex
import fnmatch

proc1 = subprocess.Popen(shlex.split('ps aux'), stdout=subprocess.PIPE)
proc2 = subprocess.Popen(shlex.split('grep python'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
proc3 = subprocess.Popen(shlex.split('wc -l'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc1.stdout.close()
proc2.stdout.close()
out1,err1=proc3.communicate()

proc4 = subprocess.Popen(shlex.split('ps aux'), stdout=subprocess.PIPE)
proc5 = subprocess.Popen(shlex.split('grep pipelineNew2'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
proc6 = subprocess.Popen(shlex.split('wc -l'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc4.stdout.close()
proc5.stdout.close()
out2,err2=proc6.communicate()

if fnmatch.fnmatch(out1, '*22') and fnmatch.fnmatch(out2, '*15'):
	#return True
	print 'True'
else:
	#return False
	#print 'False'
	print out