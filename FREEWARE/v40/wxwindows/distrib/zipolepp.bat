@echo off
rem Zip up an external distribution of OLEPP.

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external OLEPP distribution:
echo   From   %1
echo   To     %2\olepp.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\olepp.zip
cd %1
zip -a -P %3 %4 %5 %6 %7 %8 %2\olepp.zip @%wxwin\distrib\olepp.rsp

echo OLEPP archived.
goto end

:usage
echo DOS OLEPP distribution.
echo Usage: zipolepp source destination
echo e.g. zipolepp c:\wx\utils c:\wx\utils\olepp\deliver

:end



