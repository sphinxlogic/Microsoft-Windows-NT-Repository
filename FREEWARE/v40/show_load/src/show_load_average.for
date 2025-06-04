	PROGRAM	SHOW_LOAD_AVERAGE
C******************************************************************************
C
C  Author	 : Curtis Rempel
C  Date		 : April 23, 1991
C  Last modified : April 23, 1991
C
C  File used	 : <none>
C
C  Purpose:
C
C	To provide an extension to the standard VMS "SHOW" command so that the
C  system load average can be displayed as well.  The program is invoked via a
C  new parameter on the SHOW command: LOAD_AVERAGE.
C
C******************************************************************************

	IMPLICIT	NONE		! catch any undeclared stuff

	INTEGER*2	CHANNEL		! $QIOW channel
	INTEGER*2	CVT_TIME(7)	! components of a time value
	INTEGER*2	I		! counter
	INTEGER*2	IOSB(4)		! I/O status block
	INTEGER*2	MESSAGE_LEN	! message length
	INTEGER*2	N_OUTPUT	! number of processes displayed
	INTEGER*2	N_PROCESSES	! number of processes found
	INTEGER*2	OUTPUT_FILE_LEN	! output file length
	INTEGER*2	PROCESS_INDEX	! index into SORT_ARRAY

	INTEGER*4	BASE_PRIO	! base priority
	INTEGER*4	CPU_TIME	! accumulated CPU time
	INTEGER*4	CURRENT_TIME(2)	! current system time
	INTEGER*4	DEV_STS		! device status
	INTEGER*4	CONNECT_TIME(2)	! connect time
	INTEGER*4	LOGIN_TIME(2)	! process creation time
	INTEGER*4	MODE		! process mode
	INTEGER*4	NODE_NAME_LEN	! length of DECnet node name
	INTEGER*4	OWNER_PID	! process owner PID
	INTEGER*4	PID		! process ID
	INTEGER*4	SEED_PID	! wildcard $GETJPIW
	INTEGER*4	STATUS		! system service completion status
	INTEGER*4       UIC             ! UIC of caller

	INTEGER*4	CLI$GET_VALUE	! get value of /OUTPUT
	INTEGER*4	CLI$PRESENT	! check if /OUTPUT is present

	INTEGER*4	LIB$SUB_TIMES	! subtract two time quadwords

	INTEGER*4	STR$TRIM	! trim trailing spaces
	INTEGER*4	SYS$ASSIGN	! assign channel to LAVDRIVER
	INTEGER*4	SYS$DASSGN	! deassign channel to LAVDRIVER
	INTEGER*4	SYS$GETDVIW	! get LAV0: device information
	INTEGER*4	SYS$GETJPIW	! get process information
	INTEGER*4	SYS$GETSYIW	! get boot time
	INTEGER*4	SYS$GETTIM	! get system time
	INTEGER*4	SYS$NUMTIM	! convert system time into components
	INTEGER*4	SYS$QIOW	! get data from LAVDRIVER

	REAL*4		CPU_MINUTES	! CPU minutes consumed
	REAL*4		CONNECT_HOURS	! connect hours
	REAL*4		LAV_DATA(9)	! data array from LAVDRIVER
	REAL*4		MPH		! CPU minutes per elapsed hour

	CHARACTER*1	HOG		! * if being a CPU hog
	CHARACTER*1	PROCESS_MODE	! -,N,B,I,S
	CHARACTER*12	USERNAME	! a username
	CHARACTER*13	SORT_ARRAY(500)	! 500 processes maximum
	CHARACTER*15	NODE_NAME	! DECnet node name
	CHARACTER*15	PROCESS_NAME	! process name
	CHARACTER*31    HOGS_IDENTIFIER ! identifier to use /HOGS
	CHARACTER*80	IMAGE_NAME	! image being executed
	CHARACTER*80	MESSAGE(0:25)	! descriptive messages
	CHARACTER*80	OUTPUT_FILE	! /OUTPUT= filename

	LOGICAL		TITLE		! printed title yet?

	EXTERNAL	QSORT

	LOGICAL		POSSESS_IDENTIFIER

	EXTERNAL	SHOW_NOCLUSTER
	EXTERNAL	SHOW_NODRIVER
	EXTERNAL	SHOW_NONODE
	EXTERNAL	SHOW_NOTAVAIL
	EXTERNAL	SHOW_NOTAUTH

	INCLUDE 	'($DVIDEF)'
	INCLUDE 	'($IODEF)'
	INCLUDE 	'($JPIDEF)'
	INCLUDE 	'($SSDEF)'
	INCLUDE 	'($SYIDEF)'

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
	RECORD /ITMLST/ JPIBUF(11)
	RECORD /ITMLST/ SYIBUF(2)

	DVIBUF(1).BUFLEN = 4
	DVIBUF(1).ITMCOD = DVI$_DEVSTS
	DVIBUF(1).BUFADR = %LOC (DEV_STS)	! device status
	DVIBUF(1).RETADR = 0

	DVIBUF(2).RETADR = 0                    ! that's all folks...

	JPIBUF(1).BUFLEN = 4
	JPIBUF(1).ITMCOD = JPI$_PID
	JPIBUF(1).BUFADR = %LOC (PID)		! process ID
	JPIBUF(1).RETADR = 0

	JPIBUF(2).BUFLEN = 8
	JPIBUF(2).ITMCOD = JPI$_LOGINTIM
	JPIBUF(2).BUFADR = %LOC (LOGIN_TIME)	! process creation time
	JPIBUF(2).RETADR = 0

	JPIBUF(3).BUFLEN = 12
	JPIBUF(3).ITMCOD = JPI$_USERNAME
	JPIBUF(3).BUFADR = %LOC (USERNAME)	! username
	JPIBUF(3).RETADR = 0

	JPIBUF(4).BUFLEN = 4
	JPIBUF(4).ITMCOD = JPI$_PRIB
	JPIBUF(4).BUFADR = %LOC (BASE_PRIO)	! base priority
	JPIBUF(4).RETADR = 0

	JPIBUF(5).BUFLEN = 80
	JPIBUF(5).ITMCOD = JPI$_IMAGNAME
	JPIBUF(5).BUFADR = %LOC (IMAGE_NAME)	! image name
	JPIBUF(5).RETADR = 0

	JPIBUF(6).BUFLEN = 4
	JPIBUF(6).ITMCOD = JPI$_CPUTIM
	JPIBUF(6).BUFADR = %LOC (CPU_TIME)	! CPU time used
	JPIBUF(6).RETADR = 0

	JPIBUF(7).BUFLEN = 4
	JPIBUF(7).ITMCOD = JPI$_MODE
	JPIBUF(7).BUFADR = %LOC (MODE)		! process mode
	JPIBUF(7).RETADR = 0

	JPIBUF(8).BUFLEN = 15
	JPIBUF(8).ITMCOD = JPI$_PRCNAM
	JPIBUF(8).BUFADR = %LOC (PROCESS_NAME)	! process name
	JPIBUF(8).RETADR = 0

	JPIBUF(9).BUFLEN = 4
	JPIBUF(9).ITMCOD = JPI$_OWNER
	JPIBUF(9).BUFADR = %LOC (OWNER_PID)	! process owner PID
	JPIBUF(9).RETADR = 0

	JPIBUF(10).BUFLEN = 4
	JPIBUF(10).ITMCOD = JPI$_UIC
	JPIBUF(10).BUFADR = %LOC (UIC)		! UIC of caller
	JPIBUF(10).RETADR = 0

	JPIBUF(11).RETADR = 0                   ! that's all folks...

	SYIBUF(1).BUFLEN = 15
	SYIBUF(1).ITMCOD = SYI$_NODENAME
	SYIBUF(1).BUFADR = %LOC (NODE_NAME)	! DECnet node name
	SYIBUF(1).RETADR = %LOC (NODE_NAME_LEN)

	SYIBUF(2).RETADR = 0                    ! that's all folks...

	HOGS_IDENTIFIER = 'SHOW_CPU_HOGS'

