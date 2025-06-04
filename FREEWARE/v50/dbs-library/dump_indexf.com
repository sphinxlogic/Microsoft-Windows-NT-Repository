$ ! Procedure:	DUMP_INDEXF.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$	set noon
$	on control_y then goto bail_out
$	dih_say = "write sys$output"
$	dih_disk = f$edit(P1, "collapse, upcase")
$	if (dih_disk .eqs. "") then goto missing_param
$	dih_index = f$edit(P2, "collapse, upcase")
$	if (dih_index .eqs. "") then goto missing_param
$	dih_disk = dih_disk - ":" + ":"
$	dih_prefix = dih_disk + "_" + dih_index - ":"
$	dih_indexf_sys = dih_disk + "[000000]indexf.sys"
$	dih_option = "!"
$	dump_contents = 0
$	P3 = f$edit(P3, "collapse, upcase")
$ if (P3 .eqs. "") then goto no_option
$	dump_contents = -1*(P3 .eqs. f$extract(0, f$length(P3), "DUMP"))
$	if (.not. dump_contents) then dih_option = P3
$no_option:
$	dih_output = "''dih_prefix'_HEADER.OUTPUT"
$	srh_output = "''dih_prefix'_SEARCH.OUTPUT"
$	not_text = ""
$	if (.not. dump_contents) then not_text = "NOT "
$	dih_say " "
$	dih_say " Contents of the file will ''not_text'be dumped"
$	dih_say " "
$	dih_cluster = f$getdvi(dih_disk, "cluster")
$	dih_maxfiles = f$getdvi(dih_disk, "maxfiles")
$	dih_block = ((dih_maxfiles + 4095)/4096) + (4*dih_cluster) + dih_index
$	dumpit/file_header/block=(start='dih_block',count=1) -
		'dih_indexf_sys'/output='dih_output'
$	search/nohigh 'dih_output' "dentification:","owner UIC:","File name:"
$ if (.not. dump_contents) then goto try_option
$	search/nohigh 'dih_output' "  Count:"/output='srh_output'
$	open/read/error=bail_out pointers 'srh_output'
$loop:
$	read/end=end_loop/error=end_loop pointers text
$	dih_say " "
$	dih_say "''text'"
$	text = f$edit(text, "upcase, collapse") - "LBN"
$	ret_count = f$element(1, ":", text)
$	ret_start = f$element(2, ":", text)
$	dumpit/blocks=(start='ret_start',count='ret_count') 'dih_disk'
$ goto loop
$end_loop:
$ goto bail_out
$try_option:
$	'dih_option' 'dih_output'
$ goto bail_out
$missing_param:
$	dih_say "%DUMPINDEXF-E-REQPARMIS, a required parameter is missing"
$ goto bail_out
$bail_out:
$	if (f$trnlnm("pointers") .nes. "") then close/nolog pointers
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	DUMP_INDEXF.COM
$ !
$ ! Purpose:	Given a disk and file index number, dump the indexf block in
$ !		a formatted way so we can read it and dump the file up to
$ !		the end of the first lot of retrieval pointers.
$ !
$ ! Parameters:
$ !	 P1	Device name.
$ !	 P2	File index number of the file.
$ !	[P3]	Determines what is done with the output;
$ !		  <null> does nothing with the file
$ !		  DUMP will dump the contents of the disk blocks using the
$ !			retieval pointers from the header
$ !		  anything else will be executed as a command, e.g. TYPE
$ !			,TYPE/PAGE, PRINT and try to use the output of the
$ !			formatted dump as the filename.
$ !
$ ! History:
$ !		28-Jun-1989, DBS; Version V1-001
$ !  	001 -	Original version.  Taken from one that only dumped the index
$ !		entry.
$ !-==========================================================================
