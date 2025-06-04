# LibAstro Description Makefile for OpenVMS
#
#   See top-level DESCRIP.MMS for info

OBJS =  AA_HADEC.obj ABERRATION.obj ANOMALY.obj AP_AS.obj AUXIL.obj \
	CHAP95.obj CHAP95_DATA.obj CIRCUM.obj COMET.obj DBFMT.obj DELTAT.obj \
	EARTHSAT.obj EQ_ECL.obj EQ_GAL.obj FORMATS.obj HELIO.obj LIBRATION.obj \
	MISC.obj MJD.obj MOON.obj MOONCOLONG.obj NUTATION.obj OBLIQ.obj \
	PARALLAX.obj PLANS.obj PRECESS.obj REDUCE.obj REFRACT.obj RISET.obj \
	RISET_CIR.obj SPHCART.obj SUN.obj UTC_GST.obj VSOP87.obj VSOP87_DATA.obj

LibAstro.olb :  $(OBJS)
	If "''F$Search ("LibAstro.olb")'" .eqs. "" Then Library /Create LibAstro.olb	
	Library /Replace LibAstro.olb $+

clobber :
	- Purge /NoConfirm *.obj
	- Delete /NoConfirm *.obj;,*.olb;
