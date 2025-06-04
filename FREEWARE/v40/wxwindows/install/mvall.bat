@echo off
rem 4DOS batch file to rename all wxWindows C++ source files to a different extension.

if "%1" == "" goto usage
if "%2" == "" goto usage
if "%3" == "" goto usage
echo About to rename all %1 files to have extension %2
echo Type CTRL-C to abort.
inkey /W4 `Press any key to continue...` %%input

set old_ext=%1
set new_ext=%2

call mvfiles %new_ext %3\src\base\*.%old_ext
call mvfiles %new_ext %3\src\msw\*.%old_ext
call mvfiles %new_ext %3\src\x\*.%old_ext
call mvfiles %new_ext %3\contrib\wxstring\*.%old_ext
call mvfiles %new_ext %3\contrib\itsybits\*.%old_ext

call mvfiles %new_ext %3\samples\animate\*.%old_ext
call mvfiles %new_ext %3\samples\bombs\*.%old_ext
call mvfiles %new_ext %3\samples\buttnbar\*.%old_ext
call mvfiles %new_ext %3\samples\dialogs\*.%old_ext
call mvfiles %new_ext %3\samples\docviewhello\*.%old_ext
call mvfiles %new_ext %3\samples\form\*.%old_ext
call mvfiles %new_ext %3\samples\fractal\*.%old_ext
call mvfiles %new_ext %3\samples\hello\*.%old_ext
call mvfiles %new_ext %3\samples\ipc\*.%old_ext
call mvfiles %new_ext %3\samples\layout\*.%old_ext
call mvfiles %new_ext %3\samples\listbox\*.%old_ext
call mvfiles %new_ext %3\samples\mdi\*.%old_ext
call mvfiles %new_ext %3\samples\memcheck\*.%old_ext
call mvfiles %new_ext %3\samples\mfc\*.%old_ext
call mvfiles %new_ext %3\samples\minimal\*.%old_ext
call mvfiles %new_ext %3\samples\odbc\*.%old_ext
call mvfiles %new_ext %3\samples\panel\*.%old_ext
call mvfiles %new_ext %3\samples\pressup\*.%old_ext
call mvfiles %new_ext %3\samples\printing\*.%old_ext
call mvfiles %new_ext %3\samples\resource\*.%old_ext
call mvfiles %new_ext %3\samples\static\*.%old_ext
call mvfiles %new_ext %3\samples\toolbar\*.%old_ext
call mvfiles %new_ext %3\samples\types\*.%old_ext
call mvfiles %new_ext %3\samples\vlist\*.%old_ext
call mvfiles %new_ext %3\samples\wxpoem\*.%old_ext

call mvfiles %new_ext %3\utils\colours\*.%old_ext
call mvfiles %new_ext %3\utils\dialoged\src\*.%old_ext
call mvfiles %new_ext %3\utils\dib\*.%old_ext
call mvfiles %new_ext %3\utils\hytext\src\*.%old_ext
call mvfiles %new_ext %3\utils\image\src\*.%old_ext
call mvfiles %new_ext %3\utils\mfutils\src\*.%old_ext
call mvfiles %new_ext %3\utils\prologio\src\*.%old_ext
call mvfiles %new_ext %3\utils\rcparser\src\*.%old_ext
call mvfiles %new_ext %3\utils\tex2rtf\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxbuild\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxchart\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxgraph\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxgrid\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxhelp\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxhelp2\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxprop\src\*.%old_ext
call mvfiles %new_ext %3\utils\wxtree\src\*.%old_ext
call mvfiles %new_ext %3\utils\xpmshow\src\*.%old_ext

:usage
echo Renames all wxWindows C++ source files to give them a different extension.
echo Only works using the 4DOS command processor.
echo Usage:
echo   mvall old_ext new_ext wx_dir
echo Example:
echo   mvall cc cpp c:\wx
:end

