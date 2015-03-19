setTitleMatchMode 2
IfWinExist, NIRHyperSpecAcquisition
{
	WinActivate
}
IfWinNotActive, NIRHyperSpecAcquisition, WinWaitActive, NIRHyperSpecAcquisition,
WinWaitActive, NIRHyperSpecAcquisition
Send, q
	

