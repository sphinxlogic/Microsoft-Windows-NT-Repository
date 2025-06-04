      character*80 scr
      dimension xa(4),ya(4),xb(4),yb(4),xx(10),yy(10)
      dimension xc(5000),yc(5000),xd(5000),yd(5000)

      call newdev('onehalf3d.ps',12)
      call psinit(.true.)
      call plot(1.,2.,-3)

      pi=4.*abs(atan(1.))
      angdeg=60.
      ang=angdeg*pi/180.
      sang=sin(ang)
      cang=cos(ang)
      ythk=.75
      ythk2=ythk+.45

      thk=.02
      tmed=.01
      thn=0.

      xwid=3.5
      ylen=6.
      ldsh=8

      call setlw(tmed)

      xa(1)=0.
      xa(2)=xa(1)+xwid
      xa(3)=xa(2)
      xa(4)=xa(1)

      ya(1)=0.
      ya(2)=ya(1)
      ya(3)=ya(2)+ythk
      ya(4)=ya(3)


      xb(1)=xa(1)+ylen*cang
      xb(2)=xb(1)+xwid
      xb(3)=xb(2)
      xb(4)=xb(1)

      yb(1)=ya(1)+ylen*sang
      yb(2)=yb(1)
      yb(3)=yb(2)+ythk
      yb(4)=yb(3)

      call sldlin(xa(4),ya(4),xa(3),ya(3),0.)
      call sldlin(xa(3),ya(3),xb(3),yb(3),0.)
      call sldlin(xb(3),yb(3),xb(4),yb(4),0.)

      call dshlin(xa(1),ya(1),xa(2),ya(2),ldsh,0.)
      call dshlin(xb(1),yb(1),xb(2),yb(2),ldsh,0.)
      call dshlin(xa(4),ya(4),xa(4),ya(4)-ythk2,ldsh,0.)
      call dshlin(xa(3),ya(3),xa(3),ya(3)-ythk2,ldsh,0.)
      call dshlin(xb(4),yb(4),xb(4),yb(4)-ythk2,ldsh,0.)
      call dshlin(xb(3),yb(3),xb(3),yb(3)-ythk2,ldsh,0.)
      call dshlin(xa(2),ya(2),xb(2),yb(2),ldsh,0.)

      over=1.2
      yopen=ylen/6.
      ymouth=ylen/2.
      x1=xa(4)-over*cang
      y1=ya(4)-over*sang
      x2=xb(4)+over*cang
      y2=yb(4)+over*sang

      xleft=xa(4)+ymouth*cang
      xright=xleft+yopen*cang
      yleft=ya(4)+ymouth*sang
      yright=yleft+yopen*sang

c  Draw ellipses
c  Define clipping region for ellipses
      call gsav
      xc(1)=xa(4)
      yc(1)=ya(4)
      xc(2)=xb(4)
      yc(2)=yb(4)
      xc(3)=xb(3)
      yc(3)=yb(3)
      xc(4)=xa(3)
      yc(4)=ya(3)
      call clipbox(xc,yc,4)

      call plot(.5*(xleft+xright),.5*(yleft+yright),-3)
      a=1.8
      b=1.
      nc=0
      anginc=1.
      do angd=0.,360.,anginc
      ang=angd*pi/180.
      nc=nc+1
      xc(nc)=a*cos(ang)
      yc(nc)=b*sin(ang)
      xd(nc)=xc(nc)
      yd(nc)=yc(nc)-ythk
      enddo
      call sldcrv(xc,yc,nc,0.)      

c  Draw vertical dashed lines
      call setlw(0.)
      nend=76
      do n=1,nend,4
      xp=xc(n)
      yp=yc(n)
      nlst1=n
      call dshlin(xp,yp,xp,yp-ythk,6,0.)
      enddo

      nst=nc-108
      do n=nc,nst,-4
      xp=xc(n)
      yp=yc(n)
      nlst2=n
      call dshlin(xp,yp,xp,yp-ythk,6,0.)
      enddo

      call plot(-.5*(xleft+xright),-.5*(yleft+yright),-3)
      call grest

