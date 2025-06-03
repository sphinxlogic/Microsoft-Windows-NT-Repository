	INTEGER*4 FUNCTION INIT_CLI (CLI_TABLE, VERB_NAME)
C+
C
C FACILITY:  INIT_CLI.FOR  - DEC FORTRAN/OpenVMS
C            
C FUNCTIONAL DESCRIPTION:
C
C       STATUS = INIT_CLI(CLI_TABLE, VERB_NAME)
C
C       This routine allows you to define a command as either a foreign
C   command (via PROG :== $<wherever>PROG), or as a real command (via 
C   the SET COMMAND command).
C
C   	This routine is translated by me <jshen1@ford.com>.  Original
C   source is supplied by Ed Wilts<EWILTS@galaxy.gov.bc.ca> in C and
C   was written by Joe Meadows <?>.
C
C FORMAL PARAMETERS:
C
C     CLI_TABLE - The address of the command tables.  (you must do a 
C		  SET COMMAND/OBJECT and link the resulting object file
C		  into your code.
C   
C     VERB_NAME - The name of the verb.
C
C COMMON BLOCKS:
C
C       -NONE -
C
C-
	IMPLICIT INTEGER(A-Z)
	CHARACTER*(*)	VERB_NAME
	CHARACTER*512 	COMMAND
	INTEGER*4	CLI$GET_VALUE, CLI$DCL_PARSE

	STS = CLI$GET_VALUE( '$VERB', COMMAND, COMMAND_LENGTH)
	IF ( STS .AND. 1 .AND. COMMAND_LENGTH ) THEN
          IF ( VERB_NAME .EQ. COMMAND(:COMMAND_LENGTH)) THEN
	    INIT_CLI = 1	  ! the command must have been properly defined
	    RETURN
	  END IF	  
	END IF

	STS = CLI$GET_VALUE ('$LINE', COMMAND, COMMAND_LENGTH)
	IF ( .NOT. STS ) THEN
	  INIT_CLI = STS
	  RETURN
	END IF

	I = 1
	DO WHILE ((I .LE. COMMAND_LENGTH)
	2	.AND. (COMMAND(I:I) .NE. ' ') 
	2	.AND. (COMMAND(I:I) .NE. '/'))
	  IF ( I .LE. LEN(VERB_NAME)) THEN
	    COMMAND(I:I) = VERB_NAME(I:I)	  
	  ELSE
	    COMMAND(I:I) = ' '
	  END IF
	  
          I = I + 1
	END DO

	IF (I .LE. LEN(VERB_NAME)) TYPE *, 
	2	'SHS-E-VERB2LONG, Verb /', VERB_NAME, '/ was not smaller '//
	2	'than $LINE prefix'

	INIT_CLI = CLI$DCL_PARSE( COMMAND(:COMMAND_LENGTH),
	2			  %VAL(%LOC(CLI_TABLE)),,)

	RETURN
	END 
