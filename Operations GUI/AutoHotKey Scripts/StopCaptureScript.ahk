setTitleMatchMode 2

IfWinExist, startArduino
{
	WinActivate
}
IfWinNotActive, startArduino, WinWaitActive, startArduino,
WinWaitActive, startArduino
Send, q
Sleep, 500
Send, y
Sleep, 500
Send, y
Sleep, 500
Send, q

IfWinExist, startLadybugStarboard
{
	WinActivate
}
IfWinNotActive, startLadybugStarboard, WinWaitActive, startLadybugStarboard,
WinWaitActive, startLadybugStarboard
Send, q
Sleep, 500
Send, y
Sleep, 500
Send, y
Sleep, 500

IfWinExist, startLadybugPort
{
	WinActivate
}
IfWinNotActive, startLadybugPort, WinWaitActive, startLadybugPort,
WinWaitActive, startLadybugPort
Send, q
Sleep, 500
Send, y
Sleep, 500
Send, y
Sleep, 500

IfWinExist, startVelodyneStarboard
{
	WinActivate
}
IfWinNotActive, startVelodyneStarboard, WinWaitActive, startVelodyneStarboard,
WinWaitActive, startVelodyneStarboard
Send, q
Sleep, 500
Send, y
Sleep, 500
Send, y
Sleep, 500

IfWinExist, startVelodynePort
{
	WinActivate
}
IfWinNotActive, startVelodynePort, WinWaitActive, startVelodynePort,
WinWaitActive, startVelodynePort
Send, q
Sleep, 500
Send, y
Sleep, 500
Send, y
Sleep, 500
	
IfWinExist, startArduino
{
	WinActivate
	Send, q
}


