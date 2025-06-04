@echo off
rem Tar up an external distribution of wxWindows

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %1
echo   To     %2\wx166_1.tgz, %2\wx166_2.tgz, %2\wx166_3.tgz
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\*.*
cd %1

rem First, expand the wildcards in the rsp files

rem Create empty list file
erase %1\distrib\*.lis
c:\bin\touch %1\distrib\wx166_1.lis
c:\bin\touch %1\distrib\wx166_2.lis
c:\bin\touch %1\distrib\wx166_3.lis

rem Create a .rsp file with backslashes instead
rem of forward slashes
rem No need if using ls2 (from GNU-WIN32 distribution)
rem sed -e "s/\//\\/g" %1\distrib\wx_asc.rsp > %1\distrib\wx_asc.rs2

call %1\distrib\expdwild.bat %1\distrib\wx_asc.rsp %1\distrib\wx166_1.lis
call %1\distrib\expdwild.bat %1\distrib\wx_bin.rsp %1\distrib\wx166_1.lis
call %1\distrib\expdwild.bat %1\distrib\utl3_asc.rsp %1\distrib\wx166_1.lis

call %1\distrib\expdwild.bat %1\distrib\util_asc.rsp %1\distrib\wx166_2.lis
call %1\distrib\expdwild.bat %1\distrib\util_bin.rsp %1\distrib\wx166_2.lis
call %1\distrib\expdwild.bat %1\distrib\utl2_asc.rsp %1\distrib\wx166_2.lis

call %1\distrib\expdwild.bat %1\distrib\smpl_asc.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_bin.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\wxim1asc.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\wxim1bin.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\utl2_bin.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\utl3_bin.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\utl4_asc.rsp %1\distrib\wx166_3.lis
call %1\distrib\expdwild.bat %1\distrib\utl4_bin.rsp %1\distrib\wx166_3.lis

tar -c -T %1\distrib\wx166_1.lis
move archive.tar wx166_1.tar
gzip wx166_1.tar
move wx166_1.taz %2\wx166_1.tgz

tar -c -T %1\distrib\wx166_2.lis
move archive.tar wx166_2.tar
gzip wx166_2.tar
move wx166_2.taz %2\wx166_2.tgz

tar -c -T %1\distrib\wx166_3.lis
move archive.tar wx166_3.tar
gzip wx166_3.tar
move wx166_3.taz %2\wx166_3.tgz

echo wxWindows archived.
goto end

:usage
echo Tar/gzip wxWindows distribution under DOS.
echo Usage: tardist source destination
echo e.g. tardist c:\wx c:\wx\deliver

:end


