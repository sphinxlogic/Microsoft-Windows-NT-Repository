$set noon
$
$	DEASSIGN /TABLE=LNM$SYSTEM_DIRECTORY VWSBANNER
$	CREATE /NAME_TABLE /PARENT=LNM$SYSTEM_DIRECTORY -
		VWSBANNER /PROT=(S:RWED,G:RWED,O:RWED,W:RWED) 
$
$  DEFINE /TABLE=VWSBANNER BACKGROUND_REVERSE	"1"
$  DEFINE /TABLE=VWSBANNER POSITION_TOP		"1"
$  DEFINE /TABLE=VWSBANNER POSITION_RIGHT	"1"
$  DEFINE /TABLE=VWSBANNER TEXT_LINES		"8.9"
$! DEFINE /TABLE=VWSBANNER SCREEN_WIDTH		"0.60"
$  DEFINE /TABLE=VWSBANNER CUBE			"0"
$  IF F$GETSYI ("CLUSTER_MEMBER") THEN -
   DEFINE /TABLE=VWSBANNER PE_MONITOR		"1"
$  DEFINE /TABLE=VWSBANNER PAGE_IO_UPDATE	"3"
$  DEFINE /TABLE=VWSBANNER PE_UPDATE		"1"
$  DEFINE /TABLE=VWSBANNER MODE_UPDATE		"1"
$  DEFINE /TABLE=VWSBANNER CPU_UPDATE		"5"
$  DEFINE /TABLE=VWSBANNER CPU_COUNT		"180"
$  DEFINE /TABLE=VWSBANNER INCLUDE_BANNER_CPU	"0"
$  DEFINE /TABLE=VWSBANNER PRIORITY		"14"
$  DEFINE /TABLE=VWSBANNER REMOTE_DISK		"1"
$  DEFINE /TABLE=VWSBANNER DEVICE_1_NAME	"SYS$SYSDEVICE:","FOLKD$:"
$  DEFINE /TABLE=VWSBANNER DEVICE_1_TITLE	"Sys/Folk"
$  DEFINE /TABLE=VWSBANNER DEVICE_2_NAME	"VALTHO$DUA1:"
$  DEFINE /TABLE=VWSBANNER DEVICE_2_TITLE	"PageDisk"
$  IF F$GETDVI ("XQA0:", "EXISTS") THEN -
   DEFINE /TABLE=VWSBANNER DEVICE_3_NAME	"XQA!UL:"
$  IF F$GETDVI ("ESA0:", "EXISTS") THEN -
   DEFINE /TABLE=VWSBANNER DEVICE_3_NAME	"ESA!UL:"
$  DEFINE /TABLE=VWSBANNER DEVICE_3_COUNT	"10"
$  DEFINE /TABLE=VWSBANNER DEVICE_3_TITLE	"DECnet"
$
$  dft := define /table=vwsbanner
$
$  dft clock 0
$  dft cpu_monitor 0
$  dft mode_monitor 0
$  dft page_io_monitor 0
$  dft pe_monitor 0
$
$  dft border 1
$  dft edge_lines 0
$
$  dft page_io_update 1
$  dft cpu_update 1
$  dft pe_update 1
$  dft mode_update 1
$  dft clock_update 1
$
$  dft pe_monitor 1
$  prcnam = "PE banner"
$  gosub CREPRC
$  dft pe_monitor 0
$
$  dft page_io_monitor 1
$  prcnam = "IO banner"
$  gosub CREPRC
$  dft page_io_monitor 0
$
$  dft mode_monitor 1
$  prcnam = "MODE banner"
$  gosub CREPRC
$  dft mode_monitor 0
$
$  dft cpu_monitor 1
$  prcnam = "CPU banner"
$  gosub CREPRC
$  dft cpu_monitor 0
$
$  dft clock 1
$  prcnam = "CLOCK banner"
$  gosub CREPRC
$  dft clock 0
$
$  exit
$
$
$CREPRC:
$ run work3:<hobbs.wrkd$.tmp2.v50ft2>vwsbanner -
	/process_name="''prcnam'" -
	/priv=(nosame,altpri,prmmbx,cmkrnl,sysprv,sysnam) -
	/priority=14 -
	/uic='f$user()' -
	/maximum_working_set=150 -
	/extent=300 -
	/noswap -
	/output=wta0: -
	/error=sys$manager:banner_error.log
$WAIT:
$ wait 0:0:1
$ if f$trnlnm ("VWSBANNER_INITIALIZED", "VWSBANNER") .eqs. "" then goto wait
$ deassign /table=vwsbanner vwsbanner_initialized
$ return
