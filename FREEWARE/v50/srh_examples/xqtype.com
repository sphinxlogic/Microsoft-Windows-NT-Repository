$!  XQTYPE.COM
$!
$   Set NoOn
$   Set NoVerif
$   vers = "0000|XQTYPE"
$   Goto 'F$Mode()'
$!
$!  ©1989 Digital Equipment Corporation
$!  Written: S Hoffman, 21-Jan-1989
$!
$!  Uses DCL DECnet task-to-task to check the device type of the XQA0:
$!  controller on the remote system.  Can return <unknown>, <none>,
$!  <DELQA> or <DEQNA>.  Requires VMS V5.0 or better as the procedure
$!  uses block-structured IF DCL syntax.
$!
$!  Calling:
$!	@XQTYPE node
$!
$!  Procedure must be named XQTYPE.COM and must reside in the default
$!  login directory on both the local and the remote nodes.  Note that
$!  the remote login may occur either in the directory specified by a
$!  proxy, if one exists, or in the DECNET directory.  If you're not
$!  sure the default directory can be found by entering the command
$!  "DIRECTORY node::".  Requires access to the DECNET TASK object on
$!  the remote end; either by the default DECNET username/password or
$!  via a proxy.
$!
$INTERACTIVE:
$DO_P1:
$   p1 = F$Edit(p1,"COLLAPSE,UPCASE") - "::"
$   If F$Leng(p1) .eq. 0
$   Then
$	msg = "Error reading input"
$	Read/Prompt="Node? "/Error=DONE Sys$Command p1
$	Goto DO_P1
$   EndIf
$!
$   msg = "Error creating network connection"
$   Open/Read/Write/Error=DONE net 'p1'::"task=XQTYPE"
$   Read net remvers
$   Write net vers
$   Read net XQType
$   Close net
$   Write Sys$Output "XQA0: Device type on node ''p1':: is ''XQType'"
$   msg = ""
$   Goto DONE
$
$NETWORK:
$!  remove the colons to work around a FT bug in DECwindows
$   Show Process/All
$   msg = "Error receiving network connection"
$   Open/Read/Write/Error=DONE net Sys$Net
$   Write net vers
$   Read net remvers
$   If F$GetDVI("XQA0:","EXISTS")
$   Then
$	DEVType = F$GetDVI("XQA0:","DEVTYPE")
$	XQType = "<unknown>"
$	If DEVType .eq. 33 Then XQType = "DELQA"
$	If DEVType .eq. 22 Then XQType = "DEQNA"
$   Else
$	XQType = "<none>"
$   EndIf
$   Write net XQType
$   Close net
$
$BATCH:
$OTHER:
$   msg = "Unsupported F$MODE()"
$
$DONE:
$   If F$Len(msg) .ne. 0 Then Write Sys$Output msg
$   Stop
$   Exit
