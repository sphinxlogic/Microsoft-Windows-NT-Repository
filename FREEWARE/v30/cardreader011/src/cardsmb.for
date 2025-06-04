
	PROGRAM CARDSMB
C+
C 
C ABSTRACT: 
C 
C        This program is a VAX/VMS server symbiont to provide a 
C        queue on the VAX that acts as a cardreader.  A second queue is 
C        also available to mail output back to the original user.
C
C	 The cardreader queue should be assigned device CARDIN.
C        The mailback   queue should be assigned device MAILOUT.
C
C        This symbiont can connect to up to 16 queues, but only 1 
C        is active at any one time.
C        
C AUTHOR(S): 
C 
C        10-SEP-1991 Robert Eden
C 
C COMMON BLOCKS:
C  
C     State information for each stream is maintained in an array of
C     structures stored in a common block.  See the file COMMON.FOR
C     for information on this block.   This common block is used by
C     all subroutines.
C  
C 
C MODIFICATION HISTORY:
C 
C        Date     | Name  | Description
C ----------------+-------+-----------------------------------------------------
C 02-MAR-1993     |  RME  | added SYS$SNDOPR interface for INPSMB errors
C ----------------+-------+-----------------------------------------------------
C [change_entry]
C-
CDEC$ TITLE 'CARDIN READER SYMBIONT'
CDEC$ IDENT 'Version 1.1'

	IMPLICIT NONE
        INCLUDE 'COMMON/LIST'
 	INCLUDE 'SMBDEF'
 	INCLUDE '($PRVDEF)'

        PARAMETER  DBGFILE      = 'SYS$COMMON:[SYSEXE]CARDSMB.DBG'
 	PARAMETER  SS$_NORMAL   = 1
 	INTEGER  STATUS,SYS$TRNLNM,SYS$SETPRV,I
        INTEGER SYS$CREPRC,SYS$CREMBX,SYS$DELMBX
 	EXTERNAL HANDLE_ERROR,MSG_AST

C CREATE OUR OWN ERROR HANDLER
 	CALL LIB$ESTABLISH(HANDLE_ERROR)

C SET DEBUG MODE
        DEBUG  = .FALSE.
 	STATUS = SYS$TRNLNM(,'LNM$SYSTEM','CARDSMB$DEBUG',,)
        IF (STATUS) THEN 
 		    DEBUG = .TRUE.
 		    OPEN(DBG_LU,FILE=DBGFILE,SHARED,
     +                          CARRIAGECONTROL='LIST',STATUS='NEW')
     	            WRITE (DBG_LU,*) 'IBMPRTSMB - DEBUG MODE STARTED!'
		    ENDIF

C
C TURN ON CMKRNL SO WE CAN REQUEUE
C
  	STATUS = SYS$SETPRV( %VAL(1) , ! ENABLE
     +                       %REF( PRV$M_NETMBX + PRV$M_TMPMBX +
     +                             PRV$M_CMKRNL + PRV$M_SYSPRV ),
     +                       %VAL(1),  ! SET IT FOR GOOD!
     +                       %VAL(0))  ! DON'T CARE ABT OLD VALUE
	IF (DEBUG) WRITE (DBG_LU,*) 'SYS$SETPRV - STATUS',STATUS
 	IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

C
C create mailboxs and CARDREADER process
C
        STATUS = sys$crembx (,inpsmb_inp_chan,,,,,'INPSMB$MBX')
              IF (DEBUG) WRITE(DBG_LU,*) 'CREMBX1: ',STATUS
    	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

        STATUS = sys$crembx (,inpsmb_err_chan,,,,,'ERRSMB$MBX')
              IF (DEBUG) WRITE(DBG_LU,*) 'CREMBX2: ',STATUS
    	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
        
C
C initiate AST routine to handle errors
C
        CALL inpsmb_err_queue

C
C start subprocess to queue jobs
C
        STATUS = sys$creprc (,   !  PID
     1               'SYS$SYSTEM:INPSMB.EXE', 
     1               'INPSMB$MBX:', 
     1               'NLA0:', 
     1               'ERRSMB$MBX',
     1                ,           ! privs 
     1                ,           ! quota
     1               'CARDREADER',  
     1               %val(4),     ! priority
     1               ,            ! uic
     1               ,            ! termination mailbox
     1                )           ! options
              IF (DEBUG) WRITE(DBG_LU,*) 'CREPRC: ',STATUS
    	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

      OPEN(MBX_LU,FILE='INPSMB$MBX:',
     +           CARRIAGECONTROL='LIST',STATUS = 'OLD')

