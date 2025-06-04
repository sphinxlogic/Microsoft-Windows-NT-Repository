@echo off
rem Zip up an external distribution of wxGrid
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxGrid distribution:
echo   From   %1
echo   To     %2\wxgrid.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\wxgrid.zip
cd %1
zip -P %3 %4 %5 %6 %7 %8 %2\wxgrid.zip @%wxwin\distrib\wxgrid.rsp

echo wxGrid archived.
goto end

:usage
echo DOS wxGrid distribution.
echo Usage: zipgrid source destination
echo e.g. zipgrid c:\wx\utils c:\wx\utils\wxgrid\deliver

:end
