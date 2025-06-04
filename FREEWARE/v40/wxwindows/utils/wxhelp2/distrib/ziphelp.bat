@echo off
rem Zip up an external distribution of wxHelp 2
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxHelp 2 distribution:
echo   From   %1
echo   To     %2\wxhelp20.zip
echo CTRL-C if this is not correct.
pause

erase %2\*.*
copy %wxwin\utils\winstall\bin\install.exe %2
copy %1\wxhelp2\distrib\install.inf %2
copy %1\wxhelp2\docs\wxhelp.cnt %2
copy c:\bin\unzip.exe %2\pkunzip.exe

compress c:\windows\system\ctl3dv2.dll %2\ctl3dv2.dl$
compress c:\windows\system\ctl3d32.dll %2\ctl3d32.dl$
compress %1\wxhelp2\docs\wxhelp.hlp %2\wxhelp.hl$
compress %1\wxhelp2\src\wxhelp.exe %2\wxhelp.ex$

cd %2
cd %1

zip -P %3 %4 %5 %6 %7 %8 %2\sources.zip @%1\wxhelp2\distrib\wxhelp.rsp
zip -d %2\sources.zip wxhelp2\docs\wxhelp.hlp wxhelp2\docs\wxhelp.xlp wxhelp2\docs\wxhelp.ps

cd %1\wxhelp2\samples
zip -P -r %2\samples.zip *.*
cd %2
zip wxhelp20.zip *.*

echo wxHelp 2 archived.
goto end

:usage
echo DOS wxHelp 2 distribution.
echo Usage: ziphelp source destination
echo e.g. zipt2rtf c:\wx\utils c:\wx\utils\wxhelp\deliver

:end
