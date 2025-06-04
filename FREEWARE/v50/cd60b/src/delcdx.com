$ ix=0
$ sho:
$   trn = f$trnlnm(f$fao("CD$!UL",ix))
$   if trn.eqs."" then goto endsho
$   write sys$output f$fao("!UL - !AS",ix,trn)
$   ix = ix+1
$   goto sho
$ endsho:
$ read/prompt="_Selection: "/end=stopnow sys$command delstr
$ delnum = f$int(delstr)
$ if f$fao("!UL",delnum).nes.delstr then exit 20
$ if f$trnlnm(f$fao("CD$!UL",delnum)).eqs."" then exit 20
$ ix = delnum
$ loop:
$   ix = ix+1
$   cx = ix-1
$   trn = f$trnlnm(f$fao("CD$!UL",ix))
$   if trn.eqs."" then goto endloop
$   define/nolog 'f$fao("CD$!UL",cx)' 'trn'
$   goto loop
$ endloop:
$ deass 'f$fao("CD$!UL",cx)'
$ stopnow:
$ exit
