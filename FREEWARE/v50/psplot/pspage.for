c  Program pspage
c  This program creates a PostScript file containing selected pages 
c  of a source Postscript (PSPLOT) file.

      parameter(maxpag=1000)
      dimension ipage(maxpag)
      character*132 line
      character*80 scr,ifile,ofile,pages,printnow,ptray,pageorg
      character*1 bell,arg1*4,arg2*4,arg3*4
      logical ido,filend,doall,savefile,printit,lower,stepper,needlast
      integer lib$spawn
      common/commacom/ind(100)
      bell=char(7)

      call lib$get_symbol('ifile',ifile)       !Input PS file
      call lib$get_symbol('pages',pages)       !Pages to print (e.g. n, n-n)
      call lib$get_symbol('ofile',ofile)       !Output PS file
      call lib$get_symbol('printnow',printnow) !Immediate print
      call lib$get_symbol('ptray',ptray)       !Upper or lower paper tray

      pageorg=pages
      if(pages(1:1).eq.'A') then
        doall=.true.
      else
        doall=.false.
      endif

      if(index(pages(1:lenstr(pages,80)),'S').ne.0) then
        stepper=.true.
      else
        stepper=.false.
      endif

      if(doall.or.index(pages(1:lenstr(pages,80)),'A').ne.0) then
        needlast=.true.
      else
        needlast=.false.
      endif

      if(lenstr(ofile,80).ne.0) then
        savefile=.true.
      else
        savefile=.false.
      endif

      if(printnow(1:1).eq.'T') then
        printit=.true.
      else
        printit=.false.
      endif

      if(ptray(1:1).eq.'L') then
        lower=.true.
      else
        lower =.false.
      endif

      print *,' '
      print *,'Processing... '
      print *,' '

      if(.not.doall.and..not.stepper) then

        call getcomma(pages,itime)

        numpage=0
        do i=1,itime
        if(i.eq.1) then
          ist=1
        else
          ist=ind(i-1)+1
        endif
        iend=ind(i)-1
        scr=pages(ist:iend)
        call blkstp(scr,80,scr,lsc)
        if(index(scr(1:lsc),'-').eq.0) then   !Single page
          read(scr(1:lsc),'(i10)')nump
          numpage=numpage+1
          ipage(numpage)=nump
        else
          ip=index(scr(1:lsc),'-')
          read(scr(1:ip-1),'(i10)')nump1
          read(scr(ip+1:lsc),'(i10)')nump2
          do n=nump1,nump2
          numpage=numpage+1
          ipage(numpage)=n
          enddo
        endif
        enddo
        lastpage=ipage(numpage)
      endif

c  Open input file
      open(11,file=ifile,status='old',readonly)

c  If doing all pages, or an increment with all,find number of pages
      if(needlast) then
        rewind(11)
        numall=0
6       read(11,'(a132)',end=7)line
        ll=lenstr(line,132)
        if(index(line(1:ll),'showpage').ne.0)numall=numall+1
        goto 6
7       continue
        rewind(11)
      endif

c  If doing increments. set ipage now
      if(stepper) then
        call getcomma (pages,itime)
        iend=ind(i)-1
        scr=pages(1:ind(1)-1)           !1st argument
        call blkstp(scr,80,scr,lsc)
        read(scr(1:lsc),'(i10)')ifirst
        scr=pages(ind(1)+1:ind(2)-1)   !2nd argument
        call blkstp(scr,80,scr,lsc)
        if(index(scr(1:lsc),'A').ne.0) then  !Doing through end of file
          ilast=numall
        else
          read(scr(1:lsc),'(i10)')ilast
        endif
c       Find step value
c       (Skip the 'S' in the 3rd argument, and stop before last,)
        scr=pages(ind(2)+2:lenstr(pages,80)-1) !step
        call blkstp(scr,80,scr,lsc)
        read(scr(1:lsc),'(i10)')istep

c       Fill ipage array
        inum=0
        do n=ifirst,ilast,istep
         inum=inum+1
         ipage(inum)=n
        enddo
        numpage=inum
        lastpage=ipage(numpage)
      endif

c  Open output file
      if(savefile) then
      else
        ofile='kekkekkek.ps'
      endif
      open(12,file=ofile,status='new',carriagecontrol='list')

c  First, read until end of header(prologue)
5     read(11,'(a132)')line
      ll=lenstr(line,132)
      write(12,'(a)')line(1:ll)      
      if(line(1:6).ne.'/space') goto 5

      if(lower)then
