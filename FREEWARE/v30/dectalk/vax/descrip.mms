VERS       = 053
RTLIN      = SMG$:[SMG.SRC]
RTLML      = SMG$:[SMG.OBJ]
RTLIS      = SMG$:[SMG.LIS]
SMG        = SMG$:[SMG]
KIT        = SMG$:[SMG.KIT]

.ifdef debug

LSTFLG     = /NoList
MAPFLAG    = /NoMap
DBGFLG     = /Debug
TRACEFLG   = /Trace

.else
.ifdef list

LSTFLG     = /List=$(RTLIS)
MAPFLAG    = /Map=$(RTLIS)/Full/Cross
DBGFLG     = /NoDebug
TRACEFLG   = /NoTrace

.else

LSTFLG     = /NoList
MAPFLAG    = /NoMap
DBGFLG     = /NoDebug
TRACEFLG   = /NoTrace

.endif
.endif

CMSFLAGS   = /NoHistory/Output=$(mms$target_name)/NoLog/Gen:$(mms$cms_gen)
MSGFLAGS   = /File_name=shrimgmsg/Object=$(mms$target_name)$(lstflg)
MFLAGS     = /Enable=Suppression/Object=$(mms$target_name)$(lstflg)
BFLAGS     = /Opt:Level:3/Object=$(mms$target_name)$(traceflg)$(lstflg)$(dbgflg)
RFLAGS     = /Output=$(mms$target)
LINKFLAGS  = /Exec=$(mms$target)$(traceflg)$(mapflag)
SDLFLAGS   = /Lang=(BlissF=$(mms$target))$(lstflg)
SDIFLAGS   = /Parse=$(mms$target)$(lstflg)

.default
	!
	! Done building $(mms$target)
	!

.first
	old_dir := 'f$directory()'
	Define RTLIN $(rtlin)
	Define RTLML $(rtlml)
	Define RTLIS $(rtlis)
	Vec := $$(SMG)VectorTst

.last
	Set Default 'old_dir'
	Deassign RTLIN
	Deassign RTLML
	Deassign RTLIS

.suffixes .sdi .sdl .sdl~

.txt~.txt 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.dat~.dat 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.com~.com 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.mar~.mar 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.opt~.opt 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.sdl~.sdl 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.req~.req 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.b32~.b32 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.msg~.msg 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.cld~.cld 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.rno~.rno 
	$(cms) Fetch $(mms$cms_element) $(cmsflags) $(cmscomment)
.msg.sdl
	Message/Sdl=$(mms$target)/NoObject$(lstflg) $(mms$source)
.sdl.r32 
	Sdl $(sdlflags) $(mms$source)
.sdl.sdi
	Sdl $(sdiflags) $(mms$source)
.rno.release_notes
	$(runoff) $(rflags) $(mms$source)

HELP : 
	!
	! Command line:  MMS/CMS <target>
	!
	!  Valid targets are:
	!	LIB - build SMGLIB.L32 and SMGTPALIB.L32
	!	TRM - build TERMTABLE.EXE and SMGBLDTRM.EXE
	!	SHR - build SMGSHR.EXE
	!	DTK - build DTKSHR.EXE
	!	ALL - build all of the above
	!	KIT - build SMG$ kit in SYS$SCREEN:
	!
	!  All files are built in $(RTLIN) and $(RTLML)
	!

ALL : LIB, SHR, TRM, DTK

LIB : $(RTLML)SMGLIB.L32, $(RTLML)SMGTPALIB.L32

TRM : $(RTLML)TERMTABLE.EXE

DTK : $(RTLML)DTKSHR.EXE

SHR : $(RTLML)SMGSHR.EXE

