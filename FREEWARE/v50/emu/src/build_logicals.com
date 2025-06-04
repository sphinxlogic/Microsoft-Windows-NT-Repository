$ @EMU_LOGICALS
$	   def/nolog target_dir	emu5_src
$	if f$getsyi("NODE_HWTYPE") .eqs. "ALPH"
$	then
$	   def/nolog object_lib	emu5_lib:emu5_axp.olb
$	   def/nolog exe_dir	emu5_axp:
$	   def/sys/exec emu5_exe emu5_axp:
$	else
$	   def/nolog exe_dir	emu5_vax:
$	   def/nolog object_lib	emu5_lib:emu5_vax.olb
$	   def/sys/exec emu5_exe emu5_vax:
$	endif
$	if f$search("object_lib") .eqs. "" then Library/create/obj/log object_lib
$	def/nolog sub_files	target_dir:subrouts.dat  
$	def/nolog prog_files	target_dir:programs.dat  
