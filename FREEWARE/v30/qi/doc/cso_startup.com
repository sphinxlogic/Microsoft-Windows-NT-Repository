Return-path: <hornlo@okra.millsaps.edu>
Received: from okra03.millsaps.edu by Cerritos.edu (PMDF V4.2-14 #4768) id
 <01HBM4FE0CC09GXNDO@Cerritos.edu>; Tue, 26 Apr 1994 02:09:26 PST
Received: by okra.millsaps.edu (MX V4.0-1 VAX) id 10; Tue,
 26 Apr 1994 04:10:10 CDT
Date: Tue, 26 Apr 1994 04:10:12 -0500 (CDT)
From: Larry Horn <hornlo@okra.millsaps.edu>
Subject: startup file for CSO
To: TANNER@Cerritos.edu
Cc: hornlo@okra.millsaps.edu
Message-id: <0097D842.9EC71F40.10@okra.millsaps.edu>
Content-transfer-encoding: 7BIT

This and a couple following files are samples.  Edit to suit and use as you
want.

Let me know if TABs are a problem.

loh

$ _sv = f$verify(0)
$!----------------------------------------------------------------------------
$! CSO_STARTUP.COM
$! Last edit: 24-APR-1994 14:25:00.28
$!----------------------------------------------------------------------------
$!
$! This procedure should be called at system startup (via SYSTARTUP_V5.COM or
$! whatever your favorite method is).
$!
$! These directories and files should all be owned by QI_SERVER.
$!
$!----------------------------------------------------------------------------
$! customization
$!----------------------------------------------------------------------------
$!
$!	mydomain is used to determine "local" users
$!
$ mydomain = "millsaps.edu"
$!
$!
$!	mydisk is the physical device on which the CSO files reside
$!
$ mydisk = "$99$DIA34:"
$!
$!
$!	mynode is used to make the log files unique in a cluster environment
$!
$ mynode = f$edit( f$getsyi("SCSNODE"), "TRIM" )
$ if mynode .eqs. "" then mynode = f$edit( f$getsyi("NODENAME"), "TRIM" )
$ if mynode .eqs. "" then mynode = "UNKNOWN"
$!
$!----------------------------------------------------------------------------
$! useful commands
$!----------------------------------------------------------------------------
$!
$ dfn = "define/system/exec/name=no_alias"
$ dfr = "define/system/exec/name=no_alias/trans=(conc,term)"
$!
$!----------------------------------------------------------------------------
$! main
$!----------------------------------------------------------------------------
$!
$ dfn CSO_DISK		'mydisk'
$ dfr CSO_ROOT		'mydisk'[CSO.]
$!
$ dfn CSO_DIR		'mydisk'[CSO]
$!
$! directory logicals
$! ------------------
$!
$ dfn CSO_EXE		CSO_ROOT:[EXE]
$ dfn CSO_LOGDIR	CSO_ROOT:[LOG]
$ dfr CSO_HELPLIB	'mydisk'[HELP.]
$!
$! file logicals
$! -------------
$!
$ dfn CSO_LOG		CSO_LOGDIR:QI_'mynode'.LOG
$!
$ dfn CSO_CONFIG	CSO_ROOT:[DATA]DATABASE.CNF
$ dfn CSO_INDEX		CSO_ROOT:[DATA]DATABASE.INDEX
$ dfn CSO_DATA		CSO_ROOT:[DATA]DATABASE.DATA
$ dfn CSO_SITEINFO	CSO_ROOT:[DATA]SITEINFO.TXT
$!
$! other
$! -----
$!
$ dfn CSO_DOMAIN	"''mydomain'"
$ dfn CSO_MAILDOMAIN	"''mydomain'"
$!
$!----------------------------------------------------------------------------
$! exit
$!----------------------------------------------------------------------------
$!
$ exit 1 + ( 0 * f$verify(_sv) )
$!
$!----------------------------------------------------------------------------
