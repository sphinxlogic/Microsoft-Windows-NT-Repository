$!
$! This command file will create, compile and link a prorgram to give basic
$! global buffer statistics straIght from the global section info, without
$! requiring file statistics to be enabled. Up to 15 files are displayed.
$!
$! This is a small modification to a pre V5.4 version which relied on 
$! certain data structure to be readable from user mode and now need CMEXEC.
$! To say that there is room for improvement in this program is the under-
$! statement of the year, still it has done me well in the past!  Hein.
$!
$CREATE GBL.BAS
 1	OPTION TYPE = EXPLICIT
 !	 
 ! Program to monitor global buffer hit and miss rates.
 ! Requires CMEXEC priv to run. Uses RMS to OPEN file and MAP 
 ! to Global buffer on CONNECT. Uses EXEC mode routine to move
 ! counters from mapped section.
 !
 ! Side effect... will keep file open and gbobal buffer alive.
 !
 ! Usage: Pass wildcard file spec on command line as 'foreign' command.
 !
 ! Commands: ^Z = EXIT, ^W = REPAINT, RETURN = REFRESH COUNTERS.
 !
 ! Hein van den Heuvel, 1991
 !
	EXTERNAL LONG FUNCTION	SYS$CMEXEC, LIB$GET_FOREIGN, LIB$FIND_FILE, &
				LIB$FIND_FILE_END, LIB$TRIM_FILESPEC, 	    &
				SYS$OPEN(FAB$TYPE), SYS$CONNECT(RAB$TYPE),  &
				SYS$CLOSE(FAB$TYPE), SYS$SETPRV
	EXTERNAL LONG		GBL_FIND, GBL_READ, SMG$CANCEL_INPUT
	
	DECLARE STRING	FILE_SPEC, RESULT_SPEC, TMP
	DECLARE LONG	FILE_GBL(20), MAX_USE(20),			&
			CUR_HIT, MAX_HIT(20), TOT_HIT(20),		&
			CUR_MIS, MAX_MIS(20), TOT_MIS(20), 		&
			CONTEXT, DONE, I, J, STAT, FILE_COUNT, HIT_RATIO, &
			PB, KB, SCREEN, Header_display, Message_display, &
			KEYSTROKE, OLD_PRIVS(1), NEW_PRIVS(1), Clear_Messages
	EXTERNAL LONG	CONSTANT RMS$_NORMAL, RMS$_NMF, FAB$M_GET, 	&
				FAB$M_SHRUPD, PRV$M_CMEXEC, PRV$V_READALL
	DECLARE WORD	CONSTANT FAB_CODE=20483, RAB_CODE=17409
	DECLARE LONG	CONSTANT NAME_LENGHT = 32

!
!	This code should really include the fab & rab defintions from a 
!	library but as that does not appear to be available everywhere...
!
!	%INCLUDE "$FABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"

	RECORD FAB$TYPE
    	   word START,	word IFI,	long FOP,	long STS,STV	&
	  ,long ALQ,	word DEQ,	byte FAC,	byte SHR	&
	  ,long CTX,	byte RTV,	byte ORG,	byte RAT	&
	  ,byte RFM,	long JNL,	long XAB,	long NAM	&
	  ,long FNA,	long DNA,	byte FNS,	byte DNS	&
	  ,word MRS,	long MRN,	word BLS,	byte BKS	&
	  ,byte FSZ,	long DEV,	long SDC,	word GBC	&
	  ,byte ACM,	byte RCF,	long FILL
    	END RECORD
	RECORD RAB$TYPE
	   long START,	long ROP,	long STS,	long STV	&
	  ,long RFA_VBN,word RFA_ID,	word FILL_1,	long CTX	&
	  ,word FILL_2,	byte RAC,	byte TMO,	word USZ	&
	  ,word RSZ,	long UBF,	long RBF,	long RHB	&
	  ,long KBF,	byte KSZ,	byte KRF,	byte MBF	&
	  ,byte MBC,	long BKT,	long FAB,	long XAB
	END RECORD

	MAP (X) LONG	GBL_ADDRESS, GBL_HIT, GBL_MIS, GBL_USE
	MAP (Y) STRING	OUT_LINE = 47
	MAP (Z) 	RAB$TYPE RAB, FAB$TYPE FAB, STRING NAME_BUFFER = 80	

	NEW_PRIVS(0) = PRV$M_CMEXEC
	NEW_PRIVS(1) = 0

	STAT = LIB$GET_FOREIGN (FILE_SPEC, "File specification: ")
	GO TO 2 IF FILE_SPEC = ""	
	Clear_Messages = -1%
	GOSUB	Init_screen
	GOSUB	Init_files
	WHILE NOT Done
	    SELECT Keystroke
		CASE = 0%, 13%, 509%		!Init, Return, Time out
		    GOSUB Show_stat
		CASE = 23%			!Control-W
		    CALL SMG$Repaint_screen (PB)
		CASE ELSE
		    CALL SMG$Put_Line(Message_display,			&
			"This key (" + NUM1$(KEYSTROKE) + ") is not valid")
	    END SELECT	    
	    CALL SMG$READ_KEYSTROKE (KB, KEYSTROKE,,5% )

	    IF Clear_messages THEN
		Clear_messages = 0
		CALL SMG$PUT_LINE ( MESSAGE_DISPLAY, " " )
		CALL SMG$PUT_LINE ( MESSAGE_DISPLAY, " " )
		CALL SMG$PUT_LINE_WIDE (Message_display, "Global Buffer Statistics" )
	    END IF

	    Done = -1% IF Keystroke = 26%			!Control-Z?

	NEXT

	CALL SMG$PUT_LINE(MESSAGE_DISPLAY,"*EXIT*")
	GO TO 2

 Show_Stat:

	FOR I = 1% TO FILE_COUNT
		GBL_ADDRESS	= FILE_GBL(I) 			!first in MAP.
		STAT = SYS$CMEXEC ( GBL_READ , GBL_ADDRESS)	!output in MAP.
	    	CUR_HIT		= GBL_HIT - TOT_HIT(I)
		CUR_MIS		= GBL_MIS - TOT_MIS(I)
		MAX_HIT(I)	= CUR_HIT IF CUR_HIT > MAX_HIT(I)
		MAX_USE(I)	= GBL_USE IF GBL_USE > MAX_USE(I)
		MAX_MIS(I)	= CUR_MIS IF CUR_MIS > MAX_MIS(I)
		TOT_HIT(I)	= GBL_HIT
		TOT_MIS(I)	= GBL_MIS			
		J = GBL_HIT + GBL_MIS 
		J = 1% IF J = 0%
		HIT_RATIO = (100% * GBL_HIT) / J
	!		
	!	Note: The format must correspont with Init_Screen header.
	!
	!	    Hit  Miss   %  Use  Hit Miss   Use  Hit Miss
	!	 ****** *****  ***************** *************** 
	!
		CALL SYS$FAO ( "!6UL!6UL  !2UL!3(5UL) !3(5UL)",,	&
			OUT_LINE, GBL_HIT BY VALUE, GBL_MIS BY VALUE, 	&
			HIT_RATIO BY VALUE, GBL_USE BY VALUE, 		&
			CUR_HIT BY VALUE, CUR_MIS BY VALUE, 		&
			MAX_USE(I) BY VALUE, MAX_HIT(I) BY VALUE, 	&
			MAX_MIS(I) BY VALUE)
		CALL SMG$PUT_CHARS ( Screen, OUT_LINE, I, NAME_LENGHT + 1% )
	NEXT I
	RETURN

 Init_Screen:
	CALL SMG$CREATE_PASTEBOARD (PB)
	CALL SMG$CREATE_VIRTUAL_KEYBOARD (KB)
	CALL SMG$SET_BROADCAST_TRAPPING (PB, SMG$CANCEL_INPUT, KB)
	CALL SMG$CREATE_VIRTUAL_DISPLAY (15%, 80%, Screen)
	CALL SMG$CREATE_VIRTUAL_DISPLAY (2%, 80%, Header_display)
	CALL SMG$CREATE_VIRTUAL_DISPLAY (3%, 80%, Message_display)
	CALL SMG$PASTE_VIRTUAL_DISPLAY (Screen, PB, 4%, 1%)
	CALL SMG$PASTE_VIRTUAL_DISPLAY (Header_display, PB, 1%, 1%)
	CALL SMG$PASTE_VIRTUAL_DISPLAY (Message_display, PB, 21%, 1%)
	CALL SMG$PUT_LINE (Header_display, &
 "                                 _Total_________  Current______   Max__________")
	CALL SMG$PUT_LINE (Header_display, &
 "Filename                   Gbc     Hit  Miss   %  Use  Hit Miss   Use  Hit Miss")


	RETURN

 Init_files:
	FAB::START = FAB_CODE		!Set FAB$B_BID and FAB$B_BLN
	RAB::START = RAB_CODE		!Set RAB$B_BID and RAB$B_BLN
	FAB::FNA = LOC(NAME_BUFFER)	!Put Address of name_buf in Fab
	FAB::FAC = FAB$M_GET		!GET access.
	FAB::SHR = FAB$M_SHRUPD		!Allow all sharing.
	RAB::FAB = LOC(FAB::START)	!Put Address of Fab in Rab
	CONTEXT = 0%
	I = 0%
	!
	! Enable CMEXEC and READALL, allowing VMS to clean up on exit.
	!
	STAT = SYS$SETPRV (1% BY VALUE, NEW_PRIVS() BY REF, &
			   0% BY VALUE, OLD_PRIVS())
	STAT = LIB$FIND_FILE (FILE_SPEC, RESULT_SPEC, CONTEXT)
	WHILE  (STAT AND 1%) AND (I < 15%)
	    NAME_BUFFER = RESULT_SPEC	!Move into static string for LOC().
	    FAB::FNS = LEN(RESULT_SPEC)	!Put Lenght of file_name in Fab
	    FAB::IFI = 0%		!
	    STAT = SYS$OPEN(FAB)	!Open the file
    		CALL sys$exit(STAT BY VALUE) IF STAT <> RMS$_NORMAL
	    IF FAB::GBC = 0 THEN
		CALL SMG$PUT_LINE ( MESSAGE_DISPLAY, &
			RESULT_SPEC + " has NO global buffers.")
		CALL SYS$CLOSE (FAB)
	    ELSE
		I = I + 1%
    		STAT = SYS$CONNECT(RAB)	!Connect a buffer
    			CALL sys$exit(STAT BY VALUE) IF STAT <> RMS$_NORMAL
		TMP = NUM1$(FAB::GBC)
		J = NAME_LENGHT - LEN(TMP) 
		CALL LIB$TRIM_FILESPEC(RESULT_SPEC, NAME_BUFFER, J - 1%)
		CALL SMG$PUT_CHARS (SCREEN,LEFT$(NAME_BUFFER,J)+TMP,I,1%)
		FILE_GBL(I) = FAB::IFI 
		STAT = SYS$CMEXEC ( GBL_FIND, FILE_GBL(I) )
		GBL_ADDRESS	= FILE_GBL(I) 		!first in MAP.
		STAT = SYS$CMEXEC ( GBL_READ , GBL_ADDRESS)	!output in MAP.
		TOT_HIT(I)	= GBL_HIT
		TOT_MIS(I)	= GBL_MIS			
	    END IF		    	
	    STAT = LIB$FIND_FILE (FILE_SPEC, RESULT_SPEC, CONTEXT)
	NEXT 
	FILE_COUNT = I
	CALL SYS$EXIT(STAT BY VALUE) UNLESS (STAT = RMS$_NMF)
	CALL LIB$FIND_FILE_END (CONTEXT)
	RETURN
2	END
$CREATE GBL_EXEC.MAR

	$PRVDEF	GLOBAL
	$IMPDEF

	.ENTRY	GBL_FIND, ^M<R2>
	MOVAL	PIO$GW_IIOIMPA, R0		;Impure area address
	MOVZWL	(AP), R2			;Get IFI
	MOVL	IMP$L_IFABTBL(R0), R1		;Table with IFABs
10$:	CMPW	R2, IMP$W_ENTPERSEG(R0)		;In this table segment
	BLEQU	20$				;Branch if ok
	SUBW2	IMP$W_ENTPERSEG(R0), R2		;Reduce index value
	MOVL	(R1), R1			;Next segment
	BRB	10$

20$:	MOVL	(R1)[R2], R0			;Get IFAB address
	MOVL	IFB$L_GBH_PTR(R0), (AP)		;Get Global buffer header
	RET

	.ENTRY	GBL_READ, ^M<>
	MOVL	(AP)+, R0			;Get Global buffer header addr.
	MOVL	GBH$L_HIT(R0), (AP)+		;Hit count
	MOVL	GBH$L_MISS(R0), (AP)+		;Miss count
	MOVL	GBH$L_USECNT(R0), (AP)+		;Users count
	RET
	.END
$!
$!
$!
$BASIC GBL
$MACRO GBL_EXEC+SYS$LIBRARY:LIB/LIB
$LINK  GBL,GBL_EXEC,SYS$SYSTEM:SYS.STB,RMSDEF.STB
$DELETE GBL.OBJ.,GBL_EXEC.OBJ.
$GBL == "$" + F$ENVIRONMENT("DEFAULT") + "GBL.EXE"
    
