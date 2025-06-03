      PROGRAM CHECK_INSTALL
C
C  READ THE LIST OF INSTALLED FILES
C
      PARAMETER (MAX=1500)
      CHARACTER *255 FNAME, DNAME, LINE
      CHARACTER *255 OLD(MAX), NEW(MAX)
      character *100 name
      LOGICAL PRIVED, DIFFS
C
C
C --- GET LIST OF FILES AND PRIVILEGES FROM LAST WEEK
C
      NOLD = 0
      NNEW = 0
      OPEN (UNIT=80, FILE='CHECK_INSTALL.STATUS', STATUS='NEW',
     $   CARRIAGECONTROL='LIST', ERR=100)
      OPEN (UNIT=79, FILE='CHECK_INSTALL.DAT', STATUS='OLD', ERR=5)
2     READ (79,900,END=3) OLD(NOLD+1) 
      NOLD = NOLD + 1
      IF (NOLD .LT. MAX) GO TO 2
      WRITE (80,900) 'FATAL ERROR, Maximum number of files exceeded.'
      GO TO 200
C
3     CLOSE(79)
C
5     OPEN (UNIT=77, FILE='CHECK_INSTALL.TEMP', STATUS='OLD',
     $  ERR=100)
      OPEN (UNIT=78, FILE='CHECK_INSTALL.DAT', STATUS='NEW',
     $  CARRIAGECONTROL='LIST', ERR=100)
      DIFFS = .FALSE.
C
7     READ (77,900,END=9) LINE
      IF (LINE .EQ. ' ') GO TO 7
      IF (line(1:3) .ne. '   ') THEN            ! NEW DEVICE/DIRECTORY
         DNAME = LINE
         LD = LENGTH(DNAME)
      ELSE
         IF (LINE(4:4) .NE. ' ') THEN
            IB = INDEX(LINE(4:),' ')
            CALL PARSE (LINE(4:4+IB-2), DNAME(1:LD), 'FULL', FNAME)
            IF (IB .GE. 18) READ (77,900,END=9) LINE
            PRIVED = LINE(35:35) .EQ. 'P'
            NNEW = NNEW + 1
            IF (NNEW .GT. MAX) THEN
               WRITE(80,900) 'Maximum number of files exceeded.'
               GO TO 200
            ENDIF
            IF (PRIVED) THEN
               WRITE(78,900) 'P ' // FNAME(1:LENGTH(FNAME))
               NEW(NNEW) = 'P ' // FNAME
            ELSE
               WRITE(78,900) '  ' // FNAME(1:LENGTH(FNAME))
               NEW(NNEW) = '  ' // FNAME
            ENDIF
         ENDIF
      ENDIF
      GO TO 7
C
C --- OK, COMPARE THE TWO LISTS
C
9     close(77)
      ithis = 1        ! Current line in this week's list
      ilast = 1        ! Current line in last week's list
c
c --- if done with this weeks note any more files last week, then exit
c
10    if (ithis .gt. nnew) then
         do 15 i = ilast, nold
            il = index(old(i),';') - 1
            write(80,900) 'Removed ' // old(i)(3:il)
            diffs = .true.
15          continue
         go to 90
      endif
c
c --- if none left from last week, remaining must be new... exit
c
      if (ilast .gt. nold) then
         do 20 i = ithis, nnew
            it = index(new(i),';') - 1
            write(80,900) 'New file ' // new(i)(3:it)
            diffs = .true.
20          continue
         go to 90
      endif
c
c --- compare the next line from this week with the next line from 
c      last week
c
c ----- if different, why?
c
      if (new(ithis) .ne. old(ilast)) then
         itx = index(new(ithis),';') - 1
         if (itx .le. 3) itx=61
         ilx = index(old(ilast),';') - 1
         if (ilx .le. 3) ilx=61
         if (new(ithis)(3:itx) .eq. old(ilast)(3:ilx)) then
c
c --------- Ah Ha !  The file name is the same... privs or version
c ---------          different.
c
            if ((new(ithis)(1:1) .eq. 'P')  .and.
     $          (old(ilast)(1:1) .ne. 'P')) then
               call name_only(new(ithis), name, ln)
               write(80,900) 'Privilege added to ' // name(1:ln)
            else if ((new(ithis)(1:1) .ne. 'P')  .and.
     $          (old(ilast)(1:1) .eq. 'P')) then
               call name_only(new(ithis), name, ln)
               write(80,900) 'Privilege deleted from ' // name(1:ln)
            else
               call name_only(new(ithis), name, ln)
               write(80,900) 'New version of ' // name(1:ln)
            endif
            ithis = ithis + 1
            ilast = ilast + 1
         else
c
c ----- The file name is not the same, check to see if the file was 
c -----  deleted or a new one was inserted in front of it
c
c ----- Brute force... search all the rest of last week's list to
c -----  see if the next file listed in this week's list is there
c
            do 30 i = ilast+1, nold
               ilx = index(old(i),';') - 1
               if (ilx .le. 3) ilx = 61
               if (old(i)(3:ilx) .eq. new(ithis)(3:itx)) then
c
c --------------- Oh Ho !  Found it later in the list... all the
c ---------------  intervening entries must be missing.  Say so.
c
                  do 35 j = ilast, i-1
                     il = index(old(j),';') - 1
                     if (il .le. 3) il = 61
                     write(80,900)'Deleted ' // old(j)(3:il)
35                   continue
                  diffs = .true.
                  ilast = i
                  go to 10
               endif
30             continue
c
c ------- The name in this weeks list was not found nowhere in last 
c -------  weeks list, it must be a new file
c
            write(80,900)'New entry ' // new(ithis)(3:itx)
            ithis = ithis + 1
         endif
         diffs = .true.
      else
c
c ----- this entry same in both weeks lists, go to next line in each file
c
         ithis = ithis + 1
         ilast = ilast + 1
      endif
      go to 10
c
C
90    IF (DIFFS) THEN
         CLOSE(78)
         CLOSE(80)
         call exit(3)
      ENDIF
      CLOSE(78,DISPOSE='DELETE')
      CLOSE(80,DISPOSE='DELETE')
      call exit
c
100   WRITE(6,900) 'Error opening files.'
      CALL EXIT
C
200   CLOSE(80)
      call exit(3)
900   FORMAT(A)
      END
c
      subroutine name_only (big, name, ln)
      character *(*) big, name
c
      ib = index(big,'>')
      if (ib .le. 0) ib = 1
      ie = index(big,';')
      if ((ie .gt. length(big)) .or.
     $    (ie .le. (ib+1))) ie = length(big)
      name = big(ib+1:ie-1)
      ln = ie - (ib+1)
      return
      end
