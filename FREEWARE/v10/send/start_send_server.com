$  sv = 'f$verify(0)
$!++
$! File:	START_SEND_SERVER.COM
$! Author:	Darrell Burkhead
$!		COPYRIGHT © 1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
$! Date:	June 23, 1994
$!
$! Abstract:
$!
$!	Create the SEND server process, which is a multi-threaded DECnet
$!	server for the SEND_SERVER object.  You will need to create a DECnet
$!	object called SEND_SERVER.
$!
$!		$ RUN SYS$SYSTEM:NCP
$!		NCP> DEFINE OBJECT SEND_SERVER NUMBER 0
$!		NCP> SET OBJECT SEND_SERVER NUMBER 0
$!
$! Revision History:
$!
$!	V1.0		Darrell Burkhead	23-JUN-1994 14:11
$!		Creation.
$!-- 
$  required_privs = "CMKRNL,DETACH"
$  old_privs = f$setprv(required_privs)
$  if .not. f$privilege(required_privs) then goto done
$  server = f$search("MADGOAT_EXE:SEND_SERVER.EXE")
$  if server .nes. "" then -
	run/detach/uic=[1,4]/priv=(oper,netmbx,sysnam,world)-
	/proc="SEND Server" 'server'
$ done: 
$  old_privs = f$setprv(old_privs)
$  exit 1.or.f$verify(sv)
