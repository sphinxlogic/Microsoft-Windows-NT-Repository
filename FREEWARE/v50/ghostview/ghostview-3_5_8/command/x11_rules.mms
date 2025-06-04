!### X11 Rules and Macros

!### OpenVMS AXP #############################################################

.ifdef __alpha__
PLATFORM      = OpenVMS AXP ''f$getsyi(""VERSION"")'
ALPHA_EXT     = _Alpha
.endif

!### OpenVMS VAX #############################################################

.ifdef __vax__
PLATFORM      = OpenVMS VAX ''f$getsyi(""VERSION"")'
ALPHA_EXT     =
.endif

!### Compiler Specifications #################################################

OBJ_EXT       = OBJ$(ALPHA_EXT)
EXE_EXT       = EXE$(ALPHA_EXT)
MAP_EXT       = MAP$(ALPHA_EXT)
MAR_EXT       = MAR$(ALPHA_EXT)
OLB_EXT       = OLB$(ALPHA_EXT)
OPT_EXT       = OPT$(ALPHA_EXT)

.ifdef __DECC__
CC_COMMAND = $(__DECC__)
CC_COMPILER = DEC C
.endif

.ifdef __VAXC__
CC_COMMAND = $(__VAXC__)
CC_COMPILER = VAX C
.endif

.SUFFIXES :
.SUFFIXES : .$(EXE_EXT) .$(OLB_EXT) .$(OBJ_EXT) .c .mar

!### Linker Specifications ###################################################

.ifdef __alpha__
UCX_LIBRARY = SYS$SHARE:UCX$IPC_SHR.EXE/SHARE
.endif

.ifdef __vax__
UCX_LIBRARY = SYS$SHARE:UCX$IPC.OLB/LIBRARY
.endif

.ifdef __DECC__
RTL_LIBRARY = SYS$SHARE:DECC$SHR.EXE/SHARE
.endif

.ifdef __VAXC__
RTL_LIBRARY = SYS$SHARE:VAXCRTL.EXE/SHARE
.endif

!### X11 Release Specifications ##############################################

.ifdef X11R5
IF_R4 = if "?" .eqs. "" then
IF_R5 = 
.endif

.ifdef X11R4
IF_R4 =
IF_R5 = if "?" .eqs. "" then
.endif

.ifdef VERSION
SHOW_VERSION = , Version $(VERSION)
.else
SHOW_VERSION =
.endif

!### First Action ############################################################

.FIRST
   @		write_ sys$output ""
   @		write_ sys$output "  Target:       $(TARGET) $(SHOW_VERSION)"
   @ $(IF_R4)	write_ sys$output "  Platform:     X11 Release 4 on $(PLATFORM)"
   @ $(IF_R5)	write_ sys$output "  Platform:     X11 Release 5 on $(PLATFORM)"
   @		write_ sys$output "  Compiler:     $(CC_COMPILER) invoked by $(__DECC__)$(__VAXC__)"
   @		write_ sys$output "  CC Qualifier: $(CC_QUALIFIER)"
   @            mms_temp := $(CC_DEFINES)
   @		write_ sys$output "  CC Defines:   ",mms_temp
   @		write_ sys$output "  Current default: ''f$environment(""default"")'"
   @		write_ sys$output ""
   @		write_ sys$output "  Starting production of $(TARGET) $(TARGET_TYPE)"

!### Compiler Instructions ###################################################

.C.$(OBJ_EXT)
 @ _l_ = f$length("$(MMS$SOURCE)")
 @ _s_ = "$(MMS$SOURCE) "
 @ if _l_ .le. 20 then _s_ = _s_ + f$extract(0,20-_l_,"                          ")
 @ write_ sys$output "  compiling: ''_s_' -> $(MMS$TARGET)"
 @ $(CC_COMMAND)$(CC_QUALIFIER)$(CC_DEFINES) /obj=$(MMS$TARGET) $(MMS$SOURCE)
