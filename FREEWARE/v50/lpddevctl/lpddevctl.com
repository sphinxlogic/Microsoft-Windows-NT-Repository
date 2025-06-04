$ root := sys$sysroot
$ name := lpddevctl.tlb
$ modules := ltr_12_prolog,ltr_12_96_prolog,ltr_12_a5_prolog,ltr_12_a6_prolog,-
ltr_12_a6_land,lpt_prolog,lpt212_prolog
$ if p1.eqs.""
$ then
$	p1 := sys$share:'name'
$	lib := 'f$search(p1)'
$	if lib.eqs."" then-
	    p1 := 'f$trn(root,,f$trn(root,,,,,"max_index"))'[syslib]'name'
$ else
$	if f$parse(p1,,,"type").eqs."." then $ p1 := 'p1'.tlb
$	lib := 'f$search(p1)'
$ endif
$ if lib.eqs.""
$ then	say "Library ",p1," not found - will be created"
$	library/text/create=(blocks:40,module:10)/log 'p1'
$ endif
$!
$ i = 0
$ goto modget
$modloop: i = i + 1
$ copy 'mod'.stub+text-layout.txt 'mod'.txt
$ library/text/log 'p1' 'mod'
$ delete 'mod'.txt;
$modget: mod := 'f$el(i,",",modules)'
$ if (mod.nes.",") then $ goto modloop
$!
$ library/text/log 'p1' nl:/mod=msap$ctrl_d
$ library/text/log 'p1' nl:/mod=lpt_graybar_page_form
