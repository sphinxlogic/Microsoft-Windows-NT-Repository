!### makefile for Xmu shared library  

.ifdef STATIC
TARGET = XMULIB
TARGET_TYPE = object library
TARGET_EXT = $(OLB_EXT)
.else
TARGET = XMULIBSHR
TARGET_TYPE = shared library
TARGET_EXT = $(EXE_EXT)
.endif

.ifdef X11R4
VERSION = XMU_R4_V1.2
.else
.ifdef X11R5
VERSION = XMU_R5_V1.2
.else
VERSION = XMU_V1.2
.endif
.endif

CC_DEFINES =

.ifdef __DECC__
#CC_QUALIFIER = /NODEB/NOSHARE/EXT=STRICT/PREF=ALL/define=_DECC_V4_SOURCE
CC_QUALIFIER = /NODEB/NOSHARE/EXT=STRICT/PREF=ALL
.endif

.ifdef __VAXC__
CC_QUALIFIER = /NODEB/OPT=(D,I)
.endif

.include X11_ROOT:[COMMAND]X11_RULES.MMS

!### Objects ###################################################################

OBJS1 =\
ALLCMAP.$(OBJ_EXT),\
ATOMS.$(OBJ_EXT),\
CLIENTWIN.$(OBJ_EXT),\
CLOSEHOOK.$(OBJ_EXT),\
CMAPALLOC.$(OBJ_EXT)

OBJS2 =\
CRCMAP.$(OBJ_EXT),\
CRPIXFBIT.$(OBJ_EXT),\
CURSORNAME.$(OBJ_EXT),\
CVTCACHE.$(OBJ_EXT),\
CVTSTDSEL.$(OBJ_EXT)

OBJS3 =\
DELCMAP.$(OBJ_EXT),\
DISPLAYQUE.$(OBJ_EXT),\
DISTINCT.$(OBJ_EXT),\
DRAWLOGO.$(OBJ_EXT),\
DRRNDRECT.$(OBJ_EXT)

OBJS4 =\
EDITRESCOM.$(OBJ_EXT),\
FTOCBACK.$(OBJ_EXT),\
GETHOST.$(OBJ_EXT),\
GRAYPIXMAP.$(OBJ_EXT),\
INITER.$(OBJ_EXT)

OBJS5 =\
LOCBITMAP.$(OBJ_EXT),\
LOOKUP.$(OBJ_EXT),\
LOOKUPCMAP.$(OBJ_EXT),\
LOWER.$(OBJ_EXT),\
RDBITF.$(OBJ_EXT)

OBJS6 =\
SCROFWIN.$(OBJ_EXT),\
SHAPEWIDG.$(OBJ_EXT),\
STDCMAP.$(OBJ_EXT),\
STRTOBMAP.$(OBJ_EXT),\
STRTOBS.$(OBJ_EXT)

OBJS7 =\
STRTOCURS.$(OBJ_EXT),\
STRTOGRAV.$(OBJ_EXT),\
STRTOJUST.$(OBJ_EXT),\
STRTOLONG.$(OBJ_EXT),\
STRTOORNT.$(OBJ_EXT)

OBJS8 =\
STRTOSHAP.$(OBJ_EXT),\
STRTOWIDG.$(OBJ_EXT),\
UPDMAPHINT.$(OBJ_EXT),\
VISCMAP.$(OBJ_EXT),\
WIDGETNODE.$(OBJ_EXT)

OBJS9 =\
XCT.$(OBJ_EXT),\
VMS_STRTOPMAP.$(OBJ_EXT),\
VMS_COMPAT.$(OBJ_EXT)

.ifdef X11R5
OBJS10 =
.else
OBJS10 = VMS_R4_COMPAT.$(OBJ_EXT)
.endif

! removed from List: DEFERRMSG.$(OBJ_EXT) (no xlibint.h)
!                    SHAREDLIB.$(OBJ_EXT)

!### Targets ##################################################################

DEFAULT   :   $(TARGET).$(TARGET_EXT)
   @  write_ sys$output ""
   @  write_ sys$output "  $(TARGET).$(TARGET_EXT) available"
   @  write_ sys$output ""

$(TARGET).$(OLB_EXT)   :  $(OBJS1), $(OBJS2), $(OBJS3), $(OBJS4), $(OBJS5), \
			  $(OBJS6), $(OBJS7), $(OBJS8), $(OBJS9)  $(OBJS10)
  @ WRITE_ SYS$OUTPUT "  library creation of $(TARGET).$(OLB_EXT) ..."
  @ LIBRARY_/CREATE $(TARGET).$(OLB_EXT)_TEMP \
                          $(OBJS1), $(OBJS2), $(OBJS3), $(OBJS4), $(OBJS5), \
			  $(OBJS6), $(OBJS7), $(OBJS8), $(OBJS9)  $(OBJS10)
  @ LIBRARY_/COMPRESS=BLOCKS=0 $(TARGET).$(OLB_EXT)_TEMP/OUTPUT=$(TARGET).$(OLB_EXT)
  @ DELETE_ $(TARGET).$(OLB_EXT)_TEMP;*

OPTION_FILES =\
  _OBJS.$(OPT_EXT)/OPT,\
  _LINK.$(OPT_EXT)/OPT

