set cdrom=m:
set output=m:\_bbs\pcb
set indexf=00_index.txt

rem you shouldn't have to change anything below this.
set in_type=opus

echo y | del %output%\*.*
del dirs.lst
del logfile
del ..\logfile
del dlpath.lst

%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit
%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit2

set output=
set in_type=
