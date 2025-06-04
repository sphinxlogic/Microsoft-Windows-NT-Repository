      subroutine parse_file ( fname )
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **        Parse_File         **
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
C*        Read in the data file... parse it and generate the entry table
C*         Also read any calendar files.
C*
C*     INPUT ARGUMENTS :
C*          FNAME - The name of the file to parse
C*
C*     OUTPUT ARGUMENTS :
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
C*           6-OCT-1998  -  Corrected a parse bug in LOG file names
C*
C***********************************************************************
C*
      include 'kparse.cmn'
      include 'strings.cmn'
      include 'kronos.cmn'
      logical eof, err
      character *(*) fname
c
c First, check for a perpetual calendar file, then a yearly calendar 
c  file.
c
      num_jobs = 0
      nstring  = 1
      do 3 i = 1,12
         cbits(i) = 0
3        continue
      call parse_cal ( 'KRONOS_DIR:CALENDAR.DAT' )
      call parse_cal ( 'KRONOS_DIR:CALENDAR.' // now(8:11) )
c
c  Note: IUNIT can be changed in parse_line
c
      iunit = 1
      icount = 0
5     OPEN (UNIT=iunit, FILE=FNAME(1:LENGTH(FNAME)), STATUS='OLD',
     $  READONLY, ERR=1000)
c
10    call getline ( eof )
      if (.not. eof) then
         call parse_line (err)
         go to 10
      endif
      close (unit=iunit)
      return
c
c --- in the unlikely event that someone is writing to the file at 
c     exactly this time, try again
c
1000  icount = icount + 1
      if (icount .le. 5) then
         call sleep (2.0)
         go to 5
      endif
      call error (4, 'Can''t open input file.', 0)
      return
      end
c
c---end parse_file
c
      subroutine parse_cal ( filename )
c*
c*  Load calendar file info, if any
c*
      include 'kronos.cmn'
      include 'kparse.cmn'
      logical err, eof
      character *(*) filename
c
      iunit = 1
      OPEN (UNIT=iunit, FILE=filename(1:length(filename)), STATUS='OLD',
     $  READONLY, ERR=1000)
c
      call error (0,
     $ 'Loading calendar file, '//filename(1:length(filename)), 0)
c
c --- line format = 
c -----   mm/dd HOLIDAY
c -----   mm/dd-dd HOLIDAY
c -----   mm/dd command
c -----   mm/dd-dd command
c
10    call getline ( eof )
      if (.not. eof) then
         il = 1
         call token
         if (length(toke) .lt. 2) toke = ' ' // toke
         read(toke,900,err=100) mon       ! Line always starts with a month
         call token
         if (toke .eq. '/') call token
         if (length(toke) .lt. 2) toke = ' ' // toke
         read(toke,900,err=100) mday      ! Always at least a day
         il_save = il
         call token
         jday = mday
         if (toke .eq. '-') then          ! Possibly a span of days
            call token
            if (length(toke) .lt. 2) toke = ' ' // toke
            read(toke,900,err=100) jday
            call token
         endif
         if ((mday .le. 0) .or.           ! Make sure the day(s) are legal
     $       (jday .ge. 32) .or.
     $       (mday .gt. jday)) go to 100
c
c --- set HOLIDAY bits?
c
         if (toke .eq. 'HOLIDAY') then
            do 20 i = mday, jday
               cbits(mon) = ibset(cbits(mon), (i-1))
20          continue
c
c --- or record a command?
c
         else
c
c ------- ok, get rid of the date stuff since we already have it.
c -------  everything else should be parse_line compatible
c
            line = line(il_save:)
            call parse_line (err)
c
c ------- update the oncode to show that its a onetime only job
c
            if (.not. err) then
               entry(num_jobs).oncode = 10000 + 100*mon + mday
               mday = mday + 1
c
c ----------- may be a range, add an entry for each
c
               do 30 i = mday, jday
                  if (num_jobs .ge. max_jobs) then
                     call error (3,
     $               'Too many jobs in data base, increase MAX_JOBS.',0)
                     go to 500
                  endif
                  num_jobs = num_jobs + 1
                  entry(num_jobs) = entry(num_jobs-1)
                  entry(num_jobs).oncode = 10000 + 100*mon + i
30             continue
            endif
         endif
         go to 10
      endif
c
500   close (unit=iunit)
1000  return
100   call error (2, 'Illegal month or day in ' // 
     $            filename(1:length(filename)), 0)
      return
900   format(i2)
      end
c
c---end parse_cal
c
      subroutine parse_line (err)
c*
c*  Determine what type of command this is... pass to proper parsing
c*   subroutine.
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      character *132 fname
      logical err
c
      err = .false.
      il = 1
      call token
      if (toke .eq. '@') then          ! Indirect command
         fname = line(il:)
         iunit = iunit + 1
         OPEN (UNIT=iunit, FILE=fname, STATUS='OLD', ERR=1000)
         return
c
c --- read from new file.  getline will decrement iunit on EOF.
c
1000     iunit = iunit - 1
         call error (3, 'Unable to open include file ' //
     $    fname(1:length(fname)),0)
         err = .true.
      else
c
c --- We assume the parse will be valid... can correct later if any
c      errors occur
c
         if (num_jobs .ge. max_jobs) then
            call error (3,
     $         'Too many jobs in data base, increase MAX_JOBS.',0)
            return
         endif
c
c ----- the following section was modified Aug 15, 1995 to initialize
c        all fields
c
         num_jobs = num_jobs + 1
         entry(num_jobs).oncode  = 0
         entry(num_jobs).nohol   = .false.
         entry(num_jobs).pushhol = .false.
         entry(num_jobs).hourly  = .true.
         entry(num_jobs).atcode  = -2
         entry(num_jobs).ifcode  = 0
         entry(num_jobs).forptr  = 0
         entry(num_jobs).opcode  = 0
         entry(num_jobs).fptr    = 0
         entry(num_jobs).lptr    = 0
         entry(num_jobs).qptr    = 0
         entry(num_jobs).par(1)  = 0
         entry(num_jobs).character = 0
         entry(num_jobs).cputime = 0
         entry(num_jobs).priority = 0
         entry(num_jobs).notify  = .false.
c
c ------ in this version of Kronos, the phrases must be in the following
c         order.
c
         if (toke .eq. 'IF') call parse_if (err)
         if (toke .eq. 'AT') call parse_at (err)
         if ((toke .eq. 'ON') .or.
     $       (toke .eq. 'NOHOLIDAY') .or.
     $       (toke .eq. 'PUSHHOLIDAY')) call parse_on (err)
         if (toke .eq. 'FOR') call parse_for (err)
         if (toke .eq. 'THEN') call token
c
c ------ every entry must have a job phrase
c
         call parse_job (err)
c
         if (err) num_jobs = num_jobs - 1
      endif
      return
      end
c
c---end parse_line
c
      subroutine parse_if (err)
c*
c*   Parse the IF clause
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      logical err
      integer opcodes(2,9)   ! (1,i) = condition;  (2,i) = NOT condition
c
c     0 = (value <> 0)           7 = string inclusion
c     1 = less than              8 = string not included
c     2 = greater than           NYI 9 = (value and value) <> 0
c     3 = equal                  NYI 10 = (value and value) = 0
c     4 = less or equal          NYI 11 = (value or value) <> 0
c     5 = greater or equal       NYI 12 = (value or value) = 0
c     6 = not equal              13 = (value = 0)
c
      data opcodes / 1, 5,   2, 4,   3, 6,   4, 2,   5, 1,
     $   6, 3,  7, 8,  9, 10,  11, 12 /
c
c --- parse condition or NOT condition
c
      entry(num_jobs).ifcode = 1
      call token
      if (toke .eq. 'NOT') then
         not_flag = 2
         call token
      else
         not_flag = 1
      endif
c
c --- condition ::=  "(" variable [log_oper variable] ")"
c
      if (toke .eq. '(') call token
      call parse_var (1,err)
c
c --- check for logical operator
c
      entry(num_jobs).opcode = 0
      if (toke .eq. '<') then
         entry(num_jobs).opcode = opcodes(not_flag,1)
      else if (toke .eq. '>') then
         entry(num_jobs).opcode = opcodes(not_flag,2)
      else if (toke .eq. '=') then
         entry(num_jobs).opcode = opcodes(not_flag,3)
      else if (toke .eq. '<=') then
         entry(num_jobs).opcode = opcodes(not_flag,4)
      else if (toke .eq. '>=') then
         entry(num_jobs).opcode = opcodes(not_flag,5)
      else if (toke .eq. '<>') then
         entry(num_jobs).opcode = opcodes(not_flag,6)
      else if (toke .eq. 'IN') then
         entry(num_jobs).opcode = opcodes(not_flag,7)
      else if (toke .eq. '&') then
         entry(num_jobs).opcode = opcodes(not_flag,8)
      else if (toke .eq. '|') then
         entry(num_jobs).opcode = opcodes(not_flag,9)
c
c --- if no opcode was found, don't try to parse second variable
c
      else
         if (not_flag .eq. 2) entry(num_jobs).opcode = 13
         go to 10
      endif
c
c --- parse second variable
c
      call token
      call parse_var (2,err)
c
10    if (toke .eq. ')') call token
c
      return
      end
c
c---end parse_if
c
      subroutine parse_at (err)
c*
c*   Parse the AT clause
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      logical verify, err
c
c --- AT ::= ALWAYS | time (in 24 hour clock)
c
      call token
      if (toke .eq. 'ALWAYS') then
         call token
         entry(num_jobs).atcode = -1
      else
c
%ifdef interval
c
c --- Variable wakeup.  AT code is the minute of the day.
c
         if (verify (toke, '0123456789 ')) then
            call right (toke)
            read(toke,900,err=100) i
            call token
            if (toke .eq. ':') then
               call token
               if (verify (toke, '0123456789 ')) then
                  call right(toke)
                  read(toke,900,err=100) j
                  call token
               endif
            else
               j = 0
            endif
            entry(num_jobs).atcode = 60*i + j
            if (entry(num_jobs).atcode .eq. 1440) then
               entry(num_jobs).atcode = 1439
            else if ((entry(num_jobs).atcode .gt. 1439) .or.
     $          (entry(num_jobs).atcode .lt. 0)) then
               err = .true.
               call error (2,
     $          'Time must be between 00:00 and 23:59.',0)
            endif
c
c  -------  is it a range of times???
c
            if (toke .eq. '-') then
               call token
               if (verify (toke, '0123456789 ')) then
                  call right (toke)
                  read(toke,900,err=100) i
                  call token
                  if (toke .eq. ':') then
                     call token
                     if (verify (toke, '0123456789 ')) then
                        call right (toke)
                        read(toke,900,err=100) j
                        call token
                     endif
                  else
                     j = 0
                  endif
                  i = 60*i + j
                  if (i .eq. 1440) then
                     i = 1439
                  else if ((i .gt. 1439) .or. (i .lt. 0)) then
                     err = .true.
                     call error (2,
     $                'Time must be between 00:00 and 23:59.',0)
                  endif
                  entry(num_jobs).atcode =
     $               entry(num_jobs).atcode + 10000 * i
               endif
c
c  -------  AT hh:mm-hh:mm ALWAYS
c
               if (toke .eq. 'ALWAYS') then
                  entry(num_jobs).hourly = .false.
                  call token
               endif
            endif 
c
%else
c
c --- hourly.  AT code is the hour.
c
         if (verify (toke, '0123456789 ')) then
            call right (toke)
            read(toke,900,err=100) entry(num_jobs).atcode
c
c  -------  24:00 is not valid, it should be specified as 00:00 on the
c           following day.  Too complicated to fix here because of 
c  -------  potential ON problems, so just produce an error.
c
            if ((entry(num_jobs).atcode .gt. 23) .or.
     $          (entry(num_jobs).atcode .lt. 0)) then
               call error (2, 'Hour must be between 0 and 23.',0)
               err = .true.
            endif
            call token
            if (toke .eq. ':') then
               call token
               if (verify (toke, '0123456789 ')) call token
            endif
c
c  -------  is it a range of hours???
c
            if (toke .eq. '-') then
               call token
               if (verify (toke, '0123456789 ')) then
                  call right (toke)
                  read(toke,900,err=100) i
                  if ((i .gt. 23) .or. (i .lt. 0)) then
                     call error (2, 'Hour must be between 0 and 23.',0)
                     err = .true.
                  endif
                  call token
                  if (toke .eq. ':') then
                     call token
                     if (verify (toke, '0123456789 ')) call token
                  endif
                  entry(num_jobs).atcode =
     $                entry(num_jobs).atcode + 10000 * i
               endif
            endif
c
%endif
c
         else
            go to 100
         endif
      endif
      return
100   err = .true.
      call error (2, 'Time expected in AT clause.',0)
      return
900   format(78x,i2)
      end
c
c---end parse_at
c
      subroutine parse_on (err)
c*
c*   Parse the ON condition
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      logical err
c
      if ((toke .eq. 'NOHOLIDAY') .or.
     $    (toke .eq. 'PUSHHOLIDAY')) go to 90
      call token
c
c --- every day
c
      if (toke .eq. 'EVERYDAY') then
         call token
c
c --- every week day
c
      else if (toke .eq. 'EVERYWEEKDAY') then
         call token
         entry(num_jobs).oncode = 8
c
c --- number of days from start of month
c
      else if (toke .eq. 'DAY') then
         call token
         if (toke .eq. '(') call token
         call right ( toke )
         read (toke,900,err=100) i
         entry(num_jobs).oncode = 100+i
         call token
         if (toke .eq. ')') call token
c
c --- number of weekdays from start of month
c
      else if (toke .eq. 'WEEKDAY') then
         call token
         if (toke .eq. '(') call token
         call right ( toke )
         read (toke,900,err=100) i
         entry(num_jobs).oncode = 200+i
         call token
         if (toke .eq. ')') call token
c
c --- day of the week
c
      else if (toke .eq. 'MONDAY') then
         call token
         entry(num_jobs).oncode = 1
      else if (toke .eq. 'TUESDAY') then
         call token
         entry(num_jobs).oncode = 2
      else if (toke .eq. 'WEDNESDAY') then
         call token
         entry(num_jobs).oncode = 3
      else if (toke .eq. 'THURSDAY') then
         call token
         entry(num_jobs).oncode = 4
      else if (toke .eq. 'FRIDAY') then
         call token
         entry(num_jobs).oncode = 5
      else if (toke .eq. 'SATURDAY') then
         call token
         entry(num_jobs).oncode = 6
      else if (toke .eq. 'SUNDAY') then
         call token
         entry(num_jobs).oncode = 7
c
c --- number of days from end of month
c
      else if (toke .eq. 'LASTDAY') then
         call token
         if (toke .eq. '(') call token
         call right ( toke )
         read (toke,900,err=100) i
         entry(num_jobs).oncode = 300+i
         call token
         if (toke .eq. ')') call token
c
c --- number of weekdays from end of month
c
      else if (toke .eq. 'LASTWEEKDAY') then
         call token
         if (toke .eq. '(') call token
         call right ( toke )
         read (toke,900,err=100) i
         entry(num_jobs).oncode = 400+i
         call token
         if (toke .eq. ')') call token
c
      else
         err = .true.
         call Error (2, 'Day specifier expected in ON clause.',0)
         return
      endif
c
c --- check for NOHOLIDAY
c
90    if (toke .eq. 'NOHOLIDAY') then
         entry(num_jobs).nohol = .true.
         call token
      else if (toke .eq. 'PUSHHOLIDAY') then
         entry(num_jobs).pushhol = .true.
         call token
      endif
c
      return
100   call error (2, 'Number required.', 0)
      err = .true.
      return
900   format(77x,i3)
      end
c
c---end parse_on
c
      subroutine parse_for (err)
c*
c*   Parse the FOR clause
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      logical err, is_user
c
      err = .false.
c
c --- FOR UserName
c
      call token
c
c --- token should be a user name
c -----  1-12 alpha plus underscore are legal
c -----  make sure its in the Authorize file, too
c
      if (is_user(toke(1:length(toke)))) then
         entry(num_jobs).forptr = isave ( toke(1:length(toke)),.false.)
      else
         err = .true.
         call error (2, 'User ' // toke(1:length(toke)) //
     $         ' not known or not allowed batch access.', 0)
      endif
      call token
      return
      end
c
c---end parse_for
c
      subroutine parse_var (ione, err)
c*
c*  Parse a string constant, integer constant, logical name, or
c*   function
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      logical verify, err
c
c --- string constants must be enclosed in quotes
c
      if ((toke(1:1) .eq. '"') .or.
     $    (toke(1:1) .eq. '''')) then
         lt = length(toke)
         if ((toke(lt:lt) .eq. '"') .or.
     $       (toke(lt:lt) .eq. '''')) lt = lt - 1
         if (lt .lt. 2) then
            lt = 2
            toke(2:2) = ' '
         endif
         entry(num_jobs).variable(ione).value = isave ( toke(2:lt),
     $     .false. )
         entry(num_jobs).variable(ione).type = 2
         call token
c
c --- integer constant
c
      else if (verify (toke, '0123456789 ')) then
         call right (toke)
         read(toke,900) i
         entry(num_jobs).variable(ione).value = i
         entry(num_jobs).variable(ione).type = 1
         call token
c
c --- function evaluations
c
      else if (toke .eq. 'SYSTEM') then
         call parse_fun (ione,err,3)
      else if (toke .eq. 'PROCESS') then
         call parse_fun (ione,err,4)
      else if (toke .eq. 'PRINT') then
         call parse_fun (ione,err,5)
      else if (toke .eq. 'SECURITY') then
         call parse_fun (ione,err,6)
      else if (toke .eq. 'DEVICE') then
         call parse_fun (ione,err,7)
c
c --- unrecognized token... assume its a logical name
c
      else
         entry(num_jobs).variable(ione).type = 8
         entry(num_jobs).variable(ione).value =
     $         isave (toke(1:length(toke)), .false.)
         call token
      endif
      return
900   format(70x,i10)
      end
c
c---end parse_var
c
      subroutine parse_fun (ione,err,itype)
c*
c*   Parse function argument lists (function type is already parsed)
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      logical err
c
      entry(num_jobs).variable(ione).type = itype
      call token
      if (toke .eq. '(') call token
c
c  toke is now first param (always needed)
c
      entry(num_jobs).variable(ione).param1 =
     $  isave (toke(1:length(toke)), .false.)
      call token
c
c  TOKEN removes colon from device name and returns it seperately...
c   we don't need it, anyway.
c
      if (toke .eq. ':') call token
c
c  If there is a second parameter, it must be seperated by a comma
c
      if (toke .eq. ',') then
         call token
         entry(num_jobs).variable(ione).param2 =
     $     isave(toke(1:length(toke)),.false.)
         call token
      endif
      if (toke .eq. ')') call token
      return
      end
c
c---end parse_fun
c
      subroutine parse_job (err)
c*
c*  file_spec, log_file_spec, batch_queue, [p1 [,p2 [...]]]
c*
      include 'kparse.cmn'
      include 'kronos.cmn'
      character *127 file, ofile, lfile
      character *80 value, temp, def_spec
      parameter (nquals=8)
      character *9  quals(nquals), q
      data quals / 'CHARACTER', 'CPUTIME',   'LOGFILE',   'NOLOGFILE',
     $             'NOTIFY',    'PARAMETER', 'PRIORITY',  'QUEUE'/
      logical err, match, ambig, terr
      integer parse, sys$bintim
c
      entry(num_jobs).notify = .false.
c
c --- job file spec; first part is already in toke
c
      call getfile ( file, err )
      if (err) return
      if (file .eq. ' ') then
         err = .true.
         call error (2, 'No job entry specified.',0)
         return
      endif
c
c --- add directory and .COM (if omitted) and get full filespecification
c
      def_spec = def_dir(1:length(def_dir)) // '*.COM'
      istat = parse (file, def_spec(1:length(def_spec)), 'FULL', ofile)
      if (ofile .eq. ' ') then
         err = .true.
         call error (2, 'File name error.',istat)
         return
      endif
      io = index(ofile,';')
      if (io .ne. 0) ofile(io:) = ' '
c
c --- fix filespec in LINE in case we're in Kron
c
      if (file .ne. ofile) then
         io = length(ofile)
         line = ofile(1:io) // ' ' // line(il:)
         il = io + 2
         ll = length(line)
      endif
c
c --- OK, save it
c
      entry(num_jobs).fptr = isave ( ofile, .false. )
c
c === old style job was JOBNAME LOGFILE QUEUE PARAM1 PARAM2...
c === new style is      JOBNAME /LOG= /QUE= /PARAM=(..) /PRIO= /CHAR= 
c                               /CPU= /NOTIFY
c
      if (index(line(il:),'/') .eq. 0) then
c
c --- old style
c
c --- log file spec (if any)
c
         call token
         call getfile ( file, err )
         if (err) return
         if (file .eq. ' ') then
            i = 0
         else
c
c  -----  change the .COM file name to .LOG for defaults
c
            istat = parse ('.LOG', ofile, 'FULL', ofile)
c
            istat = parse (file, ofile, 'FULL', lfile)
            if (lfile .eq. ' ') then
               err = .true.
               call error (2, 'Log file name error.',istat)
               return
            endif
            i = isave ( lfile, .false. )
         endif
         entry(num_jobs).lptr = i
c
c --- batch queue (if any)
c
         call stoken
         if ((toke .eq. ' ') .or. (toke .eq. '""')) then
            i = 0
         else
            i = isave ( toke, .false. )
         endif
         entry(num_jobs).qptr = i
c
c --- parameters (if any)
c
         call stoken
         j = 0
10       if (toke .ne. ' ') then
            i = isave ( toke, .false. )
            j = j + 1
            entry(num_jobs).par(j) = i
            call stoken
            if (j .lt. 9) go to 10
         endif
         if (j .lt. 9) entry(num_jobs).par(j+1) = 0
      else
c
c --- new style
c
c --- look for LOG QUEUE PARAM PRIORITY CHARACTERISTIC CPUTIME NOTIFY
c --- get qualifier name
c
20       call getq ( q, value )
         if (q .ne. ' ') then
            call search ( quals, nquals, q, k, match, ambig )
            if ((.not. match) .or.  ambig) then
               call error (2,
     $  'Unknown or ambiguous qualifier '//q(1:length(q))//'.',0)
               err = .true.
               return
            endif
            go to (100,200,300,20,400,500,600,700), k
c
c  -----  Characteristic
c
100         call right (value)
            read (value,900,err=1000) entry(num_jobs).character
            go to 20
c
c  -----  CPUTime
c
200         call left (value)
            istat = sys$bintim ( value, entry(num_jobs).cputime )
            if (.not. istat) go to 1000
            go to 20
c
c  -----  Logfile
c
300         if (value .eq. ' ') then
               istat = parse ( '.LOG', ofile, 'FULL', lfile )
            else
               call left (value)
               istat = parse ( value, ofile, 'FULL', lfile )
            endif
            if (lfile .eq. ' ') then
               err = .true.
               call error (2, 'Log file specification error.',0)
               return
            endif
            i = isave ( lfile, .false. )
            entry(num_jobs).lptr = i
            go to 20
c
c  -----  Notify
c
400         entry(num_jobs).notify = .true.
            go to 20
c
c  -----  Parameters
c
500         if (value(1:1) .eq. '(') then
               value= value(2:)
               lv = length(value)
               if (value(lv:lv) .eq. ')') then
                  value(lv:lv) = ' '
                  lv = lv - 1
               endif
c
               ip = 1
               ii = 1
550            call getstoke (value, lv, ip, temp, terr)
               if (.not. terr) then
                  entry(num_jobs).par(ii) = isave(temp,.false.)
                  ii = ii + 1
                  if (ii .le. 8) go to 550
               endif
               entry(num_jobs).par(ii) = 0
            else
               entry(num_jobs).par(1) = isave(value,.false.)
               entry(num_jobs).par(2) = 0
            endif
            go to 20
c
c  -----  Priority
c
600         call right (value)
            read (value,900,err=1000) entry(num_jobs).priority
            go to 20
c
c  -----  Queue
c
700         entry(num_jobs).qptr = isave(value,.false.)
            go to 20
         endif
      endif
      return
1000  call error (2, 'Illegal value for qualifier.',0)
      err = .true.
      return
900   format(75x,i5)
      end
c
c---end parse_job
c
      subroutine getq ( q, value )
      character *(*) q, value
      include 'kparse.cmn'
c
      call stoken
      if (toke(1:1) .eq. '/') toke = toke(2:)
      i = index(toke,'=')
      if (i .eq. 0) then
         q = toke
         value = ' '
      else
         q = toke(1:i-1)
         value = toke(i+1:)
      endif
      return
      end
c
c---end getq
c
      subroutine getstoke (instring, len_in, next, outstring, err)
c*
c*   Get string tokens from a line.  This is quick and dirty solution to
c*       the new format parameter problem.
c*
c*   Formats:
c*     abcdef
c*     "abcd efg",abcd
c*     "abcdefg","hijk lmno",abc
c*     etc
c*
      character *(*) instring, outstring
      logical err
      character *1 look_for
c
      err = .false.
      if (next .gt. len_in) then
         err = .true.
         return
      endif
c
      if ((instring(next:next) .eq. '"') .or.
     $    (instring(next:next) .eq. '''')) then
         look_for = instring(next:next)
         next = next + 1
      else
         look_for = ','
      endif
c
      outstring = ' '
      iout = 1
10    if (next .le. len_in) then
         outstring(iout:iout) = instring(next:next)
         next = next + 1
         iout = iout + 1
         if (instring(next:next) .ne. look_for) go to 10
         next = next + 1
         if (instring(next:next) .eq. ',') next = next + 1
      endif
c
      
      return
      end
c
c---end getstoke
c
      logical function is_user ( user )
c*
c*    Verify that the user name is for a real user and that the user is 
c*    allowed to run jobs.
c*
      include '($UAIDEF)'
      include '($SSDEF)'
      include '($RMSDEF)'
      character *(*) user
      integer flags, sys$getuai
      integer *4 itmlst(4)
      integer *2 item(8)
      equivalence (item(1),itmlst(1))
c
      is_user = .false.
c
      item(1)   = 4
      item(2)   = UAI$_FLAGS
      itmlst(2) = %loc(flags)
      itmlst(3) = 0
      itmlst(4) = 0
c
      istat = sys$getuai (,,user,itmlst,,,)
c
c --- RMS$_RNF if there is no record of this user
c
      if ( .not. istat ) return
c
c --- OK, its a real user, but is the account disabled or captive?
c
      if ((flags .and. (2**UAI$V_AUTOLOGIN)) .ne. 0) return
      if ((flags .and. (2**UAI$V_CAPTIVE)) .ne. 0) return
      if ((flags .and. (2**UAI$V_DISACNT)) .ne. 0) return
      if ((flags .and. (2**UAI$V_PWD_EXPIRED)) .ne. 0) return
      if ((flags .and. (2**UAI$V_RESTRICTED)) .ne. 0) return
c
c --- AOK
c
      is_user = .true.
      return
      end
c
c---end is_user
c
      subroutine getline ( eof )
c
c  Get a full line of input... process "!" (not in text string) comments
c  and "-" last character continuation
c
      include 'kparse.cmn'
      character *132 card
      logical eof
c
5     eof = .false.
      ll = 1
      line = ' '
10    read (iunit, 900, end=100 ) card
      call strip_exc (card)
      call caps(card)
      lc = length(card)
      if (lc .eq. 0) go to 10
      line(ll:) = card(1:lc)
      ll = ll + lc
      if (ll .gt. max_line) then
         call error (3, 'Line too long to parse.', 0) 
         go to 5
      endif
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
c --- end of file... is it on the master file or an included file
c
100   if (iunit .eq. 1) then
         if (ll .eq. 1) eof = .true.
         line(ll:) = ' '
         return
      else
         iunit = iunit - 1
         if (ll .eq. 1) go to 5
      endif
900   format(a)
      end
c
c---end getline
c
      subroutine token
c
c  Return the next valid token from line, starting at location IL.
c  If not a known token, return single character
c
      include 'kparse.cmn'
      character *1 c, d
      logical verify
c
      toke = ' '
      it = 0
5     if (il .gt. ll) return
      c = line(il:il)
      il = il + 1
      if (c .eq. ' ') go to 5
      call capital (c)
c
c  Alphabetic
c
      if ((c .ge. 'A') .and. (c .le. 'Z')) then
10       it = it + 1
         if (it .gt. max_toke) call error (4,
     $            'Token too large to parse.', 0) 
         toke(it:it) = c
         c = line(il:il)
         il = il + 1
         call capital (c)
         if (index('ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$',c) .ne. 0)
     $         go to 10
c
c  Integer
c
      else if ((c .ge. '0') .and. (c .le. '9')) then
20       it = it + 1
         if (it .gt. max_toke) call error (4,
     $            'Token too large to parse.', 0) 
         toke(it:it) = c
         c = line(il:il)
         il = il + 1
         call capital (c)
         if (verify (c, '0123456789')) go to 20
c
c  Quoted string
c
      else if ((c .eq. '"') .or. (c .eq. '''')) then
         d = c
30       it = it + 1
         if (it .gt. max_toke) call error (4,
     $            'Token too large to parse.', 0) 
         toke(it:it) = c
         c = line(il:il)
         il = il + 1
         if (c .ne. d) go to 30
         it = it + 1
         if (it .gt. max_toke) call error (4,
     $            'Token too large to parse.', 0) 
         toke(it:it) = c
         il = il + 1
c
c  > and >=
c
      else if (c .eq. '>') then
         it = it + 1
         toke(it:it) = c
         c = line(il:il)
         il = il + 1
         if (c .eq. '=') then
            it = it + 1
            toke(it:it) = c
            il = il + 1
         endif
c
c  <, <=, and <>
c
      else if (c .eq. '<') then
         it = it + 1
         toke(it:it) = c
         c = line(il:il)
         il = il + 1
         if ((c .eq. '=') .or. (c .eq. '>')) then
            it = it + 1
            toke(it:it) = c
            il = il + 1
         endif
c
c  = and ==
c
      else if (c .eq. '=') then
         it = it + 1
         toke(it:it) = c
         c = line(il:il)
         il = il + 1
         if (c .eq. '=') il = il + 1
c
c  else, single character
c
      else
         it = it + 1
         toke(it:it) = c
         il = il + 1
      endif
      il = il - 1
      return
      end
c
c---end token
c
      subroutine stoken
c
c  Return the next space-delimited or quoted token from line.  
c  Due to new command style, slashes are treated as delimiters if
c  they are not quoted.
c
c  Embedded quotes are returned verbatim.
c
      include 'kparse.cmn'
      character *1 qc
c
      qc    = ' '
      toke  = ' '
      it    = 0
c
c --- skip leading spaces
c
5     if (il .gt. ll) return
      if (line(il:il) .eq. ' ') then
         il = il + 1
         go to 5
      endif
c
c --- copy string until delimiter is found (note: quote characters are
c ---  included in the output token.
c
10    it = it + 1
      if (it .gt. max_toke)
     $      call error (4, 'Token too large to parse.',0)
      toke(it:it) = line(il:il) 
c
c --- quoted string
c
      if ((line(il:il) .eq. '''') .or. (line(il:il) .eq. '"')) then
         qc = line(il:il)
20       il = il + 1
         if (il .gt. ll) then  !error, fix it up and return
            it = it + 1
            if (it .gt. max_toke)
     $         call error (4, 'Token too large to parse.',0)
            toke(it:it) = qc
            return   
         endif
         it = it + 1
         if (it .gt. max_toke)
     $       call error (4, 'Token too large to parse.',0)
         toke(it:it) = line(il:il) 
c
c ----- quote character again - either end of quote or embedded
c
         if (line(il:il) .eq. qc) then    
            if (il .ge. ll) return   ! end of line
            if (line(il+1:il+1) .eq. qc) then   ! embedded quote
               il = il + 1
               it = it + 1
               if (it .gt. max_toke)
     $            call error (4, 'Token too large to parse.',0)
               toke(it:it) = line(il:il) 
               go to 20
            endif
c
c ------- end of quoted string reached
c
c
c ------- now fall through
c                        
c
c ----- else copy it
c
         else
            go to 20
         endif
      endif
c
c --- not quoted
c
      il = il + 1
      if (il .le. ll) then
         if ((line(il:il) .eq. '/') .or.
     $       (line(il:il) .eq. ' ')) return
         go to 10
      endif
      return
      end
c
c---end stoken
c
      subroutine getfile ( file, err )
c*
c*  Extract a full filename from the input line, starting with the
c*    current token.
c*
      include 'kparse.cmn'
      character *(*) file
      logical err
c
      lnf = len(file)
      file = toke
      if (file(1:1) .eq. '"') then
         file = file(2:)
         lf = length(file)
         file(lf:lf) = ' '
      endif
      ifile = length(file) + 1
      if (ifile .eq. 1) return
c
c --- the file name is expected to end with a space, slash, or end of line
c
5     if ((line(il:il) .ne. ' ') .and.
     $    (line(il:il) .ne. '/') .and.
     $    (il .le. ll)) then
         if (ifile .gt. lnf) then
            err= .true.
            call error (2, 'Job file name too long.',0)
            return
         endif
         file(ifile:ifile) = line(il:il)
         il = il + 1
         ifile = ifile + 1
         go to 5
      endif
      if (file(ifile-1:ifile-1) .eq. '"') file(ifile-1:) = ' '
      return
      end
c
c---end getfile
c
