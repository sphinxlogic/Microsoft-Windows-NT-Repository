      subroutine do_jobs
c*
c*  We have just awakened.  Run through the list of jobs and see if
c*  any of them are to be run right now.  If so, fire them off.
c*
      include 'kronos.cmn'
      logical on_code, at_code, if_code
c
      DO 100 I = 1, num_jobs
         if (.not. on_code(i)) go to 100      ! Is it to be done today?
         if (.not. at_code(i)) go to 100      ! Is it to be done this hour?
         if (.not. if_code(i)) go to 100      ! IF condition met?
         call submit(i)                       ! All conditions met, do it !
100   CONTINUE
      return
      end
c
c---end do_jobs
c
      logical function on_code(i)
c*
c*  Determine if the ON condition is met.  If ON was not specified, or
c*  specified as EVERYDAY, the condition is always met.  Return TRUE if
c*  the condition was met, FALSE otherwise.
c*
      include 'kronos.cmn'
      common / pushing / pushit
      logical pushit
c
      pushit = .false.
c
c --- 0 = everyday
c
      if (entry(i).oncode .eq. 0) then
         on_code = .true.
c
c --- 8 = every week day
c
      else if (entry(i).oncode .eq. 8) then
         on_code = (iday .le. 5)
c
c --- 1-7 = Monday-Sunday
c
      else if (entry(i).oncode .lt. 100) then
         on_code = (entry(i).oncode .eq. iday)
c
c --- onetime only jobs
c
      else if (entry(i).oncode .gt. 10000) then
         kday = entry(i).oncode - 10000
         kmonth = kday /100
         kday = kday - (100*kmonth)
         on_code = ((kday .eq. day) .and. (kmonth .eq. month))
c
c --- day of month types...
c
      else
         j = entry(i).oncode/100
         go to (100,200,300,400), j
c
c --- 100+day = day of month
c
100         on_code = ((entry(i).oncode-100) .eq. day)
            go to 1000
c
c --- 200+day = weekday of month
c
200         if (iday .ge. 6) then    ! Weekend
               on_code = .false.
               go to 1000
            endif
            nweek = day / 7
            nday = day - 7*nweek
            j = day - 2*nweek                ! correct for full week weekends
            if (nday .gt. iday) j = j - 1    ! correct for this weeks weekend
            if (nday .gt. iday+1) j = j - 1
            on_code = ((entry(i).oncode-200) .eq. j)
            go to 1000
c
c --- 300+day = day from end of month (301=last day)
c
300         il = last_day (now(4:6))      ! Length of this month
            if (il .eq. 28) then          ! If FEB, check for leap year
               read(now(10:11), 900) iyear
               itemp = iyear/4
               if (4*itemp .eq. iyear) il = il + 1
            endif
c
            il = il + 301 - entry(i).oncode
            on_code = (day .eq. il)
            go to 1000
c
c --- 400+day = weekday from end of month (401=last weekday)
c
400         if (iday .ge. 6) then
               on_code = .false.
               return
            endif
            il = last_day (now(4:6))      ! Length of this month
            if (il .eq. 28) then          ! If FEB, check for leap year
               read(now(10:11), 900) iyear
               itemp = iyear/4
               if (4*itemp .eq. iyear) il = il + 1
            endif
c
            j = il - day + 1
            nweek = j / 7
            nday = j - 7*nweek
            j = j - 2*nweek
            if ((nday+iday) .gt. 6) j = j - 1    ! Remove Saturday
            if ((nday+iday) .gt. 7) j = j - 1    ! Remove Sunday
            on_code = (j .eq. (entry(i).oncode-400))
c        end computed goto
      endif
c
c --- on code calculated... check holiday flag
c
1000  if (on_code) then
         if (entry(i).nohol) then
            mon = m2i ( now(4:6) )
            read(now,900) mday
            on_code = .not. btest(cbits(mon),(mday-1)) 
         else if (entry(i).pushhol) then
            mon = m2i ( now(4:6) )
            read(now,900) mday
            pushit = btest(cbits(mon),(mday-1))
         endif
      endif
c
      return
900   format(i2)
      end
c
      logical function at_code(i)
c*
c*  Determine if the AT condition was met.  If AT was specified as
c*   ALWAYS or not specified, the condition is always true.  Otherwise,
c*   check to see if its this time period.  
c*
      include 'kronos.cmn'
