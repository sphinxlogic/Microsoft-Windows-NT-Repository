c  Program pslayout
c  This program prints selected pages of a source Postscript file in a 
c  user-selected format of irow by icol pages/(paper page).
c  Pslayout expects a psplot plotting library output file.

      parameter(maxpag=1000)
      character*132 line
      character*80 scr,ifile,ofile,pages,irowcolc,irowc,icolc,
     .             portraitc,printnow,ptray,pageorg,pskip
      character*4 arg1,arg2,arg3
      character*1 bell
      logical ido,filend,doall,savefile,printit,lower,portrait,stepper,
     .        needlast,skipper,skipall
      integer*4 lib$spawn
      dimension ipage(maxpag),iskip(maxpag)
      common/scalecom/scalenew
      common/commacom/ind(100)

      bell=char(7)

      call lib$get_symbol('ifile',ifile)        !Input PS file
      call lib$get_symbol('pages',pages)        !Pages to print (e.g. n, n-n)
      call lib$get_symbol('ofile',ofile)        !Output PS file
      call lib$get_symbol('printnow',printnow)  !Immediate print
      call lib$get_symbol('ptray',ptray)        !Paper tray (upper/lower)
      call lib$get_symbol('irowicol',irowcolc)  !Number of frames per row/
                                                !frames/col
      call lib$get_symbol('portrait',portraitc) !Portrait mode logical
      call lib$get_symbol('pskip',pskip)        !Page locations to skip

      pageorg=pages

c  Set required logicals using symbol text strings
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

      if(doall.or.index(pages(1:lenstr(pages,80)),'A').ne.0
     .        .or.index(pskip(1:lenstr(pskip,80)),'A').ne.0) then
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

      if(portraitc(1:1).eq.'T') then
        portrait=.true.
      else
        portrait=.false.
      endif

      if(lenstr(pskip,80).ne.0) then
        skipper=.true.
      else
        skipper=.false.
      endif

c  Parse irowcolc to number of frames/row and frames/col
      ip=index(irowcolc,'/')
      if(ip.eq.0) then
        print *,bell,'Invalid row/column argument ',irowcolc
        print *,'Pslayout aborted.'
        stop
      else
        irowc=irowcolc(1:ip-1)
        icolc=irowcolc(ip+1:lenstr(irowcolc,80))
      endif

      read(irowc(1:lenstr(irowc,80)),'(i10)')irow        
      read(icolc(1:lenstr(icolc,80)),'(i10)')icol        
        
      ifpp=irow*icol    !Frames per page

c  Determine new factor for this layout
      scalenew=amin1(1./float(irow),1./float(icol))

c  Determine width and height of these new sub-pages
c  The following values may be printer-dependent
      xorg=.2*72
      yorg=.2*72

      if(portrait) then
        usableh=10.6
        usablew=8.1
      else
        usableh=8.1
        usablew=10.6
      endif 
      wpp=usablew/float(irow)
      hpp=usableh/float(icol)

      print *,' '
      print *,'Processing... '
      print *,' '

c  Open input file
      open(11,file=ifile,status='old',readonly)

      if(needlast) then
        print *,'Determining number of frames... '
        print *,' '
        rewind(11)
        numtot=0
6       read(11,'(a132)',end=7)line
        ll=lenstr(line,132)
        if(index(line(1:ll),'showpage').ne.0)numtot=numtot+1
        goto 6
7       rewind(11)
        print *,'Number of frames in file= ',numtot
        print *,' '
      endif

      if(stepper) then                   !Parse to get pages to plot
        call getcomma (pages,itime)
        scr=pages(1:ind(1)-1)                  !1st argument
        call blkstp(scr,80,scr,lsc)
        read(scr(1:lsc),'(i10)')ifirst
        scr=pages(ind(1)+1:ind(2)-1)           !2nd argument
        call blkstp(scr,80,scr,lsc)
        if(index(scr(1:lsc),'A').ne.0) then    !Doing through end of file
          ilast=numtot
        else
          read(scr(1:lsc),'(i10)')ilast
        endif
        scr=pages(ind(2)+2:lenstr(pages,80)-1) !Increment
        call blkstp(scr,80,scr,lsc)
        read(scr(1:lsc),'(i10)')istep
        inum=0
        do n=ifirst,ilast,istep
         inum=inum+1
         ipage(inum)=n
        enddo
        numpage=inum
        lastpage=ipage(numpage)
      endif

      if(skipper) then                   !Parse to get frame locations to skip
        call getcomma (pskip,itime)
        scr=pskip(1:ind(1)-1)                !1st argument
        call blkstp(scr,80,scr,lsc)
        read(scr(1:lsc),'(i10)')iskips
        scr=pskip(ind(1)+1:ind(2)-1)         !2nd argument
        call blkstp(scr,80,scr,lsc)
        if(index(scr(1:lsc),'A').ne.0) then  !Doing through end of file
          skipall=.true.
          iskipe=maxpag		             !Iskipe is absolute frame location	
        else
          skipall=.false.
          read(scr(1:lsc),'(i10)')iskipe
        endif
        scr=pskip(ind(2)+1:lenstr(pskip,80)-1) !Increment
        call blkstp(scr,80,scr,lsc)
        read(scr(1:lsc),'(i10)')incskip

        nskip=0
        do i=iskips,iskipe,incskip
        nskip=nskip+1
        iskip(nskip)=i
        enddo
      endif

      if(.not.doall.and..not.stepper) then  !Parse to get pages to plot
        call getcomma (pages,itime)
        numpage=0
        do i=1,itime
        if(i.eq.1) then
          ist=1
        else
          ist=iend+2
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

