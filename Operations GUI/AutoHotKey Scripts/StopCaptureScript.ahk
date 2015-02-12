setTitleMatchMode 2

IfWinExist, startArduino
{
	WinActivate
}
IfWinNotActive, startArduino, WinWaitActive, startArduino,
WinWaitActive, startArduino
Send, q
Sleep, 100
Send, y
Sleep, 100
Send, y

IfWinExist, startLadybugStarboard
{
	WinActivate
}
IfWinNotActive, startLadybugStarboard, WinWaitActive, startLadybugStarboard,
WinWaitActive, startLadybugStarboard
Send, q
Sleep, 100
Send, y
Sleep, 100
Send, y

IfWinExist, startLadybugPort
{
	WinActivate
}
IfWinNotActive, startLadybugPort, WinWaitActive, startLadybugPort,
WinWaitActive, startLadybugPort
Send, q
Sleep, 100
Send, y
Sleep, 100
Send, y

IfWinExist, startVolodyneStarboard
{
	WinActivate
}
IfWinNotActive, startVolodyneStarboard, WinWaitActive, startVolodyneStarboard,
WinWaitActive, startVolodyneStarboard
Send, q
Sleep, 100
Send, y
Sleep, 100
Send, y

IfWinExist, startVelodynePort
{
	WinActivate
}
IfWinNotActive, startVelodynePort, WinWaitActive, startVelodynePort,
WinWaitActive, startVelodynePort
Send, q
Sleep, 100
Send, y
Sleep, 100
Send, y
	

