	OPTIONS /extend_source
	PROGRAM show_users

C
C PROGRAM DESCRIPTION:
C
C	Using $GETJPI and $PROCESS_SCAN to Select Process Information by
C	User Name.  This program uses SHOW_USERS.CLD to define the command
C	environment.  Valid parameters and qualifiers are:
C
C	    Parameters		Descreiption
C	    ----------		-----------------------------------------
C	    P1			The user name to search for      (PREFIX)
C
C	    Qualifiers		Description
C	    ----------		------------------------------------------
C	    CONFIRM		Confirm the processes to be
C				KILLed or FORCEXited.
C	    IMAGE=image		Search for an active image.     (WILDCARD)
C	    FORCEX		Force an image run-down.
C	    KILL		Kill (stop/id)
C	    NODE[=nodename]	Search only the specified node.    (EXACT)
C	    SYSTEM		Display JPI on the system
C				processes as well.
C	    TOTAL		Display total items found.
C
C AUTHORS:
C
C	 Brian VandeMerwe
C
C CREATION DATE: 	10-Mar-1992
C
C
C	     C H A N G E   L O G
C
C	Date     | Name  | Description
C----------------+-------+-----------------------------------------------------
C  14-Sep-1995	 | BVM	 | Added /KILL, /FORCEX, and /CONFIRM
C

	Implicit Integer ( a - z )

	Include '($jpidef)  /Nolist'
	Include '($pscandef)/Nolist'
	Include '($ssdef)   /Nolist'

	Integer*4	SYS$decprc
	Integer*4	SYS$exit
	Integer*4	SYS$forcex
	Integer*4	SYS$getjpiw
	Integer*4	SYS$process_scan
	Integer*4	status
	Integer*4	context
	Integer*4	pid
	Integer*2	iosb(4)

	Character*23	abs_time
	Character*6	current_node
	Character*6	day_mon
	Character*5	hr_min
	Character*60	imagename
	Character*60	new_imagename
	Character*8	nodename
	Character*8	old_nodename
	Character*16	prcnam
	Character*40	srch_image
	Character*12	srch_node
	Character*12	srch_user
	Character*12	username
	Character*1	yes_no

	Character*4	blink
	Character*4	bold
	Character*4	normal
	Character*4	reverse
	Character*4	underline

	Integer*2	nodename_len
	Integer*2	prcnam_len
	Integer*2	procname_len

	Integer*4	logintime

	Logical*4	done
	Logical		list_all_jobs
	Logical		total

	Structure /jpiitmlst/
	    Union
		Map
		    Integer*2	buflen
		    Integer*2	code
		    Integer*4	bufadr
		    Integer*4	retlenadr
		End Map
		Map
		    Integer*4	end_list
		End Map
	    End Union
	End Structure

	Structure /pscanitmlst/
	    Union
		Map
		    Integer*2	buflen
		    Integer*2	code
		    Integer*4	bufadr
		    Integer*4	itmflags
		End Map
		Map
		    Integer*4	end_list
		End Map
	    End Union
	End Structure

	Record /pscanitmlst/	pscanlist(4)
	Record /jpiitmlst/	jpilist(12)

	!
	! screen attributes.
	!
	blink	    = char(27) // '[5m'
	bold	    = char(27) // '[1m'
	normal	    = char(27) // '[0m'
	reverse	    = char(27) // '[7m'
	underline   = char(27) // '[4m'

	!
	! get the first characters of the users to look for.
	!
	If (CLI$present('users')) Then
	    Call CLI$get_value('users', srch_user)
	Endif

	!
	! get the name of the image to scan for.
	!
	If (CLI$present('image')) Then
	    Call CLI$get_value('image', srch_image)
	Endif

	!
	! get the name of the node to look for.
	!
	If (CLI$present('node')) Then
	    !
	    ! get the node this process is running on.
	    !
	    Call get_jpi_info(current_node)

	    !
	    ! see if the user has specified a node to search.
	    !
	    Call CLI$get_value('node', srch_node)
	Endif

	!
	! see if the user wants to see system information as well.
	!
	If (CLI$present('system')) Then
	    list_all_jobs = .True.
	Else
	    list_all_jobs = .False.
	Endif

	!
	! check for a total
	!
	If (CLI$present('total')) Then
	    total = .True.
	Else
	    total = .False.
	Endif

	!
	! remove all tabs and null characters.
	!
	Call BVM$_compress(srch_user)
	Call BVM$_compress(srch_image)
	Call BVM$_compress(srch_node)

	!
	! if the user specified "/NODE" but did not include a node to search,
	! then search the current node.
	!
	If (srch_node .Eq. ' ' .And. CLI$present('node'))
     +	  srch_node = current_node

	!
	! get the length of the strings to be used.
	!
	Call STR$trim(srch_user,    srch_user,	srch_user_len)
	Call STR$trim(srch_image,   srch_image,	srch_image_len)
	Call STR$trim(srch_node,    srch_node,	srch_node_len)

	!
	! define the scan context.
	!
	pscanlist(1).buflen	= 0
	pscanlist(1).code	= pscan$_node_csid
	pscanlist(1).bufadr	= 0
	pscanlist(1).itmflags	= pscan$m_neq

	!
	! if the user did not use the "/NODE" qualifier, then search all nodes,
	! else search the given node.
	!
	If (srch_node_len .Eq. 0) Then
	    pscanlist(2).buflen	    = 1
	    pscanlist(2).code	    = pscan$_nodename
	    pscanlist(2).bufadr	    = %loc(' ')
	    pscanlist(2).itmflags   = pscan$m_neq
	Else
	    pscanlist(2).buflen	    = srch_node_len
	    pscanlist(2).code	    = pscan$_nodename
	    pscanlist(2).bufadr	    = %loc(srch_node)
	    pscanlist(2).itmflags   = 0
	Endif

	!
	! if the user did not select a username to search, then search all
	! users, else search for the specified username.
	!
	If (srch_user_len .Eq. 0) Then
	    pscanlist(3).buflen	    = 1
	    pscanlist(3).code	    = pscan$_username
	    pscanlist(3).bufadr	    = %loc(' ')
	    pscanlist(3).itmflags   = pscan$m_neq
	Else
	    pscanlist(3).buflen	    = srch_user_len
	    pscanlist(3).code	    = pscan$_username
	    pscanlist(3).bufadr	    = %loc(srch_user)
	    pscanlist(3).itmflags   = pscan$m_prefix_match
	Endif

	pscanlist(4).end_list   = 0

	!----------------------------------------
	!
	! start the process scan.
	!
	status = SYS$process_scan(context, pscanlist)

	!
	! display the error if any.
	!
	If (.Not. status) Call LIB$signal(%val(status))

	old_nodename = ' '

	!
	! get the JPI and keep looping until there are no more process to
	! search.
	!
	done = .False.
	Do While (.Not. done)
	    !
	    ! get the PID
	    !
	    jpilist(1).buflen	    = 4
	    jpilist(1).code	    = jpi$_pid
	    jpilist(1).bufadr	    = %loc(pid)
	    jpilist(1).retlenadr    = 0

	    !
	    ! get the USERNAME
	    !
	    jpilist(2).buflen	    = len(username)
	    jpilist(2).code	    = jpi$_username
	    jpilist(2).bufadr	    = %loc(username)
	    jpilist(2).retlenadr    = %loc(username_len)

	    !
	    ! get the PROCESS NAME
	    !
	    jpilist(3).buflen	    = len(prcnam)
	    jpilist(3).code	    = jpi$_prcnam
	    jpilist(3).bufadr	    = %loc(prcnam)
	    jpilist(3).retlenadr    = %loc(prcnam_len)

	    !
	    ! get the NODENAME
	    !
	    jpilist(4).buflen	    = len(nodename)
	    jpilist(4).code	    = jpi$_nodename
	    jpilist(4).bufadr	    = %loc(nodename)
	    jpilist(4).retlenadr    = %loc(nodename_len)

	    !
	    ! get the JOBTYPE:	    (interactive, batch, detached...)
	    !
	    jpilist(5).buflen	    = 4
	    jpilist(5).code	    = jpi$_jobtype
	    jpilist(5).bufadr	    = %loc(jobtype)
	    jpilist(5).retlenadr    = 0

	    !
	    ! get the CPU TIME:	    (current CPU ticks)
	    !
	    jpilist(6).buflen	    = 4
	    jpilist(6).code	    = jpi$_cputim
	    jpilist(6).bufadr	    = %loc(cputime)
	    jpilist(6).retlenadr    = 0

	    !
	    ! get the IMAGE NAME
	    !
	    jpilist(7).buflen	    = 60
	    jpilist(7).code	    = jpi$_imagname
	    jpilist(7).bufadr	    = %loc(imagename)
	    jpilist(7).retlenadr    = 0

	    !
	    ! get the BASE PRIORITY
	    !
	    jpilist(8).buflen	    = 4
	    jpilist(8).code	    = jpi$_prib
	    jpilist(8).bufadr	    = %loc(prio_base)
	    jpilist(8).retlenadr    = 0

	    !
	    ! get the LOGIN TIME
	    !
	    jpilist(9).buflen	    = 8
	    jpilist(9).code	    = jpi$_logintim
	    jpilist(9).bufadr	    = %loc(logintime)
	    jpilist(9).retlenadr    = 0

	    jpilist(10).end_list    = 0

	    !
	    ! get the JPI from the JPILIST.
	    !
	    status = SYS$getjpiw (%val(1), context,, jpilist, iosb,,)

	    If (status) status = iosb(1)

	    !
	    ! display information on batch, local, dialup, and remote processes
	    ! unless "/SYSTEM" was specified, then display all processes.  If
	    ! the "/IMAGE" qualifier was specified, then search the image name
	    ! for any part of the specified image string.
	    !
	    If (
     +	      status

     +	      .And.

     +	      (jobtype .Ge. 2 .Or. list_all_jobs)

     +	      .And.

     +	      (index(imagename, srch_image(1:srch_image_len)) .Gt. 0 .Or.
     +	      srch_image_len .Eq. 0)

     +	      ) Then

		total_count = total_count + 1

		!
		! check for a different nodename and display the heading.
		!
		If (nodename .Ne. old_nodename) Then
		    old_nodename = nodename
		    Write (6, '(/, x, ''System: '', a4, a, a4, /)',
     +		      iostat = ios_write)
     +		      bold, nodename(1:nodename_len), normal

		    !
		    ! display the title
		    !
		    Write (6, '(x, a4, ''Pid'', 6x, ''Username'', x,
     +		      ''Process name'', x, ''Pr'', x, ''CPU'', 4x,
     +		      ''Login time'', 3x, ''Image'', 21x, a4)')
     +		      underline, normal
