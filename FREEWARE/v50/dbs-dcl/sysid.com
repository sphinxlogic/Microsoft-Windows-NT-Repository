$ ! Procedure:	SYSID.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ brief_version = (f$edit(P1,"COLLAPSE,UPCASE") .nes. "")
$ say = "write sys$output"
$ run dbslibrary:get_cluster_membership
$ members = "''f$trnlnm("cluster_membership")'"
$ this_node = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,UPCASE")
$ version = f$edit(f$getsyi("VERSION"), "TRIM")
$ software = f$getsyi("NODE_SWTYPE") - " "
$ cpuid = f$getsyi("SID")
$ cputype = f$getsyi("CPU")
$ hw_name = f$getsyi("HW_NAME")
$ hw_model = f$getsyi("HW_MODEL")
$ hw_type = f$getsyi("NODE_HWTYPE")
$ boot_time = f$getsyi("BOOTTIME")
$ boottime = f$getsyi("BOOTTIME")
$ bootdate = f$cvtime(boottime,"ABSOLUTE","DATE")
$ bootday = f$cvtime(bootdate,,"WEEKDAY")
$ boottime = f$cvtime(boottime,"ABSOLUTE","TIME")
$ page_count = f$getsyi("MEMSIZE")
$ if (vax)
$   then
$   memsize = page_count/2048
$ else
$ page_size = f$getsyi("PAGE_SIZE")
$ memsize = (page_count/1024)*(page_size/1024)
$ endif
$ node_stuff = "Node ''this_node'"
$ vers_stuff = " is running ''software' ''version' on a ''hw_name'"
$ node_space = (78 - f$length(node_stuff + vers_stuff))/2
$ type_stuff = "The hardware type is ''hw_type' and model is ''hw_model'"
$ type_space = (78 - f$length(type_stuff))/2
$ mem_stuff = "The system has !UL pages of physical memory (!UL MB)"
$ mem_stuff = f$fao(mem_stuff, page_count, memsize)
$ mem_space = (78 - f$length(mem_stuff))/2
$ boot_stuff = "and was last booted at ''boottime' on ''bootday'" -
			+ " ''bootdate'"
$ boot_space = (78 - f$length(boot_stuff))/2
$ if (f$extract(0, 1, members) .nes. "%") then -
$   members = "Current cluster membership is ''members'"
$ member_space = (78 - f$length(members))/2
$ if (member_space .lt. 0) then member_space = 0
$ format = "!#* !AS!AS"
$ if (brief_version)
$   then
$   say "''this_node' (''version') ''hw_name'  booted ''boot_time'"
$ else
$ say ""
$ say f$fao(format, node_space, node_stuff, vers_stuff)
$ say ""
$ say f$fao(format, type_space, type_stuff, "")
$ say f$fao(format, mem_space, mem_stuff, "")
$ say f$fao(format, boot_space, boot_stuff, "")
$ say f$fao(format, member_space, members, "")
$ say ""
$ endif !(brief_version)
$ exitt 1.or.f$verify(__vfy_saved)
$ !+==========================================================================
$ !
$ ! Procedure:	SYSID.COM
$ !
$ ! Purpose:	To display some of the f$getsyi() details like cpuid, cputype.
$ !
$ ! Parameters:
$ !	None.
$ !
$ ! History:
$ !		29-Jun-1988, DBS; Version V1-001
$ !	001 -	Original version.
$ !		04-Jul-1989, DBS; Version V1-002
$ !	002 -	Removed the code to handle pre-V5 stuff with the hw_name.
$ !		14-Sep-1989, DBS; Version V1-003
$ !	003 -	Added cluster membership stuff.
$ !		22-Jan-1992, DBS; Version V1-004
$ !	004 -	Added hw_model stuff.
$ !		28-Apr-1998, DBS; Version V1-005
$ !	005 -	Added code to correctly handle memory size stuff on Alphas.
$ !		14-Sep-1999, DBS; Version V1-006
$ !	006 -	Now does a "brief" display if P1 has anything in it.
$ !		19-Nov-2000, DBS; Version V1-007
$ !	007 -	Fix long standing kludge with MB calculation.
$ !-==========================================================================