C******************************************************************************
C
C   /CLUSTER and /NODE are not supported.
C
C******************************************************************************

	IF (CLI$PRESENT('CLUSTER')) CALL LIB$STOP(%VAL(%LOC(SHOW_NOCLUSTER)))
	IF (CLI$PRESENT('NODE')) CALL LIB$STOP(%VAL(%LOC(SHOW_NONODE)))

C******************************************************************************
C
C   Invent some interesting messages for the load average.
C
C******************************************************************************

	MESSAGE(0) = '.Load?  What load??'
	MESSAGE(1) = '.Load?  What load??'
	MESSAGE(2) = '.Load?  What load??'
	MESSAGE(3) = 'is not very busy'
	MESSAGE(4) = 'has cycles to spare'
	MESSAGE(5) = 'has cycles to spare'
	MESSAGE(6) = 'is keeping up'
	MESSAGE(7) = 'is keeping up'
	MESSAGE(8) = 'is keeping up'
	MESSAGE(9) = 'is starting to get loaded'
	MESSAGE(10) = 'is chugging along'
	MESSAGE(11) = 'is chugging along'
	MESSAGE(12) = 'is slowing down'
	MESSAGE(13) = 'is slowing down'
	MESSAGE(14) = 'is crawling'
	MESSAGE(15) = 'is creeping'
	MESSAGE(16) = 'is out to lunch'
	MESSAGE(17) = 'is out to dinner'
	MESSAGE(18) = 'is almost on fire'
	MESSAGE(19) = 'is resembling a Unix machine'
	MESSAGE(20) = 'is faster than a herd of turtles'
	MESSAGE(21) = 'is slower than a herd of turtles'
	MESSAGE(22) = '.Go home!'
	MESSAGE(23) = '.This is pathetic!'
	MESSAGE(24) = '.Somebody hit the halt button PLEASE!'

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
C   Read a set of values from the driver.
C
C******************************************************************************

	STATUS = SYS$ASSIGN ('LAV0:',CHANNEL,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = SYS$QIOW (,
	2		   %VAL(CHANNEL),
	2		   %VAL(IO$_READVBLK),
	2		   IOSB,
	2		   ,
	2		   ,
	2		   LAV_DATA,
	2		   36,
	2		   ,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = SYS$DASSGN (%VAL(CHANNEL))
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

C******************************************************************************
C
C   Find out which node we're running on and if we need to dump the stuff into
C   a file.
C
C******************************************************************************

	STATUS = SYS$GETSYIW (,,,SYIBUF,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = CLI$GET_VALUE ('OUTPUT',OUTPUT_FILE,OUTPUT_FILE_LEN)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	OPEN (9,FILE=OUTPUT_FILE(1:OUTPUT_FILE_LEN),STATUS='UNKNOWN')

C******************************************************************************
C
C   Display the load averages.
C
C******************************************************************************

	IF (IOSB(1) .AND. IOSB(2) .GE. 12) THEN
		I = JNINT (LAV_DATA(1))		! 1 minute CPU load average
		IF (I .GT. 24) I = 24
		STATUS = STR$TRIM (MESSAGE(I),MESSAGE(I),MESSAGE_LEN)
		IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))
		IF (MESSAGE(I)(1:1) .EQ. '.') THEN
			WRITE (9,1000) MESSAGE(I)(2:MESSAGE_LEN-1)
		ELSE
			WRITE (9,1000) NODE_NAME(1:NODE_NAME_LEN)//' '//
	2			       MESSAGE(I)(1:MESSAGE_LEN)//'.'
		END IF
		WRITE (9,1010) 'CPU',(LAV_DATA(I),I=1,3)
		IF (IOSB(2) .GE. 36)
	2		WRITE (9,1010) 'I/O',(LAV_DATA(I),I=7,9)
	ELSE
		CALL LIB$SIGNAL (%VAL(%LOC(SHOW_NOTAVAIL)))
	END IF

	IF (.NOT. CLI$PRESENT('HOGS')) GO TO 999

C******************************************************************************
C
C   Display the top 5 CPU hogs if we have the SHOW_CPU_HOGS identifier.
C
C******************************************************************************

	STATUS = SYS$GETJPIW (,,,JPIBUF,,,)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))
 
	IF (.NOT. POSSESS_IDENTIFIER(UIC,HOGS_IDENTIFIER)) THEN
		WRITE (9,*)
		CALL LIB$SIGNAL (%VAL(%LOC(SHOW_NOTAUTH)),%VAL(0))
		GO TO 999
	END IF

 	N_PROCESSES = 0

	SEED_PID = -1