!		    Write (6, '(x, 8(''-''), x, 8(''-''), x, 12(''-''), x,
!     +		      ''--'', x, 6(''-''), x, 12(''-''), x, 26(''-''))')
		Endif

		!
		! remove spaces, tabs, and null's from the strings.
		!
		Call BVM$_compress(username)
		Call BVM$_trim(prcnam)
		Call BVM$_compress(imagename)

		!
		! this section will "PARSE" the imagename for just the
		! filename.
		!
		Call BVM$_count_char(count, '>', imagename)
		Call STR$element(new_imagename, count, '>', imagename)
		Call BVM$_count_char(count, ']', new_imagename)
		Call STR$element(new_imagename, count, ']', new_imagename)
		Call STR$element(new_imagename, 0, '.', new_imagename)

		!
		! convert the logintime to plain english. (now theres a
		! contridictory in terms)
		!
		Call SYS$asctim(time_len, abs_time, logintime, 0)

		!
		! all we need to display about the logintime is, day, month,
		! hour, and minute the user logged in.
		!
		day_mon = abs_time(1:6)
		hr_min = abs_time(13:17)

		!
		! KILL and/or FORCEX the processes being shown.
		!
		If (CLI$present('kill') .Or. CLI$present('forcex')) Then
		    !
		    ! Test for a confirmation
		    !
		    If (CLI$present('confirm')) Then
			Write (6, '(x, z8.8, x, a8, x, a12, x, i2, x, i6, x, a6,
     +			  x, a5, x, a18, '' [N]: '', $)', iostat = ios_write)
     +			  pid, username, prcnam, prio_base, cputime, day_mon, hr_min,
     +			  new_imagename

			yes_no = ' '
			Read (5, '(a1)') yes_no

			!
			! Should we really KILL/FORCEX a process?
			!
			If (yes_no(1:1) .Eq. 'y' .Or. yes_no(1:1) .Eq. 'y') Then
			    If (CLI$present('forcex')) Then
				status = SYS$forcex(pid,,)
				If (status .Ne. 1) Call SYS$exit (status)
			    Endif
			    If (CLI$present('kill')) Then
				status = SYS$delprc(pid,)
				If (status .Ne. 1) Call SYS$exit (status)
			    Endif
			Endif
		    Else
			!
			! No confirmation, so just DO IT...
			!
			Write (6, '(x, z8.8, x, a8, x, a12, x, i2, x, i6, x, a6,
     +			  x, a5, x, a26)', iostat = ios_write)
     +			  pid, username, prcnam, prio_base, cputime, day_mon, hr_min,
     +			  new_imagename

			If (CLI$present('forcex')) Then
			    status = SYS$forcex(pid,,)
			    If (status .Ne. 1) Call SYS$exit (status)
			Endif
			If (CLI$present('kill')) Then
			    status = SYS$delprc(pid,)
			    If (status .Ne. 1) Call SYS$exit (status)
			Endif
		    Endif
		Else
		    !
		    ! No KILL or FORCEX so just display the information.
		    !
		    Write (6, '(x, z8.8, x, a8, x, a12, x, i2, x, i6, x, a6,
     +		      x, a5, x, a26)', iostat = ios_write)
     +		      pid, username, prcnam, prio_base, cputime, day_mon, hr_min,
     +		      new_imagename
		Endif
	    Else If (status .Eq. ss$_nomoreproc) Then
		!
		! prepare to exit program if there are no more process to
		! search.
		!
		done = .True.
	    Else If (.Not. status) Then
