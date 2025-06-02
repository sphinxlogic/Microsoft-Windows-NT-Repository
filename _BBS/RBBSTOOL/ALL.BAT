rem this is where the actual files live
set cdrom=m:
rem this is where you want your index files
set output=\_bbs\rbbs
set index_f=00_index.txt


set in_type=opus
del logfile
del ..\logfile
echo y | del %output%\*.*

%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit2

type header1 > %output%\all.dir
%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit3

type makefidx.1 > makefidx.cfg
%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit4
type makefidx.2 >> makefidx.cfg

set cdrom=
set output=
set in_type=

call makeffs
