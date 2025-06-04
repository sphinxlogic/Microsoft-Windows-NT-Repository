c  This program plots 8 panel pages of monthly model fields
      parameter(imaxx=162,jmaxx=110)
      character*8 tim,dat*9
      character*132 cmdstr
      common/plt1/cmdstr
      character*80 scr,title
      character*20 monthc(12)
      data monthc/'January','February','March','April','May','June',
     1            'July','August','September','October','November',
     2            'December'/
      common/entcom/entcv(11)
      data entcv/-1.e-3,-3.e-4,-1.e-4,-1.e-5,1.e-5,1.e-4,3.e-4,
     1            1.e-3,3.e-3,1.e-2,3.e-2/
      dimension pcalw(12),pcalv(12)
      data pcalw/10., 5., 10., 5., 2.5, 2.5, 2.5, 2.5, 10., 2.5, 2.5,
     1           2.5/
      data pcalv/10.,20.,20.,20.,20.,20.,10.,10.,10.,10.,10.,10./

      common/iiicom/iii

      dimension rmsk(imaxx,jmaxx)
      dimension colval(100),clr(3,100)
      dimension rntini(imaxx,jmaxx),phyini(imaxx,jmaxx),
     1          zooini(imaxx,jmaxx),detini(imaxx,jmaxx)
      dimension rntm(imaxx,jmaxx),phym(imaxx,jmaxx),zoom(imaxx,jmaxx),
     1          detm(imaxx,jmaxx)
      dimension rntf(imaxx,jmaxx),phyf(imaxx,jmaxx),zoof(imaxx,jmaxx),
     1          detf(imaxx,jmaxx)
      dimension rnt2(imaxx,jmaxx),phy2(imaxx,jmaxx),zoo2(imaxx,jmaxx),
     1          det2(imaxx,jmaxx)
      dimension hu1(imaxx,jmaxx),hv1(imaxx,jmaxx),hu2(imaxx,jmaxx),
     1 hv2(imaxx,jmaxx),u1(imaxx,jmaxx),
     1 u2(imaxx,jmaxx),v1(imaxx,jmaxx),v2(imaxx,jmaxx),h1(imaxx,jmaxx),
     2 h2(imaxx,jmaxx),t1(imaxx,jmaxx),t2(imaxx,jmaxx),
     3 u1av(imaxx,jmaxx),u2av(imaxx,jmaxx),v1av(imaxx,jmaxx),
     4 v2av(imaxx,jmaxx),txp(imaxx,jmaxx),typ(imaxx,jmaxx)
      dimension qs(imaxx,jmaxx),qsen(imaxx,jmaxx),qlat(imaxx,jmaxx)
      dimension hmm(imaxx,jmaxx),tent(imaxx,jmaxx),tmm(imaxx,jmaxx)
      dimension q2(imaxx,jmaxx)
      dimension went(imaxx,jmaxx),weprme(imaxx,jmaxx)
      dimension xvel(imaxx,jmaxx),yvel(imaxx,jmaxx)
      dimension div(4)
      dimension cval(100),conarr(imaxx,jmaxx),grylev(100)
      dimension iscr(20),imon(5)
      dimension rmask(imaxx,jmaxx)
      logical wclos,eclos,sclos,nclos
      common/pltmsg/msglvl
      common/conpar/ispec,ioffpp,spvall,ilegg,ilabb,nhii,ndeccn,nlbll,
     +lscal,ldash,hgtlab
      data ispec, ioffpp,spvall,ilegg,ilabb,nhii,ndeccn,nlbll,lscal,
     +ldash,hgtlab
     1  /   1 ,     1,    -999., 0,    1,    -1,    -1,     3,  1,
     + 0, .1 /
      data xmndeg/35./,xmxdeg/115./,ymndeg/-29./,ymxdeg/25./
      data ylen/4.3/
      data iarw,icon/1,0/                      !!
      data itic/0100/
      data ibrd/0000/
      data ical,calx,caly/0,1.,.01/

      pi=4.*abs(atan(1.))
      rearth=6370.e5
      dypdeg=pi/180.*rearth
c  set reference month
      month=4

c  set plot message level
      msglvl=5
      numplt=0
      degpy=(ymxdeg-ymndeg)/(jmaxx-2)
      degpx=(xmxdeg-xmndeg)/(imaxx-2)

c  define desired section
      degb=-5.
      degt=ymxdeg
      degl=40.
      degr=80.
      xlen=ylen/(degt-degb)*(degr-degl)
      print *,'xlen= ',xlen
      facc=1.
      if(xlen.gt.10.) then
        facc=10./xlen
        print*,'possibility of xlen too big ',xlen
        print *,'setting facc to ',facc
      endif
      degpnx=(degr-degl)/xlen
      degpny=(degt-degb)/ylen

c  calculate corresponding i's and j's
      do 770 i=1,imaxx
      xdeg=xmndeg+(i-.5)*degpx
      if(xdeg.ge.degl) then
        il=i
        goto 771
      endif
770   continue
      print *,'could not find il, stop'
      stop
771   do 772 i=1,imaxx
      xdeg=xmndeg+(i-.5)*degpx
      xdegp=xmndeg+(i+.5)*degpx
      if(xdeg.le.degr.and.xdegp.ge.degr) then
        ir=i
        goto 773
      endif
772   continue
      print *,'could not find ir, stop'
      stop
773   do 774 j=1,jmaxx
      ydeg=ymndeg+(j-.5)*degpy
      if(ydeg.ge.degb) then
        jb=j
        goto 775
      endif
774   continue
      print *,'could not find jb, stop'
      stop
775   do 776 j=1,jmaxx
      ydeg=ymndeg+(j-.5)*degpy
      ydeg1=ymndeg+(j+.5)*degpy
      if(ydeg.le.degt.and.ydeg1.ge.degt) then
        jt=j
        goto 877
      endif
776   continue
      print *,'could not find jt, stop'
      stop
877   print *,'il,ir,jb,jt= ',il,ir,jb,jt
      xshft=(xmndeg+(il-.5)*degpx - degl)/degpnx
      yshftb=(ymndeg+(jb-.5)*degpy - degb)/degpny
      yshftt=(degt-(ymndeg+(jt-.5)*degpy))/degpny
      print *,'xshft, yshftb,yshftt= ',xshft,yshftb,yshftt

      imax=ir-il+1 +2
      jmax=jt-jb+1 +2
      print *,'imax,jmax= ',imax,jmax
      degpin=(degr-degl)/xlen
      xgrpin=(imax-2.)/xlen
      ygrpin=(jmax-2.)/ylen
      xcmpin=(degr-degl)/xlen
      ycmpin=(degt-degb)/ylen

c  define location of labels
      xplab=.45
      yplab=ylen-.35
c  --------------------------------
c     open input file

      open(11,file='indbio3main_70.dat',
     .     form='unformatted',status='old',readonly)

