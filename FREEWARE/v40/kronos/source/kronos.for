      Program Kronos
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          Kronos           **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (650) 604-5558
C*
C*     DESCRIPTION :
C*          Kronos is the job scheduling and system monitor program.
C*          See KRONOS.DOC for details.
C*
C*     COMMON BLOCKS :
C*          kronos.cmn
C*          k_users
C*
C*     SUBPROGRAM REFERENCES :
C*          lib$date_time,    lib$day_of_week,     new_file,  
C*          parse_file,       do_jobs,             snooze,  
C*          error,            sys$purgws,          check_q,  
C*          m2i
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Kronos is not even remotely transportable.
C*
C*     LANGUAGE AND COMPILER :
C*          DEC FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          15-AUG-1989  -  Initial Release version
C*          19-APR-1990  -  New version using command language
C*          15-JUL-1991  -  Added improved error handling
C*          27-DEC-1991  -  Added cluster failover feature
C*          15-APR-1994  -  Drift in SNOOZE corrected
C*          23-NOV-1994  -  Added PURGE flag and fixed time in ERROR.LOG
C*          13-APR-1995  -  Added priority push for our jobs
C*          31-DEC-1996  -  Parameterized num of queues/users
C*          07-OCT-1998  -  Fixed bugs in the parsing routines
C*
C***********************************************************************
C*
%ifdef cluster
      include '($LCKDEF)'
      structure / status_block / 
         integer *2 lock_status, null
         integer *4 lock_id
      end structure
      record / status_block / iostatus
      common / locks / iostatus
      integer stat, sys$enqw
%endif
c
      include 'kronos.cmn'
c
      parameter (max_users=10)
      common /k_users/ p_users(max_users), n_prime, s_users(max_users),
     $  n_second
      character *12 p_users, s_users
c
      parameter (max_queues=20)
      character *31 q_names(max_queues)
      logical new_file, check_q
      integer idate(2)
%ifdef purge
      integer mem(2), sys$purgws
%endif
%ifdef cluster
c
c --- Cluster failover ...
c      Try to grab a cluster-wide lock.  If I get it then I'm on stage;
c      otherwise I just hang until whoever has the lock dies or exits.
c
      stat = sys$enqw (, %val(lck$k_exmode),iostatus,,
     $   'KRONOS_LOCK',,,,,,)
      if (.not. stat)
     $   call error (4, 'Error in obtaining cluster-wide lock.', stat)
      if (.not. iostatus.lock_status)
     $   call error (4, 'Error in obtaining cluster-wide lock.',
     $               iostatus.lock_status)
%endif
c
c --- default directory for submitted jobs
c
      def_dir = 'KRONOS_DIR:'
c
c --- initialize date and time
c --- this stuff will be done in SNOOZE from now on
c
      CALL LIB$DATE_TIME ( NOW )
c
c --- Error handling stuff
c
      OPEN (UNIT=99, FILE='KRONOS_DIR:ERROR.LOG', STATUS='NEW',
     $ CARRIAGECONTROL='LIST', ERR=10)
      write(99,920) now
      close(99)
      go to 20
10    call error (3, 'Unable to create Kronos log file!',0)
c
c --- Correct "NOW" to be equally divisible by %INTERVAL or 1 hour 
c     (whichever applies)
c
%ifdef interval
20    READ(NOW(16:17),900) MIN
      I = MOD(MIN,%INTERVAL)
      MIN = MAX((MIN - I),0)
      WRITE(NOW(16:17),900) MIN
      NOW(19:20) = '00'
%else
20    NOW(16:20) = '00:00'
%endif
      READ(NOW(13:14),900) HOUR
      READ(NOW(1:2),900) DAY
      MONTH = M2I (NOW(4:6))
      CALL LIB$DAY_OF_WEEK ( ,IDAY )
c
c --- get primary and secondary users' names for ERROR routine
c
      OPEN (UNIT=1, FILE='KRONOS_DIR:K_USERS.DAT', STATUS='OLD',
     $  readonly, ERR=50)
      n_prime = 0
      n_second = 0
30    read(1,910,end=45) p_users( n_prime+1 )
      if (p_users(n_prime+1) .ne. ' ') then
         n_prime = n_prime + 1
         if (n_prime .lt. max_users) go to 30
      endif
40    read(1,910,end=45) s_users( n_second+1 )
      n_second = n_second + 1
      if (n_second .le. max_users) go to 40
c
45    close(1)
      go to 60
c
50    n_prime = 1
      p_users(1) = 'SYSTEM'
      n_second = 0
%ifdef purge
      mem(1) = 0
      mem(2) = '7FFFFFFF'X
%endif
c
c --- Get the names of the batch queues to check
c
60    n_queues = 0
      OPEN (UNIT=1, FILE='KRONOS_DIR:QUEUES.DAT', STATUS='OLD',
     $ readonly, ERR=70)
      go to 80
70    call error (3, 'Unable to initialize batch queue names.',0)
      go to 100
80    read(1,910,end=90) q_names(n_queues+1)
      if (q_names(n_queues + 1) .ne. ' ') n_queues = n_queues + 1
      call caps (q_names(n_queues))
      if (n_queues .lt. max_queues) go to 80
90    close(1)
c
c
c
c
c
c --- ******************* Main Loop *********************
c --- loop once each sleep period, forever
c
c --- sleep for awhile, then update date/time info
c
100   call snooze
c
c --- if the data base file has been updated since last time through
c --- the loop, read the new file and "compile" it.
c
      if (new_file(idate)) then
         call parse_file ('KRONOS_DIR:KRONOS.DAT')
%ifdef purge
         call sys$purgws (mem)
%endif
      endif
c
c --- it doesn't make any sense to submit a job to see if the batch 
c     queues are running; so do it here!
c
      do 110 i = 1, n_queues
         if (check_q (q_names(i))) then
            call error (2, 'One or more batch queues are stopped.', 0)
            go to 120
         endif
110   continue
c
c --- see if there are any jobs to execute this time.  If so, do it!
c
120   call do_jobs
      go to 100
c
c
c --- ******************** End Main Loop ********************
c
c
c
900   format(i2)
910   format(a)
920   format('Kronos started at ',a)
      end
c
c---end Kronos
c
      logical function new_file (idate)
c*
c*  Has a new version of the master file been created?  Note we use the 
c*   revision date; it will match the creation date unless an update has
c*   been made by Kron.  WARNING !!!  This function only checks the file
c*   date for the master file; if an include file is updated ("@"
c*   command) the master file should be redated (with TOUCH) to force
c*   Kronos to read the new file.
c*
c*   Subprogram references:
c*       FILE_BDATES - MERLIB routine to retrieve file dates
c*       OPER        - MERLIB routine to send message to OPER console
c*                      (this is a security feature)
c*       ERROR
c*
c*   Input:
c*       IDATE - the date of the file as previously read (I*8)
c*
c*    Updates:
c*       15 oct 91 - fixed incorrect file date at startup
c*
      include 'kronos.cmn'
      character *4 save_year
      save save_year
      data save_year /'0000'/
      integer idate(2), cdate(2), rdate(2), edate(2), bdate(2)
c
c --- On Jan 1st, force a parse_file to load new calendar file.
c ---  Note, the following also forces a parse_file on Kronos startup.
c
      call file_bdates ('KRONOS_DIR:KRONOS.DAT',
     $     cdate, rdate, edate, bdate)
      if (now(8:11) .ne. save_year) then
         new_file = .true.
         save_year = now(8:11)
c
c --- has the master file been modified since last wakeup?
c
      else
c
c --- the date is a quad word.  Compare it to last file date.
c
         new_file = (rdate(1) .ne. idate(1)) .or.
     $              (rdate(2) .ne. idate(2))
      endif
c
      if (new_file) then
         idate(1) = rdate(1)
         idate(2) = rdate(2)
         CALL OPER( 'Loading new KRONOS data base.', 'CENTRAL' )
         call error (0, 'Reloading data base.', 0)
      endif
      return
      end
c
c---end new_file
c
      SUBROUTINE SNOOZE
C*
C*   UPDATE THE WAKE UP TIME FOR THE START OF THE NEXT LOOP.
C*   GO TO SLEEP UNTIL WAKEUP IS SOUNDED.
C*   INITIALIZE TIME AND DAY INFO.
C*
      INCLUDE 'KRONOS.CMN'
      INTEGER IDATE(2), ITIME(2)
      CHARACTER *23 DELTA / '1 00:00:00.00' /
      integer sys$bintim
C
C --- SCHEDULE NEXT WAKE UP FOR SPECIFIED TIME HENCE.
C ---  IF THIS RESULTS IN PASSING MIDNIGHT, ADD ONE TO DAY.
C
C
%ifdef interval
c
c --- use the following code for variable wakeup interval...
c --- must also update AT code.
c
      NOW(19:23) = '00.10'
      READ (NOW(13:14),900) HOUR
      READ (NOW(16:17),900) IMIN
c
c --- set next wakeup interval...
c ---  on rare occasions, it may take a minute to get here, thus the MOD
c ---  statement is necessary to prevent drift
c
      IMIN = IMIN + %INTERVAL
      i = mod(imin,%interval)
      imin = max((imin-i),0)
      
      IF (IMIN .GE. 60) THEN
         IMIN = IMIN - 60
         HOUR = HOUR + 1
         IF (HOUR .GE. 24) THEN
            HOUR = 0
            ISTAT = SYS$BINTIM (NOW, ITIME)
            ISTAT = SYS$BINTIM (DELTA, IDATE)
            ISTAT = LIB$ADD_TIMES (ITIME, IDATE, ITIME)
            ISTAT = LIB$FORMAT_DATE_TIME (NOW, ITIME)
         ENDIF
      ENDIF
      WRITE (NOW(13:14),900) HOUR
      WRITE (NOW(16:17),900) IMIN
      ISTAT = SYS$BINTIM (NOW, ITIME)
c
%else
c
c --- if no interval is specified, assume 1 hour
c
      NOW(16:23) = '00:00.10'
      HOUR = HOUR + 1
      IF (HOUR .GE. 24) THEN
         HOUR = 0
         ISTAT = SYS$BINTIM (NOW, ITIME)
         ISTAT = SYS$BINTIM (DELTA, IDATE)
         ISTAT = LIB$ADD_TIMES (ITIME, IDATE, ITIME)
         ISTAT = LIB$FORMAT_DATE_TIME (NOW, ITIME)
      ENDIF
      WRITE (NOW(13:14),900) HOUR
      ISTAT = SYS$BINTIM (NOW, ITIME)
c
%endif
C
C --- NAP TIME
C
%ifdef nosnooze
      CALL SLEEP (2.0)
%else
      CALL SYS$SCHDWK (,,ITIME,)
      CALL SYS$HIBER
%endif
C
C --- YAWN, STRETCH...  WHAT TIME IS IT?
C ---  NOTE: NOT A GOOD IDEA TO ASSUME A VALUE FOR "NOW"
C
      CALL LIB$DATE_TIME ( NOW )
      READ(NOW(13:14),900) HOUR
      READ(NOW(1:2),900) DAY
      MONTH = M2I (NOW(4:6))
      CALL LIB$DAY_OF_WEEK ( ,IDAY )
      RETURN
900   FORMAT(I2)
      END
C
C---END SNOOZE
C
      SUBROUTINE ERROR (SEVERITY, MESS, ISTAT)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **           ERROR           **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (650) 604-5558
C*
C*     DESCRIPTION :
C*          An error has occurred.  Provide error messages to 
C*           appropriate individuals/logs.
C*          
C*          Severity = 
C*             (4) Fatal   - notify primary/secondary; log to console; 
C*                            log to klog; exit Kronos
C*             (3) Severe  - notify primary/secondary; log to console; 
C*                            log to klog
C*             (2) Error   - notify primary; log to klog
C*             (1) Warning - log to klog
C*             (0) Informative - log to klog
C*       Level 4 implies unrecoverable internal Kronos error.
C*       Level 3 implies internal Kronos error.
C*       Level 2 implies failure to parse/submit entry.
C*
C*       Note: don't change this interface unless you change the interface 
C*       in the version in KRON.FOR, as well.
C*
C*     INPUT ARGUMENTS :
C*          SEVERITY - How bad was the error (see above)
C*          MESS     - Text of the message
C*          ISTAT    - Status code returned by System Services (0 for none)
C*
C*     SUBPROGRAM REFERENCES :
C*          SYS$GETMSG,  OPER,  SENDW,  EXIT,  DELETE,  MAIL,  WRITE_IT
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Calls nonportable system routines.
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          12-JUL-1991  -  NEW VERSION WITH SEVERITY LEVELS
C*          07-AUG-1995  -  FIXED "STATEMENT OVERFLOWS..." ERROR
C*          15-AUG-1995  -  Make level two errors mail if message 
C*                           doesn't get through with SEND
C*
C***********************************************************************
C*
%ifdef cluster
      structure / status_block / 
         integer *2 lock_status, null
         integer *4 lock_id
      end structure
      record / status_block / iostatus
      common / locks / iostatus
      integer sys$deq
%endif
c
      CHARACTER *(*) MESS
      include 'kronos.cmn'
      INTEGER SEVERITY
      CHARACTER *512 LINE
      CHARACTER *256 SYSMESS
      parameter (max_users=10)
      common /k_users/ p_users(max_users), n_prime, s_users(max_users),
     $  n_second
      character *12 p_users, s_users
      logical errr
C
c --- setup error message
c
      if (severity .eq. 4) then
         line = char(7)//'Kronos Aborting !!!'
      else if (severity .eq. 3) then
         line = char(7)//'Severe Kronos error'
      else if (severity .eq. 2) then
         line = char(7)//'Kronos error'
      else if (severity .eq. 1) then
         line = ' Kronos warning'
      else
         line = ' Kronos info'
      endif
c
      ll = length(line)
      IF (ISTAT .NE. 0) THEN
         call SYS$GETMSG ( %VAL(ISTAT), L, SYSMESS,, )
         line (ll+1:) = ' ( ' // sysmess(1:l) // ' ).'
      ELSE
         line (ll+1:) = '.'
      ENDIF
      ll = min(length(line),(len(line)-1))
      LINE(ll+1:) = char(13) // char(10) // MESS(1:LENGTH(MESS))
c
c --- message has been built, send it to the right place(s)
c
c (4) Fatal   - notify primary/secondary; log to console; 
c                 log to ERROR.LOG; exit Kronos
c (3) Severe  - notify primary/secondary; log to console; 
c                 log to ERROR.LOG
c (2) Error   - notify primary; log to ERROR.LOG
c (1) Warning - log to ERROR.LOG
c (0) Informative - log to ERROR.LOG
c
      OPEN (UNIT=99, FILE='KRONOS_DIR:ERROR.LOG', STATUS='OLD',
     $ ACCESS='APPEND', ERR=5)
      call write_it (99, now, line(2:),errr)
      close(99,err=5)
      if (.not. errr) go to 6
5     ll = min(length(line),(len(line)-1))
      line(11+1:) = char(13) // char(10) //
     $ '(*** Log failure ***)'
c
c --- escalate priority since the log file isn't working
c
      if (severity .le. 1) severity = 2
c
6     if (severity .ge. 2) then
         num = 0
         do 20 i = 1, n_prime
            call sendw (p_users(i), line, n, m)
            num = num + n
20       continue
         if (severity .eq. 2) return
c
c --- Level 3... if no Prime users were informed, tell secondary
c
         do 25 i = 1, n_second
            call sendw (s_users(i), line, n, m)
            num = num + n
25       continue
c
c --- If still no one... better use MAIL
c
         if (num .lt. 1) then
            OPEN (UNIT=98, FILE='SCRATCH:TEMP.MAIL', STATUS='NEW',
     $            CARRIAGECONTROL='LIST',ERR=40)
            call write_it (98, now, line(2:), errr)
            close (98)
            if (errr) go to 40
            do 30 i = 1, n_prime
               call mail ( 'SCRATCH:TEMP.MAIL', p_users(i), ' ')
30          continue
            call delete ('SCRATCH:TEMP.MAIL', errr)
         endif
c
c --- Level 3 and 4 go on the operator's log
c
40       line = 
     $ '###########################################################' //
     $   char(13) // char(10) // line
         CALL OPER( LINE(1:200), 'CENTRAL' )
c
c --- Fatal error.  After all reporting, end program.
c
         if (severity .eq. 4) then
%ifdef cluster
            istat = sys$deq (%val(iostatus.lock_id),,,,)
            if (.not. istat) call lib$signal(%val(istat))
%endif
            call exit
         endif
      endif
c
      RETURN
      END
C
C---END error
C
      subroutine write_it (iunit, now, line, errr)
c
c  Just write a line to the specified unit, including the current date
c    and time... wrap every 132 columns.
c
      character *(*) now, line
      logical errr
c
      errr = .false.
      write (iunit,900,err=100) now
      ll = length(line)
      is = 1
      ie = ll
10    if (ie .gt. (is+131)) ie = is+131
      write(iunit,910,err=100) line(is:ie)
      if (ie .lt. ll) then
         is = ie + 1
         ie = ll
         go to 10
      endif
      return
900   format(/' ',a)
910   format(' ',a)
100   errr = .true.
      return
      end
