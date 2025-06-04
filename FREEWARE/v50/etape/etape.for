	program etape
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	ETAPE.for
c This program manipulates foreign tapes
c format). It allows the user to look at any file or block on the tape,
c to move around anywhere in the tape, and to translate the data and
c put it in a disk file(s). EBCDIC translation may be turned off.
c
c Version 1.0
c Date:	04-feb-1982
c	17-Feb-1982	toggle EBCDIC mode (UT function) added
c	24-May-1982	write capability (WR & TM functions) added
c	23-Feb-1988	add auto-translate function - dom
c
c	Rev. 2.0  - 12-May-1988	Full-screen version
c	Rev. 2.1  - 08-Mar-1989	Added EBCDIC standard label writing
c	Rev. 2.2  - 12-Apr-1989 Added wildcard capibility to WR command
c	Rev. 2.3  - 20-Apr-1989 Bug fixes and cleanup
c	Rev. 2.4  - 27-Jun-1989	Added variable-length records, density select
c	Rev. 2.5  - 08-Aug-1989 Added special translation
c	Rev. 2.6  - 19-Sep-1989 Bug fixes to sp. trans. and GBCD
c	Rev. 2.7  - 11-Jan-1990	Added IBM floating-point to special trans
c	Rev. 2.8  - 23-Jan-1990 Fixed positioning problems (thanks to jwm)
c	Rev. 2.9  - 19-Feb-1990 Fixed BCD block truncation problem
c	Rev. 2.10 - 06-Jun-1990 Added 0 for special translation position
c	Rev. 2.11 - 30-Oct-1990 Fix for auto translation on VB tapes
c	Rev. 2.12 - 06-Dec-1990 Fix for segmented records (jwm)
c				Added UNH mods for batch work
c	Rev. 2.13 - 12-Jul-1991 Added AS and EB commands
c	Rev. 2.14 - 17-Jul-1991 Added DS command
c	Rev. 2.15 - 01-Oct-1991 Added error messages on file open
c	Rev. 2.16 - 21-Dec-1993 Minor changes from Rollo.Ross@UniSA.Edu.Au
c	Rev. 2.17 - 03-Feb-1994 Added initial support for writing IBM VB
c	Rev. 2.18 - 17-Apr-1994 Added support for reading V, VR, VS and U
c	Rev. 2.19 - 29-Apr-1994 AXP alignment, fixed VS and VR
c	Rev. 2.20 - 29-Nov-1994 Allow writing of multi-volume tapes
c	Rev. 2.21 - 07-Dec-1994 Replaced COB$ calls with native code
c	Rev. 2.22 - 06-Mar-1995 Added support for multiple volume labels on VL
c				(Don Vickers)
c	Rev. 2.23 - 08-Mar-1995 Fixed bogus subroutine call for AXP
c	Rev. 2.24 - 17-Mar-1995 Fixed problem with specload
c	Rev. 2.25 - 22-Mar-1995 Fixed block count on multi-volume writing
c	Rev. 2.26 - 28-Mar-1995 Fixes to multivolume logic, labels.
c	Rev. 2.27 - 04-Dec-1996 Fixes to 'U' recfm
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cdec$ ident 'v2.27'
	implicit none
	include '($ssdef)'
	include '($iodef)'
	include '($mtdef)'
	include '($lbrdef)'
	include '($mntdef)'
	include '($fscndef)'
	include '($rmsdef)'
	include '($lnmdef)'
c
	include 'etape_include.for'
	integer*4   i,icode,iread,irec,nc,ib,ie
	integer*4   num, bf, ios, b_off, e_off,devlen2, fldflags
	integer*4   devtype, oblk, file_context,setmode(2)
	integer*4   lnm_attributes,lnm_max_index
	integer*4   fnum,rmssts,rmsstv,iunit,condval
	character   req*2, rest*80, sysmsg*80, infile*80, dataset*17
	character   label*80,device2*80, auxout*80
	character*6 volnames(8), temp
	integer*2   msglen, retlen
	byte        ttype
	integer*4   sys$assign,sys$qiow,specload, istty
	integer*4   sys$trnlnm, sys$dassgn, sys$mount, sys$filescan
	integer*4   lib$sys_trnlog,cli$present,cli$get_value
	integer*4   lib$find_file, lib$find_file_end, lib$stop
	integer*4   lbr$open,lbr$ini_control,lbr$get_help
	external    cli$_present,cli$_absent,cli$_negated
	external    sys$filescan

	structure /itmlst2/
	    union
		map
		    integer*2  component_length
		    integer*2 item_code
		    integer*4  component_address
		end map
		map
		    integer*4 terminator
		end map
	    end union
	end structure

        record /itmlst2/ filescan_list(3)

	record /itmlst/ lnm_itmlst(5)
c
c format statements
c
1001	format(i4.4)
1002	format(i5.5)
1003	format(i6.6)
1004	format(a)
1005	format('Parameter invalid:',i3,' <',a,'>')
1006	format('SYS$QIOW returned error code:',z8)
1007	format('SYS$QIOW illegal status:',z8)
1008	format('Help error:',z8)
1009	format('Read error on file ',i)
1010	format(bn,i8)
1011	format('Max blocksize is',i,' default lrecl is',i)
1012	format('Short block of ',i,' read for block number ',i)
1013	format(i,' records read from file,',i,' blocks written to tape')
1014	format('Tape positioned at file number ',i,' block ',i)
1015	format('Input record # ',i,' truncated to len ',i6)
1016	format(1x,i,' blocks written')
1017	format('Blocksize is set at ',i,' and record length is ',i)
1018	format('AT_END     ==> ',a,' (',i,')')
1019	format(1x,i3.3,':',i4.4,' ==> TTY')
1020	format(1x,i3.3,':',i4.4,' ==> ',a,' (',i,')')
1021	format(q,a)
1022	format(a3,i3.3)

c
c setup
c
	ttyin=5
	fileout=1
	filein=2
	filespec=3
	logname='TAPE'
	log_len = 4
	at_begin=.true.		! we start at the appropriate end of the tape
	at_end=.false.		! and not here
	file=1			! positioning information
	block=1
	volnum = 1
	eofcnt = 0
	blocksize=MAXBLOCK	! until told otherwise
	lrecl=80		! most common value
	bf=blocksize/lrecl	! blocking factor
	errproc=.true.		! assume we want standard error processor
	filename='<TTY>'	! no filename, so display info
	to_tty=.true.		! initially, all output to terminal
	display = .true.	! initially update screen
	nout=0			! number of records written
	density = 0		! Make no assumptions about density
	labeled = .true.	! Assume tape will be labeled
	volname = ' '		! Assume no volume name
	mode = ebcdic		! will translate until told otherwise
	recfm = 'FB'		! Assume fixed blocked records
	doing_special = .false.	! Assume no special translation
	dataset = ' '		! Assume no dataset name forced
	specfile = '<none>'	! No special translation file
	incr_file = .true.	! assume file incrementation
	getoutnow = .false.	! set up for out-of-band AST

	lnm_itmlst(1).buffer_len	= 4
	lnm_itmlst(1).item_code		= lnm$_attributes
	lnm_itmlst(1).buffer_address	= %loc(lnm_attributes)
	lnm_itmlst(1).ret_len_adr	= 0

	lnm_itmlst(2).buffer_len	= 4
	lnm_itmlst(2).item_code		= lnm$_length
	lnm_itmlst(2).buffer_address	= %loc(devlen)
	lnm_itmlst(2).ret_len_adr	= 0

	lnm_itmlst(3).buffer_len	= 4
	lnm_itmlst(3).item_code		= lnm$_max_index
	lnm_itmlst(3).buffer_address	= %loc(lnm_max_index)
	lnm_itmlst(3).ret_len_adr	= 0

	lnm_itmlst(4).buffer_len	= 80
	lnm_itmlst(4).item_code		= lnm$_string
	lnm_itmlst(4).buffer_address	= %loc(device)
	lnm_itmlst(4).ret_len_adr	= 0

	lnm_itmlst(5).end_list		= 0

	mount_itemlist(1).buffer_len     = phyname_len
	mount_itemlist(1).item_code      = mnt$_devnam
	mount_itemlist(1).buffer_address = %loc(phyname)
	mount_itemlist(1).ret_len_adr    = 0

	mount_itemlist(2).buffer_len     = 4
	mount_itemlist(2).item_code      = mnt$_flags
	mount_itemlist(2).buffer_address = %loc(mount_flags)
	mount_itemlist(2).ret_len_adr    = 0

	mount_itemlist(3).buffer_len     = len(comment)
	mount_itemlist(3).item_code      = mnt$_comment
	mount_itemlist(3).buffer_address = %loc(comment)
	mount_itemlist(3).ret_len_adr    = 0

	mount_itemlist(4).buffer_len     = 0
	mount_itemlist(4).item_code      = 0
	mount_itemlist(4).buffer_address = 0
	mount_itemlist(4).ret_len_adr    = 0

	device  = ' '
	device2 = ' '

	retcode = sys$trnlnm (
	1	 lnm$m_case_blind, 
	1	 'LNM$FILE_DEV', 
	1	 'SYS$DISK',, 
	1	 lnm_itmlst)

	if(.not.retcode) then
	    call outscr(success,'Unable to determine current directory')
	    device = 'SYS$DISK:'
	    devlen = 9
	    go to 50
	end if
	if(((lnm_attributes.and.lnm$m_concealed) .ne. 0) .or.
	1   (lnm_max_index .ne.0)) then
	    device = 'SYS$DISK:'
	    devlen = 9
	    go to 50
	end if

	if(index(device,':') .ne.0) devlen = min(devlen,index(device,':')-1)

	lnm_itmlst(2).buffer_address	= %loc(devlen2)
	lnm_itmlst(4).buffer_address	= %loc(device2)

	retcode = sys$trnlnm (
	1	 lnm$m_case_blind, 
	1	 'LNM$FILE_DEV', 
	1	 device(1:devlen),, 
	1	 lnm_itmlst)

	if(.not.retcode) then
	    lnm_max_index = 0
	end if
	devlen = devlen + 1
	device(devlen:devlen) = ':'

	if(lnm_max_index .ne.0) then
	    go to 50
	end if

	call sys$setddir( , i, device(devlen+1:))
	devlen = devlen + i

0050	continue
c	if we have reached here, we should have a valid default device name

        filescan_list(1).item_code = fscn$_name
        filescan_list(2).item_code = fscn$_type
	filescan_list(3).terminator = 0

C*UNH   First thing, determine which mode we're working in.
C*UNH   Default is Command Mode if SYS$INPUT is *not* a 
C*UNH   terminal, but we allow user to over-ride.
C*UNH   ===================================================

	retcode = cli$present('command')
	if (retcode .eq. %loc(cli$_absent)) then
	    command_mode = (.not. istty('SYS$INPUT'))
	else if (retcode .eq. %loc(cli$_present)) then
	    command_mode = .true.
	else if (retcode .eq. %loc(cli$_negated)) then
	    command_mode = .false.
	else 
	    retcode = lib$stop(%val(retcode)) ! We must be out of sync with our
	endif				       ! CLD file.

c	Open the help library for possible use
	retcode=LBR$INI_CONTROL(lib_index,LBR$C_READ,,)
	if(.not.retcode)go to 9080
	retcode=LBR$OPEN(lib_index,'SYS$HELP:HELPLIB.HLB',,,,,)
	if(.not.retcode)go to 9090

	if(.not.command_mode) then
	    call LIB$SCREEN_INFO(ttype,devtype,line_width,lpp) ! get terminal info
	    call bldscr
	    call enable_out_of_band_smg
	else
	    line_width = 80	! Make assumption
	    call outscr(success,'ETAPE Foreign Tape Conversion -  '//
	1	'Rev. 2.27 - 04-Dec-1996')
	    call enable_out_of_band_line
	end if
c
c Make sure he has assigned 'TAPE:', assign channel for tape, then rewind it
c

	if(cli$present('DEVICE')) then
	    retcode=cli$get_value('DEVICE',logname,log_len)
	    end if
	retcode=lib$sys_trnlog(logname(1:log_len),phyname_len,phyname,,,)
	mount_itemlist(1).buffer_len     = phyname_len
	retcode=SYS$ASSIGN(logname(1:log_len),channel,,)
	if(retcode.ne.SS$_NORMAL)go to 9000
	retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND),iosb,,,,,,,,)
	at_begin = .true.
	at_end = .false.
	eofcnt = 0
c	Determine density
	retcode=SYS$QIOW(,%val(channel),%val(io$_sensemode),iosb,,,,,,,,)
	iosb4(2) = (iosb4(2).and.mt$m_density)/2**mt$v_density
	if(iosb4(2).eq.mt$k_gcr_6250) then
	    density = 6250
	else if (iosb4(2).eq.mt$k_pe_1600) then
	    density = 1600
	else if (iosb4(2).eq.mt$k_nrzi_800) then
	    density = 800
	end if

c	Read the first block to see if we can determine anything
c	about this tape.
	retcode=SYS$QIOW(,%val(channel),%val(IO$_READLBLK),iosb,,,
	1   %ref(buffer(1:1)),%val(65535),,,,)
	if (status .eq. ss$_parity) then
	    call outscr(info,'Tape appears to be blank.')
	    mode = ebcdic
	    buffer = ' '
	else
	    call check_status(*9500,*8900)
	    call translate(buffer,record,bigcount)
	    if(buffer(1:4).eq.'VOL1') then
		volname = buffer(5:10)
		mode = ascii
	    else if(record(1:4).eq.'VOL1') then
		volname = record(5:10)
	    else
		mode = gcosbcd
		call translate(buffer,buffer,bigcount)
		if(buffer(1:2).eq.'GE') then
		    volname = buffer(20:24)
		    blocksize = maxblock
		else
		    mode = ebcdic
		end if
	    end if
	endif

c	Rewind the tape again
	retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND),iosb,,,,,,,,)
	at_begin = .true.
	at_end = .false.
	eofcnt = 0

c begin of main loop. read a request from the current input and branch
c depending on what user wants.

	if(command_mode) go to 8900	!Start off with a prompt

0100	continue
	call inscr(req,rest)

	if(req.eq.'AS') go to 4820	! ASCII (force)
	if(req.eq.'AT') go to 6000	! Auto-translate
	if(req.eq.'BB') go to 0400	! Backspace block
	if(req.eq.'BF') go to 0800	! Backspace file
	if(req.eq.'BL') go to 2200	! Block length (set)
	if(req.eq.'DE') go to 6400	! Density (set)
	if(req.eq.'DS') go to 6800	! Dataset name (specify for writing)
	if(req.eq.'EB') go to 4810	! EBCDIC (force)
	if(req.eq.'EX') go to 2400	! Exit (synonym for quit)
	if(req.eq.'FI') go to 1200	! Filename (set)
	if(req.eq.'GT') go to 5900	! GCOS translation (set)
	if(req.eq.'HE') go to 1600	! Help
	if(req.eq.'LR') go to 2000	! Logical record length (set)
	if(req.eq.'OL') go to 6200	! Output label toggle (set)
	if(req.eq.'QU') go to 2400	! Quit
	if(req.eq.'RE') go to 6700	! Reset EOF indicator
	if(req.eq.'RF') go to 6300	! Record format (set)
	if(req.eq.'RW') go to 2800	! Rewind
	if(req.eq.'SB') go to 3200	! Skip block
	if(req.eq.'SD') go to 6600	! Set default (device and directory)
	if(req.eq.'SF') go to 3600	! Skip file
	if(req.eq.'ST') go to 6500	! Special translation
	if(req.eq.'TB') go to 4000	! Translate block
	if(req.eq.'TF') go to 4400	! Translate file
	if(req.eq.'TM') go to 5800	! Write tapemark
	if(req.eq.'UT') go to 4800	! Untranslate (toggle translate switch)
	if(req.eq.'VL') go to 6100	! Volume label (set)
	if(req.eq.'WH') go to 5200	! Print detailed statistics
	if(req.eq.'WR') go to 5600	! Write file
	go to 9020	! undefined response

