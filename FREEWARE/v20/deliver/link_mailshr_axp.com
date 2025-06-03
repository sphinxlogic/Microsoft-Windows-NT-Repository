$ ! image has to be linked /NOTRACEBACK if image is to be installed with privs
$ write sys$output "Linking/creating DELIVER_MAILSHR.EXE"
$ link/notraceback/share=deliver_mailshr.exe deliver,deliver_err,mailshr_axp,-
  mailshr_axp/opt
