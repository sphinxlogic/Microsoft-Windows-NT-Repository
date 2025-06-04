        SUBROUTINE QUEUE_AST (MBX_CHANNEL)
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
C	To queue a read AST on the specified channel so that our favorite
C  subroutine is invoked when the AST fires.
C
C******************************************************************************

	IMPLICIT	NONE		! catch any undeclared stuff

	INTEGER*2	IOSB(4)		! I/O status block
	INTEGER*2	MBX_CHANNEL	! channel previously assigned

	INTEGER*4	STATUS		! system service completion status

	INTEGER*4	SYS$QIOW	! queue I/O request

	EXTERNAL	AST_HANDLER	! routine we want invoked

	INCLUDE		'($IODEF)'	! I/O definitions

	STATUS = SYS$QIOW (,
	2		   %VAL(MBX_CHANNEL),
	2		   %VAL(IO$_SETMODE .OR. IO$M_READATTN),
	2		   IOSB,
	2		   ,,
	2		   AST_HANDLER,
	2		   MBX_CHANNEL,
	2		   ,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))
	IF (.NOT. IOSB(1)) CALL LIB$STOP (%VAL(IOSB(1)))

	RETURN

	END
