$!  This procedure defines the logical names used to build SDL.
$!
$	define/nolog sdl$library	lib$
$!
$!  Logical names that the PAT Parser user has to define
$	define/nolog pat_require_file   lib$:sdlpat.req
$	define/nolog pat_langsp_library	lib$:patlangsp.l32
$	define/nolog pat_lrtune_library	lib$:patlrtune.l32
$	define/nolog patblsext_lib	lib$:patblsext.l32
$	define/nolog patdeb_lib         lib$:patdeb.l32
$	define/nolog patdata_lib	lib$:patdata.l32
$	define/nolog paterror_lib	lib$:paterror.l32
$      	define/nolog patparser_lib	lib$:patparser.l32
$	define/nolog pattoken_lib	lib$:pattoken.l32
$	define/nolog sdltokdef_req	lib$:sdltokdef.r32
$	define/nolog deb_lib		lib$:deb.l32
$	define/nolog patblf_ini         src$:patblf.ini
$	define/nolog patprolog_req	src$:patprolog.req
$	define/nolog patreqpro_req	src$:patreqpro.req
$	define/nolog patswitch_req	src$:patswitch.req
$	define/nolog vms_req		src$:vms.req
$	define/nolog xport_lib          lib$:xport.l32
$!
$! Logical (possibly temporary) to modify build for ALPHA toolkit
$       define/nolog alpha_tool         true