c       Add the magic line
        write(12,'(''statusdict begin 1 setpapertray end'')')
      endif

      if(doall) then
c       Read rest of file 
16      read(11,'(a132)',end=900)line
        ll=lenstr(line,132)
        write(12,'(a)')line(1:ll)
        goto 16
      endif
  
      ipag=1      !position of input file
10    continue

c  Check to see if we're supposed to print this page
      if(doall) then
        ido=.true.
      else
        ido=.false.
        do n=1,numpage
        if(ipage(n).eq.ipag) ido=.true.
        enddo
      endif
     
      if(ido) then
       print *,'Writing page ',ipag
       call rwpag(11,12,filend)
       itot=itot+1
      else
        call rpag(11,filend)
      endif
      if(filend) goto 900
      ipag=ipag+1          !Increment page postion

      if ((doall.and.itot.eq.numall).or.
     .    (.not.doall.and.ipag.gt.lastpage)) goto 900
      goto 10

c  Shut it down
900   close(11)
      close(12)
c  Check to see if we should print
      if(printit) then
        print *,'Printing... '
        if(savefile)then
          scr='print/nofeed/name=Pspage/queue=remora '
     .         //ofile(1:lenstr(ofile,80))
        else
          scr='print/nofeed/delete/name=Pspage/queue=remora '
     .         //ofile(1:lenstr(ofile,80))
        endif
        call lib$spawn(scr)
      endif
      print *,' '
      print *,bell,'PSPAGE completed.'
      print *,' '
      print *,'Input file        : ',ifile(1:lenstr(ifile,80))
      if(stepper) then
      write(arg1,'(i4)')ifirst
      write(arg2,'(i4)')ilast
      write(arg3,'(i4)')istep
      call blkstp(arg1,4,arg1,l1)
      call blkstp(arg2,4,arg2,l2)
      call blkstp(arg3,4,arg3,l3)
      scr=arg1(1:l1)//' to '//arg2(1:l2)//' step '//arg3(1:l3)
      print *,'Pages to print    : ',scr(1:lenstr(scr,80))
      else
      print *,'Pages to print    : ',pageorg(1:lenstr(pageorg,80))
      endif
      if(savefile) then
      print *,'Output file       : ',ofile(1:lenstr(ofile,80))
      else
      print *,'Output file       : None'
      endif
      print *,'Immediate printing: ',printnow(1:lenstr(printnow,80))
      if(printit) then
      print *,'Paper tray        : ',ptray(1:lenstr(ptray,80))
      endif
      print *,' '

      end

      subroutine getcomma(str,ii)
      character*80 str
      common/commacom/ind(100)
      call blkstp(str,80,str,ls)
      str=str(1:ls)//','
      ls=ls+1
      ist=1
      ii=0
      ioff=0
1     continue
      if(index(str(ist:ls),',').ne.0) then
        ii=ii+1
        ind(ii)=ioff+index(str(ist:ls),',')    !Stores location of ,'s
        ist=ind(ii)+1
        ioff=ind(ii)
        goto 1
      endif
      return
      end

      subroutine rwpag(iuni,iuno,filend)
      logical filend
      character*132 line
      filend=.false.
1     read(iuni,'(a132)',end=10)line
      ll=lenstr(line,132)
      write(iuno,'(a)')line(1:ll)
      if(index(line(1:ll),'showpage').ne.0)return    !Hit a showpage
      goto 1
10    filend=.true.
      return
      end

      subroutine rpag(iuni,filend)
      logical filend
      character*132 line
      filend=.false.
1     read(iuni,'(a132)',end=10)line
      ll=lenstr(line,132)
      if(index(line(1:ll),'showpage').ne.0)return    !Hit a showpage
      goto 1
10    filend=.true.
      return
      end

      subroutine blkstp(ch,ndim,a,leng)
      character*1 ch(ndim),a(ndim)
c  strip out blanks only (leave in esc, etc.)
      i=1
      leng=0
20    continue
      if(ichar(ch(i)).ne.32)then
        leng=leng+1
        a(leng)=ch(i)
      endif
      if(i.eq.ndim) return
      i=i+1 
      goto 20
      end

      function lenstr(string,ls)
c  this routine finds actual length of string by eliminating trailing blanks
      character*1 string(ls)
      do 100 i=ls,1,-1
      is=i
      if(string(i).ne.char(32)) goto 200
100   continue
      is=0
200   lenstr=is
      return
      end
