	PROGRAM	ANNOUNCE_DAEMON
C******************************************************************************
C
C  Author	 : Curtis Rempel
C  Date		 : June 21, 1991
C  Last modified : June 24, 1991
C
C  File used	 : <none>
C
C  Purpose:
C
C	To dynamically update SYS$ANNOUNCE with load average and login
C  information.
C
C******************************************************************************

	IMPLICIT	NONE		! catch any undeclared stuff

	INTEGER*2	MBX_CHANNEL	! channel to SYS$ANNOUNCE mailbox
	
	INTEGER*4	DEV_STS		! device status vector
	INTEGER*4	STATUS		! system service completion status

	INTEGER*4	SYS$CREMBX	! create a mailbox
	INTEGER*4	SYS$GETDVIW	! test if driver installed
	INTEGER*4	SYS$HIBER	! go to sleep

	EXTERNAL	SHOW_NODRIVER	! driver not installed

	INCLUDE 	'($DVIDEF)'	! $GETDVI definitions
	INCLUDE 	'($SSDEF)'	! system service definitions

	STRUCTURE /ITMLST/
	  UNION
	    MAP
	      INTEGER*2 BUFLEN,ITMCOD
	      INTEGER*4 BUFADR,RETADR
	    END MAP
	    MAP
	      INTEGER*4 END_LIST
	    END MAP
	  END UNION
	END STRUCTURE

	RECORD /ITMLST/ DVIBUF(2)

	DVIBUF(1).BUFLEN = 4
	DVIBUF(1).ITMCOD = DVI$_DEVSTS
	DVIBUF(1).BUFADR = %LOC (DEV_STS)	! device status
	DVIBUF(1).RETADR = 0

	DVIBUF(2).RETADR = 0                    ! that's all folks...

C******************************************************************************
C
C   Check if LAV0: exists.
C
C******************************************************************************

	STATUS = SYS$GETDVIW (,,
	2		      'LAV0:',
	2		      DVIBUF,
	2		      ,,,)
	IF (STATUS .EQ. SS$_NOSUCHDEV) THEN
		CALL LIB$STOP(%VAL(%LOC(SHOW_NODRIVER)))
	ELSE IF (.NOT. STATUS) THEN
		CALL LIB$STOP (%VAL(STATUS))
	END IF

C******************************************************************************
C
C   Create a mailbox, queue a read AST on the channel, and go to sleep.
C   The protection mask X%2202 denies write access to world, group, and
C   system.
C
C******************************************************************************

	STATUS = SYS$CREMBX (%VAL(1),
	2		     MBX_CHANNEL,
	2		     ,
	2		     ,
	2		     %VAL('2202'X),
	2		     ,
	2		     'SYS$ANNOUNCE_MBX')
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	CALL QUEUE_AST (MBX_CHANNEL)

	STATUS = SYS$HIBER ()
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

9999	END
