	program nist_set_a

! NIST_SET_A - Set system time from the NIST time service
! Dale Dellutri, ddellutr@enteract.com, 1998-01-08

! Unit	Logical name	Description
! --	------------	-------------------------------------------------------
! 11	LU_HIST		History file
! This file contains one line for each complete run: System time, NIST time,
! Difference in seconds (system time - NIST time), msADV from the last received
! NIST time line, and one of NN (not synchronized, time not set), SS
! (synchronized, time set), or SN (synchronized, time not set).  This file is
! updated at the end of the run and will not be updated if an error occurs.
! 12	LU_DIAL		Dial file
! This file contains modem set up commands.  The program sends each line from
! from the file to the modem, ignoring responses (except that they are printed
! in the log file).  The last line must dial the modem to make the connection.
!	LU_TERM		Terminal line to connect to NIST time service
!	DF_SETTIME	Y = set time, else don't set time
!	FOR$PRINT	Log file
!	FOR$TYPE	Message file
!	SYS$TIMEZONE_DIFFERENTIAL	TDF system logical

! Log file output:
! 1. All messages (also go to the message file)
!  NIST_SET_A 00: Starting execution, system time: <DD-MMM-YYYY HH:MM:SS.CC>
!  NIST_SET_A 01: SYS$TIMEZONE_DIFFERENTIAL translation: <length> <translation>
!  NIST_SET_A 02: SYS$TIMEZONE_DIFFERENTIAL length error
!  NIST_SET_A 03: SYS$TIMEZONE_DIFFERENTIAL read error: <read status>
!  NIST_SET_A 06: DF_SETTIME translation: <length> <translation>
!  NIST_SET_A 07: LU_DIAL file is empty: error
!  NIST_SET_A 10: Time line length error, dying...
!  NIST_SET_A 11: Time line read error, dying...: <read status>
!  NIST_SET_A 12: Synchronized, time set to NIST time
!   System time: <DD-MMM-YYYY HH:MM:SS.CC>, NIST time: <DD-MMM-YYYY HH:MM:SS.CC>
!  NIST_SET_A 13: Synchronized, time not set
!   System time: <DD-MMM-YYYY HH:MM:SS.CC>, NIST time: <DD-MMM-YYYY HH:MM:SS.CC>
!  NIST_SET_A 14: Done, <end status>, <end message>
! 2. Terminal communication indications.  SSSS is iosb status (in hex), LLL is
!  data length (for reads with terminating characters, the terminator is
!  excluded from the length, but appears in the DATA), NTIME is the converted
!  NIST time received (with TDF added) (in hex), CTIME is the current system
!  time (in hex), DIFF is the difference in seconds (CTIME-NTIME).
!  C SSSS LLL DATA	! Result of clearing the typeahead buffer
!  W SSSS LLL DATA	! Data written
!  R SSSS LLL DATA	! Data read
!  I SSSS LLL DATA	! Data read and ignored during final phase
!  T          NTIME CTIME DIFF
! 3. Possible system messages indicating severe errors

