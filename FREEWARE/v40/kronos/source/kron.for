      Program Kron
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **           Kron            **
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
C*          Moffett Field, Ca.  94035-1000
C*          (650) 604-5558
C*
C*     DESCRIPTION :
C*          KRON is the user interface for Kronos.  KRON adds new jobs
C*          to the data file and verifies format, files, etc.
C*          See KRONOS.DOC for details.
C*
C*     SUBPROGRAM REFERENCES :
C*          clear,  prompt,  left,  caps,  add_job,  parse_file
C*          list_jobs,  exit
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Kron is not transportable.
C*
C*     LANGUAGE AND COMPILER :
C*          DEC FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          15-AUG-1989 -  INITIAL VERSION
C*          24-JUL-1990 -  New version with command interface
C*          04-FEB-1991 -  DELETE option deleted, RLIST option added
C*                          various changes to support new Kronos 
C*                          capability
C*          26-FEB-1991 - bug in kgetline fixed
C*          22-APR-1991 - added code for fifteen minute granularity
C*          24-APR-1991 - added FIND option
C*          15-AUG-1991 - added support for new style (/) qualifiers
C*          06-JAN-1992 - added variable interval wakeup code
C*          15-MAR-1994 - fixed bug with default directories
C*          24-JAN-1995 - added warning for non-existant job in LIST
C*          30-MAR-1995 - added Show Schedule option
C*          08-JUL-1997 - added Help to prompt; added Version command
C*
C***********************************************************************
C*
      parameter (max_queues=40)
      include 'kronos.cmn'
      include 'strings.cmn'
      common / find / nums(100), num_nums, finding, fstring
      logical finding
      character *80 fstring
      common / qs / q_names(max_queues), n_queues
      character *31 q_names
      character *23 date1, date2, d3, d4, d5
      character *80 p(2), q(2), mfile
      character *100 pline, pline1
      character *8 response
      character *4 on, off
      character *2 crlf
      logical file_parsed
      data file_parsed/.false./, finding/.false./
c
      kron_vers = 'II.11'
c
c --- file to maintain.  Defaults to master.
c
      mfile = 'KRONOS_DIR:KRONOS.DAT'
      call getfor (nq, q, np, p)
      if (np .gt. 0) mfile = p(1)
c
c --- batch queues that Kronos is to recognize.
c
      n_queues = 0
      OPEN (UNIT=1, FILE='KRONOS_DIR:QUEUES.DAT', STATUS='OLD',
     $ readonly, ERR=8)
4     read(1,910,end=6) q_names(n_queues+1)
      if (q_names(n_queues + 1) .ne. ' ') n_queues = n_queues + 1
      call caps (q_names(n_queues))
      call left (q_names(n_queues))
      if (n_queues .lt. max_queues) go to 4
6     close(1)

c
c --- Get current default directory for any added jobs.  Initially, the
c      current directory is the default.
c
8     call get_default (def_dir)
c
      call clear
      call lib$date_time ( now )       ! Needed for LIST
      READ(NOW(13:14),940) HOUR        ! and RLIST
      READ(NOW(1:2),940) DAY
      MONTH = M2I (NOW(4:6))
      CALL LIB$DAY_OF_WEEK ( ,IDAY )
c
      on = char(27) // '[1m'
      off= char(27) // '[0m'
      crlf = char(13) // char(10)
      pline = '   '
     $            // on // 'A' // off // ')dd, '
     $            // on // 'C' // off // ')alendar add, '
     $            // on // 'F' // off // ')ind, '
     $            // on // 'L' // off // ')ist,'
     $ // 'list ' // on // 'R' // off // ')unnable jobs, '
      pline1 = '  '
     $            // on // 'S' // off // ')how schedule, '
     $            // on // 'H' // off // ')elp, '
     $            // on // 'V' // off // ')ersion, '
     $            // on // 'Q' // off // ')uit ?  [ Q ]  '
      lp = length(pline1) + 1
c
c --- loop until user enters Quit
c
10    write(6,900)
      write(6,910) pline(1:length(pline))
      call prompt (pline1(1:lp))
      read(5,910,end=30) response
      call left(response)
      call caps(response)
c
c --- Add new job
c
      if (response(1:1) .eq. 'A') then
         nstring = 1
         call add_job (mfile)
         file_parsed = .false.
c
c --- Add job to calendar file
c
      else if (response(1:1) .eq. 'C') then
         nstring = 1
         call cadd_job
         file_parsed = .false.
