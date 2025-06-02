rem %1 == disc directory
rem %2 == file name
type header3 >> %output%\sffiles.%2
%cdrom%\_bbs\get_area other 0 %1 < %cdrom%\_bbs\dirs.txt >> %output%\sffiles.%2
echo path: %1 >> %output%\sffiles.%2
type header3 >> %output%\sffiles.%2
type %cdrom%\_bbs\sftool\header2 >> %output%\sffiles.%2
cd %cdrom%%1
%cdrom%\_bbs\c %in_type% spitfire %cdrom%%1 < %cdrom%00_index.txt >> %output%\sffiles.%2
cd %cdrom%\_bbs\sftool

