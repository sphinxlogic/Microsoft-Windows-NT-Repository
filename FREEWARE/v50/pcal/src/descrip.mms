# An OpenVMS MMS description file to build the Pcal program.
#
# This has been tested on VAX/OpenVMS v5.5-2 with both DEC C v5.7-004
# and VAX C v3.2-044 as well as on Alpha/OpenVMS v6.2 & v7.1 with
# DEC C v5.7-004 and on Alpha/OpenVMS v7.2 with DEC C v6.0-001.
#
# Written by: Rick Dyson (richard-dyson@uiowa.edu) for pcal v4.5
#              6-OCT-1994 
#             27-DEC-1996 updated for v4.6
#             31-JUL-1997 updated for v4.7
#              7-FEB-2000 updated for v4.7-VMS
#
# To build on an old VAX with VAX C, type
#           MMS /Macro = (VAXC=1)
#
# To build with DEC C, just type
#           MMS 
#

# Site-specific defaults which may be overridden here (cf. pcallang.h);
# uncomment the examples below and/or change them to your liking

# include Roman8 or ISO Latin1 8-bit character mappings by defining MAPFONTS
# as ROMAN8 or LATIN1 respectively
# D_MAPFONTS = "MAPFONTS=ROMAN8",

# redefine title, date, and notes font/pointsize (-t, -d, -n)
# D_TITLEFONT = "TITLEFONT=""Helvetica-Bold/48""",
# D_DATEFONT = "DATEFONT=""Helvetica-Bold/28""",
# D_NOTESFONT = "NOTESFONT=""Helvetica/8""",
D_NOTESFONT = "NOTESFONT=""Helvetica/8""",

# redefine shading (dates/fillboxes)
# D_SHADING = "SHADING=""0.6/0.99""",

# specify American (USA_DATES) or European (EUR_DATES) date parsing (-A, -E)
# D_DATE_STYLE = "DATE_STYLE=USA_DATES",

# specify first (leftmost) weekday on calendar (-F)
# D_FIRST_DAY  = "FIRST_DAY=SUN",

# specify default time zone for moon phases: "0" = GMT; "5" = Boston (-z)
# D_TIMEZONE = "TIMEZONE=""6 [Chicago]""",
D_TIMEZONE = "TIMEZONE=""6 [Chicago]""",

# generate EPS-like PostScript (-DEPS)
# D_EPS = EPS,
D_EPS = EPS,

# specify default language
# D_LANGUAGE = "LANG_DEFAULT=LANG_ENGLISH",

# customize HTML output
# D_BGCOLOR = "BGCOLOR=""ffffff""",
# D_BACKGROUND = "BACKGROUND=NULL",
# D_TEXT = "TEXT=NULL",
# D_LINK = "LINK=NULL",
# D_ALINK = "ALINK=NULL",
# D_VLINK = "VLINK=NULL",
# D_HOLIDAY_PRE = "HOLIDAY_PRE=""<font color=\""ff0000\""><b>""",
# D_HOLIDAY_POST = "HOLIDAY_POST=""</b></font>""",
# D_BLANK_STYLE = "DIVIDE_BLANK_SPACE=0",

.ifdef VAXC
EXTRAS =
OPTS = ,VAXCRTL.OPT /Option
.else
EXTRAS = /Standard = VAXC
OPTS = 
.endif

DEFINES = /Define = ($(D_MAPFONTS) $(D_TITLEFONT) $(D_DATEFONT) $(D_NOTESFONT) \
	$(D_SHADING) $(D_DATE_STYLE) $(D_FIRST_DAY) $(D_TIMEZONE) $(D_EPS) \
	$(D_LANGUAGE) $(D_BGCOLOR) $(D_BACKGROUND) $(D_TEXT) $(D_LINK) \
	$(D_ALINK) $(D_VLINK) $(D_HOLIDAY_PRE) $(D_HOLIDAY_POST) \
	$(D_BLANK_STYLE) VMS)
CFLAGS = $(CFLAGS) $(DEFINES) $(EXTRAS) /Optimize /NoDebug
LINKFLAGS = $(LINKFLAGS) /NoDebug

OBJECTS = exprpars.obj moonphas.obj pcalutil.obj readfile.obj writefil.obj
OBJLIST = exprpars,moonphas,pcalutil,readfile,writefil

.last
	@- Set Message /Facility /Severity /Identification /Text

pcal :	pcal.exe pcal.hlb
	@ !
	@ Write Sys$Output "pcal (v4.7) is now ready for use.  Use @setup to define variables"

help :	pcal.hlb
	@ !

pcal.exe :	pcal.obj $(OBJECTS) pcalinit.h
	$(LINK) $(LINKFLAGS) pcal,$(OBJLIST)$(OPTS)

pcalinit.exe :	pcalinit.obj
	$(LINK) $(LINKFLAGS) pcalinit$(OPTS)

pcalinit.h :	pcalinit.exe pcalinit.ps
	MCR Sys$Disk:[]pcalinit pcalinit.ps pcalinit.h header

pcal.hlb :	pcal.hlp
exprpars.obj :	exprpars.c pcaldefs.h
moonphas.obj :	moonphas.c pcaldefs.h pcalglob.h
pcal.obj :	pcal.c pcaldefs.h pcalglob.h pcallang.h protos.h
pcalutil.obj :	pcalutil.c pcaldefs.h pcalglob.h pcallang.h protos.h
readfile.obj :	readfile.c pcaldefs.h pcalglob.h pcallang.h protos.h
writefil.obj :	writefil.c pcaldefs.h pcalglob.h pcallang.h pcalinit.h protos.h
pcalinit :	pcalinit.exe

clean :
	@- Set Message /NoFacility /NoSeverity /NoIdentification /NoText
	@- Set Protection = Owner:RWED *.*;-1,*.obj,pcalinit.h,pcalinit.exe
	@- Purge /NoLog /NoConfirm
	@- Delete /NoLog /NoConfirm *.obj;*,pcalinit.h;,pcalinit.exe;

clobber : clean
	@- Purge /NoLog /NoConfirm
	@- Set Protection = Owner:RWED *.exe,*.hlb
	@- Delete /NoLog /NoConfirm *.exe;,pcal.hlb;

fresh :	clobber pcal
	@- Continue