! Suggested usage:
! 1. Make sure the SYS$TIMEZONE_DIFFERENTIAL is set properly.  (See
!  SYS$MANAGER:UTC$CONFIGURE_TDF.COM.)
! 2. Run the program with DF_SETTIME set to N.  Look at the log file to
!  determine the difference between the system time and the NIST time.  If the
!  difference is too large, check your system setup.
! 3. When you are satisfied, run the program with DF_SETTIME set to Y.
! 4. If you are not achieving synchronization, it's probably because the
!  program is not running at a high enough priority, or, if using a LAT line,
!  because the terminal server is too busy.
! 5. The program should work with both direct-connect and LAT terminal lines.
! 6. Suggested com file:
!  $ on error then goto error_exit
!  $ if p1 .eqs. "" then p1 := <default terminal line, for example TTA1>
!  $ if p2 .eqs. "" then p2 := N
!  $ all 'p1'
!  $ if $severity .ne. 1 then exit $status
!  $ set ter 'p1' /pasthru /nohostsync /nottsync /altypeahd -
!   /nobroadcast /noecho /nowrap /nodialup
!  $ ass/use 'p1' lu_term
!  $ ass/use nist_set_dial.par lu_dial
!  $ def/use df_settime "''p2'"
!  $ ass/use nist_set_hist.lis lu_hist
!  $ ass/use nist_set_log.lis for$print
!  $ run nist_set_a
!  $error_exit:
!  $ save_status = $status
!  $ on error then continue
!  $ deall 'p1'
!  $ sho sym save_status
!  $ exit save_status

	implicit none

	include		'($SYSSRVNAM)'
	include		'($IODEF)'
	include		'($SSDEF)'
	include		'($LNMDEF)'

	structure /read_iostat_block/
	 integer*2	iostat
	 integer*2	offset_to_terminator
	 integer*2	terminator
	 integer*2	terminator_size
	end structure

	structure /write_iostat_block/
	 integer*2	iostat
	 integer*2	byte_count
	 integer*2	mbz_a /0/
	 integer*2	mbz_b /0/
	end structure

	structure /terminator_block/
	 integer*2	mask_size
	 integer*2	mbz /0/
	 union
	  map
	   integer*4	mask_address
	  end map
	  map
	   integer*4	terminator_char_bit_mask
	  end map
	 end union
	end structure

	record /read_iostat_block/	riosb
	record /write_iostat_block/	wiosb
	record /terminator_block/	tsb_none,tsb_lf,tsb_cr,tlb_cp,tlb_chst

	integer*4	rfunc
	parameter	(rfunc=IO$_READVBLK+IO$M_TIMED)
	integer*4	wfunc
	parameter	(wfunc=IO$_WRITEVBLK+IO$M_NOFORMAT)

	integer*2	mask_cp(3) /0,0,512/	! )
	integer*2	mask_chst(3) /0,0,1032/	! #,*
	integer*2	channel
	integer*4	status,lmsg,lphnm,lcst,ltdf,tdf,i,mjd,hh,mm,ss
	integer*4	esi/1/
	integer*4	endstatus(3)/
	1	'18008010'x,	! Warning message #2
	1	'18008001'x,	! Success message #0
	1	'18008008'x/	! Warning message #1
	character*30	endmsga(3)/
	1	'Not synchronized, time not set',
	1	'Synchronized, time set',
	1	'Synchronized, time not set'/
	character*2	endmsgb(3)/'NN','SS','SN'/
	character*5	msadv
	character*8	ctdf/' '/
	character*1	cst/' '/
	character*16	cphnm/' '/
	character*256	msg/' '/
	integer*8	ntime/-1/,ctime/-1/
	character*23	cntime/' '/,cctime/' '/
	integer*4	lrb,lr
	parameter	(lrb=80)
	character*(lrb)	rb/' '/
	character*(lrb) r/' '/
	character*1	cr,lf
	parameter	(cr=char(13),lf=char(10))

9010	format (' ',a1,' ',z4.4,' ',i3,' ',a)
9020	format (' T          ',z16.16,' ',z16.16,' ',sp,f20.4)
9030	format (' ------System time------ -------NIST time-------
	1 ---Diff in seconds-- msADV ??')
9040	format (' ',a,' ',a,' ',sp,f20.4,' ',a,' ',a)

	status=SYS$ASCTIM(,%descr(cctime),,)
	if (.not. status) call LIB$SIGNAL(%val(status))
	type *,'NIST_SET_A 00: Starting execution, system time: ',cctime
	print *,'NIST_SET_A 00: Starting execution, system time: ',cctime
	tsb_none.mask_size=0
	tsb_none.terminator_char_bit_mask=0	! No terminators
	tsb_lf.mask_size=0
	tsb_lf.terminator_char_bit_mask=1024	! LF
	tsb_cr.mask_size=0
	tsb_cr.terminator_char_bit_mask=8192	! CR
	tlb_cp.mask_size=6
	tlb_cp.mask_address=loc(mask_cp)	! )
	tlb_chst.mask_size=6
	tlb_chst.mask_address=loc(mask_chst)	! #,*
