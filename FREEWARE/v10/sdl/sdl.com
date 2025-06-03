$!
$! This command file redefines the verb SDL to access the new, enhanced
$! SDL that understands conditionals, integer datatype, and other features
$! needed for ALPHA/VMS development.
$!
$ if AXP_NATIVE
$ THEN
$	set command evms$build_tools:sdl.cld
$	define alpha_sdl evms$build_tools:'p1'alpha_sdl_tv.exe
$	define alpha_sdlada evms$build_tools:'p1'alpha_sdlada_tv.exe
$	define alpha_sdlbasic evms$build_tools:'p1'alpha_sdlbasic_tv.exe
$	define alpha_sdlbas evms$build_tools:'p1'alpha_sdlbasic_tv.exe
$	define alpha_sdlbliss evms$build_tools:'p1'alpha_sdlbliss_tv.exe
$	define alpha_sdlbliss64 evms$build_tools:'p1'alpha_sdlbliss64_tv.exe
$	define alpha_sdlblissf evms$build_tools:'p1'alpha_sdlblissf_tv.exe
$	define alpha_sdlcc evms$build_tools:'p1'alpha_sdlcc_tv.exe
$	define alpha_sdlc evms$build_tools:'p1'alpha_sdlcc_tv.exe
$	define alpha_sdldtr evms$build_tools:'p1'alpha_sdldtr_tv.exe
$	define alpha_sdlepascal evms$build_tools:'p1'alpha_sdlepascal_tv.exe
$	define alpha_sdlfortran evms$build_tools:'p1'alpha_sdlfortran_tv.exe
$	define alpha_sdlfor evms$build_tools:'p1'alpha_sdlfortran_tv.exe
$	define alpha_sdlfortV3 evms$build_tools:'p1'alpha_sdlfortv3_tv.exe
$	define alpha_sdllisp evms$build_tools:'p1'alpha_sdllisp_tv.exe
$	define alpha_sdlmacro evms$build_tools:'p1'alpha_sdlmacro_tv.exe
$	define alpha_sdlmac evms$build_tools:'p1'alpha_sdlmacro_tv.exe
$	define alpha_sdlnparse evms$build_tools:'p1'alpha_sdlnparse_tv.exe
$	define alpha_sdlpascal evms$build_tools:'p1'alpha_sdlpascal_tv.exe
$	define alpha_sdlpas evms$build_tools:'p1'alpha_sdlpascal_tv.exe
$	define alpha_sdlpli evms$build_tools:'p1'alpha_sdlpli_tv.exe
$	define alpha_sdlsdml evms$build_tools:'p1'alpha_sdlsdml_tv.exe
$	define alpha_sdltpu evms$build_tools:'p1'alpha_sdltpu_tv.exe
$	define alpha_sdluil evms$build_tools:'p1'alpha_sdluil_tv.exe
$!
$! Because the VAX cross-tool ALPHA_SDL*.EXE images link against UVMTHRTL,
$! the VESTed images need to activate UVMTHRTL_TV.  We define that logical
$! here.
$!
$	define uvmthrtl_tv evms$build_tools:uvmthrtl_d56_tv
$ endif
$ if AXP_CROSS
$ then
$	set command evms$build_tools:sdl.cld
$	define alpha_sdl evms$build_tools:'p1'alpha_sdl.exe
$	define alpha_sdlada evms$build_tools:'p1'alpha_sdlada.exe
$	define alpha_sdlbasic evms$build_tools:'p1'alpha_sdlbasic.exe
$	define alpha_sdlbas evms$build_tools:'p1'alpha_sdlbasic.exe
$	define alpha_sdlbliss evms$build_tools:'p1'alpha_sdlbliss.exe
$	define alpha_sdlbliss64 evms$build_tools:'p1'alpha_sdlbliss64.exe
$	define alpha_sdlblissf evms$build_tools:'p1'alpha_sdlblissf.exe
$	define alpha_sdlcc evms$build_tools:'p1'alpha_sdlcc.exe
$	define alpha_sdlc evms$build_tools:'p1'alpha_sdlcc.exe
$	define alpha_sdldtr evms$build_tools:'p1'alpha_sdldtr.exe
$	define alpha_sdlepascal evms$build_tools:'p1'alpha_sdlepascal.exe
$	define alpha_sdlfortran evms$build_tools:'p1'alpha_sdlfortran.exe
$	define alpha_sdlfor evms$build_tools:'p1'alpha_sdlfortran.exe
$	define alpha_sdlfortV3 evms$build_tools:'p1'alpha_sdlfortv3.exe
$	define alpha_sdllisp evms$build_tools:'p1'alpha_sdllisp.exe
$	define alpha_sdlmacro evms$build_tools:'p1'alpha_sdlmacro.exe
$	define alpha_sdlmac evms$build_tools:'p1'alpha_sdlmacro.exe
$	define alpha_sdlnparse evms$build_tools:'p1'alpha_sdlnparse.exe
$	define alpha_sdlpascal evms$build_tools:'p1'alpha_sdlpascal.exe
$	define alpha_sdlpas evms$build_tools:'p1'alpha_sdlpascal.exe
$	define alpha_sdlpli evms$build_tools:'p1'alpha_sdlpli.exe
$	define alpha_sdlsdml evms$build_tools:'p1'alpha_sdlsdml.exe
$	define alpha_sdltpu evms$build_tools:'p1'alpha_sdltpu.exe
$	define alpha_sdluil evms$build_tools:'p1'alpha_sdluil.exe
$ endif
$ exit
