@echo off
rem Tar up an external distribution of wxWindows: but
rem putting in separate ASCII and binary files
rem This seems to be the one that works, using
rem separate tar programs for conversion/non-conversion
rem of ASCII/binary files.

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %1
echo   To     %2\wx167_1.tgz, %2\wx167_2.tgz, %2\wx167hlp.tgz, %2\wx167ps.tgz, %2\wx167htm.tgz
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\*.tgz
cd %1

rem First, expand the wildcards in the rsp files

rem Create empty list file
erase %1\distrib\*.lis
c:\bin\touch %1\distrib\wx167asc.lis
c:\bin\touch %1\distrib\wx167bin.lis
c:\bin\touch %1\distrib\wx167hlp.lis
c:\bin\touch %1\distrib\wx167ps.lis
c:\bin\touch %1\distrib\wx167xlp.lis

rem Create a .rsp file with backslashes instead
rem of forward slashes
rem No need if using ls2 (from UNIX95 distribution)
rem sed -e "s/\//\\/g" %1\distrib\wx_asc.rsp > %1\distrib\wx_asc.rs2

call %1\distrib\expdwild.bat %1\distrib\wx_asc.rsp %1\distrib\wx167asc.lis
call %1\distrib\expdwild.bat %1\distrib\util_asc.rsp %1\distrib\wx167asc.lis
call %1\distrib\expdwild.bat %1\distrib\utl2_asc.rsp %1\distrib\wx167asc.lis
call %1\distrib\expdwild.bat %1\distrib\utl3_asc.rsp %1\distrib\wx167asc.lis
call %1\distrib\expdwild.bat %1\distrib\utl4_asc.rsp %1\distrib\wx167asc.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_asc.rsp %1\distrib\wx167asc.lis
call %1\distrib\expdwild.bat %1\distrib\wxim1asc.rsp %1\distrib\wx167asc.lis

call %1\distrib\expdwild.bat %1\distrib\wx_bin.rsp %1\distrib\wx167bin.lis
call %1\distrib\expdwild.bat %1\distrib\util_bin.rsp %1\distrib\wx167bin.lis
call %1\distrib\expdwild.bat %1\distrib\utl2_bin.rsp %1\distrib\wx167bin.lis
call %1\distrib\expdwild.bat %1\distrib\utl3_bin.rsp %1\distrib\wx167bin.lis
call %1\distrib\expdwild.bat %1\distrib\utl4_bin.rsp %1\distrib\wx167bin.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_bin.rsp %1\distrib\wx167bin.lis
call %1\distrib\expdwild.bat %1\distrib\wxim1bin.rsp %1\distrib\wx167bin.lis

rem Docs
call %1\distrib\expdwild.bat %1\distrib\wx_hlp.rsp %1\distrib\wx167hlp.lis
call %1\distrib\expdwild.bat %1\distrib\wx_ps.rsp %1\distrib\wx167ps.lis
call %1\distrib\expdwild.bat %1\distrib\wx_html.rsp %1\distrib\wx167htm.lis

rem Do some further massaging of the .lis files
sed -e "s/\\/\//g" %1\distrib\wx167asc.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx167asc.lis

sed -e "s/\\/\//g" %1\distrib\wx167bin.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx167bin.lis

sed -e "s/\\/\//g" %1\distrib\wx167hlp.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx167hlp.lis

sed -e "s/\\/\//g" %1\distrib\wx167ps.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx167ps.lis

sed -e "s/\\/\//g" %1\distrib\wx167htm.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx167htm.lis

rem 'tar' converts linefeeds.
tar -c -T %1\distrib\wx167asc.lis -f %2\wx167.tar
rem pause Press a key to continue.

rem This converts to lower case
ren %2\wx167.tar %2\wx167_1.tar
gzip32 %2\wx167_1.tar
ren %2\wx167_1.tar.gz %2\wx167_1.tgz

rem No linefeed conversion wanted
rem Note: GNU tar seems to crash with a full destination path, so
rem pander to it.
targnu -c -T %1\distrib\wx167bin.lis -f wx167_2.tar
move wx167_2.tar %2
gzip32 %2\wx167_2.tar
ren %2\wx167_2.tar.gz %2\wx167_2.tgz

targnu -c -T %1\distrib\wx167hlp.lis -f wx167_hlp.tar
move wx167_hlp.tar %2
gzip32 %2\wx167_hlp.tar
ren %2\wx167_hlp.tar.gz %2\wx167hlp.tgz

tar -c -T %1\distrib\wx167ps.lis -f %2\wx167ps.tar
gzip32 %2\wx167ps.tar
ren %2\wx167ps.tar.gz %2\wx167ps.tgz

targnu -c -T %1\distrib\wx167htm.lis -f wx167htm.tar
move wx167htm.tar %2
gzip32 %2\wx167htm.tar
ren %2\wx167htm.tar.gz %2\wx167htm.tgz

cd %2
echo wxWindows archived.
goto end

:usage
echo Tar/gzip wxWindows distribution under DOS, making an ASCII and binary file
echo Usage: tardist2 source destination
echo e.g. tardist2 d:\wx d:\wx\deliver

:end