! Translate SYS$TIMEZONE_DIFFERENTIAL to get TDF in seconds
	call srtrnlnm(ctdf,ltdf,
	1	'SYS$TIMEZONE_DIFFERENTIAL','LNM$SYSTEM_TABLE')
	type *,'NIST_SET_A 01: SYS$TIMEZONE_DIFFERENTIAL translation:',
	1 ltdf,' ',ctdf
	print *,'NIST_SET_A 01: SYS$TIMEZONE_DIFFERENTIAL translation:',
	1 ltdf,' ',ctdf
	if (ltdf .lt. 1 .or. ltdf .gt. 8) then
	 type *,'NIST_SET_A 02: SYS$TIMEZONE_DIFFERENTIAL length error'
	 print *,'NIST_SET_A 02: SYS$TIMEZONE_DIFFERENTIAL length error'
	 call exit('1800801A'x)
	end if
	read (unit=ctdf(1:ltdf),fmt='(i<ltdf>)',iostat=status) tdf
	if (status .ne. 0) then
	 type *,'NIST_SET_A 03: SYS$TIMEZONE_DIFFERENTIAL read error:',status
	 print *,'NIST_SET_A 03: SYS$TIMEZONE_DIFFERENTIAL read error:',status
	 call exit('1800801A'x)
	end if
! Get the set time flag
	call srtrnlnm(cst,lcst,'DF_SETTIME','LNM$PROCESS')
	type *,'NIST_SET_A 06: DF_SETTIME translation:',lcst,' ',cst
	print *,'NIST_SET_A 06: DF_SETTIME translation:',lcst,' ',cst
! Get a channel to the terminal device
	status=SYS$ASSIGN('LU_TERM',channel,,)
	if (.not. status) call LIB$SIGNAL(%val(status))
! Clear the typeahead buffer
	call srctba
! Open the LU_DIAL file, read each line and write it to the modem (with an
! appended cr), then wait 3 seconds.  The last line must dial the modem (no
! wait after the last line)
	open (unit=12,file='LU_DIAL',status='old',access='sequential',
	1 organization='sequential',form='formatted',readonly)
	read (unit=12,fmt='(q,a)',end=8010) lr,r
	do while (.true.)
	 if (lr .ne. 0) then
	  call srctba
	  call srwria(r(1:lr)//cr)
	 end if
	 read (unit=12,fmt='(q,a)',end=1010) lr,r
	 call srwaia(-3_8*10000000_8)
	end do
1010	close (unit=12)
! Now wait for connection.  If a LAT line, no connect message, so just wait.
! Read, timeout of 20 secs, with lf as terminator
	call srreaa(20,tsb_lf)
! Now read each line with a timeout of 5 secs, lf terminator, until <OTM>
	do while (index(rb(1:max(1,riosb.offset_to_terminator)),'<OTM>')
	1 .eq. 0)
	 call srreaa(5,tsb_lf)
	end do
! Read one more line, timeout of 5 secs, lf terminator
	call srreaa(5,tsb_lf)
! Now read time data lines, terminated by )
1020	call srreaa(5,tlb_cp)
! Convert time in line
	if (riosb.offset_to_terminator .ne. 47) then
	 type *,'NIST_SET_A 10: Time line length error, dying...'
	 print *,'NIST_SET_A 10: Time line length error, dying...'
	 goto 2010
	end if
	read (unit=rb,fmt='(i5,10x,i2,x,i2,x,i2,10x,a)',iostat=status)
	1 mjd,hh,mm,ss,msadv
	if (status .ne. 0) then
	 type *,'NIST_SET_A 11: Time line read error, dying...:',status
	 print *,'NIST_SET_A 11: Time line read error, dying...:',status
	 goto 2010
	end if
	ntime=10000000_8*(86400_8*mjd+3600_8*hh+60_8*mm+ss+tdf)
