$!  Command procedure to compile PASLIB.  Make P1 anything to enable PP's.
$!  (PP is a Pascal source code pre-processor developed by Jonathan Ridler.)
$!  (PP may be found in D_SP:[JONATHAN.PP])
$
$!  When adding or removing modules, be sure to change the PP entry and the
$!  APPEND entry below.
$
$ if p1 .eqs. ""  then  goto  JUST_COMPILE
$
$ pp acldef
$ ws ""
$ pp arbdef
$ ws ""
$ pp clidef
$ ws ""
$ pp convdef
$ ws ""
$ pp dcxdef
$ ws ""
$ pp edtdef
$ ws ""
$ pp fdldef
$ ws ""
$ pp fildef
$ ws ""
$ pp lbrdef
$ ws ""
$ pp nmadef
$ ws ""
$ pp pasdef
$ ws ""
$ pp psmdef
$ ws ""
$ pp smbdef
$ ws ""
$ pp statedef
$ ws ""
$ pp sysdef
$ ws ""
$ pp tpudef
$ ws ""
$ pp uafdef
$ ws ""
$ pp paslib
$ ws ""
$
$JUST_COMPILE:
$
$ ws "Creating cumulative source file PASLIB.SRC ..."
$ create /log paslib.src			! cumulative source list
$ append paslib.pas, -
acldef.pas,arbdef.pas,clidef.pas,convdef.pas,dcxdef.pas, -
edtdef.pas,fdldef.pas,lbrdef.pas, -
pasdef.pas,psmdef.pas,smbdef.pas, -
sysdef.pas,tpudef.pas,uafdef.pas    paslib.src
$
$ on error  then  exit
$ ws "Compiling PASLIB ..."
$ pascal paslib /noobject /check=all /usage=all
$
$ ws "Transfering files to SYS_LIBRARY: ..."
$ copy   PASLIB.PEN,PASLIB.SRC SYS_LIBRARY: /PROTECT=(S:RWED,O:RWED,G,W:R) /LOG
$ delete PASLIB.PEN;*, PASLIB.SRC;* /LOG
$ purge  SYS_LIBRARY:PASLIB.* /LOG
$
$ exit
