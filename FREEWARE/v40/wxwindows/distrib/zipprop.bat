@echo off
rem Zip up an external distribution of wxProp.

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxCLIPS distribution:
echo   From   %1
echo   To     %2\wxprop.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\wxprop.zip
cd %1
zip -a -P %3 %4 %5 %6 %7 %8 %2\wxprop.zip @%wxwin\distrib\wxprop.rsp

echo wxProp archived.
goto end

:usage
echo DOS wxProp distribution.
echo Usage: zipprop source destination
echo e.g. zipprop c:\wx\utils c:\wx\utils\wxprop\deliver

:end



