$!  Command procedure to compile PASLIB.
$
$ say "Creating cumulative source file PASLIB.SRC ..."
$ create /log paslib.src			! cumulative source list
$ append paslib.pas, -
arbdef.pas, -
pasdef.pas, -
sysdef.pas,uafdef.pas    paslib.src
$
$ on error  then  exit
$ say "Compiling PASLIB ..."
$ pascal paslib /noobject /check=all /usage=all
$
$ say "Transfering files to SYS_LIBRARY: ..."
$ copy   PASLIB.PEN,PASLIB.SRC SYS_LIBRARY: /PROTECT=(S:RWED,O:RWED,G,W:R) /LOG
$ delete PASLIB.PEN;*, PASLIB.SRC;* /LOG
$ purge  SYS_LIBRARY:PASLIB.* /LOG
$
$ exit
