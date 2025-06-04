$ ! File adaptstarterfilevms.com
$ set noverify
$ on error then goto erreur
$ open/write out tmp.c
$ write out "int main(){}"
$ close out
$ cc/noobj/lis/show=all tmp.c
$ open/read in tmp.lis
$debutChercheSymboles:
$ read/end=pasTrouveSymboles in ligne
$ if f$extract(0,12,ligne) .nes. "These macros" then goto debutChercheSymboles
$ goto trouveSymboles
$erreur:
$ write sys$output ">> !!! << Problem; aborted"
$ exit $status
$pasTrouveSymboles:
$ write sys$output ">>>Installed 'cc' does not tell predefined macros;"
$ write sys$output ">>>'starter.dcc' file may have to be adapted by hand."
$ append predefmacvms'p1.txt starter.dcc
$ goto fin1
$trouveSymboles:
$ read/end=fin in ligne
$ initialLines = 1
$ open/write out bid.tmp
$suite:
$ read/end=fin in ligne
$ ligne=f$edit(ligne,"COMPRESS,TRIM")
$ if f$length(ligne) .GT. 1 then goto suite2
$ if initialLines
$ then
$   initialLines = 0
$   goto suite
$ endif
$ goto fin
$suite2:
$ if f$length(ligne) .EQ. 0 then goto suite
$ symbole=f$element(0,"=", ligne)
$ ligne=f$extract(f$length(symbole)+1, 10000, ligne)
$ ligne=f$edit(ligne,"TRIM")
$ if f$extract(0,1,ligne) .nes. """"
$ then
$   valeur=f$element(0," ", ligne)
$ else
$   valeur=""""+f$element(1,"""", ligne)+""""
$ endif
$ write out "#define "+symbole+" "+valeur
$ ligne=f$extract(f$length(valeur)+1,10000,ligne)
$ ligne=f$edit(ligne,"TRIM")
$ goto suite2
$fin:
$ close in
$ close out
$ sort bid.tmp bid.tmp
$ ! The following loop because of problems with dcl command 'append'
$ open/read in bid.tmp
$ open/append out starter.dcc
$bcl:
$ read/end=endCopy in ligne
$ write out ligne
$ goto bcl
$endCopy:
$ close in
$ close out
$ delete/noconfirm/nolog bid.tmp;*
$fin1:
$ delete/noconfirm/nolog tmp.c;*, tmp.lis;*
$ ! End file adaptstarterfilevms.com
