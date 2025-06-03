Return-path: <hornlo@okra.millsaps.edu>
Received: from okra03.millsaps.edu by Cerritos.edu (PMDF V4.2-14 #4768) id
 <01HBM4LLLTDS9GXK83@Cerritos.edu>; Tue, 26 Apr 1994 02:14:27 PST
Received: by okra.millsaps.edu (MX V4.0-1 VAX) id 17; Tue,
 26 Apr 1994 04:15:13 CDT
Date: Tue, 26 Apr 1994 04:15:15 -0500 (CDT)
From: Larry Horn <hornlo@okra.millsaps.edu>
Subject: one-time setup for QI
To: TANNER@Cerritos.edu
Cc: hornlo@okra.millsaps.edu
Message-id: <0097D843.53845920.17@okra.millsaps.edu>
Content-transfer-encoding: 7BIT

This is real tenative.  This just sets up the account for the server and
adds it to the UCX services database.  I've not yet checked for the "proper"
quotas for the account nor for additional qualifiers for the UCX SET SERVICE
command, but this will get it working.

The "enable service" should probably be commented out, or a note placed
somewhere that the QI_SERVER.COM file should already be in place.

$!----------------------------------------------------------------------------
$! UCX_SETUP_QI.COM
$! Last edit: 24-APR-1994 14:55:33.12
$!----------------------------------------------------------------------------
$!
$! ***
$ write sys$output "This command file must be edited before use."
$ write sys$output "You should have already executed CSO_STARTUP."
$ exit
$! ***
$!
$!----------------------------------------------------------------------------
$! customization
$!----------------------------------------------------------------------------
$!
$ uic = "[xxx,yyy]"
$!
$!----------------------------------------------------------------------------
$! main
$!----------------------------------------------------------------------------
$!
$!	set up the account; I'm open for suggestions for quota values
$!
$ mcr authorize add QI_SERVER -
	/uic='uic' -
	/account=CSO_GRP -
	/owner="CSO - QI server" -
	/device=CSO_DISK -
	/directory=CSO -
	/flag=(disctly,defcli,restricted) -
	/network -
	/nobatch /nolocal /nodialup /noremote -
	/priv=(noall,netmbx,tmpmbx) -
	/defpriv(noall,netmbx,tmpmbx)
$!
$!
$!	create the directories
$!
$ credir = "create/directory/owner=QI_SERVER/prot=(s:rwe,o:rwe,g,w)
$!
$ credir CSO_DISK:[CSO]
$ credir CSO_ROOT:[EXE]
$ credir CSO_ROOT:[LOG]
$ credir CSO_ROOT:[DATA]
$ credir CSO_ROOT:[HELP]
$ credir CSO_ROOT:[HELP.NATIVE]
$ credir CSO_ROOT:[HELP.PH]
$!
$!
$!	restricted accounts require a LOGIN.COM (or perhaps set LGICMD to NL:)
$!
$ copy NL: CSO_DISK:[CSO]LOGIN.COM
$ open/append TMPLUN CSO_DISK:[CSO]LOGIN.COM
$ write TMPLUN "$! LOGIN.COM for QI_SERVER"
$ close TMPLUN
$!
$!
$!	define the UCX service; add/modify other qualifiers as needed
$!
$ ucx set service QI -
	/user=QI_SERVER -
	/proc=QI_SERVER -
	/port=105 -
	/file=CSO_EXE:QI_SERVER.COM -
	/log=( CSO_ROOT:[LOG]QI_SERVER_STARTUP.COM, ALL )
$!
$!
$!	enable the service
$!
$ ucx enable service QI
$!
$!----------------------------------------------------------------------------
$! exit
$!----------------------------------------------------------------------------
$!
$ exit
$!
$!----------------------------------------------------------------------------
