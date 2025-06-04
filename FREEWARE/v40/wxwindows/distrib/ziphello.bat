@echo off
if "%1" == "" goto usage

echo Zipping up an external distribution of the hello demo...
echo   To     %1\demo.zip
echo CTRL-C if this is not correct.
pause

erase %2\demo.zip
cd %wxwin\samples\hello
echo Zipping...
zip -P %3 %4 %5 %6 %7 %8 %1\demo.zip @%wxwin\distrib\hello.rsp
zip -a %1\demo.zip c:\windows\system\ctl3d.dll
echo Demo archived.
goto end

:usage
echo Zip up hello demo.
echo Usage: ziphello destination
echo e.g. ziphello c:\wx\deliver

:end

