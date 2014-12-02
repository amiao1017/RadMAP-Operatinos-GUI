Run, C:\Program Files (x86)\NovAtel Inc\CDU 3.9.0\NovAtelCDU.exe
SetTitleMatchMode 2
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  101,  84
Sleep, 100
WinWait, Open, 
IfWinNotActive, Open, , WinActivate, Open, 
WinWaitActive, Open, 
MouseClick, left,  396,  61
Sleep, 9000
WinWait, NovAtel CDU, 
IfWinNotActive, NovAtel CDU, , WinActivate, NovAtel CDU, 
WinWaitActive, NovAtel CDU, 
MouseClick, left,  762,  76
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  522,  61
Sleep, 100
WinWait, Options, 
IfWinNotActive, Options, , WinActivate, Options, 
WinWaitActive, Options, 
MouseClick, left,  182,  127
MouseClick, left,  182,  127 ;double click
Sleep, 100
Send, {CTRLDOWN}v{CTRLUP} ;paste the path to the log file
MouseClick, left,  559,  451
Sleep, 100
WinWait, CONFIG - Logging Control, 
IfWinNotActive, CONFIG - Logging Control, , WinActivate, CONFIG - Logging Control, 
WinWaitActive, CONFIG - Logging Control, 
MouseClick, left,  362,  69
Sleep, 100