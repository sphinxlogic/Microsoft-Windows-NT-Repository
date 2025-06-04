$ ! Procedure:	SYS_FIND_NODES.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ facility = procedure_name
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
		+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$ say = "write sys$output"
$ debug_lnm = procedure_name + "_DEBUG"
$ sys_debug = "!"
$ symbol = f$edit(P1,"UPCASE,COLLAPSE")
$ selection = f$edit(P2,"UPCASE,COLLAPSE")
$ if (symbol .eqs. "") then symbol = "SYS_NODE_LIST"
$ if (selection .eqs. "") then selection = "*"
$ if ((P3 .nes. "") .or. f$trnlnm(debug_lnm)) then -
$    sys_debug = "write/symbol sys$output"
$ 'symbol' == ""
$ on control_y then goto bail_out
$
$ nodes_in = f$element(0,"-",selection)
$ select_in = f$element(1,"/",nodes_in)
$ if (select_in .eqs. "/") then select_in = ""
$ nodes_in = f$element(0,"/",nodes_in)
$ if (nodes_in .eqs. "") then nodes_in = "*"
$'sys_debug' "nodes in   <",nodes_in,"/",select_in,">"
$
$ nodes_out = f$element(1,"-",selection)
$ if (nodes_out .eqs. "-") then nodes_out = ""
$ select_out = f$element(1,"/",nodes_out)
$ if (select_out .eqs. "/") then select_out = ""
$ nodes_out = f$element(0,"/",nodes_out)
$ if ((nodes_out .eqs. "") .and. (select_out .nes. "")) then nodes_out = "*"
$'sys_debug' "nodes out  <",nodes_out,"/",select_out,">"
$
$ nodes_dot_dat = "DBSLIBRARY:NODES.DAT"
$ if (f$search(nodes_dot_dat) .eqs. "")
$   then
$   say "%''facility'-F-NONODE, node list file is missing"
$   say "-''facility'-I-GETHELP, please notify someone who cares"
$   exitt 1.or.f$verify(__vfy_saved)
$ endif !(f$search(nodes_dot_dat) .eqs. "")
$
$ default_transport = f$trnlnm("RSTUFF_DEFAULT_TRANSPORT")
$ default_transport = f$edit(default_transport,"COLLAPSE,UPCASE")
$ if (default_transport .eqs. "") then default_transport = "DECNET"
$'sys_debug' "default transport is <",default_transport,">"
$ 
$ node_count = 0
$ open/read/error=end_read_node_loop node_file 'nodes_dot_dat'
$read_node_loop:
$   read/end_of_file=end_read_node_loop node_file this_node_text
$   this_node_text = f$edit(this_node_text,"UPCASE,COLLAPSE,UNCOMMENT")
$   if (this_node_text .eqs. "") then goto read_node_loop
$   if (f$extract(0,5,this_node_text) .eqs. "<END>") then -
$     goto end_read_node_loop
$   node_count = node_count + 1
$   this_node = f$element(0,",",this_node_text)
$   this_node_name = f$element(0,"%",this_node)
$   this_node_transport = f$element(1,"%",this_node)
$   this_node_type = f$element(1,",",this_node_text)
$   if ((this_node_transport .eqs. "%") .or. (this_node_transport .eqs. ""))
$     then this_node_transport = default_transport
$   endif
$   if (this_node_type .eqs. ",") then this_node_type = ""
$   node_'node_count' == "''this_node_name'"
$   node_'this_node_name' == "''this_node_type'"
$   node_'this_node_name'_transport == "''this_node_transport'"
$  goto read_node_loop
$end_read_node_loop:
$ closee/nolog node_file
$
$ all_nodes_in = ""
$ counter = 0
$iloop:
$   counter = counter + 1
$   if (counter .gt. node_count) then goto end_iloop
$   this_node_name = node_'counter'
$   this_node_type = node_'this_node_name'
$   if ((select_in .eqs. "") .or. -
	f$locate(select_in,this_node_type) .ne. f$length(this_node_type))
$     then
$     all_nodes_in = all_nodes_in + "," + this_node_name
$   endif
$  goto iloop
$end_iloop:
$ all_nodes_in = all_nodes_in - ","
$'sys_debug' "select in  <",all_nodes_in,">"
$
$ all_nodes_out = ""
$ if ((nodes_out .nes. "") .or. (select_out .nes. ""))
$   then
$   counter = 0
$oloop:
$     counter = counter + 1
$     if (counter .gt. node_count) then goto end_oloop
$     this_node_name = node_'counter'
$     this_node_type = node_'this_node_name'
$     if ((select_out .nes. "") .and. -
	  f$locate(select_out,this_node_type) .ne. f$length(this_node_type))
