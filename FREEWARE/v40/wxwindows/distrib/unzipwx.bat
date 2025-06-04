@echo off
if "%1" == "" goto usage
if "%2" == "" goto usage
echo Installing DOS wxWindows distribution from %1 to %2
echo This may make subdirectories below %2.
echo CTRL-C if this is wrong.
pause

echo Unzipping...
unzip -o -d %1\wxwinasc.zip %2
unzip -o -d %1\wxwinbin.zip %2
echo Resetting archive bits...
attrib /Q /S -A %2\*.*

echo wxWindows unzipped.
goto end

:usage
echo Unzipwx -- unzip a distribution of wxWindows
echo Usage: unzipwx source destination
echo e.g. unzipwx a: c:\c\wx

:end
