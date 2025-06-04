$ !++
$ ! SMGRTLBLD.COM  -	build the component SMGRTL (SMGSHR.EXE).
$ !			See edit history at the end of this file.
$ !--
$ ON SEVERE_ERROR THEN GOTO FAIL
$ ON ERROR THEN CONTINUE
$ !
$ ! branch to phase to be run.
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
$ ! create directories on the target disk.
$ !
$ @SYSBLDCOM$:CREATEDIR
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL DTKSHRLNK.OPT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL DTKSHRVEC.DAT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL SMGBLDTRM.OPT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL SMGMAPTRM.OPT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL SMGRTLASM.DAT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL SMGSHRLNK.OPT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL SMGSHRVEC.DAT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL SMGTERMS.TXT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$FETCHSRC 'VMSCMS$GENERATION' SMGRTL TERMTABLE.TXT 'TARGDEV'['FACNAM'.COM] VMS$:[SMGRTL.CMS]
$ !
$ GOTO EXIT
$ !+
$ ! end of INIT phase
$ !-
$UPDATE:
$ !+
$ ! UPDATE phase
$ !-
$ !
$ SET NOON
$ !
$ ! delete the files we are about to create (from previous builds).
$ !
$ DELETE OBJ$:*.*;*,LIS$:*.*;*,LIB$:*.*;*
$ !
$ SRCUPDATE SMG$ARCHITECTURE.COM	! Define the architecture symbols.
$ @SRC$:SMG$ARCHITECTURE.COM
$ !
$ DELSHRLIB LIB$:DTKMSG.MAR
$ DELSHRLIB SRC$:DTKMSG.SDL
$ DELSHRLIB LIB$:DTKDEF.MAR
$ DELSHRLIB LIB$:DTKDEF.R32
$ !
$ DELSHRLIB LIB$:SMGMSG.MAR
$ DELSHRLIB SRC$:SMGMSG.SDL
$ DELSHRLIB LIB$:SMGDEF.MAR
$ DELSHRLIB LIB$:SMGDEF.R32
$ DELSHRLIB SRC$:SMGLNK.REQ
$ DELSHRLIB SRC$:SMGTERM.REQ
$ DELSHRLIB SRC$:SMG$ROUTINES.MAR
$ !
$ IF VAX THEN REMSHRLIB STRLNK.REQ
$ IF VAX THEN REMSHRLIB RTLLIB.L32
$ !
$ ! create new updated files not updated in ASSEM phase.
$ !
$ SRCUPDATE DTK$ROUTINES.SDL
$ SRCUPDATE DTKDATSTR.REQ
$ SRCUPDATE DTKDEF.SDL
$ SRCUPDATE DTKMSG.MSG
$ SRCUPDATE DTKMACROS.REQ
$ SRCUPDATE DTKPROLOG.REQ
$ !
$ SRCUPDATE SMG$ARCHITECTURE.REQ
$ SRCUPDATE SMG$ROUTINES.SDL
$ SRCUPDATE SMGMSG.MSG
$ SRCUPDATE SMGDEF.SDL
$ SRCUPDATE SMGKCB.SDL
$ SRCUPDATE SMGKDE.SDL
$ SRCUPDATE SMGKQB.SDL
$ SRCUPDATE SMGKTH.SDL
$ SRCUPDATE SMGTRMPTR.SDL
$ SRCUPDATE SMGDATSTR.REQ
$ SRCUPDATE SMGLIB.REQ
$ SRCUPDATE SMGLNK.REQ
$ SRCUPDATE SMGMACROS.REQ
$ SRCUPDATE SMGPROLOG.REQ
$ SRCUPDATE SMGSCRMAC.REQ
$ SRCUPDATE SMGSCRTCB.REQ
$ SRCUPDATE SMGTABDEF.REQ
$ SRCUPDATE SMGTERM.REQ
$ SRCUPDATE SMGTPALIB.REQ
$ SRCUPDATE SMGTPACTL.REQ
$ SRCUPDATE SMGTRMMAC.REQ
$ SRCUPDATE SMGTRMSTR.R32
$ !
$ ! create SDL source files from MESSAGE source files.
$ !
$ MESSAGE/SDL=SRC$:/NOLIST/NOOBJECT SRC$:DTKMSG
$ MESSAGE/SDL=SRC$:/NOLIST/NOOBJECT SRC$:SMGMSG
$ !
$ ! run SDL conversions for VMSLIB component to use in SRCLIB (next) phase.
$ !
$ SDL/VMS/NOLIST/LANG=(BLISS=LIB$:,MACRO=LIB$:)	SRC$:DTKDEF
$ SDL/VMS/NOLIST/LANG=(MACRO=LIB$:)		SRC$:DTKMSG
$ SDL/VMS/NOLIST/LANG=(MACRO=LIB$:)		SRC$:SMGMSG
$ SDL/VMS/NOLIST/LANG=(BLISS=LIB$:,MACRO=LIB$:)	SRC$:SMGDEF
$ SDL/VMS/NOLIST/LANG=(BLISS=LIB$:,MACRO=LIB$:)	SRC$:SMGTRMPTR
$ SDL/NOLIST/LANG=(BLISSF=LIB$:)		SRC$:SMGKCB
$ SDL/NOLIST/LANG=(BLISSF=LIB$:)		SRC$:SMGKDE
$ SDL/NOLIST/LANG=(BLISSF=LIB$:)		SRC$:SMGKQB
$ SDL/NOLIST/LANG=(BLISSF=LIB$:)		SRC$:SMGKTH
$ SDL/NOLIST/LANG=(MACRO=LIB$:)                 SRC$:SMG$ROUTINES
$ !                                            
$ ! share message source files w/other component(s) that use them.
$ !
$ CPYSHRLIB SRC$:DTKMSG.SDL		! share file with VMSLIB component
$ CPYSHRLIB LIB$:DTKMSG.MAR		! share file with VMSLIB component
$ CPYSHRLIB LIB$:SMGMSG.MAR		! share file with VMSLIB component
$ CPYSHRLIB SRC$:SMGMSG.SDL		! share file with VMSLIB component
$ !
$ ! share require files w/other component(s) that use them.
$ !
$ CPYSHRLIB LIB$:DTKDEF.MAR		! share file with VMSLIB component
$ CPYSHRLIB LIB$:DTKDEF.R32		! share file with VMSLIB component
$ !
$ CPYSHRLIB LIB$:SMGDEF.MAR		! share file with VMSLIB component
$ CPYSHRLIB LIB$:SMGDEF.R32		! share file with VMSLIB component
$ CPYSHRLIB LIB$:SMGTRMPTR.MAR		! share file with VMSLIB component
$ CPYSHRLIB LIB$:SMGTRMPTR.R32		! share file with VMSLIB component
$ CPYSHRLIB SRC$:SMGLNK.REQ		! share file with COBRTL component
$ CPYSHRLIB SRC$:SMGTERM.REQ		! share file with COBRTL component
$ CPYSHRLIB LIB$:SMG$ROUTINES.MAR       ! share file with VMSLIB component
$ SET ON
$ !
$ ! get rid of old versions of files just updated.
$ !
$ PURGE SRC$:
$ GOTO EXIT
$ !+
$ ! end of UPDATE phase
$ !-
$SRCLIB:
$ !+
$ ! SRCLIB phase 
$ !-
$ @SRC$:SMG$ARCHITECTURE.COM
$ !
$ ! delete assembly checkpoint file (from previous builds).
$ !
$ DELETE FASMCHECK.CHK;*
$ !
$ ! delete the files we are about to create (from previous builds).
$ !
$ DELETE LIB$:DTKRTL.OLB;*,DTKMSGPTR.OLB;*,DTKMSGTXT.OLB;*,DTKVECTRS.OLB;*
$ !
$ DELETE LIB$:SMGRTL.MLB;*
$ DELETE LIB$:SMGRTL.OLB;*,SMGMSGPTR.OLB;*,SMGMSGTXT.OLB;*,SMGVECTRS.OLB;*
$ !
$ ! get library/require files from other components as needed.
$ !
$ IF VAX THEN GETSHRLIB STRLNK.REQ
$ !
$ ! move files just obtained into other subdirectories as needed.
$ !
$ IF VAX THEN RENAME LIB$:STRLNK.REQ	SRC$:
$ !
$ ! create macro source libraries.  insert modules as appropriate.
$ !
$ LIBRARY/CREATE/MACRO LIB$:SMGRTL.MLB
$ LIBRARY/MACRO LIB$:SMGRTL LIB$:SMGDEF.MAR
$ !
$ ! create object libraries.  these will be filled in during the ASSEM phase.
$ !
$ LIBRARY/CREATE/OBJECT LIB$:DTKRTL
$ LIBRARY/CREATE/OBJECT LIB$:DTKMSGPTR
$ LIBRARY/CREATE/OBJECT LIB$:DTKMSGTXT
$ LIBRARY/CREATE/OBJECT LIB$:DTKVECTRS
$ !
$ LIBRARY/CREATE/OBJECT LIB$:SMGRTL
$ LIBRARY/CREATE/OBJECT LIB$:SMGMSGPTR
$ LIBRARY/CREATE/OBJECT LIB$:SMGMSGTXT
$ LIBRARY/CREATE/OBJECT LIB$:SMGVECTRS
$ !
$ ! assign RTL-specific logical names.
$ !
$ DEFINE 	RTLIN		SRC$:
$ DEFINE 	RTLML		LIB$:
$ DEFINE 	RTLSTARLE	SYS$LIBRARY:STARLET.L32
$ DEFINE	RTLTPAMAC	SYS$LIBRARY:TPAMAC.L32
$ DEFINE	RTLLIB		SYS$LIBRARY:LIB.L32
$ !
$ ! create the Bliss source library SMGLIB.  For this to work in the system
$ ! build, this is done after all other facilities have completed in the 
$ ! SRCLIB phase (coordinated by running this facility in SRCLIB_1).  This
$ ! has no impact for FACTSTBLDs.
$ !
$ DELSHRLIB LIB$:SMGLIB.L32
$ !
$ DELETE LIS$:SMGLIB.L32;*,LIS$:SMGLIB.LIS;*
$ BLISS/LIBRARY=LIB$:/LIST=LIS$:/SOURCE=REQUIRE SRC$:SMGLIB
$ !
$ ! Create a BLISS source library specific to TERMTABLE support.
$ !
$ DELETE LIS$:SMGTPALIB.L32;*,LIS$:SMGTPALIB.LIS;*
$ BLISS/LIBRARY=LIB$:SMGTPALIB/LIST=LIS$:SMGTPALIB/SOURCE=REQUIRE -
	SRC$:SMGTPALIB
$ !
$ ! share bliss libraries with other facilities
$ !
$ CPYSHRLIB LIB$:SMGLIB.L32		! Share file with COBRTL component
$ !
$ !
$ ! deassign RTL-specific logical names.
$ !
$ DEASSIGN	RTLIN
$ DEASSIGN	RTLML
$ DEASSIGN	RTLSTARLE
$ DEASSIGN	RTLTPAMAC
$ DEASSIGN	RTLLIB
$ !
$ GOTO EXIT
$ !+
$ ! end of SRCLIB phase
$ !-
$ASSEM:
$ !+
$ ! ASSEM phase 
$ !-
$ @SRC$:SMG$ARCHITECTURE.COM
$ !
$ ! create bliss source libraries.  this can't be done in the SRCLIB phase
$ ! because references are made to STARLET.L32, which didn't exist until now.
$ !
$ ! SMGLIB.L32 WAS CREATED IN THE PREVIOUS (SRCLIB) PHASE.  THIS IS A SPECIAL
$ ! CASE SO THAT ALL OF THE VARIOUS RTL FACILITIES CAN RUN CONCURRENTLY IN
$ ! THE ASSEM PHASE.  THIS IS DONE BY A SPECIAL PHASE CALLED SRCLIB_1 WHEN
$ ! RUNNING THE SYSTEM BUILD.  THERE IS NO IMPACT FOR A FACTSTBLD.
$ !
$ ! get bliss source libraries from other facilities as needed.
$ !
$ IF VAX THEN GETSHRLIB RTLLIB.L32
$ !
$ ! delete the files we are about to create (from previous builds); cannot use
$ ! wildcard delete because library listings and object libraries were just 
$ ! created.  So just PURGE.
$ !
$ PURGE LIS$:
$ PURGE OBJ$:
$ !
$ ! invoke system build assembly procedure to perform assemblies.  This will 
$ ! compile all modules as specified in SMGRTLASM.DAT.  Objects will be placed
$ ! in libraries according to SMGRTLASM.DAT specifications as well.
$ !
$ @SYSBLDCOM$:FASM SMGRTL "" Y 'UPDATES N Y
$ !
$ ! purge again.  this ensures minimal disk usage.
$ !
$ PURGE LIS$:
$ PURGE OBJ$:
$ !
$ GOTO EXIT
$ !+
$ ! end of ASSEM phase
$ !-
$OBJLIB:
$ !+
$ ! OBJLIB phase 
$ !-
$ @SRC$:SMG$ARCHITECTURE.COM
$ !
$ ! Create facility object libraries from object modules built during
$ ! the previous (ASSEM) phase.
$ !
$ LIBRARY/OBJECT LIB$:SMGMSGTXT.OLB		OBJ$:SMGMSGTXT.OBJ;
$ DELETE					OBJ$:SMGMSGTXT.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:SMGMSGPTR.OLB		OBJ$:SMGMSGPTR.OBJ;
$ DELETE					OBJ$:SMGMSGPTR.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:SMGVECTRS.OLB		OBJ$:SMGVECTOR.OBJ;
$ DELETE					OBJ$:SMGVECTOR.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:SMGRTL.OLB		OBJ$:SMG*.OBJ;
$ DELETE					OBJ$:SMG*.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:DTKMSGTXT.OLB		OBJ$:DTKMSGTXT.OBJ;
$ DELETE					OBJ$:DTKMSGTXT.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:DTKMSGPTR.OLB		OBJ$:DTKMSGPTR.OBJ;
$ DELETE					OBJ$:DTKMSGPTR.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:DTKVECTRS.OLB		OBJ$:DTKVECTOR.OBJ;
$ DELETE					OBJ$:DTKVECTOR.OBJ;
$ !
$ LIBRARY/OBJECT LIB$:DTKRTL.OLB		OBJ$:DTK*.OBJ;
$ DELETE					OBJ$:DTK*.OBJ;
$ !
$ !
$ ! extract message pointer object files from message object library and insert
$ ! them into facility object library.
$ !
$ LIBRARY/EXTRACT=*/OUT=OBJ$:DTKMSGPTR.OBJ LIB$:DTKMSGPTR
$ LIBRARY LIB$:DTKRTL OBJ$:DTKMSGPTR.OBJ
$ DELETE OBJ$:DTKMSGPTR.OBJ;
$ !
$ LIBRARY/EXTRACT=*/OUT=OBJ$:SMGMSGPTR.OBJ LIB$:SMGMSGPTR
$ LIBRARY LIB$:SMGRTL OBJ$:SMGMSGPTR.OBJ
$ DELETE OBJ$:SMGMSGPTR.OBJ;
$ !
$ ! extract individual vector object files from vector object library, to be
$ ! used in next phase to link the shareable image(s).
$ !
$ LIBRARY/EXTRACT=DTK$VECTOR/OUT=OBJ$:DTKVECTOR.OBJ LIB$:DTKVECTRS
$ LIBRARY/EXTRACT=SMG$VECTOR/OUT=OBJ$:SMGVECTOR.OBJ LIB$:SMGVECTRS
$ !
$ ! share the object library for this facility with other facilities that may
$ ! need it.
$ !
$ CPYRESOBJ LIB$:DTKRTL.OLB
$ CPYRESOBJ LIB$:SMGRTL.OLB
$ !
$ ! cause SMG messages to be built into SHRIMGMSG.EXE
$ ! 
$ LIBRARY/EXTRACT=DTK$MSGDEF/OUT=OBJ$:DTKMSG.OBB LIB$:DTKMSGTXT
$ CPYSYSMSG OBJ$:DTKMSG.OBB
$ DELETE OBJ$:DTKMSG.OBB;*
$ !
$ LIBRARY/EXTRACT=SMG$MSGDEF/OUT=OBJ$:SMGMSG.OBB LIB$:SMGMSGTXT
$ CPYSYSMSG OBJ$:SMGMSG.OBB
$ DELETE OBJ$:SMGMSG.OBB;*
$ !
$ GOTO EXIT
$ !+
$ ! end of OBJLIB phase
$ !-
$LNKLIB:
$ !+
$ ! LNKLIB phase 
$ !-
$ @SRC$:SMG$ARCHITECTURE.COM
$ !
$ ! delete the files we are about to create (from previous builds).
$ !
$ DELETE EXE$:DTKSHR.EXE;*,MAP$:DTKSHR.MAP;*
$ DELETE EXE$:SMGSHR.EXE;*,MAP$:SMGSHR.MAP;*
$ !
$ ! link the Run-Time Library Shareable Image SMGSHR.
$ !
$ IF VAX
$ THEN
$   LINK/NOSYSLIB/NOTRACEBACK/SHARE=EXE$:SMGSHR/MAP=MAP$:SMGSHR/FULL/CROSS -
    COM$:SMGSHRLNK/OPTIONS
