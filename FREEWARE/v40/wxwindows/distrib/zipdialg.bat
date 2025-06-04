@echo off
rem Zip up an external distribution of Dialog Editor
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external Dialog Editor distribution:
echo   From   %1
echo   To     %2\dialoged.zip
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\dialoged.zip %2\source.zip
cd %1
zip -P %3 %4 %5 %6 %7 %8 %2\source.zip @%wxwin\distrib\dialoged.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\source.zip @%wxwin\distrib\wxprop.rsp

zip -d %2\source.zip dialoged\docs\dialoged.hlp
zip -d %2\source.zip dialoged\docs\dialoged.ps

copy %wxwin\utils\winstall\bin\install.exe %2
copy %1\dialoged\src\install.inf %2
copy %1\dialoged\samples\sample1.wxr %2

cd %2

compress %1\dialoged\src\dialoged.exe %2\dialoged.ex$
compress %1\dialoged\docs\dialoged.hlp %2\dialoged.hl$
copy %1\dialoged\docs\dialoged.cnt %2

compress c:\windows\system\ctl3dv2.dll %2\ctl3dv2.dl$
compress c:\windows\system\ctl3d32.dll %2\ctl3d32.dl$

zip dialoged.zip *.*

echo Dialog Editor archived.
goto end

:usage
echo DOS Dialog Editor distribution.
echo Usage: zipdialg source destination
echo e.g. zipdialg c:\wx\utils c:\wx\utils\dialoged\deliver

:end
