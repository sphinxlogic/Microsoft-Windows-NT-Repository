rem %1 == input directory name
type %cdrom%\_bbs\opus\header1 > %cdrom%%1\files.bbs
%cdrom%\_bbs\get_area other 1 %1 < \_bbs\dirs.txt >> %cdrom%%1\files.bbs
echo  path: %1 >> %cdrom%%1\files.bbs
type %cdrom%\_bbs\opus\header2 >> %cdrom%%1\files.bbs
cd %cdrom%%1
%cdrom%\_bbs\c %in_type% opus < %cdrom%00_index.txt >> %cdrom%files.bbs
del %cdrom%descript.ion
%cdrom%\_bbs\c %in_type% 4dos < %cdrom%00_index.txt >> %cdrom%descript.ion
cd %cdrom%\_bbs\opus

