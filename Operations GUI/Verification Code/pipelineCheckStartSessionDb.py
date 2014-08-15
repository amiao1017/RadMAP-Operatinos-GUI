import subprocess
import shlex
import fnmatch

proc1 = subprocess.Popen(shlex.split('ps aux'), stdout=subprocess.PIPE)
proc2 = subprocess.Popen(shlex.split('grep python'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
proc3 = subprocess.Popen(shlex.split('wc -l'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc1.stdout.close()
proc2.stdout.close()
out1,err1=proc3.communicate()

####Include binary gile log check. need to know output

if fnmatch.fnmatch(out1, '*14'):
	#return True
	print 'True'
elif fnmatch.fnmatch(out1, '*13'):
	print 'Restart startMISTI'
else:
	#return False
	#print 'False'
	print out