c
c --- List all jobs in data base
c
      else if (response(1:1) .eq. 'L') then
c
         call clear
c
         if (.not. file_parsed) then
            call parse_file (mfile)
            file_parsed = .true.
         endif
         call list_jobs
c
c --- List only those jobs that would run if Kronos awakened now
c
      else if (response(1:1) .eq. 'R') then
         if (.not. file_parsed) then
            call parse_file (mfile)
            file_parsed = .true.
         endif
c
c --- We let DO_JOBS do all the hard work, but since we want the jobs
c ---  that will be run next wakeup, we must fool DO_JOBS into thinking 
c ---  its then!
c
c --- Update NOW in case we've been in Kron for awhile
c
         call lib$date_time ( now )
         READ(NOW(13:14),940) HOUR
         READ(NOW(1:2),940) DAY
         MONTH = M2I (NOW(4:6))
         CALL LIB$DAY_OF_WEEK ( ,IDAY )
c
c --- NOTE --- the following calculation fails after 11:00 PM on the 
c              last day of the month
c
c --- Add the interval to the actual time since the job executes any 
c     time in the interval
c
%ifdef interval
         read(now(16:17),940) min
         i = mod(min,%INTERVAL)
         min = min + %INTERVAL - i
         if (min .gt. 59) then
            min = min - 60
            hour = hour + 1
            if (hour .gt. 23) then
               hour = hour - 24
               day = day + 1
               write(now(1:2),940) day
            endif         
            write(now(13:14),940) hour
         endif
         write(now(16:17),940) min
%else
         hour = hour + 1
         if (hour .gt. 23) then
            hour = hour - 24
            day = day + 1
            write(now(1:2),940) day
         endif         
         write(now(13:14),940) hour
%endif
c
         call do_jobs
c
c --- Find
c
      else if (response(1:1) .eq. 'F') then
         call prompt ( 'Find what string ? ' )
         read(5,910) fstring
         if (fstring .ne. ' ') then
            call caps (fstring)
            finding = .true.
            num_nums = 0
            if (.not. file_parsed) then
               call parse_file (mfile)
               file_parsed = .true.
            endif
            call list_jobs
            finding = .false.
            if (num_nums .eq. 0) then
               write(6,930)
            else
               do 20 j = 1, num_nums
                  call list_kernel ( nums(j) )
20             continue
            endif
         endif
c
c --- Show Schedule
c
      else if (response(1:1) .eq. 'S') then
         call clear
c
         if (.not. file_parsed) then
            call parse_file (mfile)
            file_parsed = .true.
         endif
         call show_sched
c
c --- Quit
c
      else if ((response(1:1) .eq. ' ') .or.
     $         (response(1:1) .eq. 'Q')) then
         go to 30
c
c --- Help
c
      else if ((response(1:1) .eq. 'H') .or.
     $         (response(1:1) .eq. '?')) then
         write(6,950)
c
c --- Version
c
      else if (response .eq. 'V') then
         call file_dates ('kronos_dir:kronos.exe', date1, d3, d4, d5)
         call file_dates ('kronos_dir:kron.exe', date2, d3, d4, d5)
         write(6,960) kron_vers, date1, date2
      else
         write(6,920)
      endif
c
      go to 10
30    call exit
c
900   format(/)
910   format(a)
920   format(' Illegal option... try again.')
930   format(' String not found.')
940   format(i2)
950   format(//
     $ ' Choose an option from the list by typing in at least one',/,
     $ ' character of the command.  The commands are as follows:',//,
     $ '    Add - add a new job to the daily data base.',//,
     $ '    Calendar Add - add a new job to one of the calendar data',/,
     $ '          bases (this year''s or permanent).  You will be,',/,
     $ '          asked which calendar file.',//,
     $ '    List - list all jobs in the data bases (i.e., daily and',/,
     $ '          calendar files) with full details - one per screen.'//
     $ '    Runnable list - list only those files in the data bases',/,
     $ '          that will run the next time Kronos wakes up.',//,
     $ '    Find - find a job in the data bases that contains the',/,
     $ '          specified string.  You will be prompted for the',/,
     $ '          string.  Any text may be searched; for example,',/,
     $ '          entering MONDAY will display all jobs that are to',/,
     $ '          be run on Mondays.  Searches are not case sensitive.'/
     $ '    Show Schedule - show what jobs are scheduled to be run'/
     $ '          either today or this week.'/
     $ /'    Quit - exit the program.')