c  Open output file
      if(savefile) then             !Use user-supplied filename
      else
        ofile='kekkekkek.ps'        !Scratch file
      endif
      open(12,file=ofile,status='new',carriagecontrol='list')

c  First, read input file until end of header(prologue)
5     read(11,'(a132)')line
      write(12,'(a)')line(1:lenstr(line,132))      
      if(line(1:6).ne.'/space') goto 5

      if(lower)then
c       Add the magic line
        write(12,'(''statusdict begin 1 setpapertray end'')')
      endif
       
      ipag=1      !position of input file, i.e. actual original frame number
      iout=0      !Frames/page written
      itot=0      !Total frames written
      iframe=0    !Total frame counter (running sum of iout)
     
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
        if(skipper) then        !Check to see if we should skip this location
500       continue            
          do i=1,nskip
          if(iframe+1.eq.iskip(i)) then
            print *,'Skipping location ',iframe+1
c           Move to proper origin
            call getorg(iout,xorg,yorg,usableh,hpp,wpp,irow,xmove,ymove)
            write(12,'(2f8.2,'' translate gsave'')')xmove,ymove
            iout=iout+1
            iframe=iframe+1
            goto 500
          endif
          enddo
        endif            

        if(iout.eq.ifpp) then    !Need new page
          write(12,'(''showpage'')')
          iout=0
        endif
        if(iout.eq.0) then
c         Set proper orientation
          if(.not.portrait) 
     .       write(12,'(''90 rotate 0 -8.5 inch translate'')')
c         Draw frame boundaries on the page
c         Set linewidth of the borders
          write(12,'(''.01 Slw'')')         
          xw=usablew*72.
          xwm=-xw
          yh=usableh*72.
          write(12,100),xorg,yorg,xw,yh,xwm
100       format('newpath ',2f8.2,' moveto ',f8.2,' 0 rlineto'/
     .        ' 0 ',f8.2,' rlineto ',f8.2,' 0 rlineto closepath stroke')

          do n=1,irow-1
           xpp=n*wpp*72.+xorg
           write(12,110)xpp,yorg,yh
110        format('newpath ',2f8.2,' moveto 0 ',f8.2,' rlineto stroke')
          enddo

          do n=1,icol-1
           ypp=n*hpp*72.+yorg
           write(12,120)xorg,ypp,xw
120        format('newpath ',2f8.2,' moveto ',f8.2,' 0 rlineto stroke')
          enddo
        endif

c       Move to proper origin
        call getorg(iout,xorg,yorg,usableh,hpp,wpp,irow,xmove,ymove)
        write(12,'(2f8.2,'' translate gsave'')')xmove,ymove

c       Adjust xorigin back after gsave, since (0,0) is actually off the left
c       edge of the page. It's effect will be lost at the next grestore.
        xadj=-.1*72.
        write(12,'(f8.2,'' 0 translate'')')xadj 

        if(needlast) then
          print 150, ipag, numtot
150       format(1x,'Writing file frame ',i4,' of ',i4)
        else
          print 160, ipag
160       format(1x,'Writing file frame ',i4)
        endif         
        call rwpag(11,12,filend)
        write(12,'(''grestore'')')
        iout=iout+1
        iframe=iframe+1
        itot=itot+1
c       Write the original frame number in lower left corner
        call wrtfr(12,ipag,wpp)
      else
        call rpag(11,filend)
      endif

      if(filend) goto 900  !Hit end-of-file
  
      ipag=ipag+1          !Increment page postion

      if((doall.and.itot.eq.numtot).or.
     .   (.not.doall.and.ipag.gt.lastpage)) then
        goto 900           !We're done
      else
        goto 10
      endif

c  Shut it down
900   close(11)
      write(12,'(''showpage'')')
      close(12)

      if(printit) then
        print *,bell,'Printing file ... '
        if(savefile) then
          scr='print/nofeed/name=Pslayout/queue=remora '
     .        //ofile(1:lenstr(ofile,80))
        else
          scr='print/nofeed/delete/name=Pslayout/queue=remora '
     .        //ofile(1:lenstr(ofile,80))
        endif
        call lib$spawn(scr)
      endif

      print *,' '
      print *,bell,'PSLAYOUT completed.'
      print *,' '
        print *,'Input file         : ',ifile(1:lenstr(ifile,80))
      if(doall) then
        print *,'Pages to print     : All'
      elseif(stepper) then
        write(arg1,'(i4)')ifirst
        write(arg2,'(i4)')ilast
        write(arg3,'(i4)')istep
        call blkstp(arg1,4,arg1,l1)
        call blkstp(arg2,4,arg2,l2)
        call blkstp(arg3,4,arg3,l3)
        scr=arg1(1:l1)//' to '//arg2(1:l2)//' step '//arg3(1:l3)
        print *,'Pages to print     : ',scr(1:lenstr(scr,80))
      else
        print *,'Pages to print     : ',pageorg(1:lenstr(pageorg,80))
      endif
      if(savefile) then
        print *,'Output file        : ',ofile(1:lenstr(ofile,80))
      else
        print *,'Output file        : None'
      endif
      if(printit) then
        print *,'Immediate printing : Yes'
      else
        print *,'Immediate printing : No'
      endif
      if(lower) then
        print *,'Paper tray         : Lower'
      else
        print *,'Paper tray         : Upper'
      endif
        print *,'Frames per row     : ',irowc(1:lenstr(irowc,80))
        print *,'Frames per column  : ',icolc(1:lenstr(icolc,80))
      if(portrait) then
        print *,'Page orientation   : Portrait'
      else
        print *,'Page orientation   : Landscape'
      endif
      if(skipper) then
        write(arg1,'(i4)')iskips
        if(skipall) then
          arg2='End'
        else
          write(arg2,'(i4)')iskipe
        endif
        write(arg3,'(i4)')incskip
        call blkstp(arg1,4,arg1,l1)
        call blkstp(arg2,4,arg2,l2)
        call blkstp(arg3,4,arg3,l3)
        scr=arg1(1:l1)//' to '//arg2(1:l2)//' step '//arg3(1:l3)
        print *,'Frames to skip     : ',scr(1:lenstr(scr,80))
      else
        print *,'Frames to skip     : None'
      endif
      print *,' '

      end


      subroutine wrtfr(iun,ipag,wpp)
      character*80 scr
      write(iun,'(''/Curfnt /Times-Roman findfont def 8 Setf'')')
      xloc=(wpp-.45)*72.
      yloc=.05*72.
      write(iun,'(2f8.2,'' moveto'')')xloc,yloc
      write(scr,'(i4)')ipag
      call blkstp(scr,80,scr,lsc)
      scr='(Frame '//scr(1:lsc)//') show'
      lsc=lenstr(scr,80)
      write(iun,'(a)')scr(1:lsc)
      return
      end

      subroutine getorg(iout,xorg,yorg,usableh,hpp,wpp,irow,xmove,ymove)
      if(iout.eq.0) then
        xmove=xorg
        ymove=yorg+(usableh-hpp)*72.
      elseif(mod(iout,irow).eq.0) then
        xmove=-(irow-1)*wpp*72.
        ymove=-hpp*72.
      else
        xmove=wpp*72.
        ymove=0.
      endif      
      return
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
      logical filend,hitscale
      character*132 line,str
      common/scalecom/scalenew

      filend=.false.
      hitscale=.false.
1     read(iuni,'(a132)',end=10)line
      ll=lenstr(line,132)
      if(index(line(1:ll),'showpage').ne.0) then
        return                  !Hit a showpage, but don't write it to the file
      elseif(index(line(1:ll),'scale').ne.0.and..not.hitscale)then 
        hitscale=.true.         !Hit 1st scale command 
        write(str,'(2f8.2,'' scale'')')scalenew,scalenew
        write(iuno,'(a)')str(1:lenstr(str,132))
      endif
      write(iuno,'(a)')line(1:ll)
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
c  Strip out blanks only (leave in esc, etc.)
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
c  This routine finds actual length of string by eliminating trailing blanks
      character*1 string(ls)
      do 100 i=ls,1,-1
      is=i
      if(string(i).ne.char(32)) goto 200
100   continue
      is=0
200   lenstr=is
      return
      end
