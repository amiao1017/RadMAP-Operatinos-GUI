SetTitleMatchMode, 2
IfWinExist, WinUSB driver
{
	IfWinNotActive, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, , WinActivate, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
	WinWaitActive, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
	PixelGetColor, systemColor,  1149,  132
	Sleep, 100
	PixelGetColor, startColor, 110, 113
	Sleep, 100
	if systemColor = 0x20FF20
	{
		if startColor = 0x00FF00
		{
			clipboard = False
		}
	}
	if systemColor = 0xFF2020
	{
		if startColor = 0x0000FF
		{
			clipboard = True
		}
	}
}
else
{
clipboard = False
}