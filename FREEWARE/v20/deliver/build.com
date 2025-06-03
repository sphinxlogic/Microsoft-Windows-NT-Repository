$ ! BUILD.COM - Command file to build the DELIVER_MAILSHR image
$ ! Written by Ned Freed, 15-Oct-1985
$ !
$ message deliver_err
$ pascal deliver
$ CPUTYPE = F$GETSYI("CPU")
$ IF (CPUTYPE .GE. 128) THEN GOTO IS_AXP
$IS_VAX:
$ macro mailshr
$ link/share=deliver_mailshr.exe/notrace deliver,deliver_err,mailshr,-
  mailshr/opt
$ goto END
$IS_AXP:
$ macro/migration mailshr_axp
$ link/share=deliver_mailshr.exe/notrace deliver,deliver_err,mailshr_axp,-
  mailshr_axp/opt
$END:
