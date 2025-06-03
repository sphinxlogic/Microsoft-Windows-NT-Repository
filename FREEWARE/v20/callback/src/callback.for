        PROGRAM callback
        IMPLICIT NONE
C
C  Written by Robert Eden   robert@cpvax.tu.com
C             Comanche Peak Steam Electric Station
C             (817) 897-0491
C 
C If you use this program, why not drop me a line and let me know!
C
C example:
C	$ callback :== $sys$login:callback.exe
C       $ callback 9w 555-1212
C

        INCLUDE '($IODEF)/NOLIST'
        INCLUDE '($TTDEF)/NOLIST'
        INCLUDE '($TT2DEF)/NOLIST'

C FUNCTION DEFINITIONS
 	INTEGER SYS$ASSIGN,SYS$QIOW,LIB$GET_FOREIGN,LIB$GET_SYMBOL
 	INTEGER SYS$DASSGN,SEND,RECEIVE

	STRUCTURE /CHAR_BUFF_TYPE/
	    BYTE CLASS,TYPE
	    INTEGER*2 BUFF_SIZ
	    INTEGER*4 CHAR1 ! NOTE: BITS 24-31 CONTAIN PAGE SIZE
	    INTEGER*4 CHAR2
	    END STRUCTURE    !CHAR_BUFF_TYPE

	RECORD /CHAR_BUFF_TYPE/ CHAR_BUFF

C LOCAL CONSTANTS
        CHARACTER CR*1 , LF*1 

C LOCAL VARIABLES
        INTEGER STATUS,CHAN,STRLEN,I
        INTEGER*2 CMDLEN

        CHARACTER RETNUM*20,DIAL_COMMAND*80,STRING*80

        LOGICAL   DEBUG

C DEFINE CONSTANTS AND INITIAL CONDITIONS
        CR = CHAR(13)
        LF = CHAR(10)
        DEBUG = .FALSE.

C CHECK FOR DEBUG SYMBOL SET, IF SO, OPEN DEBUG FILE
 	CALL LIB$GET_SYMBOL('CALLBACK$DEBUG',STRING,STRLEN,,)
        IF (STRLEN.NE.0) DEBUG = .TRUE.

        IF (DEBUG) OPEN (UNIT=1,FILE=STRING(1:STRLEN),STATUS='NEW')

C GET OUR PARAMETER
        status = LIB$GET_FOREIGN( STRING,'(retnum) ',CMDLEN,)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
            RETNUM = STRING(1:CMDLEN)
            IF (DEBUG) WRITE(1,*) 'RETNUM: ',RETNUM

C OPEN CONNECTION TO MODEM
        STATUS = SYS$ASSIGN('TT',CHAN,,)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

	STATUS = SYS$QIOW(,%VAL(CHAN),%VAL(IO$_SENSEMODE),,,,
	1                  CHAR_BUFF,%VAL(12),,,,,)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

	CHAR_BUFF.CHAR1 = IBSET (CHAR_BUFF.CHAR1,  TT$V_NOBRDCST)
	CHAR_BUFF.CHAR1 = IBSET (CHAR_BUFF.CHAR1,  TT$V_NOTYPEAHD)
	CHAR_BUFF.CHAR2 = IBSET (CHAR_BUFF.CHAR2, TT2$V_PASTHRU)

	STATUS = sys$qiow(,%VAL(CHAN),%VAL(IO$_SETMODE),,,,
	1                  CHAR_BUFF,%VAL(12),,,,,)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
	
        STATUS = SEND(CHAN,'Stand by ',9)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

C
C GET CONTROL OF MODEM!
C
        CALL LIB$WAIT(2.0)    
        STATUS = SEND(CHAN,'+++',3)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
        CALL LIB$WAIT(2.0)

