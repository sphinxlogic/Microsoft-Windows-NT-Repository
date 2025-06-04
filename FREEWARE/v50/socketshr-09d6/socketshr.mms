!++
!	Build file for SOCKETSHR_xxx.EXE
!	V1.1			8-Aug-1994	IfN/Mey
!
!	.INCLUDED from the package specific MMS-Files.
!	Input macros:
!
!	PACK		- UCX, NETLIB, etc...
!	PACKOBJS	- specific object modules
!	TRACE		- if specified, compile with trace code
!
!	01-Dec-1998	J. Malmberg	GCC / DECCRTL build
!					MMS barfs on comments ending in a dash.
!
!	05-Dec-1998	J. Malmberg	DECCRTL must not be used with the
!					FILE * structure from VAXCRTL
!					So SOCKETSHR/VAX must be built against
!					the VAXCRTL (ugh) to keep programs
!					from breaking.
!			    *		For Images that you want to be free
!					of the old bugs, I am providing a
!					SOCKETSHRD_xxx.EXE.  At this time it
!					is only implemented for GCC.  To
!					Give the DEC C users some incentive
!					to add the stuff needed and notice
!					that they are otherwise silently
!					breaking existing programs if they
!					do not!
!					You would not use VAXC if you can
!					avoid it, and GCC is a free replacement.
!--x

!__ALPHA__ = 1		! MMK does it automatically

.IFDEF __ALPHA__
ARCH		= ALPHA
__DECC__	= 1
.ELSE
ARCH		= VAX
.IFDEF __GCC__
.ELSE
__VAXC__ 	= 1
.ENDIF
XFROBJ		= ,socketshr_xfr_vax.obj
.ENDIF

.IFDEF TRACE
CCDEFINE	= /define=($(PACK),TRACE)
.ELSE
CCDEFINE	= /define=($(PACK))
.ENDIF

.IFDEF DEBUG
.ELSE
DEBUG		=
.ENDIF

.IFDEF LDEBUG
.ELSE
LDEBUG		= /NOTRACE
.ENDIF

.IFDEF __GCC__
CLIST		=
.ELSE
CLIST		= /list/show=all
.ENDIF

.IFDEF __DECC__
CFLAGS		= /OBJECT=$(MMS$TARGET_NAME) $(DEBUG) $(CCDEFINE) -
		/prefix=all /stand=vaxc
! Not correct for VAX.  Must complied to link with VAXCRTL or will not
! work with existing programs that were.
.ELSE
CFLAGS		= /OBJECT=$(MMS$TARGET_NAME) $(DEBUG) $(CCDEFINE)
.ENDIF
MFLAGS		= /OBJECT=$(MMS$TARGET_NAME) $(DEBUG)
LFLAGS		= $(LDEBUG)

#
.c.obj		:
	cc $(CFLAGS) $(MMS$SOURCE) $(CLIST)

.mar.obj	:
	macro $(MFLAGS) $(MMS$SOURCE) /list/show=exp

OBJS		= si_socket.obj, $(PACKOBJS)
.IFDEF __GCC__
OPT		= socketshr_$(PACK)_$(ARCH)_GNUC.opt
OPT2		= socketshrd_$(PACK)_$(ARCH)_GNUC.opt
.ELSE
OPT		= socketshr_$(PACK)_$(ARCH).opt
OPT2		= socketshrd_$(PACK)_$(ARCH)_GNUC.opt
.ENDIF
XFROPT		= [-]socketshr_xfr_$(ARCH).opt

.IFDEF __ALPHA__
.ELSE
.IFDEF __VAXC__
.ELSE
both : socketshr_$(PACK), socketshrd_$(PACK)
	write sys$output "socketshr and socketshrd are up to date."
.ENDIF
.ENDIF

socketshr_$(PACK) :	$(OBJS) $(XFROBJ) , $(OPT), $(XFROPT), \
			descrip.mms, [-]socketshr.mms
		$(link) $(LFLAGS)/share=socketshr_$(PACK).exe -
			/map=$*.map/full  -
			$(OBJS), $(OPT)/opt, $(XFROPT)/opt

socketshrd_$(PACK) :	$(OBJS) $(XFROBJ) , $(OPT), $(XFROPT), \
			descrip.mms, [-]socketshr.mms
		$(link) $(LFLAGS)/share=socketshrd_$(PACK).exe -
			/map=socketshrd_$(PACK).map/full -
			$(OBJS), $(OPT2)/opt, $(XFROPT)/opt

socketlib_$(PACK) : $(OBJS)
		library/create socketlib_$(PACK)_$(ARCH).olb $(OBJS)

si_socket.obj :			[-]si_socket.c

ntoh_hton.obj :			[-]ntoh_hton.c

inet.obj :			[-]inet.c

trnlnm.obj :			[-]trnlnm.c

readdb.obj :			[-]readdb.c

socketshr_xfr_$(ARCH).obj :	[-]socketshr_xfr_$(ARCH).mar

