;Run, C:\Program Files (x86)\DJI Product\Ground Station 4.0.10\release\GroundStationVer4.exe
SetTitleMatchMode 2
DetectHiddenText, On

if WinExist("NovAtel") {
	WinActivate, NovAtel
    ;Msgbox, Ready to start recording GPS?
}
else {
	clipboard = GPS False
	return
}


WinActive("NovAtel")
time = %A_now%
date = %A_MMMM% %A_DD%, %A_YYYY%
StartTime := A_TickCount
FileCreateDir, C:\Users\misti\Desktop\RadMAP-Operatinos-GUI\gps_%date%\

while (1){

if WinExist("NovAtel") {
	WinActive("NovAtel")
	if WinExist("CONFIG - ASCII")
	{
		clipboard = GPS True
		;MsgBox, Window Exists
		WinGetText, text
		Loop, parse, text, `n, `r
				
		
		IfInString, A_LoopField, #BESTPOS
		{
		;MsgBox, %A_LoopField%
		FileAppend, 
		(
		%A_LoopField%`n
		), C:\Users\misti\Desktop\RadMAP-Operatinos-GUI\gps_%date%\%time%.txt
		Sleep, 1000
		}
		
		}else
	{
		clipboard = GPS False
	}
	}else{
	clipboard = GPS False
	return
	}
	}
	
	
