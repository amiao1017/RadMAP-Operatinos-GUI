@echo off
set MISTI_CONFIG="C:\cygwin\home\misti\misti\src\mistibug_truck\cfg\misti20130220.cfg"
ladybugCapture %MISTI_CONFIG% 0 %1
if [%2] == [] goto end
if %2 == 0 goto end
pause
exit
:end