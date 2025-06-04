*******************************************************************************
******** File Splitter            Andy Gould                Sept 1996  ********
*******************************************************************************

	program gsplit

	implicit none

C       The following are ANSI escape sequences used for color enhancement:
C       ( There are some extra colors that are not used in this program )

        character*1     esc /27/
	character*3     blink / '[5m' /
	character*3     normal / '[0m' /
	character*4     bold / '[01m' /
	character*4     blk / '[30m' /
	character*4     red / '[31m' /
	character*4     grn / '[32m' /
	character*4     yel / '[33m' /
	character*4     blu / '[34m' /
	character*4     mag / '[35m' /
	character*4     aqu / '[36m' /
	character*4     wht / '[37m' /
	character*4     bblk / '[40m' /
	character*4     bred / '[41m' /
	character*4     bgrn / '[42m' /
	character*4     byel / '[43m' /
	character*4     bblu / '[44m' /
	character*4     bmag / '[45m' /
	character*4     baqu / '[46m' /
	character*4     bwht / '[47m' /

C       Program variables:

        character*128   bbuff		! Buffer used for batch file creation
	character*64    inf             ! Source file to be split
        character*64    outf            ! Output file names buffer
        character*64    dpath           ! Directory path name for output files
        character*64    dfile           ! Destination file name - extension
        character*64    spath /' '/     ! Source file directory name
	integer*2     	i, j, k         ! General purpose
        integer*2       sdot            ! Location of '.' in source file name
        integer*2       brace           ! Location of end of directory name
        integer*2       getlen          ! Function to get string length
        integer*2       slen            ! Location of destination path end
        integer*2       dlen            ! Length of output file name
        integer*2       dplen           ! Length of output file path
	integer*2       disk            ! Current disk file
        integer*2       ndisks          ! Total number of output disk files
        integer*2       tcnt /0/        ! Temporary file counter for batch
        integer*2       dmax /2800/     ! Maximum number of output blocks
        character*512   abyte           ! Buffer to transfer data
        character*1     ans             ! Disk size query answer

C       Set up header:

        write(6,'(1x,7a/a/a,a,a)') esc,wht,esc,bold,esc,bred,
	1 '                                                ',
	1 '         File Splitter by Andy Gould             ',
	1 '                                                 ',esc,normal 

C       Get diskette size; default 1.44 Megabytes

        write(6,'(1x/1x,7a,$)') esc,blu,esc,bold,
	1 ' Enter Disk Size; 1=.7Mb, 2=2.8Mb, 3(Default<CR>)=1.4Mb : ',
	1  esc,red
        read(6,'(a)') ans
        if (ans .eq. '1') dmax = 1400
        if (ans .eq. '2') dmax = 5600

C       Get source file directory and name; default to local directory:

	write(6,'(1x/1x,7a,$)') esc,blu,esc,bold,
	1   ' Enter Path and Source File Name: ',esc,red
        read(6,'(a)',err=90) inf
        brace = index(inf,']') 
        if (brace .eq. 0) brace = index(inf,':')
        if (brace .gt. 0) spath = inf(1:brace)
        sdot = index(inf(brace+1:),'.') + brace
        dfile = inf(brace+1:sdot)
	open(10, file=inf, status='OLD',
	1    carriagecontrol='NONE',
	1    err= 96)

C       Get destination directory; default same as source:

        write(6,'(1x/1x,3a/3a,$)') esc,blu,
	1  ' S = Same as source path, <CR> = local directory',
	1  '  Enter New Destination Path or S or <CR>: ',esc,red
        read(6,'(a)') dpath
        slen = index(inf,']')
        if (slen .eq. 0) slen = index(inf,':')       
        if (slen .lt. 2) then
          if (dpath(1:1) .eq. 'S' .or. dpath(1:1) .eq. 's') then
             dpath = spath
          else
             dpath = ' '
          endif
        endif

        dplen = getlen(dpath)
        dlen = getlen(dfile)

        write(6,'(1x/1x,2a)') esc,yel