!		Call Lib$signal(%val(status))
	    Endif

	Enddo

	If (total) Write (6, '(/, '' Total items: '', i4)') total_count

	End


!-------------------------------------------------------------------------------
! Subroutine to count specified characters in a given string.
!-------------------------------------------------------------------------------
	Subroutine BVM$_count_char (found, element, text)

	Implicit Integer ( a - z )

	Character*1	element
	Character*(*)	text

	!
	! get the length of the string
	!
	Call STR$trim(text, text, text_len)

	!
	! search the string "TEXT" and count the number of times the "ELEMENT"
	! was found.
	!
	found = 0
	Do kntr = 1, text_len
	    If (text(kntr:kntr) .Eq. element) found = found + 1
	Enddo

	Return
	End


!-------------------------------------------------------------------------------
! Subroutine to remove all spaces and tabs from the in_text parameter.
!-------------------------------------------------------------------------------
	Subroutine BVM$_compress (in_text)

	Character*(*)	in_text
	Character*200	holder

	!
	! define a "holding tank" to manipulate the text
	!
	holder = in_text

	len1 = len(holder)
	len2 = len(in_text)
	in_text = ' '

	!
	! scan the string.
	!
	kntr1 = 1
	Do kntr2 = 1, len2

	    !
	    ! remove all spaces, tabs, and nulls
	    !
	    If (.Not. (holder(kntr2:kntr2) .Eq. ' ' .Or.
     +	      holder(kntr2:kntr2) .Eq. char(9) .Or.
     +	      holder(kntr2:kntr2) .Eq. char(0))) Then
		in_text(kntr1:kntr1) = holder(kntr2:kntr2)
		kntr1 = kntr1 + 1
	    Endif
	Enddo

	Return
	End


