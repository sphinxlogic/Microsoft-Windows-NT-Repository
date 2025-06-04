	SUBROUTINE AST_HANDLER (MBX_CHANNEL)
C******************************************************************************
C
C  Author	 : Curtis Rempel
C  Date		 : June 24, 1991
C  Last modified : June 24, 1991
C
C  File used	 : <none>
C
C  Purpose:
C
C	To write system loading information to a mailbox which is associated
C  with the SYS$ANNOUNCE logical name.  This routine is called whenever a
C  read operation completes on the mailbox via an asynchronous system trap.
C
C******************************************************************************

	IMPLICIT	NONE		! catch any undeclared stuff

	INTEGER*2	BJOBCNT		! number of batch jobs
	INTEGER*2	I		! counter
	INTEGER*2	IJOBCNT		! # of interactive jobs
	INTEGER*2	IOSB(4)		! I/O status block
	INTEGER*2	LAV_CHANNEL	! load average driver channel
	INTEGER*2	MBX_CHANNEL	! mailbox channel
	INTEGER*2	MBX_MSG_LEN	! length of mailbox message text
	INTEGER*2	MSG1_LEN	! length of header logical translation
	INTEGER*2	MSG2_LEN	! length of trailer logical translation
	INTEGER*2	TRANSLATION_LEN	! length of logical translation

	INTEGER*4	STATUS		! system service completion status

	INTEGER*4	LIB$DATE_TIME	! get system date/time

	INTEGER*4	SYS$ASSIGN	! assign channel to LAVDRIVER
	INTEGER*4	SYS$DASSGN	! deassign channel to LAVDRIVER
	INTEGER*4	SYS$FAO		! format output
	INTEGER*4	SYS$QIOW	! get data from LAVDRIVER
	INTEGER*4	SYS$TRNLNM	! translate a logical name

	REAL*4		LAV_DATA(9)	! data array from LAVDRIVER

	CHARACTER*2	CRLF		! carriage return / line feed
	CHARACTER*5	AVG(9)		! a load average value string
	CHARACTER*23	DATE_TIME	! current date/time
	CHARACTER*255	MSG1		! header logical translation
	CHARACTER*255	MSG2		! trailer logical translation
	CHARACTER*255	TRANSLATION	! logical translation
	CHARACTER*32768 MBX_MSG		! mailbox message text

	LOGICAL		MSG1_EXISTS	! .TRUE. if ANNOUNCE_MSG1 defined
	LOGICAL		MSG2_EXISTS	! .TRUE. if ANNOUNCE_MSG2 defined

	EXTERNAL	SYS$GW_BJOBCNT	! global symbol from SYS.MAP
	EXTERNAL	SYS$GW_IJOBCNT	! global symbol from SYS.MAP

	EXTERNAL	SHOW_NOTAVAIL	! load not available

	INCLUDE 	'($IODEF)'	! I/O definitions
	INCLUDE		'($LNMDEF)'	! logical name definitions
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

	RECORD /ITMLST/ LNMBUF(3)

	LNMBUF(1).BUFLEN = 255
	LNMBUF(1).ITMCOD = LNM$_STRING		! translation
	LNMBUF(1).BUFADR = %LOC(TRANSLATION)
	LNMBUF(1).RETADR = 0

	LNMBUF(2).BUFLEN = 4
	LNMBUF(2).ITMCOD = LNM$_LENGTH		! translation length
	LNMBUF(2).BUFADR = %LOC(TRANSLATION_LEN)
	LNMBUF(2).RETADR = 0

	LNMBUF(3).END_LIST = 0			! that's all folks...

	CRLF = CHAR(13)//CHAR(10)