KIT : $(RTLML)SMGSHR.EXE, 	$(RTLML)TERMTABLE.EXE, 	-
      $(RTLML)SMGBLDTRM.EXE,	$(RTLML)SMGTERMS.TXT,	-
      $(KIT)SMG$(VERS).RELEASE_NOTES, $(KIT)KITINSTAL.COM, 	-
      $(KIT)BLDSMGKIT.COM, 	$(KIT)BLDSMGKIT.DAT
	Set Default $(KIT)
	Copy/NoLog $(RTLML)smgshr.exe,$(RTLML)smgbldtrm.exe 	$(KIT)
	Copy/NoLog $(RTLML)termtable.exe,$(RTLML)smgterms.txt 	$(KIT)
	Copy/NoLog $(RTLML)smgdef.sdi,$(RTLML)smgtrmptr.sdi 	$(KIT)
	Copy/NoLog $(RTLML)smg$routines.sdi,$(RTLML)smgmsg.sdi	$(KIT)
	@bldsmgkit $(KIT)
	Set Prot:wo:re smg$(VERS).%/log
	Delete/NoLog smgdef.sdi.,smgtrmptr.sdi.,smg$routines.sdi.
	Delete/NoLog smgshr.exe.,termtable.exe.,smgbldtrm.exe.,smgmsg.sdi.
	Delete/NoLog bldsmgkit.com.,bldsmgkit.dat.,kitinstal.com.,smgterms.txt.

$(KIT)SMG$(VERS).RELEASE_NOTES : $(KIT)SMG$(VERS).RNO
	$(runoff) $(rflags) $(mms$source)

!+
! Build the bliss library to compile the SMG$ sources against.
!-

$(RTLML)SMGLIB.L32 : $(RTLIN)SMGLIB.REQ,    $(RTLIN)SMGLNK.REQ, 	-
		     $(RTLIN)SMGTERM.REQ,   $(RTLIN)SMGDATSTR.REQ, 	-
		     $(RTLIN)SMGMACROS.REQ, $(RTLIN)SMGPROLOG.REQ,	-
		     $(RTLML)SMGKTH.R32,    $(RTLML)SMGKQB.R32, 	-
		     $(RTLML)SMGKCB.R32,    $(RTLML)SMGDEF.R32,		-
		     $(RTLML)SMGKDE.R32,    $(RTLML)SMGTRMPTR.R32,	-
		     $(RTLML)SMGDEF.SDI,    $(RTLML)SMGTRMPTR.SDI,	-
		     $(RTLML)SMGMSG.SDI,    $(RTLML)SMG$ROUTINES.SDI

$(RTLML)SMGTRMPTR.R32 : $(RTLIN)SMGTRMPTR.SDL
	Sdl/Vms/Lang=(Bliss=$(mms$target))$(lstflg) $(mms$source)
$(RTLML)SMGDEF.R32 : $(RTLIN)SMGDEF.SDL
	Sdl/Vms/Lang=(Bliss=$(mms$target))$(lstflg) $(mms$source)
$(RTLML)SMGKCB.R32 : $(RTLIN)SMGKCB.SDL
$(RTLML)SMGKDE.R32 : $(RTLIN)SMGKDE.SDL
$(RTLML)SMGKTH.R32 : $(RTLIN)SMGKTH.SDL
$(RTLML)SMGKQB.R32 : $(RTLIN)SMGKQB.SDL

$(RTLIN)SMGMSG.SDL : $(RTLIN)SMGMSG.MSG
	Message/Sdl=$(mms$target)/NoObject$(lstflg) $(mms$source)

!+
! Build the files needed for STARLETSD.
!-

$(RTLML)SMG$ROUTINES.SDI : $(RTLIN)SMG$ROUTINES.SDL
$(RTLML)SMGTRMPTR.SDI	 : $(RTLIN)SMGTRMPTR.SDL
$(RTLML)SMGDEF.SDI 	 : $(RTLIN)SMGDEF.SDL
$(RTLML)SMGMSG.SDI 	 : $(RTLIN)SMGMSG.SDL

!+
! Build the Bliss Library to build the foreign terminal sources against.
!-

$(RTLML)SMGTPALIB.L32 : $(RTLIN)SMGTPALIB.REQ,	$(RTLIN)SMGTPACTL.REQ,	-
			$(RTLIN)SMGTABDEF.REQ,	$(RTLIN)SMGTRMMAC.REQ

!+
! Build the foreign terminal file and its compiler.
!-

$(RTLML)TERMTABLE.EXE : $(RTLML)SMGBLDTRM.EXE, -
			$(RTLML)TERMTABLE.TXT, -
			$(RTLML)SMGTERMS.TXT
	Define termtable $(RTLML)termtable
	Set Default $(RTLML)
	Run $(RTLML)smgbldtrm

$(RTLML)SMGBLDTRM.EXE : $(RTLML)SMGBLDTRM.OBJ, $(RTLML)SMGBOOTAB.OBJ,	-
			$(RTLML)SMGNUMTAB.OBJ, $(RTLML)SMGSTRTAB.OBJ,	-
			$(RTLML)SMGSTATAB.OBJ, $(RTLML)SMGSTR2TAB.OBJ,	-
			$(SMG)TERMDEFS.OBJ
	$(link) $(linkflags) $(mms$source_list)

$(RTLML)SMGBLDTRM.OBJ : $(RTLIN)SMGBLDTRM.B32
$(RTLML)SMGBOOTAB.OBJ : $(RTLIN)SMGBOOTAB.B32
$(RTLML)SMGNUMTAB.OBJ : $(RTLIN)SMGNUMTAB.B32
$(RTLML)SMGSTR2TAB.OBJ : $(RTLIN)SMGSTR2TAB.B32
$(RTLML)SMGSTRTAB.OBJ : $(RTLIN)SMGSTRTAB.B32
$(RTLML)SMGSTATAB.OBJ : $(RTLIN)SMGSTATAB.B32

!+
! Build the DTK$ sharable image - DTKSHR.EXE
!-

$(RTLML)DTKSHR.EXE : $(RTLIN)DTKSHRLNK.OPT, $(RTLIN)DTKPROLOG.REQ, -
		     $(RTLIN)DTKDATSTR.REQ, $(RTLIN)DTKMACROS.REQ, -
		     $(RTLIN)DTKSHRVEC.DAT, -
		     $(RTLML)DTKDEF.R32,    $(RTLML)DTKDEF.SDI,    -
		     $(RTLML)DTK$ROUTINES.SDI, $(RTLML)DTKMSG.SDI, -
		     $(RTLML)DTKRTL.OLB( -
			DTK$UTIL=$(RTLML)DTKUTIL.OBJ, -
			DTK$MSGDEF=$(RTLML)DTKMSG.OBJ, -
			DTK$VECTOR=$(RTLML)DTKVECTOR.OBJ -
			)
	Define lib$ $(RTLML)
	$(link) $(linkflags)/Share/NoSyslib $(RTLIN)dtkshrlnk.opt/opt
	Vec $(RTLML)dtkshr.exe $(RTLIN)dtkshrvec.dat

$(RTLML)DTKDEF.R32    : $(RTLIN)DTKDEF.SDL
	Sdl/Vms/Lang=(Bliss=$(mms$target))$(lstflg) $(mms$source)
$(RTLML)DTKVECTOR.OBJ : $(RTLIN)DTKVECTOR.MAR
$(RTLML)DTKUTIL.OBJ   : $(RTLIN)DTKUTIL.B32
$(RTLML)DTKMSG.OBJ    : $(RTLIN)DTKMSG.MSG
$(RTLIN)DTKMSG.SDL    : $(RTLIN)DTKMSG.MSG
	Message/Sdl=$(mms$target)/NoObject$(lstflg) $(mms$source)

!+
! Build the files needed for STARLETSD.
!-

$(RTLML)DTK$ROUTINES.SDI : $(RTLIN)DTK$ROUTINES.SDL
$(RTLML)DTKDEF.SDI       : $(RTLIN)DTKDEF.SDL
$(RTLML)DTKMSG.SDI	 : $(RTLIN)DTKMSG.SDL

!+
! Build the SMG$ sharable image - SMGSHR.EXE
!-

