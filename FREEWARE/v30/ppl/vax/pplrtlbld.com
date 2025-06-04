$ !++
$ !
$ ! PPLRTLBLD.COM - 	build the component PPLRTL.
$ ! 			assumes definitions for:  LIB$, LIS$, SRC$, EXE$, MAP$,
$ !						  OBJ$, and SYSBLDCOM$.
$ !			assumes existence of:	  SYSBLDCOM$:CREATEDIR.COM
$ !						  SYSBLDCOM$:FASM.COM
$ !						  SYSBLDCOM$:VECTORTST.COM
$ !
$ ! 1-001 - adapted from PASRTLBLD.	CMF 17-Nov-1986
$ ! 1-002 - Correct spelling mistakes and module names.  CMF 16-JAN-1987
$ ! 1-003 - Add a GETSHRLIB of RTLPSECT.REQ.  MDL 26-Jan-1987
$ ! 1-004 - Have vectortst run at the end of link phase	CMF 16-FEB-1987
$ ! 1-005 - To use correct message file name.	CMF 16-feb-1987
$ ! 1-006 - To add the message file for the facility to Starlet CMF 16-FEHB-1987
$ ! 1-007 - Move REMSHRLIBs.  MDL 15-Jun-1987
$ ! 1-008 - Put in ppl$routines and ppl$def .sdl files.	DLR 9-OCT-1987
$ ! 1-009 - Move DEFINEs to PPLRTLASM.  Build OLBs in OBJLIB.  MDL 23-Nov-1987
$ ! 1-010 - Remove /INSERT from LIB/OBJ commands in case build restarted
$ !	    Richard Bishop 30-Jun-1990
$ ! 1-011 - Use BASE_LEVEL.OPT.  SBL 24-Aug-1990
$ ! 1-012 - BASE_LEVEL.opt not avail in MAGIC - remove.  PJC  4-mar-1991
$ !
$ !--
$ ON SEVERE_ERR THEN GOTO FAIL
$ ON ERROR THEN CONTINUE
$ !
$ ! branch n p[hase to be run.
$ !
$ STATUS = 1
$ GOTO 'PHASE'
$ !
$ !
$ !
$INIT:
$ !+
$ ! INIT phase
$ !-
$ !
$ ! create directoreis on the target disk.
$ !
$ @SYSBLDCOM$:CREATEDIR
$FETCHSRC 'VMSCMS$GENERATION' PPLRTL PPLRTLASM.DAT 'TARGDEV'['FACNAM'.COM] VMS$:[PPLRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' PPLRTL PPLRTLLNK.OPT 'TARGDEV'['FACNAM'.COM] VMS$:[PPLRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' PPLRTL PPLRTLVEC.DAT 'TARGDEV'['FACNAM'.COM] VMS$:[PPLRTL.CMS]
$ !
$ GOTO EXIT
$ !+
$ ! end of init phase
$ !-
$UPDATE:
$ ! +
$ ! UPDATE phase
$ !-
$ !
$ SET NOON
$ !
$ ! delete the files we are about to create ( from previous builds ).
$ !
$ REMSHRLIB RTLPSECT.REQ
$ DELSHRLIB LIB$:PPLMSG.MAR
$ DELSHRLIB LIB$:PPLMSG.R32
$!**v
$ DELSHRLIB LIB$:PPL$DEF.MAR
$ DELSHRLIB LIB$:PPL$DEF.R32
$!**^
$ DELETE OBJ$:*.*;*,LIS$:*.*;*,LIB$:*.*;*
$ !
$ ! create new updated files not updated in ASSEM phase
$ !
$!**v
$ SRCUPDATE PPL$ROUTINES.SDL
$ SRCUPDATE PPL$DEF.SDL
$!**^
$ SRCUPDATE PPLLIB.REQ
$ SRCUPDATE PPLMSG.MSG
$ !
$ ! create SDL source files from MESSAGE surce files.
$ !
$ MESSAGE/SDL=LIB$:/NOOBJECT/NOLIST SRC$:PPLMSG
$ !
$ ! Run SDL conversions for VMSLIB component to use in SRCLIB ( next ) Phase
$ !
$ SDL/VMS/NOLIST/LANG=(BLISS=LIB$:,MACRO=LIB$:) LIB$:PPLMSG
$!**v
$ SDL/VMS/NOLIST/LANG=(BLISS=LIB$:,MACRO=LIB$:) SRC$:PPL$DEF
$!	!ppl$routines.sdl gets built in bldresult.com (facility=SYSBLD)
$!**^
$ !
$ ! Share require files with other component(s) that use them.
$ !
$ CPYSHRLIB LIB$:PPLMSG.MAR
$ CPYSHRLIB LIB$:PPLMSG.R32
$!**v
$! CPYSHRLIB LIB$:PPLMSG.SDL	!maybe some day an RTL will do this
$ CPYSHRLIB LIB$:PPL$DEF.R32
$ CPYSHRLIB LIB$:PPL$DEF.MAR
$!**^
$ !
$ SET ON
$ !
$ ! Get rid of old versions of files just updated.
$ !
$ PURGE SRC$:
$ GOTO EXIT
$ !+
$ ! End of UPDATE phase
$ !-
$SRCLIB:
$ !+
$ ! SRCLIB phase
$ !-
$ !
$ ! Delete files from previous builds.
$ ! 
$ !
$ ! Delete assembly checkpoint file ( from previous builds ).
$ !
$ DELETE FASMCHECK.CHK;*
$ !
$ ! Get library/require files from other components as needed.
$ !
$ GETSHRLIB RTLPSECT.REQ
$ !
$ ! Move files just obtained into other subdirectories as needed.
$ !
$ RENAME LIB$:RTLPSECT.REQ SRC$:
$ !
$!**
$ DELETE LIB$:PPLRTL.OLB;*, PPLMSGPTR.OLB;*, PPLMSGTXT.OLB;*, PPLVECTRS.OLB;*
$!**^
$ !
$ ! Create macro source libraries.  Insert modules as appropriate.
$ !
$ ! NONE
$ !
$ ! Create object libraries.  These will be filled in during the ASSEM phase.
$ !
$ LIBRARY/CREATE/OBJECT LIB$:PPLRTL
$ LIBRARY/CREATE/OBJECT LIB$:PPLMSGPTR
$ LIBRARY/CREATE/OBJECT LIB$:PPLMSGTXT
$ LIBRARY/CREATE/OBJECT LIB$:PPLVECTRS
$ !
$ GOTO EXIT
$ !+
$ ! End of SRCLIB phase
$ !-
$ASSEM:
$ !+
$ ! ASSEM phase
$ !-
$ !
$ ! assign RTL-specific logical names.
$ !
$ DEFINE	RTLIN		SRC$:
$ DEFINE	RTLML		LIB$:
$ DEFINE	RTLSTARLE	SYS$LIBRARY:STARLET.L32
$ DEFINE	RTLTPAMAC	SYS$LIBRARY:TPAMAC.L32 
$ DEFINE	RTLLIB		SYS$LIBRARY:LIB.L32
$ !
$ ! Create BLISS source libraries.  This can't be done in the SRCLIB phase
$ ! because references are make to STARTLET.L32, which didn't exist until now.
$ !
$ DELETE LIS$:PPLLIB.L32;*,LIS$:PPLLIB.LIS;*
$ BLISS/LIBRARY=LIB$:/LIST=LIS$:/SOURCE=REQUIRE SRC$:PPLLIB
$ !
$ ! Deassign RTL-specific logical names.
$ !
$ DEASSIGN	RTLIN
$ DEASSIGN	RTLML
$ DEASSIGN	RTLSTARLE
$ DEASSIGN	RTLTPAMAC
$ DEASSIGN	RTLLIB
$ !
$ ! Delete the files we are about to create ( from previous builds ); cannot
$ ! use wildcard delete because library listings and object libraries were
$ ! just created.  So just PURGE.
$ !
$ PURGE LIB$:
$ PURGE OBJ$:
$ !
$ ! Invoke system build assembly procedure to perfo9rm assemblies.  This will
$ ! compile all modules as specified in PPLRTLASM.DAT.  Objects will be placed
$ ! in libraries according to PPLRTLASM.DAT specifications as well.
$ !
$ @SYSBLDCOM$:FASM PPLRTL "" Y 'UPDATES N Y
$ !
$ ! Purge again.  This ensures minimal disk usage.
$ !
$ PURGE LIS$:
$ PURGE OBJ$:
$ !
$ GOTO EXIT
$ !+
$ ! End of ASSEM phase
$ !-
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
$ LIBRARY/OBJECT LIB$:PPLVECTRS.OLB	OBJ$:PPLVECTOR.OBJ;
$ DELETE				OBJ$:PPLVECTOR.OBJ;
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
$ LIBRARY/EXTRACT=PPL$VECTOR/OUT=OBJ$:PPLVECTOR.OBJ LIB$:PPLVECTRS
$ !
$ ! Share the object library for this facility with other facilities that may
$ ! need it
$ !
$ CPYRESOBJ LIB$:PPLRTL.OLB
$ !
$ GOTO EXIT
$ !+
$ ! End of OBJLIB phase
$ !-
$LNKLIB:
$ !+
$ !LNKLIB phase
$ !-
$ !
$ ! Delete the files we are about to create ( from previous builds ).
$ !
$ DELETE EXE$:PPLRTL.EXE;*,MAP$:PPLRTL.MAP;*
$ !
$ ! Link the Run-Time Library Shareable Image PPLRTL
$ !
$ LINK/NOSYSLIB/NOTRACEBACK/SHARE=EXE$:PPLRTL/MAP=MAP$:PPLRTL/FULL/CROSS-
  COM$:PPLRTLLNK/OPTIONS
$ !
$ ! Copy it into it's destination on the result disk, and insert it into the
$ ! shareable image library there.
$ !
$ CPYSYSLIB EXE$:PPLRTL.EXE
$ CPYSYSLIB OBJ$:PPLVECTOR.OBJ
$ INSHARLIB EXE$:PPLRTL
$ !
$ GOTO EXIT
$ !+
$ ! End of LNKLIB phase
$ !-
$LINK:
$ !+
$ ! LINK phase
$ !-
$ !
$ ! Delete the files we are about to create from previous builds
$ !
$ DELETE EXE$:PPLMSG.EXE;*
$ !
$ ! Link the Run-Time Library Message Image PPLMSG
$ !
$ LINK/NOTRACEBACK/NOMAP/SHARE=EXE$:PPLMSG LIB$:PPLMSGTXT/INCLUDE=PPL$MSGDEF
$ !
$ ! Copy it into it's destination on the result disk.
$ !
$ CPYSYSMSG EXE$:PPLMSG.EXE
$ !
$ !
$ ! Invoke system build vector test procedure to be sure the vectored entry
$ ! points in the shareable image(s) are correct.
$ !
$ @SYSBLDCOM$:VECTORTST EXE$:PPLRTL
$ !
$ GOTO EXIT
$ !+
$ ! End of LINK phase
$ !-
$ !
$RESULT:
$ !+
$ ! RESULT phase
$ !-
$ !
$ GOTO EXIT
$ !+
$ ! End of RESULT phase
$ !-
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
$ ! Exit, returning ststus to caller.
$ !
$ !++
$ ! End of file PPLRTLBLD.COM
$ !--
