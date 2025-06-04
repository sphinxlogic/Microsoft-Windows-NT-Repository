$ alpha = f$getsyi("HW_MODEL").ge.1024
$ if alpha
$ then	type = "AXP"
$	opt = ""
$ else	if f$search("SYS$SYSTEM:DECC$COMPILER.EXE").nes.""
$	then type = "VAX_DECC"
$	     opt = ""
$	else type = "VAX_VAXC"
$	     opt = ",VAXCRTL.OPT/OPTIONS"
$	endif
$ endif
$ write sys$output "Linking UNZIP.''type'_EXE..."
$ LINK/NOTRACE /EXE=UNZIP.'type'_EXE UNZIP.'type'_OLB/INCLUDE=UNZIP/LIBRARY,-
		unzip.opt/OPT'opt'
$ write sys$output "Linking UNZIP_CLI.''type'_EXE..."
$ LINK/NOTRACE /EXE=UNZIP_CLI.'type'_EXE unzipcli.'type'_OLB-
		/INCLUDE=(UNZIP,VMS_UNZIP_CMDLINE,VMS_UNZIP_CLD)/LIBRARY,-
		UNZIP.'type'_OLB/LIBRARY,unzip.opt/OPT'opt'
$ write sys$output "Linking UNZIPSFX.''type'_EXE..."
$ LINK/NOTRACE /EXE=UNZIPSFX.'type'_EXE -
	UNZIPSFX.'type'_OLB/INCLUDE=UNZIP/LIBRARY,unzipsfx.opt/OPT'opt'
$ write sys$output "Linking UNZIPSFX_CLI.''type'_EXE..."
$ LINK/NOTRACE /EXE=UNZIPSFX_CLI.'type'_EXE unzsxcli.'type'_OLB-
		/INCLUDE=(UNZIP,VMS_UNZIP_CMDLINE,VMS_UNZIP_CLD)/LIBRARY,-
	UNZIPSFX.'type'_OLB/LIBRARY,unzipsfx.opt/OPT'opt'
$ exit
