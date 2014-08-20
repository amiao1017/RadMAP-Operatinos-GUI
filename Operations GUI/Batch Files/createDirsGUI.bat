@echo off
set year=%date:~-4,4%
set month=%date:~-10,2%
set day=%date:~-7,2%
set hourMilitary=%time:~-11,2%

::::: identify hours in Civilian time, and identify AM or PM
for /f "tokens=1-2*" %%t in ('time /t') do (
set twelvehourtime=%%t
set AMPM=%%u
)

set hourCivilian=%twelvehourtime:~0,2%

::::: I like my hours double digit
if "%AMPM%" == "AM" set hourMilitary=%hourCivilian%

set minute=%time:~-8,2%
set second=%time:~-5,2%
set millisecond=%time:~-2,2%

::::: identify Name of the day and provide short date for today
for /f "tokens=1-2*" %%v in ('date /t') do (
set dayName=%%v
set todaysdate=%%w
)

set runName=Run_%year%%month%%day%_%hourMilitary%%minute%%second%
md %runName%
cd %runName%
md ladybug
md ladybug\port
md ladybug\starboard
md lidar
md lidar\port
md lidar\starboard
md time_sync_files
md gps