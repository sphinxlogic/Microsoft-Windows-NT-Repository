$! File created by DCL_DIET at 16-SEP-1999 06:26:51.86 from
$! DBS0:[COM]SEND_IT.COS;
$__vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$if (f$type('__vfy') .eqs. "") then __vfy = 0
$__vfy_saved = f$verify(&__vfy)
$procedure = f$element(0,";",f$environment("PROCEDURE"))
$procedure_name = f$parse(procedure,,,"NAME")
$facility = procedure_name
$location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$set noon
$on control_y then goto bail_out
$say = "write sys$output"
$src_file_list = f$edit(P1,"COLLAPSE,UPCASE")
$nodes = f$edit(P2,"COLLAPSE,UPCASE")
$dst_file_list = f$edit(P3,"COLLAPSE,UPCASE")
$access_name = f$edit(P4,"COLLAPSE,UPCASE")
$if (P1 .eqs. "") then goto bail_out
$ask = "@DBSLIBRARY:SYS_ASK"
$if (nodes .eqs. "") then nodes = "*"
$flush = "set output_rate"
$ss$_unsupported = 3658
$if (f$mode() .eqs. "INTERACTIVE") then flush = "!"
$if (f$verify())
$then
$define_nl = "!"
$deassign_nl = "!"
$log_option = "/log"
$else
$define_nl = "define"
$deassign_nl = "deassign"
$log_option = "/nolog"
$endif
$this_node = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,UPCASE")
$node_alias = f$trnlnm("SYS$CLUSTER_NODE","LNM$SYSTEM_TABLE") - "_" - ":" - ":"
$run dbslibrary:get_cluster_membership
$nodes_to_skip = "," + f$trnlnm("CLUSTER_MEMBERSHIP") + "," -
+ this_node + "," + node_alias + ","
$pid = f$getjpi("","PID")
$user_name = f$edit(f$getjpi("","USERNAME"),"COLLAPSE,UPCASE")
$if (access_name .eqs. "") then access_name = user_name
$ask send_password "''access_name''s password" "" ENSZ
$password = send_password
$deletee/symbol/global send_password
$if (sys__control_y .or. sys__control_z) then goto bail_out
$attempts = 0
$failures = 0
$@dbslibrary:sys_find_nodes node_list "''nodes'"
$default_transport = f$trnlnm("RSTUFF_DEFAULT_TRANSPORT")
$default_transport = f$edit(default_transport,"COLLAPSE,UPCASE")
$if (default_transport .eqs. "") then default_transport = "DECNET"
$node_count = 0
$node_loop:
$node = f$element(node_count,",",node_list)
$node_count = node_count + 1
$if (node .eqs. ",") then goto end_node_loop
$if (node .eqs. "") then goto node_loop
$if (f$locate(",''node',",nodes_to_skip) .eq. f$length(nodes_to_skip))
$then
$transport = default_transport
$__transport = "NODE_''node'_TRANSPORT" - "." - "." - "."
$if (f$type('__transport') .nes. "") then transport = &__transport
$gosub send_files
$else
$say "%''facility'-I-WONT, not sending to node ''node'"
$endif
$goto node_loop
$end_node_loop:
$attempts_plural = " was"
$failures_plural = ""
$if (attempts .ne. 1) then attempts_plural = "s were"
$if (failures .ne. 1) then failures_plural = "s"
$final_message = "''attempts' file''attempts_plural'" -
+ " sent with ''failures' failure''failures_plural'"
$say f$fao("(!8%T) %''facility'-I-SUMMARY, !AS",0,final_message)
$bail_out:
$exitt 1+(0*'f$verify(__vfy_saved)')
$send_files:
$file_count = 0
$src_file_list_loop:
$src_file_spec = f$element(file_count,",",src_file_list)
$dst_file_spec = f$element(file_count,",",dst_file_list)
$file_count = file_count + 1
$if (src_file_spec .eqs. ",") then goto end_src_file_list_loop
$if (src_file_spec .eqs. "") then goto src_file_list_loop
$if (dst_file_spec .eqs. ",") then dst_file_spec = ""
$last_file = ""
$src_node = f$parse(src_file_spec,,,"NODE","SYNTAX_ONLY") - "::"
$gosub process_file_spec
$goto src_file_list_loop
$end_src_file_list_loop:
$return
$process_file_spec:
$dst_node = f$parse(dst_file_spec,,,"NODE","SYNTAX_ONLY")
$dst_file_spec = dst_file_spec - dst_node - "::"
$tmp_file_spec = dst_file_spec
$dst_device = f$element(0,":",tmp_file_spec)
$if (dst_device .eqs. tmp_file_spec) then dst_device = ""
$if (dst_device .nes. "") then dst_device = dst_device + ":"
$tmp_file_spec = tmp_file_spec - dst_device
$dst_directory = f$element(0,"]",tmp_file_spec)
$if (dst_directory .eqs. tmp_file_spec) then dst_directory = ""
$if (dst_directory .nes. "") then dst_directory = dst_directory + "]"
$tmp_file_spec = tmp_file_spec - dst_directory
$search_loop:
$source = f$search(src_file_spec)
$if ((source .eqs. "") .or. (source .eqs. last_file)) then -
$goto end_search_loop
$last_file = source
$file_prot = f$file_attributes(source,"PRO")
$file_size = f$file_attributes(source,"EOF")
$src_directory = f$parse(source,,,"DIRECTORY")
$if ((dst_directory .eqs. "") .and. (dst_device .eqs. "")) then -
$dst_directory = src_directory
$src_device = f$parse(source,,,"DEVICE")
$if (dst_device .eqs. "") then dst_device = src_device
$src_name = f$parse(source,,,"NAME")
$src_type = f$parse(source,,,"TYPE")
$default_destination = src_name + src_type
$dst_name = f$parse(dst_file_spec,default_destination -
,,"NAME","SYNTAX_ONLY")
$dst_type = f$parse(dst_file_spec,default_destination -
,,"TYPE","SYNTAX_ONLY")
$destination = node + """''access_name' ''password'""::" -
+ dst_device + dst_directory -
+ dst_name + dst_type + ";"
$say f$fao("(!8%T) %To !6AS from !AS  (!UL block!%S)" -
,0,node,source,file_size)
$flush
$attempts = attempts + 1
$define_nl sys$output nl:
$define_nl sys$error nl:
$if (transport .eqs. "IP")
$then
$copying/ftp'log_option' 'source' 'destination'
$copy_status = $status
$else
$if (transport .eqs. "DECNET")
$then
$copying'log_option' 'source' 'destination' /protection=('file_prot')
$copy_status = $status
$else
$copy_status = ss$_unsupported
$endif
$endif
$deassign_nl sys$error
$deassign_nl sys$output
$if (.not. copy_status)
$then
$say "  ------>>> last copy failed"
$say f$message(copy_status)
$failures = failures + 1
$flush
$endif
$goto search_loop
$end_search_loop:
$end_process_file_spec:
$return
