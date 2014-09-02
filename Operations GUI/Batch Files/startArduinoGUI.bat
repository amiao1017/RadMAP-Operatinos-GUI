@echo off
arduinoCapture 3 %1
if [%2] == [] goto end
if %2 == 0 goto end
pause
exit
:end