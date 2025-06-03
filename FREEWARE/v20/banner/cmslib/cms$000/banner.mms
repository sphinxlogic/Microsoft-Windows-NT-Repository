*8D
 !********************************************************************************
*8E
*8I
*10D
 4!********************************************************************************
*10E
*10I
 !********************************************************************************
*10E
*8E
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
*28D
 	    .hlp~ .mms~ .opt~ .uil~ 
*28E
*28I
 	    .hlp~ .mms~ .opt~ .uil~ .H .sdl .r32 .sdl .sdl~
*28E
 
 
*28I
 .sdl~.sdl : ; $(CMS) FETCH $(GENMACRO) /nohist/out=$* $< $(CMSCOMMENT)
*28E
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
 
*28I
 .sdl.r32 :
 	sdl/language=bliss $<
 
 .sdl.h :
 	sdl/language=CC $<
 
*28E
 .req.l32 :
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
         $(bliss)/library=$(mms$target) $(blibflags)$(bliss_quals) $<
 
 .r32.l32 :
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
         $(bliss)/library=$(mms$target) $(blibflags)$(bliss_quals) $<
  
 .b32.obj :
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
         $(bliss) $(bflags)$(bliss_quals) $<
 
 .bli.obj :
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
         $(bliss) $(bflags)$(bliss_quals) $<
 .c.obj :
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
*20D
 	$(cc) $(cflags)$(c_quals) $< 
*20E
*20I
*21D
 	$(cc) $(cflags)$(c_quals)/DEFINE=(VMS) $< 
*21E
*21I
 	$(cc) $(cflags)$(c_quals) $< 
*21E
*20E
 
 .uil.uid :
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
 	uil $<
 
 
 !********************************************************************************
 !
 ! MMS Target for Banner
 !
 !********************************************************************************
 
 
 BANNER$KIT	: - 
 		BANNER_IMAGE -
*6D
 		BANNER000.A
*6E
*6I
*11D
 		BANNER001.A
*11E
*11I
*12D
 		BANNER002.A
*12E
*12I
*13D
 		BANNER003.A
*13E
*13I
*16D
 		BANNERXM004.A
*16E
*16I
*17D
 		BANNERXM005.A
*17E
*17I
*18D
 		BANNER005.A
*18E
*18I
*19D
 		BANNER006.A
*19E
*19I
*21D
 		BANNER007.A
*21E
*21I
*23D
 		BANNER008.A
*23E
*23I
*25D
 		BANNER009.A
*25E
*25I
*28D
 		BANNER010.A
*28E
*28I
*29D
 		BANNER100.A
*29E
*29I
 		BANNER020.A
*29E
*28E
*25E
*23E
*21E
*19E
*18E
*17E
*16E
*13E
*12E
*11E
*6E
 	pur *.*
 
 BANNER_IMAGE	: -
 		BNR$CODE -
 		$(bnr_source)BANNER.OPT -
 		$(bnr_source)BANNERLINK.COM
*2I
 	set proc/name=bannerlink
*2E
 	@$(bnr_source)BANNERLINK.COM
 
*7D
 BANNER000.A	: -
*7E
*7I
*11D
 BANNER001.A	: -
*11E
*11I
*12D
 BANNER002.A	: -
*12E
*12I
*13D
 BANNER003.A	: -
*13E
*13I
*16D
 BANNERXM004.A	: -
*16E
*16I
*17D
 BANNERXM005.A	: -
*17E
*17I
*18D
 BANNER005.A	: -
*18E
*18I
*19D
 BANNER006.A	: -
*19E
*19I
*21D
 BANNER007.A	: -
*21E
*21I
*24D
 BANNER008.A	: -
*24E
*24I
*25D
 BANNER009.A	: -
*25E
*25I
*28D
 BANNER010.A	: -
*28E
*28I
*29D
 BANNER100.A	: -
*29E
*29I
 BANNER020.A	: -
*29E
*28E
*25E
*24E
*21E
*19E
*18E
*17E
*16E
*13E
*12E
*11E
*7E
 		$(bnr_source)decw$banner.uid -
 		$(bnr_source)decw$banner.hlp -
 		$(bnr_source)decw$banner.dat -
 		$(bnr_source)kitinstal.com
*2I
*3D
 	set proc/name=$<
*3E
*3I
*4D
 	set proc/name=$*
*4E
*4I
 	set proc/name=_$*
*4E
*3E
*2E
*8I
 	$purge *.*
*8E
*13D
 	$backup/log Banner*.c,*.h,Banner.olb,decw$banner.hlp, banner.opt, -
*13E
*13I
*28D
 	$backup/log Banner*.c,*.h,*.olb,decw$banner.hlp, banner.opt, -
*28E
*13E
*14I
*28D
 		decw$starti18n.com,llvinstal.com,llvinstal.dat, -
*28E
*14E
*28D
 		decw$banner.uil,decw$banner.uid,decw$banner.dat, -
*28E
*28I
 	$backup/log *.olb,decw$banner.hlp, banner.opt, -
 		decw$banner.uid,decw$banner.dat, -
*28E
*5D
 		decw$bannerglobe.dat,bannerlink.com, kitinstal.com $</sav
*5E
*5I
 		decw$bannerglobe.dat,bannerlink.com, kitinstal.com $*.A/sav
*5E
 	pur *.*
 
 !********************************************************************************
 !
 ! MMS MACRO for BANNER.OLB update
 !
 !********************************************************************************
 
 BNR$CODE	: -
 	$(bnr_source)BANNER.OLB(BANNER=$(bnr_source)BANNER.OBJ) -
