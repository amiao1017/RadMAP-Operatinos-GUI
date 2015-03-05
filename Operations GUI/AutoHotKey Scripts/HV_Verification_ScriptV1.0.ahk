IfWinNotExist, iseg VmeControl Version 0.87
{
	clipboard = False
}

WinWait, iseg VmeControl Version 0.87, 
IfWinNotActive, iseg VmeControl Version 0.87, , WinActivate, iseg VmeControl Version 0.87, 
WinWaitActive, iseg VmeControl Version 0.87, 
MouseClick, left,  56,  75
Sleep, 100
PixelGetColor, color,  747,  183
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  739,  205
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  739,  236
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  739,  267
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
MouseClick, left,  132,  77
Sleep, 100
PixelGetColor, color,  753,  179
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  756,  208
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  753,  229
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  759,  270
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  761,  299
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  760,  332
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  756,  358
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  756,  390
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  756,  417
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  754,  449
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  767,  478
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
PixelGetColor, color,  767,  502
Sleep, 100
if color = 0x00FFFF
{
clipboard = False
Exit
}
clipboard = True