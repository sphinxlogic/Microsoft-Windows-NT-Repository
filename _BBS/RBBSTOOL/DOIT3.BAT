type header3 >> %output%\all.dir
..\get_area other 0 %1 < \_bbs\dirs.txt >> %output%\all.dir
echo path: %1 >> %output%\all.dir
type header3 >> %output%\all.dir
type header2 >> %output%\all.dir
cd %cdrom%%1
%cdrom%\_bbs\c %in_type% rbbs %cdrom%%1 < %cdrom%00_index.txt >> %output%\all.dir
cd %cdrom%\_bbs\rbbstool


