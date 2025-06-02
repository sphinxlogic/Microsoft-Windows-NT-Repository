set cdrom=m:
set output=m:\_bbs\pcb

rem you shouldn't have to change anything below this.
set in_type=cica

echo y | del %output%\*.*
del dirs.lst
del logfile
del ..\logfile

%cdrom%\_bbs\each_dir 1    \_bbs\dirs.txt   doit

set output=
set in_type=
