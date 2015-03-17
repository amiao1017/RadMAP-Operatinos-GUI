;Run, C:\Program Files (x86)\DJI Product\Ground Station 4.0.10\release\GroundStationVer4.exe
SetTitleMatchMode 2
DetectHiddenText, On

if WinExist("NovAtel") {
	WinActivate, NovAtel
    ;Msgbox, Ready to start recording GPS?
}
else {
	clipboard = GPS False
	clipboard = %clipboard%
	return
}


WinActive("NovAtel")
;time = %A_now%
;date = %A_MMMM% %A_DD%, %A_YYYY%
;StartTime := A_TickCount
;FileCreateDir, E:\GPS_Verification\gps_%date%\


if WinExist("NovAtel") {
	WinActive("NovAtel")
	if WinExist("CONFIG - ASCII")
	{
		clipboard = GPS True
		clipboard = %clipboard%
		}else
	{
		clipboard = GPS False
		clipboard = %clipboard%
	}
	}else{
	clipboard = GPS False
	clipboard = %clipboard%
	return
	}

	
	
