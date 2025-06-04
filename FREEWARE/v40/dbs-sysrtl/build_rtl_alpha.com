$ ! Procedure:	BUILD_DBSSYSRTL_ALPHA.COM
$ set noon
$ linkk/share=dbssysrtl.alpha_exe -
	/map=dbssysrtl.alpha_map/full -
	/nodebug -
	/notraceback -
	/nouserlibrary -
	/nonative_only -
	/sysexe=selective -
	sys$input/options

	gsmatch		= lequal,1,9
	ident		= "X1-009"

	psect_attr	= sysbrdcststuff,noshr

	symbol_vector = ( -
		BIO_CLOSE			= procedure, -
		BIO_OPEN			= procedure, -
		BIO_OPEN_RO			= procedure, -
		BIO_READ			= procedure, -
		BIO_WRITE			= procedure, -
		CLU_BROADCAST			= procedure, -
		CLU_MEMBERSHIP			= procedure, -
		DCL_ENVI_DEPTH			= procedure, -
		DCL_ENVI_PROCEDURE		= procedure, -
		FOR_SIGNAL			= procedure, -
		FOR_STOP			= procedure, -
		LIB_AVAILABLE			= procedure, -
		LIB_CHECKSUM2			= procedure, -
		LIB_COMPARE_BLOCKS		= procedure, -
		LIB_COUNT_BITS			= procedure, -
		LIB_CVT_L_DELTA			= procedure, -
		LIB_CVT_T_L			= procedure, -
		LIB_GET_INPUT			= procedure, -
		LIB_OUTPUT_BKT			= procedure, -
		LIB_OUTPUT_HM2			= procedure, -
		LIB_OUTPUT_SEG_T		= procedure, -
		LIB_OUTPUT_SEG_TZB		= procedure, -
		LIB_OUTPUT_SEG_ZB		= procedure, -
		LIB_PATCH_BLOCK			= procedure, -
		LIB_SUB_ASC_TIMES		= procedure, -
		STR_COLLAPSE			= procedure, -
		STR_COMPRESS			= procedure, -
		STR_ELEMENT			= procedure, -
		STR_EVALUATE			= procedure, -
		STR_JUST_LEFT			= procedure, -
		STR_JUST_RIGHT			= procedure, -
		STR_LEN				= procedure, -
		STR_LOWERCASE			= procedure, -
		STR_MATCH			= procedure, -
		STR_SUBSTITUTE			= procedure, -
		STR_SUBTRACT			= procedure, -
		STR_TRIM			= procedure, -
		STR_UPPERCASE			= procedure, -
		SYS_BROADCAST			= procedure, -
		SYS_DISABLE_ECHO		= procedure, -
		SYS_ENABLE_ECHO			= procedure, -
		SYS_GET_DISK_DEVICES		= procedure, -
		SYS_GET_TAPE_DEVICES		= procedure, -
		SYS_JOBTYPE			= procedure, -
		SYS_MEMSIZE			= procedure, -
		SYS_MODE			= procedure, -
		SYS_PARSE			= procedure, -
		SYS_PEEK			= procedure, -
		SYS_POKE			= procedure, -
		SYS_FIND_ETHER_DEVICE		= procedure, -
		LIB_FORMAT_PROTECTION		= procedure, -
		SYS_CHECK_SOFTWARE_EXPIRY1	= procedure, -
		STR_CMATCH			= procedure, -
		STR_SOUNDEX			= procedure, -
		LIB_VALIDATE_FH2		= procedure, -
		LIB_OUTPUT_FH2			= procedure, -
		LIB_PATCH_INPUT_INTEGER		= procedure, -
		LIB_PATCH_VALID_BYTE		= procedure, -
		LBR_OUTPUT_MLB_MODULE		= procedure, -
		LIB_GENERATE_SEED		= procedure, -
		LIB_RANDOM			= procedure, -
		SYS_TRAP_CONTROLC		= procedure, -
		LAN_STARTUP_PRM			= procedure, -
		LAN_FORMAT_HEADER		= procedure )

	sys_objects.alpha_olb/library
	sys$loadable_images:dcldef.stb
	sys$library:tecoshr_tv.exe/shareable
$! delete dbssysrtl.alpha_map;
$ library/create/share dbssysrtl.alpha_olb
$ library dbssysrtl.alpha_olb dbssysrtl.alpha_exe
$ exitt 1