c  read in model run parameters
      read(11,end=11000)title,tim,dat
      read(11,end=11000) rmask,onehaf,wclos,sclos,eclos,nclos,hphi,
     1          h1det,timdet,timthc,timkwd,alpha,dx,dy,dt,h1in,
     1          h2in,h1ent,h1min,timent,t2in,t3in,visch,condh,condt,
     1          visch4,condh4,condt4,em,cd,cdsw,diss2,wscmin,twfac,
     1          wktfac,rchfac,hemin,smumix,deltat,temin,ce0,ce1,tnew1,
     1          tnew2,island,ndbeg,month,
     1          az,an,deathp,deathz,phip,phiz,grazz,f0,eps,pgrowth,
     .          e,rkn,rkw,q0,wsink,gamma,gammap,gammad,gamma3,rnt0,
     .          rnt3,rntst,altnrm,qmix,parfac,condn4,condp4,condz4,
     .          condd4

      read(11)rntini,phyini,zooini,detini

      do 170 i=1,imaxx-2
      do 170 j=1,jmaxx-2
      rntini(i,j)=rntini(i+1,j+1)
      phyini(i,j)=phyini(i+1,j+1)
      zooini(i,j)=zooini(i+1,j+1)
      detini(i,j)=detini(i+1,j+1)
170   continue

      lt=lenstr(title,80)
      scr='npzd'//title(1:lt)//'.ps'
      call blkstp(scr,80,scr,lsc)

      call newdev(scr(1:lsc),lsc)
      call psinit(' ',.true.,.false.)

c  print out parameter sheet
      goto 6666
