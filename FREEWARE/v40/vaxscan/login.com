$ SET NOVERIFY
$ SET NOON
$!
$! Execute SCAN login.com
$!
$ @scan$$disk:[scan.com]login
$!
$! Establish default protection
$!
$ SET PROTECTION=(W,S:RE,G:RWED,O:RWED) /DEFAULT
$!
$! Commands to run in interactive mode ONLY
$!
$ if "''f$mode()'" .nes. "INTERACTIVE" then goto keep_going
$ define/nolog basic$edit lse$edit
$!!! define/nolog lse$section basic$:[basic.com]jjh
$!!! define/nolog lse$environment basic$:[basic.com]jjh
$ HOME :== SET DEFAULT SYS$LOGIN
$ T*ype :== type
$ rec*all :== recall
$ SET TERM/INSERT/line_editing
$ SET VER
$ SHOW DEV SCAN$$DISK
$ SET NOVER
$!
$! Set the prompt to indicate that this is the SCAN account
$!
$ esc = ""
$ esc[0,8] = 27
$ node_name = f$edit(f$trnlnm ("sys$node", "lnm$system_table"), -
                        "LOWERCASE, COLLAPSE") - "::"
$ set prompt= "''esc'[1m''node_name'_scan> ''esc'[0m"
$!
$ KEEP_GOING:
$!kop! @tool$:scasetup
$!
$!  DCL command line
$!
$ SUB*MIT :== SUBMIT/KEEP/NOPRINT/NOTIFY
$ PRINT  :== PRINT/NOTIFY
$ T*YPE  :== TYPE
$ SETCOM 	:==	@scan$$disk:[scan.com]setcom
$ SETRTL 	:==	@scan$$disk:[scan.com]setrtl
$ scandtm	:==	@scan$$disk:[scan.com]setdtm
$ anad*ump :== $tool$:anadump_v2.exe
$!
$! assign/nolog/table=lnm$process_directory -
$!!	lnm$process, lnm$job, scan_logicals, decw$logical_names, -
$!!	lnm$group, lnm$system lnm$file_dev
$! dele [scan.dtm.scalib]*.*;*
$! sca create lib [scan.dtm.scalib]
$ sca set lib scan$$disk:[scan.dtm.scalib]
$!
$! Define mail logicals for members of the team
$!
$ define/nolog sharon tools::swalker
$ define/nolog dennis tools::periquet
$ define/nolog john tools:sutton
$!
$! Temporary commands, if any
$!
$! define scnrtl rprobj$:scnrtl.exe
$! define scnmsg rprobj$:scnmsg.exe
$! define imagelib sys$login:imagelib.olb
$ scan :== scan/ana=scan$$disk:[scan.dtm.ana]