955   format(' Function not yet available.')
960   format(/' Kronos version ',a,/,' Kronos executable created ',a,/,
     $       ' Kron executable created ', a)
      end
c
c---end Kron
c
      SUBROUTINE ERROR (SEVERITY, MESS, ISTAT)
C*
C*    ERROR, Give reason and continue.
C*
C*    Note:  This routine is needed because it may be called by KPARSE.
C*    Don't mess with the interface unless you change the version in 
C*    KRONOS.FOR, as well.
C*
      CHARACTER *512 LINE
      CHARACTER *256 SYSMESS
      CHARACTER *(*) MESS
      INTEGER SEVERITY
C
c --- Severity=4 is a Kronos condition, Kron needn't really abort.
c
      if (severity .eq. 4) then
         line = char(7)//'Kron semifatal error !!!'
      else if (severity .ne. 0) then
         line = char(7)//'Kron error'
      endif
c
c --- text of message (if any)
c
      ll = length(line)
      IF (ISTAT .NE. 0) THEN
         IS = SYS$GETMSG ( %VAL(ISTAT), L, SYSMESS,, )
         line (ll+1:) = ' ( ' // sysmess(1:l) // ' ).'
      ELSE
         line (ll+1:) = '.'
      ENDIF
c
c --- write it
c
      ll = length(line)
      LINE(ll+1:) = char(13) // char(10) // MESS(1:LENGTH(MESS)) //
     $              char(13) // char(10)
      call lib$put_output (line(1:length(line)))
      RETURN
      END
C
C---END error
C
      subroutine list_jobs
c*
c*  List all jobs in the data base with run info
c*
      include 'strings.cmn'
      include 'kronos.cmn'
      common / find / nums(100), num_nums, finding, fstring
      logical finding
      character *80 fstring
c
      do 10 i = 1,num_jobs
         if (.not. finding) then
            call clear
            write(6,915) (maxstring-nstring)
         endif
         call list_kernel(i)
c
c ----- pause
c
         if ((i .ne. num_jobs) .and. (.not. finding)) then
            write(6,920) char(27), char(27)
            read(5,930,end=100)
         endif
10    continue
100   return
915   format(' Space left in string heap = ',I6)
920   format(//,' ',a1,
     $ '[1m --- Press <RETURN> to continue... ^Z for menu ---',a1,'[0m')
930   format()
      end
c
c---end list_jobs
c
      subroutine list_kernel (i)
c*
c*   List one job in the data base with run information
c*
      include 'kronos.cmn'
      common / find / nums(100), num_nums, finding, fstring
      logical finding, exists
      character *80 fstring
      character *132 temp, temp1
      character *9 wday(7), hstring
      character *6 astring
      character *4 ops(12)
      data wday /'Monday', 'Tuesday', 'Wednesday', 'Thursday',
     $ 'Friday', 'Saturday', 'Sunday'/
      data ops /'<', '>', '==', '<=', '>=', '<>', 'IN', 'NIN',
     $ '&', 'NOT&', '|', 'NOT|'/
c
      if (.not. finding) write(6,900) i
c
c --- IF
c
      if (entry(i).ifcode .ne. 0) then
         temp = ' '
         call show_var (1, i, temp, 1)
         if (entry(i).opcode .ne. 0) then
            lt = length(temp) + 2
            temp(lt:) = ops(entry(i).opcode)
            lt = length(temp) + 2
            call show_var (2, i, temp, lt)
         endif
c
         temp = 'If ' // temp
         if (finding) then
            temp1 = temp
            call caps (temp1)                    
            if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
               num_nums = num_nums + 1
               nums(num_nums) = i
               return
            endif
         else
            write(6,910) temp(1:length(temp))
         endif
      endif
c
c --- AT
c
      if (entry(i).atcode .eq. -1) then
         write(6,940)      ! at always
      else if (entry(i).atcode .ne. -2) then
c
%ifdef interval
c
         if (entry(i).atcode .lt. 10000) then
            ihour = entry(i).atcode / 60
            imin  = entry(i).atcode - (60*ihour)
            write(temp,950) ihour,imin
         else
            k = entry(i).atcode / 10000
            j = entry(i).atcode - 10000*k
            ihour = j / 60
            imin  = j - (60*ihour)
            ihourk= k / 60
            imink = k - (60*ihourk)
            if (entry(i).hourly) then
               astring = ' '
            else
               astring = 'ALWAYS'
            endif
            write(temp,955) ihour,imin,ihourk,imink,astring
         endif