c
%ifdef interval
      if (entry(i).atcode .eq. -1) then
         at_code = .true.
      else if (entry(i).atcode .eq. -2) then
         read(now(16:17),900) imin
         at_code = imin .lt. %INTERVAL 
      else
c
c  for variable wakeup, we use minute of the day instead of hour
c
900   format(i2)
         read(now(16:17),900) imin
         min = imin + 60*hour
         if (entry(i).atcode .gt. 10000) then      ! range of times
            k = entry(i).atcode / 10000
            j = entry(i).atcode - 10000*k
            if (k .gt. j) then
               at_code = (((j .ge. min) .and. (j .lt. (min+%INTERVAL)))
     $                                   .or.
     $                    ((min .ge. j) .and. (min .le. k)))
            else
               at_code = (((min+%INTERVAL ) .ge. k) .or. (min .le. j)) 
            endif
            if (entry(i).hourly) then
               at_code = at_code .and. (imin .lt. %INTERVAL)
            endif
         else
            at_code = ((entry(i).atcode .ge. min) .and.
     $                 (entry(i).atcode .lt. (min+%INTERVAL)))
         endif
c
%else
      if (entry(i).atcode .lt. 0) then
         at_code = .true.
      else
c
c   just use the hour
c
         if (entry(i).atcode .gt. 10000) then
            k = entry(i).atcode / 10000
            j = entry(i).atcode - 10000*k
            if (k .gt. j) then
               at_code = ((hour .ge. j) .and. (hour .le. k))
            else
               at_code = ((hour .ge. k) .or. (hour .le. j)) 
            endif
         else
            at_code = (entry(i).atcode .eq. hour)
         endif
c
%endif
c
      endif
      return
      end
c
c---end at_code
c
      logical function if_code(i)
c*
c*  Determine if IF condition was met.  If no IF clause was specified,
c*   the condition is always met.
c*
      include 'kronos.cmn'
      integer do_opcode
      logical string_in_1, string_in_2
c
c --- no IF clause
c
      if (entry(i).ifcode .eq. 0) then
         ival = 1
c
c --- IF clause specified...
c ---  evaluate variables, always at least one
c ---  IEVAL does the real work
c
      else
         string_in_1 = .false.
         string_in_2 = .false.
         ival1 = ieval (entry(i).variable(1).type,
     $                  entry(i).variable(1).param1,
     $                  entry(i).variable(1).param2,
     $                  entry(i).variable(1).value,
     $                  string_in_1)
         ival = ival1     ! in case we used the degenerate IF 
c
c ----- NOT value
c
         if (entry(i).opcode .eq. 13) then
            if (ival .ne. 0) then
               ival = 0
            else
               ival = 1
            endif
c
c ----- if there was a logical operator, evaluate the second variable
c -----  and perform the logical function
c
         else if (entry(i).opcode .ne. 0) then
            ival2 = ieval (entry(i).variable(2).type,
     $                     entry(i).variable(2).param1,
     $                     entry(i).variable(2).param2,
     $                     entry(i).variable(2).value,
     $                     string_in_2)
c
            ival = do_opcode (entry(i).opcode, ival1, string_in_1,
     $                                         ival2, string_in_2)
c
c ------- delete temporary string used by variable 2
c
            if ((string_in_2) .and.
     $          (entry(i).variable(2).type .ne.2)) call free (ival2)
         endif
c
c ----- delete temporary string used by variable 1
c -----  note that ival1 is not set to zero by the FREE
c
         if ((string_in_1) .and.
     $       (entry(i).variable(1).type .ne. 2)) call free (ival1)
      endif
c
c --- DO_OPCODE returns non-zero for true.  If there is no opcode, then
c ---  this has the effect of "integer<>0" or "string<>''" for
c ---  variable 1.
c
      if_code = (ival .ne. 0)
      return
      end
c
c---end if_code
c
      function ieval (itype, ip1, ip2, ival, isstring)
c*
c*   Evaluate constant, logical name, or function
c*
      character *80 lname, pname
      character type*1, form*10
      logical isstring
c
      go to (10,20,30,40,50,60,70,80), itype
c
c --- literal number
c
10    ieval = ival
      isstring = .false.
      return
c
c
c --- literal string.  
c
20    ieval = ival
      isstring = .true.
      return
c
c --- System function.
c
30    ieval = isystem (ip1, isstring)
      return
c
c --- Process function
c
40    ieval = iprocess (ip1, ip2, isstring)
      return