10	STATUS = SYS$GETJPIW (,SEED_PID,,JPIBUF,,,)
	IF (STATUS .EQ. SS$_NOMOREPROC) GO TO 20
	IF (STATUS .EQ. SS$_SUSPENDED) GO TO 10
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	DO I = 1,15
		IF (PROCESS_NAME(I:I) .EQ. CHAR(0)) PROCESS_NAME(I:I) = ' '
	END DO

	IF (PROCESS_NAME(1:15) .EQ. 'SWAPPER        ') GO TO 10

	CPU_MINUTES = (FLOATJ (CPU_TIME) / 100.0) / 60.0

	STATUS = SYS$GETTIM (CURRENT_TIME)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = LIB$SUB_TIMES (CURRENT_TIME,LOGIN_TIME,CONNECT_TIME)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	STATUS = SYS$NUMTIM (CVT_TIME,CONNECT_TIME)
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	CONNECT_HOURS = FLOATI (CVT_TIME(3) * 24) +
	2		FLOATI (CVT_TIME(4)) +
	2		(FLOATI (CVT_TIME(5)) / 60.0) +
	2		(FLOATI (CVT_TIME(6)) / 3600.0)

	IF (CONNECT_HOURS .EQ. 0.0) GO TO 10

	MPH = CPU_MINUTES / CONNECT_HOURS

	N_PROCESSES = N_PROCESSES + 1

	WRITE (SORT_ARRAY(N_PROCESSES),'(F5.2,Z8)') MPH,PID
	IF (SORT_ARRAY(N_PROCESSES)(1:1) .EQ. ' ')
	2	SORT_ARRAY(N_PROCESSES)(1:1) = '0'

	GO TO 10

