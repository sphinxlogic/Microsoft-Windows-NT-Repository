cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c	Include file for ETAPE
c	Rev. 1.1  - 13-Apr-1988
c	Rev. 1.2  - 14-Jun-1989
c	Rev. 2.5  - 28-Jul-1989
c	Rev. 2.12 - 06-Dec-1990
c	Rev. 2.17 - 03-Feb-1994
c	Rev. 2.18 - 17-Apr-1994
c	Rev. 2.19 - 29-Apr-1994
c	Rev. 2.20 - 29-Nov-1994
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	parameter   maxblock= 65535	    ! Largest tape blocksize
	parameter   maxrec  = 32763	    ! Maximum disk recordsize
	parameter   ebcdic  = 1
	parameter   ascii   = 2
	parameter   gcosbcd = 3
	PARAMETER   Warning = 0
	PARAMETER   Success = 1
	PARAMETER   Error   = 2
	PARAMETER   Info    = 3
	PARAMETER   Fatal   = 4

	character   filename*80,volname*6,phyname*40,logname*32,recfm*2
	character   buffer*(maxblock),record*(maxblock)
	character   buffer2*(maxblock),record2*(maxblock)
	character   device*80,specfile*80
	character   ttyout*80
	character   comment*78

	logical     at_end,at_begin,to_tty,incr_file,getoutnow,errproc
	logical     refresh,display,labeled,command_mode,doing_special

	integer*4   iosb4(2),phyname_len,nblock,eofcnt,lib_index,density
	integer*4   display_id_1,display_id_2,display_id_3,display_id_4
	integer*4   pasteboard,keyboard_id
	integer*4   ttyin,filein,fileout,filespec
	integer*4   retcode,bigcount,blocksize,block,file,nout,lrecl
	integer*4   log_len,devlen,sofar,volnum,mode,mount_flags

	integer*2   iosb(4),channel,status,count
	integer*2   line_width,lpp

	equivalence (status,iosb(1),iosb4(1)),(count,iosb(2))

c	Define item list structure
	structure	/itmlst/
	    union
		map
		    integer*2	buffer_len
		    integer*2	item_code
		    integer*4	buffer_address
		    integer*4	ret_len_adr
		    end map
		map
		    integer*4	end_list
		    end map
		end union
	    end structure

	record/itmlst/ mount_itemlist(4)

	common /etape_int4/ iosb4, phyname_len, nblock, eofcnt, lib_index,
	1   density, display_id_1, display_id_2, display_id_3,
	2   display_id_4, pasteboard, keyboard_id, ttyin, filein, fileout,
	3   filespec, retcode, bigcount, blocksize, block, file, nout,
	4   lrecl, log_len, devlen, sofar, volnum, mode, mount_flags

	common /etape_int2/ channel, line_width, lpp

	common /etape_logical/ at_end, at_begin, to_tty, incr_file,
	1   getoutnow, errproc, refresh, display, labeled, command_mode,
	2   doing_special

	common /etape_char/ filename, volname, phyname, logname, recfm,
	1   buffer, record, buffer2, record2, device, specfile, ttyout,
	1   comment

	common/mount_itm/ mount_itemlist
