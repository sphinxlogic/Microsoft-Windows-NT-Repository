$!
$! Build procedure for KPRINT utility
$!
$! To build KPRINT utility, you need VAX C (VAX) or DEC C (Alpha) compiler.
$!
$! Written for OpenVMS Freeware CD-ROM V2.0 by r.miyabi, 4-Nov-1995
$!
$ say := write sys$output
$ CPU = f$edit(f$getsyi("arch_name"), "UPCASE,TRIM")
$ say "Building KPRINT for ", CPU, " architecture...."
$ cur_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE","SYNTAX_ONLY")
$ cur_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY","SYNTAX_ONLY")
$ define/process src$ 'cur_dev''cur_dir'
$ define/process jsy$devil   src$
$ define/process jsy$lib     sys$library
$!
$ if CPU .eqs. "VAX"
$ then !VAX
$   define/job kpri$opt       src$:kpri.opt
$ else !Alpha
$   define/job kpri$opt       src$:kpri_alpha.opt
$ endif
$!
$ if CPU .eqs. "VAX"
$ then !VAX
$     say "Now compiling...."   
$     CC /obj/list/machine SRC$:DEVCOMMON.C
$     CC /obj/list/machine SRC$:DEVTEXT.C
$     CC /obj/list/machine SRC$:DEVSIXEL.C
$     if f$search("DEVILIB.OLB") .eqs. "" then LIBRARY/create DEVILIB.OLB
$     LIBRARY DEVILIB.OLB DEVCOMMON.OBJ, DEVTEXT.OBJ, DEVSIXEL.OBJ
$     CC /OPTIMIZE/obj/list SRC$:KPRINT.C
$     CC /OPTIMIZE/obj/list SRC$:KPRIRMS.C
$     CC /OPTIMIZE/obj/list SRC$:KPRIUTIL.C
$     CC /OPTIMIZE/obj/list SRC$:KPRIPARSE.C
$     message/obj/list src$:kprimsg.msg
$     CC /OPTIMIZE/obj/list SRC$:FEXCONV.C
$     CC /OPTIMIZE/obj/list SRC$:PRECONV.C
$     if f$search("KPRILIB.OLB") .eqs. "" then LIBRARY/create KPRILIB.OLB
$     LIBRARY KPRILIB.OLB KPRINT.OBJ, KPRIRMS.OBJ, KPRIUTIL.OBJ, -
      KPRIPARSE.OBJ, KPRIMSG.OBJ, FEXCONV.OBJ, PRECONV.OBJ
$     say "Now linking...."   
$     LINK /map=kpri /full /cross /notrace /nouserlibrary /exec=KPRI_VAX -
      kprilib/inc=kpri$kprint/lib, devilib/lib, kpri$opt/option
$ else !Alpha
$     say "Now compiling...."   
$     CC/STANDARD=VAXC/PREFIX=ALL /OPTIMIZE/INCLUDE=(SRC$)/obj/list/machine -
      SRC$:DEVCOMMON.C
$     CC/STANDARD=VAXC/PREFIX=ALL /OPTIMIZE/INCLUDE=(SRC$)/obj/list/machine -
      SRC$:DEVTEXT.C
$     CC/STANDARD=VAXC/PREFIX=ALL /OPTIMIZE/INCLUDE=(SRC$)/obj/list/machine -
      SRC$:DEVSIXEL.C
$     if f$search("DEVILIB.OLB") .eqs. "" then LIBRARY/create DEVILIB.OLB
$     LIBRARY DEVILIB.OLB DEVCOMMON.OBJ, DEVTEXT.OBJ, DEVSIXEL.OBJ
$     CC/STANDARD=VAXC/PREFIX=ALL /NOOPTIMIZE/INCLUDE=(SRC$)/obj/list SRC$:KPRINT.C
$     CC/STANDARD=VAXC/PREFIX=ALL /NOOPTIMIZE/INCLUDE=(SRC$)/obj/list SRC$:KPRIRMS.C
$     CC/STANDARD=VAXC/PREFIX=ALL /NOOPTIMIZE/INCLUDE=(SRC$)/obj/list SRC$:KPRIUTIL.C
$     CC/STANDARD=VAXC/PREFIX=ALL /NOOPTIMIZE/INCLUDE=(SRC$)/obj/list SRC$:KPRIPARSE.C
$     message/obj/list src$:kprimsg.msg
$     CC/STANDARD=VAXC/PREFIX=ALL /NOOPTIMIZE/INCLUDE=(SRC$)/obj/list SRC$:FEXCONV.C
$     CC/STANDARD=VAXC/PREFIX=ALL /NOOPTIMIZE/INCLUDE=(SRC$)/obj/list SRC$:PRECONV.C
$     if f$search("KPRILIB.OLB") .eqs. "" then LIBRARY/create KPRILIB.OLB
$     LIBRARY KPRILIB.OLB KPRINT.OBJ, KPRIRMS.OBJ, KPRIUTIL.OBJ, -
      KPRIPARSE.OBJ, KPRIMSG.OBJ, FEXCONV.OBJ, PRECONV.OBJ
$     say "Now linking...."   
$     LINK/map=kpri /full /cross /notrace /nouserlibrary/exec=KPRI_ALPHA -
      kprilib/inc=kpri$kprint/lib, devilib/lib, kpri$opt/option
$ endif
$ exit