c
      subroutine submit (i)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          SUBMIT           **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     SUBPROGRAM :
C*          SUBMIT WITH PARAMETERS (Kronos version)
C*
C*     AUTHOR :
C*          Arthur E. Ragosta
C*          RAGOSTA@MERLIN.ARC.NASA.GOV
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (650) 604-5558
C*
C*     PURPOSE :
C*          TO SUBMIT A JOB FOR BATCH EXECUTION
C*          Note:  SUBMIT is in the Kronos main source file because
C*             the KRON program uses a dummy SUBMIT program for its
C*             "List Runnable" option.
C*
C*     INPUT ARGUMENTS :
C*          I - number of entry
C*
C*     Internal :
C*          FNAME     - FILESPEC FOR THE '.COM' FILE TO SUBMIT
C*          QUEUE     - THE NAME OF THE BATCH QUEUE
C*          LOG       - LOG FILE NAME (BLANK FOR NONE)
C*          CHARACTER - CHARACTERISTIC
C*          CPUTIME   - MAX CPU TIME (10 MSEC TICS)
C*          PRIORITY  - JOB PRIORITY
C*          NOTIFY    - NOTIFY USER FLAG
C*          NP        - NUMBER OF PARAMETERS
C*          P         - PARAMETERS
C*
C*     SUBPROGRAM REFERENCES :
C*          LENGTH, PARSE, SYS$SNDJBCW, STRCPY, ERROR, GET_DELTA
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          ABSOLUTELY NOT TRANSPORTABLE
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           7-APR-89   INTITIAL VERSION
C*          27-APR-90   Kronos version (different error handling)
C*          31-JAN-91   Added FOR USER processing
C*          23-JUL-91   Added Priority, Characteristic, Notify, CPUtime
C*           9-JUL-92   Added PUSHHOLIDAY
C*          14-OCT-98   Made a little cleaner
C*
C***********************************************************************
C*
      include 'kronos.cmn'
      common / pushing / pushit
      logical pushit
      character *12 uname
      character *15 queue
      character *80 p(9)
      character *127 log, fname
      INTEGER SYS$SNDJBCW, delta(2)
      INCLUDE '($SJCDEF)'
      STRUCTURE /ITMLST/
         UNION
            MAP
               INTEGER *2 BUFLEN, ITMCOD
               INTEGER *4 BUFADR, RETADR
            END MAP
            MAP
               INTEGER *4 END_LIST
            END MAP
         END UNION
      END STRUCTURE
C
      STRUCTURE /IOSBLK/
         INTEGER *4 STS, ZEROED
      END STRUCTURE
C
      RECORD /ITMLST/ SUBMIT_LIST(20)
      RECORD /IOSBLK/ IOSB
C
      np = 0
10    if (entry(i).par(np+1) .ne. 0) then
         np = np + 1
         call strcpy (p(np), entry(i).par(np))
         go to 10
      endif
C
C --- QUEUE NAME
C
      if (entry(i).qptr .eq. 0) then
         queue = '%QUEUE'
      else
         call strcpy (queue, entry(i).qptr)
      endif
c
      SUBMIT_LIST( 1).BUFLEN = LENGTH(QUEUE)
      SUBMIT_LIST( 1).ITMCOD = SJC$_QUEUE
      SUBMIT_LIST( 1).BUFADR = %LOC(QUEUE)
      SUBMIT_LIST( 1).RETADR = 0
C
C --- .COM FILE NAME
C
      call strcpy (fname, entry(i).fptr)
      SUBMIT_LIST( 2).BUFLEN = LENGTH(FNAME)
      SUBMIT_LIST( 2).ITMCOD = SJC$_FILE_SPECIFICATION
      SUBMIT_LIST( 2).BUFADR = %LOC(FNAME)
      SUBMIT_LIST( 2).RETADR = 0