! Read for # or *
	status=SYS$QIOW(,%val(channel),%val(rfunc),riosb,,,
	1 %ref(rb),%val(lrb),%val(5),%ref(tlb_chst),,)
	if (.not. status) call LIB$SIGNAL(%val(status))
	status=SYS$GETTIM(%ref(ctime))
	if (.not. status) call LIB$SIGNAL(%val(status))
	if (riosb.iostat) then
	 if (rb(riosb.offset_to_terminator+1:riosb.offset_to_terminator+1)
	1 .eq. '#') then
	  if (cst .eq. 'Y') then
	   status=SYS$SETIME(%ref(ntime))
	   print 9010,'R',riosb.iostat,riosb.offset_to_terminator,
	1   rb(1:riosb.offset_to_terminator+1)
	   print 9020,ntime,ctime,dfloat(ctime-ntime)/10000000d0
	   if (.not. status) call LIB$SIGNAL(%val(status))
	   status=SYS$ASCTIM(,%descr(cntime),%ref(ntime),)
	   if (.not. status) call LIB$SIGNAL(%val(status))
	   status=SYS$ASCTIM(,%descr(cctime),%ref(ctime),)
	   if (.not. status) call LIB$SIGNAL(%val(status))
	   type *,'NIST_SET_A 12: Synchronized, time set to NIST time'
	   type *,' System time: ',cctime,', NIST time: ',cntime
	   print *,'NIST_SET_A 12: Synchronized, time set to NIST time'
	   print *,' System time: ',cctime,', NIST time: ',cntime
	   esi=2
	  else
	   print 9010,'R',riosb.iostat,riosb.offset_to_terminator,
	1   rb(1:riosb.offset_to_terminator+1)
	   print 9020,ntime,ctime,dfloat(ctime-ntime)/10000000d0
	   status=SYS$ASCTIM(,%descr(cntime),%ref(ntime),)
	   if (.not. status) call LIB$SIGNAL(%val(status))
	   status=SYS$ASCTIM(,%descr(cctime),%ref(ctime),)
	   if (.not. status) call LIB$SIGNAL(%val(status))
	   type *,'NIST_SET_A 13: Synchronized, time not set'
	   type *,' System time: ',cctime,', NIST time: ',cntime
	   print *,'NIST_SET_A 13: Synchronized, time not set'
	   print *,' System time: ',cctime,', NIST time: ',cntime
	   esi=3
	  end if
	  call srwria('%')
	  goto 2010
	 else	! Terminator must be a star
	  call srwria('*')
	  print 9010,'R',riosb.iostat,riosb.offset_to_terminator,
	1  rb(1:riosb.offset_to_terminator+1)
	  print 9020,ntime,ctime,dfloat(ctime-ntime)/10000000d0
	 end if
	else
	 print 9010,'R',riosb.iostat,riosb.offset_to_terminator,
	1 rb(1:riosb.offset_to_terminator+1)
	 print 9020,ntime,ctime,dfloat(ctime-ntime)/10000000d0
	 call LIB$SIGNAL(%val(riosb.iostat))
	end if
! Read for lf terminator, then loop if no timeout or hangup
	status=SYS$QIOW(,%val(channel),%val(rfunc),riosb,,,
	1 %ref(rb),%val(lrb),%val(5),%ref(tsb_lf),,)
	if (.not. status) call LIB$SIGNAL(%val(status))
	print 9010,'R',riosb.iostat,riosb.offset_to_terminator,
	1 rb(1:riosb.offset_to_terminator+1)
	if (riosb.iostat) goto 1020

! Update history file
2010	inquire (file='LU_HIST',exist=status)
	if (status) then
	 open (unit=11,file='LU_HIST',status='old',access='append',
	1 organization='sequential',form='formatted')
	else
	 open (unit=11,file='LU_HIST',status='new',access='sequential',
	1 organization='sequential',form='formatted')
	 write (unit=11,fmt=9030)
	end if
	if (esi .eq. 1) then
	 if (ntime .ne. -1) then
	  status=SYS$ASCTIM(,%descr(cntime),%ref(ntime),)
	  if (.not. status) call LIB$SIGNAL(%val(status))
	 end if
	 if (ctime .ne. -1) then
	  status=SYS$ASCTIM(,%descr(cctime),%ref(ctime),)
	  if (.not. status) call LIB$SIGNAL(%val(status))
	 end if
	end if
	write (unit=11,fmt=9040) cctime,cntime,
	1 dfloat(ctime-ntime)/10000000d0,msadv,endmsgb(esi)
	close (unit=11)
! Read and discard lines as they arrive until a timeout or hangup
2020	do while (riosb.iostat)
	 status=SYS$QIOW(,%val(channel),%val(rfunc),riosb,,,
	1 %ref(rb),%val(lrb),%val(5),%ref(tsb_lf),,)
	 if (.not. status) call LIB$SIGNAL(%val(status))
	 print 9010,'I',riosb.iostat,riosb.offset_to_terminator,
	1 rb(1:riosb.offset_to_terminator+1)
	end do
2030	status=SYS$GETMSG(%val(riosb.iostat),%ref(lmsg),
	1 %descr(msg),%val(15),)
	if (.not. status) call LIB$SIGNAL(%val(status))
	do i=1,lmsg,80
	 type *,msg(i:min(i+79,lmsg))
	 print *,msg(i:min(i+79,lmsg))
	end do
! Wait 10 seconds
	call srwaia(-10_8*10000000_8)
! Clear the typeahead buffer again
	call srctba