c
%else
c
         if (entry(i).atcode .lt. 10000) then
            write(temp,960) entry(i).atcode 
         else
            k = entry(i).atcode / 10000
            j = entry(i).atcode - 10000*k
            write(temp,965) j,k
         endif
c
%endif
c
         if (finding) then
            temp1 = temp
            call caps(temp1)
            if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
               num_nums = num_nums + 1
               nums(num_nums) = i
               return
            endif
         else
            write(6,910) temp(1:length(temp))
         endif
      endif
c
c --- ON
c
      if (entry(i).oncode .ne. 0) then
         k = entry(i).oncode
         if (entry(i).nohol) then
            hstring = 'NOHOLIDAY'
         else if (entry(i).pushhol) then
            hstring = 'PUSHHOLIDAY'
         else
            hstring = ' '
         endif
         if ((k .ge. 1) .and. (k .le. 7)) then           ! Day of week
            write(temp,970) wday(k), hstring
         else if (k .eq. 8) then                         ! Every weekday
            write(temp,976) hstring
         else if ((k .ge. 101) .and. (k .le. 131)) then  ! day of month
            write(temp1,980) (k-100)
            call ordinal(temp1)
            lt = left(temp1)
            write(temp,971) temp1(1:lt), hstring
         else if ((k .ge. 201) .and. (k .le. 222)) then  ! weekday of month
            write(temp1,980) (k-200)
            call ordinal(temp1)
            lt = left(temp1)
            write(temp,972) temp1(1:lt), hstring
         else if ((k .ge. 301) .and. (k .le. 331)) then  ! end of month
            write(temp1,980) (k-300)
            call ordinal(temp1)
            lt = left(temp1)
            write(temp,973) temp1(1:lt), hstring
         else if ((k .ge. 401) .and. (k .le. 422)) then  ! weekday end of mth
            write(temp1,980) (k-400)
            call ordinal(temp1)
            lt = left(temp1)
            write(temp,974) temp1(1:lt), hstring
         else                                            ! Calendar entry
            mm = (k-10000) / 100
            md = (k-10000) - 100*mm
            write(temp,975) mm, md, hstring
         endif
c
         if (finding) then
            temp1 = temp
            call caps(temp1)
            if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
               num_nums = num_nums + 1
               nums(num_nums) = i
               return
            endif
         else
            write(6,910) temp(1:length(temp))
         endif
      endif
c
c --- FOR
c
      if (entry(i).forptr .ne. 0) then
         call strcpy ( temp, entry(i).forptr)
         temp = ' For '//temp
c
         if (finding) then
            temp1 = temp
            call caps(temp1)
            if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
               num_nums = num_nums + 1
               nums(num_nums) = i
               return
            endif
         else
            write(6,910) temp(1:length(temp))
         endif
      endif
c
c --- job entry.  Always something here
c
c ------ job file name
c
      call strcpy (temp, entry(i).fptr)   
c
      if (finding) then
         temp1 = temp
         call caps(temp1)
         if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
            num_nums = num_nums + 1
            nums(num_nums) = i
            return
         endif
      else
         write(6,9000) temp(1:length(temp))
         if (.not. exists(temp(1:length(temp)))) write (6,9005), 
     $      char(27), char(27)
      endif
c
c ------ log file name
c
      if (entry(i).lptr .eq. 0) then
         if (.not. finding) write(6,9010)
      else
         call strcpy (temp, entry(i).lptr)
         if (finding) then
            temp1 = temp
            call caps(temp1)
            if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
               num_nums = num_nums + 1
               nums(num_nums) = i
               return
            endif
         else
            write(6,9020) temp(1:length(temp))
         endif
      endif
c 
c ------ batch queue name
c
      if (entry(i).qptr .eq. 0) then
         temp = '%QUEUE'
      else
         call strcpy (temp, entry(i).qptr)
      endif
c
      if (finding) then
         temp1 = temp
         call caps(temp1)
         if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
            num_nums = num_nums + 1
            nums(num_nums) = i
            return
         endif
      else
         write(6,9030) temp(1:length(temp))
      endif
c
c ------ 0-8 parameters
c
      j = 1
5     if (entry(i).par(j) .ne. 0) then
         call strcpy (temp, entry(i).par(j))
         if (finding) then
            temp1 = temp
            call caps(temp1)
            if (index(temp1,fstring(1:length(fstring))) .ne. 0) then
               num_nums = num_nums + 1
               nums(num_nums) = i
               return
            endif
         else
            write(6,9040) j, temp(1:length(temp))
         endif
         if (j .le. 7) then
            j = j + 1
            go to 5
         endif
      endif
