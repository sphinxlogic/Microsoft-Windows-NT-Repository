$!
$	arch = f$getsyi ("Arch_name")
$	file= f$search ("[.''arch']SYSPQL.EXE")
$	dbgfile= f$search ("[.''arch']SYSPQL.STB")
$	name= f$parse (file,,,"NAME")
$	sysman:= $sysman
$	add= p1.nes."REMOVE"
$	replace= p1.eqs."REPLACE"
$	v= f$verify()
$!
$	on warning then goto exit
$	if add
$	then	copy 'file' sys$common:[sys$ldr];/prot=w:Re/log
$		if dbgfile.nes."" then -
			copy 'dbgfile' sys$common:[sys$ldr];/prot=w:Re/log
$		set file/nomove sys$loadable_images:syspql.exe;
$		if replace then goto exit
$		sysman sys_loadable add SYSPQL 'name' /log -
			/load_step=sysinit-
			/severity=info/mess="SYSPQL.EXE loaded"
$	else
$		sysman sys_loadable remove SYSPQL 'name' /log
$		delete sys$common:[sys$ldr]'name'.exe;*,.stb;*/log
$	endif
$	v= f$verify (0)
$	@ sys$update:vms$system_images
$exit:	set noon
$	v:= f$verify (v)
