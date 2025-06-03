!
! MMS build file for SEND
!

.IFDEF __ALPHA__
DBG_QUALS=/DEBUG/NOOPT
.ELSE
DBG_QUALS=/DEBUG
EXE=.EXE
OBJ=.OBJ
L32=.L32
.ENDIF

.IFDEF __DEBUG__
DEBUG=$(DBG_QUALS)
LDEBUG=/DEBUG
.ELSE
DEBUG=/NODEBUG
LDEBUG=/NODEBUG/NOTRACE
.ENDIF

BFLAGS	= $(BFLAGS)$(DEBUG)
LINKFLAGS = $(LINKFLAGS)$(LDEBUG)

both			: send_decnet$(EXE), send_server$(exe)
	!SEND client and server built.

send_decnet$(EXE)	: send_decnet$(OBJ), send_common$(OBJ),-
			  send_cld$(OBJ), send_msg$(OBJ)
	$(LINK)$(LINKFLAGS) $(MMS$SOURCE_LIST)

send_server$(EXE)	: send_server$(obj), send_common$(OBJ)
	$(LINK)$(LINKFLAGS) $(MMS$SOURCE_LIST)

send_decnet$(OBJ)	: send_decnet.b32, send_defs$(L32), send_comm_defs$(L32)
send_cld$(OBJ)		: send_cld.cld
send_msg$(OBJ)		: send_msg.msg

send_server$(OBJ)	: send_server.b32, send_server_defs$(L32), -
			  send_comm_defs$(L32)

send_common$(OBJ)	: send_common.b32

send_defs$(L32)		: send_defs.r32, fields$(L32), send_comm_defs$(L32)
send_server_defs$(L32)	: send_server_defs.r32, fields$(L32)
send_comm_defs$(L32)	: send_comm_defs.r32, fields$(L32)
fields$(L32)		: fields.r32

send.hlp		: send.sdml
	document $(MMS$SOURCE) help hlp
