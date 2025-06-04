$ oldSysOut = F$TRNLMN("sys$output")
$ define/nolog sys$output restrydcc.txt
$ define/nolog sys$error sys$output
$ DCCDFLTS :== "-zcc"
$ dcc trydcc
$ deassign sys$error
$ define/nolog sys$output 'oldSysOut !back to initial value
$ DCCDFLTS :== ""
$ diff restrydcc.txt
$ if $SEVERITY .EQ. 1 then delete restrydcc.txt;-1
