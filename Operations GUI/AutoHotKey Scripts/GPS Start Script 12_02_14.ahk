Run, C:\Program Files (x86)\NovAtel Inc\CDU 3.9.0\NovAtelCDU.exe
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  95,  90
Sleep, 100
WinWait, Open, 
IfWinNotActive, Open, , WinActivate, Open, 
WinWaitActive, Open, 
MouseClick, left,  379,  51
Sleep, 9000
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  777,  81
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  529,  56
Sleep, 100
WinWait, Options, 
IfWinNotActive, Options, , WinActivate, Options, 
WinWaitActive, Options, 
MouseClick, left,  138,  123
MouseClick, left,  138,  123
Sleep, 100
Send, {CTRLDOWN}v{CTRLUP}
MouseClick, left,  527,  454
Sleep, 100
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  730,  257
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  561,  9
Sleep, 100