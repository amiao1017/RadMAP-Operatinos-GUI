@echo off
velodyneCapture 1 %MISTI_CONFIG% 1 %1
if [%2] == [] goto end
if %2 == 0 goto end
pause
exit
:end