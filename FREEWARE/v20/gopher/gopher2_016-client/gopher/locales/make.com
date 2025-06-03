$ vg = 'f$verify(0)'
$!********************************************************************
$! wilkinson
$! 1.1
$! 1994/05/31
$!
$! J.Lance Wilkinson, Penn State University C&IS - LCS.
$!********************************************************************
$! MODULE: message.com
$! compiling script for VMS "internationalized" messages
$!*********************************************************************
$! Revision History:
$! message.com,v
$! Revision 1.1  1994/5/31  wilkinson
$! 
$! Initial revision
$! Revision 1.2  1994/7/12  wilkinson
$! Attempted AXP support
$! Revision 1.3  1994/7/13  wilkinson/macrides
$! Firmed up AXP support
$! Revision 1.4  1994/7/14  wilkinson
$! Made MAKE.COM search out all .msg files and process them rather than hard-
$!	coding specific .msg files.
$!
$!********************************************************************/
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ vg1 = f$verify(1)
$!
$!	Preprocess the .MSG files into VMS message files
$!
$!
$ vg1 = 'f$verify(0)'
$ if f$getsyi("HW_MODEL").ge.1024
$ then
$   vg1 = f$verify(1)
$   preprocess := "edit/tpu/nosection/nodisplay/command=message.tpu/init=AXP"
$   vg1 = 'f$verify(0)'
$ else
$   vg1 = f$verify(1)
$   preprocess := "edit/tpu/nosection/nodisplay/command=message.tpu/init=VAX"
$   vg1 = 'f$verify(0)'
$ endif
$
$ prep_loop:
$   file = f$search("*.msg")
$   if file .eqs. "" then goto done_prep
$   filename := 'f$parse(file,,,"name")
$   vg1 = f$verify(1)
$   preprocess/output='filename.vms$msg  'filename.msg
$   vg1 = 'f$verify(0)'
$   goto prep_loop
$ done_prep:
$ purge/nolog *.vms$msg
$ purge/nolog *.opt
$ vg1 = f$verify(1)
$!
$!	Compile the resultant .VMS$MSG files into VMS message files
$!		Normally, one would use the /nosymbols option.  But
$!		in order for the symbol GOPHER_LANG_* to be defined
$!		in the sharable image, /nosymbols can't be used.
$!
$ vg1 = 'f$verify(0)'
$ msgs_loop:
$   file = f$search("*.vms$msg")
$   if file .eqs. "" then goto done_msgs
$   filename := 'f$parse(file,,,"name")
$   vg1 = f$verify(1)
$   message 'filename.vms$msg
$   vg1 = 'f$verify(0)'
$   goto msgs_loop
$ done_msgs:
$ purge/nolog *.obj
$ vg1 = f$verify(1)
$!
$!	Create non-executable message files
$!
$ vg1 = 'f$verify(0)'
$ link_loop:
$   file = f$search("*.obj")
$   if file .eqs. "" then goto done_link
$   filename := 'f$parse(file,,,"name")
$   vg1 = f$verify(1)
$   link/sharable=gopher_msg_'filename.exe 'filename.obj, 'filename.opt/opt
$   vg1 = 'f$verify(0)'
$   goto link_loop
$ done_link:
$ purge/nolog gopher_msg_*.exe
$ delete/nolog *.obj;*
$ delete/nolog *.vms$msg;*
$ delete/nolog *.opt;*
$!
$ vg1 = 'f$verify(0)'
$ CLEANUP:
$    vg1 = f$verify(vg)
$exit
