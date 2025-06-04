$! File: UPLOAD_ALL_SERVERS.COM
$!
$! This procedure produces an upload of all servers in the TSM
$! database that are currently reachable
$!
$!
$! 17-Feb-1997	J. Malmberg
$!
$!===================================================================
$!
$!
$! Set up initial conditions
$!--------------------------
$pid = f$getjpi("","PID")
$temp_file1 = "TSM_SERVER_1_''pid'.TMP"
$temp_file2 = "TSM_SERVER_2_''pid'.TMP"
$temp_file3 = "TSM_SERVER_3_''pid'.TMP"
$!
$! Get the list of registered servers
$!------------------------------------
$if f$search(temp_file1) .nes. "" then delete 'temp_file1';*
$tsm/out='temp_file1' show server *
$!
$if f$search(temp_file2) .nes. "" then delete 'temp_file2';*
$search/out='temp_file2' 'temp_file1'/exact "Available"
$!
$!
$if f$search(temp_file3) .nes. "" then delete 'temp_file3';*
$search/out='temp_file3' 'temp_file1'/exact "Unavailable"
$!
$!
$! Look up each server in the list
$!----------------------------------
$my_proc = f$environment("PROCEDURE")
$get_server = f$parse("get_my_server_setup.com;0",my_proc)
$!
$open/read tmp2 'temp_file2'
$server_loop:
$    read/end=server_loop_end tmp2 line_in
$    exit_clue = f$extract(0,6,line_in)
$    if exit_clue .eqs. " TOTAL" then goto server_loop_end
$    line_in = f$edit(line_in,"trim,compress")
$    svr_name = f$element(0," ",line_in)
$!
$!   Get the Server Setup file
$!-----------------------------
$    save_ver = f$verify(0)
$    @'get_server' 'svr_name' TRUE
$    if save_ver then set verify
$!
$    goto server_loop
$server_loop_end:
$close tmp2
$!
$!
$all_exit:
$if f$trnlnm("TMP2","LNM$PROCESS_TABLE",,"SUPERVISOR") -
	.nes. "" then close tmp2
$!if f$search(temp_file1) .nes. "" then delete 'temp_file1';*
$!if f$search(temp_file2) .nes. "" then delete 'temp_file2';*
$!if f$search(temp_file3) .nes. "" then delete 'temp_file3';*
$exit