*9I
 	$(bnr_source)BANNER.OLB(BANNERMAIN=$(bnr_source)BANNERMAIN.OBJ) -
*22I
 	$(bnr_source)BANNER.OLB(BANNERSOUND=$(bnr_source)BANNERSOUND.OBJ) -
*22E
 	$(bnr_source)BANNER.OLB(BANNERDRAW=$(bnr_source)BANNERDRAW.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERSYSTEM=$(bnr_source)BANNERSYSTEM.OBJ) -
*9E
 	$(bnr_source)BANNER.OLB(BANNERSUBS=$(bnr_source)BANNERSUBS.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERPROCESS=$(bnr_source)BANNERPROCESS.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERMESSAGE=$(bnr_source)BANNERMESSAGE.OBJ) -
*23D
 	$(bnr_source)BANNER.OLB(BANNERMONITOR=$(bnr_source)BANNERMONITOR.OBJ) -
*23E
*23I
 	$(bnr_source)BANNER.OLB(BANNERLINK=$(bnr_source)BANNERLINK.OBJ) -
*23E
*26I
*27D
 	$(bnr_source)BANNER.OLB(BANNERMON=$(bnr_source)BANNERMON.OBJ) -
*27E
*27I
 	$(bnr_source)BANNER.OLB(BANNERMONITOR=$(bnr_source)BANNERMONITOR.OBJ) -
*27E
*26E
 	$(bnr_source)BANNER.OLB(BANNERCPU=$(bnr_source)BANNERCPU.OBJ) -
*15I
 	$(bnr_source)BANNER.OLB(BANNERTP=$(bnr_source)BANNERTP.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERQP=$(bnr_source)BANNERQP.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERQB=$(bnr_source)BANNERQB.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERCLU=$(bnr_source)BANNERCLU.OBJ) -
*15E
 	$(bnr_source)BANNER.OLB(BANNERPE=$(bnr_source)BANNERPE.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERLCK=$(bnr_source)BANNERLCK.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERCWPS=$(bnr_source)BANNERCWPS.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERGLOBE=$(bnr_source)BANNERGLOBE.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERCUBE=$(bnr_source)BANNERCUBE.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERHANOI=$(bnr_source)BANNERHANOI.OBJ) -
 	$(bnr_source)BANNER.OLB(BANNERCLOCK=$(bnr_source)BANNERCLOCK.OBJ) -
*9I
 	$(bnr_source)BANNER.OLB(BANNERALARM=$(bnr_source)BANNERALARM.OBJ) -
*9E
 	$(bnr_source)BANNER.OLB(BANNERREMOTE=$(bnr_source)BANNERREMOTE.OBJ) 
 	pur *.*
 
 
 !********************************************************************************
 !
 ! MMS dependancies for the code modules
 !
 !********************************************************************************
*31D
 $(bnr_source)bannerremote.obj : -
*31E
*28D
 		$(bnr_source)PEM_DEF.R32 -
 		$(bnr_source)BANNER-PEM.R32 -
*28E
*28I
*30D
 		$(bnr_source)BANNER_PE.R32
*30E
*30I
*31D
 		$(bnr_source)BANNER_PE.R32 -
 		$(bnr_source)lanudef.R32 -
 		$(bnr_source)pem_def.R32 
*31E
*31I
 $(bnr_source)bannerremote.obj : $(bnr_source)BANNER.H
 !		$(bnr_source)BANNER_PE.R32 -
 !		$(bnr_source)lanudef.R32 -
 !		$(bnr_source)pem_def.R32 
*31E
*30E
*28E
 
 $(bnr_source)banner.obj : -
 		$(bnr_source)BANNER.H 
*9I
 $(bnr_source)bannermain.obj : -
 		$(bnr_source)BANNER.H 
*22I
 $(bnr_source)bannersound.obj : -
 		$(bnr_source)BANNER.H 
*22E
 $(bnr_source)bannerdraw.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannersystem.obj : -
 		$(bnr_source)BANNER.H 
*9E
 $(bnr_source)bannersubs.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannerclock.obj : -
 		$(bnr_source)BANNER.H 
*9I
 $(bnr_source)banneralarm.obj : -
 		$(bnr_source)BANNER.H 
*9E
 $(bnr_source)bannerglobe.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannerhanoi.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannercpu.obj : -
 		$(bnr_source)BANNER.H 
*15I
 $(bnr_source)bannertp.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannerclu.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannerqp.obj : -
 		$(bnr_source)BANNER.H 
 $(bnr_source)bannerqb.obj : -
 		$(bnr_source)BANNER.H 
*15E
*23D
 $(bnr_source)bannermonitor.obj : -
*23E
*23I
 $(bnr_source)bannerlink.obj : -
*23E
 		$(bnr_source)BANNER.H 
*26I
*27D
 $(bnr_source)bannermon.obj : -
*27E
*27I
 $(bnr_source)BANNERMONITOR.obj : -
*27E
 		$(bnr_source)BANNER.H 
*26E
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
*28D
 		$(bnr_source)BANNER.H 
*28E
*28I
*31D
 		$(bnr_source)BANNER.H -
*31E
*30D
 		$(bnr_source)BANNER_PE.H 
*30E
*30I
*31D
 		$(bnr_source)BANNER_PE.H -
 		$(bnr_source)lanudef.H -
 		$(bnr_source)pem_def.H 
*31E
*31I
 		$(bnr_source)BANNER.H 
 !		$(bnr_source)BANNER_PE.H -
 !		$(bnr_source)lanudef.H -
 !		$(bnr_source)pem_def.H 
*31E
*30E
*28E
 
 
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
