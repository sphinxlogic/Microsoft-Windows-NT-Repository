@echo off
rem **** BATCH FILE TO CLEAN WXWIN *SAMPLES* FOR MS C7/VC++ 1.5 ****

if "%WXWIN" == "" goto err1
if "%1" == "" goto usage

:doit
if "%1" == "" goto end

echo Cleaning sample %1 using VC++ 1.x or MS C7.

cd %WXWIN\samples\%1
nmake -f makefile.dos clean
shift
goto doit

goto end

:err1
echo vcclsmpl: you must set WXWIN in your autoexec.bat file, and
echo reboot.
goto end

:usage
echo vcclsmpl: specify program directories on the command line, e.g,
echo vcclsmpl hello minimal fractal
goto end

:end
pause