! De-assign the channel and finish
	status=SYS$DASSGN(%val(channel))
	if (.not. status) call LIB$SIGNAL(%val(status))
	type *,'NIST_SET_A 14: Done, ',endstatus(esi),', ',endmsga(esi)
	print *,'NIST_SET_A 14: Done, ',endstatus(esi),', ',endmsga(esi)
	call exit(endstatus(esi))

8010	type *,'NIST_SET_A 07: LU_DIAL file is empty: error'
	print *,'NIST_SET_A 07: LU_DIAL file is empty: error'
	call exit('1800801A'x)

	contains

	subroutine srctba
	implicit none
9010	format (' ',a1,' ',z4.4,' ',i3,' ',a)
1010	status=SYS$QIOW(,%val(channel),%val(rfunc),riosb,,,
	1 %ref(rb),%val(lrb),%val(0),%ref(tsb_none),,)
	if (.not. status) call LIB$SIGNAL(%val(status))
	if (riosb.iostat .ne. SS$_TIMEOUT .and. .not. riosb.iostat)
	1	call LIB$SIGNAL(%val(riosb.iostat))
	if (riosb.offset_to_terminator .ge. 1) then
	 print 9010,'C',riosb.iostat,riosb.offset_to_terminator,
	1 rb(1:riosb.offset_to_terminator)
	 goto 1010
	else
	 print 9010,'C',riosb.iostat,riosb.offset_to_terminator
	end if
	return
	end subroutine

	subroutine srwaia(wtime)
	implicit none
	integer*8	wtime
	status=SYS$SCHDWK(,,wtime,)
	if(.not. status) call LIB$SIGNAL(%val(status))
	status=SYS$HIBER()
	if(.not. status) call LIB$SIGNAL(%val(status))
	return
	end subroutine

	subroutine srwria(msg)
	implicit none
	character*(*)	msg
9010	format (' ',a1,' ',z4.4,' ',i3,' ',a)
	status=SYS$QIOW(,%val(channel),%val(wfunc),wiosb,,,
	1 %ref(msg),%val(len(msg)),,%val(0),,)
	if (.not. status) call LIB$SIGNAL(%val(status))
	print 9010,'W',wiosb.iostat,len(msg),msg
	if (.not. wiosb.iostat) call LIB$SIGNAL(%val(wiosb.iostat))
	return
	end subroutine

	subroutine srreaa(timeout,tb)
	implicit none
	integer*4	timeout
	record /terminator_block/	tb
9010	format (' ',a1,' ',z4.4,' ',i3,' ',a)
	status=SYS$QIOW(,%val(channel),%val(rfunc),riosb,,,
	1 %ref(rb),%val(lrb),%val(timeout),%ref(tb),,)
	if (.not. status) call LIB$SIGNAL(%val(status))
	print 9010,'R',riosb.iostat,riosb.offset_to_terminator,
	1 rb(1:riosb.offset_to_terminator+1)
	if (.not. riosb.iostat) call LIB$SIGNAL(%val(riosb.iostat))
	return
	end subroutine

	subroutine srtrnlnm (es,esl,lognam,tabnam)
	implicit none
	character*(*)	es	      	! equivalence string
	integer*4	esl		! equivalence string length
	character*(*)   lognam		! logical name
	character*(*)	tabnam		! logical name table
	integer*4	attr/LNM$M_CASE_BLIND/
	structure	/itmlst/
	 union
	  map
	   integer*2	buflen
	   integer*2	code
	   integer*4	bufadr
	   integer*4	retlenadr
	  end map
	  map
	   integer*4	end_list
	  end map
	 end union
	end structure
	record	/itmlst/ lnmlist(2)
	lnmlist(1).buflen	= len(es)
	lnmlist(1).code		= lnm$_string
	lnmlist(1).bufadr	= %loc(es)
	lnmlist(1).retlenadr	= %loc(esl)
	lnmlist(2).end_list	= 0
	status=SYS$TRNLNM(%ref(attr),%descr(tabnam),%descr(lognam),,
	1	%ref(lnmlist))
	if (status .eq. SS$_NOLOGNAM .or. status .eq. SS$_NOPRIV) then
	 es=' '
	 esl=0
	 return
	else if (status .eq. SS$_NORMAL) then
	 return
	else
	 call LIB$SIGNAL(%val(status))
	end if
	end subroutine

	end