c
c --- Print function
c
50    ieval = iprint (ip1, isstring)
      return
c
c --- Security function
c
60    ieval = isecurity (ip1, isstring)
      return
c
c --- Device function
c
70    ieval = idevice (ip1, ip2, isstring)
      return
c
c --- Logical name translation.  If the name translates to an integer
c ---  string, replace the string with the integer.
c
80    call strcpy (lname, ival)
      call transl8 (lname, pname)
c
c --- No translation (should we produce an error here?)
c
      if ((pname .eq. ' ') .or. (pname .eq. lname)) then
         ieval = 0
         return
      endif
c
      call categ (pname, type, form)
c
c --- if it looks like an integer
c
      if (type .eq. 'I') then
         call right (pname)
         read(pname,900) ieval
         isstring = .false.
c
c --- otherwise store in temp heap
c
      else
         ieval = isave (pname, .true.)
         isstring = .true.
      endif
      return
900   format(70x,i10)
      end
c
c---end ieval
c
      integer function do_opcode (iop, i1, s1, i2, s2)
c*
c*   Perform logical operation on the values returned from the
c*    evaluation of two constants, logical names, or functions
c*
c*   Note: string equality/inequality used to be done just by comparing
c*    the pointers.  This doesn't work anymore because the comparison
c*    could be with a temp variable.
c*
      character *80 c1, c2
      logical s1, s2
c
      do_opcode = 0
c
c --- There are no opcodes which allow mixing integer and string
c ---  variable types
c
      if (((.not. s1)  .and.  s2)  .or.
     $    (s1  .and.  (.not. s2))) then
            call error (2,
     $       'Operations with mixed strings and integers not allowed.',
     $       0)
            return
      endif
c
      go to (10,20,30,40,50,60,70,80), iop
c
c ---    <
c
10    if (s1) then
         call strcpy (c1, i1)
         call strcpy (c2, i2)
         if (c1 .lt. c2) do_opcode = 1
      else
         if (i1 .lt. i2) do_opcode = 1
      endif
      return
c
c ---    >
c
20    if (s1) then
         call strcpy (c1, i1)
         call strcpy (c2, i2)
         if (c1 .gt. c2) do_opcode = 1
      else
         if (i1 .gt. i2) do_opcode = 1
      endif
      return
c
c ---    ==
c
30    if (s1) then
         call strcpy (c1, i1)
         call strcpy (c2, i2)
         if (c1 .eq. c2) do_opcode = 1
      else
         if (i1 .eq. i2) do_opcode = 1
      endif
      return
c
c ---    <=
c
40    if (s1) then
         call strcpy (c1, i1)
         call strcpy (c2, i2)
         if (c1 .le. c2) do_opcode = 1
      else
         if (i1 .le. i2) do_opcode = 1
      endif
      return
c
c ---    >=
c
50    if (s1) then
         call strcpy (c1, i1)
         call strcpy (c2, i2)
         if (c1 .ge. c2) do_opcode = 1
      else
         if (i1 .ge. i2) do_opcode = 1
      endif
      return
c
c ---    <>
c
60    if (s1) then
         call strcpy (c1, i1)
         call strcpy (c2, i2)
         if (c1 .ne. c2) do_opcode = 1
      else
         if (i1 .ne. i2) do_opcode = 1
      endif
      return
c
c --- IN
c
70    if (.not. s1) then
         call error (2, 'IN requires string data.',0)
         return
      endif
      call strcpy (c1, i1)
      call strcpy (c2, i2)
      do_opcode = index(c2,c1(1:length(c1)))
      return
c
c --- NOT IN
c
80    if (.not. s1) then
         call error (2, 'NOT IN requires string data.',0)
         return
      endif
      call strcpy (c1, i1)
      call strcpy (c2, i2)
      if (index(c2,c1(1:length(c1))) .eq. 0) do_opcode = 1
      return
      end
c
c---end do_opcode
c
      subroutine get_delta (delta)
c*
c*   Return a date in delta format for the PUSHHOLIDAY qualifier
c*   i.e., when is the next non-holiday?
c*
      include 'kronos.cmn'
      integer delta(2)
      character *23 adelta
c
      adelta = '0001 00:00:00.00'
      if (iday .eq. 5) adelta(4:4) = '3'
      if (iday .eq. 6) adelta(4:4) = '2'
      call sys$bintim (adelta,delta)
      return
      end
c
c---end get_delta
c