$(TARGET).$(EXE_EXT)   : $(OBJS1), $(OBJS2), $(OBJS3), $(OBJS4), $(OBJS5), \
			 $(OBJS6), $(OBJS7), $(OBJS8), $(OBJS9)  $(OBJS10)
  @ WRITE_ SYS$OUTPUT "  generating $(TARGET).$(OPT_EXT)"
  @ OPEN_/WRITE FILE  _OBJS.$(OPT_EXT)
  @ WRITE_ FILE "!"
  @ WRITE_ FILE "! _OBJS.$(OPT_EXT) generated by DESCRIP.$(MMS_EXT)" 
  @ WRITE_ FILE "!"
  @ WRITE_ FILE "IDENTIFICATION=""$(VERSION)"""
  @ WRITE_ FILE "$(OBJS1)"
  @ WRITE_ FILE "$(OBJS2)"
  @ WRITE_ FILE "$(OBJS3)"
  @ WRITE_ FILE "$(OBJS4)"
  @ WRITE_ FILE "$(OBJS5)"
  @ WRITE_ FILE "$(OBJS6)"
  @ WRITE_ FILE "$(OBJS7)"
  @ WRITE_ FILE "$(OBJS8)"
  @ WRITE_ FILE "$(OBJS9)"
  @ WRITE_ FILE "$(OBJS10)"
  @ WRITE_ FILE "$(RTL_LIBRARY)"
  @ WRITE_ FILE "SYS$SHARE:DECW$XEXTLIBSHR/SHARE"
  @ WRITE_ FILE "SYS$SHARE:DECW$XLIBSHR/SHARE"
  @ $(IF_R4) WRITE_ FILE "SYS$SHARE:DECW$XTSHR/SHARE"
  @ $(IF_R5) WRITE_ FILE "SYS$SHARE:DECW$XTLIBSHRR5/SHARE"
  @ CLOSE_ FILE
  @ WRITE_ SYS$OUTPUT "  generating $(TARGET).$(MAP_EXT) ..."
  @ LINK_/NODEB/NOSHARE/NOEXE/MAP=$(TARGET).$(MAP_EXT)/FULL _OBJS.$(OPT_EXT)/OPT
  @ WRITE_ SYS$OUTPUT "  analyzing $(TARGET).$(MAP_EXT) ..."
  @ @X11_LIBRARY:ANALYZE_MAP.COM $(TARGET).$(MAP_EXT) XMU_DIRECTORY:_LINK.$(OPT_EXT)
  @ WRITE_ SYS$OUTPUT "  linking $(TARGET).$(EXE_EXT) ..."
  @ LINK_/NODEB/SHARE=$(TARGET).$(EXE_EXT)/MAP=$(TARGET).$(MAP_EXT)/FULL $(OPTION_FILES)

# dependencies!!!  --  Richard Levitte  /* thank you ###jp### */
 
ALLCMAP.$(OBJ_EXT)	:	STDCMAP.H
ATOMS.$(OBJ_EXT) 	:	ATOMS.H
CLOSEHOOK.$(OBJ_EXT) 	:	VMS_DEFS.H CLOSEHOOK.H VMS_R4_COMPAT.H
CURSORNAME.$(OBJ_EXT) 	:	CHARSET.H CURUTIL.H
CVTCACHE.$(OBJ_EXT)	:	VMS_DEFS.H CVTCACHE.H DISPLAYQUE.H CLOSEHOOK.H
CVTSTDSEL.$(OBJ_EXT)	:	VMS_SOCKET.H ATOMS.H STDSEL.H SYSUTIL.H VMS_COMPAT.H
DISPLAYQUE.$(OBJ_EXT)	:	DISPLAYQUE.H CLOSEHOOK.H
EDITRESCOM.$(OBJ_EXT)	:	XAW_DIRECTORY:CARDINALS.H EDITRESP.H CHARSET.H
GETHOST.$(OBJ_EXT)	:	VMS_R4_COMPAT.H
INITER.$(OBJ_EXT)	:	INITER.H
LOCBITMAP.$(OBJ_EXT)	:	CVTCACHE.H DISPLAYQUE.H CLOSEHOOK.H DRAWING.H VMS_R4_COMPAT.H
LOOKUPCMAP.$(OBJ_EXT)	:	STDCMAP.H
LOWER.$(OBJ_EXT)	:	CHARSET.H
RDBITF.$(OBJ_EXT)	:	VMS_R4_COMPAT.H
SHAPEWIDG.$(OBJ_EXT)	:	CONVERTERS.H DRAWING.H
STDCMAP.$(OBJ_EXT)	:	STDCMAP.H
STRTOBMAP.$(OBJ_EXT)	:	CONVERTERS.H DRAWING.H VMS_R4_COMPAT.H
STRTOBS.$(OBJ_EXT)	:	CONVERTERS.H CHARSET.H
STRTOCURS.$(OBJ_EXT)	:	CONVERTERS.H DRAWING.H CURUTIL.H VMS_DEFS.H
STRTOGRAV.$(OBJ_EXT)	:	CONVERTERS.H VMS_R4_COMPAT.H CHARSET.H
STRTOJUST.$(OBJ_EXT)	:	VMS_R4_COMPAT.H CONVERTERS.H CHARSET.H
STRTOLONG.$(OBJ_EXT)	:	CONVERTERS.H
STRTOORNT.$(OBJ_EXT)	:	VMS_R4_COMPAT.H CONVERTERS.H CHARSET.H
STRTOSHAP.$(OBJ_EXT)	:	CONVERTERS.H CHARSET.H
STRTOWIDG.$(OBJ_EXT)	:	VMS_DEFS.H
VISCMAP.$(OBJ_EXT)	:	STDCMAP.H
WIDGETNODE.$(OBJ_EXT)	:	XAW_DIRECTORY:CARDINALS.H CHARSET.H WIDGETNODE.H
XCT.$(OBJ_EXT)		:	XCT.H VMS_COMPAT.H
VMS_STRTOPMAP.$(OBJ_EXT) :	CONVERTERS.H DRAWING.H
VMS_R4_COMPAT.$(OBJ_EXT) :
VMS_COMPAT.$(OBJ_EXT)	:
