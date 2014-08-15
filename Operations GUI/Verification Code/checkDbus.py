#import dbus
import fnmatch

#result = dbus.SessionBus()
#result = "<dbus._dbus.SessionBus (session) at 0x7f00b151e5f0>"
result = "Failed to connect to socket /tmp/misti: Connection refused"
failPattern = ["Failed*", "failed*", "Fail*", "fail*", "*Connection refused"]
successPattern = ["*Session*", "*session*"]
if any(fnmatch.fnmatch(result,fPattern) for fPattern in failPattern):
	#return False
	print "False"
elif any(fnmatch.fnmatch(result,sPattern) for sPattern in successPattern):
	#return True
	print "True"
else:
	print "Script Failed"