C
C INITIALIZE SYMBIONT
C
 	STATUS = SMB$INITIALIZE( %REF(SMBMSG$K_STRUCTURE_LEVEL),
     +                           %REF(MSG_AST), ! OUR AST ROUTINE
     +                           %REF(NUM_STREAMS)) ! USE ALL 16 STREAMS
	IF (DEBUG) WRITE (DBG_LU,*) 'SMB$INITIALIZE - STATUS',STATUS
 	IF (STATUS.NE.SS$_NORMAL) CALL SYS$EXIT( %VAL(STATUS) )

C
C MAIN LOOP!
C
100     IF (TASKS.EQ.0) CALL SYS$HIBER()  ! NOTHING TO DO, SO WAIT
 	DO I=1,NUM_STREAMS
 	    IF (DEBUG) WRITE(DBG_LU,*) 'MAIN_LOOP:',I,STREAM(I).STATE
 	    CUR_ID = I
 	    IF ((STREAM(I).STATE.EQ.STRM_START).AND.
     +         (STREAM(I).DEV_NAM.EQ.'CARDIN')) CALL RUN_CARDREADER(I)
 	    IF ((STREAM(I).STATE.EQ.STRM_START).AND.
     +         (STREAM(I).DEV_NAM.EQ.'MAILOUT')) CALL RUN_MAILOUT(I)
 	    IF (STREAM(I).STATE.EQ.STRM_ABORT)   CALL ABORT_TASK(I)
            END DO
        WRITE(MBX_LU,*) '$EOJ'  ! make sure we've cleared the reader
 	GOTO 100

   	END

 	SUBROUTINE MSG_AST
C+
C 
C FUNCTIONAL DESCRIPTION:	*** AST CONTEXT ***
C 
C    This AST routine is used to process messages received from the
C    JOB_CONTROLLER.  It calls routines to notifiy streams to abort,
C    and other routines to set up structures for a new job.
C 
C 
C-
 	IMPLICIT NONE
 	INCLUDE 'COMMON'
 	INCLUDE 'SMBDEF'

 	INTEGER STATUS,ID,REQUEST

 	PARAMETER SS$_NORMAL = 1

 	IF (DEBUG) WRITE (DBG_LU,*) 'MSG_AST ACTIVE'

 	STATUS = SMB$CHECK_FOR_MESSAGE()
 	IF (STATUS.NE.0) THEN
	     STATUS = SMB$READ_MESSAGE( %REF (ID),
     +                                  %DESCR(MESSAGE),
     +                                  %REF (REQUEST))

	     IF (DEBUG) WRITE (DBG_LU,*) 'READ_MESSAGE: ',REQUEST,STATUS
	     IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

 	ID = ID + 1 ! our ID's must start with 1, for FORTRAN arrays
             IF (REQUEST.EQ.SMBMSG$K_START_STREAM) THEN
                                    CALL START_STREAM(ID)
 	ELSE IF ((REQUEST.EQ.SMBMSG$K_STOP_STREAM) .OR.
     +           (REQUEST.EQ.SMBMSG$K_RESET_STREAM)) THEN 
                                    CALL STOP_STREAM(ID)
 	ELSE IF ((REQUEST.EQ.SMBMSG$K_START_TASK)) THEN
                                    CALL START_TASK(ID)
 	ELSE IF ((REQUEST.EQ.SMBMSG$K_STOP_TASK)) THEN
 				    CALL ABORT_TASK(ID)
             ENDIF
 	IF (TASKS.EQ.0)   CALL SYS$WAKE
        IF (ACTIVE.EQ.0)  CALL SYS$EXIT(%VAL(SS$_NORMAL))
 	ENDIF
        RETURN
        END


 	SUBROUTINE START_STREAM(ID)
C+
C 
C FUNCTIONAL DESCRIPTION:	*** AST CONTEXT ***
C 
C    This procedure processes the START_STREAM request from the JOB CONTROLLER,
C    effectivly starting the queue.  The /ON= parameter is read and stored
C    in the STREAM(ID) common area.
C
C    note: one is added to the JOB CONTROLLER stream ID so we can use it
C          as an array reference.
C 
C-
	IMPLICIT NONE
	INCLUDE 'COMMON'
	INCLUDE 'SMBDEF'
	
 	PARAMETER SS$_NORMAL = 1

	CHARACTER*100 DATA_STR
	INTEGER*4     DATA(25)
	INTEGER*2     DATA_LEN
	EQUIVALENCE   (DATA_STR , DATA(1))

	INTEGER CONTEXT,STATUS,ITEM
 	INTEGER 	ID		 ! STREAM ID

	IF (DEBUG) WRITE(DBG_LU,*) '        START-STREAM ACTIVE'

	STREAM(ID).DEV_STS = SMBMSG$M_LOWERCASE + SMBMSG$M_SERVER
 	STREAM(ID).STATE   = STRM_AVAIL
 	STREAM(ID).ABORT   = .FALSE.
        STREAM(ID).PAUSED  = .FALSE.

