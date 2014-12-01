IfWinExist, iseg VmeControl Version 0.87
{
  WinActivate
}
IfWinNotActive, iseg VmeControl Version 0.87, , WinActivate, iseg VmeControl Version 0.87, 
WinWaitActive, iseg VmeControl Version 0.87, 
MouseClick, left,  46,  75
Sleep, 100
Send, {CTRLDOWN}f{CTRLUP}
MouseClick, left,  116,  73
Sleep, 100
Send, {CTRLDOWN}f{CTRLUP}
Sleep, 75000
MouseClick, left,  1268,  21
Sleep, 100
Send, {ENTER}



