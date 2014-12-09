SetTitleMatchMode 2
IfWinExist, Demo Application SIS3302
{
  WinActivate
}
WinWait, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
IfWinNotActive, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, , WinActivate, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
WinWaitActive, Demo Application SIS3302 / SIS3320 / SIS3320-250 ADC  Neutron/Gamma  - 4. June 2011 - SIS3150USB-WinUSB driver, 
MouseClick, left,  110,  131
Sleep, 100
Send, {SHIFTDOWN}{F4}{SHIFTUP}{SHIFTDOWN}{F4}{SHIFTUP}{ENTER}