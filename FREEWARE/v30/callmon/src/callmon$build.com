$!  CALLMON - A Call Monitor for OpenVMS Alpha
$!
$!  File:     CALLMON$BUILD.COM
$!  Author:   Thierry Lelegard
$!  Version:  1.0
$!  Date:     24-JUL-1996
$!
$!  Abstract: This command procedure rebuilds CALLMON. If MMS is available,
$!            it is preferable to use the MMS description file CALLMON.MMS.
$!
$!  Without debug support:  $ @CALLMON$BUILD
$!  With debug support:     $ @CALLMON$BUILD DEBUG
$!
$!
$ if p1.eqs."DEBUG"
$ then
$   dbg_cflags    := /debug/nooptimize
$   dbg_m64flags  := /debug/nooptimize
$   dbg_adaflags  := /debug/nooptimize
$   dbg_linkflags := /debug
$ else
$   dbg_cflags    := /nodebug/optimize
$   dbg_m64flags  := /nodebug/nooptimize
$   dbg_adaflags  := /nodebug/optimize
$   dbg_linkflags := /nodebug
$ endif
$!
$ lis_cflags := /list/machine_code
$ lis_m64flags := /list/machine_code
$ lis_adaflags := /list/machine_code
$ lis_linkflags := /map/cross/full
$!
$ cflags := 'dbg_cflags' 'lis_cflags' /prefix=all
$ m64flags := 'dbg_m64flags' 'lis_m64flags'
$ adaflags := 'dbg_adaflags' 'lis_adaflags' /smart
$ linkflags := 'dbg_linkflags' 'lis_linkflags' /sysexe
$ acslinkflags := 'dbg_linkflags' 'lis_linkflags'
$!
$ adalib := [-.adalib]
$!
$!  Build CALLMON message definition files for C and Ada
$!
$ message callmon$messages.msg
$ cc 'cflags' message_definition.c + sys$library:sys$lib_c.tlb/library
$ link 'linkflags' /nodebug message_definition.obj
$ gendef :== $sys$disk:[]message_definition.exe
$ gendef -c callmon$messages.obj callmonmsg
$ gendef -ada callmon$messages.obj callmonmsg
$!
$!  Build CALLMON library
$!
$ library/create/object callmon.olb
$!
$ library/replace callmon.olb callmon$messages.obj
$ macro/alpha 'm64flags' callmon$jacket.m64
$ library/replace callmon.olb callmon$jacket.obj
$ cc 'cflags' callmon$alphacode.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$alphacode.obj
$ cc 'cflags' callmon$branch.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$branch.obj
$ cc 'cflags' callmon$dump.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$dump.obj
$ cc 'cflags' callmon$fixup.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$fixup.obj
$ cc 'cflags' callmon$image.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$image.obj
$ cc 'cflags' callmon$init.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$init.obj
$ cc 'cflags' callmon$intercept.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$intercept.obj
$ cc 'cflags' callmon$memprot.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$memprot.obj
$ cc 'cflags' callmon$reloc.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$reloc.obj
$ cc 'cflags' callmon$routine.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$routine.obj
$ cc 'cflags' callmon$util.c + sys$library:sys$lib_c.tlb/library
$ library/replace callmon.olb callmon$util.obj
$!
$!  Build C example files
$!
$ cc 'cflags' example_getvm.c
$ link 'linkflags' example_getvm.obj, callmon.olb/library
$!
$ cc 'cflags' example_motif.c
$ cc 'cflags' example_motif_vm.c
$ link 'linkflags' example_motif.obj, example_motif_vm.obj, -
    callmon.olb/library, example_motif.opt/options
$!
$ cc 'cflags' example_shr.c
$ cc 'cflags' example_shr_main.c
$ link/nodebug/share example_shr.obj, example_shr.opt/options
$ link 'linkflags' example_shr_main.obj, callmon.olb/library, -
    example_shr_main.opt/options
$!
$!  Build Ada examples
$!
$ if f$search("sys$system:acs.exe").eqs."" then goto no_ada
$!
$ if f$search("''adalib'adalib.alb").eqs."" then -
$   acs create library/nolog 'adalib'
$ acs set library/nolog 'adalib'
$!
$ acs load /wait/nolog callmon_.ada, callmonmsg_.ada, example_collection_.ada, -
    example_collection.ada, example_tasking.ada
$ acs recompile/wait/nolog 'adaflags' example_tasking
$ acs enter foreign/replace callmon.olb/library callmon
$ link :== link/sysexe
$ acs link 'acslinkflags' example_tasking
$ no_ada:
$!
$ exit