c	Note, next routine will be 6900

c
c backspace tape specified number of blocks
c this will not backspace past a tape mark, or from end of tape
c
0400	if(at_end.or.at_begin)go to 9060
	read(rest,1010,IOSTAT=ios,err=9030)num
	num=max(1,num)
	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPRECORD),iosb,,,
	1   %val(-num),,,,,)
c*******************************************************************************
c	Replaced with jwm's code
c	call check_status(*9500,*8900)
c
c
c if end_of_file condition, then step the controller forward past the
c tape mark we just skipped, so we are positioned at the same file
c we started at. this is not necessary if end_of_tape condition is
c encountered; here we assume we have just done a rewind
c
c	if(status.eq.SS$_ENDOFTAPE)then
c		at_begin=.true.
c		file=1
c		block=1
c	elseif(status.eq.SS$_ENDOFFILE)then
c		num=1
c		incr_file=.false.	! we aren't really skipping a file
c		go to 3300  ! branch to skip forward routine
c	else
c		block=max(block-bigcount,1)
c	endif
c	at_end=.false.
c	go to 8900
c
c backspace specified number of files. No action if at beginning of tape,
c otherwise position at beginning of last file backspaced over. Note that
c if you backspace 1 file, this puts you at the beginning of the current
c file, not at the previous file.
c
c 0800	if(at_begin)go to 9060	! can't go back any further
c	read(rest,1010,IOSTAT=ios,err=9030)num
c	num=max(1,num)
c	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
c	1   %val(-num),,,,,)
c
c	call check_status (*9500,*8900)
c
c check status
c
c	if(status.eq.SS$_ENDOFTAPE)then
c	    file=1
c	    block=1
c	    at_begin=.true.
c	    at_end=.false.
c	    go to 8900
c	    endif
c	file=max(file-num+1,1)
c	block=0
c	num=1
c	incr_file=.false.	! not really skipping files
c	go to 3300  ! to advance past tape mark
c	End of replaced code
c*******************************************************************************
	if (status .eq. ss$_endoffile) then                          !jwm
	    eofcnt = 0                                               !jwm
	    block=1                                                  !jwm
	    num = 1                                                  !jwm
	incr_file=.false.       ! not really skipping files          !jwm
	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPRECORD),iosb,,, !jwm
	1   %val(num),,,,,)                                          !jwm
	call check_status(*9500,*8900)                               !jwm
	block = 1                                                    !jwm
	at_begin=.false.                                             !jwm
	incr_file=.true.        !reset for further processing         jwm
	if(command_mode) then                                        !jwm
	    go to 8900                                               !jwm
	else                                                         !jwm
	    go to 100                                                !jwm
	end if                                                       !jwm
	else                                                         !jwm
	    call check_status(*9500,*8900)                           !jwm
	endif                                                        !jwm
c
c
c if end_of_file condition, then step the controller forward past the
c tape mark we just skipped, so we are positioned at the same file
c we started at. this is not necessary if end_of_tape condition is
c encountered; here we assume we have just done a rewind
c
	if(status.eq.SS$_ENDOFTAPE)then
	    at_begin=.true.
	    file=1
	    block=1
	else
	    block=max(block-bigcount,1)
	endif
	at_end=.false.
	go to 8900
c
c backspace specified number of files. No action if at beginning of tape,
c otherwise position at beginning of last file backspaced over. Note that
c if you backspace 1 file, this puts you at the beginning of the current
c file, not at the previous file.
c
0800	if(at_begin)go to 9060  ! can't go back any further
	read(rest,1010,IOSTAT=ios,err=9030)num
	num=max(1,num)
	if (file .eq. 1 .or. (file - num) .lt. 2) then      !jwm
	    go to 2800			! change to REWIND
	endif                                               !jwm
	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
	1   %val(-num),,,,,)
 
	call check_status (*9500,*8900)
c
c check status
c
	if(status.eq.SS$_ENDOFTAPE)then
	    file=1
	    block=1
	    at_begin=.true.
	    at_end=.false.
	    go to 8900
	    endif
	file=max(file-num+1,1)
	num=1
	incr_file=.false.       ! not really skipping files
c       go to 3300      ! to advance past tape mark         jwm
c       * The following code was changed.            *
c       * It was moved from label 3300 and modified. *
	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPRECORD),iosb,,,    !jwm
	1   %val(num),,,,,)                                             !jwm
	call check_status(*9500,*8900)                                  !jwm
	block = 1                                                       !jwm
	at_begin=.false.                                                !jwm
	incr_file=.true.        !reset for further processing            jwm
	if(command_mode) then                                           !jwm
	    go to 8900                                                  !jwm
	else                                                            !jwm
	    go to 100                                                   !jwm
	end if                                                          !jwm
c************** end of jwm changes ******************************************

c
c read a file name and open it. First close any other file that is
c open on unit fileout. If no parameter specified, or error occurs,
c send output to tty.
c
1200	if(.not.to_tty)close(fileout)
	if(rest.eq.' ')then
		to_tty=.true.
		filename='<TTY>'
	else
		filename=rest
		call validate_filename
		open(unit=fileout,name=device(1:devlen)//filename,
	1	    carriagecontrol='LIST',iostat=icode,
	1	recl=maxrec,organization='sequential',status='NEW',err=9070)
		to_tty=.false.
		nout=0
	endif
	go to 8900
c
c print help messages
c
1600	continue		    ! Give help
	if(.not.command_mode) then
	    call smg$save_physical_screen(pasteboard,i,,)
	    end if
	if(rest.eq.' ')then	! general help requested
	    status=LBR$GET_HELP(lib_index,,,,'ETAPE')
	else
	    status=LBR$GET_HELP(lib_index,,,,'ETAPE',rest(1:2))
	endif
	if(.not.status) call outscr(info,
	1   'Help is not available on that topic')
	if(.not.command_mode) then
	    write(*,*)'Press return to continue'
	    read(*,1001,iostat=status)sysmsg
	    call smg$restore_physical_screen(pasteboard,i)
	    end if
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if
c
c Specify logical record length. Need not be a divisor of blocksize.
c
2000	read(rest,1010,IOSTAT=ios,err=9030)lrecl
	if(lrecl.gt.MAXREC)then
	    lrecl=80
	    write(ttyout,1011)MAXREC,lrecl
	    call outscr(success,ttyout)
	endif
	if(lrecl.le.0)then
	    call outscr(success,'Record length set to minimum of 1')
	    lrecl=1
	endif
	if(command_mode) then
	    write(ttyout,1017)blocksize,lrecl
	    call outscr(success,ttyout)
	    go to 8900
	else
	    go to 100
	end if
c
c specify blocksize.
c
2200	read(rest,1010,IOSTAT=ios,err=9030)blocksize
	if(blocksize.gt.MAXBLOCK)then
	    blocksize=MAXBLOCK
	    write(ttyout,1011)MAXBLOCK,lrecl
	    call outscr(success,ttyout)
	endif
	if(blocksize.lt.18)then
	    call outscr(success,'Minimum blocksize is 18 bytes')
	    blocksize=18
	    endif
	if(lrecl.gt.blocksize)lrecl=blocksize	! that wouldn't do at all !
	if(command_mode) then
	    write(ttyout,1017)blocksize,lrecl
	    call outscr(success,ttyout)
	    go to 8900
	else
	    go to 100
	end if
c
c Quit and unload the tape offline and close any open files
c
2400	if(.not.to_tty)close(fileout)
	retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND.or.IO$M_NOWAIT)
	1   ,iosb,,,,,,,,)
	call delscr
	go to 9999
c
c rewind tape, but don't unload
c
2800	retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND),iosb,,,,,,,,)
	if(retcode.ne.SS$_NORMAL)go to 9040
	file=1
	block=1
	at_begin=.true.
	at_end=.false.
	eofcnt = 0
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if
c
c skip blocks in forward direction. Stop at end of file or tape or volume
c and in that case, stay positioned at beginning of next file (if any).
c statement 3300 can also be reached from portions of code that do the
c backspacing, as backspacing can put you before a tapemark.
c
3200	if(at_end)go to 8900
	read(rest,1010,IOSTAT=ios,err=9030)num
	num=max(1,num)
	incr_file=.true.
3300	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPRECORD),iosb,,,
	1   %val(num),,,,,)
	incr_file=.true.
	call check_status(*9500,*8900)
	block=block+bigcount
	at_begin=.false.
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if
c
c skip files in a forward direction
c
3600	if(at_end)go to 8900
	read(rest,1010,IOSTAT=ios,err=9030)num
	num=max(1,num)
	retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
	1   %val(num),,,,,)
	call check_status(*9500,*8900)
	file=file+bigcount
	block=1
	at_begin=.false.
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if
c
c translate blocks. '*' inplies all remaining blocks in this file. Note
c that we go by tape blocks, not logical records as specified by lrecl.
c
4000	if(at_end)go to 8900
	if(rest.eq.'*')then
	    num=99999999
	else
	    read(rest,1010,IOSTAT=ios,err=9030)num
	    num=max(1,num)
	endif
c
c loop on files
c
	do i=1,num
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_READLBLK),iosb,,,
	1	%ref(buffer(1:1)),%val(blocksize),,,,)
	    call check_status(*9500,*8900)
	    if(mode.eq.gcosbcd) then
		continue
	    else if((bigcount.lt.blocksize).and.(recfm.eq.'FB'))then
		write(ttyout,1012)bigcount,block
		call outscr(success,ttyout)
	    endif
	    block=block+1
	    at_begin=.false.
c
c Deblock (with translation) and write the output
c
	    call deblock
	    end do
	go to 8900
c
c translate consecutive files and merge to output
c
4400	if(at_end)go to 8900
	if(rest.eq.'*')then
	    num=99999999
	else
	    read(rest,1010,IOSTAT=ios,err=9030)num
	    num=max(1,num)
	endif
c
c loop on files
c
	do i=1,num
	    call translate_file(*9500,*8900)
	    end do
	go to 8900
c
c change translation (invert)
c
4800	if (mode.eq.ebcdic) then
	    mode = ascii
	    if(command_mode) call outscr(success,'Tape mode is ASCII')
	else
	    mode = ebcdic
	    if(command_mode) call outscr(success,'Tape mode is EBCDIC')
	end if
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if

4810	mode = ebcdic
	if(command_mode) call outscr(success,'Tape mode is EBCDIC')
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if

4820	mode = ascii
	if(command_mode) call outscr(success,'Tape mode is ASCII')
	if(command_mode) then
	    go to 8900
	else
	    go to 100
	end if