20	CALL QSORT (SORT_ARRAY,N_PROCESSES)

	TITLE = .FALSE.

	PROCESS_INDEX = N_PROCESSES + 1
	N_OUTPUT = 0

30	PROCESS_INDEX = PROCESS_INDEX - 1

	IF (PROCESS_INDEX .GT. N_PROCESSES .OR. N_OUTPUT .EQ. 5) GO TO 999

	READ (SORT_ARRAY(PROCESS_INDEX)(6:13),'(Z8)') PID

	STATUS = SYS$GETJPIW (,PID,,JPIBUF,,,)
	IF (STATUS .EQ. SS$_NONEXPR .OR.
	2   STATUS .EQ. SS$_SUSPENDED) GO TO 30
	IF (.NOT. STATUS) CALL LIB$STOP (%VAL(STATUS))

	DO I = 1,15
		IF (PROCESS_NAME(I:I) .EQ. CHAR(0)) PROCESS_NAME(I:I) = ' '
	END DO

	READ (SORT_ARRAY(PROCESS_INDEX)(1:5),'(F5.2)') MPH

	IF (MODE .EQ. JPI$K_OTHER) PROCESS_MODE = '-'
	IF (MODE .EQ. JPI$K_NETWORK) PROCESS_MODE = 'N'
	IF (MODE .EQ. JPI$K_BATCH) PROCESS_MODE = 'B'
	IF (MODE .EQ. JPI$K_INTERACTIVE) PROCESS_MODE = 'I'
	IF (OWNER_PID .NE. 0) PROCESS_MODE = 'S'

	DO I = 1,80
		IF (IMAGE_NAME(I:I) .EQ. CHAR(0)) IMAGE_NAME(I:I) = ' '
	END DO

	I = 80

40	IF (IMAGE_NAME(I:I) .EQ. ']') GO TO 50

	I = I - 1

	IF (I .NE. 0) GO TO 40

50	IF (I .EQ. 0) THEN
		IMAGE_NAME = 'DCL'
	ELSE IF (I .EQ. 80) THEN
		IMAGE_NAME = '<error>'
	ELSE
		IMAGE_NAME = IMAGE_NAME(I+1:80)
		IMAGE_NAME = IMAGE_NAME(1:INDEX(IMAGE_NAME,'.EXE')-1)
	END IF

	IF (IMAGE_NAME(1:8) .EQ. 'LOGINOUT') GO TO 30

	IF (.NOT. TITLE) THEN
		WRITE (9,1020)
		TITLE = .TRUE.
	END IF

	IF (MPH .GT. 30.0) THEN
		HOG = '*'
	ELSE
		HOG = ' '
	END IF

	WRITE (9,1030) USERNAME,PROCESS_NAME,PID,IMAGE_NAME(1:20),
	2	       PROCESS_MODE,BASE_PRIO,MPH,HOG

	N_OUTPUT = N_OUTPUT + 1

	GO TO 30

999	WRITE (9,*)

	CLOSE (9)

C******************************************************************************
C
C   FORMAT statements.
C
C******************************************************************************

1000	FORMAT (/,1X,A,//,T11,'1 min   5 min  15 min',/)
1010	FORMAT (1X,A,T10,3(F6.2,2X))
1020	FORMAT (//,' Top 5 CPU Hogs:',//,
	2	1X,'Username',T15,'Process Name',T31,'PID',T40,'Image',
	2	T61,'Mode',T68,'Prio',T75,'MPH',/,
	2	1X,'--------',T15,'------------',T31,'--------',T40,
	2	'--------------------',
	2	T61,'----',T68,'----',T75,'-----')
1030	FORMAT (1X,A12,T15,A15,T31,Z8,T40,A20,T62,A1,T69,I2,T75,F5.2,A1)

9999	END
