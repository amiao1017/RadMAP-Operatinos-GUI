Run, C:\Program Files (x86)\NovAtel Inc\CDU 3.9.0\NovAtelCDU.exe
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  101,  85
Sleep, 100
WinWait, Open, 
IfWinNotActive, Open, , WinActivate, Open, 
WinWaitActive, Open, 
MouseClick, left,  380,  50
Sleep, 9000 ;for the gps gui to load after opening
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  779,  97
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  535,  64
Sleep, 100
WinWait, Options, 
IfWinNotActive, Options, , WinActivate, Options, 
WinWaitActive, Options, 
MouseClick, left,  390,  121
Sleep, 100
WinWait, Save As, 
IfWinNotActive, Save As, , WinActivate, Save As, 
WinWaitActive, Save As, 
Send, {CTRLDOWN}a{CTRLUP}{BACKSPACE}
Sleep, 100
MouseClick, left,  118,  66
Sleep, 100
MouseClick, left,  225,  155
Sleep, 100
MouseClick, left,  225,  155
Sleep, 100
MouseClick, right,  247,  39
Sleep, 100
MouseClick, left,  276,  73
Sleep, 100
MouseClick, left,  136,  414
MouseClick, left,  136,  414
Sleep, 100
MouseClick, left,  137,  423
Sleep, 100
MouseClick, right,  138,  423
Sleep, 100
MouseClick, left,  159,  503
Sleep, 100
Send, \gps\logfile.gps{ENTER}
WinWait, Options, 
IfWinNotActive, Options, , WinActivate, Options, 
WinWaitActive, Options, 
MouseClick, left,  548,  451
Sleep, 100
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  734,  263
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  562,  7
Sleep, 100