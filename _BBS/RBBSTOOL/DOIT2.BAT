rem %1 == disc directory
type %cdrom%\_bbs\rbbstool\header1 >> %output%\%2.dir
type %cdrom%\_bbs\rbbstool\header3 >> %output%\%2.dir
%cdrom%\_bbs\get_area other 0 %1 < %cdrom%\_bbs\dirs.txt >> %output%\%2.dir
echo path: %1  >> %output%\%2.dir
type %cdrom%\_bbs\rbbstool\header2 >> %output%\%2.dir
cd %1
%cdrom%\_bbs\c %in_type% rbbs %1 < %index_f% >> %output%\%2.dir
cd \_bbs\rbbstool


