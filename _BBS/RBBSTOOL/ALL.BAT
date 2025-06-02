rem this is where the actual files live
set cdrom=m:
rem this is where you want your index files
set output=\_bbs\rbbs


set in_type=cica
del logfile
del ..\logfile
del %output%\*.*

%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit2

type header1 > %output%\all.dir
%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit3

set cdrom=
set output=
set in_type=

call makeffs
