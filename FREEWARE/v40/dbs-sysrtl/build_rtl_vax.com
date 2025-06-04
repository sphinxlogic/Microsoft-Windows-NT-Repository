$ ! Procedure:	BUILD_DBSSYSRTL_VAX.COM
$ set noon
$ create dbssysrtl.mar
	.title	dbssysrtl
	.ident	"X1-009"

	.library	"DBSLIBRARY:SYS_MACROS.MLB"

	def_psect $$$$$vectors, type=CODE, alignment=QUAD
	def_psect _sys_code, type=CODE, alignment=LONG

; allocate transfer vectors using the XFER macro
; only add new routines by using existing spare definitions
; "delete" routines by using the DUMMY macro

.macro	xfer	module

	.transfer module
	.mask	module
	jmp	l^'module'+2

.endm	xfer

.macro	dummy	routine

	xfer	routine

	set_psect _sys_code
	.entry -
routine, ^m<>
	ret
	reset_psect

.endm	dummy

.macro	spare	number

	xfer	sys_spare_'number

	set_psect _sys_code
	.entry -
sys_spare_'number, ^m<>
	ret
	reset_psect

.endm	spare

	set_psect $$$$$vectors

	xfer	BIO_CLOSE
	xfer	BIO_OPEN
	xfer	BIO_OPEN_RO
	xfer	BIO_READ
	xfer	BIO_WRITE
	xfer	CLU_BROADCAST
	xfer	CLU_MEMBERSHIP
	xfer	DCL_ENVI_DEPTH
	xfer	DCL_ENVI_PROCEDURE
	xfer	FOR_SIGNAL
	xfer	FOR_STOP
	xfer	LIB_AVAILABLE
	xfer	LIB_CHECKSUM2
	xfer	LIB_COMPARE_BLOCKS
	xfer	LIB_COUNT_BITS
	xfer	LIB_CVT_L_DELTA
	xfer	LIB_CVT_T_L
	xfer	LIB_GET_INPUT
	xfer	LIB_OUTPUT_BKT
	xfer	LIB_OUTPUT_HM2
	xfer	LIB_OUTPUT_SEG_T
	xfer	LIB_OUTPUT_SEG_TZB
	xfer	LIB_OUTPUT_SEG_ZB
	xfer	LIB_PATCH_BLOCK
	xfer	LIB_SUB_ASC_TIMES
	xfer	STR_COLLAPSE
	xfer	STR_COMPRESS
	xfer	STR_ELEMENT
	xfer	STR_EVALUATE
	xfer	STR_JUST_LEFT
	xfer	STR_JUST_RIGHT
	xfer	STR_LEN
	xfer	STR_LOWERCASE
	xfer	STR_MATCH
	xfer	STR_SUBSTITUTE
	xfer	STR_SUBTRACT
	xfer	STR_TRIM
	xfer	STR_UPPERCASE
	xfer	SYS_BROADCAST
	xfer	SYS_DISABLE_ECHO
	xfer	SYS_ENABLE_ECHO
	xfer	SYS_GET_DISK_DEVICES
	xfer	SYS_GET_TAPE_DEVICES
	xfer	SYS_JOBTYPE
	xfer	SYS_MEMSIZE
	xfer	SYS_MODE
	xfer	SYS_PARSE
	xfer	SYS_PEEK
	xfer	SYS_POKE
	xfer	SYS_FIND_ETHER_DEVICE		; DBS, 20-Nov-1991
	xfer	LIB_FORMAT_PROTECTION		; DBS, 08-Jan-1992
	xfer	SYS_CHECK_SOFTWARE_EXPIRY1	; DBS, 17-Jan-1992
	xfer	STR_CMATCH			; DBS, 13-Jul-1992
	xfer	STR_SOUNDEX			; DBS, 13-Jul-1992
	xfer	LIB_VALIDATE_FH2		; DBS, 13-Jul-1992
	xfer	LIB_OUTPUT_FH2			; DBS, 15-Jul-1992
	xfer	LIB_PATCH_INPUT_INTEGER		; DBS, 15-Jul-1992
	xfer	LIB_PATCH_VALID_BYTE		; DBS, 15-Jul-1992
	xfer	LBR_OUTPUT_MLB_MODULE		; DBS, 15-Jul-1992
	xfer	LIB_GENERATE_SEED		; DBS, 20-Aug-1992
	xfer	LIB_RANDOM			; DBS, 20-Aug-1992
	xfer	SYS_TRAP_CONTROLC		; DBS, 14-Jul-1994
	xfer	LAN_STARTUP_PRM			; DBS, 14-Jul-1994
	xfer	LAN_FORMAT_HEADER		; DBS, 14-Jul-1994
	spare	05
	spare	04
	spare	03
	spare	02
	spare	01
	spare	00

	.end
$ macroo dbssysrtl.mar/object=dbssysrtl.vax_obj
$ deletee/nolog dbssysrtl.mar;*
$ linkk/share=dbssysrtl.vax_exe -
	/map=dbssysrtl.vax_map/full -
	/nodebug -
	/notraceback -	
	/nouserlibrary -
	dbssysrtl.vax_obj -
	,sys$input/options

	gsmatch		= lequal,1,9
	ident		= "X1-009"

	psect_attr	= sysbrdcststuff,noshr

	cluster = dbssysrtl_vectors
	collect = dbssysrtl_vectors-
			,$$$$$vectors
	cluster = dbssysrtl_nonvectors
	collect = dbssysrtl_nonvectors-
			,_sys_code-
			,$code-
			,$pdata-
			,_ots$code
	cluster = dbssysrtl_data
	collect = dbssysrtl_data-
			,_sys_data-
			,$local-
			,$array-
			,$desc-
			,sysbrdcststuff-
			,msgptr$aaaaaaaa-
			,msgptr$aaaaaaab-
			,msgptr$aaaaaaac-
			,msgptr$section

	sys_objects.vax_olb/library
$ delete dbssysrtl.vax_obj;
$! delete dbssysrtl.vax_map;
$ library/create/share dbssysrtl.vax_olb
$ library dbssysrtl.vax_olb dbssysrtl.vax_exe
$ exitt 1
