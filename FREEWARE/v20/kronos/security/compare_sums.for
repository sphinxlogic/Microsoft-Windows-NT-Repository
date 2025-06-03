      program compare
c*
c*  The purpose of this program is to compare two checksum files and
c*    report important differences (if any).
c*
      parameter (max=1500)
      character *120 this(max), last(max)
      logical diffs
c
      diffs = .false.
      OPEN (UNIT=2, FILE='CHECKSUM.DAT', STATUS='NEW',
     $   carriagecontrol='LIST', ERR=1100)
      write(2,*)
     $ 'Comparison of last week''s executables with this week''s'
      write(2,*) ' '
      write(2,*) ' '
c
c --- read this weeks checksums
c
      OPEN (UNIT=1, FILE='CHECKSUM.OUT', STATUS='OLD', ERR=100)
      nthis = 0
1     read(1,900,end=2) this(nthis+1)
      nthis = nthis + 1     ! Number of entries in this week's file
      if (nthis .gt. max) then
         write(2,*)
     $    'Maximum number of entries in this weeks'' file exceeded.'
         diffs = .true.
         go to 1000
      endif
      go to 1
c
c --- read last weeks checksums
c
2     close(1)
      nlast = 0
      OPEN (UNIT=1, FILE='CHECKSUM.OUT;-1', STATUS='OLD', ERR=100)
3     read(1,900,end=4) last(nlast+1)
      nlast = nlast + 1     ! Number of entries in last week's file
                            !  no need to check or would have failed
                            !  last week.
      go to 3
c
4     close(1)
      ithis = 1        ! Current line in this week's list
      ilast = 1        ! Current line in last week's list
c
c --- BIG LOOP... compare each line
c
c --- if done with this weeks note any more files last week, then exit
c
10    if (ithis .gt. nthis) then
         do 15 i = ilast, nlast
            il = index(last(i),';') - 1
            if (il .lt. 21) il = 80
            write(2,*) 'Deleted ' // last(i)(21:il)
            diffs = .true.
15          continue
         go to 1000
      endif
c
c --- if none left from last week, remaining must be new... exit
c
      if (ilast .gt. nlast) then
         do 20 i = ithis, nthis
            it = index(this(i),';') - 1
            if (it .lt. 14) it = 80
            write(2,*) 'New file ' // this(i)(14:length(this(i)))
            diffs = .true.
20          continue
         go to 1000
      endif
c
c --- compare the next line from this week with the next line from 
c      last week
c
c ----- if different, why?
c
      if (this(ithis) .ne. last(ilast)) then
         itx = index(this(ithis),';') - 1
         if (itx .le. 0) itx=120
         ilx = index(last(ilast),';') - 1
         if (ilx .le. 0) ilx=120
         if (this(ithis)(24:itx) .eq. last(ilast)(24:ilx)) then
c
c --------- Ah Ha !  The file name is the same... The checksum must be
c ---------          different.  Could be new version or patched.
c
            if (this(ithis)(itx:min0((itx+5),119)) .ne.
     $          last(ilast)(ilx:min0((ilx+5),119))) then
               write(2,*) 'New version of ' // this(ithis)(24:itx)
            else
               if ((this(ithis)(20:21) .eq. ' 0') .or.
     $             (last(ilast)(20:21) .eq. ' 0')) then
                  write(2,*) 'File was in use ' //this(ithis)(24:itx)
               else
                  write(2,*) 'Patch made to ' // this(ithis)(24:itx)
               endif
            endif
            diffs = .true.
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
            do 30 i = ilast+1, nlast
               ilx = index(last(i),';') - 1
               if (ilx .le. 0) ilx = 120
               if (last(i)(24:ilx) .eq. this(ithis)(24:itx)) then
c
c --------------- Oh Ho !  Found it later in the list... all the
c ---------------  intervening entries must be missing.  Say so.
c
                  do 35 j = ilast, i-1
                     il = index(last(j),';') - 1
                     if (il .le. 0) il = 120
                     write(2,*)'Deleted '//last(j)(24:il)
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
            write(2,*)'New file ' // this(ithis)(24:itx)
            diffs = .true.
            ithis = ithis + 1
         endif
      else
c
c ----- this entry same in both weeks lists, go to next line in each file
c
         ithis = ithis + 1
         ilast = ilast + 1
      endif
      go to 10
c
c --- in case of error, signal success so file with error message
c ---  in it will be mailed to system people
c
100   write (2,*) 'Unable to open input file.'
      diffs = .true.
c
c --- if differences were found, save the output file and signal
c ---  DCL to mail it to system people
c
1000  if (diffs) then
         close (unit=2)
         call exit (1)
      else
         close(unit=2,dispose='DELETE')
         call exit (3)
      endif
1100  stop 'Unable to open output file.'
900   format(a)
      end
