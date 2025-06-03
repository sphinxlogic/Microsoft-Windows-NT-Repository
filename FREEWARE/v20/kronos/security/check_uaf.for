      program check_uaf
c*
c*  The purpose of this program is to check changes made to the system
c*   authorization file since last month.  At present, the program 
c*   checks for new users, deleted users, changes to privileges, and
c*   changes to flags.
c*
      parameter (num=1000)
      include '($UAIDEF)'
      character *1500 s
      character *255 fname
      character *80 exempt
      character *12 suser(num),   user(num)
      character *8  sprivs(num),  privs(num),  priv
      integer *4    sflags(num),  flags(num),  flag
      integer *2    snfails(num), nfails(num), nfail
      integer       lasti(2,num), tlasti(2)
      integer       lastn(2,num), tlastn(2)
      logical dif
c
c     List of accounts that are allowed to be idle
c
      data exempt /'+DECNET+OBSERVER+PCFS$ACCOUNT+PCSA$RMI+'/  
c
      structure / itmlst /
         integer *2 buflen, itmcod
         integer    bufadr, retadr
      end structure
      record / itmlst / list(6)
c
c --- build item list for GETUAI
c
      list(1).buflen = 4
      list(1).itmcod = uai$_flags
      list(1).bufadr = %loc(flag)
      list(1).retadr = 0
      list(2).buflen = 8
      list(2).itmcod = uai$_priv
      list(2).bufadr = %loc(priv)
      list(2).retadr = 0
      list(3).buflen = 2
      list(3).itmcod = uai$_logfails
      list(3).bufadr = %loc(nfail)
      list(3).retadr = 0
      list(4).buflen = 8
      list(4).itmcod = uai$_lastlogin_i
      list(4).bufadr = %loc(tlasti)
      list(4).retadr = 0
      list(5).buflen = 8
      list(5).itmcod = uai$_lastlogin_n
      list(5).bufadr = %loc(tlastn)
      list(5).retadr = 0
      list(6).buflen = 0
      list(6).itmcod = 0
c
      call lib$day (n_since_1858)
c
c --- last month's list (or earlier)
c
      OPEN (UNIT=2, FILE='CHECK_UAF.DAT', STATUS='OLD', ERR=100)
      nlast = 0
 5    nlast = nlast + 1
      read(2,910,err=5,end=10) user(nlast), privs(nlast),
     $                         flags(nlast), nfails(nlast)
      if (nlast .eq. num) then
         write(6,*) 'Maximum number of users exceeded.'
         nlast = nlast + 1
      else
         go to 5
      endif
10    nlast = nlast - 1
      close (unit=2)
c
c --- temp file in case there are changes
c
      OPEN (UNIT=3, FILE='CHANGES.DAT', STATUS='NEW', 
     $ CARRIAGECONTROL='LIST', ERR=100)
c
c --- CHECK_UAF.DAT stores the list of users, privs, flags, and failures
c --- new version... will be deleted if the same as last months
c
      OPEN (UNIT=4, FILE='CHECK_UAF.DAT', STATUS='NEW', 
     $ CARRIAGECONTROL='LIST', ERR=100)
C
      write(3,*)' '
      write(3,*)
     $ 'There have been changes to the authorize file as follows : '
      write(3,*)' '
      dif = .false.
c
c ---         get list of authorized users
c --- this is necessary because GETUAI doesn't support wildcards
c
      CALL PARSE ( 'SYSUAF', 'SYS$SYSTEM:SYSUAF.DAT', 'FULL', FNAME )
      OPEN (UNIT=1, FILE=FNAME, STATUS='OLD', READONLY, SHARED, ERR=100)
      nnow = 0
15    read(1,900,err=200,end=30) s
      nnow = nnow + 1
      if (nnow .gt. num) then
         write(6,*) 'Maximum number of users exceeded.'
      else
         suser(nnow) = s(5:16)                  ! Username
         call left (suser(nnow))
         go to 15
      endif
30    close(1)
c
c --- use GETUAI to get the rest of the info
c
      do 32 k = 1, nnow
         call sys$getuai (,, suser(k)(1:length(suser(k))), list,,,)
         sprivs(k) = priv
         sflags(k) = flag
         snfails(k) = nfail
         lasti(1,k) = tlasti(1)
         lasti(2,k) = tlasti(2)
         lastn(1,k) = tlastn(1)
         lastn(2,k) = tlastn(2)
         write(4,910) suser(k), priv, flag, nfail
32    continue
c
c
c --- OK, got all the data for this time and last...
c ---  compare two sets of arrays for differences
c
c
      iu = 1
      is = 1
35    if (user(iu) .eq. suser(is)) then
c
c ----- user was here last time; check privileges, flags, and login
c        failures for changes.
c
c
c  -----  check if the user hasn't logged in in a lonnnnnnnnng time...
c
         call lib$day (n_login_i, lasti(1,is))
         call lib$day (n_login_n, lastn(1,is))
         if (((n_since_1858 - n_login_i) .gt. 120) .and.
     $       ((n_since_1858 - n_login_n) .gt. 120) .and.
     $        (0 .eq. (sflags(is) .and. 2**uai$v_disacnt))) then
            if (index(exempt,'+' // suser(is)(1:length(suser(is))) //
     $         '+') .eq. 0) then 
               write(3,*) 'User '// suser(is)(1:length(suser(is))) //
     $                 ' should be disusered.'
               dif = .true.
            endif
         endif
c
         if (privs(iu) .ne. sprivs(is)) then
            write(3,*)'User ',user(iu),' has had privileges changed.'
            dif = .true.
         endif
c
         if (flags(iu) .ne. sflags(is)) then
            write(3,*)'User ',user(iu),' has had flags changed.'
            dif = .true.
         endif
c
c  -----  report a problem if the number of failures is not zero and...
c          the number has changed since last week or
c          the user has logged on within the week
c
         if ((snfails(is) .ne. 0) .and.
     $      ((nfails(iu) .ne. snfails(is)) .or.
     $      ((n_since_1858-n_login_i) .lt. 7))) then
c
            write(3,*) 'User ',suser(is)(1:length(suser(is))),' has ',
     $                  snfails(is),' login failures.'
            dif = .true.
         endif
      else
c
c ----- a user has been added or deleted
c
         if (user(iu) .lt. suser(is)) then
            write(3,*)'User ', user(iu),' has been deleted.'
            dif = .true.
            is = is - 1
         else
            write(3,*)'New user ',suser(is)
            dif = .true.
            iu = iu - 1
         endif
      endif
      iu = iu + 1
      is = is + 1
      if ((iu .le. nlast) .and. (is .le. nnow)) go to 35
c
c --- if one array was longer than the other, there may still be new or
c ---  deleted entries
c
      do 40 l = iu, nlast
         write(3,*) 'User ',user(l),' has been deleted.'
         dif = .true.
40       continue
      do 50 l = is, nnow
         write(3,*) 'New user ',suser(l)
         dif = .true.
50       continue
c
c --- anything to report?
c
      if (dif) then
         close(unit=3)
         close(unit=4)
      else
         close(unit=3,dispose='DELETE')
         close(unit=4,dispose='DELETE')
      endif
      call exit
c
100   write(6,*) 'Unable to open files.'
      call exit(3)
200   write(6,*) 'Error reading SYSUAF.'
      call exit(3)
c
900   format(a)
910   format(a12,2x,a8,2x,a4,2x,i4)
      end
