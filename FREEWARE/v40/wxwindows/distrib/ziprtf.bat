@echo off
rem Zip up wxWindows RTF files
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive wxWindows Word RTF files
echo   From   %1
echo   To     %2\wx166rtf.zip
echo CTRL-C if this is not correct.
pause

erase %2\wx166rtf.zip

cd %1
echo Zipping...
zip -P %3 %4 %5 %6 %7 %8 %2\wx166rtf.zip  @%1\distrib\wx_rtf.rsp

cd %2

echo wxWindows RTF documents archived.
goto end

:usage
echo wxWindows RTF documents distribution.
echo Usage: ziprtf source destination
echo e.g. ziprtf c:\wx b:

:end
