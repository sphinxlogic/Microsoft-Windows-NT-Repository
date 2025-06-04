@echo off
rem Zip up a distribution of wxWindows
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive DOS wxWindows distribution:
echo   From   %1
echo   To     %2\wxwinasc.zip, %2\wxwinbin.zip
echo CTRL-C if this is not correct.
pause

erase %2\wxwinasc.zip
erase %2\wxwinbin.zip
cd %1
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\wx_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\util_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\utl2_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\utl3_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\utl4_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\smpl_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\aiai_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\xtra_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\xtr2_asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\wxim1asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\wxim2asc.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinasc.zip @%1\distrib\wx_xlp.rsp

zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\wx_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\util_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\utl2_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\utl3_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\utl4_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\smpl_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\aiai_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\xtra_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\xtr2_bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\wxim1bin.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\wx_ps.rsp
zip -P -i- %3 %4 %5 %6 %7 %8 %2\wxwinbin.zip @%1\distrib\wx_hlp.rsp

echo wxWindows archived.
goto end

:usage
echo DOS wxWindows distribution copying to floppy.
echo Usage: zipwx source destination
echo e.g. zipwx d:\wx c:\linux

:end