C
C --- .LOG FILE (IF ANY)
C
      if (entry(i).lptr .eq. 0) then
         SUBMIT_LIST( 3).BUFLEN = 0
         SUBMIT_LIST( 3).ITMCOD = SJC$_NO_LOG_SPECIFICATION
         SUBMIT_LIST( 3).BUFADR = 0
         SUBMIT_LIST( 3).RETADR = 0
         IP = 4
      else
         call strcpy (log, entry(i).lptr)
         SUBMIT_LIST( 3).BUFLEN = LENGTH(Log)
         SUBMIT_LIST( 3).ITMCOD = SJC$_LOG_SPECIFICATION
         SUBMIT_LIST( 3).BUFADR = %LOC(Log)
         SUBMIT_LIST( 3).RETADR = 0
         SUBMIT_LIST( 4).BUFLEN = 0
         SUBMIT_LIST( 4).ITMCOD = SJC$_NO_LOG_SPOOL
         SUBMIT_LIST( 4).BUFADR = 0
         SUBMIT_LIST( 4).RETADR = 0
         SUBMIT_LIST( 5).BUFLEN = 0
         SUBMIT_LIST( 5).ITMCOD = SJC$_NO_LOG_DELETE
         SUBMIT_LIST( 5).BUFADR = 0
         SUBMIT_LIST( 5).RETADR = 0
         IP = 6
      ENDIF
c
c --- if for another user then
c
      if (entry(i).forptr .ne. 0) then
         call strcpy (uname, entry(i).forptr)
         submit_list(ip).buflen = length(uname)
         submit_list(ip).itmcod = SJC$_USERNAME
         submit_list(ip).bufadr = %loc(uname)
         submit_list(ip).retadr = 0
         ip = ip + 1
      endif
c
c --- Characteristic
c
      if (entry(i).character .ne. 0) then
         submit_list(ip).buflen = 4
         submit_list(ip).itmcod = SJC$_CHARACTERISTIC_NUMBER
         submit_list(ip).bufadr = %loc(entry(i).character)
         submit_list(ip).retadr = 0
         ip = ip + 1
      endif
c
c --- cputime limit
c
      if (entry(i).cputime .ne. 0) then
         submit_list(ip).buflen = 4
         submit_list(ip).itmcod = SJC$_CPU_LIMIT
         submit_list(ip).bufadr = %loc(entry(i).cputime)
         submit_list(ip).retadr = 0
         ip = ip + 1
      endif
c
c --- priority
c
      if (entry(i).priority .ne. 0) then
         submit_list(ip).buflen = 4
         submit_list(ip).itmcod = SJC$_PRIORITY
         submit_list(ip).bufadr = %loc(entry(i).priority)
         submit_list(ip).retadr = 0
         ip = ip + 1
%ifdef hi_priority
      else
         iprior = 200
         submit_list(ip).buflen = 4
         submit_list(ip).itmcod = SJC$_PRIORITY
         submit_list(ip).bufadr = %loc(iprior)
         submit_list(ip).retadr = 0
         ip = ip + 1
%endif
      endif
c
c --- notify
c
      if (entry(i).notify) then
         submit_list(ip).buflen = 0
         submit_list(ip).itmcod = SJC$_NOTIFY
         submit_list(ip).bufadr = 0
         submit_list(ip).retadr = 0
         ip = ip + 1
      endif
c
c --- PUSHHOLIDAY
c
      if (pushit) then
         call get_delta (delta)
         submit_list(ip).buflen = 8
         submit_list(ip).itmcod = SJC$_AFTER_TIME
         submit_list(ip).bufadr = %loc(delta)
         submit_list(ip).retadr = 0
         ip = ip + 1
      endif
C
C --- PARAMETERS
C
      SUBMIT_LIST(IP  ).ITMCOD = SJC$_PARAMETER_1
      SUBMIT_LIST(IP+1).ITMCOD = SJC$_PARAMETER_2
      SUBMIT_LIST(IP+2).ITMCOD = SJC$_PARAMETER_3
      SUBMIT_LIST(IP+3).ITMCOD = SJC$_PARAMETER_4
      SUBMIT_LIST(IP+4).ITMCOD = SJC$_PARAMETER_5
      SUBMIT_LIST(IP+5).ITMCOD = SJC$_PARAMETER_6
      SUBMIT_LIST(IP+6).ITMCOD = SJC$_PARAMETER_7
      SUBMIT_LIST(IP+7).ITMCOD = SJC$_PARAMETER_8
      DO 20 j = 1, NP
         SUBMIT_LIST(IP).BUFLEN = LENGTH(P(J))
         SUBMIT_LIST(IP).BUFADR = %LOC(P(J))
         SUBMIT_LIST(IP).RETADR = 0
         IP = IP + 1
20       CONTINUE
C
C --- ALL DONE
C
      SUBMIT_LIST(IP).END_LIST = 0
