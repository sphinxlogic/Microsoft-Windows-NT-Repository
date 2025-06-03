!
!  MMS build file for BAT
!
!  Hunter Goatley, July 15, 1992
!
.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
OLB = .OLB
.ENDIF

.IFDEF __DEBUG__
BFLAGS = $(BFLAGS)/DEBUG/NOOPTIMIZE
LINKFLAGS = $(LINKFLAGS)/DEBUG
.ELSE
LINKFLAGS = $(LINKFLAGS)/NOTRACE
.ENDIF

USEBLISS = 1			!Comment out if you don't have BLISS

OBJS	= bat, bat_msg, bat_cld, hg$get_input
SOURCES = bat.b32, bat.mar, hg$get_input.b32, hg$get_input.mar, -
	  bat_cld.cld, bat_msg.msg, descrip.mms, bat.rnh, build_bat.com

bat	: bat$(EXE), bat.hlp
	!BAT built

kit 	: $(SOURCES)
	set default [-]
	@h:vms_share [.bat]*.*; bat 60

bat$(EXE) : BAT$(OLB)($(OBJS))
	$(LINK)$(LINKFLAGS) BAT$(OLB)/INCLUDE=BAT/LIBRARY

.IFDEF USEBLISS
bat$(obj)		: bat.b32
hg$get_input$(obj)	: hg$get_input.b32
.ELSE
bat.obj		: bat.mar
hg$get_input	: hg$get_input.mar
.ENDIF

bat_msg.obj	: bat_msg.msg
bat_cld.obj	: bat_cld.cld

bat.hlp		: bat.rnh
	RUNOFF $(MMS$SOURCE)