C       Split the file into sub-files:

	do disk = 1, 64
          write(outf,'(a,a,i3.3)') dpath(1:dplen), dfile(1:dlen), disk
          k = getlen(outf)
          write(6,'(a,a,a)')' - Creating Disk File ',outf(1:k),' .....'
	  
          open(11, file=outf,
	1  carriagecontrol='NONE',
	1  status='UNKNOWN',access='DIRECT',recl=128, err=97)

          ndisks = disk

	  do k = 1, dmax
            read(10,'(a)',err=98, end=66) abyte
            write(11, rec=k ,err=99) abyte
	  enddo
          
          close(11)

	enddo

66      write(6,'(2x/2x,2a,i4,3a/)') esc,blu,ndisks,
	1  ' Disk files Created',esc,normal

C       Ask if DOS batch file generation wanted:

        write(6,'(1x/1x,7a,$)') esc,blu,esc,bold,
	1 ' Create DOS Concatenation File ? N, Y(<CR>def) : ',esc,red
        read(6,'(a)') ans
        if (ans .eq. 'N' .or. ans .eq. 'n') go to 100
        write(6,'(1x,2a/)') esc,normal

C       Create DOS batch file:

        open(11, file=dpath//dfile(1:4)//'join.bat',
	1  status='UNKNOWN',err=97)

        write(11,'(a)',err=91) '** Files Concatenator  - Andy Gould **'
       
        do disk = 1, ndisks, 4
          write(bbuff,'(a,a,i3.3)') ' Copy /b ', dfile(1:dlen), disk
          do i = 1, 3
            j = disk + i
            if (j .le. ndisks ) then
              k = getlen(bbuff)
              write(bbuff(k+1:),'(a,a,i3.3)') '+', dfile(1:dlen), j
            endif
          enddo
          if (ndisks .gt. 4) then
            tcnt = tcnt + 1
            k = getlen(bbuff)
            write(bbuff(k+1:),'(a,i2.2)') ' ag.', tcnt 
          else
            k = getlen(bbuff)
            write(bbuff(k+1:),'(a,a)') ' ',inf(brace+1:sdot+3) 
          endif
          k = getlen(bbuff)
          write(11,'(a)',err=91) bbuff(1:k)
        enddo

        if (ndisks .gt. 4) then
          write(bbuff,'(a,i2.2)') ' Copy /b  ag.', 1
          do i = 2, tcnt
            k = getlen(bbuff)
            write(bbuff(k+1:),'(a,i2.2)') '+ag.',tcnt 
          enddo
          k = getlen(bbuff)
          write(bbuff(k+1:),'(1x,a,a,a)')
	1   dpath(1:dplen),dfile(1:dlen),'exe' 
          k = getlen(bbuff)
          write(11,'(a)',err=91) bbuff(1:k)

          do i = 1, tcnt
            write(11,'(a,i3.3)',err=91)' Del ag.', i
          enddo
        endif
       
        close(11)

        write(6,'(1x/1x,10a)') esc,blu,esc,bold,
	1 ' DOS batch file ' ,dfile(1:4),'join.bat created - ',
	1   esc,blink,'Good Luck !'

	goto 100

90	type *, 'SPLIT Source Filename Error'
	goto 100

91      type *, 'SPLIT Batch file Write Error'
        goto 100

96	type *, 'SPLIT Source File Open Error'
	goto 100

97      type *, 'SPLIT Destination File Open Error'
        goto 100

98      type *, 'SPLIT Read Error'
        goto 100

99	type *, 'SPLIT Write Error'

100	close(10)
	close(11)
        write(6,'(1x,2a/)') esc,normal
	stop 'SPLIT Done'

	end

C       Function to get last non-space character position from string:

	integer function getlen(buffer)

        implicit none

        character*(*) buffer
        integer*2     i, nlen

        nlen = len(buffer)

        do i = nlen, 1, -1
          getlen = i
          if (buffer(i:i) .ne. ' ') goto 100
        enddo

100     return

        end

