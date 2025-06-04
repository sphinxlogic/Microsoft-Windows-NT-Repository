$!
$!	PPLRTLBLD.COM	- Build procedure to rebuild the PPLRTL images.
$!
$!	Step 1:		Backup the whole [PPLRTL...] directory tree onto
$!			a writeable scratch storage, preserving the tree.
$!
$!	Step 2:		Edit and modify this PPLRTLBLD.COM procedure to
$!			change the first statement and set the default
$!			directory to where the PPLRTL tree has been copied.
$!
$!	Step 3:		Either execute this PPLRTLBLD procedure interactively
$!			or submit it as a batch job.
$!
$! set default dev:[pplrtl]
$!
$ devdir = f$environment("default")
$ prefix = f$extract(0,f$length(devdir)-1,devdir)
$ suffix = devdir - prefix
$ define/nolog  src$    'prefix'.src'suffix'
$ define/nolog  lis$    'prefix'.lis'suffix'
$ define/nolog  lib$    'prefix'.obj'suffix'
$ define/nolog  exe$    'prefix'.obj'suffix'
$ define/nolog  map$    'prefix'.lis'suffix'
$ define/nolog  obj$    'prefix'.obj'suffix'
$ define/nolog  com$    'prefix'.com'suffix'
$ define/nolog  shrlib$ 'prefix'.shrlib'suffix'
$ define/nolog  tools$	'prefix'.tools'suffix'
$!
$ SET COMMAND tools$:BLISS_AN.CLD
$ DEFINE/nolog BLISS32EN tools$:BLISS32EN
$ bliss :== bliss/a32/notrace/env=nofp/check=(noalign,address_taken)
$!
$ !++
$ !
$ ! PPLRTLBLD.COM - 	build the component PPLRTL.
$ !--
$ ON SEVERE_ERR THEN GOTO FAIL
$ ON ERROR THEN CONTINUE
$ !
$ DELETE OBJ$:*.*;*,LIS$:*.*;*,LIB$:*.*;*
$ !
$ SET ON
$ !
$ ! Get rid of old versions of files just updated.
$ !
$ PURGE SRC$:
$ COPY shrLIB$:RTLPSECT.REQ SRC$:
$ DELETE LIB$:PPLRTL.OLB;*, PPLMSGPTR.OLB;*, PPLMSGTXT.OLB;*, PPLVECTRS.OLB;*
$ !
$ ! MESSAGE modules
$ !
$ MESSAGE/NOSYMBOLS/LIS=LIS$:PPLMSGTXT/OBJ=OBJ$:PPLMSGTXT shrlib$:PPLMSG.MSG
$ MESSAGE/FILE_NAME=PPLMSG/LIS=LIS$:PPLMSGPTR/OBJ=OBJ$:PPLMSGPTR shrlib$:PPLMSG.MSG
$ !
$ ! Create object libraries.  These will be filled in during the ASSEM phase.
$ !
$ LIBRARY/CREATE/OBJECT LIB$:PPLRTL
$ LIBRARY/CREATE/OBJECT LIB$:PPLMSGPTR
$ LIBRARY/CREATE/OBJECT LIB$:PPLMSGTXT
$ LIBRARY/CREATE/OBJECT LIB$:PPLVECTRS
$ !
$ DEFINE	RTLIN		SRC$:
$ DEFINE	RTLML	     	LIB$:
$ DEFINE	RTLSTARLE	shrlib$:STARLET.L32
$ DEFINE	RTLTPAMAC	shrlib$:TPAMAC.L32 
$ DEFINE	RTLLIB		shrlib$:LIB.L32
$ !
$ ! Create BLISS source libraries.  
$ !
$ DELETE LIS$:PPLLIB.L32;*,LIS$:PPLLIB.LIS;*
$ define/nolog sys$library shrlib$
$ copy shrlib$:pplmsg.r32 obj$:
$ BLISS/LIBRARY=LIB$:/LIST=LIS$:/SOURCE=REQUIRE SRC$:PPLLIB
$ BLISS/LIBRARY=LIB$:/LIST=LIS$:/SOURCE=REQUIRE SHRLIB$:RTLBUILTINS
$ !
$ ! Delete the files we are about to create ( from previous builds ); cannot
$ ! use wildcard delete because library listings and object libraries were
$ ! just created.  So just PURGE.
$ !
$ PURGE LIB$:
$ PURGE OBJ$:
$ !
$ ! Compile all modules and insert object modules in libraries.
$ !
$ deassign sys$library 
$ MACRO/TIE/ENABLE=SUPPRESSION/DISABLE=(GLOBAL,TRACEBACK)-
		/LIS=LIS$:PPLBITS-
		/OBJ=OBJ$:PPLBITS-
		SYS$LIBRARY:ARCH_DEFS.MAR+-
		src$:PPLBITS.MAR
$ MACRO/TIE/ENABLE=SUPPRESSION/DISABLE=(GLOBAL,TRACEBACK)-
		/LIS=LIS$:PPLVMPAGE-
		/OBJ=OBJ$:PPLVMPAGE-
		SYS$LIBRARY:ARCH_DEFS.MAR+-
		src$:PPLVMPAGE.MAR
