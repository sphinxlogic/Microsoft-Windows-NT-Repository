$! B U I L D . C O M
$!
$! Automated build procedure for XGL. See the Makefile for information on
$! the compiler defines.
$!
$! Modification History
$! 4-Mar-1991	Jeff E. Nelson (jnelson@tle.enet.dec.com)   Author
$! 11-Mar-1991  Jay Leafey (leafey@leafey.enet.dec.com)     revised
$!
$ cc_defines = "READDIR_TYPE_DIRECT,HAS_ASYNC_INPUT,STDLIB,"
$ cc_defines = cc_defines + "FGETPOS,VOID_POINTERS"
$ cc_switches = p1
$ link_switches = p2
$ source_files = "grasp_run,parser,actions,gif,windows,pcdata,utils,libutils,timer,pic,pcx"
$ source_index = 0
$
$compile_loop:
$	source_file = f$element(source_index,",",source_files)
$	if source_file .eqs. ","
$	then
$		goto link_image
$	endif
$	if f$search("''source_file'.obj") .eqs. ""
$	then
$		call compile "''source_file'.c" "''cc_defines'" "''cc_switches'"
$	else
$		if f$cvtime(f$file("''source_file'.obj","rdt"),"comparison") -
		.lts. f$cvtime(f$file("''source_file'.c","rdt"),"comparison")
$		then
$			call compile "''source_file'.c" "''cc_defines'" -
				"''cc_switches'"
$		endif
$	endif
$	source_index = source_index + 1
$	goto compile_loop
$
$link_image:
$ link/exe=xviewgl'link_switches' -
    grasp_run,parser,actions,gif,windows,pcdata,utils,libutils,timer,pic,pcx,sys$input:/opt
sys$share:vaxcrtl/share
sys$share:decw$xlibshr/share
$
$ exit
$
$compile: subroutine
$	define/user	sys sys$library:
$	define/user	x11 decw$include:
$	cc'cc_switches'/define=('p2') 'p1'
$	exit
$ endsubroutine