c  Draw bottom dashed curve
c  Define clipping region for ellipses
      call gsav
      call plot(.5*(xleft+xright),.5*(yleft+yright),-3)
      xx(1)=xc(nlst1)
      yy(1)=yc(nlst1)
      xx(2)=xx(1)+5.
      yy(2)=yy(1)
      xx(3)=xx(2)
      yy(3)=yy(2)-5
      xx(4)=xx(1)
      yy(4)=yy(3)
      call clipbox(xx,yy,4)
      call dshcrv(xd,yd,nend,6,0.)      
      call plot(-.5*(xleft+xright),-.5*(yleft+yright),-3)
      call grest

      call gsav
      call plot(.5*(xleft+xright),.5*(yleft+yright),-3)
      xx(1)=xc(nlst2)
      yy(1)=yc(nlst2)
      xx(2)=xx(1)+5.
      yy(2)=yy(1)
      xx(3)=xx(2)
      yy(3)=yy(2)-5
      xx(4)=xx(1)
      yy(4)=yy(3)
      call clipbox(xx,yy,4)

      nd=0
      do n=nc,nlst2,-1
      nd=nd+1
      xc(nd)=xd(n)
      yc(nd)=yd(n)
      enddo
      call dshcrv(xc,yc,nd,6,0.)      
      call plot(-.5*(xleft+xright),-.5*(yleft+yright),-3)
      call grest

c  Shade shoreline
      do n=1,11
      shthk2=.5-(n-1)*.05
      gryl=.98-(n-1)*.025
      xx(1)=x1
      yy(1)=y1
      xx(2)=xleft      
      yy(2)=yleft      
      xx(3)=xx(2)-shthk2
      yy(3)=yy(2)
      xx(4)=xx(1)-shthk2
      yy(4)=yy(1)
      call filrgn(xx,yy,4,gryl)

      xx(1)=xright
      yy(1)=yright
      xx(2)=x2      
      yy(2)=y2      
      xx(3)=xx(2)-shthk2
      yy(3)=yy(2)
      xx(4)=xx(1)-shthk2
      yy(4)=yy(1)
      call filrgn(xx,yy,4,gryl)
      enddo

      shthk=.2
      rdiv=.15
      xp=xleft
      yp=yleft
5     continue
      call sldlin(xp,yp,xp-shthk,yp,.01)
      xp=xp-rdiv*cang
      yp=yp-rdiv*sang
      if(xp.ge.x1.and.yp.ge.y1) goto 5

      xp=xright
      yp=yright
10    continue
      call sldlin(xp,yp,xp-shthk,yp,.01)
      xp=xp+rdiv*cang
      yp=yp+rdiv*sang
      if(xp.le.x2.and.yp.le.y2) goto 10

      call sldlin(x1,y1,xleft,yleft,0.)
      call sldlin(xright,yright,x2,y2,0.)

      call dshlin(xleft,yleft,xleft,yleft-ythk,6,tmed)
      call dshlin(xright,yright,xright,yright-ythk,6,tmed)
      call dshlin(x1,y1-ythk,xb(1),yb(1),ldsh,tmed)

c  Text labels
      size=.14
      xp=xwid/3.
      ym=ya(4)-ythk/2.
      call grksym(xp,ym-size/2.,size,41,0.,1,1)
      call subber(1h1,1,size,0.)

      xp=xwid*2./3.
      call keksym(xp,ym-size/2.,size,1hH,0.,1,1)
      call subber(1h1,1,size,0.)
      yoff=.1
      call sldlin(xp,ym+yoff,xp,ya(4),0.)
      call farohed(xp,ya(4),0.,.1,20.,0,.true.)
      call sldlin(xp,ym-yoff,xp,ya(1),0.)
      call farohed(xp,ya(1),180.,.1,20.,0,.true.)

      xp=1.35
      yp=ya(1)-.4
      call grksym(xp,yp,size,41,0.,1,1)
      call subber(1h2,1,size,0.)
      call keksym(999.,999.,size,5h ( > ,0.,5,0)
      call grksym(999.,999.,size,41,0.,1,1)
      call subber(1h1,1,size,0.)
      call keksym(999.,999.,size,2h ),0.,2,0)

      xp=xleft-.5
      yp=yleft+.3
      ang=0.
      call keksym(xp,yp,size,1hM,ang,1,0)
      call subber(1hr,1,size,ang)

      x1=xp+.3
      yp=yp+size/2.
      x2=x1+.4
      call sldlin(x1,yp,x2,yp,0.)
      call farohed(x2,yp,90.,.1,20.,0,.true.)

      xp=xleft+1.1
      yp=yleft
      call grksym(xp,yp,size,41,0.,1,1)
      call subber(1hr,1,size,0.)

      xp=4.3
      yp=yleft
      call grksym(xp,yp,size,41,0.,1,1)
      call subber(1h1,1,size,0.)

      size=.15
      xp=1.1
      yp=7.5
      call keksym(xp,yp,size,1h1,0.,1,0)
      call onehlf(999.,999.,size,0.,0)
      call keksym(999.,999.,size,30h-layer, variable-density model,
     .            0.,30,0)

      call plotnd
      end
