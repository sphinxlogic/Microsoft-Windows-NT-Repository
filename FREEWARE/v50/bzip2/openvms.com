$
$ opt = "|''p1'|"
$
$ ! Set up the defaults
$ !
$ ccopt = "|"
$ linkopt = "|"
$ mainopt = "|M|"
$
$ if f$length(opt) .ne. f$locate("|D|",opt) 
$ then 
$   ! Request debugging
$   !
$   ccopt = "|D|NO|"
$   linkopt = "|D|"
$ endif
$
$ mainccopt = "|M|" + ccopt
$ shrlinkopt = "|S|" + ccopt
$
$ ! Create the object library
$ ! 
$ libr bzip2.olb/object/create
$
$ ! Compile the source files
$ ! 
$ call cc BZIP2           'mainccopt'
$ call cc BZIP2RECOVER    'mainccopt'
$ call cc UNZCRASH        'mainccopt'
$
$ call cc BLOCKSORT       'ccopt'
$ call cc BZLIB           'ccopt'
$ call cc COMPRESS        'ccopt'
$ call cc CRCTABLE        'ccopt'
$ call cc DECOMPRESS      'ccopt'
$ call cc HUFFMAN         'ccopt'
$ call cc RANDTABLE       'ccopt'
$ call cc SPEWG           'ccopt'
$
$ ! Create the core executables
$ ! 
$ call link BZIP2         'linkopt'
$ call link BZIP2RECOVER  'linkopt'
$ call link UNZCRASH      'linkopt'
$
$ ! Create the shareable image
$ ! 
$ call link_shr           'linkopt'
$
$ ! Clean up the remaining mess
$ ! 
$ delete BZIP2.OBJ;*
$ delete BZIP2RECOVER.OBJ;*
$ delete UNZCRASH.OBJ;*
$ delete bzip2.olb;*
$
$ exit
$
$
$cc: subroutine
$
$ ! Compile a source module
$ ! 
$ file = f$parse(p1,,,"NAME")
$ opt = "|''p2'|"
$ ccopt = " "
$ if f$length(opt) .ne. f$locate("|D|",opt) 
$ then 
$   ccopt = ccopt + "/DEBUG"
$ endif
$ if f$length(opt) .ne. f$locate("|NO|",opt) 
$ then 
$   ccopt = ccopt + "/NOOPTIMIZE"
$ endif
$ cc 'ccopt' 'file' 
$ if f$length(opt) .eq. f$locate("|M|",opt) 
$ then 
$   libr bzip2.olb 'file'
$   objfile = f$parse(".OBJ;*",p1)
$   delete 'objfile'
$ endif
$ endsubroutine
$
$
$link: subroutine
$
$ ! Link an executable
$ ! 
$ file = f$parse(p1,,,"NAME")
$ opt = "|''p2'|"
$ linkopt = " "
$ optfile = " "
$ exefile = "/EXECUT=''file'." + f$getsyi("ARCH_NAME") + "_EXE"
$ if f$length(opt) .ne. f$locate("|D|",opt) 
$ then 
$   linkopt = linkopt + "/DEBUG"
$ endif
$ optname = "LIBBZIP2_" + f$getsyi("ARCH_NAME") + ".OPT"
$ link 'exefile' 'linkopt' 'file' ,bzip2.olb/lib 'optfile'
$ endsubroutine
$
$
$link_shr: subroutine
$
$ ! Create the shareable image
$ ! 
$ opt = "|''p1'|"
$ linkopt = " "
$ optname = "??"
$ shrfile = "??"
$ if f$length(opt) .ne. f$locate("|D|",opt) 
$ then 
$   linkopt = linkopt + "/DEBUG"
$ endif
$ if f$getsyi("ARCH_NAME") .eqs. "VAX"
$ then
$   macro LIBBZIP2_XFR
$   file = "LIBBZIP2_XFR"
$   optname = ",LIBBZIP2_VAX.OPT/OPTION"
$   shrfile = "/SHARE=LIBBZIP2_SHR.VAX_EXE"
$ else
$   file = "bzip2.olb/include=BZLIB"
$   optname = ",LIBBZIP2_ALPHA.OPT/OPTION"
$   shrfile = "/SHARE=LIBBZIP2_SHR.ALPHA_EXE"
$ endif
$ link 'shrfile' 'linkopt' 'file' ,bzip2.olb/lib 'optfile'
$ endsubroutine
$
