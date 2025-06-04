C LOG DATA
	Integer*4 function logdata(buff,nbytes,filspc,ifil)
	integer*4 rabptr,rabsts,sys$flush,for$rab
c really want to treat buf as an array of bytes, but round
c here to allow more efficient I/O
	Integer*4 buff(1)
	INTEGER*4 nbytes,ifil
	character*255 filspc
	integer*4 init
	save init,ioff
	data init/0/
	data ioff/0/
C log the data buffered in buff, for nbytes bytes, to a file
C whose specification is in filspc if ifil .ne. 0
	if(init.ne.0)goto 1
	ioff=0
	init=1
	if(ifil.eq.0)goto 2
	open(unit=1,file=filspc,status='unknown',access='sequential',
     1  recl=18000,form='formatted')
	goto 3
2	continue
	open(unit=1,file='FVlog.jrn',status='unknown',access='sequential',
     1  recl=18000,form='formatted')
3	continue
1	continue
	if(nbytes.lt.0)goto 100
	if(ioff.eq.1)goto 1000
c dummy version, ignores errors in writing.
C Write 1000 bytes/record, just padding end with whatever's there.
C This adds some extra junk at the end
	nlongs=(nbytes+3)/4
	write(1,8000,err=1000)(buff(ii),ii=1,nlongs)
8000	format(128a4)
c flush to disk
	rabptr=for$rab(1)
	rabsts=sys$flush(%val(rabptr))
	goto 1000
100	continue
c close call (issued at exit, so no worry about reuse after that)
	close(unit=1)
	ioff=1
1000	continue
	logdata=1
	return
	end