!-------------------------------------------------------------------------------
! Subroutine to remove all tabs and nulls from the in_text parameter.
!-------------------------------------------------------------------------------
	Subroutine BVM$_trim (in_text)

	Character*(*)	in_text
	Character*200	holder

	!
	! define a "holding tank" to manipulate the text
	!
	holder = in_text

	len1 = len(holder)
	len2 = len(in_text)
	in_text = ' '

	!
	! scan the string.
	!
	kntr1 = 1
	Do kntr2 = 1, len2

	    !
	    ! remove all tabs and nulls
	    !
	    If (.Not. (
     +	      holder(kntr2:kntr2) .Eq. char(9) .Or.
     +	      holder(kntr2:kntr2) .Eq. char(0))) Then
		in_text(kntr1:kntr1) = holder(kntr2:kntr2)
		kntr1 = kntr1 + 1
	    Endif
	Enddo

	Return
	End


!-------------------------------------------------------------------------------
! Section to get the current node on which this program is being executed.
!-------------------------------------------------------------------------------
	OPTIONS /extend_source
	Subroutine get_jpi_info (return_node)

	Implicit Integer ( a - z )

	Include '($jpidef)'

	Character*6	usrs_node
	Character*6	return_node

	Integer*4	usrs_node_len

	!
	! define the Structure used for the GETJPI function
	!
	Structure /itmlst/
	    Union
		Map
		    Integer*2	buflen, itmcod
		    Integer*4	bufadr, retadr
		End Map
		Map
		    Integer*4	end_list
		End Map
	    End Union
	End Structure    !itmlst

	!
	! define the Structure for the i/o status block
	!
	Structure /iosblk/
	    Integer*4	sts, zeroed
	End Structure    !iosblk

	!
	! create a record element for the GETJPI function to use.
	!
	Record /itmlst/ jpi_list(2)
	Record /iosblk/ iosb

	!
	! return the nodename
	!
	jpi_list(1).buflen = len(usrs_node)
	jpi_list(1).itmcod = jpi$_nodename
	jpi_list(1).bufadr = %loc(usrs_node)
	jpi_list(1).retadr = %loc(usrs_node_len)

	jpi_list(2).end_list = 0

	!
	! retrieve the information.
	!
	result = SYS$getjpiw (,,, jpi_list, iosb,,)

	Write (return_node, '(a)') usrs_node(1:usrs_node_len)

	Return
	End
