@echo off
if "%1" == "" goto end
echo Checking file %1...
gawk -f check.awk %1
quit
:end
echo Tex2RTF input checker.
echo Usage: tcheck file
