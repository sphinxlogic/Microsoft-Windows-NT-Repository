$ on warning   then goto error_trap
$ on control_y then exit
$!
$!************************
$! Initialize
$!************************
$ set process/privilege=(noall, netmbx, tmpmbx, sysprv, world)
$ set process/name="IUFINGERD"
$!
$ define sys$scratch sys$nowhere:[nobody]
$!
$!************************
$! Start the Server
$!************************
$ set command sys$nowhere:[nobody]iufingerd.cld
$ iufingerd 
$ goto final_exit
$!
$!************************
$! Error Trap
$!************************
$ error_trap:
$ ss_status = $status
$ on warning then continue
$ cant_bind = %x00038004
$ if (ss_status .eq. cant_bind) then goto final_exit
$ error_message = f$message(ss_status)
$!
$ node = f$getsyi("NODENAME")
$ mail SYS$INPUT SYSTEM /subject="''node' - ''error_message'"
An IUFINGERD server has experienced a runtime error.  The node and reason
for the error should appear on the subject line of this message.

Please investigate this problem as quickly as possible.
Thank you.
$!
$!************************
$! Final Exit
$!************************
$ final_exit:
$ exit
