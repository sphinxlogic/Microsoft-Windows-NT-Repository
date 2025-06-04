!+
!
! MMS file for KILL.EXE.
!
! COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
!-
!
! Set up the default VAX extensions.
!
.IFDEF EXE
.ELSE
EXE = .EXE
OBJ = .OBJ
L32 = .L32
OLB = .OLB
.ENDIF

.IFDEF __ALPHA__
BDEBUG = /DEBUG/NOOPTIMIZE
.ELSE
BDEBUG = /DEBUG
.ENDIF

.IFDEF __DEBUG__
DEBUG = $(BDEBUG)
LDEBUG = /DEBUG
.ELSE
DEBUG = /NOTRACE/NODEBUG
LDEBUG = /NOTRACE/NODEBUG
.ENDIF

BFLAGS = $(DEBUG)/OBJECT=$(MMS$TARGET)
LINKFLAGS = $(LDEBUG)/EXE=$(MMS$TARGET)

all			: kill$(exe), kill.hlp
	@ !
!
! KILL.EXE
!
kill$(EXE)		: kill$(OBJ), kill_msg$(OBJ), kill_cld$(OBJ)
	$(LINK) $(LINKFLAGS) $(MMS$SOURCE_LIST)
kill$(OBJ)		: kill.b32, kill_defs$(L32)
kill_msg$(OBJ)		: kill_msg.msg
kill_cld$(OBJ)		: kill_cld.cld

kill_defs$(L32)		: kill_defs.r32,fields$(L32)
fields$(L32)		: fields.r32
!
! KILL.HLP
!
kill.hlp		: kill.sdml
	document kill help hlp
