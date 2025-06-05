$ vfysav = f$verify(f$trnlnm("SDL_DEBUG"))
$!
$! This command file redefines the verb SDL to access the Alpha SDL
$! tool -- Alpha SDL is a descendant of the VAX SDL tool that was
$! created to support the requirements of OpenVMS Alpha.  Despite the
$! name, Alpha SDL operates on both the OpenVMS Alpha and OpenVMS VAX
$! platforms.
$!
$! This procedure determines which images are needed for the current
$! system architecture (VAX or Alpha).
$!
$ Arch       = f$getsyi("ARCH_NAME")
$ Device     = f$parse(f$environment("PROCEDURE"),,,"DEVICE")
$ Direct     = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY")
$ Direct     = Direct - "][" - "><" - ">" - "<" - "]" - "["
$ DevDir     = Device + "[" + Direct + "]"
$ DevDirArch = Device + "[" + Direct + "." + Arch + "_IMAGES]"
$
$!
$! If the I64 cross-build environment is lurking, alter our use of 
$! the SET COMMAND verb slightly -- the I64 cross-build environment
$! would normally try to activate the I64 version of CDU, and the
$! I64 version of CDU cannot be used to modify the running Alpha
$! command tables.  In other words, the following DEFINE/USER
$! command will fool the cross-build SET COMMAND verb definition
$! into using the local CDU; into allowing the SET COMMAND for SDL.
$!
$ if f$trnlnm("IA64_CDU") .nes. "" then define/user/nolog IA64_CDU sys$system:cdu
$ Set Command  'DevDir'sdl.cld
$
$
$ Define/NoLog alpha_sdl	'DevDirArch'alpha_sdl.exe
$ Define/NoLog alpha_sdlada	'DevDirArch'alpha_sdlada.exe
$ Define/NoLog alpha_sdlbasic	'DevDirArch'alpha_sdlbasic.exe
$ Define/NoLog alpha_sdlbas	'DevDirArch'alpha_sdlbasic.exe
$ Define/NoLog alpha_sdlbliss	'DevDirArch'alpha_sdlbliss.exe
$ Define/NoLog alpha_sdlbliss64	'DevDirArch'alpha_sdlbliss64.exe
$ Define/NoLog alpha_sdlblissf	'DevDirArch'alpha_sdlblissf.exe
$ Define/NoLog alpha_sdlcc	'DevDirArch'alpha_sdlcc.exe
$ Define/NoLog alpha_sdlc	'DevDirArch'alpha_sdlcc.exe
$ Define/NoLog alpha_sdldtr	'DevDirArch'alpha_sdldtr.exe
$ Define/NoLog alpha_sdlepascal	'DevDirArch'alpha_sdlepascal.exe
$ Define/NoLog alpha_sdlfortran	'DevDirArch'alpha_sdlfortran.exe
$ Define/NoLog alpha_sdlfor	'DevDirArch'alpha_sdlfortran.exe
$ Define/NoLog alpha_sdlfortV3	'DevDirArch'alpha_sdlfortv3.exe
$ Define/NoLog alpha_sdllisp	'DevDirArch'alpha_sdllisp.exe
$ Define/NoLog alpha_sdlmacro	'DevDirArch'alpha_sdlmacro.exe
$ Define/NoLog alpha_sdlmac	'DevDirArch'alpha_sdlmacro.exe
$ Define/NoLog alpha_sdlnparse	'DevDirArch'alpha_sdlnparse.exe
$ Define/NoLog alpha_sdlpascal	'DevDirArch'alpha_sdlpascal.exe
$ Define/NoLog alpha_sdlpas	'DevDirArch'alpha_sdlpascal.exe
$ Define/NoLog alpha_sdlpli	'DevDirArch'alpha_sdlpli.exe
$ Define/NoLog alpha_sdlsdml	'DevDirArch'alpha_sdlsdml.exe
$ Define/NoLog alpha_sdltpu	'DevDirArch'alpha_sdltpu.exe
$ Define/NoLog alpha_sdluil	'DevDirArch'alpha_sdluil.exe
$
$ vfysav = f$verify(vfysav)
$ EXIT
