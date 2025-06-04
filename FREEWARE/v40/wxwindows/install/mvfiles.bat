@echo off
rem 4DOS batch file to rename C++ source files to a different extension.

if "%1" == "" goto usage
if "%2" == "" goto usage
rem echo About to rename files to have extension %1
rem echo Type CTRL-C to abort.
rem inkey /W4 `Press any key to continue...` %%input

set ext=%1
:label
if "%2" == "" goto end

ren %2 %@PATH[%2]%@NAME[%2].%ext
shift
goto label

:usage
echo Renames C++ source files to give them a different extension.
echo Only works using the 4DOS command processor.
echo Usage:
echo   mvfiles new_ext files
echo Example:
echo   mvfiles cpp file1.cc file2.cc
:end

