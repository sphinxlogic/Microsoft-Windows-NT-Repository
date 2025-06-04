@echo off
rem **** BATCH FILE TO CLEAN WXWIN SAMPLES AND UTILS FOR MS C7/VC++ 1.5 ****

if "%WXWIN" == "" goto err1
if "%1" == "" goto usage

:doit
if "%1" == "" goto end

echo Cleaning %1 using VC++ 1.x or MS C7.

cd %WXWIN\%1
nmake -f makefile.dos clean
shift
goto doit

goto end

:err1
echo vcclean: you must set WXWIN in your autoexec.bat file, and
echo reboot.
goto end

:usage
echo vcclean: specify program directories on the command line, e.g,
echo vcclean samples\hello samples\minimal utils\colour
goto end

:end
pause




