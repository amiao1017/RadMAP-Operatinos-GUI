import subprocess
import shlex

proc1 = subprocess.Popen(shlex.split('ps aux'), stdout=subprocess.PIPE)
proc2 = subprocess.Popen(shlex.split('grep busExtender'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
proc3 = subprocess.Popen(shlex.split('wc -l'), stdin=proc1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc1.stdout.close()
proc2.stdout.close()
out,err=proc3.communicate()

if fnmatch.fnmatch(out, '*3'):
	#return True
	print 'True'
else:
	#return False
	#print 'False'
	print out