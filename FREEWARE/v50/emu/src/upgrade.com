$!Upgrade.com	Converts Macro 32 VAX files ready for alpha	10/4/95
$	if f$mode() .eqs. "BATCH" THEN set nover
$	upgrade_debug = 0
$	prev_file = ""
$	input_regs = "INPUT=<R2,R3,R4,R5,R6,R7,R8,R9,R10,R11>"
$	preserve_regs = "PRESERVE=<R2,R3,R4,R5,R6,R7,R8,R9,R10,R11>"
$	output_regs = "OUTPUT=<R2,R3,R4,R5,R6,R7,R8,R9,R10,R11>"
$
$	if f$trnlnm("upgrade_debug").nes. "" then upgrade_debug = 1
$	if p1 .eqs. ""
$	then
$	   read sys$command p1 /prompt="Files to convert to AXP format >"/end=fin
$	endif
$	files_to_convert = f$parse(".MAR;", p1)
$search_loop:
$	file_to_convert = f$search (files_to_convert,2)
$	if (file_to_convert .nes. "") .and. (file_to_convert .nes. prev_file)
$	then
$	   write sys$output "Converting ''file_to_convert'..."
$	   call upgrade_file "''file_to_convert'"
$	   prev_file = file_to_convert
$	   goto search_loop
$	endif
$fin:	exit
$upgrade_file:	subroutine
$	on control_y then goto y_abort
$	on error then goto err
$
$	file = f$parse (".MAR;",p1)
$	op_file_type = ".MAR_AXP;"
$	upgraded_file = f$parse (op_file_type,p1)
$	if f$search (file,1) .eqs. ""
$	then
$	   write sys$output f$fao("File !AS not found. Upgrade not started", file)
$	   exit 1
$	endif
$
$!
$!Read pass one to determine all bsbw routine names
$	write sys$output f$fao ("Reading !AS!AS...", -
		f$parse(file,,,"name"), f$parse(file,,,"type"))
$	open/read/share ip_file 'file'
$	bsbw_count = f$int(0)
$	entry_point_count = f$int(0)
$	rcd_minus_1 = ""
$read_loop:
$	read ip_file rcd /end=eof_read_loop
$	upcased_rcd = f$edit(rcd, "upcase,uncomment")
$!Deal with .ENTRY instructions
$	if f$locate (".ENTRY", upcased_rcd) .nes. f$length(upcased_rcd)
$	then	!Found an .entry point
$	   declare_parameter_count = f$int(-1)
$	   parameter_remainder = ""
$	   parameter_string = ""
$	   entry_name = ""
$	   remaining_entry_string = upcased_rcd - ".ENTRY"
$	   remaining_entry_string = f$edit(remaining_entry_string,"compress,trim")
$	   entry_name = f$element(0,",",remaining_entry_string)
$	   entry_name = f$edit(entry_name, "compress,trim")
$	   if entry_name .eqs. "," then entry_name = "??"
$	   entry_point_count  = f$int(entry_point_count+1)
$	   if upgrade_debug then write sys$output f$fao("Found .ENTRY point !AS", entry_name)
$	endif
$!Deal with BSBW instructions
$	if f$locate ("BSBW", upcased_rcd) .nes. f$length(upcased_rcd)
$	then	!Found an BSBW point
$	   bsbw_string_position = f$locate ("BSBW", upcased_rcd)+4
$	   bsbw_string_reminder_length = f$int(f$length(upcased_rcd)-bsbw_string_position)
$	   bsbw_string_reminder = f$extract(bsbw_string_position,bsbw_string_reminder_length, upcased_rcd)
$	   bsbw_string_reminder = f$edit (bsbw_string_reminder, "compress,trim")
$	   bsbw_name = f$element (0,";",bsbw_string_reminder)
$	   if bsbw_name .eqs. ";" then bsbw_name = bsbw_string_reminder
$	   bsbw_name = f$edit (bsbw_name, "compress,trim")
$	   if bsbw_count .lt. 1
$	   then
$	      bsbw_name_1 = bsbw_name
$	      bsbw_count = f$int(bsbw_count+1)
$	      if upgrade_debug then write sys$output f$fao("Found subroutine !AS", bsbw_name)	   
$	   else
$	      temp_bsbw_count = f$int(0)
$bsbw_loop:   temp_bsbw_count = f$int(temp_bsbw_count+1)
$	      if temp_bsbw_count .le. bsbw_count
$	      then
$	         temp_bsbw_name = bsbw_name_'temp_bsbw_count'
$		 if temp_bsbw_name .nes. bsbw_name then goto bsbw_loop
$	      else
$	         bsbw_count = f$int(bsbw_count+1)
$		 bsbw_name_'bsbw_count' = bsbw_name
$		 if upgrade_debug then write sys$output f$fao("Found subroutine !AS", bsbw_name)	   
$	      endif
$	   endif
$	endif
$		
$	rcd_minus_1 = upcased_rcd
$	goto read_loop
$eof_read_loop:
$	close ip_file
$	write sys$output f$fao ("!SL .ENTRY & !SL BSBW instructions found",-
		f$int(entry_point_count), f$int(bsbw_count))
$!
$!Read pass two to rewrite the new file
$	write sys$output f$fao ("Creating !AS!AS...", -
		f$parse(file,,,"name"), op_file_type)
$	open/read/share ip_file 'file'
$	open/write op_file 'upgraded_file'
$	entry_point_count = f$int(0)
$	rcd_minus_1 = ""
$read_loop2:
$	read ip_file rcd /end=eof_read_loop2
$	upcased_rcd = f$edit(rcd, "upcase,uncomment")
$!Deal with .ENTRY instructions
$	if f$locate (".ENTRY", upcased_rcd) .nes. f$length(upcased_rcd)
$	then	!Found an .entry point
$	   declare_parameter_count = f$int(-1)
$	   parameter_remainder = ""
$	   parameter_string = ""
$	   entry_name = ""
$	   remaining_entry_string = upcased_rcd - ".ENTRY"
$	   remaining_entry_string = f$edit(remaining_entry_string,"compress,trim")
$	   entry_name = f$element(0,",",remaining_entry_string)
$	   entry_name = f$edit(entry_name, "compress,trim")
$	   if entry_name .eqs. ","
$	   then
$	      entry_name = "??"
$	      write sys$output f$fao ("%UPGRADE-W-UNKENT, Can not determine .ENTRY name in line :!AS", rcd)
$	   endif
$	   if (f$locate ("DECLARE", rcd_minus_1) .nes. f$length(rcd_minus_1))
$	   then
$	      parameter_remainder = f$element(1,"<",rcd_minus_1)
$	      if parameter_remainder .nes. "<"
$	      then
$		 parameter_string = f$element(0,">",parameter_remainder)
$	   	 declare_parameter_count = f$int(0)
$		 if (parameter_string .nes. ">") .and. (parameter_string .nes. "")
$		 then
$param_loop:	    param = f$element(declare_parameter_count,",",parameter_string)
$		    if param .nes. ","
$		    then
$		       declare_parameter_count = f$int(declare_parameter_count+1)
$		       goto param_loop
$		    endif
$		 endif
$	      endif
$	   endif
$	   entry_point_count  = f$int(entry_point_count+1)
$	   msg = f$fao(";!AS",rcd)
$	   if upgrade_debug then write sys$output msg
$	   write op_file msg
$	   if declare_parameter_count .eq. -1
$	   then
$	      write sys$output f$fao ("%UPGRADE-W-UNKARGS, Unknown argument count for (!AS). Assumed 12", rcd)
$	      msg = f$fao ("!_.CALL_ENTRY!_MAX_ARGS=12, HOME_ARGS=TRUE, !AS, !AS, LABEL=!AS", -
		input_regs, preserve_regs, entry_name)
$	      if upgrade_debug then write sys$output msg
$	      write op_file msg
$	   else
$	      msg = f$fao ("!_.CALL_ENTRY!_MAX_ARGS=!UL, HOME_ARGS=TRUE, !AS, !AS, LABEL=!AS",-
		f$int(declare_parameter_count), input_regs, preserve_regs, entry_name)
$	      if upgrade_debug then write sys$output msg
$	      write op_file msg
$	   endif
$	   goto read_loop2
$	endif
$!Deal with BSBW instructions
$	if f$locate (":", upcased_rcd) .nes. f$length(upcased_rcd)
$	then	!Found a label 
$	   temp_label = f$element(0,":",upcased_rcd)
$	   temp_bsbw_count = f$int(0)
$check_bsbw_loop:
$	   temp_bsbw_count = f$int(temp_bsbw_count+1)
$	   if temp_bsbw_count .le. bsbw_count
$	   then
$	      temp_bsbw_name = bsbw_name_'temp_bsbw_count'
$	      if temp_bsbw_name .nes. temp_label then goto check_bsbw_loop
$	      rcd_label_part = f$element(0,":",rcd) + ":"
$	      rcd_remainder = rcd - rcd_label_part
$	      if upgrade_debug then write sys$output f$fao("!AS", rcd_label_part)
$	      write op_file rcd_label_part
$	      msg = F$FAO("!_.JSB_ENTRY!_!AS, !AS", input_regs, output_regs)
$	      if upgrade_debug then write sys$output msg
$	      write op_file msg
$	      if upgrade_debug then write sys$output f$fao("!AS", rcd_remainder)
$	      write op_file rcd_remainder
$	      goto read_loop2
$	   endif
$	endif
$!Deal with /MACROLIB.MLB/
$	if f$locate ("MACROLIB.MLB", upcased_rcd) .nes. f$length(upcased_rcd)
$	then	!Found a label 
$	   msg = f$fao("!_.library!_/lib$:macrolib.mlb/")
$	   if upgrade_debug then write sys$output msg
$	   write op_file msg
$	   goto read_loop2
$	endif
$
$write_oprcd:
$	write op_file rcd
$	rcd_minus_1 = upcased_rcd
$	goto read_loop2
$eof_read_loop2:
$	close op_file
$	close ip_file
$	if upgrade_debug then write sys$output f$fao ("!SL .ENTRY & !SL BSBW instructions found",-
		f$int(entry_point_count), f$int(bsbw_count))
$	write sys$output f$fao ("Upgraded !AS!AS, created !AS!AS", f$parse(file,,,"name"), -
			f$parse(file,,,"type"), f$parse(file,,,"name"), op_file_type)

$	exit_status = 1
$finish:
$	if f$trnlnm ("ip_file") .nes. "" then close ip_file
$	if f$trnlnm ("op_file") .nes. "" then close op_file
$	
$	exit 'exit_status'
$
$err:
$	exit_status = $status
$	write sys$output "Error. Aborting current procedure..."
$	goto finish
$y_abort:
$	exit_status = 44
$	write sys$output "Control y Aborting current procedure..."
$	goto finish
$
$	return
$	endsubroutine
