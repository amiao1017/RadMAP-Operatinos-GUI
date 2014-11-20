Run, C:\Program Files (x86)\WeatherLink\WeatherLink 6.0.0.exe
SetTitleMatchMode 2
WinWait, WeatherLink, 

if WinExist("WeatherLink") {
	WinActivate, WeatherLink
}

WinActive("WeatherLink")
MouseClick, left,  24,  74
Sleep, 100
WinWait, Open Station, 

if WinExist("Open Station"){
	WinActivate, Open Station
}

WinActive("Open Station")
MouseClick, left,  261,  69
Sleep, 100
MouseClick, left,  441,  50
Sleep, 100
WinWait, WeatherLink, 

if WinExist("WeatherLink") {
	WinActivate, WeatherLink
}

MouseClick, left,  267,  79
Sleep, 100