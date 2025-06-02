rem %1 == disc directory
rem %indexf% == 00_index.txt
if exist %cdrom%%1\%indexf% goto ok
echo no such file: %cdrom%%1\%indexf% >> logfile
goto end
:ok
if exist %output%\dir%2 goto bad
goto ok2
:bad
echo duplicate output file: %output%\dir%2 >> logfile
goto end
:ok2
type %cdrom%\_bbs\pcbtool\header1 > %output%\dir%2
%cdrom%\_bbs\get_area other 0 %1 < \_bbs\dirs.txt >> %output%\dir%2
echo path: %1 >> %output%\dir%2
type %cdrom%\_bbs\pcbtool\header2 >> %output%\dir%2
rem cd %cdrom%%1
rem %cdrom%\_bbs\c %in_type% pcb %cdrom%%1 < %cdrom%%1\%indexf% >> %output%\dir%2
rem cd %cdrom%\_bbs\pcbtool
cd %cdrom%%1
%cdrom%\_bbs\c %in_type% pcb %cdrom%. < %cdrom%%indexf% >> %output%\dir%2
cd %cdrom%\_bbs\pcbtool
dirlst %output%\dir%2 %cdrom%%1\ %2 >> dirs.lst
:end
