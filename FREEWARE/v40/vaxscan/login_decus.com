$! LOGIN.COM for the SCAN project.
$ old_verify = 'f$verify( ( f$mode() .eqs. "INTERACTIVE" ) .and. f$verify() )'
$ show time!
$
$ write sys$error "prcnam was ''f$getjpi("","prcnam")'"
$ lgnmode = f$mode()
$! if lgnmode .eqs. "BATCH" .or. lgnmode .eqs. "NETWORK" .or. -
$!		lgnmode .eqs. "OTHER" then -
	$ write sys$error "Running on ''f$getsyi("nodename")', (''lgnmode')"
$ if lgnmode .nes. "INTERACTIVE" then goto onwardb
$
$onward:
$ set noon
$ on control_y then continue
$ if .not. f$getdvi("sys$command","trm") then goto noterm
$ set process/name=scn_'f$element(0,":",f$getjpi("","terminal"))
$! Keep the noise in one window, the session manager, which does not run this
$! file.
$ if 0 .eq. f$locate("FT",f$getjpi("","terminal"))	! was "TW"
$ then
$  set broadcast=(none,dcl)
$ endif
$! You'd better pay attention to the Session Manager window with those BROs off.
$ if 0 .eq. f$locate("TN",f$getjpi("","terminal")) then @logintitle
$
$! if .not. f$getdvi("sys$output","trm") then goto onwardb
$! Put Q & A stuff here.
$
$noterm:
$onwardb:
$ write sys$error "prcnam is ''f$getjpi("","prcnam")'"
$ write sys$error ""
$
$ define/job scan$$disk scan$disk:
$ define/job scan$disk disk$scan:
$ define/table=lnm$process_directory scan_logicals lnm$group
$ write sys$error "defining scan logicals..."
$ if "" .eqs. f$trnlnm("scancom$","scan_logicals") then -
	@disk$scan:[scan.com]scan_logicals
$ write sys$error "calling scan login..."
$ @disk$scan:[scan]login.com
$ write sys$error "proceeding..."
$
$! This only affects the execution of this file, not outer or inner @s
$ set noon
$ on control_y then continue
$
$ set message/facility/identification/severity/text
$ set protection=(s:rwed,o:rwed,g:re,w)/default
$
$! Queues for SCAN_BUILD and children.
$ define/job	cluster_build	clu_batch
$ define/job	work	clu_fast
$ define/job	build	clu_batch
$ define/job	tools_build	clu_batch
$
$ if .not. f$getdvi("sys$command","trm") then goto noterm2
$
$ set control=t
$
$ esc = ""
$ esc[0,8] = 27
$
$! Put the nodename in the prompt.
$ n = f$edit( f$logical("sys$node") - "_" - "::" , "lowercase" )
$ if n .eqs. "" then n = f$getsyi("nodename")	! Leave it upcase to note failure of sys$node.
$ op = f$edit(f$environment("prompt"),"trim")
$ avo = f$getdvi("sys$command", "tt_avo")
$ if avo then bold = esc+"[0;1m"
$ if avo then bbu = esc+"[0;1;4;5m"
$ if avo then norm = esc+"[0m"
$ if f$getdvi("sys$command", "tt_deccrt") then appl = esc+"="
$ if f$locate(esc,op) .eq. f$length(op) then -
	$ set prompt = "''font'''bbu'''n' ''op'''norm'''appl' "
$noterm2:
$
$ prcnam = f$getjpi("","prcnam")
$
$! show time
$ if lgnmode .nes. "INTERACTIVE" then $ goto all_done
$
$! Question and answer section:
$
$ on control_y then goto all_done
$!!Empty Q&A section!!.
$
$all_done:
$ set noon
$ show time!
$ old_verify = f$verify(old_verify)
$! end of LOGIN.COM
