setTitleMatchMode 2

IfWinExist, startArduino
{
	clipboard = False
	clipboard = %clipboard%
	return
}
Sleep, 100

IfWinExist, startLadybugStarboard
{
	clipboard = False
	clipboard = %clipboard%
	return
}
Sleep, 100
IfWinExist, startLadybugPort
{
	clipboard = False
	clipboard = %clipboard%
	return
}

Sleep, 100
IfWinExist, startVolodyneStarboard
{
	clipboard = False
	clipboard = %clipboard%
	return
}
Sleep, 100
IfWinExist, startVelodynePort
{
	clipboard = False
	clipboard = %clipboard%
	return
}
Sleep, 100
clipboard = True
clipboard = %clipboard%
return
