SetTitleMatchMode, 2
IfWinNotExist, WinUSB driver
{
	Run, C:\LiquidScintArray\sis3320_sis3302_sis3320_NeutronGamma_with-SIS3150USB-WINUSB\sis3302_sis3320_NeutronGamma.exe
	WinWait, Load Configuration File, 
	IfWinNotActive, Load Configuration File, , WinActivate, Load Configuration File, 
	WinWaitActive, Load Configuration File, 
	MouseClick, left,  246,  278
	Sleep, 100
	Send, {ENTER}
	Sleep, 100
}
WinWait, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
IfWinNotActive, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, , WinActivate, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
WinWaitActive, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver,
MouseClick, left,  262,  36
Sleep, 100
MouseClick, left,  258,  54
Sleep, 100	

PixelGetColor, startColor, 110, 113
Sleep, 100 
if startColor = 0x00FF00
{
	MouseClick, left,  43,  293
	Sleep, 100
	MouseClick, left,  109,  114
	Sleep, 100
}