c
      if (finding) return
c
c --- Characteristic
c
      if (entry(i).character .ne. 0) write (6,9050) entry(i).character      
c
c --- CPUtime
c
      if (entry(i).cputime .ne. 0) then
         temp = ' '
         istat = lib$format_date_time (temp,entry(i).cputime)
         write(6,9060) temp(1:length(temp))
      endif
c
c --- Notify
c
      if (entry(i).notify) write (6,9070)
c
c --- priority
c
      if (entry(i).priority .ne. 0) write(6,9080) entry(i).priority
c
100   return
900   format(//' *** Job entry number ',I2,' ***'/)
910   format(' ',a)
c
940   format(' At ALWAYS')
%ifdef interval
950   format(' At ',I2.2,':',I2.2)
955   format(' At ',I2.2,':',I2.2,'-',I2.2,':',I2.2,2x,a)
%else
960   format(' At ',I2,':00')
965   format(' At ',I2,':00-',I2,':00')
%endif
c
970   format(' On ',a,1x,a)
971   format(' On ',a,' day of month   ',a)
972   format(' On ',a,' weekday of month   ',a)
973   format(' On ',a,' day from end of month   ',a)
974   format(' On ',a,' weekday from end of month   ',a)
975   format(' On ',i2.2,'/',i2.2,3x,a)
976   format(' On EveryWeekday   ',a)
980   format(I3)
c990   format(' For ',a)
9000  format('    File name = ',a)
9005  format('    ',a1,'[5m*** warning... job file does not exist! ***',
     $ a1,'[0m')
9010  format('    No log file.')
9020  format('    Log file = ',a)
9030  format('    Batch queue = ',a)
9040  format('        Parameter ',i1,' = ',a)
9050  format('    Characteristic ',i3)
9060  format('    CPU time limit = ',a)
9070  format('    Notify')
9080  format('    Priority = ',i3)
      end
c
c---end list_jobs
c
      subroutine show_var (ione, i, temp, lt)
c*
c*  Add a variable and all its parameters to string 'temp'
c*
      include 'kronos.cmn'
      character *(*) temp
c
      go to (10,20,30,40,50,60,70,80), entry(i).variable(ione).type
c
c --- integer
c
10    write(temp(lt:),900) entry(i).variable(ione).value
      call left(temp(lt:))
      return
c
c --- string
c
20    temp(lt:lt) = '"'
      call strcpy (temp(lt+1:), entry(i).variable(ione).value)
      it = length(temp) + 1
      temp(it:it) = '"'
      return
c
c --- system function
c
30    temp(lt:) = 'SYSTEM('
      it = lt + 7
      call show_fun (ione, i, temp, it)
      return
c
c --- process function
c
40    temp(lt:) = 'PROCESS('
      it = lt + 8
      call show_fun (ione, i, temp, it)
      return
c
c --- print function
c
50    temp(lt:) = 'PRINT('
      it = lt + 6
      call show_fun (ione, i, temp, it)
      return
c
c --- security function
c
60    temp(lt:) = 'SECURITY('
      it = lt + 9
      call show_fun (ione, i, temp, it)
      return
c
c --- device function
c
70    temp(lt:) = 'DEVICE('
      it = lt + 7
      call show_fun (ione, i, temp, it)
      return
c
c --- logical name
c
80    call strcpy (temp(lt:), entry(i).variable(ione).value)
      return
900   format(i10)
      end
c
c---end show_var
c
      subroutine show_fun (ione, i, temp, it)
c*
c*  Place the parameter list for the function in "temp"
c*
      character *(*) temp
      include 'kronos.cmn'
c
      call strcpy (temp(it:), entry(i).variable(ione).param1)
      it = length(temp) + 1
      if (entry(i).variable(ione).param2 .ne. 0) then
         temp(it:it) = ','
         it = it + 1
         call strcpy (temp(it:), entry(i).variable(ione).param2)
         it = length(temp) + 1
      endif
      temp(it:it) = ')'
      return
      end
c
c---end show_fun
c
      subroutine add_job (mfile)
c*
c*  Add a new job to the data base.  Prompt the user for the parts and
c*       do as much error checking as is reasonable.
c*
      character *(*) mfile
      character *512 tline
      character *9 ttime
      logical ok, err
c
      call add_kernel (tline, ok)
      if (ok) then
c
c --- everything OK... see if data base is free and if so, add job entry
c
c --- if the current time is near the hour (59 minutes and 58/59 secs or
c      00 minutes and 0/1 secs) then pause 5 seconds just to be certain 
c ---  Kronos and Kron don't both try to access the data base.
c
c ---  Note :  This technique doesn't work for 15 minute interval
c
         call time (ttime)
         if (((ttime(4:5) .eq. '00') .and.
     $           ((ttime(7:8) .eq. '00') .or. (ttime(7:8) .eq. '01')))
     $                 .or.
     $       ((ttime(4:5) .eq. '59') .and.
     $           ((ttime(7:8) .eq. '59') .or. (ttime(7:8) .eq. '58'))))
     $      then
            write(6,9000) '6 second pause'
            call sleep(6.)
         endif
         OPEN (UNIT=1, FILE=mfile, STATUS='OLD',
     $     access='APPEND', carriagecontrol='LIST', ERR=1000)
         call write_line (tline)
         close (1)
      endif
      return
c
1000  write(6,990) mfile
      ok = .false.
      call prompt ('Do you wish to create a new file ? ')
      call yesno (ok, err)
      if (err) go to 1000
      if (ok) then
         OPEN (UNIT=1, FILE=mfile, STATUS='NEW', 
     $    carriagecontrol='LIST', ERR=1010)
         call write_line(tline)
         close(1)
      endif
      return
c
1010  write(6,980)
      return
980   format(' Unable to create data base file.')
990   format(' Unable to open data base file ',a)
9000  format(' ',a)
      end
c
c---end add_job
c
      subroutine cadd_job
c*
c*  Add a new job to the calendar file.  Prompt the user for the parts and
c*       do as much error checking as is reasonable.
c*
      character *512 tline
      character *60 fname
      character *20 dstring
      logical ok
c
c get which file, date stuff
c
      fname = 'KRONOS_DIR:CALENDAR.'
      call prompt ('Update file ' // fname(1:length(fname)) )
      read(5,900) fname(21:)
      if (fname(21:) .eq. ' ') fname(21:) = 'DAT'
      call caps (fname)
c
      write(6,910)
      read(5,900) dstring
      call left(dstring)
      call caps(dstring)
c
      call add_kernel (tline,ok)
      if (.not. ok) return
      OPEN (UNIT=1, FILE=fname(1:length(fname)), STATUS='OLD',
     $  access='APPEND', carriagecontrol='LIST', ERR=100)
      go to 200
c
100   write (6,920) fname(1:length(fname))
      OPEN (UNIT=1, FILE=fname(1:length(fname)), STATUS='NEW',
     $  carriagecontrol='LIST',ERR=1000)
c
200   call write_line(dstring(1:length(dstring)+1) //
     $              tline(1:length(tline)))
      close(1)
      return
1000  write(6,990)
      return
900   format(a)
910   format(' mm/dd HOLIDAY  or'/' mm/dd-dd HOLIDAY  or'/
     $       ' mm/dd  or'/' mm/dd-dd')
920   format(' Creating file ',a)
990   format(' Unable to open calendar file.')
      end
c
c---end cadd_job
c
      subroutine write_line ( line )
c*
c*  Write a line to the file.  If the line is too long to fit on a 
c*   screen, try to break it at a space and put a continuation mark at
c*   the end of the line.
c*
      character *(*) line
c
      ll = length(line)
      istart = 1
      iend = 80
c
c --- more than one line left to print?
c
10    iend = min0(iend,ll)
      if (iend .lt. ll) then
c
c --- yes... break it at a space
c
         iends = iend
20       if (line(iend:iend) .ne. ' ') then      ! Try to backup to a space
            iend = iend - 1
            if (iend .gt. (istart+1)) go to 20   ! Can't do it...
            iend = iends + 10                     ! Look forward a bit
            go to 10
         endif
c
c --- OK, print the line with continuation
c
         if (istart .eq. 1) then
            write (1,900) line(istart:iend) // '-'
         else
            write (1,910) line(istart:iend) // '-'
         endif
         istart = iend + 1
         iend = istart+79
         go to 10
      else
c
c --- last line of printout
c
         if (istart .eq. 1) then
            write (1,900) line(istart:iend)
         else
            write (1,910) line(istart:iend)
         endif
      endif
      return
900   format(a)
910   format(3x,a)
      end
c
c---end write_line
c
      subroutine add_kernel (tline,ok)
c*
c*   This routine is called by both the ADD function and the CALENDAR 
c*   ADD function.  It gets all information and builds the clean entry
c*   in "tline".
c*
      include 'kronos.cmn'
      include 'kparse.cmn'
      character *(*) tline
      character *127 fname
      logical err, exists, yes, ok
      parameter (max_queues=40)
      common / qs / q_names(max_queues), n_queues
      character *31 q_names
c
c --- need to initialize num_jobs because we are going to use kparse
c
      num_jobs = 1
      iunit = 5
      tline = ' '
      ok = .true.
c
c --- get IF clause
c
10    it = 1
      write(6,910)
      call kgetline
15    if (line .ne. ' ') then
         call caps(line)
         il = 1
         ll = length(line)
         call token
         if (toke .ne. 'IF') then
            line = 'IF ' // line
            go to 15
         endif
         call parse_if (err)
         if (err) then
            err = .false.
            go to 10
         endif
         tline = line
         it = length(tline) + 2
      endif
c
c --- get AT clause
c
20    write (6,920)
      call kgetline
25    if (line .ne. ' ') then
         call caps(line)
         il = 1
         ll = length(line)
         call token
         if (toke .ne. 'AT') then
            line = 'AT ' // line
            go to 25
         endif
         call parse_at (err)
         if (err) then
            err = .false.
            go to 20
         endif
         tline(it:) = line
         it = length(tline) + 2
      endif
c
c --- get ON clause
c
30    write (6,930)
      call kgetline
35    if (line .ne. ' ') then
         call caps(line)
         il = 1
         ll = length(line)
         call token
         if ((toke .ne. 'ON') .and.
     $       (toke .ne. 'NOHOLIDAY') .and.
     $       (toke .ne. 'PUSHHOLIDAY')) then
            line = 'ON ' // line
            go to 35
         endif
         call parse_on (err)
         if (err) then
            err = .false.
            go to 30
         endif
         tline(it:) = line
         it = length(tline) + 2
      endif
c
c --- get FOR clause
c
37    write (6,935)
      call kgetline
38    if (line .ne. ' ') then
         call caps(line)
         il = 1
         ll = length(line)
         call token
         if (toke .ne. 'FOR') then
            line = 'FOR ' // line
            go to 38
         endif
         call parse_for (err)
         if (err) then
            err = .false.
            go to 37
         endif
         tline(it:) = line
         it = length(tline) + 2
      endif
c
c --- get JOB entry
c
40    write(6,940)
      call kgetline
      ll = length(line)
      if (ll .eq. 0) then
         ok = .false.
         return
      endif
      call caps(line)
      il = 1
      call token
c
c  if TOKE .eq. '?' then... show directory of *.com
c
      call parse_job (err)
      if (err) then
         err = .false.
         go to 40
      endif
c
c --- add full filespec for job
c
      tline(it:) = line
c
c --- verify file exists
c
      call strcpy (fname, entry(1).fptr)
      if (.not. exists(fname)) then
         call prompt (
     $    'File doesn''t exist... add it anyway ? [Y] ')
         yes = .true.
         call yesno (yes, err)
         if (err .or. (.not. yes)) then
            err = .false.
            go to 40
         endif
      endif
c
c --- check queue name
c
      if (entry(1).qptr .ne. 0) then
         call strcpy (fname, entry(1).qptr)
         do 50 i = 1, n_queues
            if (fname .eq. q_names(i)) go to 60
50       continue
         call prompt (
     $    'Queue name is not in QUEUES.DAT... add job anyway ? [Y] ')
         yes = .true.
         call yesno (yes, err)
         if (err .or. (.not. yes)) then
            err = .false.
            go to 40
         endif
      endif
c
60    return
900   format(a)
910   format(' IF [NOT] (variable [op variable]) ')
920   format(' AT hh:mm [-hh:mm [ALWAYS]]')
930   format(' ON DAY(n) | WEEKDAY(n) | LASTDAY(n) | LASTWEEKDAY(n) |'/
     $ '    MONDAY..SUNDAY | EVERYWEEKDAY     [NOHOLIDAY | PUSHHOLIDAY]')
935   format(' FOR user')
940   format(' file_spec  log_file_spec  batch_queue  p1-p9',/,
     $    '                       or',/,
     $    ' file_spec /LOG= /QUEUE= /PRIOR= /CPU= /CHARAC= /PARAM=()')
      end
c
c---end add_kernel
c
      subroutine submit(i)
c*
c*  Special KRON version of submit used since DO_JOBS does most of the
c*   work.  It is used to list runnable jobs.
c*
      call clear
      call list_kernel (i)
c
c ----- pause
c
      write(6,920)
      read(5,930,end=100)
      return
c
100   call exit      ! It should return to the menu...
c
920   format(/,' --- Press <RETURN> to continue ---')
930   format()
      end
c
c---end submit
c
      subroutine kgetline 
c
c  Get a full line of input... ignore "!" (not in text string) comments
c  and check "-" last character continuation
c
c  Modified version of getline for KRON only
c
      include 'kparse.cmn'
      character *132 card
c
5     ll = 1
      line = ' '
10    read (iunit, 900, end=100 ) card
      call strip_exc(card)
100   lc = left(card)
      if (lc .eq. 0) return
      line(ll:) = card(1:lc)
      ll = ll + lc
      if (ll .gt. max_line) call error (4,
     $            'Line too long to parse.', 0) 
c
c --- if the line ends in a dash (-), its continued on next line
c
      if (line(ll-1:ll-1) .eq. '-') then
         line(ll-1:ll-1) = ' '
         go to 10
      endif
      line(ll:) = ' '
      return
c
900   format(a)
      end
c
c---end kgetline
c
      SUBROUTINE Show_sched 
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **        Show_sched         **
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
C*          Moffett Field, Ca.  94035-1000
C*          (650) 604-5558
C*
C*     DESCRIPTION :
C*          Show the schedule of jobs to be run
C*
C*     COMMON BLOCKS :
C*
C*     SUBPROGRAM REFERENCES :
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          30-MAR-1995  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *80 pline
      character *4 on, off, resp
c
      on = char(27) // '[7m'
      off= char(27) // '[0m'
      pline = 'Show ' //
     $         on // 'W' // off // 'eek or ' //
     $         on // 'D' // off // 'ay ?  [ D ] '
      lp = length(pline) + 1
      call prompt (pline(1:LP))
      read(5,900) resp
      call left(resp)
      call caps(resp)
c
      if (resp(1:1) .eq. 'W') then     ! Week
         call show_week
      else                             ! Day
         call show_day
      endif
c
      write(6,920) char(27), char(27)
      read(5,910,end=100)
100   return
c
900   format(a)
910   format()
920   format(//,' ',a1,
     $ '[1m --- Press <RETURN> to continue. ---',a1,'[0m')
      END
C
C---END Show_sched
C
      subroutine show_day
      include 'kronos.cmn'
      character *128 temp
      character *2 old_min
      logical on_code, at_code, first
c
      call clear
      write(6,900) now(1:11)
      old_min = now(16:17)
      itemp = hour
c
c --- loop over each hour in the day
c
      do 10 i = 0, 23
         first = .true.
c
c  ----  we have to fool at_code into doing the work by 
c           temporarily setting the hour to the loop variable
c
         hour = i
         now(16:17) = '00'
         min_tmp = 0
c
c  ----  brute force, check each job!
c
2        do 5 j = 1, num_jobs
c
c   -----   letting on_code do the work is OK, since we're checking 
c              today
c
            if (on_code(j)) then
               if (at_code(j)) then    ! not right... fractions
                  if (first) then
                     write(6,910) i, i+1
                     first = .false.
                  endif
                  call strcpy (temp, entry(j).fptr)
                  write (6,920) temp(1:length(temp))
               endif
            endif
5        continue
%ifdef INTERVAL
         min_tmp = min_tmp + %INTERVAL
         if (min_tmp .lt. 60) then
            write(now(16:17),930) min_tmp
            go to 2
         endif
%endif
10    continue
c
c --- restore "hour" and minutes incase used elsewhere in KRON
c
      hour = itemp
      now(16:17) = old_min
      return
900   format(/, ' Kronos job schedule for ',a,//)
910   format('   Jobs scheduled between ',i2.2, ':01 and ',
     $ i2.2,':00')
920   format('      ', a)
930   format(i2.2)
      end
c
c---end show_day
c
      subroutine show_week
c*
c*  Fudge the date to show today is Monday, then loop over week
c*   using show_day to do the work.
c*
c
c --- take into account beginning/end of month problem
c
c
c --- get our offset from Monday
c
c 
c --- subtract difference
c
c
c --- loop for each day
c
c
c --- restore date
c
      return
      end
c
c---end show_week
c
