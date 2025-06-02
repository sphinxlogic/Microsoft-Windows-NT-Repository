set cdrom=m:
set output=m:\_bbs\wc30
set in_type=cica

del logfile
del %output%\allfiles.txt

%cdrom%\_bbs\each_dir 1    %cdrom%\_bbs\dirs.txt   doit2
%cdrom%\_bbs\each_dir 1    %cdrom%\_bbs\dirs.txt   doit

set in_type=
set cdrom=