C 
C search message items for DEV_NAM + DEV_LEN
C
         CONTEXT = 0 
 	 STREAM(ID).ID = ID - 1 ! SMB ID IS ONE LESS THAN FORTRAN'S
100      STATUS = SMB$READ_MESSAGE_ITEM(%DESCR(MESSAGE),
     +                                  %REF(CONTEXT  ),
     +                                  %REF(ITEM     ),
     +                                %DESCR(DATA_STR ),
     +                                  %REF(DATA_LEN ))
	 IF (DEBUG) WRITE(DBG_LU,*) 'READ_ITEM:',ITEM,STATUS
         IF (STATUS.EQ.SMB$_NOMOREITEMS) STATUS = SS$_NORMAL
	 IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

 	 IF (ITEM.EQ.SMBMSG$K_DEVICE_NAME) THEN
 			      STREAM(ID).DEV_NAM = DATA_STR(1:DATA_LEN)
 			      STREAM(ID).DEV_LEN = DATA_LEN
 			      ENDIF
	 IF (CONTEXT.NE.0) GOTO 100 

C TELL THE JOB CONTROLLER WE'RE READY

	 STATUS = SMB$SEND_TO_JOBCTL( %REF(STREAM(ID).ID),
     +				      %REF(SMBMSG$K_START_STREAM),
     +				      %VAL(0), ! ACCOUNTING
     +				      %VAL(0), ! CHECKPOINT
     +				      %REF(STREAM(ID).DEV_STS),
     +				      %VAL(0))
	 IF (DEBUG) WRITE(DBG_LU,*) 'SEND_START_STREAM',ID,STATUS
	 IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

 	 ACTIVE = ACTIVE + 1

	 RETURN
	 END

 	SUBROUTINE STOP_STREAM(ID)
C+
C 
C FUNCTIONAL DESCRIPTION:	*** AST CONTEXT ***
C 
C    This routine processes the STOP_STREAM requeues from the JOB CONTROLLER
C    effectivly stopping the queue.
C 
C 
C-

	IMPLICIT NONE
	INCLUDE 'COMMON'
	INCLUDE 'SMBDEF'

	INTEGER STATUS,ID

	IF (DEBUG) WRITE(DBG_LU,*) '                STOP-STREAM ACTIVE'

C
C if STATE=STRM_RUN or STRM_ABORT then we must stop the current task
C before we stop the stream
C
 	IF ((STREAM(ID).STATE.EQ.STRM_RUN) .OR.
     +      (STREAM(ID).STATE.EQ.STRM_ABORT)) CALL ABORT_TASK(ID)

	 STATUS = SMB$SEND_TO_JOBCTL( %REF(STREAM(ID).ID),
     +				      %REF(SMBMSG$K_STOP_STREAM),
     +				      %VAL(0), ! ACCOUNTING
     +				      %VAL(0), ! CHECKPOINT
     +				      %VAL(0), ! DEVICE_STATUS
     +				      %VAL(0)) ! ERROR
	 IF (DEBUG) WRITE(DBG_LU,*) 'SEND_STOP_STREAM:',ID,STATUS
	 IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

         STREAM(ID).STATE = STRM_DOWN
         ACTIVE = ACTIVE - 1

 	 RETURN
	 END

 	SUBROUTINE ABORT_TASK(ID)
C+
C 
C FUNCTIONAL DESCRIPTION:	*** AST CONTEXT ***
C 
C    This procedure processes the ABORT_TASK request from the JOB CONTROLLER
C    It closes any files that may be open, and resets the queue state to
C    available.
C      
C 
C-
 	IMPLICIT NONE
        INCLUDE 'COMMON'
 	INCLUDE 'SMBDEF'
 	INCLUDE '($SJCDEF)'
 	
 	INTEGER ID,STATUS

C
C ABORT IS TRUE!!
C
602 	 CLOSE(INP_LU,DISPOSE='DELETE',ERR=603)

C NOTIFY JOB CONTROLLER WE'RE DONE!
603 	     STREAM(ID).STATE = STRM_AVAIL
             TASKS = TASKS - 1
 	     STATUS = SMB$SEND_TO_JOBCTL( %REF(STREAM(ID).ID),
     +	     			           %REF(SMBMSG$K_TASK_COMPLETE),
     +				           %DESCR(STREAM(ID).ACCOUNTNG),
     +				           %VAL(0), ! CHECKPOINT
     +				           %REF(STREAM(ID).DEV_STS),
     +				           %REF(STREAM(ID).ERROR)) 
	      IF (DEBUG) WRITE(DBG_LU,*) 'SEND COMPLETE:',ID,STATUS
  	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
             
