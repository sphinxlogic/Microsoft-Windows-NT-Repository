$ 	verify='f$verify(0)
$ 	exit="exit 1+0*f$verify(verify)"
$!
$!	COPYRIGHT (C) 1986, 1988
$!
$!	DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS
$!
$!	This software is furnished  under a license for use only on a single
$!	computer  system and  may be  copied only with  the inclusion of the
$!	above Copyright notice.  This software, or any other copies thereof,
$!	may not be provided or  otherwise made available to any other person
$!	except for use on such system and to one who agrees to these license
$!	terms.  Title to and  ownership of the  software  shall at all times
$!	remain in DEC.
$!
$!	The information in this software is subject to change without notice
$!	and should  not be construed  as a commitment  by Digital  Equipment
$!	Corporation.
$!
$!	DEC assumes  no  responsibility  for  the use or  reliability of its
$!	software on equipment which is not supplied by DEC.
$!
$!	Title		Procedure MAILDIR
$!
$!	Abstract	Procedure to provide directory listing of all
$!			a user's mail folders
$!
$!	Author		R. A. Bishop
$!
$!	Environment	Any VMS/MicroVMS system, V4.4 or later
$!
$!	Parameters	P1=""		provides author, date and subject
$!
$!			P1="FULL"	provides author, date, subject,
$!					record count, file identifier
$!					and replied/new flags. The folder 
$!					name is appended to each record
$!
$!			P2=""		uses the default mail file for 
$!					the user
$!
$!			P2="directory"	uses the mail file in the specified 
$!					directory. The MAIL.LIS output file 
$!					is renamed to this directory.
$!
$!	Notes	1.	Choice of P1 is equivalent to MAIL's "DIRECTORY" 
$!			or "DIRECTORY/FULL" commands.
$!
$!		2.	The default format uses 80 columns; the "FULL" 
$!			format uses in excess of 132 columns, but is 
$!			suitable for searching.
$!
$	if f$log("verify") .nes. "" then v=f$verify(f$log("verify"))
$	on warning then goto error
$	on control_y then goto error
$	set on
$	page_length=60
$	known_attributes="None,Replied,New message,Marked,Foreign"
$	if f$search("maildir.tmp") .nes. "" then delete maildir.tmp;*
$	if f$search("maildir.wrk") .nes. "" then delete maildir.wrk;*
$	if f$search("maildir.out") .nes. "" then delete maildir.out;*
$	if f$search("mail.lis") .nes. "" then delete mail.lis;*
$	open/write out maildir.wrk
$	if p2 .nes. "" then write out "set file ''p2'"
$	write out "directory/folder"
$	write out "exit"
$	close out
$	define/user sys$input maildir.wrk
$	define/user sys$output maildir.tmp
$	define/user mail$init nl:
$	mail
$	open/read in maildir.tmp
$	open/write out maildir.wrk
$	if p2 .nes. "" then write out "set file ''p2'"
$	start="Listing of folders in "
$loop1:
$	read/err=error in line
$	if f$locate(start,line) .eq. f$length(line) then goto loop1
$	mailfile=line-start
$	read/err=error in line
$loop2:
$	read/end=end1/err=error in line
$	line=f$edit(line,"trim,compress")
$	line1=f$element(0," ",line)
$	line2=f$element(1," ",line)
$	write out "select ''line1'"
$	if p1 .eqs. "FULL" then write out "directory/full"
$	if p1 .nes. "FULL" then write out "directory"
$	if line2 .eqs. " " then goto end1
$	write out "select ''line2'"
$	if p1 .eqs. "FULL" then write out "directory/full"
$	if p1 .nes. "FULL" then write out "directory"
$	goto loop2
$end1:
$	close in
$	close out
$	define/user sys$input maildir.wrk
$	define/user sys$output maildir.out
$	define/user mail$init nl:
$	mail
$	open/read in maildir.out
$	open/write out mail.lis
$	lines=0
$	folders=0
$	messages=0
$	records=0
$	blanks=""
$	attr_replied=0
$	attr_new=0
$	attr_foreign=0
$	attr_marked=0
$loop3:
$	read/err=error in line
$	if f$extract(0,1,line) .nes. "%" then goto loop3
$loop4:
$	number=f$integer(f$element(1," ",line))
$	folders=folders+1
$	messages=messages+number
$	if lines+number .le. page_length-6 then goto same_page
$	if lines-((lines/page_length)*page_length) .eq. 0 then -
		blanks=""
$	if lines-((lines/page_length)*page_length) .ne. 0 then -
		blanks=f$fao("!^")
$	lines=number+4
$	goto page
$same_page:
$	if lines .ne. 0 then blanks=f$fao("!/!/")
$	if lines .ne. 0 then lines=lines+2
$	lines=lines+number+4
$page:
$	read/err=error in line
$	folder=f$edit(line,"trim")
$	if p1 .eqs. "FULL"
$	then
$		write out f$fao("!AS!80AS (!SL message!%S)!/",-
			blanks,folder,number)
