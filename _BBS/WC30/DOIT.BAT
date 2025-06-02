rem %1 == disc directory
rem %3 == file name
if exist %cdrom%%1\00_index.txt goto ok
echo no such file: %cdrom%%1\00_index.txt >> logfile
goto end
:ok
cd %cdrom%%1
%cdrom%\_bbs\c %in_type% wildcat %cdrom%%1 < %cdrom%00_index.txt >> %output%\allfiles.txt
cd \_bbs\wc30
:end