$!$ macro/alpha/machine-
$!		/LIS=LIS$:PPLJMPCODE-
$!		/OBJ=OBJ$:PPLJMPCODE-
$!		SYS$LIBRARY:ARCH_DEFS.MAR+SYS$LIBRARY:STARLET.INCLUDE+SYS$LIBRARY:LIB.INCLUDE+-
$!		src$:PPLJMPCODE.M64
$ copy shrlib$:ppljmpcode.obj obj$:
$ copy shrlib$:ppljmpcode.lis lis$:
$ define/nolog sys$library shrlib$
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLALLOC-
		/OBJ=OBJ$:PPLALLOC-
		src$:PPLALLOC.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLCHOOSE-
		/OBJ=OBJ$:PPLCHOOSE-
		src$:PPLCHOOSE.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLCREBARR-
		/OBJ=OBJ$:PPLCREBARR-
		src$:PPLCREBARR.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLCREPROC-
		/OBJ=OBJ$:PPLCREPROC-
		src$:PPLCREPROC.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLCRESEM-
		/OBJ=OBJ$:PPLCRESEM-
		src$:PPLCRESEM.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLCRESPINLOCK-
		/OBJ=OBJ$:PPLCRESPINLOCK-
		src$:PPLCRESPINLOCK.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLDELETE-
		/OBJ=OBJ$:PPLDELETE-
		src$:PPLDELETE.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLEVENT-
		/OBJ=OBJ$:PPLEVENT-
		src$:PPLEVENT.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLFINDID-
		/OBJ=OBJ$:PPLFINDID-
		src$:PPLFINDID.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLINTERF-
		/OBJ=OBJ$:PPLINTERF-
		src$:PPLINTERF.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLNAMES-
		/OBJ=OBJ$:PPLNAMES-
		src$:PPLNAMES.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLRELSPINLOCK-
		/OBJ=OBJ$:PPLRELSPINLOCK-
		src$:PPLRELSPINLOCK.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLRETSEMVAL-
		/OBJ=OBJ$:PPLRETSEMVAL-
		src$:PPLRETSEMVAL.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLSECT-
		/OBJ=OBJ$:PPLSECT-
		src$:PPLSECT.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLSEIZESPINLOCK-
		/OBJ=OBJ$:PPLSEIZESPINLOCK-
		src$:PPLSEIZESPINLOCK.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLSEM-
		/OBJ=OBJ$:PPLSEM-
		src$:PPLSEM.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLTELL-
		/OBJ=OBJ$:PPLTELL-
		src$:PPLTELL.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLTOP-
		/OBJ=OBJ$:PPLTOP-
		src$:PPLTOP.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLUNIQUE-
		/OBJ=OBJ$:PPLUNIQUE-
		src$:PPLUNIQUE.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLVM-
		/OBJ=OBJ$:PPLVM-
		src$:PPLVM.B32
$ BLISS/TIE/NOTRACE/OPTIMIZE=(LEVEL:3)-
		/LIS=LIS$:PPLWORKQ-
		/OBJ=OBJ$:PPLWORKQ-
		src$:PPLWORKQ.B32
$ deassign sys$library 
$ !
$ ! Purge again.  This ensures minimal disk usage.
$ !
$ PURGE LIS$:
$ PURGE OBJ$:
$ !
$OBJLIB:
$ !+
$ ! OBJLIB phase
$ !-
$ !
$ ! Create facility object libraries from object modules built during
$ ! the previous (ASSEM) phase.
$ !
$ LIBRARY/OBJECT LIB$:PPLMSGTXT.OLB	OBJ$:PPLMSGTXT.OBJ;
$ DELETE				OBJ$:PPLMSGTXT.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:PPLMSGPTR.OLB	OBJ$:PPLMSGPTR.OBJ;
$ DELETE				OBJ$:PPLMSGPTR.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:PPLRTL.OLB	OBJ$:*.OBJ;
$ DELETE				OBJ$:*.OBJ;
$ !
$ !
$ ! Extract message pointer object files from messages object library and
$ ! insert them into facility object library.
$ !
$ LIBRARY/EXTRACT=*/OUT=OBJ$:PPLMSGPTR.OBJ LIB$:PPLMSGPTR
$ LIBRARY LIB$:PPLRTL OBJ$:PPLMSGPTR.OBJ
$ DELETE OBJ$:PPLMSGPTR.OBJ;*
$ !
$ ! Extract individual vector object files from vector object library, to be
$ ! used in next phase to link the shareable image(s).
$ !
$ !-
$LNKLIB:
$ !+
$ !LNKLIB phase
$ !
$ LIBRARY/ALPHA/EXTRACT=(AMAC$*)/OUTPUT=LIB$:TMP SYS$LIBRARY:STARLET
$ LIBRARY/ALPHA_OUT/CREATE LIB$:AMAC$EMULATION_ROUTINES LIB$:TMP
$ DELETE LIB$:TMP.OBJ;*
$ !
$ ! Delete the files we are about to create ( from previous builds ).
$ !
$ DELETE EXE$:PPLRTL.EXE;*,MAP$:PPLRTL.MAP;*
$ !
$ ! Link the Run-Time Library Shareable Image PPLRTL
$ !
$ LINK/NOSYSLIB/NOTRACEBACK/SHARE=EXE$:PPLRTL/MAP=MAP$:PPLRTL/FULL-
    /NONATIVE_ONLY/CROSS COM$:PPLRTLLNK/OPTIONS
$ !
$LINK:
$ !+
$ ! LINK phase
$ !-
$ ! Delete the files we are about to create from previous builds
$ !
$ DELETE EXE$:PPLMSG.EXE;*
$ !
$ ! Link the Run-Time Library Message Image PPLMSG
$ !
$ LINK/NOTRACEBACK/NOMAP/SHARE=EXE$:PPLMSG -
   LIB$:PPLMSGTXT/INCLUDE=PPL$MSGDEF
$ !
$ GOTO EXIT
$ !
$ ! Failure exit
$ !
$FAIL:
$ STATUS = $STATUS
$ !
$ ! Common exit
$ !
$EXIT:
$ !
$ ! Exit, returning status to caller.
$ !