900    RETURN
       END
 	
	SUBROUTINE START_TASK(ID)
C+
C 
C FUNCTIONAL DESCRIPTION:	*** AST CONTEXT ***
C 
C    This procedure processes the START_TASK request from the JOB CONTROLLER
C    It reads the parameters assigned to the job and sets up the common
C    area appropriatly.
C
C    A GETQUI call is also made to get information on the form the user
C    has specified.  IBM setup information is expected to be stored here.
C     
C    
C-
 	IMPLICIT NONE
	INCLUDE '($QUIDEF)'
        INCLUDE 'COMMON'
 	INCLUDE 'SMBDEF'

 	PARAMETER SS$_NORMAL = 1

 	CHARACTER*100 DATA_STR
	INTEGER*4     DATA(25)
	INTEGER*2     DATA_LEN
	EQUIVALENCE   (DATA_STR , DATA(1))

 	STRUCTURE /ITMLST_TYPE/
 		INTEGER*2 BUFLEN,ITMCOD
 		INTEGER*4 BUFADR,LENADR
 		END STRUCTURE
 	RECORD	/ITMLST_TYPE/    ITMLST(15)
	INTEGER STATUS,CONTEXT,ITEM,ID,I,SYS$GETQUIW

	IF (DEBUG) WRITE(DBG_LU,*) '              START_TASK STARTED'

        TASKS                  = TASKS+1
        STREAM(ID).STATE       = STRM_START

C APPLY DEFAULTS
        STREAM(ID).UIC         = '00800080'X  ! DEFAULT UIC, (SHOULDN'T HAPPEN)
	STREAM(ID).FILE	       = ' '
	STREAM(ID).USER	       = ' '	
	STREAM(ID).JOB	       = ' '
        STREAM(ID).FILE_LEN    = 1
        STREAM(ID).USER_LEN    = 1
        STREAM(ID).JOB_LEN     = 1

C
C READ MESSAGE ITEMS
C
         CONTEXT = 0 
100      STATUS = SMB$READ_MESSAGE_ITEM(%DESCR(MESSAGE  ),
     +                                  %REF(CONTEXT    ),
     +                                  %REF(ITEM       ),
     +                                  %DESCR(DATA_STR ),
     +                                  %REF(DATA_LEN   ))
	 IF (DEBUG) WRITE(DBG_LU,*) 'READ_ITEM:',ITEM,STATUS
         IF (STATUS.EQ.SMB$_NOMOREITEMS) STATUS = SS$_NORMAL
	 IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))


      IF (ITEM.EQ.SMBMSG$K_ENTRY_NUMBER) THEN
					STREAM(ID).ENTRY    = DATA(1)

      ELSE IF (ITEM.EQ.SMBMSG$K_FILE_IDENTIFICATION ) THEN
                                STREAM(ID).FID_INFO = DATA_STR(1:28)

      ELSE IF (ITEM.EQ.SMBMSG$K_FILE_SPECIFICATION ) THEN
                                STREAM(ID).FILE_LEN = DATA_LEN
				STREAM(ID).FILE = DATA_STR(1:DATA_LEN)

      ELSE IF (ITEM.EQ.SMBMSG$K_JOB_NAME  ) THEN
				STREAM(ID).JOB_LEN  = DATA_LEN
				STREAM(ID).JOB = DATA_STR(1:DATA_LEN)

      ELSE IF (ITEM.EQ.SMBMSG$K_USER_NAME)   THEN
				STREAM(ID).USER_LEN = DATA_LEN
				STREAM(ID).USER = DATA_STR(1:DATA_LEN)
     	   IF (DEBUG) WRITE(DBG_LU,*) 'USERNAME:',DATA_STR(1:DATA_LEN)
      ENDIF

      IF (CONTEXT.NE.0) GOTO 100 