C
%ifdef test_only
      WRITE(6,*) 'Would have submitted job ' // fname
      IF (PUSHIT) WRITE(6,*) 'Should be pushed.'
%else
C
C --- Note: SYS$SNDJBCW sometimes returns success even if the job wasn't
C     submitted.  Thus the unusual set of IFs below.
C
      J = SYS$SNDJBCW (,%VAL(SJC$_ENTER_FILE),,SUBMIT_LIST,IOSB,,)
      IF (J) J = IOSB.STS
      IF (.NOT. J) then
         CALL error (2,  'Unable to submit job ' //
     $                         fname(1:max(1,length(fname))), J)
         OPEN (UNIT=99, FILE='KRONOS_DIR:ERROR.LOG', STATUS='OLD',
     $    ACCESS='APPEND')
         write(99,900) 'Queue = ' // queue 
         if (np .gt. 0) then
            write(99,900) 'With parameters.'
            do 30 ii = 1,np
               write(99,900) '    ' // p(ii)
30          continue
         endif
         if (log .ne. ' ') then
            llog = min0(120,length(log))
            write(99,900) 'Log file = ' // log(1:llog)
         endif
         if (entry(i).forptr .ne. 0) write(99,900) 'For = ' // uname
         if (entry(i).character .ne. 0) write(99,900) 'Characteristics.'
900      format(' ',a)
         close(99)
      endif
%endif
      RETURN
      END
C
C---END SUBMIT
C
      LOGICAL FUNCTION CHECK_Q ( QUEUE )
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **         CHECK_Q           **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     SUBPROGRAM :
C*          Check queue
C*
C*     AUTHOR :
C*          ARTHUR E. RAGOSTA
C*          MS 219-1
C*          NASA AMES RESEARCH CENTER
C*          MOFFETT FIELD, CA  94035
C*          (650) 604-5558
C*
C*     PURPOSE :
C*          Check a batch queue to make sure it exists and is running
C*          Return TRUE if anything is wrong; FALSE otherwise
C*
C*     INPUT ARGUMENTS :
C*          QUEUE - THE NAME OF THE BATCH QUEUE
C*
C*     SUBPROGRAM REFERENCES :
C*          sys$getquiw, length
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          ABSOLUTELY NOT TRANSPORTABLE
C*
C*     LANGUAGE AND COMPILER :
C*          VAX FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           15-Oct-91  -  Initial version
C*
C***********************************************************************
C*
      CHARACTER *(*) QUEUE
      INCLUDE '($QUIDEF)'
      INTEGER SYS$GETQUIW, STATUS, FLAGS
      STRUCTURE /ITMLST/
         UNION
            MAP
               INTEGER *2 BUFLEN, ITMCOD
               INTEGER *4 BUFADR, RETADR
            END MAP
            MAP
               INTEGER *4 END_LIST
            END MAP
         END UNION
      END STRUCTURE
C
      STRUCTURE /IOSBLK/
         INTEGER *4 STS, ZEROED
      END STRUCTURE
C
      RECORD /ITMLST/ GET_LIST(6)
      RECORD /IOSBLK/ IOSB
C
      CHECK_Q = .TRUE.
      IF (QUEUE .EQ. ' ') RETURN
      GET_LIST( 1).BUFLEN = LENGTH(QUEUE)
      GET_LIST( 1).ITMCOD = QUI$_SEARCH_NAME
      GET_LIST( 1).BUFADR = %LOC(QUEUE)
      GET_LIST( 2).BUFLEN = 4
      GET_LIST( 2).ITMCOD = QUI$_QUEUE_STATUS
      GET_LIST( 2).BUFADR = %LOC(FLAGS)
      GET_LIST( 3).END_LIST = 0
C 
      STATUS = SYS$GETQUIW (,%VAL(QUI$_DISPLAY_QUEUE),,GET_LIST,IOSB,,)
      IF (STATUS) STATUS = IOSB.STS
      IF (.NOT. STATUS ) RETURN
C
C --- CHECK FOR STOPPED, STALLED, ETC
C
      IF (((FLAGS .AND. QUI$M_QUEUE_CLOSED) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_PAUSED) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_PAUSING) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_RESETTING) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_STALLED) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_STOPPED) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_STOPPING) .NE. 0) .OR.
     $    ((FLAGS .AND. QUI$M_QUEUE_UNAVAILABLE) .NE. 0)) RETURN
      CHECK_Q = .FALSE.
      RETURN
      END
C
C---END CHECK_Q
C
