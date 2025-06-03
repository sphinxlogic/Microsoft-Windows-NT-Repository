!********************************************************************************
!
! MMS build procedure for DECW$BANNER DECwindows application.
!
!	Jim Saunders	23 November 1988
!
!
!
!********************************************************************************

.SUFFIXES :

.SUFFIXES : .exe .olb .hlb .obj .l32 .b32 .bli .c .com -
	    .dat .r32 .h .req  .txt .hlp .mms .opt .uid .uil -
	    .b32~ .bli~ .c~ .h~ .com~ .dat~ .r32~ .req~ .txt~ -
	    .hlp~ .mms~ .opt~ .uil~ .H .sdl .r32 .sdl .sdl~


.sdl~.sdl : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.mms~.mms : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.req~.req : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.r32~.r32 : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.com~.com : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.hlp~.hlp : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.b32~.b32 : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.bli~.bli : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.c~.c     : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.h~.h     : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.opt~.opt     : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.uil~.uil     : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
.dat~.dat     : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)

.sdl.r32 :
	sdl/language=bliss $<

.sdl.h :
	sdl/language=CC $<

.req.l32 :
	set proc/name=_$*
        $(bliss)/library=$(mms$target) $(blibflags)$(bliss_quals) $<

.r32.l32 :
	set proc/name=_$*
        $(bliss)/library=$(mms$target) $(blibflags)$(bliss_quals) $<
 
.b32.obj :
	set proc/name=_$*
        $(bliss) $(bflags)$(bliss_quals) $<

.bli.obj :
	set proc/name=_$*
        $(bliss) $(bflags)$(bliss_quals) $<
.c.obj :
	set proc/name=_$*
	$(cc) $(cflags)$(c_quals) $< 

.uil.uid :
	set proc/name=_$*
	uil $<


!********************************************************************************
!
! MMS Target for Banner
!
!********************************************************************************


BANNER$KIT	: - 
		BANNER_IMAGE -
		BANNER020.A
	pur *.*

BANNER_IMAGE	: -
		BNR$CODE -
		$(bnr_source)BANNER.OPT -
		$(bnr_source)BANNERLINK.COM
	set proc/name=bannerlink
	@$(bnr_source)BANNERLINK.COM

BANNER020.A	: -
		$(bnr_source)decw$banner.uid -
		$(bnr_source)decw$banner.hlp -
		$(bnr_source)decw$banner.dat -
		$(bnr_source)kitinstal.com
	set proc/name=_$*
	$purge *.*
	$backup/log *.olb,decw$banner.hlp, banner.opt, -
		decw$banner.uid,decw$banner.dat, -
		decw$bannerglobe.dat,bannerlink.com, kitinstal.com $*.A/sav
	pur *.*

!********************************************************************************
!
! MMS MACRO for BANNER.OLB update
!
!********************************************************************************

BNR$CODE	: -
	$(bnr_source)BANNER.OLB(BANNER=$(bnr_source)BANNER.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERMAIN=$(bnr_source)BANNERMAIN.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERSOUND=$(bnr_source)BANNERSOUND.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERDRAW=$(bnr_source)BANNERDRAW.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERSYSTEM=$(bnr_source)BANNERSYSTEM.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERSUBS=$(bnr_source)BANNERSUBS.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERPROCESS=$(bnr_source)BANNERPROCESS.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERMESSAGE=$(bnr_source)BANNERMESSAGE.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERLINK=$(bnr_source)BANNERLINK.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERMONITOR=$(bnr_source)BANNERMONITOR.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERCPU=$(bnr_source)BANNERCPU.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERTP=$(bnr_source)BANNERTP.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERQP=$(bnr_source)BANNERQP.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERQB=$(bnr_source)BANNERQB.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERCLU=$(bnr_source)BANNERCLU.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERPE=$(bnr_source)BANNERPE.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERLCK=$(bnr_source)BANNERLCK.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERCWPS=$(bnr_source)BANNERCWPS.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERGLOBE=$(bnr_source)BANNERGLOBE.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERCUBE=$(bnr_source)BANNERCUBE.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERHANOI=$(bnr_source)BANNERHANOI.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERCLOCK=$(bnr_source)BANNERCLOCK.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERALARM=$(bnr_source)BANNERALARM.OBJ) -
	$(bnr_source)BANNER.OLB(BANNERREMOTE=$(bnr_source)BANNERREMOTE.OBJ) 
	pur *.*


!********************************************************************************
!
! MMS dependancies for the code modules
!
!********************************************************************************
$(bnr_source)bannerremote.obj : $(bnr_source)BANNER.H
!		$(bnr_source)BANNER_PE.R32 -
!		$(bnr_source)lanudef.R32 -
!		$(bnr_source)pem_def.R32 

$(bnr_source)banner.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannermain.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannersound.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerdraw.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannersystem.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannersubs.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerclock.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)banneralarm.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerglobe.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerhanoi.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannercpu.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannertp.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerclu.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerqp.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerqb.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerlink.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)BANNERMONITOR.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannermessage.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannercube.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerprocess.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerlck.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannercwps.obj : -
		$(bnr_source)BANNER.H 
$(bnr_source)bannerpe.obj : -
		$(bnr_source)BANNER.H 
!		$(bnr_source)BANNER_PE.H -
!		$(bnr_source)lanudef.H -
!		$(bnr_source)pem_def.H 


!********************************************************************************
!
! MMS start and finish options
!
!********************************************************************************
.first

	@ WRITE SYS$OUTPUT "************* Starting Build of DECW$BANNER.EXE **********"
	@ DEFINE VMI$KWD []
	@ CONTINUE

.last	
	@ WRITE SYS$OUTPUT "********** End of banner build **********"
	@ continue
