$ v = 'f$verify(0)'
$!
$!  VMSMAKE.COM - DCL command procedure to build GNU grep on VMS
$!
$!  To use VAX C, define cc := cc
$!  To use GNU C, define cc := gcc
$!
$!  19931125	F.Macrides		macrides@sci.wfeb.edu
$!		eliminated prefix qualifier for DECC
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		modifications for use with VMSGopherServer distribution
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$    cc := cc/stand=vaxc
$ else
$    cc := cc
$!   cc := gcc
$ endif
$ show sym cc
$ say := write sys$output
$ if f$search("egrep.c") .eqs. "" then copy grep.c egrep.c
$ say "Compiling alloca...."
$ 'cc' alloca
$ say "Compiling dfa...."
$ 'cc' dfa
$ say "Compiling getopt...."
$ 'cc' getopt
$ say "Compiling regex...."
$ 'cc' regex
$ say "Compiling shell_mung...."
$ 'cc' shell_mung
$ say "Compiling grep...."
$ 'cc' grep
$ say "Compiling egrep...."
$ 'cc'/define=egrep egrep
$ if cc.eqs."GCC"
$ then
$    opt := "GNU_CC:[000000]OPTIONS.OPT"
$ else
$    if cc.eqs."CC"
$    then
$       opt := "[-.optfiles]vaxc.opt"
$    else
$       opt := "[-.optfiles]decc.opt"
$    endif
$ endif 
$ say "Linking grep...."
$ link/notrace/exe=[-.exe]grep.exe -
      grep,shell_mung,alloca,dfa,getopt,regex,'opt'/options
$ say "Linking egrep...."
$ link/notrace/exe=[-.exe]egrep.exe -
      egrep,shell_mung,alloca,dfa,getopt,regex,'opt'/options
$ say "Generating on-line help...."
$ runoff grep.rnh
$ say "Cleaning up temporary files...."
$ delete/nolog/noconfirm egrep.c;*,*.obj;*
$ say "Build of GNU greg and egrep completed"
$ v1 = f$verify(v)
$exit
