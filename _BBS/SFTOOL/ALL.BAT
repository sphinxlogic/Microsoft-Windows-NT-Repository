set cdrom=m:
set output=\_bbs\sf

rem you shouldn't need to change anything below this.

set in_type=cica

del %output%\*.*

%cdrom%\_bbs\each_dir 1    %cdrom%\_bbs\dirs.txt   doit1
%cdrom%\_bbs\each_dir 1    %cdrom%\_bbs\dirs.txt   doit

set cdrom=
set output=
set in_type=