C******************************************************************************
C
C   Translate ANNOUNCE_MSG1 and ANNOUNCE_MSG2
C
C******************************************************************************

	STATUS = SYS$TRNLNM (,'LNM$SYSTEM_TABLE','ANNOUNCE_MSG1',,LNMBUF)
	IF (STATUS .EQ. SS$_NOLOGNAM) THEN
		MSG1_EXISTS = .FALSE.
	ELSE IF (.NOT. STATUS) THEN
		CALL LIB$STOP (%VAL(STATUS))
	ELSE 
		MSG1_EXISTS = .TRUE.
		MSG1 = CRLF//TRANSLATION
		MSG1_LEN = TRANSLATION_LEN + 2
	END IF

	STATUS = SYS$TRNLNM (,'LNM$SYSTEM_TABLE','ANNOUNCE_MSG2',,LNMBUF)
	IF (STATUS .EQ. SS$_NOLOGNAM) THEN
		MSG2_EXISTS = .FALSE.
	ELSE IF (.NOT. STATUS) THEN
		CALL LIB$STOP (%VAL(STATUS))
	ELSE 
		MSG2_EXISTS = .TRUE.
		MSG2 = TRANSLATION(1:TRANSLATION_LEN)//CRLF
		MSG2_LEN = TRANSLATION_LEN + 2
	END IF

C******************************************************************************
C
C   Read a set of values from the driver.
C
C******************************************************************************

	STATUS = SYS$ASSIGN ('LAV0:',LAV_CHANNEL,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = SYS$QIOW (,
	2		   %VAL(LAV_CHANNEL),
	2		   %VAL(IO$_READVBLK),
	2		   IOSB,
	2		   ,
	2		   ,
	2		   LAV_DATA,
	2		   36,
	2		   ,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = SYS$DASSGN (%VAL(LAV_CHANNEL))
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

C******************************************************************************
C
C   Build announcement message.
C
C******************************************************************************

	IF (IOSB(1)) THEN
		IF (MSG1_EXISTS)
	2		CALL SEND_MAILBOX (MBX_CHANNEL,MSG1,MSG1_LEN)
		STATUS = LIB$DATE_TIME (DATE_TIME)
		IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))
		CALL GET_JOBCNT (%VAL(%LOC(SYS$GW_IJOBCNT)),
	2			 %VAL(%LOC(SYS$GW_BJOBCNT)),
	2			 IJOBCNT,
	2			 BJOBCNT)
		DO I = 1,9
			WRITE (AVG(I),'(F5.2)') LAV_DATA(I)
		END DO
		STATUS = SYS$FAO ('!/                  '//
	2			  '!20AS!/!/Load averages:   '//
	2			  '1 min   5 min  15 min!/',
	2			  MBX_MSG_LEN,MBX_MSG,
	2			  DATE_TIME(1:20))
		IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

		CALL SEND_MAILBOX (MBX_CHANNEL,MBX_MSG,MBX_MSG_LEN)

		STATUS = SYS$FAO ('          CPU:   !3(8AS)!/'//
	2			  '          I/O:   !3(8AS)!/',
	2			  MBX_MSG_LEN,MBX_MSG,
	2			  AVG(1),AVG(2),AVG(3),
	2			  AVG(7),AVG(8),AVG(9))
		IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

		CALL SEND_MAILBOX (MBX_CHANNEL,MBX_MSG,MBX_MSG_LEN)

		STATUS = SYS$FAO ('Interactive jobs : !3UL!/'//
	2			  'Batch jobs       : !3UL!/',
	2			  MBX_MSG_LEN,MBX_MSG,
	2			  %VAL(IJOBCNT),%VAL(BJOBCNT))
		IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

		CALL SEND_MAILBOX (MBX_CHANNEL,MBX_MSG,MBX_MSG_LEN)

		IF (MSG2_EXISTS)
	2		CALL SEND_MAILBOX (MBX_CHANNEL,MSG2,MSG2_LEN)

	ELSE
		CALL LIB$STOP (%VAL(%LOC(SHOW_NOTAVAIL)))
	END IF

C******************************************************************************
C
C   Drop it in the mailbox.
C
C******************************************************************************

	STATUS = SYS$QIOW (,
	2		   %VAL(MBX_CHANNEL),
	2		   %VAL(IO$_WRITEOF .OR. IO$M_NOW),
	2		   IOSB,
	2		   ,,,,,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))
	IF (.NOT. IOSB(1)) CALL LIB$STOP (%VAL(IOSB(1)))

C******************************************************************************
C
C   Queue another read AST for next time.
C
C******************************************************************************

	CALL QUEUE_AST (MBX_CHANNEL)

	RETURN

	END     
