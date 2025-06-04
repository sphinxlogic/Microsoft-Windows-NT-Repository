$ define/user sys$error restrydcc.txt
$ dcc -zcc trydcc
$ diff restrydcc.txt
$ if $SEVERITY .EQ. 1 then delete restrydcc.txt;-1