$(RTLML)SMGSHR.EXE : $(RTLIN)SMGSHRLNK.OPT, $(RTLIN)SMGPROLOG.REQ, -
		     $(RTLIN)SMGSHRVEC.DAT, -
		     $(RTLML)SMGRTL.OLB( -
			SMG$ALLOW_ESCAPE=$(RTLML)SMGALLESC.OBJ, -
			SMG$BUILD_TERM_TABLE=$(RTLML)SMGBLDTRM.OBJ, -
			SMG$BOOLEAN_TABLES=$(RTLML)SMGBOOTAB.OBJ, -
			SMG$$AB_DEFKEY_CLD=$(RTLML)SMGDEFKEY.OBJ, -
			SMG$DISPLAY_CHANGE=$(RTLML)SMGDISCHA.OBJ, -
			SMG$DISPLAY_DHDW=$(RTLML)SMGDISDHW.OBJ, -
			SMG$DISPLAY_DRAW=$(RTLML)SMGDISDRW.OBJ, -
			SMG$DISPLAY_HELP=$(RTLML)SMGDISHLP.OBJ, -
			SMG$DISPLAY_SUBPROCESS=$(RTLML)SMGDISSUB.OBJ, -
			SMG$SELECT=$(RTLML)SMGSELECT.OBJ, -
			SMG$WINDOW=$(RTLML)SMGWINDOW.OBJ, -
			SMG$$DISPLAY_INPUT=$(RTLML)SMGDISINP.OBJ, -
			SMG$DISPLAY_LINKS=$(RTLML)SMGDISLIN.OBJ, -
			SMG$DISPLAY_OUTPUT=$(RTLML)SMGDISOUT.OBJ, -
			SMG$DISPLAY_USER=$(RTLML)SMGDISUSR.OBJ, -
			SMG$INPUT=$(RTLML)SMGINPUT.OBJ, -
			SMG$KEYPAD=$(RTLML)SMGKEYPAD.OBJ, -
			SMG$$KEY_UTIL=$(RTLML)SMGKEYUTI.OBJ, -
			SMG$MAP_TERM_TABLE=$(RTLML)SMGMAPTRM.OBJ, -
			SMG$MIN=$(RTLML)SMGMIN.OBJ, -
			SMG$$MINIMUM_UPDATE=$(RTLML)SMGMINUPD.OBJ, -
			SMG$MISC=$(RTLML)SMGMISC.OBJ, -
			SMG$MSGDEF=$(RTLML)SMGMSG.OBJ, -
			SMG$$NUMBER_PARAMETERS=$(RTLML)SMGNUMPAR.OBJ, -
			SMG$NUMERIC_TABLES=$(RTLML)SMGNUMTAB.OBJ, -
			SMG$$PRVINP=$(RTLML)SMGPRVINP.OBJ, -
			SMG$PUT_VIRTUAL_DISPLAY_ENCODED=$(RTLML)SMGPUTENC.OBJ, -
			SMG$$PUT_TEXT_TO_BUFFER=$(RTLML)SMGPUTTEX.OBJ, -
			SMG$$SCROLL_AREA=$(RTLML)SMGSCROLL.OBJ, -
			SMG$$SIM_TERM=$(RTLML)SMGSIMTRM.OBJ, -
			SMG$STATEMENT_TABLES=$(RTLML)SMGSTATAB.OBJ, -
			SMG$STRING_TABLES=$(RTLML)SMGSTRTAB.OBJ, -
			SMG$STRING2_TABLES=$(RTLML)SMGSTR2TAB.OBJ, -
			SMG$INTERFACE_TERM_TABLE=$(RTLML)SMGUSRTRM.OBJ, -
			SMG$VECTOR=$(RTLML)SMGVECTOR.OBJ -
			)
	Define lib$ $(RTLML)
	$(link) $(linkflags)/Share/Nosyslib $(RTLIN)smgshrlnk.opt/opt
	Vec $(RTLML)smgshr.exe $(RTLIN)smgshrvec.dat