$ ENDIF
$ !
$ IF EVAX
$ THEN
$   LINK/NOSYSLIB/NOTRACEBACK/SHARE=EXE$:SMGSHR/MAP=MAP$:SMGSHR/FULL/CROSS -
    COM$:SMGSHRLNK/OPTIONS,RESOBJ$:BASE_LEVEL/OPTIONS
$ ENDIF
$ !
$ IF VAX
$ THEN
$   LINK/NOSYSLIB/NOTRACEBACK/SHARE=EXE$:DTKSHR/MAP=MAP$:DTKSHR/FULL/CROSS -
    COM$:DTKSHRLNK/OPTIONS
$ ENDIF
$ !
$ IF EVAX
$ THEN
$   LINK/NOSYSLIB/NOTRACEBACK/SHARE=EXE$:DTKSHR/MAP=MAP$:DTKSHR/FULL/CROSS -
    COM$:DTKSHRLNK/OPTIONS,RESOBJ$:BASE_LEVEL/OPTIONS
$ ENDIF
$ !
$ ! copy it into it's destination on the result disk, and insert it into the 
$ ! shareable image library there.
$ !
$ CPYSYSLIB EXE$:DTKSHR.EXE
$ CPYSYSLIB OBJ$:DTKVECTOR.OBJ 
$ INSHARLIB EXE$:DTKSHR
$ !
$ CPYSYSLIB EXE$:SMGSHR.EXE
$ CPYSYSLIB OBJ$:SMGVECTOR.OBJ 
$ INSHARLIB EXE$:SMGSHR
$ !
$ GOTO EXIT
$ !+
$ ! end of LNKLIB phase
$ !-
$LINK:
$ !+
$ ! LINK phase 
$ !-
$ @SRC$:SMG$ARCHITECTURE.COM
$ ! 
$ ! Link the termtable compiler.
$ !
$ IF VAX
$ THEN
$   LINK/EXE=EXE$:SMGBLDTRM/MAP=MAP$:SMGBLDTRM/FULL/CROSS/NOTRACE -
    COM$:SMGBLDTRM/OPTIONS
