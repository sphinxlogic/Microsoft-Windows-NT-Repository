#
#  MMS file to build FLIST
#
#  Author:	Hunter Goatley, goathunter@WKUVX1.BITNET
#
#  Date:	November 19, 1992
#
#  To compile with the AMC Porting Toolkit for OpenVMS AXP:
#
#	$ MMS/MACRO=ALPHA=1
#
#
.IFDEF __ALPHA__
CC	= CC/NOLIST/STANDARD=VAXC
LINK	= LINK/NOTRACE
.ELSE
CC	= CC
LINK	= LINK/NOTRACE
C_OPT	= ,FLIST_VAX.OPT/OPTIONS
.ENDIF

flist :		flist$(exe), flist.tpu$section, flist.hlp
	! FLIST build complete

flist$(exe) :	flist$(obj)
	$(LINK)/EXEC=$(MMS$TARGET) $(MMS$SOURCE)$(C_OPT)

flist$(obj) :	flist.c

flist.tpu$section :	flist.tpu
	edit/tpu/nodisplay/nosection/nojournal/command=$(MMS$SOURCE)

flist.hlp :	flist.rnh
	runoff $(MMS$SOURCE)
