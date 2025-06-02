rem %1 == input directory name
cd %cdrom%%1
type %cdrom%\_bbs\opus\header1 > %cdrom%files.bbs
%cdrom%\_bbs\get_area other 1 %1 < \_bbs\dirs.txt >> %cdrom%files.bbs
echo  path: %1 >> %cdrom%files.bbs
type %cdrom%\_bbs\opus\header2 >> %cdrom%files.bbs
%cdrom%\_bbs\c %in_type% opus < %cdrom%%index_f% >> %cdrom%files.bbs
%cdrom%\_bbs\c %in_type% 4dos < %cdrom%%index_f% > %cdrom%descript.ion
cd %cdrom%\_bbs\opus

