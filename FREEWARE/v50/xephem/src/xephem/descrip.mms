#   XEphem v3.2.3 MMS Description Makefile
#
#   See top-level DESCRIP.MMS file for info
#

INCS = /Include = ([], [-.LibAstro])
DEFS = $(INCS) /Prefix = All /Warnings = NoInformationals
CFLAGS = $(CFLAGS) $(DEFS)
OPTS = Sys$Disk:[]Options.opt/Option

OBJS =  ATLAS.obj BDL.obj BROADCAST.obj CALMENU.obj CLOSEMENU.obj \
	COMPILER.obj CONSTEL.obj DATAMENU.obj DB.obj DBMENU.obj EARTHMAP.obj \
	EARTHMENU.obj EXPLODEGIF.obj FALLBACKS.obj FITS.obj FORMATS.obj \
	FSMENU.obj GSC.obj GSCNET.obj HELPMENU.obj HOMEIO.obj JUPMENU.obj \
	LISTMENU.obj MAINMENU.obj MARSMENU.obj MARSMMENU.obj MOONMENU.obj \
	MOONNF.obj MSGMENU.obj NETMENU.obj OBJMENU.obj PATCHLEVEL.obj \
	PLOTMENU.obj PLOT_AUX.obj PPM.obj PREFERENCES.obj PROGRESS.obj PS.obj \
	QUERY.obj ROTATED.obj SATMENU.obj SITES.obj SKYFILTMENU.obj \
	SKYFITS.obj SKYHIST.obj SKYLIST.obj SKYVIEWMENU.obj SOLSYSMENU.obj \
	SRCHMENU.obj TIME.obj TIPS.obj TRAILMENU.obj URANUSMENU.obj USNO.obj \
	VERSIONMENU.obj WCS.obj XMISC.obj

xephem.exe :	xephem.obj $(OBJS)
	- $(LINK) $(LINKFLAGS) $+,$(OPTS)

homeio.obj :	homeio.c
	- $(CC) $(CFLAGS) $+

netmenu.obj :	netmenu.c
	- $(CC) $(CFLAGS) $+

clean :
	- Purge /NoConfirm *.obj, *.exe

clobber :	clean
	- Delete /NoConfirm *.obj;, *.exe;
