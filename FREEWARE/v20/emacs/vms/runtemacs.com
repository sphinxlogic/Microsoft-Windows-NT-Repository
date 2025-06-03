$! This is a subcommand to testemacs.com.
$! It was automatically generated.
$! Do NOT run this directly.  Instead, run testemacs.com
$! and use the symbols `runtemacs' and `runtemacs_d'.
$ proc_dir = f$parse("A.;0",f$environment("PROCEDURE")) - "A.;0"
$ args = ""
$ if args .nes. "" .or. p8 .nes "" then args = """" + p8 + """ " + args
$ if args .nes. "" .or. p7 .nes "" then args = """" + p7 + """ " + args
$ if args .nes. "" .or. p6 .nes "" then args = """" + p6 + """ " + args
$ if args .nes. "" .or. p5 .nes "" then args = """" + p5 + """ " + args
$ if args .nes. "" .or. p4 .nes "" then args = """" + p4 + """ " + args
$ if args .nes. "" .or. p3 .nes "" then args = """" + p3 + """ " + args
$ if args .nes. "" .or. p2 .nes "" then args = """" + p2 + """ " + args
$ define/user EMACSLOADPATH "EMACS_LIBRARY:[000000.LISP],EMACS_LIBRARY:[000000.COMMON.SITE-LISP],EMACS_LIBRARY:[000000.COMMON.LISP]"
$ define/user EMACSPATH "EMACS_LIBRARY:[000000.LIB-SRC]"
$ define/user EMACSDATA "EMACS_LIBRARY:[000000.ETC]"
$ define/user TERMCAP "EMACS_LIBRARY:[000000.ETC]TERMCAP.DAT"
$ define/user INFOPATH "EMACS_LIBRARY:[000000.INFO]"
$ define/user SYS$INPUT SYS$COMMAND
$ mcr 'proc_dir'temacs'p1' -map 'proc_dir'temacs'p1'.dump 'args'
$ exit
