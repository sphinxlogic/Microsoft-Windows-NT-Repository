$!
$! KITLINK.COM - link Nmail
$!		 can be used from VMSINSTAL kit installations
$!	         or standalone
$!
$! P1 = "*"         : link everything (default)
$!      "D"         : link daemon
$!	"M"         : link mail interface 
$!	"P"         : link mail interface protected image
$!	"Q"         : link queue utility program 
$!
$! P2 = "VAX"	    : link for VAX   (defaults to same type
$!    = "ALPHA"	    : link for ALPHA   as running system)
$!
$! P3 = "/NOMAP"    : no link maps (default)
$!	"/MAP"      : default link maps
$!	"/MAP/FULL" : full link maps
$!
$	vmi = (f$type(vmi$callback) .nes. "")
$
$	if vmi       then on warning then exit $status
$	if .not. vmi then set noon
$	if vmi	     then dir  = "vmi$kwd:
$	if .not. vmi then dir  = ""
$	if vmi       then say  = "!"
$	if .not. vmi then say  = "write sys$output"
$	if vmi	     then xren = "!"
$	if .not. vmi then xren = "rename/nolog"
$	if vmi	     then host = "''vmi$sys_architecture'"
$	if .not. vmi then host = f$edit(f$getsyi("arch_name"),"upcase,trim")
$
$	p1 = f$edit(p1,"collapse,upcase")
$	if p1 .eqs. "" .or. p1 .eqs. "*" then p1 = "DMPQ"
$
$	_link_daemon   = (f$locate("D",p1) .ne. f$length(p1))
$	_link_mail     = (f$locate("M",p1) .ne. f$length(p1))
$	_link_mailp    = (f$locate("P",p1) .ne. f$length(p1))
$	_link_util     = (f$locate("Q",p1) .ne. f$length(p1))
$
$	if host .eqs. "" then host = "VAX"
$	target = host
$	if p2 .nes. "" then target = f$edit(p2,"upcase,trim")
$	if target .nes. "VAX" .and. target .nes. "ALPHA" then exit 20 ! badparam
$
$	mapq = "/nomap"
$	p3 = f$edit(p3,"collapse,upcase")
$	mm = f$locate("/MAP/",p3)
$	p3 = f$extract(mm+4,999,p3) + f$extract(0,mm+4,p3)
$	if p3 .nes. "" then mapq = p3
$
$	olb  = "nmail_" + target 
$	stb  = ""
$	qual = "/notrace /nodebug"
$	if target .nes. host    then qual = "/" + target + " " + qual
$	if target .eqs. "ALPHA" then qual = qual + " /sysexe"
$	if target .eqs. "VAX"   then stb = "+ sys$system:sys.stb/select"
$
$link1:
$	if .not. _link_daemon then goto link2
$	say "nm$daemon"
$	link 'qual' 'mapq'='dir'nm$daemon /exec='dir'nm$daemon -
		'dir''olb'/library/include=(nm$symbiont,nm$message) +-
		'dir'ident/options 'stb'
$	xren 'dir'nm$daemon.* nm$daemon_'target'
$
$link2:
$	if .not. _link_util then goto link3
$	say "nm$queman"
$	link 'qual' 'mapq'='dir'nm$queman /exec='dir'nm$queman -
		'dir''olb'/library/include=(nm$clint,nm$message) +-
		'dir'ident/options 'stb'
$	xren 'dir'nm$queman.* nm$queman_'target'
$
$link3:
$	if .not. _link_mailp then goto link4
$	say "nm_mailshrp"
$	open/write $nm$ 'dir'nm_mailshrp.opt
$	if target .eqs. "ALPHA"
$	then
$	    write $nm$ "  symbol_vector=( -"
$	    write $nm$ "    nm$uss_create_ctl_file=procedure, -"
$	    write $nm$ "    nm$uss_close_ctl_file=procedure, -"
$	    write $nm$ "    nm$uss_submit_job=procedure -"
$	    write $nm$ "    )"
$	endif
$	write $nm$ "  gsmatch=lequal,1,1"
$	close $nm$
$	link 'qual' /nosysshr /protect 'mapq'='dir'nm_mailshrp /share='dir'nm_mailshrp -
		'dir''olb'/libr/incl=(nm$chmode) +-
		'dir'nm_mailshrp/options +-
		'dir'ident/options
$	delete/nolog 'dir'nm_mailshrp.opt;*
$	xren 'dir'nm_mailshrp.* nm_mailshrp_'target'
$
$link4:
$	if .not. _link_mail then goto linkz
$	say "nm_mailshr"
$	open/write $nm$ 'dir'nm_mailshr.opt
$	if target .eqs. "ALPHA"
$	then
$	    write $nm$ "  symbol_vector=( -"
$	    write $nm$ "    mail$c_prot_major=data, -"
$	    write $nm$ "    mail$c_prot_minor=data, -"
$	    write $nm$ "    mail$protocol=procedure -"
$	    write $nm$ "    )"
$	else
$	    write $nm$ "  universal=mail$c_prot_major"
$	    write $nm$ "  universal=mail$c_prot_minor"
$	    write $nm$ "  universal=mail$protocol"
$	endif
$	write $nm$ "  nm_mailshrp/share"
$	write $nm$ "  gsmatch=equal,9,3"
$	close $nm$
$	if vmi
$	then tmp = dir + "nm_mailshrp.exe"
$	else tmp = dir + "nm_mailshrp_''target'.exe"
$	endif
$	if f$search(tmp) .eqs. "" .and. f$search("sys$share:nm_mailshrp.exe") .nes. ""
$	then write sys$output "No file ''tmp' found - using image in sys$share:"
$	     tmp = "sysSshare:nm_mailshrp.exe"
$	endif
$	define/user nm_mailshrp 'tmp'
$	link 'qual' 'mapq'='dir'nm_mailshr /share='dir'nm_mailshr -
		'dir''olb'/libr/incl=(nm$mail,nm$message) +-
		'dir'nm_mailshr/options +-
		'dir'ident/options
$	delete/nolog 'dir'nm_mailshr.opt;*
$	xren 'dir'nm_mailshr.* nm_mailshr_'target'
$
$linkz:
$	exit 1
$!
$! End KITLINK.COM
$!
