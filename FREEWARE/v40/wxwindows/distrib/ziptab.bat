@echo off
rem Zip up an external distribution of wxTab
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxTab distribution:
echo   From   %1
echo   To     %2\wxtab.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\wxtab.zip
cd %1
zip -P %3 %4 %5 %6 %7 %8 %2\wxtab.zip @%wxwin\distrib\wxtab.rsp

echo wxTab archived.
goto end

:usage
echo DOS wxTab distribution.
echo Usage: ziptab source destination
echo e.g. ziptab c:\wx\utils c:\wx\utils\wxtab\deliver

:end