$       then
$       all_nodes_out = all_nodes_out + "," + this_node_name
$     endif
$    goto oloop
$end_oloop:
$ endif !((nodes_out .nes. "") .or. (select_out .nes. ""))
$ all_nodes_out = all_nodes_out - ","
$'sys_debug' "select out <",all_nodes_out,">"
$
$ if (nodes_in .eqs. "*") then nodes_in = all_nodes_in
$ real_nodes_in = ""
$ counter = 0
$check_nodes_in_items:
$   item = f$element(counter,",",nodes_in)
$   counter = counter + 1
$   if (item .eqs. "") then goto check_nodes_in_items
$   if (item .eqs. ",") then goto end_check_nodes_in_items
$   if (item .eqs. "*") then item = all_nodes_in
$   real_nodes_in = real_nodes_in + "," + item
$ goto check_nodes_in_items
$end_check_nodes_in_items:
$ nodes_in = real_nodes_in
$'sys_debug' "real in    <",real_nodes_in,">"
$
$ clean_list = ""
$ counter = 0
$cleanup_list:
$   item = f$element(counter,",",nodes_in)
$   counter = counter + 1
$   if (item .eqs. "") then goto cleanup_list
$   if (item .eqs. ",") then goto end_cleanup_list
$   if (f$locate(",''item'",clean_list) .eq. f$length(clean_list)) then -
$	clean_list = clean_list + ",''item'"
$ goto cleanup_list
$end_cleanup_list:
$ nodes_in = clean_list
$'sys_debug' "clean list <",clean_list,">"
$
$ exclude = nodes_out + "," + all_nodes_out
$ counter = 0
$check_exclude_item_loop:
$   item = f$element(counter,",",exclude)
$   counter = counter + 1
$   if (item .eqs. "") then goto check_exclude_item_loop
$   if (item .eqs. ",") then goto end_check_exclude_item_loop
$   if (item .nes. "") then nodes_in = nodes_in - ",''item'"
$ goto check_exclude_item_loop
$end_check_exclude_item_loop:
$
$ nodes_in = "," + nodes_in + ","	! for the next bit...
$strip_leading_commas:
$   nodes_in = nodes_in - ","
$ if (f$extract(0,1,nodes_in) .eqs. ",") then goto strip_leading_commas
$strip_trailing_commas:
$   nodes_in = f$extract(0,f$length(nodes_in)-1,nodes_in)
$ if (f$extract(f$length(nodes_in)-1,1,nodes_in) .eqs. ",") then -
$   goto strip_trailing_commas
$'sys_debug' "...winners <",nodes_in,">"
$ 'symbol' == "''nodes_in'"
$bail_out:
$ exitt 1.or.f$verify(__vfy_saved)
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_FIND_NODES.COM
$ !
$ ! Purpose:	To process a node list and return a "clean" list.
$ !
$ ! Parameters:
$ !	[P1]	The global symbol to define with the result.  Defaults to
$ !		sys_node_list.
$ !	[P2]	The node list, defaults to *.
$ !		General format is:
$ !
$ !		   <include> [/<select>] [ - <exclude> [/<select>] ]
$ !
$ !		where <include> can be * or a comma delimited list
$ !		      <exclude> can be a comma delimited list
$ !		      <select>  is text to match the attributes in the node
$ !				list file
$ !
$ !		Note that it will exclude the current node if a null parameter
$ !		is used, i.e. it will include all other nodes.
$ !	[P3]	If SHOW then will display a list of items included and
$ !		excluded from ths list.  (Anything will work...)
$ !		The logical <procedure_name>_DEBUG will also work.
$ !
$ !	Line format in NODES.DAT is:
$ !
$ !		<nodename>[%<transport>] [,attribute [/attribute...]]
$ !
$ ! History:
$ !		31-Oct-1988, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		24-Jan-1989, DBS; Version V1-002
$ !	002 -	Added verify stuff.
$ !		15-Jun-1989, DBS; Version V1-003
$ !	003 -	Renamed from SYS_NODES to allow for change in parameter order
$ !		to conform to the standard...  Changed P3 from DEBUG to SHOW.
$ !		29-Sep-1989, DBS; Version V1-004
$ !	004 -	Changed to use SYS_NODE as the local node.
$ !		09-Oct-1989, DBS; Version V1-005
$ !	005 -	Changed to use dbsnodes in place of wmc$manager:nodes.dat.
$ !		24-Jun-1994, DBS; Version V1-006
$ !	006 -	Fixed up handling of nodes in a cluster.
$ !		02-Jul-1998, DBS; Version V1-007
$ !	007 -	Now allow the capability to have other things in the node list
$ !		file by extracting the first element of the line (comma
$ !		delimited).
$ !		02-Jul-1998, DBS; Version V1-008
$ !	008 -	Added selection by machine type - see description above.
$ !		14-Sep-1999, DBS; Version V1-009
$ !	009 -	Now allow transport/protocol definition in the node name.
$ !		The node name in nodes.dat can now be <node>[%<transport>].
$ !
$ !		16-Sep-1999, DBS; Version V2-001
$ !	001 -	Major rehash and cleanup.
$ !		Added use of SYS_FIND_NODES_DEFAULT_TRANSPORT.
$ !-==========================================================================