c
c print detailed statistics
c
5200	write(ttyout,1014)file,block
	call outscr(success,ttyout)
	if(to_tty)then
	    call outscr(success,'Output currently going to TTY')
	else
	    call outscr(success,'Output currently going to '//
	1	device(1:devlen)//filename)
	    write(ttyout,1016)nout
	    call outscr(success,ttyout)
	endif
	call outscr(success,'Default directory is '//device(1:devlen))
	write(ttyout,1017)blocksize,lrecl
	call outscr(success,ttyout)
	if(mode.eq.ebcdic)then
	    call outscr(success,'Mode is EBCDIC')
	else if (mode.eq.ascii) then
	    call outscr(success,'Mode is ASCII')
	else
	    call outscr(success,'Mode is GCOS BCD')
	endif
	if(recfm.eq.'FB') then
	    call outscr(success,'Record format is Fixed')
	else if(recfm.eq.'D ') then
	    call outscr(success,'Record format is ANSI Variable')
	else if(recfm.eq.'S ') then
	    call outscr(success,'Record format is ANSI Segmented')
	else if(recfm.eq.'V ') then
	    call outscr(success,'Record format is IBM Variable')
	else if(recfm.eq.'VB') then
	    call outscr(success,'Record format is IBM Var. Blocked')
	else if(recfm.eq.'VS') then
	    call outscr(success,'Record format is IBM Var. Spanned')
	else if(recfm.eq.'VR') then
	    call outscr(success,'Record format is IBM VBS')
	else if(recfm.eq.'U ') then
	    call outscr(success,'Record format is IBM Undefined')
	else
	    call outscr(success,'Record format is Unknown')
	end if
	if(doing_special) then
	    call outscr(success,'Special translation from file '//specfile)
	else
	    call outscr(success,'No special translation')
	end if

	go to 8900
c
c Write a file to tape. Block the records according to the current values
c of lrecl and blocksize. Translate to ebcdic if desired.
c
5600	continue
	retcode = lib$find_file_end (file_context)
	if(mode.eq.gcosbcd) then
	    call outscr(error,'Writing is not supported in GBCD mode')
	    go to 8900
	    end if
	if((recfm.ne.'FB') .and. (recfm.ne.'VB')) then
	    call outscr(error,'Only FB and VB record formats supported')
	    go to 8900
	    end if

c	If we are at the end of the (non-empty) tape, we must backspace
c	to before the final tape mark.

	if (at_end .and. (file .gt. 1)) then
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
	1	%val(-1),,,,,)
 	    call check_status (*9500,*8900)
	end if

5610	retcode = lib$find_file (rest, infile, file_context,,,,)
	if(retcode.eq.rms$_nmf) then
	    call outscr(success,'No more files to process')
c	    Assume this is the last file, and write another tape mark.  This
c	    will be backspaced if additional files are written.
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9100
	    go to 8900
	else
	    call outscr(success,'Writing file '//infile)
	    end if
	open(unit=filein,name=infile,status='OLD',err=9070,iostat=icode,
	1   access='SEQUENTIAL',readonly)

	at_end = .false.

	call str$trim(infile,infile,i)
	retcode = sys$filescan(infile(1:i), filescan_list, fldflags,
	1   auxout, retlen)

	b_off = filescan_list(1).component_address - %loc(auxout) + 1
	e_off = b_off + filescan_list(1).component_length +
	1   filescan_list(2).component_length - 1

c	Set the proper tape density.  If the user hasn't changed it, this
c	doesn't accomplish anything.
	if(at_begin) then
	    retcode=SYS$QIOW(,%val(channel),%val(io$_sensemode),iosb,,,,,,,,)
	    setmode(1) = iosb4(1)
	    setmode(2) = iosb4(2) .and. (.not. mt$m_density)
	    if(density.eq.6250) then
		setmode(2) = setmode(2) + (mt$k_gcr_6250 * 2**mt$v_density)
	    else if(density.eq.1600) then
		setmode(2) = setmode(2) + (mt$k_pe_1600 * 2**mt$v_density)
	    else if(density.eq.800) then
		setmode(2) = setmode(2) + (mt$k_nrzi_800 * 2**mt$v_density)
	    else
c		If you don't understand it, don't mess with it.
		setmode(2) = iosb4(2)
	    end if

	    retcode=SYS$QIOW(,%val(channel),%val(io$_setmode),iosb,,,
	1	setmode,,,,,)
	    if(retcode.ne.ss$_normal) then
		call outscr(success,'Unable to set output density.')
		call outscr(error,'Write command terminated.')
		go to 8900
		end if
	    end if

	if(labeled) then			! Do the labels
	    if(file.eq.1) then			! Need a VOL1 label
c		Assemble and write VOL1
		label(01:04) = 'VOL1'		! Identifier
		label(05:10) = volname		! Volume serial number
		label(11:11) = '0'		! Reserved
		label(12:21) = ' '		! VTOC pointer (not used)
		label(22:31) = ' '		! Reserved
		label(32:41) = ' '		! Reserved
		label(42:51) = ' '		! Owner name & address
		label(52:80) = ' '		! Reserved
		call untranslate(label,80)
		retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1	    %ref(label(1:1)),%val(80),,,,)
		if(retcode.ne.SS$_NORMAL)go to 9040
		end if
	    at_begin = .false.
c	    Assemble and write HDR1
	    label(01:04) = 'HDR1'		! Identifier
	    if (dataset .ne. ' ') then
		label(05:21) = dataset		! Data set identifier
	    else
		label(05:21) = auxout(b_off:e_off)
	    endif
	    label(22:27) = volname		! Data set serial number
	    write(label(28:31),1001) volnum	! Volume sequence number
            write(label(32:35),1001)file	! Data set sequence number
	    label(36:39) = '    '		! Generation number
	    label(40:41) = '  '			! Generation version number
	    label(42:42) = ' '			! Creation date 1st char
	    call julday(label(43:47))		! Creation date
	    label(48:53) = ' 00000'		! Expiration date
	    label(54:54) = '0'			! Data set security
	    label(55:60) = '000000'		! Block count
	    label(61:73) = 'VMS ETAPE'		! System code
	    label(74:80) = '       '		! Reserved
	    call untranslate(label,80)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1	%ref(label(1:1)),%val(80),,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9040
c	    Assemble and write HDR2
	    label(01:04) = 'HDR2'		! Identifier
	    label(05:05) = recfm(1:1)		! Record format
	    write(label(06:10),1002)blocksize	! Block length
	    write(label(11:15),1002)lrecl	! Record length
	    label(16:16) = ' '			! Density ( Not applied!!)
	    if (density .eq. 800) then
		label(16:16) = '2'
	    else if (density .eq. 1600) then
		label(16:16) = '3'
	    else if (density .eq. 6250) then
		label(16:16) = '4'
	    end if
	    label(17:17) = '0'			! Data set position
	    label(18:34) = ' '			! Job/job step identification
	    label(35:36) = '  '			! Tape recording technique
	    label(37:37) = ' '			! Control characters
	    label(38:38) = ' '			! Reserved
	    label(39:39) = recfm(2:2)		! Block attribute
	    label(40:80) = ' '			! Reserved
	    call untranslate(label,80)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1	%ref(label(1:1)),%val(80),,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9040

c	    write a tape mark.
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9100
c	    file = file + 1			    ! 2.26
	end if

	iread=0	! count of records read from file
	oblk=0	! count of tape blocks written
	irec=0	! count of records in this block
	if (recfm .eq. 'FB') then
	    sofar=0	    ! count of bytes in this block so far
	else
	    sofar=4
	endif


5700	read(filein,1021,end=5750,err=9110)nc,record(1:nc)
	irec=irec+1
	iread=iread+1
	if (recfm .eq. 'FB') then
	    if(nc.lt.lrecl)then	! pad record with blanks
		record(nc+1:lrecl)=' '
	    nc = lrecl
	    endif
	endif

	if (nc.gt.lrecl)then	! truncate record & print message
	    write(ttyout,1015)iread,lrecl
	    call outscr(info,ttyout)
	endif
c
c Check if this record will fit in current block. If it won't, then
c output the current block to the tape.
c
	if (((recfm .eq. 'FB') .and. ((lrecl + sofar) .gt. blocksize)) .or.
	1   ((recfm .ge. 'VB') .and. ((nc + sofar + 4) .gt. blocksize))) then
	    if(recfm .eq. 'VB') then
		buffer(1:1) = char(sofar/256)
		buffer(2:2) = char(mod(sofar,256))
		buffer(3:3) = char(0)
		buffer(4:4) = char(0)
	    endif
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1	%ref(buffer(1:1)),%val(sofar),,,,)

	    block = block + 1
	    oblk=oblk+1

c	Check for end of physical volume
	    if(iosb(1).eq.SS$_ENDOFTAPE) then
		call outscr(success,'End of volume '//char(volnum+48)//
	1	    ' encountered')

c		write a tape mark
		retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
		if(retcode.ne.SS$_NORMAL)go to 9100
c		file = file + 1				! 2.26

		if(labeled) then			! Do the labels
c		    Assemble and write EOV1
		    label(01:04) = 'EOV1'		! Identifier
		    if (dataset .ne. ' ') then
			label(05:21) = dataset		! Data set identifier
		    else
			label(05:21) = auxout(b_off:e_off)
		    endif
		    label(22:27) = volname		! Data set serial number
		    write(label(28:31),1001) volnum	! Volume sequence number
      		    write(label(32:35),1001)file	! Data set sequence number
		    label(36:39) = '    '		! Generation number
		    label(40:41) = '  '			! Generation version number
		    label(42:42) = ' '			! Creation date 1st char
		    call julday(label(43:47))		! Creation date
		    label(48:53) = ' 00000'		! Expiration date
		    label(54:54) = '0'			! Data set security
c		    label(55:60) = '000000'		! Block count
	            write(label(55:60),1003)oblk	!++2.26 Block count
		    label(61:73) = 'VMS ETAPE'		! System code
		    label(74:80) = '       '		! Reserved
		    call untranslate(label,80)
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1		%ref(label(1:1)),%val(80),,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9040
c		    Assemble and write EOV2
		    label(01:04) = 'EOV2'		! Identifier
		    label(05:05) = recfm(1:1)		! Record format
		    write(label(06:10),1002)blocksize	! Block length
		    write(label(11:15),1002)lrecl	! Record length
		    label(16:16) = ' '			! Density ( Not applied!!)
		    if (density .eq. 800) then
			label(16:16) = '2'
		    else if (density .eq. 1600) then
			label(16:16) = '3'
		    else if (density .eq. 6250) then
			label(16:16) = '4'
		    end if
		    label(17:17) = '1'			! Data set position
		    label(18:34) = ' '			! Job/job step identification
		    label(35:36) = '  '			! Tape recording technique
		    label(37:37) = ' '			! Control characters
		    label(38:38) = ' '			! Reserved
		    label(39:39) = recfm(2:2)		! Block attribute
		    label(40:80) = ' '			! Reserved
		    call untranslate(label,80)
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1		%ref(label(1:1)),%val(80),,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9040

c		    write a tape mark.
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9100
c		    file = file + 1			    ! 2.26
		end if

		write(ttyout,1013)iread,oblk
		call outscr(success,ttyout)

c	Dismount current volume and mount next.
		retcode = sys$dassgn(%val(channel))
		call sys$dismou(logname(1:log_len),)
		volnum = volnum + 1
c-2.22		comment = 'Please mount relative volume '//char(volnum+48)//
c-2.22	1	    ' on device '//phyname(1:phyname_len)
c+++2.22
		temp = volname
		if(volnum.le.8) then
		      volname = volnames(volnum)
		else
		       volname = ' '
		end if
		if(volname(1:1) .eq. ' ') then
		     comment = 'Please mount relative volume '//
	1	       char(volnum+48)// ' on device '//phyname(1:phyname_len)
		     write(volname,1022) temp(1:3),volnum
		else
		     comment = 'Please mount volume '//volname//
	1	       ' on device '//phyname(1:phyname_len)
		end if
c---2.22
		mount_flags = mnt$m_foreign + mnt$m_ovr_access
		call outscr(success,comment)
		retcode = sys$mount(mount_itemlist)
		if(retcode.ne.SS$_NORMAL) call lib$signal(%val(retcode))
		retcode=SYS$ASSIGN(logname(1:log_len),channel,,)
		if(retcode.ne.SS$_NORMAL) call lib$signal(%val(retcode))
		retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND),iosb,,,,,,,,)
		if(retcode.ne.SS$_NORMAL) call lib$signal(%val(retcode))
c+++2.22
		call outscr(success,'Beginning volume '//volname)
		call smg$put_chars(display_id_1,volname,1,39,,,,)
c---2.22
c-2.22		write(volname,1022) volname(1:3),volnum
c-2.22		call outscr(success,'Beginning volume '//char(volnum+48))
		oblk = 0

c		Set appropriate density (same as previous reel(s))
		retcode=SYS$QIOW(,%val(channel),%val(io$_sensemode),iosb,,,,,,,,)
		setmode(1) = iosb4(1)
		setmode(2) = iosb4(2) .and. (.not. mt$m_density)
		if(density.eq.6250) then
		    setmode(2) = setmode(2) + (mt$k_gcr_6250 * 2**mt$v_density)
		else if(density.eq.1600) then
		    setmode(2) = setmode(2) + (mt$k_pe_1600 * 2**mt$v_density)
		else if(density.eq.800) then
		    setmode(2) = setmode(2) + (mt$k_nrzi_800 * 2**mt$v_density)
		else
c		    If you don't understand it, don't mess with it.
		    setmode(2) = iosb4(2)
		end if

		retcode=SYS$QIOW(,%val(channel),%val(io$_setmode),iosb,,,
	1	    setmode,,,,,)
		if(retcode.ne.ss$_normal) then
		    call outscr(success,'Unable to set output density.')
		    call outscr(error,'Write command terminated.')
		    go to 8900
		end if

		if(labeled) then			! Do the labels
c		    Assemble and write VOL1
		    label(01:04) = 'VOL1'		! Identifier
		    label(05:10) = volname		! Volume serial number
		    label(11:11) = '0'		! Reserved
		    label(12:21) = ' '		! VTOC pointer (not used)
		    label(22:31) = ' '		! Reserved
		    label(32:41) = ' '		! Reserved
		    label(42:51) = ' '		! Owner name & address
		    label(52:80) = ' '		! Reserved
		    call untranslate(label,80)
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1		%ref(label(1:1)),%val(80),,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9040
c		    Assemble and write HDR1
		    label(01:04) = 'HDR1'		! Identifier
		    if (dataset .ne. ' ') then
			label(05:21) = dataset		! Data set identifier
		    else
			label(05:21) = auxout(b_off:e_off)
		    endif
		    label(22:27) = volname		! Data set serial number
		    write(label(28:31),1001) volnum	! Volume sequence number
      		    write(label(32:35),1001)file	! Data set sequence number
		    label(36:39) = '    '		! Generation number
		    label(40:41) = '  '			! Generation version number
		    label(42:42) = ' '			! Creation date 1st char
		    call julday(label(43:47))		! Creation date
		    label(48:53) = ' 00000'		! Expiration date
		    label(54:54) = '0'			! Data set security
		    label(55:60) = '000000'		! Block count
		    label(61:73) = 'VMS ETAPE'	    	! System code
		    label(74:80) = '       '		! Reserved
		    call untranslate(label,80)
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1		%ref(label(1:1)),%val(80),,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9040
c		    Assemble and write HDR2
		    label(01:04) = 'HDR2'		! Identifier
		    label(05:05) = recfm(1:1)		! Record format
		    write(label(06:10),1002)blocksize	! Block length
		    write(label(11:15),1002)lrecl	! Record length
		    label(16:16) = ' '			! Density ( Not applied!!)
		    if (density .eq. 800) then
			label(16:16) = '2'
		    else if (density .eq. 1600) then
			label(16:16) = '3'
		    else if (density .eq. 6250) then
			label(16:16) = '4'
		    end if
		    label(17:17) = '0'			! Data set position
		    label(18:34) = ' '			! Job/job step identification
		    label(35:36) = '  '			! Tape recording technique
		    label(37:37) = ' '			! Control characters
		    label(38:38) = ' '			! Reserved
		    label(39:39) = recfm(2:2)		! Block attribute
		    label(40:80) = ' '			! Reserved
		    call untranslate(label,80)
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1		%ref(label(1:1)),%val(80),,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9040

c		    write a tape mark.
		    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
		    if(retcode.ne.SS$_NORMAL)go to 9100
c		    file = file + 1			    ! 2.26
		end if
	    else if (retcode.ne.SS$_NORMAL) then
		go to 9040
	    endif

	    irec=1
	    if (recfm .eq. 'FB') then
		sofar=0
	    else
		sofar=4
	    endif
	    call upscr2
	    endif
c
c Add this record to the current block.
c
	if (recfm .eq. 'FB') then
	    ib = sofar + 1
	    ie = ib + lrecl - 1
	else
	    ib = sofar + 5
	    ie = ib + nc - 1
	endif

	if (recfm .eq. 'VB') then
	    buffer(sofar+1:sofar+1) = char((nc+4)/256)
	    buffer(sofar+2:sofar+2) = char(mod((nc+4),256))
	    buffer(sofar+3:sofar+3) = char(0)
	    buffer(sofar+4:sofar+4) = char(0)
	endif

	buffer(ib:ie)=record(1:nc)

	call untranslate(buffer(ib:ie),nc)
	sofar = ie
	go to 5700
c
c At end of input file, may have to output partial block.
c
5750	continue
	if(recfm .eq. 'VB') then
	    buffer(1:1) = char(sofar/256)
	    buffer(2:2) = char(mod(sofar,256))
	    buffer(3:3) = char(0)
	    buffer(4:4) = char(0)
	endif
	retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1   %ref(buffer(1:1)),%val(sofar),,,,)
	if(retcode.ne.SS$_NORMAL)go to 9040
	oblk=oblk+1
	call upscr2
	write(ttyout,1013)iread,oblk
	call outscr(success,ttyout)
	close(filein)
c	Write a tape mark.
	retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
	if(retcode.ne.SS$_NORMAL)go to 9100
c	file = file + 1				! 2.26
c	Write labels if appropriate
	if(labeled) then			! Do the labels
c	    Assemble and write EOF1
	    label(01:04) = 'EOF1'		! Identifier
	    if (dataset .ne. ' ') then
		label(05:21) = dataset		! Data set identifier
		dataset = ' '			! Clear for next file.
	    else
		label(05:21) = auxout(b_off:e_off)
	    endif
	    label(22:27) = volname		! Data set serial number
	    write(label(28:31),1001) volnum	! Volume sequence number
            write(label(32:35),1001)file	! Data set sequence number
	    label(36:39) = '    '		! Generation number
	    label(40:41) = '  '			! Generation version number
	    label(42:42) = ' '			! Creation date 1st char
	    call julday(label(43:47))		! Creation date
	    label(48:53) = ' 00000'		! Expiration date
	    label(54:54) = '0'			! Data set security
	    write(label(55:60),1003)oblk	! Block count
	    label(61:73) = 'VMS ETAPE'		! System code
	    label(74:80) = '       '		! Reserved
	    call untranslate(label,80)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1	%ref(label(1:1)),%val(80),,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9040
c	    Assemble and write EOF2
	    label(01:04) = 'EOF2'		! Identifier
	    label(05:05) = recfm(1:1)		! Record format
	    write(label(06:10),1002)blocksize	! Block length
	    write(label(11:15),1002)lrecl	! Record length
	    label(16:16) = ' '			! Density (Default blank)
	    if (density .eq. 800) then
		label(16:16) = '2'
	    else if (density .eq. 1600) then
		label(16:16) = '3'
	    else if (density .eq. 6250) then
		label(16:16) = '4'
	    end if
	    label(17:17) = '0'			! Data set position
	    if (volnum .gt. 1) label(17:17) = '1'
	    label(18:34) = ' '			! Job/job step identification
	    label(35:36) = '  '			! Tape recording technique
	    label(37:37) = ' '			! Control characters
	    label(38:38) = ' '			! Reserved
	    label(39:39) = recfm(2:2)		! Block attribute
	    label(40:80) = ' '			! Reserved
	    call untranslate(label,80)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITELBLK),iosb,,,
	1	%ref(label(1:1)),%val(80),,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9040
c	    write a tape mark.
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
	    if(retcode.ne.SS$_NORMAL)go to 9100
	    end if

	block=1
	at_end=.true.
	file=file+1
	go to 5610		! go back to write another file
c
c Entry point for writing a tape mark.
c
5800	retcode=SYS$QIOW(,%val(channel),%val(IO$_WRITEOF),iosb,,,,,,,,)
	if(retcode.ne.SS$_NORMAL)go to 9100
	block=1
	at_end=.true.
	file=file+1
	go to 8900

5900	mode = gcosbcd
	blocksize = maxblock
	if(command_mode) then
	    call outscr(success,'Tape mode is GBCD')
	    go to 8900
	else
	    go to 100
	end if


c
c	Auto-translate function -- process HDR1&HDR2 records from
c	this point on and translate accordingly
c
6000	continue
	if(rest.eq.'*')then
	    num=99999999
	else
	    read(rest,1010,IOSTAT=ios,err=9030)num
	    num=max(1,num)
	end if
	call auto_translate(num,*9500)
	go to 8900

6100	continue				! VL command
c--2.22	volname = rest
c+++2.22
c		Parse the volume label tokens
c
	volnum = 1
	e_off = index(rest,' ')
	rest = rest(:e_off) // ','
6110	continue
	if(rest .eq. ' ') goto 6120
	e_off = index(rest,',')
	b_off = e_off - 1
	e_off = e_off + 1
	if(b_off.gt.6.or.b_off.lt.0) b_off = 6
	volnames(volnum) = rest(:b_off)
	rest = rest(e_off:)
	volnum = volnum + 1
	if(volnum.le.8) goto 6110
6120	continue
	volnum = 1
	volname = volnames(1)
c
c---2.22
	go to 8900

6200	continue				! OL command
	labeled = .not. labeled
	if(labeled) then
	    call outscr(success,'Label mode is ON')
	else
	    call outscr(success,'Label mode is OFF')
	end if
	go to 8900

