rem %1 == disc directory
rem %3 == file name
if exist %cdrom%%1\%index_f% goto ok
echo no such file: %cdrom%%1\%index_f% >> logfile
goto end
:ok
cd %cdrom%%1
%cdrom%\_bbs\c %in_type% wildcat %cdrom%. < %cdrom%%index_f% >> %output%\allfiles.txt
cd \_bbs\wc30
:end