$	else
$		spaces=69-f$length(folder)-f$length(number)
$		if number .eq. 1 then spaces=spaces+1
$		write out f$fao("!AS!AS!''spaces'< !>(!SL message!%S)!/",-
			blanks,folder,number)
$	endif
$	if f$mode() .eqs. "INTERACTIVE" then write sys$output -
		f$fao("%MAILDIR-I-NEW_FOLDER, started folder !AS (!SL message!%S)",-
		folder,number)
$loop5:
$	read/end=end2/err=error in line
$	if f$extract(0,1,line) .eqs. "%" then goto loop4
$	read/end=end2/err=error in line_ext
$loop5a:
$	if f$extract(0,1,line_ext) .nes. f$fao("!_") then goto write1
$	line_ext=f$edit(line_ext,"trim,compress")
$	read/end=write3/err=error in line_ext_2
$	if f$extract(0,1,line_ext_2) .nes. f$fao("!_") then goto write1a
$	line_ext_2=f$edit(line_ext_2,"trim,compress")
$	attribute_list=line_ext_2-f$element(0," ",line_ext_2)
$	attributes="["
$	element=0
$loop6:
$	attribute=f$edit(f$element(element,",",attribute_list),"trim")
$	if attribute .eqs. "," then goto end3
$	if f$locate(attribute,known_attributes) .eq. f$length(known_attributes) 
$	then 
$		write sys$output "******** Unexpected attribute: ",attribute," ********"
$	else
$		if attribute .nes. "None"
$		then
$			attributes=attributes+","+f$extract(0,1,attribute)
$			attr_'f$element(0," ",attribute)'=attr_'f$element(0," ",attribute)'+1
$		endif
$	endif
$	element=element+1
$	goto loop6
$end3:
$	if attributes .eqs. "[" 
$	then
$		attributes=""
$	else
$		attributes=attributes-","+"]"
$	endif
$	size=f$integer(f$element(0," ",line_ext))
$	if f$type(size) .nes. "INTEGER" then size=0
$	records=records+size
$	size_string=f$fao("(!SL record!%S)",size)
$	quote=f$locate("'",line_ext)
$	ident=f$extract(quote,f$length(line_ext)-quote,line_ext)
$	write out f$fao("!80AS !16AS !18AS !8AS !AS",-
		line,size_string,ident,attributes,folder)
$	goto loop5
$write1:
$	write out line
$	line=line_ext
$	goto write1b
$write1a:
$	write out line
$	line=line_ext_2
$write1b:
$	if f$extract(0,1,line) .eqs. "%" then goto loop4
$	read/end=write2/err=error in line_ext
$	goto loop5a
$write2:
$	write out line
$	goto end2
$write3:
$	write out line
$	write out line_ext
$end2:
$	close in
$	close out
$	if p2 .nes. "" then rename/log mail.lis 'p2'
$	if p1 .nes. "FULL"
$	then
$		write sys$output -
			f$fao("%MAILDIR-I-BRIEF_SUMMARY, "+-
			"!SL message!%S in "+-
			"!SL folder!%S in mail file !AS",-
			messages,folders,mailfile)
$		goto exit
$	endif
$	attribute_totals=" ("
$	if attr_replied .ne. 0 then attribute_totals=attribute_totals+-
		", "+f$string(attr_replied)+" replied"
$	if attr_new .ne. 0 then attribute_totals=attribute_totals+-
		", "+f$string(attr_new)+" new"
$	if attr_marked .ne. 0 then attribute_totals=attribute_totals+-
		", "+f$string(attr_marked)+" marked"
$	if attr_foreign .ne. 0 then attribute_totals=attribute_totals+-
		", "+f$string(attr_foreign)+" foreign"
$	if attribute_totals .eqs. " (" 
$	then
$		attribute_totals=""
$	else
$		attribute_totals=attribute_totals-", "+")"
$	endif
$	write sys$output -
		f$fao("%MAILDIR-I-FULL_SUMMARY, !SL record!%S in "+-
		"!SL message!%S!AS!/!_!_in "+-
		"!SL folder!%S in mail file !AS",-
		records,messages,attribute_totals,folders,mailfile)
$	goto exit
$error:
$	write sys$output "An error has occurred. Aborting...."
$	show symbol/all
$	close/nolog in
$	close/nolog out
$	if f$search("mail.lis") .nes. "" then delete mail.lis;*
$exit:
$	if f$search("maildir.tmp") .nes. "" then delete maildir.tmp;*
$	if f$search("maildir.wrk") .nes. "" then delete maildir.wrk;*
$	if f$search("maildir.out") .nes. "" then delete maildir.out;*
$	exit
