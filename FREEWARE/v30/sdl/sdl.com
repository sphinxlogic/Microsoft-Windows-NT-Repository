$!
$! This command file redefines the verb SDL to access the new, enhanced
$! SDL that understands conditionals, integer datatype, and other features
$! needed for OpenVMS Alpha development.
$!
$ IF AXP_NATIVE .OR. AXP_CROSS
$ THEN
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
$ ENDIF
$ EXIT