5555  continue


      call chopit(0.,0.)

      xsep=1.2
      ysep=1.3-.4/facc
      call plot(0.,0.,-3)
      facc=.45
      call factor(facc)
      call setfnt(20)
      xorg=2.-.15/facc
      yorg=2*ylen+4+2.5/facc
      call plot(0.,0.,-3)

      call plot(xorg,yorg,-3)

      do 11000 ijk=1,1
      iun=10+ijk
      nqs=0
      ikkend=100
      do 10000 ikk=1,ikkend

      read(iun,end=11000) ndsv,kp,k,km,
     1    ((hu1(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((hv1(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((h1(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((t1(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((hu2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((hv2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((h2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((t2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((qs(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((qsen(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((qlat(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((txp(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((typ(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((hmm(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((tmm(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((tent(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((went(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((weprme(ii,jj),ii=1,imaxx),jj=1,jmaxx)
       read(iun)
     1    ((phym(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((phyf(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((phy2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((zoom(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((zoof(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((zoo2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((rntm(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((rntf(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((rnt2(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((detm(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((detf(ii,jj),ii=1,imaxx),jj=1,jmaxx),
     1    ((det2(ii,jj),ii=1,imaxx),jj=1,jmaxx)


      print 4445, ndsv
4445  format(1x,'Data for day ',i5)
      newmth=mod(ndsv/30+month-1,12)+1
      print *,'newmth= ',newmth

c  Skip unwanted months
      if(newmth.ne.9) goto 10000
  
c  Calculate u's and v's
      do 940 jj=1,jmaxx
      do 940 ii=1,imaxx-1
      if(h1(ii,jj).lt.1.e-3.or.h1(ii+1,jj).lt.1.e-3) then
        u1(ii,jj)=0.
      else
        u1(ii,jj)=2.*hu1(ii,jj)/(h1(ii,jj)+h1(ii+1,jj))
      endif
940   continue
      do 941 jj=1,jmaxx-1
      do 941 ii=1,imaxx
      if(h1(ii,jj).lt.1.e-3.or.h1(ii,jj+1).lt.1.e-3) then
        v1(ii,jj)=0.
      else
        v1(ii,jj)=2.*hv1(ii,jj)/(h1(ii,jj)+h1(ii,jj+1))
      endif
941   continue

      do 942 jj=1,jmaxx
      do 942 ii=1,imaxx-1
      if(h2(ii,jj).lt.1.e-3.or.h2(ii+1,jj).lt.1.e-3) then
        u2(ii,jj)=0.
      else
        u2(ii,jj)=2.*hu2(ii,jj)/(h2(ii,jj)+h2(ii+1,jj))
      endif
942   continue
      do 943 jj=1,jmaxx-1
      do 943 ii=1,imaxx
      if(h2(ii,jj).lt.1.e-3.or.h2(ii,jj+1).lt.1.e-3) then
        v2(ii,jj)=0.
      else
        v2(ii,jj)=2.*hv2(ii,jj)/(h2(ii,jj)+h2(ii,jj+1))
      endif
943   continue

c  **************************************************
c  Average u,v points to to get a h points
      do 975 i=2,imaxx
      do 975 j=2,jmaxx
      u1av(i,j)=(u1(i,j)+u1(i-1,j))/2.
      u2av(i,j)=(u2(i,j)+u2(i-1,j))/2.
      v1av(i,j)=(v1(i,j)+v1(i,j-1))/2.
      v2av(i,j)=(v2(i,j)+v2(i,j-1))/2.
975   continue

c  Fill arrays properly
      do 980 i=1,imaxx-2
      do 980 j=1,jmaxx-2
      u1av(i,j)=u1av(i+1,j+1)
      u2av(i,j)=u2av(i+1,j+1)
      v1av(i,j)=v1av(i+1,j+1)
      v2av(i,j)=v2av(i+1,j+1)
      went(i,j)=went(i+1,j+1)
      weprme(i,j)=weprme(i+1,j+1)
      hmm(i,j)=hmm(i+1,j+1) /100. !meters
      h1(i,j)=h1(i+1,j+1)/100.    !meters
      h2(i,j)=h2(i+1,j+1)/100.    !meters
      tmm(i,j)=tmm(i+1,j+1)
      t1(i,j)=t1(i+1,j+1)
      t2(i,j)=t2(i+1,j+1)
      qs(i,j)=qs(i+1,j+1)/2.39e-5         !w/m2
      qsen(i,j)=qsen(i+1,j+1)/2.39e-5     !w/m2
      qlat(i,j)=qlat(i+1,j+1)/2.39e-5     !w/m2
      q2(i,j)=-h2(i+1,j+1)*(t2(i+1,j+1)-t2in)/timthc/2.39e-5
      tent(i,j)=tent(i+1,j+1)
      txp(i,j)=txp(i+1,j+1)
      typ(i,j)=typ(i+1,j+1)
      rntm(i,j)=rntm(i+1,j+1)
      rntf(i,j)=rntf(i+1,j+1)
      rnt2(i,j)=rnt2(i+1,j+1)
      phym(i,j)=phym(i+1,j+1)
      phyf(i,j)=phyf(i+1,j+1)
      phy2(i,j)=phy2(i+1,j+1)
      zoom(i,j)=zoom(i+1,j+1)
      zoof(i,j)=zoof(i+1,j+1)
      zoo2(i,j)=zoo2(i+1,j+1)
      detm(i,j)=detm(i+1,j+1)
      detf(i,j)=detf(i+1,j+1)
      det2(i,j)=det2(i+1,j+1)
980   continue

      calsze=.12
      down=calsze*.8+.02

      do 40000 iii=1,8
      if(iii.le.2) then
        iarw=1
        icon=0
      else
        iarw=0
        icon=1
      endif

123   continue
      if(icon.ne.1) goto 45000
      print *,'il,ir,jb,jt= ',il,ir,jb,jt

      vmin=1.e20
      vmax=-1.e20

      do 4300 i=il,ir
      inew=i-il+1
      do 4300 j=jb,jt
      jnew=j-jb+1
      if(iii.eq.999) then
        conarr(inew,jnew)=tmm(i,j)
      elseif(iii.eq.4) then
        conarr(inew,jnew)=hmm(i,j)
      else if(iii.eq.3) then
        conarr(inew,jnew)=.5*(weprme(i,j)+abs(weprme(i,j)))*86400/100. !m/day
      else if(iii.eq.5) then
        conarr(inew,jnew)=rntm(i,j)
      else if(iii.eq.7) then
        conarr(inew,jnew)=zoom(i,j)
      else if(iii.eq.6) then
        conarr(inew,jnew)=phym(i,j)
      else if(iii.eq.8) then
        conarr(inew,jnew)=detm(i,j)
      endif
      if(rmask(i+1,j+1).ne.0.) then
        vmin=amin1(vmin,conarr(inew,jnew))
        vmax=amax1(vmax,conarr(inew,jnew))
      endif
      if(rmask(i+1,j+1).eq.0.)conarr(inew,jnew)=-999.
4300  continue

c  Contour plots
      yle=ylen-(yshftt+yshftb)
      xle=xlen-2.*xshft
      xle=xlen-1./xgrpin
      yle=ylen-1./ygrpin

c  Offset contour boundary
      call plot(.5/xgrpin,.5/ygrpin,-3)

      ng=11
      gbig=.1
      gsmall=1.

      if(iii.eq.3) then
        if(newmth.ge.4.and.newmth.le.10) then
          winc=5.e-1
        else
          winc=1.e-1
        endif
        wmin=0.
        do n=1,ng
        cval(n)=wmin+(n-1)*winc
        grylev(n)=gsmall+(n-1)*(gbig-gsmall)/(ng-1.)
        print *,'cval, grylev= ',cval(n),grylev(n)
        enddo
        if(newmth.ge.4.and.newmth.le.10) then  !avoid noise
          cval(1)=2.5e-1
        else
          cval(1)=5.e-2
        endif
      else
        if(iii.eq.5) then
          cval(1)=.25
          cval(2)=.5
          cval(3)=1.
          do n=1,ng
           if(n.ge.4)cval(n)=2.+(n-4)*2.
           grylev(n)=gsmall+(n-1)*(gbig-gsmall)/(ng-1.)
          enddo
        elseif(iii.eq.4) then
          big=140.
          small=40.
          do n=1,ng
          cval(n)=small+(n-1)*(big-small)/(ng-1.)
          grylev(n)=gsmall+(n-1)*(gbig-gsmall)/(ng-1.)
          enddo
        elseif(iii.eq.6) then
          cval(1)=.125
          cval(2)=.25
          cval(3)=.5
          cval(4)=.75
          cval(5)=1.
          cval(6)=1.5
          cval(7)=2.0
          cval(8)=2.5
          cval(9)=3.0
          cval(10)=3.5
          cval(11)=4.
        elseif(iii.eq.7) then
          cval(1)=.05
          cval(2)=.1
          cval(3)=.15
          cval(4)=.2
          cval(5)=.25
          cval(6)=.3
          cval(7)=.4
          cval(8)=.5
          cval(9)=.6
          cval(10)=.7
          cval(11)=.8
        elseif(iii.eq.8) then
          cval(1)=.25
          cval(2)=.5
          cval(3)=.75
          cval(4)=1.
          cval(5)=1.25
          cval(6)=1.5
          cval(7)=2.
          cval(8)=2.5
          cval(9)=3.
          cval(10)=3.5
          cval(11)=4.
        endif
      endif
      call confill(conarr,imaxx,imax-2,jmax-2 ,xle,
     .             yle,cval,grylev,ng,ioffpp,spvall)

c  Surround grayscale with contours
      call setlw(0.01)
      ilabb=0
      call conrec(conarr,imaxx,imax-2,jmax-2 ,xle, yle,cval,ng)
      call setlw(0.0)

c  Reset origin
      call plot(-.5/xgrpin,-.5/ygrpin,-3)

      call graybar(iii,newmth,xlen,ylen,ng,cval,grylev)

c  Draw outline
      call drwbrd(xlen,ylen,.false.)
C  Label degrees and draw border
      size=.13
      call labely(size,xlen,ylen,degb,degt,degpin)
      if(iii.eq.7.or.iii.eq.8)
     1   call labelx(size,xlen,ylen,degl,degr,degpin)
      call border(xlen,ylen,itic,ibrd,8,1,6,1)

      size=.2

      xunit=999.
      yunit=yplab+.03
      if(iii.eq.999) then
        call keksym(xplab+.8/facc,yplab+.03,size,1ht,0.,1,0)
        call subber(1hm,1,size,0.)
        call keksym(xunit,yunit,calsze,2h (,0.,2,0)
        call super(1ho,1,calsze,0.)
        call keksym(999.,999.,calsze,2hc),0.,2,0)
      elseif(iii.eq.4) then
        call keksym(xplab,yplab+.03,size,1hh,0.,1,0)
        call subber(1hm,1,size,0.)
        call keksym(xunit,yunit,calsze,5h  (m),0.,5,0)
      else if(iii.eq.3) then
        call keksym(xplab,yplab+.03,size,1hw,0.,1,0)
        call subsup(1hm,1,1h+,1,size,0.)
        call keksym(xunit,yunit,calsze,8h   (m/d),0.,8,0)
      else if(iii.eq.5) then
        call keksym(xplab,yplab+.03,size,1hN,0.,1,0)
        call subber(1hm,1,size,0.)
        call keksym(xunit,yunit,calsze,3h  (,0.,3,0)
        call grksym(999.,999.,calsze,36,0.,1,0.)
        call keksym(999.,999.,calsze,8hmolN/kg),0.,8,0)
      else if(iii.eq.7) then
        call keksym(xplab,yplab+.03,size,1hZ,0.,1,0)
        call subber(1hm,1,size,0.)
        call keksym(xunit,yunit,calsze,3h  (,0.,3,0)
        call grksym(999.,999.,calsze,36,0.,1,0.)
        call keksym(999.,999.,calsze,8hmolN/kg),0.,8,0)
      else if(iii.eq.6) then
        call keksym(xplab,yplab+.03,size,1hP,0.,1,0)
        call subber(1hm,1,size,0.)
        call keksym(xunit,yunit,calsze,3h  (,0.,3,0)
        call grksym(999.,999.,calsze,36,0.,1,0.)
        call keksym(999.,999.,calsze,8hmolN/kg),0.,8,0)
      else if(iii.eq.8) then
        call keksym(xplab,yplab+.03,size,1hD,0.,1,0)
        call subber(1hm,1,size,0.)
        call keksym(xunit,yunit,calsze,3h  (,0.,3,0)
        call grksym(999.,999.,calsze,36,0.,1,0.)
        call keksym(999.,999.,calsze,8hmolN/kg),0.,8,0)
      endif

c  Plot minimum and maximum values
      size=.1
      xp=xplab
      yp=1.6/facc
      call keksym(xp,yp,size,5hMax: ,0.,5,0)
      call keknum(xp+.2/facc,999.,size,vmax,0.,3,0)
      yp=yp-.1/facc
      call keksym(xp,yp,size,5hMin: ,0.,5,0)
      call keknum(xp+.2/facc,999.,size,vmin,0.,3,0)

45000 continue

c  arrows
      if(iarw.ne.1) goto 6500
c  do wind,u1-v1,u2-v2

c  fill arrays with appropriate velocities
      vmin=1.e20
      vmax=-vmin
      do 7500 i=il,ir
      inew=i-il+1
      do 7500 j=jb,jt
      jnew=j-jb+1
      rmsk(inew,jnew)=rmask(i+1,j+1)
      if(iii.eq.1) then
        xvel(inew,jnew)=txp(i,j)*rmask(i+1,j+1)
        yvel(inew,jnew)=typ(i,j)*rmask(i+1,j+1)
      else
        xvel(inew,jnew)=u1av(i,j)*rmask(i+1,j+1)
        yvel(inew,jnew)=v1av(i,j)*rmask(i+1,j+1)
      endif
      sp=sqrt(xvel(inew,jnew)**2+yvel(inew,jnew)**2)
      vmin=amin1(vmin,sp)
      vmax=amax1(vmax,sp)
      if(iii.eq.2) then
        xv=xvel(inew,jnew)
        yv=yvel(inew,jnew)
        if(xv.ne.0..or.yv.ne.0.)then
          theta=atan2(yv,xv)
        else
          theta=0.
        endif
        sp=sqrt(xv*xv+yv*yv)
        amp=sqrt(sp)
        xvel(inew,jnew)=amp*cos(theta)
        yvel(inew,jnew)=amp*sin(theta)
      endif
7500  continue

      size=.2
      if(iii.eq.1) then
        lm=lenstr(monthc(newmth),20)
        read(monthc(newmth),'(5a4)')imon
        call keksym(0.,ylen+.25,size,imon,0.,lm,0)
        write(scr,'(i10)')ndsv
        call blkstp(scr,80,scr,ls)
        scr=' (Day '//scr(1:ls)//')'
        ls=lenstr(scr,80)
        read(scr,'(20a4)')iscr
        call keksym(999.,999.,.6*size,iscr,0.,ls,0)

        lt=lenstr(title,80)
        read(title,'(20a4)')iscr
        call keksym(2*xlen+xsep,ylen+.3,1.1*size,iscr,0.,lt,2)
      endif

c  Draw border
      call border(xlen,ylen,itic,ibrd,8,1,6,1)
      call drwbrd(xlen,ylen,.false.)
      size=.13
      call labely(size,xlen,ylen,degb,degt,degpin)
      size=.2
      if(iii.eq.1) then
        call grksym(xplab,yplab+.03,size,43,0.,1,0)
        call keksym(999.,999.,calsze,9h  (dyn/cm,0.,9,0)
        call super(1h2,1,calsze,0.)
        call keksym(999.,999.,calsze,1h),0.,1,0)
      elseif(iii.eq.2) then
        call keksym(xplab,yplab+.03,size,1hv,0.,1,0)
        call subsup(1h1,1,1h',1,size,0.)
        call keksym(999.,999.,calsze,9h   (cm/s),0.,9,0)
        call onehlf(xplab+.45/facc,yplab+.03+.05,calsze,0.,1,0)
      endif
      xmax=-9999.
      ymax=-9999.
      do 500 i=1,imax
      do 500 j=1,jmax
      if(abs(xvel(i,j)).gt.xmax) xmax=abs(xvel(i,j))
      if(abs(yvel(i,j)).gt.ymax) ymax=abs(yvel(i,j))
  500 continue

      print 505,xmax,ymax
  505 format(1x,'xmax,ymax   ',2e15.3)

c  calculate calx, caly if not specified
      if(ical.eq.1) goto 570
      do 560 k=1,2
      if(k.eq.1) rmax=xmax
      if(k.eq.2) rmax=ymax
      if(rmax.eq.0.) then
      if(k.eq.1) calx=0.
      if(k.eq.2) caly=0.
      go to 560
      endif
      do 550 i=-7,4
      div(1)=10.**i
      div(2)=2.*div(1)
      div(3)=5.*div(1)
      div(4)=10.*div(1)
      do 520 l=1,3
      ls=l
      if(rmax.ge.div(l).and.rmax.le.div(l+1)) goto 535
  520 continue
  550 continue
      print 551, rmax
  551 format(1x,'could not find exponent for rmax=  ',e10.4)
      goto 12000
  535 continue
      if(k.eq.1) calx=div(ls+1)
      if(k.eq.2) caly=div(ls+1)
  560 continue
  570 continue

      if(iii.eq.1) then
       if(newmth.eq.7) then
         calx=5.
         caly=5.
       else
        calx=2.
        caly=2.
       endif
      else
        calx=10.
        caly=10.
      endif

      pcalx=calx
      pcaly=caly
      do 865 i=1,imax
      do 865 j=1,jmax
      xvel(i,j)=xvel(i,j)/xcmpin
      yvel(i,j)=yvel(i,j)/ycmpin
865   continue
      calx=calx/xcmpin
      caly=caly/ycmpin
      vecmax=-1.e38
      do 61 i=1,imax
      do 61 j=1,jmax
   61 vecmax=amax1(vecmax,sqrt(xvel(i,j)**2+yvel(i,j)**2))
      if(vecmax.eq.0.) goto 6500
c  define length of maximum vector, in inches
      vecmxl=.40
c  vecscl is cm/s represented by 1 inch
      vecscl=vecmax/vecmxl
      write(6,110) vecscl, pcalx, pcaly
  110 format('0','cm/s represented by one inch= ',e15.7,' calib vector',
     1 ' represents ',e10.3,' and ',e10.3,' cm/s in the x and y ',
     2 'directions')
c  draw calibration vectors
      xor=xplab
      yor=yplab-.13

      xor=xlen-.55/facc
      yor=ylen-.05/facc

      x1=calx/vecscl
      y1=caly/vecscl
      if(pcalx.gt.pcaly) then
        pnum=pcalx
        plen=x1
      else
        pnum=pcaly
        plen=y1
      endif

      call plot(xor,yor,3)
      call plot(xor+plen,yor,2)
      call arohed(xor+plen,yor,90.,.06,30.,0)
      if (amod(pnum,1.).eq.0.) then
          nd=-1
      else
          nd=1
      endif
      call keknum(xor,yor-calsze-down,calsze,pnum,0.,nd,0)
      if(iii.eq.1) then
        call keksym(999.,999.,calsze,7h dyn/cm,0.,7,0)
        call super(1h2,1,calsze,0.)
      else
        call keksym(999.,999.,calsze,7h (cm/s),0.,7,0)
        xover=.87
        call onehlf(xor+xover,yor-calsze-down+.05,calsze,0.,1,0)
      endif

c  Offset arrows to put points in middle of respective boxes
      call plot(.5/xgrpin,.5/ygrpin,-3)

      modj=2
      modi=4
      do 65 j=jmax-2,1,-modj
      is=modi-mod(j,modi)
      is1=is-1
      if(iii.eq.2) then
c       Pick a starting point which will show currents near eastern boundary
        do 4500 i=2,imax
        if(rmsk(i,j).ne.0.and.rmsk(i-1,j).eq.0) then
          is1=i
          goto 4510
        endif
4500    continue
4510    continue
      endif

      do 64 i=is1+1,imax-2,modi
      xx=(i-1.)/xgrpin
      yy=(j-1.)/ygrpin
      uin=xvel(i,j)/vecscl
      vin=yvel(i,j)/vecscl
      rr=sqrt(uin**2+vin**2)
c  Set threshold for arrow plotting if desired
      thresh=.05
      if(rr.lt.thresh) go to 64
      x1=xx+uin
      y1=yy+vin
      call plot(xx,yy,3)
      call plot(x1,y1,2)
      anghed=30.
      ang=atan2(vin,uin)*180./pi
      call arohed(x1,y1,90.-ang,.08,anghed,0)
 64   continue
 65   continue

c  Reset origin
      call plot(-.5/xgrpin,-.5/ygrpin,-3)

      size=.1
      xp=xplab
      yp=1.6/facc
      call keksym(xp,yp,size,5hMax: ,0.,5,0)
      call keknum(xp+.2/facc,999.,size,vmax,0.,2,0)
      if(iii.eq.1) then
        call keksym(999.,999.,size,7h dyn/cm,0.,7,0)
        call super(1h2,1,size,0.)
      elseif(iii.eq.2) then
        call keksym(999.,999.,size,5h cm/s,0.,5,0)
      endif

6500  continue

      numplt=numplt+1
      print *,'iii= ',iii
c     if(mod(iii,8).eq.0.and.newmth.ne.4) then
      if(mod(iii,8).eq.0.and.newmth.ne.9) then !No newpage after Sept plot
        print *,'calling chopit for iii= ',iii
        call chopit(0.,0.)
        call plot(xorg,yorg,-3)
      elseif(mod(iii,2).eq.1) then
        call plot(xlen+xsep,0.,-3)
      elseif(iii.eq.2) then
        call plot(-(xlen+xsep),-(ylen+ysep),-3)
      else
        call plot(-(xlen+xsep),-(ylen+ysep+.7),-3)
      endif

40000 continue
10000 continue
11000 continue
12000 continue
      call plotnd
      stop

6666  continue
c  bio variable parameter sheet
      xmid=4.
      sep=.3
      size=.11
      xunit=2.7
      call plot(1.75,8.75,-3)

      call keksym(0.,1.,.2,26hIndian Ocean Biology Model,0.,26,0)

      scr='RUN: '//title(1:lenstr(title,80))//'   '//tim//' '//dat
      call hiliteg(0.05,.6,.15,scr(1:lenstr(scr,80)),0.,.2,0,0,1.,0.)

      call keksym(0.,0.,size,1ha,0.,1,0)
      call subber(1hz,1,size,0.)
      call keknum(10*size,999.,size,az,0.,2,0)
      call keksym(0.,-1.*sep,size,1ha,0.,1,0)
      call subber(1hn,1,size,0.)
      call keknum(10*size,999.,size,an,0.,2,0)

      call grksym(0.,-2.*sep,size,36,0.,1,0)
      call subber(1hp,1,size,0.)
      if(deathp.ne.0.) then
        arg=1./deathp/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*deathp*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call grksym(0.,-3.*sep,size,36,0.,1,0)
      call subber(1hz,1,size,0.)
      if(deathz.ne.0.) then
        arg=1./deathz/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*deathz*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call grksym(0.,-4.*sep,size,50,0.,1,0)
      call subber(1hp,1,size,0.)
      call keknum(10*size,999.,size,phip,0.,4,0)

      call grksym(0.,-5.*sep,size,50,0.,1,0)
      call subber(1hz,1,size,0.)
      call keknum(10*size,999.,size,phiz,0.,4,0)

      call keksym(0.,-6.*sep,size,1hg,0.,1,0)
      call subber(1hr,1,size,0.)
      if(grazz.ne.0.) then
        arg=1./grazz/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,2,0)
        else
          arg=int(100*grazz*86400.+.5)/100.
          call keknum(10.*size,999.,size,arg,0.,2,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call keksym(0.,-7*sep,size,1hF,0.,1,0)
      call subber(1h0,1,size,0.)
      call keknum(10*size,999.,size,f0,0.,1,0)
      call grksym(xunit,-7*sep,size,36,0.,1,0)
      call keksym(999.,999.,size,7hmolN/kg,0.,7,0)

      call keksym(0.,-8.*sep,size,1hg,0.,1,0)
      if(pgrowth.ne.0.) then
        arg=1./pgrowth/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*pgrowth*86400.+.5)/10.
          call keknum(10*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call keksym(0.,-9.*sep,size,1he,0.,1,0)
      if(e.ne.0.) then
        arg=1./e/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*e*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call keksym(0.,-10.*sep,size,1hN,0.,1,0)
      call subber(1h0,1,size,0.)
      call keknum(10*size,999.,size,rnt0,0.,1,0)
      call grksym(xunit,-10.*sep,size,36,0.,1,0)
      call keksym(999.,999.,size,7hmolN/kg,0.,7,0)

      call keksym(0.,-11.*sep,size,1hN,0.,1,0)
      call subber(1h3,1,size,0.)
      call keknum(10*size,999.,size,rnt3,0.,1,0)
      call grksym(xunit,-11.*sep,size,36,0.,1,0)
      call keksym(999.,999.,size,7hmolN/kg,0.,7,0)

      call keksym(0.,-12.*sep,size,1hk,0.,1,0)
      call subber(1hn,1,size,0.)
      call kekexp(10*size,999.,size,rkn,0.,5,0)
      call keksym(xunit,999.,size,1hm,0.,1,0)
      call super(2h-1,2,size,0.)
      call keksym(999.,999.,size,1h(,0.,1,0)
      call grksym(999.,999.,size,36,0.,1,0)
      call keksym(999.,999.,size,8hmolN/kg),0.,8,0)
      call super(2h-1,2,size,0.)

      call keksym(0.,-13.*sep,size,1hk,0.,1,0)
      call subber(1hw,1,size,0.)
      call kekexp(10*size,999.,size,rkw,0.,5,0)
      call keksym(xunit,999.,size,1hm,0.,1,0)
      call super(2h-1,2,size,0.)

      call keksym(0.,-14.*sep,size,1hQ,0.,1,0)
      call subber(1h0,1,size,0.)
      call keknum(10*size,999.,size,q0,0.,1,0)
      call keksym(xunit,999.,size,3hW/m,0.,3,0)
      call super(1h2,1,size,0.)

      call keksym(0.,-15.*sep,size,1hw,0.,1,0)
      call subber(1hs,1,size,0.)
      wsink=wsink*86400.
      call keknum(10*size,999.,size,wsink,0.,1,0)
      call keksym(xunit,999.,size,3hm/d,0.,3,0)

      call grksym(0.,-16.*sep,size,27,0.,1,0)
      if(gamma.ne.0.) then
        arg=1./gamma/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*gamma*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call grksym(0.,-17.*sep,size,27,0.,1,0)
      call subber(1hp,1,size,0.)
      if(gammap.ne.0.) then
        arg=1./gammap/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*gammap*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call grksym(0.,-18.*sep,size,27,0.,1,0)
      call subber(1hd,1,size,0.)
      if(gammad.ne.0.) then
        arg=1./gammad/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*gammad*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      call grksym(0.,-19.*sep,size,27,0.,1,0)
      call subber(1h3,1,size,0.)
      if(gamma3.ne.0.) then
        arg=1./gamma3/86400.
        if(arg.gt.1.1) then
          call keksym(10*size,999.,size,2h1/,0.,2,0)
          call keknum(999.,999.,size,arg,0.,1,0)
        else
          arg=int(10*gamma3*86400.+.5)/10.
          call keknum(10.*size,999.,size,arg,0.,1,0)
        endif
      else
        call keksym(10*size,999.,size,2h1/,0.,2,0)
        call keksym(999.,999.,size,8hInfinity,0.,8,0)
      endif
      call keksym(xunit,999.,size,1hd,0.,1,0)
      call super(2h-1,2,size,0.)

      hfac=100.    !Not read in, so hardwire here!!!!!!!!!!!!!!!!!!!!!
      call keksym(0.,-20.*sep,size,1hH,0.,1,0)
      call subber(5hgamma,5,size,0.)
      call keknum(10*size,999.,size,hfac,0.,1,0)
      call keksym(xunit,999.,size,1hm,0.,1,0)

      call grksym(0.,-21.*sep,size,34,0.,1,0)
      call subber(1hq,1,size,0.)
      call kekexp(10*size,999.,size,qmix,0.,3,0)
      call keksym(xunit,999.,size,2hcm,0.,2,0)
      call super(1h2,1,size,0.)
      call keksym(999.,999.,size,2h/s,0.,2,0)

      call grksym(0.,-22.*sep,size,34,0.,1,0)
      call subber(2h4n,2,size,0.)
      call kekexp(10*size,999.,size,condn4,0.,2,0)
      call keksym(xunit,999.,size,2hcm,0.,2,0)
      call super(1h4,1,size,0.)
      call keksym(999.,999.,size,2h/s,0.,2,0)

      call grksym(0.,-23.*sep,size,34,0.,1,0)
      call subber(2h4p,2,size,0.)
      call kekexp(10*size,999.,size,condp4,0.,2,0)
      call keksym(xunit,999.,size,2hcm,0.,2,0)
      call super(1h4,1,size,0.)
      call keksym(999.,999.,size,2h/s,0.,2,0)

      call grksym(0.,-24.*sep,size,34,0.,1,0)
      call subber(2h4z,2,size,0.)
      call kekexp(10*size,999.,size,condz4,0.,2,0)
      call keksym(xunit,999.,size,2hcm,0.,2,0)
      call super(1h4,1,size,0.)
      call keksym(999.,999.,size,2h/s,0.,2,0)

      call grksym(0.,-25.*sep,size,34,0.,1,0)
      call subber(2h4d,2,size,0.)
      call kekexp(10*size,999.,size,condd4,0.,2,0)
      call keksym(xunit,999.,size,2hcm,0.,2,0)
      call super(1h4,1,size,0.)
      call keksym(999.,999.,size,2h/s,0.,2,0)

      call grksym(0.,-26.*sep,size,50,0.,1,0)
      call keknum(10*size,999.,size,parfac,0.,2,0)

      goto 5555
      end


      subroutine colorbar(ip,xl,yl,np,cval,color)
      dimension cval(100),color(3,100)
      dimension xa(4),ya(4)
      common/entcom/entcv(11)

      call plot(0.,-.7,-3)

      hgt=.2
      xsep=.05
c     xsep=.00
      wth=(xl-(np-1.)*xsep)/float(np)

      do n=1,np
      cv=cval(n)
      call setcolr(color(1,n),color(2,n),color(3,n))
      xa(1)=(n-1)*(wth+xsep)
      xa(2)=xa(1)+wth
      xa(3)=xa(2)
      xa(4)=xa(1)
      ya(1)=0.
      ya(2)=ya(1)
      ya(3)=hgt
      ya(4)=ya(3)
      call filrgnc(xa,ya,4)
      call setcolr(0.,0.,0.)  !back to black
      if(n.ne.np) then
        if(ip.eq.4) then
          call kekexp(xa(2)+xsep/2.,-.2,.09,cv,0.,1,1)
        elseif(ip.eq.6) then
          call keknum(xa(2)+xsep/2.,-.2,.11,cv,0.,2,1)
        else
          call keknum(xa(2)+xsep/2.,-.2,.11,cv,0.,1,1)
        endif
      endif
      enddo

      call plot(0.,.7,-3)
      return
      end

      subroutine graybar(ip,mon,xl,yl,np,cval,grylev)
      dimension cval(np),grylev(np),iscr(20)
      dimension xa(10),ya(10)
      common/entcom/entcv(11)
      call plot(0.,-.7,-3)

      hgt=.2
      xsep=.05
      wth=(xl-(np-1.)*xsep)/float(np)
      do n=1,np
      cv=cval(n)
      gry=grylev(n)

      print *,'ip,cv,gry= ',ip,cv,gry
      xa(1)=(n-1)*(wth+xsep)
      xa(2)=xa(1)+wth
      xa(3)=xa(2)
      xa(4)=xa(1)
      ya(1)=0.
      ya(2)=ya(1)
      ya(3)=hgt
      ya(4)=ya(3)
      call filrgn(xa,ya,4,gry)

      if(gry.eq.1.) then
        xa(5)=xa(1)
        ya(5)=ya(1)
        call dshcrv(xa,ya,5,12,0.)
      endif
      if(n.ne.np)call nozero(xa(2)+xsep/2.,-.2,.11,1,cv,4,iscr,20,ls)
      enddo

      call plot(0.,.7,-3)
      return
      end

      subroutine nozero(xp,yp,size,mjus,fnum,ndec,iarr,ndim,ls)
      dimension iarr(ndim)
      character*80 scr,frmt

      write(frmt,'(''(f10.'',i3,'')'')')ndec
      call blkstp(frmt,80,frmt,lf)
      scr=' '
      write(scr,frmt(1:lf))fnum

c  strip leading zero
      call blkstp(scr,80,scr,ls)
      call blkstp(scr,80,scr,ls)
      if(scr(1:3).eq.'-0.') then
        scr='-'//scr(3:ls)
      elseif(scr(1:2).eq.'0.') then
        scr=scr(2:ls)
      endif
      call blkstp(scr,80,scr,ls)

c  strip trailing zero's
100   continue
      if(scr(ls:ls).eq.'0'.and.scr(ls-1:ls-1).ne.'.') then
        ls=ls-1
        goto 100
      endif

      read(scr(1:ls),'(20a4)')iarr
      call keksym(xp,yp,size,iarr,0.,ls,mjus)
      return
      end

      subroutine drwbrd(xlen,ylen,iocean)
c  Draw outline
      parameter(ibnd=17)
      logical iocean
      dimension xbnd(ibnd),ybnd(ibnd),xcrv(ibnd),ycrv(ibnd)
      dimension xwipe(100),ywipe(100)
      parameter (nsri=2)
      dimension xsri(nsri),ysri(nsri)
      parameter(mada=7)
      dimension xmada(mada),ymada(mada)
      data xsri/39.5,40./
      data ysri/12.5,12./
      data xmada/9.,9.,14.,15.,15.,12.,9./
      data ymada/3.,11.5,17.,17.,14.,3.,3./

      data xbnd/0.,0.,8.,11.,11.,2.5,2.5,13.,19.,19.,
     1          28.,30.5,32.,38.,39.5,40.,40./
      data ybnd/0.,1.5,10.,15.,16.,16.,17.,20.5,26.,
     1          30.,30.,25.5,25.5,12.5,12.5,
     1          14.,0./

      thk=.03
      do 100 i=1,ibnd
      xcrv(i)=xlen*xbnd(i)/40.
      ycrv(i)=ylen*ybnd(i)/30.
100   continue

c  fill in ocean
      if(iocean) then
        call setcolr(0.,.9,.95)
        call filrgnc(xcrv,ycrv,ibnd)
      endif

c  wipe outer regions

      do 20 i=1,10
      xwipe(i)=xcrv(i)
      ywipe(i)=ycrv(i)
20    continue
      xwipe(11)=0.
      ywipe(11)=ylen
      call filrgn(xwipe(1),ywipe(1),11,1.)

      do 25 i=1,6
      xwipe(i)=xcrv(i+10)
      ywipe(i)=ycrv(i+10)
25    continue
      xwipe(7)=xlen
      ywipe(7)=ylen
      call filrgn(xwipe(1),ywipe(1),7,1.)

      call drwcrv(xcrv,ycrv,ibnd-1,thk,.false.)
c  realistic islands
c       sri lanka

      do 110 i=1,nsri
      xcrv(i)=xlen*xsri(i)/40.
      ycrv(i)=ylen*ysri(i)/30.
110   continue
      call sldlin(xcrv(1),ycrv(1),xcrv(2),ycrv(2),thk)

c  madagascar
      do 120 i=1,mada
      xcrv(i)=xlen*xmada(i)/80.
      ycrv(i)=ylen*ymada(i)/54.
120   continue

      xwipe(1)=xlen
      xwipe(2)=xwipe(1)+.05
      xwipe(3)=xwipe(2)
      xwipe(4)=xwipe(1)
      ywipe(1)=0.
      ywipe(2)=ywipe(1)
      ywipe(3)=ylen
      ywipe(4)=ywipe(3)
      call filrgn(xwipe(1),ywipe(1),4,1.)

      do 101 i=1,ibnd
      xcrv(i)=xlen*xbnd(i)/40.
      ycrv(i)=ylen*ybnd(i)/30.
101   continue
      call plot(xcrv(ibnd-1),ycrv(ibnd-1),3)
      call plot(xcrv(ibnd),ycrv(ibnd),2)
      call plot(xcrv(1),ycrv(1),2)

      return
      end

      subroutine gprint(arr,scf,imax,jmax,istart,iend,iskip,jstart,
     1 jend,jskip)
      dimension arr(imax,jmax),irl(200)
      sc=scf
      if(iskip.lt.0) then
        i1=max0(iend,istart)
        i2=min0(iend,istart)
      else
        i1=min0(iend,istart)
        i2=max0(iend,istart)
      endif
      if(jskip.lt.0) then
        j1=max0(jend,jstart)
        j2=min0(jend,jstart)
      else
        j1=min0(jend,jstart)
        j2=max0(jend,jstart)
      endif
      if(sc.ne.0.) go to 20
      rmax=0.
      do 10 j=j1,j2,jskip
      do 10 i=i1,i2,iskip
      r=abs(arr(i,j))
      rmax=amax1(rmax,r)
10    continue
      sc=1.
      if(rmax.ne.0.) then
        r=alog10(rmax)
        intr=int(r)
        sc=10.0**(intr-2)
        if((rmax.lt.1.).and.(r.ne.float(intr))) sc=sc/10.
      endif
20    continue
      print 200,i1,i2,iskip,j1,j2,jskip,sc
200   format(1x,'i:(',i3,',',i3,',',i3,')  j:(',i3,',',i3,
     1 ',',i3,')    multiply numbers by ',e15.6)
      do 40 j=j1,j2,jskip
      do 30 i=i1,i2,iskip
c  round to nearest integer
      irl(i)=nint(arr(i,j)/sc)
30    continue
      write(6,99)(irl(i),i=i1,i2,iskip)
99    format(1x,33i4)
40    continue
      write(6,98)
98    format(//)
      return
      end

      subroutine labely(size,xlen,ylen,degb,degt,degpin)
c  label degrees along y axis
      character*80 scr
      dimension iscr(20)
      common/iiicom/iii
      ikek=1
      xp=xlen+.5
      yp=-size/2.
      deg=degb
355   continue
      yt=yp+size/2.
      call plot(xlen,yt,3)
      call plot(xlen-.1,yt,2)
      if(mod(iii,2).eq.0) then
        if (deg.eq.degt.or.deg.eq.degb.or.amod(deg,10.).eq.0.) then
          if(deg.ne.0.) then
            write(scr,'(i3)')int(abs(deg)+.1)
            call blkstp(scr,80,scr,ls)
            if(deg.gt.0.)then
              scr=scr(1:ls)//'N'
            elseif(deg.lt.0.)then
              scr=scr(1:ls)//'S'
            endif
            read(scr,'(20a4)')iscr
            call keksym(xp,yp,size,iscr,0.,ls+1,2)
          else
            call keksym(xp,yp,size,2hEq,0.,2,2)
          endif
        endif
      endif
      if(ikek.ne.0) then
        deginc=5.
      else
        deginc=4.
      endif
      yp=yp+deginc/degpin
      if(yp.gt.ylen+1.e-2)goto 356
      deg=deg+deginc
      ikek=ikek+1
      goto 355
356   continue
      return
      end

      subroutine labelx(size,xlen,ylen,degl,degr,degpin)
c  label degrees along x axis
      ieast=0
      iwest=0
      xp=0.
      yp=-.3
      deg=degl
455   continue
      if(deg.le.180) then
        if(amod(deg,10.).eq.0.) then
          ieast=ieast+1
          call keknum(xp,yp,size,deg,0.,-1,1)
          call keksym(999.,999.,size,1hE,0.,1,0)
        endif
      else
        degg=180-(deg-180)
        if(amod(degg,10.).eq.10.) then
          iwest=iwest+1
          call keknum(xp,yp,size,degg,0.,-1,1)
          call keksym(999.,999.,size,1hW,0.,1,0)
        endif
      endif
      xp=xp+5.0/degpin
      if(xp.gt.xlen+1.e-1)goto 456
      deg=deg+5.0
      goto 455
456   continue
      return
      end

      subroutine coltab(pmin,pmax,rdiv,rinc,iright,numcolr,colval,clr,
     .                  iadj)
      parameter(imaxx=162,jmaxx=110)
      dimension color(3)
      logical iadj
      dimension colval(100),clr(3,100)

      print *,'entered coltab with min,max= ',pmin,pmax
c  calculate "nice" numbers; divisible by rdiv, inc by rinc

      if(rdiv.gt.1.) then
        iex=0
        ptar=int(pmax+.5)
        do while (amod(ptar,rdiv).ne.0.)
        pt=ptar+rinc
        ptar=int(pt*10.+1.e-4)/10.
        enddo
        iright=1
        pmax=ptar
        ptarm=ptar
      else
c       first, get pmax between 0 and 10
        iex=0
        do while (pmax.lt.1.)
          iex=iex+1
          pmax=pmax*10
        enddo
        ptar=int(pmax*10)/10.
        do while (amod(ptar,.5).ne.0.)
          pt=ptar+.1
          ptar=int(pt*10.+1.e-4)/10.
        enddo
        iright=iex+1
        if(amod(ptar,1.).ne.0.)iright=iright+1
        ptarm=ptar
        pmax=ptar*(10.**(-iex))
      endif
c     print *,'pmax= ',pmax
      iexm=iex
      m=ptar/rdiv
c     print *,'m,iexm,ptarm= ',m,iexm,ptarm
c do pmin
      if(pmin.ne.0.) then
        ptar=abs(pmin)
        if(rdiv.gt.1.) then
          iex=0
          ptar=int(ptar+.5)
          do while (amod(ptar,rdiv).ne.0.)
          pt=ptar+rinc
          ptar=int(pt*10.+1.e-4)/10.
          enddo
          iright=1
          ptarn=ptar
        else
          iex=0
          do while (ptar.lt.1.)
            iex=iex+1
            ptar=ptar*10
          enddo
          pp=int(ptar*10+1.e-4)/10.
          if(ptar.ne.pp)ptar=pp+.1
c         print *,'pp,ptar= ',pp,ptar
          do while (amod(ptar,rdiv).ne.0.)
            pt=ptar+rinc
            ptar=int(pt*10.+1.e-4)/10.
          enddo
          iright=iex+1
          if(amod(ptar,1.).ne.0.)iright=iright+1
          ptarn=ptar
        endif
        iexn=iex
        n=ptar/rdiv
c       print *,'n,iexn,ptarn= ',n,iexn,ptarn
        if(pmin.lt.0..and.iadj) then
          iex=max0(iexm,iexn)
          print *,'max iex= ',iex
          m=m*10**(iex-iexm)
          n=n*10**(iex-iexn)
c         print *,'adjusted m,n= ',m,n
c         now adjust n such that 0 is plotted on scale
c         condition is that n*10/(n+m) is an integer
          do while(mod(n*10,(m+n)).ne.0)
             n=n+1
          enddo
c         print *,'new n= ',n
          ptar=n*rdiv
        endif
        pmin=ptar*(10.**(-iexn))*sign(1.,pmin)
      endif

      pi=4.*abs(atan(1.))

      plen=pmax-pmin
      pinc=plen/50.
      cfac=1.    !darkening factor
      do 932 nn=1,numcolr
      rn=pmin+(nn-1)*(pmax-pmin)/(numcolr-1.)

      p11=pmin+.5*plen
      p12=pmin+.75*plen

      if(rn.le.p11) then
        color(1)=0.
      elseif(rn.le.p12) then
        color(1)=(rn-p11)/(p12-p11)
      else if(rn.le.pmax) then
        color(1)=1.
      endif
      color(1)=cfac*color(1)

      p21=pmin+.25*plen
      p22=pmin+.75*plen
      if(rn.le.p21) then
        color(2)=(rn-pmin)/(p21-pmin)
      elseif(rn.le.p22) then
        color(2)=1.
      elseif(rn.le.pmax) then
        color(2)=1.-(rn-p22)/(pmax-p22)
      endif
      color(2)=cfac*color(2)

      p31=pmin+.25*plen
      p32=pmin+.5*plen
      if(rn.le.p31) then
c       color(3)=1.
        color(3)=amin1(1.,(rn-pmin)/(p31-pmin)+.3)
      elseif(rn.le.p32) then
        color(3)=1.-(rn-p31)/(p32-p31)
      elseif(rn.le.pmax) then
        color(3)=0.
      endif
      color(3)=cfac*color(3)
      do nc=1,3
      clr(nc,nn)=color(nc)
      enddo
      colval(nn)=rn
932   continue
      print *,'leaving coltab with min,max,inc= ',pmin,pmax,pinc
      return
      end