C
C WE DON'T GET A CORRECT LENGTH FOR THE USERNAME, LET'S FIND OUR OWN
C
      STREAM(ID).USER_LEN = INDEX(STREAM(ID).USER,' ')-1

      STREAM(ID).ERROR(1) = 0
      STREAM(ID).ERROR(2) = 0
      STREAM(ID).ERROR(3) = 0
      STREAM(ID).ERROR(4) = 0
      STREAM(ID).PAUSED   = .FALSE.
      STREAM(ID).ABORT    = .FALSE.
      STREAM(ID).ACCOUNTNG(1) = 0
      STREAM(ID).ACCOUNTNG(2) = 0
      STREAM(ID).ACCOUNTNG(3) = 0
      STREAM(ID).ACCOUNTNG(4) = 0

      RETURN
      END

	SUBROUTINE RUN_MAILOUT(ID)
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    This procedure does the main work for MAILOUT queues.
C    Mail is sent via the MAIL$ calls to the username of the job.
C    creating the INPSMB process and passing stuff to it.
C    
C-
      IMPLICIT NONE
      INCLUDE 'COMMON'
      INCLUDE 'SMBDEF'
      INCLUDE '($SJCDEF)'
      INCLUDE '($SSDEF)'
      INCLUDE '($MAILDEF)'


      CHARACTER*80  SUBJECT

      INTEGER STATUS,CONTEXT,ID,SUB_LEN
      INTEGER SYS$SNDJBCW      
      INTEGER mail$send_begin  , mail$send_add_attribute
      INTEGER mail$send_end    , mail$send_add_bodypart
      INTEGER mail$send_message, mail$send_add_address

      STRUCTURE /itm_list/
              integer*2 buflen,item
              integer*4 buffer,retlen
          END STRUCTURE    !itm_list

      RECORD /itm_list/ mail_item(5),null_list


      null_list.item = 0
      null_list.buflen = 0
      
      IF (DEBUG) WRITE(DBG_LU,*) '              RUN_MAILOUT STARTED'

C
C Tell JOB controller we're starting
C
	      STATUS = SMB$SEND_TO_JOBCTL( %REF(STREAM(ID).ID),
     +	     			           %REF(SMBMSG$K_START_TASK),
     +				           %VAL(0), ! ACCOUNTING
     +				           %VAL(0), ! CHECKPOINT
     +				           %REF(STREAM(ID).DEV_STS),
     +				           %REF(STREAM(ID).ERROR)) 
	      IF (DEBUG) WRITE(DBG_LU,*) 'SEND START TASK:',ID,STATUS
    	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

C
C begin conversation with MAIL
C
      context = 0 
      status = mail$send_begin(context,null_list,null_list)
	    IF (DEBUG) WRITE(DBG_LU,*) 'MAIL$SEND_BEGIN:',ID,STATUS
            if (.not.status) call lib$signal(%val(status))

C
C build our SUBJECT and send attributes to MAIL
C
      subject = 'output of VAX JOB: '//stream(id).job
      sub_len = min( (20+stream(id).job_len), len(subject))

      mail_item(1).item    = mail$_send_from_line
      mail_item(1).buffer  = %loc(stream(id).user)
      mail_item(1).buflen  =      stream(id).user_len

      mail_item(2).item    = mail$_send_subject
      mail_item(2).buffer  = %loc(subject)
      mail_item(2).buflen  =      sub_len
      
      mail_item(3).item    = 0
      mail_item(3).buflen  = 0

      status = mail$send_add_attribute(context,mail_item,null_list)
	IF (DEBUG) WRITE(DBG_LU,*) 'MAIL$SEND_ADD_ATTRIBUTE:',ID,STATUS
        if (.not.status) call lib$signal(%val(status))

C
C build distribution list 
C
      mail_item(1).item   = mail$_send_username
      mail_item(1).buffer  = %loc(stream(id).user)
      mail_item(1).buflen  =      stream(id).user_len
      
      mail_item(2).item   = 0
      mail_item(2).buflen = 0

      status = mail$send_add_address(context,mail_item,null_list)
	 IF (DEBUG) WRITE(DBG_LU,*) 'MAIL$SEND_ADD_ADDRESS:',ID,STATUS
         if (.not.status) call lib$signal(%val(status))
       
C
C give mail the FILEID to send
C

      mail_item(1).item   = mail$_send_FID
      mail_item(1).buflen =  6
      mail_item(1).buffer = %loc(stream(id).NAM_FID)

      mail_item(2).item   = MAIL$_SEND_FID
      mail_item(2).buflen =  6
      mail_item(2).buffer = %loc(stream(id).NAM_FID)

      mail_item(3).item   = MAIL$_SEND_DEFAULT_NAME
      mail_item(3).buflen =  LEN(stream(id).NAM_DVI)
      mail_item(3).buffer = %loc(stream(id).NAM_DVI)

      mail_item(4).item   = 0
      mail_item(4).buflen = 0

      status = mail$send_add_bodypart(context,mail_item,null_list)
	    IF (DEBUG) WRITE(DBG_LU,*) 'MAIL$SEND_ADD_BODYPART:',ID,STATUS
            if (.not.status) call lib$signal(%val(status))

