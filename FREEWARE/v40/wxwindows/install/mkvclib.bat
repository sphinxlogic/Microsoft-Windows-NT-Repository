@echo off
rem **** BATCH FILE TO MAKE WX.LIB FOR MS C7/VC++ 1.5 ****

if "%WXWIN" == "" goto err1
echo Making wx.lib using VC++ 1.x or MS C7.
echo CTRL-C to abort.
pause

cd %WXWIN\src\msw
nmake -f makefile.dos
goto end


:err1
echo mkvclib: you must set WXWIN in your autoexec.bat file, and
echo reboot.

:end
pause


