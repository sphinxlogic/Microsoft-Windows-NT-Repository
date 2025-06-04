	LOGICAL FUNCTION POSSESS_IDENTIFIER (UIC,IDENTIFIER)
C******************************************************************************
C
C  Author	 : Curtis Rempel
C  Date 	 : June 17, 1991
C  Last modified : June 17, 1991
C
C  Files used	 : SYS$SYSTEM:RIGHTSLIST.DAT (implicit)
C
C  Purpose:
C
C	This function returns .TRUE. if the given UIC possesses the given
C  identifier.
C
C******************************************************************************

	IMPLICIT	NONE		  ! catch any undeclared stuff

	INTEGER*4	AN_IDENTIFIER	  ! an identifier retrieved
	INTEGER*4	CONTEXT 	  ! context for SYS$FIND_HELD
	INTEGER*4	HOLDER(2)	  ! retrieve identifiers for this guy
	INTEGER*4	IDENTIFIER_VALUE  ! identifier we're looking for
	INTEGER*4	STATUS		  ! system service completion status
	INTEGER*4	UIC		  ! UIC of caller

	INTEGER*4	SYS$ASCTOID	  ! convert identifer to value
	INTEGER*4	SYS$FIND_HELD	  ! retrieve identifiers for a UIC

	CHARACTER*31	IDENTIFIER	  ! target identifier

	INCLUDE 	'($SSDEF)'

C******************************************************************************
C
C  Convert the identifier to binary.
C
C******************************************************************************

	STATUS = SYS$ASCTOID (IDENTIFIER,IDENTIFIER_VALUE,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	HOLDER(1) = UIC
	HOLDER(2) = 0
	
	CONTEXT = 0

C******************************************************************************
C
C  Traverse the list of identifiers this guy has to check for match.
C
C******************************************************************************

	POSSESS_IDENTIFIER = .FALSE.

10	STATUS = SYS$FIND_HELD (HOLDER,AN_IDENTIFIER,,CONTEXT)

	IF (STATUS .EQ. SS$_NOSUCHID) THEN
		CALL SYS$FINISH_RDB (CONTEXT)
		RETURN
	ELSE IF (IDENTIFIER_VALUE .EQ. AN_IDENTIFIER) THEN
		POSSESS_IDENTIFIER = .TRUE.
		CALL SYS$FINISH_RDB (CONTEXT)
		RETURN
	ELSE IF (.NOT. STATUS) THEN
		CALL LIB$STOP (%VAL(STATUS))
	END IF

	GO TO 10

	END