$(RTLML)SMGALLESC.OBJ : $(RTLIN)SMGALLESC.B32
$(RTLML)SMGBLDTRM.OBJ : $(RTLIN)SMGBLDTRM.B32
$(RTLML)SMGBOOTAB.OBJ : $(RTLIN)SMGBOOTAB.B32 
$(RTLML)SMGDEFKEY.OBJ : $(RTLIN)SMGDEFKEY.CLD
$(RTLML)SMGDISCHA.OBJ : $(RTLIN)SMGDISCHA.B32
$(RTLML)SMGDISDHW.OBJ : $(RTLIN)SMGDISDHW.B32
$(RTLML)SMGDISDRW.OBJ : $(RTLIN)SMGDISDRW.B32
$(RTLML)SMGDISHLP.OBJ : $(RTLIN)SMGDISHLP.B32
$(RTLML)SMGDISINP.OBJ : $(RTLIN)SMGDISINP.B32
$(RTLML)SMGDISLIN.OBJ : $(RTLIN)SMGDISLIN.B32
$(RTLML)SMGDISOUT.OBJ : $(RTLIN)SMGDISOUT.B32
$(RTLML)SMGDISSUB.OBJ : $(RTLIN)SMGDISSUB.B32
$(RTLML)SMGDISUSR.OBJ : $(RTLIN)SMGDISUSR.B32
$(RTLML)SMGINPUT.OBJ  : $(RTLIN)SMGINPUT.B32
$(RTLML)SMGKEYPAD.OBJ : $(RTLIN)SMGKEYPAD.B32
$(RTLML)SMGKEYUTI.OBJ : $(RTLIN)SMGKEYUTI.MAR
$(RTLML)SMGMAPTRM.OBJ : $(RTLIN)SMGMAPTRM.B32
$(RTLML)SMGMIN.OBJ    : $(RTLIN)SMGMIN.B32
$(RTLML)SMGMINUPD.OBJ : $(RTLIN)SMGMINUPD.B32
$(RTLML)SMGMISC.OBJ   : $(RTLIN)SMGMISC.B32
$(RTLML)SMGMSG.OBJ    : $(RTLIN)SMGMSG.MSG
$(RTLML)SMGNUMPAR.OBJ : $(RTLIN)SMGNUMPAR.B32
$(RTLML)SMGNUMTAB.OBJ : $(RTLIN)SMGNUMTAB.B32
$(RTLML)SMGPRVINP.OBJ : $(RTLIN)SMGPRVINP.B32
$(RTLML)SMGPUTENC.OBJ : $(RTLIN)SMGPUTENC.B32
$(RTLML)SMGPUTTEX.OBJ : $(RTLIN)SMGPUTTEX.B32
$(RTLML)SMGSCREEN.OBJ : $(RTLIN)SMGSCREEN.B32
$(RTLML)SMGSCRINP.OBJ : $(RTLIN)SMGSCRINP.B32
$(RTLML)SMGSCRMIS.OBJ : $(RTLIN)SMGSCRMIS.B32
$(RTLML)SMGSCROLL.OBJ : $(RTLIN)SMGSCROLL.B32
$(RTLML)SMGSELECT.OBJ : $(RTLIN)SMGSELECT.B32
$(RTLML)SMGSIMTRM.OBJ : $(RTLIN)SMGSIMTRM.B32, -
			$(RTLIN)SMGSCRMAC.REQ, -
			$(RTLIN)SMGSCRTCB.REQ, -
			$(RTLIN)SMGTRMSTR.R32
$(RTLML)SMGSTATAB.OBJ : $(RTLIN)SMGSTATAB.B32
$(RTLML)SMGSTR2TAB.OBJ : $(RTLIN)SMGSTR2TAB.B32
$(RTLML)SMGSTRTAB.OBJ : $(RTLIN)SMGSTRTAB.B32
$(RTLML)SMGUSRTRM.OBJ : $(RTLIN)SMGUSRTRM.B32
$(RTLML)SMGWINDOW.OBJ : $(RTLIN)SMGWINDOW.B32
$(RTLML)SMGVECTOR.OBJ : $(RTLIN)SMGVECTOR.MAR