$ ENDIF
$ !
$ IF EVAX
$ THEN
$   LINK/EXE=EXE$:SMGBLDTRM/MAP=MAP$:SMGBLDTRM/FULL/CROSS/NOTRACE -
    COM$:SMGBLDTRM/OPTIONS,RESOBJ$:BASE_LEVEL/OPTIONS
$ ENDIF
$ !
$ ! Link the program that creates the global section
$ !
$ IF VAX
$ THEN
$   LINK/NOTRACE/EXE=EXE$:SMGMAPTRM/MAP=MAP$:SMGMAPTRM/FULL/CROSS -
    COM$:SMGMAPTRM/OPTIONS
$ ENDIF
$ !
$ IF EVAX
$ THEN
$   LINK/NOTRACE/EXE=EXE$:SMGMAPTRM/MAP=MAP$:SMGMAPTRM/FULL/CROSS -
    COM$:SMGMAPTRM/OPTIONS,RESOBJ$:BASE_LEVEL/OPTIONS
$ ENDIF
$ !
$ ! invoke system build vector test procedure to be sure the vectored entry
$ ! points in the shareable image(s) are correct.
$ !
$ IF VAX THEN @SYSBLDCOM$:VECTORTST EXE$:DTKSHR
$ IF VAX THEN @SYSBLDCOM$:VECTORTST EXE$:SMGSHR
$ !
$ GOTO EXIT
$ !+
$ ! end of LINK phase
$ !
$RESULT:
$ !+
$ ! RESULT phase
$ !-
$ @SRC$:SMG$ARCHITECTURE.COM
$ !
$ ! copy SMGBLDTRM.EXE to [SYSEXE]
$ !
$ CPYRESEXE EXE$:SMGBLDTRM.EXE
$ !
$ ! Copy SMGMAPTRM.EXE to [SYSEXE]
$ !
$ CPYRESEXE EXE$:SMGMAPTRM.EXE
$ !
$ ! Copy the SMGTERMS.TXT and TERMTABLE.TXT files into [SYSEXE]
$ ! as well.  These files are copied unmodified to [SYSEXE].
$ !
$ CPYRESEXE COM$:SMGTERMS.TXT
$ CPYRESEXE COM$:TERMTABLE.TXT
$ !
$ ! Now run the termtable compiler which we just created (SMGBLDTRM.EXE)
$ ! which will read in the TERMTABLE.TXT file and which will
$ ! create the TERMTABLE.EXE file which will be mapped as a global section
$ ! by program SMGMAPTRM.EXE which we also just built.
$ ! By defining logical name TERMTABLE to point to RESEXE$, this causes
$ ! the compiler to take input from TERMTABLE.TXT on RESEXE$ and produce
$ ! TERMTABLE.EXE on RESEXE$ which we then move to EXE$.
$ !
$ DEFINE/USER_MODE TERMTABLE RESEXE$:TERMTABLE
$ DEFINE/USER_MODE SMGTERMS  RESEXE$:SMGTERMS.TXT
$ !
$ ! The following two lines cause TERMTABLE.EXE to be generated using the
$ ! compiler just built.  However, when building ALPHA images on VAX this
$ ! will not work.  Use the system default one instead.
$ !
$ IF VAX 
$ THEN 
$   DEFINE/USER_MODE LIBRTL    SYS$LIBRARY:LIBRTL.EXE
$   RUN EXE$:SMGBLDTRM	! Gets inputs and puts outputs from/to RESEXE$:TERMTABLE
$ ENDIF
$ !
$ IF EVAX 
$ THEN 
$   MCR SMGBLDTRM	! Gets inputs and puts outputs from/to RESEXE$:TERMTABLE
$ ENDIF
$ COPY RESEXE$:TERMTABLE.EXE EXE$:
$ !
$ ! Copy the resulting TERMTABLE.EXE to [SYSEXE]
$ !
$ CPYRESEXE EXE$:TERMTABLE.EXE
$ !
$ !
$ GOTO EXIT
$ !+
$ ! end of RESULT phase
$ !-
$ !
$ ! failure exit 
$ !
$FAIL:
$ STATUS = $STATUS
$ !
$ ! common exit
$ !
$EXIT:
$ !
$ ! exit, returning status to caller.
$ !
$ EXIT STATUS
$ !++
$ !
$ ! SMGRTLBLD.COM  -	build the component SMGRTL (SMGSHR.EXE).
$ !			assumes definitions for: LIB$, LIS$, SRC$, EXE$, MAP$,
$ !						 OBJ$ and SYSBLDCOM$.
$ !			assumes existence of:	 SYSBLDCOM$:CREATEDIR.COM
$ !						 SYSBLDCOM$:FASM.COM
$ !						 SYSBLDCOM$:VECTORTST.COM
$ !
$ ! 1-001 - adapted from RTLBLD.  MDL 22-Aug-1983
$ ! 1-002 - delete checkpoint file from old builds.  MDL 24-Aug-1983
$ ! 1-003 - clean up CPY/DELSHRLIBs.  MDL 16-Sep-1983
$ ! 1-004 - build SMGLIB.L32 in special SRCLIB_1 phase, similar to LIBRTL.
$ !	    COBRTL references it.  MDL 27-Feb-1984
$ ! 1-005 - Add in new SDL files, REQ files for integration of SMG output
$ !	    routines.  Also build TERMTABLE related images in this facility.
$ !	    LEB 6-Mar-1984
$ ! 1-006 - Add in SRCUPDATE of SMGDATSTR.REQ.  LEB 7-Mar-84
$ ! 1-007 - Add in REMSHRLIB, GETSHRLIB and RENAME of file STRLNK.REQ.
$ !	    TH/LEB 13-Mar-1984
$ ! 1-008 - Fix LINK and RESULT phases to build TERMTABLE properly and get
$ !		the .TXT files from the master disk. TH
$ ! 1-009 - Use logical name TERMTABLE to get TERMTABLE.EXE built in RESEXE$
$ !	    and then copy it to EXE$. STAN 21-Mar-1984.
$ ! 1-010 - Define logical for where to get SMGTERMS.TXT. STAN 21-Mar-1984.
$ ! 1-011 - Add a LIB/EXTRACT from SMGVECTRS.OLB to get SMGVECTOR.OBJ
$ !	    LEB 26-Mar-1984
$ ! 1-012 - Link termtable /NOTRACE.  TS 8-Mar-1985
$ ! 1-013 - Point LIBRTL at SYS$LIBRARY in RESULT so SMGBLDTRM doesn't get
$ !	    GSMATCH problems when LIBRTL changes.  MDL 26-Mar-1985
$ ! 1-014 - Generate SDL output from SMGMSG.  TS 6-Aug-1985
$ ! 1-015 - Fix bug in 014.
$ ! 1-016 - LINK/NOTRACEBACK.  MDL 21-AUG-1985
$ ! 1-017 - Add building of DTK facility.  TS 6-Sep-1985
$ ! 1-018 - Make up special versions of SDL definition files for inclusion
$ !	    into STARLETSD.  Link SMGMAPTRM /NOTRACE.  MDL & TS  27-Jan-1986
$ ! 1-019 - Change order of files in 018.  TS 1-May-1986
$ ! 1-020 - Move VECTORTST to LINK phase.  TS 6-Aug-1986
$ ! 1-021 - Add SRCUPDATEs of SMG,DTK$ROUTINES.SDL.  MDL 3-Oct-1986
$ ! 1-022 - Break up SMGDEFS into SMGDEF and SMGMSG.  TS 4-Nov-1986
$ ! 1-023 - Move REMSHRLIBs.  MDL 15-Jun-1987
$ ! 1-024 - Add SMGMSG to STARLET.MLB  TS 15-Oct-1987
$ ! 1-025 - Move DEFINEs to SMGRTLASM.  Build OLBs in OBJLIB.  MDL 23-Nov-1987
$ ! 1-026 - Add generation of keyword macros for SMGRTL.  CHS 14-Dec-1988
$ ! 1-027 - Change SRC$:SMG$ROUTINES to LIB$:SMG$ROUTINES. SME 22-Dec-1988
$ ! X-3   - Remove /INSERT qualifier from LIBRARIAN commands.  This changes
$ !         behavior to /REPLACE, making the procedures more restartable.
$ !         Drew Mason 3-Apr-1991
$ ! 1-028 - Merge VAX source into EVAX source. IYS 27-DEC-1991
$ !	    Conditionalize the handling of STRLNK.REQ and RTLLIB.L32 and
$ !	    LINKing SMGSHR,DTKSHR,SMGBLDTRM,SMGMAPTRM.
$ ! 1-029 - Add inclusion of SMG$ARCHITECTURE.COM for each phase to pick up
$ !	    architectural specific definitions.
$ !--
$ !++
$ ! end of file SMGRTLBLD.COM
$ !--