C
C set modem configs the way we likem..
C
C   S0=0 - don't answer
C   E0 - turn echo off
C   Q0 - disable quiet mode
C   V1 - enable verbose mode
C  &S0 - keep DSR high  (don't disconnect)
C  s37 - fallback to v32
C
C   H0 - hang up!
C
        STRING = 'S0=0 Q0 V1 &S0 E0 H0 s37=7'//CR
        STRLEN = INDEX (STRING,CR) 

        STATUS = SEND(CHAN,'AT',2)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	CALL LIB$WAIT(0.5)

        STATUS = SEND(CHAN,STRING,STRLEN)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	IF (DEBUG) WRITE(1,*) 'HANGUP  COMMAND:',STRING(1:STRLEN)

        STATUS = RECEIVE(CHAN,STRING,STRLEN)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	IF (DEBUG) WRITE(1,*) 'HANGUP RESPONCE:',STRING(1:STRLEN)

C
C MODEM HUNG UP... ALL OURS.. LET'S CALL OUR PARTY BACK
C                   
        DIAL_COMMAND = 'TD '//RETNUM//CR
              I = INDEX(DIAL_COMMAND,CR)

 	CALL LIB$WAIT(2.0) ! let phone stabliize

        STATUS = SEND(CHAN,'AT',2)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	CALL LIB$WAIT(0.5)

        STATUS = SEND(CHAN,DIAL_COMMAND,I)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
        IF (DEBUG) WRITE(1,*) 'DIAL_COMMAND ',DIAL_COMMAND(1:I-1)

        I = 0
        DO WHILE (I.LT.5)
	    STATUS = RECEIVE(CHAN,STRING,STRLEN)
		IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	    IF (DEBUG) WRITE(1,*) 'CALL RESPONCE: ',STRING(1:STRLEN)
	    I = I + 1
	    IF (INDEX(STRING,'CONNECT').NE.0) I = 99
	    IF (INDEX(STRING,'NO CARRIER').NE.0) I = 50
	    END DO

	IF (I.LT.99) GOTO 8000 ! NO CONNECTION, GET OUTA HERE	

C
C WE'RE BACK NOW... GET THE MODEM'S ATTENTION AGAIN
C
        CALL LIB$WAIT(2.0)   

        STATUS = SEND(CHAN,'WELCOME BACK, ONE SEC ',22)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

        CALL LIB$WAIT(2.0)   
        STATUS = SEND(CHAN,'+++',3)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
        CALL LIB$WAIT(2.0)   

C
C NOW RESET THE MODEM HANGUP STUFF AND RETURN TO CALLER
C 
C
C &S1 - track DSR signal
C other parameters reset automatically at logocc
C
	 STRING = '&S1 O'//CR
	 STRLEN = INDEX(STRING,CR)

         STATUS = SEND(CHAN,'AT',2)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	 CALL LIB$WAIT(0.5)

         STATUS = SEND(CHAN,STRING,STRLEN)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
  	 IF (DEBUG) WRITE(1,*) 'RESET SEQUENCE: ',STRING(1:STRLEN)

        I = 0
        DO WHILE (I.LT.4)
	    STATUS = RECEIVE(CHAN,STRING,STRLEN)
		IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 	    IF (DEBUG) WRITE(1,*) 'RESET RESPONCE: ',STRING(1:STRLEN)
	    I = I + 1
	    IF (strlen.eq.0) I = i + 2
	    IF (INDEX(STRING,'CONNECT').NE.0) I = 99
	    IF (INDEX(STRING,'NO CARRIER').NE.0) I = 50
	    END DO

	IF (I.LT.99) GOTO 8000 ! NO CONNECTION, GET OUTA HERE	

C
C ALL done! reset terminal modes thank the user and return
C
	CHAR_BUFF.CHAR1 = IBCLR (CHAR_BUFF.CHAR1,  TT$V_NOBRDCST)
	CHAR_BUFF.CHAR1 = IBCLR (CHAR_BUFF.CHAR1,  TT$V_NOTYPEAHD)
	CHAR_BUFF.CHAR2 = IBCLR (CHAR_BUFF.CHAR2, TT2$V_PASTHRU)

	STATUS = sys$qiow(,%VAL(CHAN),%VAL(IO$_SETMODE),,,,
	1                  CHAR_BUFF,%VAL(12),,,,,)
            IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

        STATUS = SEND( CHAN, 'Thank you'//cr//LF,11)
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

        STATUS = SYS$DASSGN(%VAL(CHAN))
             IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
 
 	GOTO 9000

C
C OH NOOOOOOOOOOOOOOOOOOOO
C Something went wrong.  Disconnect user (will also reset modem)
C just in case...
C
8000    print *, 'CALLBACK-F-NONRECOVER, unrecoverable error'
 	IF (DEBUG) WRITE (1,*) 'NON-RECOVERABLE ERROR!'
        IF (DEBUG) CLOSE(1)
        CALL SYS$DELPRC(,)  ! GOOD NIGHT!

9000    CONTINUE
 	IF (DEBUG) CLOSE(1)
        END


        INTEGER FUNCTION SEND(CHAN,STRING,STRLEN)
C
C send a string to CHAN
C
        IMPLICIT NONE
        INCLUDE '($IODEF)/NOLIST'

        INTEGER CHAN,STATUS,STRLEN,SYS$QIOW
        INTEGER*2 IOSB(4)
        
        CHARACTER STRING*(*)

        STATUS = SYS$QIOW( %VAL(0), ! EVENT FLAGS,
     +           %VAL(CHAN) ,
     +           %VAL(IO$_WRITEVBLK + IO$M_NOFORMAT),
     +           %REF(IOSB),
     +           %VAL(0),%VAL(0), ! ASTADR,ASTPRM,
     +           %REF(STRING),%VAL(STRLEN), , , , )

        SEND = STATUS  
        RETURN
 	END        


        INTEGER FUNCTION RECEIVE(CHAN,STRING,STRLEN)
C
C receive a string from CHAN.
C 120 second timeout, ignore blank lines
C
        IMPLICIT NONE
        INCLUDE '($IODEF)'

        INTEGER CHAN,STATUS,STRLEN,SYS$QIOW
        INTEGER*2 IOSB(4)
        
        CHARACTER STRING*(*)


10     STATUS = SYS$QIOW( %VAL(0), ! EVENT FLAGS,
     +          %VAL(CHAN) ,
     +          %VAL(IO$_READVBLK+IO$M_TIMED+IO$M_CVTLOW+IO$M_NOECHO),
     +          %REF(IOSB),
     +          %VAL(0),%VAL(0), ! ASTADR,ASTPRM,
     +          %REF(STRING),%VAL(LEN(STRING)),
     +          %VAL(120), , , )

        IF ((IOSB(1).EQ.1).AND.            ! IGNORE LF LINES
     +      (IOSB(2).EQ.1).AND.            !
     +      (IOSB(3).EQ.13).AND.
     +      (STRING(1:1).EQ.CHAR(10)))  GOTO 10  


        STRLEN = IOSB(2)

        IF  (STATUS .AND. (STRING(1:1).EQ.CHAR(13))) THEN  ! DROP STARTING CR
                         STRING = STRING(2:)
                         STRLEN = STRLEN - 1
                         ENDIF

        IF (STATUS .AND. (STRING(1:1).EQ.CHAR(10))) THEN   ! DROP STARTING LF
                         STRING = STRING(2:)
                         STRLEN = STRLEN - 1
                         ENDIF

        IF ((IOSB(1).EQ.1).AND.(STRLEN.LE.0)) GOTO 10      ! IGNORE BLANK LINE

        RECEIVE = STATUS  
        RETURN
 	END        

