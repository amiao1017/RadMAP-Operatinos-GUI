Run, C:\Program Files (x86)\WeatherLink\WeatherLink 6.0.0.exe
WinWait, WeatherLink 6.0.0, 
IfWinNotActive, WeatherLink 6.0.0, , WinActivate, WeatherLink 6.0.0, 
WinWaitActive, WeatherLink 6.0.0, 
MouseClick, left,  32,  68
Sleep, 100
WinWait, Open Station, 
IfWinNotActive, Open Station, , WinActivate, Open Station, 
WinWaitActive, Open Station, 
MouseClick, left,  247,  73
Sleep, 100
MouseClick, left,  435,  58
Sleep, 100
WinWait, WeatherLink 6.0.0, 
IfWinNotActive, WeatherLink 6.0.0, , WinActivate, WeatherLink 6.0.0, 
WinWaitActive, WeatherLink 6.0.0, 
MouseClick, left,  269,  67
Sleep, 100
