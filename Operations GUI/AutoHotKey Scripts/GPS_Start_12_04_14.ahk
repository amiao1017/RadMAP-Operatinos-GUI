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
Send, {CTRLDOWN}v{CTRLUP}{ENTER}
WinWait, Options, 
IfWinNotActive, Options, , WinActivate, Options, 
WinWaitActive, Options, 
MouseClick, left,  542,  451
Sleep, 100
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  737,  268
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  559,  7
Sleep, 100