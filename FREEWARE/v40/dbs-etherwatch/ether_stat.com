$! Program Name            : ETHER_STAT.COM
$!   Original Author       : NUCMGR
$!   Date                  : 14-APR-1996 
$!   Program Description   : This is the interface to ETHER_STAT.PL
$!                         : It takes care of checking you have the proper
$! privileges enabled. This command file assumes ETHER_STAT.PL is located
$! in the same directory. Running it without argument will display the
$! usage summary.
$! No need to say you need to have Perl installed on your machine  ...
$!
$
$ INSTALL_DIR   = F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,,"NO_CONCEAL")-"]["
$ INSTALL_DIR   = F$EXTRACT(0,F$LOCATE("]",INSTALL_DIR)+1,INSTALL_DIR)
$ IF P1.EQS.""
$ THEN
$  perl 'INSTALL_DIR'ETHER_STAT.PL 
$ ELSE
$  ! Test privilege
$  chgprvsts = f$setprv("PHY_IO")
$  if f$privilege("PHY_IO").eqs."FALSE" then exit 34
$
$  CPRIO = F$GETJPI("","AUTHPRI")	
$  PRIO = f$int(CPRIO)-1
$  write sys$output "Running at priority = ''PRIO'"
$  set process/priority='PRIO'
$  on CONTROL_Y then GOTO EXIT
$  perl 'INSTALL_DIR'ETHER_STAT.PL 'P1' 'P2' 'P3'
$ EXIT:
$  if chgprvsts.nes."" then  set process/priv=('chgprvsts') 
$  set process/priority='CPRIO'
$  exit
$ ENDIF
