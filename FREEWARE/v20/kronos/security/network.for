      program security_summary
      common line, nfail, nnet, nn
      character *132 line
c
      OPEN (UNIT=7, FILE='NETWORK.OUT', STATUS='NEW',
     $   CARRIAGECONTROL='LIST', ERR=100)
      OPEN (UNIT=10, FILE='NETWORK.ACCT', STATUS='OLD', ERR=150)
      nfail = 0
      nnet = 0
      nn = 0
c
10    read(10,900,end=200) line
      if (line(1:13) .eq. 'LOGIN FAILURE') then
         call failure
      else if (line(1:7) .eq. 'NETWORK') then
         call net
      else
         write(7,910) line(1:40)
      endif
      call skip_to_ff
      go to 10
100   write(6,920)
      call exit
150   write(7,930)
      call exit
200   call endup
900   format(a)
910   format('Unknown process ',a)
920   format('Unable to open output file.')
930   format('Unable to open input file.')
      end
c
      subroutine skip_to_ff
      common line, nfail, nnet, nn
      character *132 line
c
10    read(10,900,end=100) line
      if (line(1:1) .ne. char(12)) go to 10
      return
100   call endup
900   format(a)
      end
c
c   LOGIN FAILURE  --  check to see if it's serious enough to report
c
      subroutine failure
      common line, nfail, nnet, nn
      character *132 line
      character *16 uname, tname*6, stars*6
c
      nfail = nfail + 1
      i = 0
10    read(10,900,end=100) line
      i = i + 1
      if ((i .le. 20) .and. (line(1:9) .ne. 'Username:')) go to 10
      if (i .gt. 20) then
         write(7,910)
         return
      endif
      uname = line(20:31)
      call left(uname)
      do 20 i = 1,4
20    read(10,900,end=100) line
      tname = line(19:24)        
      call left(tname)
c
c  Report all failures across the network
c
      if ((tname .eq. ' ') .or. (tname(1:1) .eq. 'R')) then
         read(10,900,end=100) line
         tname = line(19:24)
         if (tname .ne. ' ') then
            call right(tname)
            read(tname,940) inode    
            call left(tname)
c
c  If from outside of Ames, flag it to catch attention
c
            stars = ' '
            if ((inode .lt. 24576) .or. (inode .ge. 25600))
     $         stars = '******'
            write(7,930) uname(1:length(uname)), tname(1:length(tname)),
     $         stars
            nn = 1
         endif
c
c  Not a network failure, if the failure is attributed to <login>, there
c   is no real information in the failure anyway
c
      else
         if (uname .ne. '<login>') then
            write(7,920) uname(1:length(uname)), tname(1:length(tname))
            nn = 1
         endif
      endif
      return
100   call endup
900   format(a)
910   format('Problem searching for login failure.')
920   format('Login failure for user ',a,' on terminal ',a)
930   format('Login failure for user ',a,' on remote node ',a,'    ',a)
940   format(i6)
      end
c
      subroutine net
      common line, nfail, nnet, nn
      character *132 line
      character uname*16, rnode*6
c
      i = 0
10    read(10,900,end=100) line
      i = i + 1
      if ((i .le. 20) .and. (line(1:9) .ne. 'Username:')) go to 10
      if (i .gt. 20) then
         write(7,910)
         return
      endif
      uname = line(20:31)
      call left(uname)
      do 20 i = 1,5
20    read(10,900,end=100) line
      rnode = line(19:24)        
      read(rnode,940) inode
      call left(rnode)
      if (((inode .lt. 24577) .or. (inode .gt. 25599)) .and.
     $   ((inode .lt. 63488) .or. (inode .gt. 64511))) then 
         if (rnode .eq. ' ') then
c            write(7,925) uname(1:length(uname))
         else
            write(7,920) uname(1:length(uname)), rnode(1:length(rnode))
            nnet = nnet + 1
         endif
      endif
      return
100   call endup
900   format(a)
910   format('Problem searching for network access.')
920   format('Network process for user ',a,' on node ',a)
925   format('Network process for user ',a,' on TCP/IP')
940   format(i6)
      return
      end
c
      subroutine endup
      common line, nfail, nnet, nn
      character *132 line
c
      nstat = 1
      write(7,940) nnet, nfail
      if ((nnet .ne. 0) .or. (nn .ne. 0)) nstat = 3
      call exit ( nstat )
940   format(//,i5,' total network processes from outside of Ames',/,
     $          i5,' total login failures') 
      end
