setTitleMatchMode 2
IfWinExist, 2iHyperSpecAcquisition
{
	WinActivate
}
IfWinNotActive, 2iHyperSpecAcquisition, WinWaitActive, 2iHyperSpecAcquisition,
WinWaitActive, 2iHyperSpecAcquisition
Send, q
	