C
C send message and cleanup
C
      status = mail$send_message(context,null_list,null_list)
	    IF (DEBUG) WRITE(DBG_LU,*) 'MAIL$SEND_MESSAGE:',ID,STATUS
            if (.not.status) call lib$signal(%val(status))

      status = mail$send_end(context,null_list,null_list)
	    IF (DEBUG) WRITE(DBG_LU,*) 'MAIL$SEND_END:',ID,STATUS
            if (.not.status) call lib$signal(%val(status))

C NOTIFY JOB CONTROLLER WE'RE DONE!
500	     STREAM(ID).STATE = STRM_AVAIL
 	     STATUS = SMB$SEND_TO_JOBCTL(  %REF(STREAM(ID).ID),
     +	     			           %REF(SMBMSG$K_TASK_COMPLETE),
     +				           %DESCR(STREAM(ID).ACCOUNTNG),
     +				           %VAL(0), ! CHECKPOINT
     +				           %REF(STREAM(ID).DEV_STS),
     +				           %REF(STREAM(ID).ERROR)) 
	      IF (DEBUG) WRITE(DBG_LU,*) 'SEND COMPLETE:',ID,STATUS
  	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

       RETURN

C
C ABORT REQUESTED.... LET'S GET OUTA HERE
C
600    STREAM(ID).STATE = STRM_ABORT

       RETURN

       END

	SUBROUTINE RUN_CARDREADER(ID)
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    This procedure does the main work of the CARDIN queue. 
C    The input file is opened by FILE_ID, and passed line by line
C    to the mailbox that hopefully has the INPSMB process on the other
C    end.
C    
C    Errors are written to the file SYS$COMMON:[SYSEXE]INPSMB.ERR
C 
C-
 	IMPLICIT NONE
        INCLUDE 'COMMON'
 	INCLUDE 'SMBDEF'
 	INCLUDE '($SJCDEF)'
 	INCLUDE '($SSDEF)'


 	EXTERNAL FIDOPEN

        CHARACTER*80 STRING
	INTEGER STATUS,SYS$SNDJBCW,ID,I,J

	IF (DEBUG) WRITE(DBG_LU,*) '              RUN_CARDREADER STARTED'

      OPEN(INP_LU,FILE=STREAM(ID).FILE(1:STREAM(ID).FILE_LEN),
     +           STATUS='OLD',READONLY,USEROPEN=FIDOPEN)

C FILES OPENED, NOTIFY JOB CONTROLLER
	      STATUS = SMB$SEND_TO_JOBCTL( %REF(STREAM(ID).ID),
     +	     			           %REF(SMBMSG$K_START_TASK),
     +				           %VAL(0), ! ACCOUNTING
     +				           %VAL(0), ! CHECKPOINT
     +				           %REF(STREAM(ID).DEV_STS),
     +				           %REF(STREAM(ID).ERROR)) 
	      IF (DEBUG) WRITE(DBG_LU,*) 'SEND START TASK:',ID,STATUS
    	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

C
C MAIN COPY LOOP - copy text of report to .JCL file
C
151	FORMAT(Q,A)
152	FORMAT(A)
 	IF (DEBUG) WRITE(DBG_LU,*) 'COPY STARTED'
150     IF (STREAM(ID).ABORT) GOTO 600
            STREAM(ID).ACCOUNTNG(2) = STREAM(ID).ACCOUNTNG(2) + 1
	    READ(INP_LU,FMT=151,END=200 ) I,STRING
 	    I = MAX(1,MIN(I,80)) ! cardreader only handles 80 character lines