6300	continue				! RF command
	recfm = rest
	if (recfm .eq. 'F ') recfm = 'FB'
	if( (recfm.ne.'FB').and.(recfm.ne.'D ').and.(recfm.ne.'S ').and.
	1   (recfm.ne.'V ').and.(recfm.ne.'VB').and.(recfm.ne.'VR').and.
	1   (recfm.ne.'VS').and.(recfm.ne.'U ')) then
	    call outscr(success,'Record format '//recfm//' not recognized')
	    call outscr(success,'fixed records assumed')
	    recfm = 'FB'
	end if
	go to 8900

6400	continue				! DE command
	read(rest,1010,iostat=ios,err=9030)num
	if((num.ne.800).and.(num.ne.1600).and.(num.ne.6250)) then
	    call outscr(success,'Invalid density specified, command ignored')
	else
	    density = num
	endif
	go to 8900

6500	continue
	if(rest.eq.' ') then
	    call outscr(success,'Special translation turned OFF')
	    doing_special = .false.
	    specfile = '<none>'
	else
	    specfile = rest
	    retcode = specload()
	    if(retcode.ne.ss$_normal) then
		call outscr(success,'Special translation disabled due to errors')
		doing_special = .false.
		specfile = '<none>'
	    else
		doing_special = .true.
	    end if
	end if
	go to 8900

6600	continue	      
	call str$trim (device, rest, devlen)
	go to 8900

c	Reset EOF indicator
6700	if(.not.at_end) then
	    call outscr(success,'not at EOF')
	end if
	at_end = .false.
	errproc = .false.
	call outscr(success,'Errors will not cause rewind')
	go to 8900

c	Specify dataset name for next file output
 6800	continue
	dataset = rest
	go to 8900
c
c update position and display it in the form:
c fff:bbbb ==> [tty | filename (nout)]
c fff is current file number, bbbb is block number ("AT_END" displayed if
c at end of tape). TTY displayed if output to tty, otherwise the output
c filename and the number of records written to it are displayed.
c
 8900	if(at_end.and.to_tty)then
	    call outscr(success,'AT_END    ==> TTY')
	elseif(at_end.and..not.to_tty)then
	    call str$trim(filename,filename,i)
	    write(ttyout,1018)filename(1:i),nout
	    call outscr(success,ttyout)
	elseif(.not.at_end.and.to_tty)then
	    write(ttyout,1019)file,block
	    call outscr(success,ttyout)
	else
	    call str$trim(filename,filename,i)
	    write(ttyout,1020)file,block,filename(1:i),nout
	    call outscr(success,ttyout)
	endif
	go to 100
c
c errors
c
9000	call outscr(success,'Can''t assign channel for tape.')
	call outscr(success,'Please verify that logical name '//
	1   logname(1:log_len))
	call outscr(success,'is assigned to a valid device.')
	call outscr(fatal,'Current assignment is:'//phyname(1:phyname_len))
	call anykey
	call delscr
	go to 9999
9020	call outscr(info,' Request <'//req//'> not understood.'//
	1   ' Type HE or press <HELP> for help')
	go to 100
9030	write(ttyout,1005)ios,rest
	call outscr(fatal,ttyout)
	go to 100
9040	write(ttyout,1006)retcode
	call outscr(success,ttyout)
9045	call SYS$GETMSG(%val(retcode),msglen,sysmsg,,)
	call outscr(retcode,sysmsg(1:msglen))
	go to 9500
9060	call outscr(fatal,'Illegal operation at beginning or end of tape')
	go to 100
9070	continue
	call errsns (fnum,rmssts,rmsstv,iunit,condval)
	call str$trim(filename,filename,i)
	call outscr(success,'Error opening file:'//filename)
	call outscr(rmssts,' ')
	to_tty=.true.
	go to 8900
9080	write(ttyout,1008)retcode
	call outscr(success,ttyout)
	go to 9095
9090	write(ttyout,1008)retcode
	call outscr(success,ttyout)
9095	call SYS$GETMSG(%val(retcode),msglen,sysmsg,,)
	call outscr(retcode,sysmsg(1:msglen))
	go to 0100
9100	call outscr(warning,'Error writing tapemark')
	go to 9045
9110	write(ttyout,1009)icode
	call outscr(fatal,ttyout)
	go to 5750
c
c common fatal error handler. rewind tape; if successful, he can continue,
c otherwise we might as well stop
c
9500	if(.not.errproc) then
	    call outscr(success,'Tape not rewound.  '//
	1	'Positioning may be invalid.')
	    go to 8900
	end if
	retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND),iosb,,,,,,,,)
	if(retcode.ne.SS$_NORMAL)then
	    call outscr(fatal,'Fatal error, must stop')
	    if(.not.to_tty)close(fileout)
	    call anykey
	    call delscr
	    go to 9999
	else
	    call outscr(success,'Tape rewound...')
	    at_begin=.true.
	    at_end=.false.
	    eofcnt = 0
	    file=1
	    block=1
	    go to 8900
	endif

9999	continue
	end

c subroutine package for use with ETAPE program. includes the following:
c	deblock - breaks logical tape blocks into records
c	translate - translate from ebcdic to ascii
c	untranslate - translate from ascii to ebcdic
c	out_tty - outputs tape record to terminal
c	out_file - outputs tape record to file
c

	subroutine deblock
	implicit none
	include '($dscdef)'
	include 'sys$library:foriosdef.for'
	include 'etape_include.for'
	integer trecl			! lrecl of special translated record
	integer recl			! current logical record length
	integer segtype			! IBM variable segment type
	integer bksiz			! Blocksize for IBM variable
	integer rec			! record number written to tty
	integer ios			! for io status checking
	integer used			! Size (so far) of a segmented record
	integer   i,kl
	record/dscdef1/ null_string

	null_string.dsc$w_maxstrlen = 0
	null_string.dsc$b_dtype     = dsc$k_dtype_t
	null_string.dsc$b_class     = dsc$k_class_s
	null_string.dsc$a_pointer   = 0


1001	format(i4.4)
1002	format(i5.5)

	call translate(buffer,buffer2,bigcount)
	if(mode.eq.gcosbcd) then
	    call gdeblock(bigcount)
	    return
	    end if
	rec=0
c	IBM F or FB
	if(recfm.eq.'FB') then
	    do i=1,bigcount,lrecl
		kl=min(i+lrecl-1,bigcount)
		rec=rec+1
		if(to_tty)then
		    call out_tty(buffer2(i:kl),rec)
		else
		    if(doing_special) then
			call spectrans(buffer(i:kl),buffer2(i:kl),
	1		    record2,trecl)
			call out_file(record2(1:trecl),fileout)
		    else
			call out_file(buffer2(i:kl),fileout)
		    end if
		    nout=nout+1
		endif
		end do
c	ANS Variable
	else if(recfm.eq.'D ') then
	    i = 1		    ! current record position
	    read(buffer2(i:i+3),1001,iostat=ios) recl
	    do while(ios.eq.0)
		rec = rec + 1
		if(to_tty)then
		    if(recl.ne.0) then
			call out_tty(buffer2(i+4:i+recl-1),rec)
		    else
			call out_tty(' ',rec)
		    end if
		else
		    if(recl.ne.0) then
			if(doing_special) then
			    call spectrans(buffer(i+4:i+recl-1),
	1			buffer2(i+4:i+recl-1),record2,trecl)
			    call out_file(record2(1:trecl),fileout)
			else
			    call out_file(buffer2(i+4:i+recl-1),fileout)
			end if
		    else
			call out_file(null_string,fileout)
		    end if
		    nout=nout+1
		endif
	    i = i + recl
	    if(i.le.bigcount-4) then
		read(buffer2(i:i+3),1001,iostat=ios) recl
	    else
		ios = for$ios_enddurrea
	    end if
	    end do

c	Read IBM V, VB, VS, VBS
	else if(recfm(1:1).eq.'V') then	! IBM Variable (all variations)
	    bksiz = ichar(buffer(1:1))*256 + ichar(buffer(2:2))
	    i = 5			! current record position
	    do while(i.lt.bksiz)
		recl = ichar(buffer(i:i))*256 + ichar(buffer(i+1:i+1))
c		obtain the 'type descriptor?'
		segtype = ichar(buffer(i+2:i+2))*256 + ichar(buffer(i+3:i+3))
		if (segtype .eq.0) then	    ! Record is contained in this segment
		    rec = rec + 1
		    if(to_tty)then
			if(recl.ne.0) then
			    call out_tty(buffer2(i+4:i+recl-1),rec)
			else
			    call out_tty(' ',rec)
			end if
		    else
			if(recl.ne.0) then
			    if(doing_special) then
				call spectrans(buffer(i+4:i+recl-1),
	1			    buffer2(i+4:i+recl-1),record2,trecl)
				call out_file(record2(1:trecl),fileout)
			    else
				call out_file(buffer2(i+4:i+recl-1),fileout)
			    end if
			else
			    call out_file(null_string,fileout)
			end if
			nout=nout+1
		    end if
		else if (segtype .eq.256) then	    ! Record is begun in this segment
		    record = buffer2(i+4:i+recl-1)
		    used = recl-4
		    if(used.gt.maxrec) then		! Longer than I handle
			call outscr(success,'Segmented record too long')
			call outscr(success,'Truncated to 32766 characters')
			used = maxrec
		    end if
		else if (segtype .eq.512) then	    ! Record is finished in this segment
		    record(used+1:) = buffer2(i+4:i+recl-1)
		    used = used + recl - 4
		    if(used.gt.maxrec) then		! Longer than I handle
			call outscr(success,'Segmented record too long')
			call outscr(success,'Truncated to 32766 characters')
			used = maxrec
		    end if
		    rec = rec + 1
		    if(to_tty)then
			if(recl.ne.0) then
			    call out_tty(record(1:used),rec)
			else
			    call out_tty(' ',rec)
			end if
		    else
			if(recl.ne.0) then
			    call out_file(record(1:used),fileout)
			else
			    call out_file(null_string,fileout)
			end if
			nout=nout+1
		    end if
		else if (segtype .eq.768) then	    ! Neither beginning nor end
		    record(used+1:) = buffer2(i+4:i+recl-1)
		    used = used + recl - 4
		    if(used.gt.maxrec) then		! Longer than I handle
			call outscr(success,'Segmented record too long')
			call outscr(success,'Truncated to 32766 characters')
			used = maxrec
		    end if
		else
		    write (ttyout,1001)segtype
		    call outscr(success,'Segment type '//ttyout(1:4)//
	1		' not recognized')
		    call outscr(success,'This segment will be ignored')
		end if
		i = i + recl
	    end do
c	********** untested code for segmented records
	else if(recfm.eq.'S ') then
	    i = 1		    ! current record position
	    read(buffer2(i+1:i+4),1002,iostat=ios) recl
	    do while(ios.eq.0)
		if(buffer2(i:i).eq.'0') then		! Same as variable
		    rec = rec + 1
		    if(to_tty)then
			if(recl.ne.0) then
			    call out_tty(buffer2(i+5:i+recl-1),rec)
			else
			    call out_tty(' ',rec)
			end if
		    else
			if(recl.ne.0) then
			    if(doing_special) then
				call spectrans(buffer(i+5:i+recl-1),
	1			    buffer2(i+5:i+recl-1),record2,trecl)
				call out_file(record2(1:trecl),fileout)
			    else
				call out_file(buffer2(i+5:i+recl-1),fileout)
			    end if
			else
			    call out_file(null_string,fileout)
			end if
			nout=nout+1
		    endif
		else if(buffer2(i:i).eq.'1') then	! First of a record
		    record = buffer2(i+5:i+recl-1)
		    used = recl - 5
		    if(used.gt.maxrec) then		! Longer than I handle
			call outscr(success,'Segmented record too long')
			call outscr(success,'Truncated to 32766 characters')
			used = maxrec
		    end if
		else if(buffer2(i:i).eq.'2') then	! Append to record
		    record(used+1:) = buffer2(i+5:i+recl-1)
		    used = used + recl - 5
		    if(used.gt.maxrec) then		! Longer than I handle
			call outscr(success,'Segmented record too long')
			call outscr(success,'Truncated to 32766 characters')
			used = maxrec
			end if
		else if(buffer2(i:i).eq.'3') then	! Terminate record
		    record(used+1:) = buffer2(i+5:i+recl-1)
		    used = used + recl - 5
		    if(used.gt.maxrec) then		! Longer than I handle
			call outscr(success,'Segmented record too long')
			call outscr(success,'Truncated to 32766 characters')
			used = maxrec
			end if
		    rec = rec + 1
		    if(to_tty)then
			if(used.ne.0) then
			    call out_tty(record(1:used),rec)
			else
			    call out_tty(' ',rec)
			end if
		    else
			if(used.ne.0) then
			    if(doing_special) then
				call outscr(success,'Special translate not '//
	1			    'supported on segmented records')
				call outscr(success,'Translation has been disabled')
				doing_special = .false.
				end if
			    call out_file(record(1:used),fileout)
			else
			    call out_file(null_string,fileout)
			end if
			nout=nout+1
		    end if
		end if
c           i = i + recl + 5    !5 dec 1990  jwm
            i = i + recl
	    if(i.le.bigcount-5) then
		read(buffer2(i+1:i+4),1001,iostat=ios) recl
	    else
		ios = for$ios_enddurrea
	    end if
	    end do
	    if(ios.eq.for$ios_inpconerr.and.buffer2(i:i+4).ne.'^^^^^') then
		call outscr(success,'Error processing segmented record')
		call outscr(success,'Text was:'//buffer2(i:i+4))
		getoutnow = .true.
		end if
	else if(recfm.eq.'U ') then
	    if(to_tty)then
		if(bigcount.ne.0) then
		    call out_tty(buffer2(1:bigcount),rec)
		else
		    call out_tty(' ',rec)
		end if
	    else
		if(bigcount.ne.0) then
		    call out_file(buffer2(1:bigcount),fileout)
		else
		    call out_file(null_string,fileout)
		end if
		nout=nout+1
	    endif
	end if
	call upscr2
	return
	end

	subroutine translate(trabufi,trabufo,tracount)
	implicit none
	include 'etape_include.for'
	character trabufi*(*),trabufo*(*) ! contains one logical tape block
	integer*4 tracount		! # characters in buffer
c
	if(mode.eq.ebcdic) then
	    call LIB$TRA_EBC_ASC(trabufi(1:tracount),trabufo(1:tracount))
	else if (mode.eq.gcosbcd) then
	    call bcdasc(trabufi,trabufo,tracount)
	else
	    trabufo = trabufi
	end if

	return
	end

	subroutine untranslate(trabuf,tracount)
	implicit none
	include 'etape_include.for'
	character trabuf*(*)		! contains one logical tape block
	integer*4 tracount		! # characters in buffer
c
	if(mode.eq.ebcdic) then
	    call LIB$TRA_ASC_EBC(trabuf(1:tracount),trabuf(1:tracount))
	    end if
	return
	end

	subroutine out_tty(outbuf,rec)
	implicit none
	include 'etape_include.for'
	character outbuf*(*)		! buffer to print (1 record)
	integer rec			! record number within block
	integer oline
	integer*2 length
	integer*4 istat,str$trim
	integer lines_per_record,i,kl

c
c format neatly for terminal using information in common block about
c page width and length (assume length of 0 or > 25 means hard copy terminal)
c
	istat=str$trim(outbuf,outbuf,length)
	oline=line_width-8	! characters available for output
	lines_per_record=(length+oline-1)/oline
c
c check if this record will overflow screen
c
	if(lpp.le.25.and.lpp.gt.0.and.lines_per_record*(rec-1).gt.lpp)then
	    call anykey
	    endif
c
c output the record
c
	do i=1,length,oline
	    kl=min(i+oline-1,length)
	    if(i.eq.1)then
		write(ttyout,0003)rec,outbuf(i:kl)
		call outscr(success,ttyout)
	    else
		call outscr(success,'       '//outbuf(i:kl))
	    endif
	    end do
c
	return
0003	format(1x,i4.4,':	',a)
	end

	subroutine out_file(outbuf,unit)
	implicit none
	character outbuf*(*)		! output record (deblocked & translated)
	integer unit			! where to write it
	integer length,istat,str$trim
c
0001	format(a)

	istat=str$trim(outbuf,outbuf,length)
	if(length .gt. 0) then
	    write(unit,1)outbuf(:length)
	else
	    write(unit,'()')
	endif
	return
	end

	subroutine check_status(*,*)
	implicit none
	include 'etape_include.for'
	include '($ssdef)'
	character   sysmsg*80
	integer*4   msglen

 1006	format(' SYS$QIOW returned error code:',z8)
 1007	format(' SYS$QIOW illegal status:',z8)
0007	format(' Block ',i,' truncated from length 'i)

	bigcount=jzext(count)

	if(retcode.ne.SS$_NORMAL) then
	    write(ttyout,1006)retcode
	    call outscr(success,ttyout)
	    call SYS$GETMSG(%val(retcode),msglen,sysmsg,,)
	    call outscr(retcode,sysmsg(1:msglen))
	    return 1
	    end if

	if(getoutnow) then
	    getoutnow = .false.
	    return 2
	    end if
	if(status.ne.SS$_ENDOFFILE) eofcnt=0
	if(status.eq.SS$_NORMAL)then
	    return

	else if(status.eq.SS$_ENDOFTAPE.or.status.eq.SS$_ENDOFVOLUME) then
	    at_end=.true.
	    at_begin=.false.

	else if(status.eq.SS$_ENDOFFILE) then
	    at_begin=.false.
	    nblock=block-1
	    block=1
	    if(incr_file)file=file+1
	    eofcnt=eofcnt+1
	    if(eofcnt.gt.1) then
		at_end=.true.
		at_begin=.false.
		end if

	else if(status.eq.SS$_DATAOVERUN) then
	    write(ttyout,7)block,bigcount
	    call outscr(fatal,ttyout)

	else
	    write(ttyout,1007)status
	    call outscr(success,ttyout)
	    call SYS$GETMSG(%val(status),msglen,sysmsg,,)
	    call outscr(status,sysmsg(1:msglen))
	    return 1
	end if
	return
	end

	subroutine auto_translate(num,*)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	AUTO_TRANSLATE.FOR
c	This routine processes standard labelled tapes and translates files
c	accordingly.
c
c	Rev. 2.6  - 19-Sep-1989
c	Rev. 2.15 - 01-Oct-1991
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include     'etape_include.for'
	include     '($ssdef)'
	include     '($mntdef)'
	include     '($iodef)'
	integer	    fnum,rmssts,rmsstv,iunit,condval
	integer     ios,tblock,num,icode
	integer     sys$qiow,sys$dassgn,sys$assign,sys$mount


 1010	format(bn,i8)
0008	format(' Max blocksize is',i,' default lrecl is',i)
0028	format(' Translated ',i,' blocks, EOF1 indicated ',i)
0030	format(' Translating ',a17,' Block=',i5,' Lrecl=',i5)

 6000	continue
	if (at_end.or.(num.eq.0)) then
	    filename = '<TTY>'
	    to_tty = .true.
	    call upscr
	    return
	    end if
c	First, read and translate a block (which in a label is a record)
c	until we find HDR1 and HDR2.

	retcode=SYS$QIOW(,%val(channel),%val(IO$_READLBLK),iosb,,,
	1   %ref(buffer(1:1)),%val(80),,,,)
	call check_status(*9500,*8900)
	if (at_end) then
	    filename = '<TTY>'
	    to_tty = .true.
	    call upscr
	    return
	    end if

	at_begin=.false.

c	Translate if necessary
	call translate(buffer,buffer,bigcount)
c	Do the label processing (includes translating files)
c HDR1  cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	if(buffer(1:4).eq.'HDR1') then		!Begin a new file
	    filename=buffer(5:21)
	    call validate_filename
	    open(unit=fileout,name=device(1:devlen)//filename,
	1	carriagecontrol='LIST',iostat=icode,
	1	recl=32000,organization='sequential',status='NEW',err=9070)
	    to_tty=.false.
	    nout=0

c HDR2  cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	else if(buffer(1:4).eq.'HDR2') then
	    recfm = buffer(5:5)//buffer(39:39)
	    if (recfm(1:1) .eq. 'U') recfm = 'U '	! 2.27
	    if (recfm .eq. 'F ') recfm = 'FB'
	    if( (recfm.ne.'FB').and.(recfm.ne.'D ').and.(recfm.ne.'S ').and.
	1	(recfm.ne.'V ').and.(recfm.ne.'VB').and.(recfm.ne.'VR').and.
	1	(recfm.ne.'VS').and.(recfm.ne.'U ')) then
		call outscr(success,'Record format '//recfm//' not recognized')
		call outscr(success,'fixed records assumed')
		recfm = 'FB'
	    end if
	    read(buffer(6:10),1010,IOSTAT=ios)blocksize
	    if(blocksize.gt.MAXBLOCK)then
		blocksize=MAXBLOCK
		write(ttyout,8)MAXBLOCK,lrecl
		call outscr(success,ttyout)
		endif
	    if(blocksize.lt.18)then
		call outscr(info,'Minimum blocksize is 18 bytes')
		blocksize=18
	        endif
	    read(buffer(11:15),1010,IOSTAT=ios)lrecl
	    if(lrecl.le.0)then
		call outscr(info,'Record length set to blocksize')
		lrecl=blocksize
		endif
	    if(lrecl.gt.MAXREC)then
		lrecl=80
		write(ttyout,8)MAXREC,lrecl
		call outscr(success,ttyout)
		endif
	    if(lrecl.gt.blocksize.and.(recfm.ne.'S ').and.(recfm.ne.'VR')
	1	.and.(recfm.ne.'VS')) then
		lrecl=blocksize	! that wouldn't do at all 
		end if
c	Skip to end of file (end of label)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
	1	%val(1),,,,,)
	    call check_status(*9500,*8900)
	    if(command_mode) then
		write(ttyout,30)filename,blocksize,lrecl
		call outscr(success,ttyout)
		end if
c	Now, translate the file
	    call upscr
	    call translate_file(*9500,*8900)

c EOF1  cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	else if(buffer(1:4).eq.'EOF1') then
	    read(buffer(55:60),1010,IOSTAT=ios)tblock
	    if(nblock.ne.tblock) then
		write(ttyout,28)nblock,tblock
		call outscr(success,ttyout)
		end if
	    close(fileout)
	    to_tty=.true.
c	    Skip to end of file (end of label)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
	1	%val(1),,,,,)
	    call check_status(*9500,*8900)
	    eofcnt = 1
	    num = num - 1	! Decrement remaining file count

c VOL1  cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	else if(buffer(1:4).eq.'VOL1') then
	    volname = buffer(5:10)
c	    call outscr(success,'Beginning tape '//volname)

c EOV1  cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	else if(buffer(1:4).eq.'EOV1') then	!End of volume processing
	    volnum=volnum+1
	    call outscr(success,'End of volume '//char(volnum+47)//
	1	' encountered')
	    retcode = sys$dassgn(%val(channel))
	    call sys$dismou(logname(1:log_len),)
	    comment = 'Please mount relative volume '//char(volnum+48)//
	1	' on device '//phyname(1:phyname_len)
	    call outscr(success,comment)
	    mount_flags = mnt$m_foreign+mnt$m_nowrite+mnt$m_ovr_access
	    retcode = sys$mount(mount_itemlist)
	    retcode=SYS$ASSIGN(logname(1:log_len),channel,,)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_REWIND),iosb,,,,,,,,)
	    call outscr(success,'Beginning volume '//char(volnum+48))
c	Skip the tape label (trusting soul, aren't I?)
	    retcode=SYS$QIOW(,%val(channel),%val(IO$_SKIPFILE),iosb,,,
	1	%val(1),,,,,)
	    call check_status(*9500,*8900)
	    at_begin=.false.
	    at_end=.false.
	    file=1
	    block=1
c	    Now, translate the file
	    call upscr
	    call translate_file(*9500,*8900)
	else

c       cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	    call outscr(warning,'Unrecognized label '//buffer(1:4))
	    call outscr(fatal,'Please position tape and try again')
	    return
	endif

	go to 6000

 8900	return

 9500	return 1

 9070	continue
	call errsns (fnum,rmssts,rmsstv,iunit,condval)
	call outscr(success,'Error opening file:'//filename)
	call outscr(rmssts,' ')
	to_tty=.true.
	go to 6000

	end

	subroutine translate_file(*,*)
	implicit none
	include 'etape_include.for'
	include '($ssdef)'
	include '($iodef)'
	integer sys$qiow

1012	format(' Short block of ',i,' read for block number ',i)
1013	format(' File contained ',i7,' records.')

6050	retcode=SYS$QIOW(,%val(channel),%val(IO$_READLBLK),iosb,,,
	1   %ref(buffer(1:1)),%val(blocksize),,,,)
	call check_status(*9500,*8900)
	if(status.eq.SS$_ENDOFFILE) then
	    write(ttyout,1013)nout
	    call outscr(success,ttyout)
	    return
	    end if
	if(mode.eq.gcosbcd) then
	    continue
	else if((bigcount.lt.blocksize).and.(recfm.eq.'FB')) then
	    write(ttyout,1012)bigcount,block
	    call outscr(success,ttyout)
	endif
	block=block+1
	at_begin=.false.
	call deblock
	go to 6050

9500	return 1
8900	return 2
	end

	subroutine enable_out_of_band_smg
	implicit none
	include 'etape_include.for'
	external smg_oob_routine
	integer smg$set_out_of_band_asts

	integer istat,ccmask
	data ccmask/'008000C0'x/	! ^F, ^G & ^W

	istat = smg$set_out_of_band_asts(pasteboard,ccmask,smg_oob_routine,)
	return
	end

	subroutine smg_oob_routine(oob_structure)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	SMG_OOB_ROUTINE.FOR
c	This routine sets the flag to allow ETAPE to exit gracefully
c	by pressing the ^F key, turn updating on and off with ^G, and
c	repaint the screen with ^W.
c	This is called by an out-of-band AST
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include 'etape_include.for'
	include '($smgdef)'

	record/smgdef/ oob_structure

	if(oob_structure.smg$b_character.eq.6) then
	    getoutnow = .true.
	else if(oob_structure.smg$b_character.eq.7) then
	    display = .not. display
	else if(oob_structure.smg$b_character.eq.23) then
	    call smg$repaint_screen(pasteboard)
	end if
	return
	end

	subroutine enable_out_of_band_line
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	ENABLE_OUT_OF_BAND_LINE.FOR
c	This routine sets up an out-of-band AST when running in line mode
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include	    '($iodef)'
	include	    '($smgdef)'
	integer*4   oob_iosb(2)
	integer*4   outband_mask(2),lchan
	external    line_oob_routine

c	call lib$insv (1, smg$k_trm_ctlf, 1, outband_mask(2))
	call lib$insv (1, smg$k_trm_ctrlg, 1, outband_mask(2))


	call sys$assign('tt',lchan,,)
	call sys$qiow(,%val(lchan),%val(io$_setmode + io$m_outband)
	1    ,oob_iosb,,,line_oob_routine,outband_mask,,,,)
	return

	entry disable_out_of_band_line

	call sys$qiow(,%val(lchan),
	1	%val(io$_setmode + io$m_outband),,,,,,,,,)
	return
	end

	subroutine line_oob_routine
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	LINE_OOB_ROUTINE.FOR
c	This routine sets the flag to allow ETAPE to exit gracefully
c	by pressing the ^F key, and report status with ^G
c	This is called by an out-of-band AST
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include 'etape_include.for'

 1014	format('Tape positioned at file number ',i,' block ',i)
 1016	format(1x,i,' blocks written')
 1017	format('Blocksize is set at ',i,' and record length is ',i)

	write(ttyout,1014)file,block
	call outscr(success,ttyout)
	if(to_tty)then
	    call outscr(success,'Output currently going to TTY')
	else
	    call outscr(success,'Output currently going to '//filename)
	    write(ttyout,1016)nout
	    call outscr(success,ttyout)
	endif
	write(ttyout,1017)blocksize,lrecl
	call outscr(success,ttyout)
	if(mode.eq.ebcdic)then
	    call outscr(success,'Mode is EBCDIC.')
	else if (mode.eq.ascii) then
	    call outscr(success,'Mode is ASCII')
	else
	    call outscr(success,'Mode is GCOS BCD')
	endif
	return
	end

	subroutine bcdasc(trabufi,trabufo,tracount)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	BCDASC.FOR
c	This routine converts GCOS BCD into ASCII
c
c	Rev. 04-Aug-1989
c	Rev. 18-Sep-1989
c	Rev. 19-Feb-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include 'etape_include.for'
	character trabufi*(*), trabufo*(*)
	integer*4 i,j,tracount
	character*1 bcd(0:63)
	data bcd/'0','1','2','3','4','5','6','7',
	1	 '8','9','[','#','@',':','>','?',
	2	 ' ','A','B','C','D','E','F','G',
	3	 'H','I','&','.',']','(','<','\',
	4	 '^','J','K','L','M','N','O','P',
	5	 'Q','R','-','$','*',')',';','''',
	6	 '+','/','S','T','U','V','W','X',
	7	 'Y','Z','_',',','%','=','"','!'/

	j=1
	trabufi(tracount+1:tracount+2) = char(0)//char(0)
	do i = 1,(tracount+2),3
	    buffer2(j:j) = bcd(zext(ichar(trabufi(i:i)))/4)
	    buffer2(j+1:j+1) = bcd((ichar(trabufi(i:i)).and.'03'x) * 16 +
	1	zext(ichar(trabufi(i+1:i+1)))/16)
	    buffer2(j+2:j+2) = bcd((ichar(trabufi(i+1:i+1)).and.'0F'x) * 4 +
	1	zext(ichar(trabufi(i+2:i+2)))/64)
	    buffer2(j+3:j+3) = bcd((ichar(trabufi(i+2:i+2)).and.'3F'x))
	    j=j+4
	    end do
	trabufo = buffer2(1:j-1)
	return
	end

	subroutine gdeblock(blksize)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	GDEBLOCK.FOR
c	This routine a deblocks a standard variable-length GCOS block
c
c	Rev. 04-Aug-1989
c	Rev. 18-Sep-1989
c	Rev. 06-Dec-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include 'etape_include.for'
	character bcd*64
	data bcd/
	1'0123456789[#@:>? ABCDEFGHI&.](<\^JKLMNOPQR-$*);''+/STUVWXYZ_,%="!'/
	integer*4 blksize,rec
	integer   gbs,grl,next
c
	rec=0
	if(buffer(1:2).eq.'GE') then
	    if(to_tty)then
		call out_tty(buffer2(1:blksize),1)
	    else
		call out_file(buffer2(1:blksize),fileout)
		nout=nout+1
	    endif
	    call upscr2
	    return
	    end if
	if(blksize.eq.0) return
	gbs=(index(bcd,buffer2(4:4))-1)*4096 +
	1   (index(bcd,buffer2(5:5))-1)*64 +
	2   (index(bcd,buffer2(6:6))-1)
	next=1
	do while(next.lt.gbs)
	    grl = (index(bcd,buffer2(next*6+1:next*6+1))-1)*4096 +
	1	(index(bcd,buffer2(next*6+2:next*6+2))-1)*64 +
	2	(index(bcd,buffer2(next*6+3:next*6+3))-1)
	    rec=rec+1
	    if(to_tty)then
		call out_tty(buffer2(next*6+7:next*6+6+grl*6),rec)
	    else
		call out_file(buffer2(next*6+7:next*6+6+grl*6),fileout)
		nout=nout+1
	    endif
	    next = next + grl + 1
	    end do
	call upscr2
	return
	end

	subroutine bldscr
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	BLDSCR.FOR
c	This routine creates the SMG screen and fills in static fields.
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include 'etape_include.for'
	external broadcast_handler

	call smg$create_pasteboard(pasteboard,,,)
	call smg$create_virtual_keyboard(keyboard_id,,,,)
	call smg$create_virtual_display(10,78,display_id_1,
	1	smg$m_border,,)
	call smg$create_virtual_display(8,78,display_id_2,
	1	smg$m_border,,)
	call smg$create_virtual_display(2,78,display_id_3,
	1	smg$m_border,,)
	call smg$create_virtual_display(24,80,display_id_4,,,)
	call smg$label_border(display_id_1,
	1	'ETAPE Foreign Tape Conversion -  '//
	2	'Rev. 2.26 - 28-Mar-1995',smg$k_top,,
	2	smg$m_reverse,,)
	call smg$label_border(display_id_2,
	1	'Output Messages',smg$k_top,,
	2	smg$m_reverse,,)
	call smg$label_border(display_id_3,
	1	'Command Line',smg$k_top,,
	2	smg$m_reverse,,)

	call smg$set_cursor_mode(pasteboard,1)

	call smg$put_chars(display_id_1,'Tape character set is', 03,05,,,,)
	call smg$put_chars(display_id_1,'Tape label writing',    04,05,,,,)
	call smg$put_chars(display_id_1,'Record format is',      05,05,,,,)
	call smg$put_chars(display_id_1,'Current Settings:',     06,05,,,,)
	call smg$put_chars(display_id_1,' Device:',              07,05,,,,)
	call smg$put_chars(display_id_1,' Trans:',               08,05,,,,)
	call smg$put_chars(display_id_1,' File:',                09,05,,,,)


	call smg$put_chars(display_id_1,'Tape Label',            01,27,,,,)
	call smg$put_chars(display_id_1,'Block Length',          03,41,,,,)
	call smg$put_chars(display_id_1,'Logical Record Size',   04,41,,,,)
	call smg$put_chars(display_id_1,'Tape density',          05,41,,,,)
	call smg$put_chars(display_id_1,'File Number',           06,41,,,,)
	call smg$put_chars(display_id_1,'Block Number',          07,41,,,,)
	call smg$put_chars(display_id_1,'Records output',        08,41,,,,)
	call smg$put_chars(display_id_2,
	1	'To enter commands press the DO key or press CRTL/D',
	2	1,13,,smg$m_bold,,)

	call smg$paste_virtual_display(display_id_1,pasteboard,2,2,)
	call smg$paste_virtual_display(display_id_2,pasteboard,13,2,)
	call smg$paste_virtual_display(display_id_3,pasteboard,22,2,)

	call smg$set_display_scroll_region(display_id_2,2,)
	call smg$set_display_scroll_region(display_id_3,,)

	call smg$set_keypad_mode(keyboard_id,1)

	call smg$set_cursor_abs(display_id_2,2,1)

	call upscr
	call smg$set_broadcast_trapping(pasteboard,broadcast_handler)

	return
	end

	subroutine inscr(c2,rest)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	INSRC.FOR
c	This routine will input a command and/or process keypad strokes.
c
c	Rev. 04-Aug-1989
c	Rev. 2.23 - 08-Mar-1995 Altered definition of 'rest'
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include '($trmdef)'
	include 'sys$library:foriosdef'
	include 'etape_include.for'

	integer   i,j
	integer*2 keystroke,command_len

	character command_str*72,c2*(*),rest*(*)

 1001	format(a2,1x,a)
 1002	format('$ETAPE>')
 1003   format(i5.5)
	if(command_mode) then
	    write(*,1002)
	    read(*,1001,iostat=i)c2,rest
	    if(i.eq.-1) c2 = 'QU'
	    call str$upcase (c2, c2)
	    call str$upcase (rest, rest)
	    return
	    end if

c	Do screen inquire
	call upscr
	command_len = 0
	command_str = ' '
	rest = ' '

0010	call smg$read_keystroke(keyboard_id,keystroke,,,,,)

	if(keystroke.eq.smg$k_trm_do.or.
	1  keystroke.eq.smg$k_trm_ctrld) then		! Execute a command
	    call smg$set_cursor_mode(pasteboard,0)
	    call smg$read_string(keyboard_id,command_str,'ETAPE>',63,
	1	trm$m_tm_cvtlow,,,command_len,,display_id_3,,,)
	    call smg$set_cursor_mode(pasteboard,1)
	    c2 = command_str(1:2)
	    rest = command_str(4:command_len)
	    goto 900
	    end if

	if( (keystroke.eq.smg$k_trm_ctrlz) .or.		! Quit
	1   (keystroke.eq.smg$k_trm_f10)) then
	    c2 = 'QU'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_pf1) goto 400		! Gold

	if( (keystroke.eq.smg$k_trm_pf2) .or.		! Help
	1   (keystroke.eq.smg$k_trm_help)) then
	    call hlpscr
	    endif

	if(keystroke.eq.smg$k_trm_pf3) then		! Toggle translation
	    c2 = 'UT'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_pf4) then		! Filename
	    call smg$erase_display(display_id_1,9,13,9,69)
	    call smg$set_cursor_abs(display_id_1,9,13)
	    call smg$set_cursor_mode(pasteboard,0)
	    call smg$read_string(keyboard_id,rest,,60,trm$m_tm_cvtlow
	1	,,,,,display_id_1,,,)
	    call smg$set_cursor_mode(pasteboard,1)
	    c2 = 'FI'
	    goto 900
	    end if


	if(keystroke.eq.smg$k_trm_kp0) then		! Translate block
	    c2 = 'TB'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_kp1) then		! Skip block
	    c2 = 'SB'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_kp2) then		! Skip file
	    c2 = 'SF'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_kp3) then		! Set default
	    call smg$erase_display(display_id_1,7,13,7,40)
	    call smg$set_cursor_abs(display_id_1,7,13)
	    call smg$set_cursor_mode(pasteboard,0)
	    call smg$read_string(keyboard_id,rest,,60,trm$m_tm_cvtlow
	1	,,,,,display_id_1,,,)
	    call smg$set_cursor_mode(pasteboard,1)
	    c2 = 'SD'
	    goto 900
	    end if

	if(keystroke.eq.smg$k_trm_kp4) then		! Rewind
	    c2 = 'RW'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_kp5) then		! Record format
	    if(recfm.eq.'FB') then
		recfm = 'D '
	    else if(recfm.eq.'D ') then
		recfm = 'S '	    
	    else if(recfm.eq.'S ') then
		recfm = 'V '	    
	    else if(recfm.eq.'V ') then
		recfm = 'VB'	    
	    else if(recfm.eq.'VB') then
		recfm = 'VS'	    
	    else if(recfm.eq.'VS') then
		recfm = 'VR'	    
	    else if(recfm.eq.'VR') then
		recfm = 'U '	    
	    else
		recfm = 'FB'
	    end if
		call upscr
	    end if

	if(keystroke.eq.smg$k_trm_kp6) then	    ! Special translate
	    call smg$erase_display(display_id_1,8,13,8,40)
	    call smg$set_cursor_abs(display_id_1,8,13)
	    call smg$set_cursor_mode(pasteboard,0)
	    call smg$read_string(keyboard_id,rest,,60,trm$m_tm_cvtlow
	1	,,,,,display_id_1,,,)
	    call smg$set_cursor_mode(pasteboard,1)
	    c2 = 'ST'
	    goto 900
	    end if

	if(keystroke.eq.smg$k_trm_kp7) then		! Volume name
	    call smg$erase_display(display_id_1,'     ',1,39,1,44)
	    call smg$set_cursor_abs(display_id_1,1,39)
	    call smg$set_cursor_mode(pasteboard,0)
	    call smg$read_string(keyboard_id,rest,,6,trm$m_tm_cvtlow
	1	,,,,,display_id_1,,,)
	    call smg$set_cursor_mode(pasteboard,1)
	    c2 = 'VL'
	    goto 900
	    end if

	if(keystroke.eq.smg$k_trm_kp8) then		! Output labels
	    c2 = 'OL'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_kp9) then		! GCOS translation
	    c2 = 'GT'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_enter) then		! Auto translate *
	    c2 = 'AT'
	    rest = '*'
	    goto 900
	    endif

	if(keystroke.eq.smg$k_trm_minus) then		! Blocksize
	    c2 = 'BL'
	    i = 3
	    j = blocksize
	    goto 300
	    endif

	if(keystroke.eq.smg$k_trm_comma) then		! Record length
	    c2 = 'LR'
	    i = 4
	    j = lrecl
	    goto 300
	    endif

	if(keystroke.eq.smg$k_trm_period) then		! Translate file
	    c2 = 'TF'
	    goto 900
	    endif

	goto 10


c	Input a 5 digit number
 0300	continue
	call smg$erase_display(display_id_1,i,61,i,69)
	call smg$set_cursor_abs(display_id_1,i,63)
	call smg$set_cursor_mode(pasteboard,0)
	call smg$read_string(keyboard_id,rest,,5,,,,,,display_id_1,,,)
	call smg$set_cursor_mode(pasteboard,1)
	if (rest.eq.' ') write(rest,1003) j
	goto 900

c	GOLD key processing
 0400	continue
	call smg$read_keystroke(keyboard_id,keystroke,,,,,)
	if(keystroke.eq.smg$k_trm_kp1) c2 = 'BB'	! Backspace block
	if(keystroke.eq.smg$k_trm_kp2) c2 = 'BF'	! Backspace file
	if(keystroke.eq.smg$k_trm_pf2) c2 = 'HE'	! Help
	if(keystroke.eq.smg$k_trm_enter) then		! Auto translate single
	    c2 = 'AT'
	    endif
	goto 900

 0900	return
	end

	subroutine outscr(stat,outstr)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	OUTSCR.FOR
C*UNH   Issues success and error messages to the output device.  If we are
C*UNH	talking directly to a user (i.e. .NOT. Command_Mode) we ignore Stat.
C*UNH	If we are in Command_Mode, we do one of the following:
C*UNH	
C*UNH	   Stat     Action
C*UNH	   ----     -----------------------------------------------------
C*UNH       0       Signal a warning after displaying message.
C*UNH	    1       Display message without signaling.
C*UNH	    2	    Signal an error after displaying message.
C*UNH	    3       Display message without signaling.
C*UNH	    4	    Signal a fatal error after displaying message AND *HALT*
C*UNH      >4	    Ignore message and signal the supplied Stat.
C*UNH               If Stat is an even number, we will *HALT*.
C*UNH
C*UNH	The goal is to retain the original behavior for messages to interactive
C*UNH	users while providing proper error returns for the batch jobs.
C*UNH	Note that any ERROR (2), FATAL ERROR (4), or EVEN Status greater than
C*UNH	4 will cause us to exit with that status, otherwise a normal FORTRAN
C*UNH	STOP is performed which, of course, exits with a NORMAL status.
c
c	Rev. 06-Dec-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include 'etape_include.for'

	integer outlen
	integer stat,libstat,lib$stop,lib$signal

	character *(*) outstr
	character*200 dummy
	call str$trim(dummy,outstr,outlen)
	if (.not. command_mode) then
	    call smg$put_with_scroll(display_id_2,outstr(1:outlen),,,,,)
	else if (stat .le. 4) then
	    write(*,*)outstr(1:outlen)
	    if (.not. stat) libstat = lib$signal(%val(stat))	! 4 is an auto-
	else if (.not. stat) then
	    libstat = lib$stop(%val(stat))
	else 
	    libstat = lib$signal(%val(stat))
	endif

	return	! Maybe...

	end

	subroutine broadcast_handler(ast,r0,r1,pc,psl)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	BROADCAST_HANDLER.FOR
c	This routine traps broadcast messages and shunts them into display_2
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include '($ssdef)'
	include 'etape_include.for'
	character   message*128
	integer*2   message_len
	integer	    ast,r0,r1,pc,psl,istat
	integer	    smg$get_broadcast_message

	istat = ast	! used to overcome error message
	istat = r0	! used to overcome error message
	istat = r1	! used to overcome error message
	istat = pc	! used to overcome error message
	istat = psl	! used to overcome error message
	istat = smg$get_broadcast_message(pasteboard, message, message_len)
	do while(istat.eq.ss$_normal)
	    call smg$put_line (display_id_2,
	1	message(1:message_len), 1,,, smg$m_wrap_char,,)
	    istat = smg$get_broadcast_message(pasteboard, message, message_len)
	    end do
	return
	end

	subroutine upscr
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	UPSCR.FOR
c	Update all variable fields on the screen.
c
c	Rev. 04-Aug-1989
c	Rev. 19-Feb-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include 'etape_include.for'

	character numout*9

 9000	format(i9)

	if(command_mode) return

	if(mode.eq.gcosbcd) then
	    call smg$put_chars(display_id_1,'Variable',3,62,,,,)
	else
	    write(numout,9000)blocksize
	    call smg$put_chars(display_id_1,numout,3,61,,,,)
	end if

	if(mode.eq.gcosbcd.or.recfm.ne.'FB') then
	    call smg$put_chars(display_id_1,'Variable',4,62,,,,)
	else
	    write(numout,9000)lrecl
	    call smg$put_chars(display_id_1,numout,4,61,,,,)
	end if

	write(numout,9000)density
	call smg$put_chars(display_id_1,numout,5,61,,,,)

	call smg$put_chars(display_id_1,device(1:27),7,13,,,,)
	call smg$put_chars(display_id_1,specfile(1:27),8,13,,,,)
	call smg$put_chars(display_id_1,filename,9,13,,,,)
	call smg$put_chars(display_id_1,volname,1,39,,,,)

	if(mode.eq.ebcdic) then
		call smg$put_chars(display_id_1,'EBCDIC',3,27,,,,)
	else if (mode.eq.ascii) then
		call smg$put_chars(display_id_1,'ASCII ',3,27,,,,)
	else
		call smg$put_chars(display_id_1,'GBCD  ',3,27,,,,)
	endif

	if(labeled) then
		call smg$put_chars(display_id_1,'Enabled ',4,27,,,,)
	else
		call smg$put_chars(display_id_1,'Disabled',4,27,,,,)
	end if


	if(mode.eq.gcosbcd) then
	    call smg$put_chars(display_id_1,'GCOS Var.',5,27,,,,)
	else if(recfm.eq.'FB') then
	    call smg$put_chars(display_id_1,'Fixed    ',5,27,,,,)
	else if(recfm.eq.'D ') then
	    call smg$put_chars(display_id_1,'ANS Var. ',5,27,,,,)
	else if(recfm.eq.'S') then
	    call smg$put_chars(display_id_1,'ANS Seg. ',5,27,,,,)
	else if(recfm.eq.'V ') then
	    call smg$put_chars(display_id_1,'IBM Var. ',5,27,,,,)
	else if(recfm.eq.'VB') then
	    call smg$put_chars(display_id_1,'IBM VB   ',5,27,,,,)
	else if(recfm.eq.'VS') then
	    call smg$put_chars(display_id_1,'IBM VS   ',5,27,,,,)
	else if(recfm.eq.'VR') then
	    call smg$put_chars(display_id_1,'IBM VBS  ',5,27,,,,)
	else if(recfm.eq.'U ') then
	    call smg$put_chars(display_id_1,'IBM Undef',5,27,,,,)
	else
	    call smg$put_chars(display_id_1,'Unknown',5,27,,,,)
	end if

	if(at_begin) then
		call smg$put_chars(display_id_1,'BOT',6,72,,
	1		smg$m_reverse,,)
	elseif(at_end) then
		call smg$put_chars(display_id_1,'EOT',6,72,,
	1		smg$m_reverse+smg$m_blink,,)
	else
		call smg$put_chars(display_id_1,'   ',6,72,,,,)
	endif

	entry upscr2
	if(.not. display) return
	if(command_mode) return
	write(numout,9000)file
	call smg$put_chars(display_id_1,numout,6,61,,,,)

	write(numout,9000)block
	call smg$put_chars(display_id_1,numout,7,61,,,,)

	write(numout,9000)nout
	call smg$put_chars(display_id_1,numout,8,61,,,,)

	return
	end

	subroutine delscr
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	DELSCR.FOR
c	This routine deletes the SMG screen (if applicable)
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include 'etape_include.for'

	if(command_mode) return

	call smg$set_cursor_mode(pasteboard,0)
	call smg$set_keypad_mode(keyboard_id,0)
	call smg$disable_broadcast_trapping(pasteboard)

	call smg$delete_virtual_keyboard(keyboard_id)

	call smg$delete_virtual_display(display_id_1)
	call smg$delete_virtual_display(display_id_2)
	call smg$delete_virtual_display(display_id_3)
	call smg$delete_virtual_display(display_id_4)

	call smg$delete_pasteboard(pasteboard)

	return
	end

	subroutine hlpscr
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	HLPSCR.FOR
c	Produce and process the HELP screen
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include 'etape_include.for'
	parameter   num_topics = 18
	integer*4   keystrokes(num_topics),i,j,save_screen
	character*2 help_topics(num_topics)
	character*2 gold_topics(num_topics)
	character*1 dummy
	integer keystroke

	data keystrokes/
	1   SMG$K_TRM_PF1, SMG$K_TRM_PF2, SMG$K_TRM_PF3,
	1   SMG$K_TRM_PF4, SMG$K_TRM_KP0, SMG$K_TRM_KP1,
	1   SMG$K_TRM_KP2, SMG$K_TRM_KP3, SMG$K_TRM_KP4,
	1   SMG$K_TRM_KP5, SMG$K_TRM_KP6, SMG$K_TRM_KP7,
	1   SMG$K_TRM_KP8, SMG$K_TRM_KP9, SMG$K_TRM_ENTER,
	1   SMG$K_TRM_MINUS, SMG$K_TRM_COMMA, SMG$K_TRM_PERIOD/
	data help_topics/
	1   '  ', 'HE', 'UT', 'FI', 'TB', 'SB',
	1   'SF', 'SD', 'RW', 'RF', 'ST', 'VL',
	1   'OL', 'GT', 'AT', 'BL', 'LR', 'TF'/
	data gold_topics/
	1   '  ', '  ', '  ', '  ', '  ', 'BB',
	1   'BF', '  ', '  ', '  ', '  ', '  ',
	1   '  ', '  ', '  ', '  ', '  ', '  '/

 1001	format(a)

	call smg$unpaste_virtual_display(display_id_1,pasteboard)
	call smg$unpaste_virtual_display(display_id_2,pasteboard)
	call smg$unpaste_virtual_display(display_id_3,pasteboard)

	call smg$draw_rectangle(display_id_4,1,44,21,80,,)
	call smg$draw_line(display_id_4,5,44,5,80)
	call smg$draw_line(display_id_4,9,44,9,80)
	call smg$draw_line(display_id_4,13,44,13,80)
	call smg$draw_line(display_id_4,17,44,17,71)
	call smg$draw_line(display_id_4,1,53,17,53)
	call smg$draw_line(display_id_4,1,62,21,62)
	call smg$draw_line(display_id_4,1,71,21,71)
	call smg$draw_rectangle(display_id_1,1,44,21,80,,)
	if(mode.eq.ebcdic) then
	    call smg$put_chars(display_id_4,' ASCII  ',3,63,,,,)! PF3
	else
	    call smg$put_chars(display_id_4,' EBCDIC ',3,63,,,,)! PF3
	end if
	call smg$put_chars(display_id_4,'  GBCD  ',07,63,,,,)	! KP9
	call smg$put_chars(display_id_4,'  Gold  ',03,45,,
	1	smg$m_reverse,,)				! PF1
	call smg$put_chars(display_id_4,'  Help  ',02,54,,,,)	! PF2
	call smg$put_chars(display_id_4,'  Help  ',04,54,,
	1	smg$m_reverse,,)				! PF2 (Gold)
	call smg$put_chars(display_id_4,' Output ',02,72,,,,)	! PF4
	call smg$put_chars(display_id_4,' File   ',03,72,,,,)	! PF4
	call smg$put_chars(display_id_4,' Block  ',06,72,,,,)	! KP-
	call smg$put_chars(display_id_4,' Length ',07,72,,,,)	! KP-
	call smg$put_chars(display_id_4,' Logical',10,72,,,,)	! KP,
	call smg$put_chars(display_id_4,' Record ',11,72,,,,)	! KP,
	call smg$put_chars(display_id_4,' Output ',06,54,,,,)	! KP8
	call smg$put_chars(display_id_4,' Labels ',07,54,,,,)	! KP8
	call smg$put_chars(display_id_4,' Volume ',06,45,,,,)	! KP7
	call smg$put_chars(display_id_4,' Label  ',07,45,,,,)	! KP7
	call smg$put_chars(display_id_4,' Special',10,63,,,,)	! KP6
	call smg$put_chars(display_id_4,'Trnslate',11,63,,,,)	! KP6
	call smg$put_chars(display_id_4,' Recfm  ',11,54,,,,)	! KP5
	call smg$put_chars(display_id_4,' Rewind ',11,45,,,,)	! KP4
	call smg$put_chars(display_id_4,'Set Def ',14,63,,,,)	! KP3
	call smg$put_chars(display_id_4,'Skp File',14,54,,,,)	! KP2
	call smg$put_chars(display_id_4,'Bck File',16,54,,
	1	smg$m_reverse,,)				! KP2 (GOLD)
	call smg$put_chars(display_id_4,'Skp Blck',14,45,,,,)	! KP1
	call smg$put_chars(display_id_4,'Bck Blck',16,45,,
	1	smg$m_reverse,,)				! KP1 (GOLD)
	call smg$put_chars(display_id_4,'    Translate    ',
	1	18,45,,,,)					! KP0
	call smg$put_chars(display_id_4,'      Block      ',
	1	19,45,,,,)					! KP0
	call smg$put_chars(display_id_4,'Trnslate',18,63,,,,)	! KP.
	call smg$put_chars(display_id_4,'  File  ',19,63,,,,)	! KP.
	call smg$put_chars(display_id_4,'  Auto  ',14,72,,,,)	! KP ENTER
	call smg$put_chars(display_id_4,'Trnslate',15,72,,,,)	! KP ENTER
	call smg$put_chars(display_id_4,'  All   ',16,72,,,,)	! KP ENTER
	call smg$put_chars(display_id_4,'  Auto  ',18,72,,
	1	smg$m_reverse,,)				! KP ENT (GOLD)
	call smg$put_chars(display_id_4,'Trnslate',19,72,,
	1	smg$m_reverse,,)				! KP ENT (GOLD)

	call smg$put_chars(display_id_4,
	1	'CTRL/A      Insert/Overstrike mode toggle',16,1,,,,)
	call smg$put_chars(display_id_4,
	1	'CTRL/F      Terminate translation',17,1,,,,)
	call smg$put_chars(display_id_4,
	1	'CTRL/G      Toggle screen updating',18,1,,,,)
	call smg$put_chars(display_id_4,
	1	'CTRL/W      Refresh screen',19,1,,,,)
	call smg$put_chars(display_id_4,
	1	'CTRL/Z      Exit',20,1,,,,)
	call smg$put_chars(display_id_4,
	1	'Press a key for help on that key.',22,1,,,,)
	call smg$put_chars(display_id_4,
	1	'To exit, press the space bar.',23,1,,,,)

	call smg$paste_virtual_display(display_id_4,
	1	pasteboard,1,1,)

	keystroke = 0
	do while(keystroke .ne. smg$k_trm_space)
	call smg$read_keystroke(keyboard_id,keystroke,,,,,)
	do i = 1,num_topics
	    if (keystroke .eq. keystrokes(i)) then
		call smg$save_physical_screen(pasteboard,save_screen,,)
	        if(help_topics(i) .ne. '  ') then
		    call LBR$GET_HELP(lib_index,,,,'ETAPE',help_topics(i))
		    end if
	        if(gold_topics(i) .ne. '  ') then
		    call LBR$GET_HELP(lib_index,,,,'ETAPE',gold_topics(i))
		    end if
		write(*,*)'Press return to continue'
		read(*,1001,iostat=j)dummy
		call smg$restore_physical_screen(pasteboard,save_screen)
		end if
	    end do
	end do

	call smg$unpaste_virtual_display(display_id_4,pasteboard)

	call smg$paste_virtual_display(display_id_1,
	1	pasteboard,2,2,)
	call smg$paste_virtual_display(display_id_2,
	1	pasteboard,13,2,)
	call smg$paste_virtual_display(display_id_3,
	1	pasteboard,22,2,)
	return
	end

	subroutine anykey
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	ANYKEY.FOR
c	Read a keystroke and return
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include '($smgdef)'
	include 'etape_include.for'

	integer keystroke
	character*1 temp

 1001	format(a)


	if(command_mode) then
	    call outscr(success,'Press RETURN to continue')
	    read(*,1001)temp
	else
	    call outscr(success,'Press any key to continue')
	    call smg$read_keystroke(keyboard_id,keystroke,,,,,)
	end if
	return
	end

	subroutine validate_filename
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	VALIDATE_FILENAME.FOR
c	This routine will scan a filename and alter it to be a valid VMS
c	filename.  This is particularly useful for the AT command and files
c	containing multiple periods (i.e. aaa.bbb.ccc).
c
c	Rev. 04-Aug-1989
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include     'etape_include.for'
	character   test*1
	logical	    period
	integer	    i, filename_length
	integer	    str$trim

 1001	format(i4.4)

	period = .false.
	retcode = str$trim (filename, filename, filename_length)
	call str$upcase(filename,filename)
	if(filename(1:1).eq.'.') then	! Not a pleasing occurrance
	    filename = filename(2:)
	    filename_length = filename_length - 1
	    end if
	do i = 1,filename_length
	    test = filename(i:i)
	    if ((test.ge.'0'.and.test.le.'9') .or.
	1	(test.ge.'A'.and.test.le.'Z') .or.
	1	(test.eq.'_') .or.
	1	(test.eq.'-') ) then
		continue
	    else
		if(test.eq.'.') then
		    if (.not.period) then
			period = .true.
		    else
			filename(i:i) = '_'
		    endif
		else
		    filename(i:i) = '_'
		end if
	    end if
	    end do
	if(filename.eq.' ') then	    ! GCOS likes to write blank names
	    filename = 'FILE0000.DAT'
	    write(filename(5:8),1001)file
	    end if
	return
	end

	integer function specload
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	SPECLOAD.FOR
c	This routine reads a file of special translation directives and loads
c	them into internal format.  Return is ss$_normal if no errors are
c	encountered, ss$_abort otherwise.
c
c	Rev. 04-Aug-1989
c	Rev. 19-Sep-1989
c	Rev. 11-Jan-1990
c	Rev. 06-Jun-1990
c	Rev. 06-Feb-1991
c	Rev. 2.24 - 17-Mar-1995
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	include 'etape_include.for'
	include		'($ssdef)'
	integer		reclen
	character	tranrec*80, num_bytes*6

	parameter	maxfld = 500	    ! Maximum number of fields
	byte		trans_type(maxfld)
	integer*2	start(maxfld),length(maxfld)
	integer		numfld,next
	common/sp_trans/numfld,start,length,trans_type


 1001	format(q,a)
 1002	format(i5)

	specload = ss$_normal
	open(unit=filespec,readonly,status='old',name=specfile,err=850)
	numfld = 0
	next = 1
0010	read(filespec,1001,end=900)reclen,tranrec(1:reclen)
	if(tranrec(1:1).eq.'!') go to 10
	numfld = numfld + 1
	if(numfld.gt.maxfld) then
	    call outscr(success,'Too many fields for special translation')
	    numfld = maxfld
	    go to 850
	    end if
	read(tranrec(06:10),1002,err=800)start(numfld)
	read(tranrec(12:16),1002,err=800)length(numfld)
	if (start(numfld).eq.0) then
	    start(numfld) = next
	end if
	call str$upcase(tranrec,tranrec)
	if(tranrec(1:4).eq.'CHAR') then
	    trans_type(numfld) = 1
	else if(tranrec(1:4).eq.'PACK') then
	    trans_type(numfld) = 2
	    length(numfld) = (length(numfld)+2)/2   !Convert to bytes
	else if(tranrec(1:4).eq.'BIN4') then
	    trans_type(numfld) = 3
	else if(tranrec(1:4).eq.'BIN2') then
	    trans_type(numfld) = 4
	else if(tranrec(1:4).eq.'IBF4') then
	    trans_type(numfld) = 5
	else
	    call outscr(success,'Unrecognized translation option'//
	1	tranrec(1:4))
	    go to 850
	end if
	next = length(numfld) + start(numfld)
	go to 10

 0800	call outscr(success,'Error interpreting number')
	call outscr(success,'Text is '//tranrec(1:reclen))
 0850	specload = ss$_abort
	return
	
 0900	continue
	write(num_bytes,'(i5.5)') next-1
	call outscr(success,'Special translation enabled for '//
	1   num_bytes//' character record.')
	return
	end

	subroutine spectrans(trabufiu,trabufit,trabufo,outpos)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	SPECTRANS.FOR
c	This routine does special translations as specified by the ST command
c	for reading tapes
c
c	Rev. 04-Aug-1989
c	Rev. 19-Sep-1989
c	Rev. 11-Jan-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	character*(*)	trabufiu,trabufit,trabufo
	include 'etape_include.for'
	integer outpos,i,j,k

	parameter	maxfld = 500	    ! Maximum number of fields
	byte		trans_type(maxfld)
	integer*2	start(maxfld),length(maxfld)
	integer		numfld,ibmfvax,istat
	common/sp_trans/numfld,start,length,trans_type

 1001	format(a,' error in:',z8.8,' record ',i6,' position ',i6)
	outpos = 1
	do i = 1,numfld
	    if(trans_type(i).eq.1) then			    ! Character
		trabufo(outpos:outpos+length(i)-1) =
	1	    trabufit(start(i):start(i)+length(i)-1)
	    else if(trans_type(i).eq.2) then		    ! Packed
		trabufo(outpos:outpos+length(i)-1) =
	1	    trabufiu(start(i):start(i)+length(i)-1)
	    else if(trans_type(i).eq.3) then		    ! Binary 4-byte
		do j = outpos,outpos+length(i)-1,4
		    k = start(i) + j - outpos
		    trabufo(j  :j  ) = trabufiu(k+3:k+3)
		    trabufo(j+1:j+1) = trabufiu(k+2:k+2)
		    trabufo(j+2:j+2) = trabufiu(k+1:k+1)
		    trabufo(j+3:j+3) = trabufiu(k  :k)
		    end do
	    else if(trans_type(i).eq.4) then		    ! Binary 2-byte
		do j = outpos,outpos+length(i)-1,2
		    k = start(i) + j - outpos
		    trabufo(j  :j  ) = trabufiu(k+1:k+1)
		    trabufo(j+1:j+1) = trabufiu(k  :k  )
		    end do
	    else if(trans_type(i).eq.5) then		    ! IBM float single
		do j = outpos,outpos+length(i)-1,4
		    istat = ibmfvax(%ref(trabufiu(j:j)),%ref(trabufo(j:j)))
		    if(istat.eq.1) then
			write(ttyout,1001)'Underflow',trabufiu(j:j+3),nout,j
			call outscr(success,ttyout)
		    else if(istat.eq.2) then
			write(ttyout,1001)'Overflow',trabufiu(j:j+3),nout,j
			call outscr(success,ttyout)
		    end if
		    end do
	    end if
	    outpos = outpos + length(i)
	    end do
	outpos = outpos - 1	!Total record length
	return
	end

	subroutine specuntrans(trabufiu,trabufit,trabufo,outpos)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	SPECUNTRANS.FOR
c	This routine does special translations as specified by the ST command
c	when writing tapes
c	Note: The only difference from SPECTRANS is the call to VAXFIBM rather
c	      than IBMFVAX.
c
c	Rev. 25-Jan-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	character*(*)	trabufiu,trabufit,trabufo
	include 'etape_include.for'
	integer outpos,i,j,k

	parameter	maxfld = 500	    ! Maximum number of fields
	byte		trans_type(maxfld)
	integer*2	start(maxfld),length(maxfld)
	integer		numfld,vaxfibm,istat
	common/sp_trans/numfld,start,length,trans_type

 1001	format(a,' error in:',z8.8,' record ',i6,' position ',i6)
	outpos = 1
	do i = 1,numfld
	    if(trans_type(i).eq.1) then			    ! Character
		trabufo(outpos:outpos+length(i)-1) =
	1	    trabufit(start(i):start(i)+length(i)-1)
	    else if(trans_type(i).eq.2) then		    ! Packed
		trabufo(outpos:outpos+length(i)-1) =
	1	    trabufiu(start(i):start(i)+length(i)-1)
	    else if(trans_type(i).eq.3) then		    ! Binary 4-byte
		do j = outpos,outpos+length(i)-1,4
		    k = start(i) + j - outpos
		    trabufo(j  :j  ) = trabufiu(k+3:k+3)
		    trabufo(j+1:j+1) = trabufiu(k+2:k+2)
		    trabufo(j+2:j+2) = trabufiu(k+1:k+1)
		    trabufo(j+3:j+3) = trabufiu(k  :k)
		    end do
	    else if(trans_type(i).eq.4) then		    ! Binary 2-byte
		do j = outpos,outpos+length(i)-1,2
		    k = start(i) + j - outpos
		    trabufo(j  :j  ) = trabufiu(k+1:k+1)
		    trabufo(j+1:j+1) = trabufiu(k  :k  )
		    end do
	    else if(trans_type(i).eq.5) then		    ! IBM float single
		do j = outpos,outpos+length(i)-1,4
		    istat = vaxfibm(%ref(trabufiu(j:j)),%ref(trabufo(j:j)))
		    if(istat.eq.1) then
			write(ttyout,1001)'Underflow',trabufiu(j:j+3),nout,j
			call outscr(success,ttyout)
		    else if(istat.eq.2) then
			write(ttyout,1001)'Overflow',trabufiu(j:j+3),nout,j
			call outscr(success,ttyout)
		    end if
		    end do
	    end if
	    outpos = outpos + length(i)
	    end do
	outpos = outpos - 1	!Total record length
	return
	end

	options	    /check=nooverflow
	integer function ibmfvax(ibm_float,vax_float)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	IBMFVAX.FOR
c	This routine translates IBM single-precision floating point numbers
c	to VAX F-floating numbers.
c	The integer return is a status of the operation:
c	    0 - Successful
c	    1 - Underflow occurred (replaced with zero)
c	    2 - Overflow occurred (replaced with zero)
c
c	Rev. 11-Jan-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	implicit none
	integer characteristic,ioverlay
	byte	ibm_float(4), bioverlay(4), vax_float(4)
	equivalence (ioverlay,bioverlay)

	ibmfvax = 0			! Assume things will go okay

c	Special case for actual zero

	if ((ibm_float(2).eq.0) .and.
	1   (ibm_float(3).eq.0) .and.
	2   (ibm_float(4).eq.0)) then
	    vax_float(1) = 0
	    vax_float(2) = 0
	    vax_float(3) = 0
	    vax_float(4) = 0
	    return
	end if

	characteristic = (((ibm_float(1).and.'7f'x) - 64) * 4) + 128
c	ioverlay = 0
c	bioverlay(1) = ibm_float(1)
c	sign2 = ioverlay/128

	ioverlay = 0
	bioverlay(3) = ibm_float(2)
	bioverlay(2) = ibm_float(3)
	bioverlay(1) = ibm_float(4)
	do while((ioverlay.and.'00800000'x).eq.0)
	    ioverlay = ioverlay * 2
	    characteristic = characteristic - 1
	    end do

	if (characteristic.lt.0) then
	    vax_float(1) = 0
	    vax_float(2) = 0
	    vax_float(3) = 0
	    vax_float(4) = 0
	    ibmfvax = 1
	    return
	else if (characteristic.gt.255) then
	    vax_float(1) = 0
	    vax_float(2) = 0
	    vax_float(3) = 0
	    vax_float(4) = 0
	    ibmfvax = 2
	    return
	end if
	vax_float(1) = (bioverlay(3) .and. '7f'x) .or.
	1   ishft((characteristic.and.'01'x),7)

	vax_float(2) = ishft(characteristic,-1) .or. (ibm_float(1).and.'80'x)
	vax_float(3) = bioverlay(1)
	vax_float(4) = bioverlay(2)

	return
	end

	options	    /check=nooverflow
	integer function vaxfibm(vax_float,ibm_float)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	VAXFIBM.FOR
c	This routine translates VAX F-floating numbers
c	to IBM single-precision floating point numbers.
c	The integer return is a status of the operation:
c	    0 - Successful
c	    1 - Underflow occurred (replaced with zero)
c	    2 - Overflow occurred (replaced with zero)
c
c	Rev. 25-Jan-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	implicit none
	integer characteristic,ioverlay,test
	byte	ibm_float(4), bioverlay(4), vax_float(4)
	equivalence (ioverlay,bioverlay)
	integer for$jibits		! So the argument type is not checked

	vaxfibm = 0			! Assume things will go okay

c	Special case for actual zero

	if ((vax_float(1).eq.0) .and.
	2   (vax_float(2).eq.0) .and.
	3   (vax_float(3).eq.0) .and.
	4   (vax_float(4).eq.0)) then
	    ibm_float(1) = 0
	    ibm_float(2) = 0
	    ibm_float(3) = 0
	    ibm_float(4) = 0
	    return
	end if

	characteristic = for$jibits(vax_float(1),7,8)

	ioverlay = 0
	bioverlay(3) = vax_float(1) .or. '80'x	    ! VAX assumes high order bit
	bioverlay(2) = vax_float(4)
	bioverlay(1) = vax_float(3)

	test = 4 - mod(characteristic,4)
	if (test.ne.4) then
	    ioverlay = ioverlay / (2**test)
	    if(characteristic.gt.128) then
		characteristic = characteristic + 4
	    end if
	end if
	characteristic = (characteristic -128) / 4 + 64

	if (characteristic.lt.0) then
	    ibm_float(1) = 0
	    ibm_float(2) = 0
	    ibm_float(3) = 0
	    ibm_float(4) = 0
	    vaxfibm = 1
	    return
	else if (characteristic.gt.127) then
	    ibm_float(1) = 0
	    ibm_float(2) = 0
	    ibm_float(3) = 0
	    ibm_float(4) = 0
	    vaxfibm = 2
	    return
	end if
	ibm_float(4) = bioverlay(1)
	ibm_float(3) = bioverlay(2)
	ibm_float(2) = bioverlay(3)
	ibm_float(1) = characteristic .or. (vax_float(2).and.'80'x)
	return
	end

	logical function istty(devname)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	ISTTY.FOR
c
c	Returns TRUE if the specified device is a TERMINAL.  Note that if the
c	supplied value is not a valid device name, we will always return false.
c	Rev. 06-Dec-1990
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	implicit none
	character devname*(*)		! Declare formal parameter.

					!==[External Goodies]==================
	include '($dcdef)'              ! GETDVI symbols.
	include '($ssdef)'		! System Service definitions.
	include '($dvidef)'		! GETDVI definitions.
	integer sys$getdviw,		! System routine - Get Device Info.
	1   lib$stop			! RT lib routine - Stop with Message.
					!==[Data Structures]===================
	structure /itmlst/		! VMS usage: item_list_3
	    integer*2 buflen		! Length of response buffer.
	    integer*2 code		! Request for device class
	    integer*4 bufadr		! Address of response buffer
	    integer*4 retlenadr		! Address of response length
	    integer*4 endlist /0/	! Longword to end the list.
	end structure                   !
					!==[Local Storage]=====================
	integer*4 class,		! Response buffer, a 4 byte code.
	1   classlen,			! Response length
	1   status			! Fuction call return status.
					!
	record /itmlst/ dvi_list	! GETDVI transfer vector.
					!======================================

	dvi_list.buflen = 4
	dvi_list.code = dvi$_devclass
	dvi_list.bufadr = %loc(class)
	dvi_list.retlenadr = %loc(classlen)
	status = sys$getdviw(,,devname,dvi_list,,,,,)
	if ((.not. status) .and. (status .ne. ss$_ivdevnam)) then
	    call lib$stop(%val(status))
	else
	    istty = ((class .eq. dc$_term) .or. (class .eq. dc$_workstation))
	endif
	return
	end


	subroutine julday(date)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c	JULDATE.FOR
c
c	This routine supplies the current Julian date in the form YYDDD.
c	It is functionally equivalent to cob$acc_day
c	Rev. 07-Dec-1994
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	implicit none
	character date*(*)
	character currdate*11, lastyear*11
	integer daynow, daylast, year, quad(2)

1001	format(7x,i4)
1002	format('31-DEC-',i4)
1003	format(a2,i3.3)
	call SYS$ASCTIM (, currdate,,)
	call LIB$DAY (daynow,,)
	read(currdate,1001)year
	write(lastyear,1002)year-1
	call SYS$BINTIM (lastyear, quad)
	call LIB$DAY (daylast, quad)
	write(date,1003)currdate(10:11), daynow-daylast
	return
	end
