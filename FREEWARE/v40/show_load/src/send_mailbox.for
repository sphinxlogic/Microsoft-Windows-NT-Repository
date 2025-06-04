	SUBROUTINE SEND_MAILBOX (MBX_CHANNEL,MBX_MSG,MBX_MSG_LEN)
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
C	To write a buffer into a mailbox specified by the channel argument.
C
C******************************************************************************

	IMPLICIT	NONE		! catch any undeclared stuff

	INTEGER*2	IOSB(4)		! I/O status block
	INTEGER*2	MBX_CHANNEL	! mailbox channel
	INTEGER*2	MBX_MSG_LEN	! length of mailbox message

	INTEGER*4	STATUS		! system service completion status

	INTEGER*4	SYS$QIOW	! queue I/O request

	CHARACTER*32768	MBX_MSG		! mailbox message

	INCLUDE		'($IODEF)'	! I/O definitions

	STATUS = SYS$QIOW (,
	2		   %VAL(MBX_CHANNEL),
	2		   %VAL(IO$_WRITEVBLK),
	2		   IOSB,
	2		   ,,
	2		   %REF(MBX_MSG),
	2		   %VAL(MBX_MSG_LEN),
	2		   ,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))
	IF (.NOT. IOSB(1)) CALL LIB$STOP (%VAL(IOSB(1)))

	RETURN

	END