C
C the following section is necessary to handle junk characters that show
C up when the print originates from the SNAGWY SNAPRE service.
C 
            J = INDEX (STRING,' '//CR)            ! TRIM SP CR
            DO WHILE (J.GT.0)
                STRING(j:) = string(j+2:)
                J = INDEX (STRING,' '//CR)
                END DO

            J = INDEX (STRING,CR//LF) 	           ! TRIM CR LF
            DO WHILE (J.GT.0)
                STRING(j:) = string(j+2:)
                J = INDEX (STRING,CR//LF)
                END DO

            J = INDEX (STRING,LF) 	           ! TRIM LF
            DO WHILE (J.GT.0)
                STRING(j:) = string(j+1:)
                J = INDEX (STRING,CR//LF)
                END DO

      	    WRITE(MBX_LU,FMT=152) STRING(1:I)
            GOTO 150
C
C close files, they're keepers
C
200   WRITE(MBX_LU,*) '$EOJ'
      CLOSE(INP_LU)


C NOTIFY JOB CONTROLLER WE'RE DONE!
500	     STREAM(ID).STATE = STRM_AVAIL
 	     STATUS = SMB$SEND_TO_JOBCTL(  %REF(STREAM(ID).ID),
     +	     			           %REF(SMBMSG$K_TASK_COMPLETE),
     +				           %DESCR(STREAM(ID).ACCOUNTNG),
     +				           %VAL(0), ! CHECKPOINT
     +				           %REF(STREAM(ID).DEV_STS),
     +				           %REF(STREAM(ID).ERROR)) 
	      IF (DEBUG) WRITE(DBG_LU,*) 'SEND COMPLETE:',ID,STATUS
  	      IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

       RETURN

C
C ABORT REQUESTED.... LET'S GET OUTA HERE
C
600    STREAM(ID).STATE = STRM_ABORT

       RETURN


       END

 	INTEGER*4 FUNCTION HANDLE_ERROR(SIGARGS, MECHARGS)
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    This routine handles normal errors.  Should an FORTRAN error
C    happen during a job, this routine would trap it and notify
C    the JOB CONTROLLER that the stream died.
C
C    The stream is not stopped and other streams are unaffected. (I hope)
C 
C 
C-
 	IMPLICIT NONE
 	INCLUDE 'COMMON'
 	INCLUDE '($SSDEF)'
	INCLUDE 'SMBDEF'

 	INTEGER*4 SIGARGS(*), MECHARGS(5),STATUS
 	INTEGER*4 SYS$UNWIND,SYS$FAOL,SYS$GETMSG
        INTEGER*2     MSGLEN
        CHARACTER*255 MSG
 	EXTERNAL MAJOR_ERROR


 	IF (SIGARGS(2).EQ.SS$_UNWIND) GOTO 700 !UNWIND IN PROGRESS!

 	CALL LIB$ESTABLISH(MAJOR_ERROR)

         IF (DEBUG) THEN
 	     WRITE(DBG_LU,*,ERR=600) 'ERROR DETECTED - ',SIGARGS(2)
             status = sys$getmsg (%val(sigargs(2)),msglen,msg,,)
   	     WRITE(DBG_LU,*,ERR=600) 'SYS$GETMSG - ',STATUS

             status = sys$faol (
     1                    msg(1:msglen), 
     1                    msglen, 
     1                    msg, 
     1                    sigargs(4))
   	     WRITE(DBG_LU,*,ERR=600) 'SYS$FAO- ',STATUS
   	     WRITE(DBG_LU,*,ERR=600) 'TEXT     ',MSG(1:MSGLEN)
             ENDIF

 	IF (SIGARGS(2).NE.SS$_NORMAL) THEN 
 	     STREAM(CUR_ID).ERROR(4) = STREAM(CUR_ID).ERROR(3)
 	     STREAM(CUR_ID).ERROR(3) = STREAM(CUR_ID).ERROR(2)
 	     STREAM(CUR_ID).ERROR(2) = SIGARGS(2)
             IF (STREAM(CUR_ID).ERROR(1).LT.3) 
     +                 STREAM(CUR_ID).ERROR(1)=STREAM(CUR_ID).ERROR(1)+1


C
C NOTE: WE MUST CLEAN EVERYTHING UP BEFORE WE TELL THE JOB CONTROLLER
C       
 	IF (STREAM(CUR_ID).STATE.GT.STRM_AVAIL) THEN 
 	     WRITE(MBX_LU,fmt=*,err=102) '$EOJ'
102	     CLOSE(INP_LU,ERR=103)
         
103          STREAM(CUR_ID).STATE = STRM_AVAIL
	     STATUS = SMB$SEND_TO_JOBCTL( %REF(STREAM(CUR_ID).ID),
     +	     			      %REF(SMBMSG$K_TASK_COMPLETE),
     +				      %DESCR(STREAM(CUR_ID).ACCOUNTNG),
     +				      %VAL(0),         ! CHECKPOINT
     +				      %REF(STREAM(CUR_ID).DEV_STS),
     +				      %REF(STREAM(CUR_ID).ERROR)) 
	     IF (DEBUG) WRITE(DBG_LU,*) 'SEND COMPLETE:',CUR_ID,STATUS
 	     IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))
             END IF
        END IF

C
C UNWIND TO OUR MAIN ROUTINE (WHERE THE ERROR HANDLER WAS STARTED)
C
600 	STATUS = SYS$UNWIND(%REF(MECHARGS(3)) ,%VAL(0))
 	     IF (.NOT.STATUS) CALL LIB$SIGNAL(%VAL(STATUS))

700     CONTINUE
 	RETURN

 	END

 	INTEGER*4 FUNCTION MAJOR_ERROR(SIGARGS,MECHARGS)
 	IMPLICIT NONE
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    We got trouble if this baby executes.  This is an error handler
C    for our error handler.  It aborts the server process with the
C    best known error code.  The JOB CONTOLLER then notifies OPCOM
C    of an "UNEXPECTED SYMBIONT TERMINATION" and printes this error code.
C    
C 
C 
C-

 	INTEGER*4 SIGARGS(*), MECHARGS(5)

 	CALL SYS$EXIT(%VAL(SIGARGS(2)))

 	RETURN
 	END


 	INTEGER FUNCTION FIDOPEN(FAB,RAB,LUN)
C+
C 
C FUNCTIONAL DESCRIPTION:                         *** USER OPEN CONTEXT ***
C 
C
C This function opens a file by FID.  It is necessary because of a bug
C in the JOB CONTROLLER where the text filename given can be WRONG!!
C (Try a print/header of something in SYS$COMMON and check it out)
C
C A short subroutine is used to modify the NAM block because FORTRAN
C doesn't directly provide indirect addressing.
C 
C FUNCTION VALUE:
C 
C    A VMS status code returned by $CREATE system service
C 
C-
 	IMPLICIT NONE
 	INCLUDE '($FABDEF)'
        INCLUDE 'COMMON'

 	RECORD /FABDEF/ FAB

 	INTEGER SYS$CONNECT,SYS$OPEN
 	INTEGER RAB,LUN  ! WE DON'T LOOK AT THESE ANYWAY

        CALL SETFID(%VAL(FAB.FAB$L_NAM))

        FAB.FAB$L_FOP = FAB.FAB$L_FOP .OR. FAB$M_NAM

 	FIDOPEN =    SYS$OPEN(FAB) 	     ! OPEN AND CONNECT TO FILE
 	IF (DEBUG) WRITE(DBG_LU,*) 'USEROPEN FIDOPEN -',FIDOPEN
 	IF (FIDOPEN) FIDOPEN = SYS$CONNECT(RAB)
 	IF (DEBUG) WRITE(DBG_LU,*) '         CONNECT-',FIDOPEN

	IF (.NOT.FIDOPEN) CALL LIB$SIGNAL(%VAL(FIDOPEN))

  	RETURN
 	END

        
        SUBROUTINE SETFID(NAM)
C
c This routine simply inserts the current fid into the RMS NAM block
C         
        IMPLICIT NONE
        INCLUDE '($NAMDEF)'
        INCLUDE 'COMMON'
         
        RECORD /NAMDEF/ NAM

        NAM.NAM$T_DVI    = STREAM(CUR_ID).NAM_DVI
        NAM.NAM$W_FID(1) = STREAM(CUR_ID).NAM_FID(1)
        NAM.NAM$W_FID(2) = STREAM(CUR_ID).NAM_FID(2)
        NAM.NAM$W_FID(3) = STREAM(CUR_ID).NAM_FID(3)

        NAM.NAM$W_DID(1) = STREAM(CUR_ID).NAM_DID(1)
        NAM.NAM$W_DID(2) = STREAM(CUR_ID).NAM_DID(2)
        NAM.NAM$W_DID(3) = STREAM(CUR_ID).NAM_DID(3)

        RETURN

        END

      SUBROUTINE inpsmb_err_queue
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    This routine queues a read to the INPSMB process's error mailbox
C    An AST is then triggered to notify the operator of the problem.
C 
C-
      IMPLICIT NONE
      include '($IODEF)'
      include 'common'
 
      integer status,sys$qio
      external inpsmb_err_ast

      status = sys$qio (
     1             %val(0), 
     1             %val(inpsmb_err_chan), 
     1             %val(io$_readvblk), 
     1             %ref(inpsmb_iosb), 
     1             %ref(inpsmb_err_ast), 
     1             %val(0), 
     1             %ref(inpsmb_msg.text), 
     1             %val(len(inpsmb_msg.text)),,,,)
              if (.not.status) call lib$signal(%val(status))

      RETURN
      END

      subroutine inpsmb_err_ast
C+
C 
C FUNCTIONAL DESCRIPTION:    *** AST CONTEXT ***
C 
C    This AST routine is caused by IO completion to the INPSMB error
C    mailbox.  It forwards the contents of the mailbox to OPCOM.
C 
C 
      IMPLICIT NONE
      INCLUDE '($OPCDEF)'
      INCLUDE 'common'
      integer status,sys$sndopr

      inpsmb_msg.type =  OPC$_RQ_RQST+(OPC$M_NM_CLUSTER*256)
      status = sys$sndopr(inpsmb_msg.string(1:inpsmb_iosb.length+20),)
          if (.not.status) call lib$signal(%val(status))

      call inpsmb_err_queue
      END
