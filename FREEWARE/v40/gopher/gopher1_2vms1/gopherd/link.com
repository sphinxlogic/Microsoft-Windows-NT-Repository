$ v = 'f$verify(0)'
$! LINK.COM
$!      link VMS Gopher Server
$!
$!  19940311	D.Sherman		dennis_sherman@unc.edu
$!		changed WINS to WOLLONGONG
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting fixups
$!  19931116	D.Sherman		dennis_sherman@unc.edu
$!		rewritten for Gopher1_2VMS-1 release
$!  19930909    D.Sherman               dennis_sherman@unc.edu
$!              rewritten version for Gopher1_2VMS0, based in part on
$!              the Gopher1_2VMS0 client make.com
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ agent = 0
$ IF P1 .EQS. ""
$ THEN
$ write sys$output "Acceptable TCP/IP agents are"
$ write sys$output " [1] MultiNet (default)"
$ write sys$output " [2] UCX"
$ write sys$output " [3] Wollongong"
$ read sys$command/prompt="Agent [1,2,3] (RETURN = [1]) " agent
$ ENDIF
$ if agent .eq. 1 .or. agent .eq. "" .or. p1 .eqs. "MULTINET" then -
    option = "MULTINET"
$ if agent .eq. 2 .or. p1 .eqs. "UCX" then option = "UCX"
$ if agent .eq. 3 .or. p1 .eqs. "WOLLONGONG" then option = "WOLLONGONG"
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$!      DECC/Alpha:
$   optfile = "''option'" + "_DECC"
$ else
$!	VAXC:
$   optfile = "''option'" + "_VAXC"
$ endif
$ show sym optfile
$!
$ v1 = f$verify(1)
$ link /executable=[-.exe]gopherd.exe -
    [-.obj]compatible.obj,  [-.obj]daarray.obj,-
    [-.obj]debug.obj,	    [-.obj]ext.obj,-
    [-.obj]ftp.obj,	    [-.obj]gdgopherdir.obj,-
    [-.obj]globals.obj,	    [-.obj]gopherd.obj,-
    [-.obj]gopherdconf.obj, [-.obj]gsgopherobj.obj,-
    [-.obj]index.obj,	    [-.obj]kernutils.obj,-
    [-.obj]site.obj,	    [-.obj]sockets.obj,-
    [-.obj]special.obj,	    [-.obj]strstring.obj,-
    [-.obj]util.obj,	    [-.obj]waisgopher.obj,-
    [-.optfiles]ident/opt,  [-.optfiles]'optfile'/opt
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit
