        OPTIONS         /EXTEND_SOURCE
        SUBROUTINE      SHS_FIND_HELP (KEYWORD, ADD_USER_HELP_LIBRARY)
C+
C
C FUNCTIONAL DESCRIPTION:
C
C       SHS_FIND_HELP.FOR - To satisfies an initial help request for
C		SHS.  The help module is searched in such order: 
C             	`ADD_USER_HELP_LIBRARY', root library - SYS$HELP:HELPLIB.HLB,
C             	system, group, and process logical name table for 
C               default help libraries
C
C FORMAL PARAMETERS:
C
C     KEYWORD - Character String: keyword to search for help
C     ADD_USER_HELP_LIBRARY - Additional user specified help library
C			      (Character String)
C
C COMMON BLOCKS:
C
C     -NONE-
C
C AUTHOR:
C
C     James Shen (jshen1@ford.com)
C
C CREATION DATE: Sept, 1995
C
C MODIFIED BY:
C
C        : VERSION
C 01    -
C
C-
	IMPLICIT 	INTEGER*4(A-Z)
	
	CHARACTER*(*)	KEYWORD
	CHARACTER*(*)	ADD_USER_HELP_LIBRARY

	INCLUDE		'($HLPDEF)'
	INCLUDE		'($RMSDEF)'

	EXTERNAL	LIB$PUT_OUTPUT,
	2		LIB$GET_INPUT
	
	INTEGER*4	LIB$FIND_FILE
	INTEGER*4	LBR$OUTPUT_HELP	

	INTEGER*4	STATUS,
	2		FLAGS,
	2		CONTEXT /0/

	CHARACTER*255	RETURN_FILE_NAME

	STATUS = LIB$FIND_FILE( ADD_USER_HELP_LIBRARY,
	2			RETURN_FILE_NAME,
	2			CONTEXT,
	2			,,)

	IF ( STATUS .EQ. RMS$_NORMAL) THEN
	  FLAGS = (HLP$M_PROMPT) 
	  STATUS = LBR$OUTPUT_HELP( LIB$PUT_OUTPUT,
	2			   ,
	2			   KEYWORD,
	2			   ADD_USER_HELP_LIBRARY,
	2			   FLAGS,
	2			   LIB$GET_INPUT)

	  IF ( .NOT. STATUS ) CALL LIB$SIGNAL(%VAL(STATUS))
        ELSE IF ( STATUS .EQ. RMS$_FNF) THEN
	  ! we want prompt for input
	  ! we want enable searching process logical name table for default help library            
	  ! we want enable searching group logical name table for default help library              
	  ! we want enable searching system logical name table for default help library             
	  ! we want list of default libraries available is output with the list of topics available.

	  FLAGS =(HLP$M_PROMPT) +
	2        (HLP$M_PROCESS) +
	2        (HLP$M_GROUP) +	
	2        (HLP$M_SYSTEM) +
	2        (HLP$M_LIBLIST)	
                                        
	  STATUS = LBR$OUTPUT_HELP(LIB$PUT_OUTPUT,
	2			  ,
	2			  KEYWORD,
	2			  'SYS$HELP:HELPLIB.HLB',
	2			  FLAGS,
	2			  LIB$GET_INPUT)

	  IF ( .NOT. STATUS ) CALL LIB$SIGNAL(%VAL(STATUS))
	ELSE
	  CALL LIB$SIGNAL(%VAL(STATUS))
	END IF

	RETURN

	END
