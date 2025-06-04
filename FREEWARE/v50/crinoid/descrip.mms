!
!   descrip.mms file for CRINOID
!       perl should already be set up
!       installation directory should be defined
!       @configure sets up the various macro definitions needed
!


.ifdef INSTALLDIR
.else
INSTALLDIR = [.install]
INSTALLLIB = [.install.lib]
.endif

CREATE_OPT = @create_opt.com

.ifdef SHOW
VCQSHOW = /show=($(SHOW))
.else
VCQSHOW =
.endif
.ifdef VERBOSE
VCQ = /list$(VCQSHOW)
VLQ = /map
MCQ = /list
.else
MCQ = /nolist
VCQ = /nolist
VLQ = /nomap
.endif
VCD =

.ifdef PERLDEBUG
PERLOPT = :DBGPERL
.else
PERLOPT = :PERL
.endif

.ifdef DEBUG
DCQ = /debug/noopt
DLQ = /debug
DDD = DBG
DCD = ,EXC_WORKAROUND=$(EXC_WORKAROUND)
DEBUG_TARGETS = -
    ,$(INSTALLDIR)DBG$CRINOID.INI  -
    ,$(INSTALLDIR)DBG$TENTACLE.INI -
    ,$(INSTALLDIR)DBG$LOGGER.INI   -
    ,$(INSTALLDIR)DO_DEBUG.COM
.else
DCQ = /nodebug/opt
DLQ = /nodebug/notrace
DDD = NDB
DCD =
DEBUG_TARGETS =
.endif

.ifdef __AXP__
ARCH = AXP
ACQ  = /PREFIX=ALL
E = .AXE
O = .ABJ
.else
ARCH = VAX
ACQ  =
E = .EXE
O = .OBJ
.endif
ACD =

.ifdef NO_PERSONA
PCD = ,NO_PERSONA
.endif

CRINOIDLIB = CRINOID_$(ARCH)_$(DDD).OLB
CDEFS   = XYZZY$(ACD)$(DCD)$(VCD)$(PCD)$(XCD)

CFLAGS  =    $(ACQ)$(DCQ)$(VCQ)/DEFINE=($(CDEFS))/Incl=([],$(PERLINC))
LFLAGS  =    $(DLQ)$(VLQ)
MSGFLAGS =   $(MCQ)

.suffixes
.suffixes  $(E) .OPT .OLB $(O) .C .MSG

.C$(O)    :
          $(CC)$(CFLAGS)/OBJ=$(MMS$TARGET) $(MMS$SOURCE)

.MSG$(O)  :
          $(MESSAGE)$(MSGFLAGS)/OBJECT=$(MMS$TARGET) $(MMS$SOURCE)

$(O).OLB  :
          @ IF F$SEARCH("$(MMS$TARGET)") .EQS. "" THEN $(LIBR)/CREATE/LOG $(MMS$TARGET)
          LIBRARIAN $(LIBRFLAGS) $(MMS$TARGET) $(MMS$SOURCE)

$(O).OPT  :
          $(CREATE_OPT) $(MMS$TARGET) $(MMS$SOURCE) $(CRINOIDLIB)/LIB

.OPT.$(E) :
          @ if "$(NO_PERSONA)" .nes. "" .and. "$(MMS$SOURCE)" .eqs. "STUB.OPT" .and. "$(ARCH)" .eqs. "AXP" then opt = "/sysexe"
          link$(LFLAGS)/exe=$(MMS$TARGET)'opt' $(MMS$SOURCE)/opt


all      :   CRINOID tentacle stub logger startup crinoid_msg $(SERVICE_NAME).com
         @ continue
CRINOID    : CRINOID$(E)
         @ continue
tentacle : tentacle$(E)
         @ continue
stub     : stub$(E)
         @ continue
logger   : logger$(E)
         @ continue
startup  : startup$(E)
         @ continue
perl_root:[lib.extutils]embed.pm :
         @ continue
configure.mms :
         @ continue
crinoid_msg : crinoid_msg$(E)
         @ continue

perlxsi.c    : perl_root:[lib.extutils]embed.pm  configure.mms
             perl "-MExtUtils::Embed" -e"xsinit('perlxsi.c');"

CRINOID$(E)  : CRINOID.opt  CRINOID$(O)  $(CRINOIDLIB)
tentacle$(E) : tentacle.opt TENTACLE$(O) $(CRINOIDLIB)
stub$(E)     : stub.opt     STUB$(O)     $(CRINOIDLIB)
logger$(E)   : logger.opt   LOGGER$(O)   $(CRINOIDLIB)
startup$(E)  :              STARTUP$(O)  $(CRINOIDLIB)
             link/exe=$(MMS$TARGET)  startup$(O),$(CRINOIDLIB)/LIB
crinoid_msg$(E) : crinoid_msg$(O)
             link/share=$(MMS$TARGET) $(MMS$SOURCE)

tentacle.opt : configure.mms
             $(CREATE_OPT) tentacle.opt tentacle$(O) $(CRINOIDLIB)/LIB $(PERLOPT) :linkopt=$(DLQ)

stub.opt     : configure.mms
        @ if "$(NO_PERSONA)" .nes. "" then tail2 = ":comment=need_cmkrnl"
        @ if "$(NO_PERSONA)" .nes. "" .and. "$(ARCH)" .eqs. "VAX" .and. f$search("sys$loadable_imagest:iodef.stb") .nes. "" then tail = "sys$loadable_images:iodef.stb/selective"
        @ if "$(NO_PERSONA)" .nes. "" .and. "$(ARCH)" .eqs. "VAX" .and. f$search("sys$loadable_imagest:iodef.stb") .eqs. "" then tail = "sys$system:sys.stb/selective"
        $(CREATE_OPT) stub.opt stub$(O) $(CRINOIDLIB)/LIB 'tail' 'tail2'

logger.opt   : configure.mms
             $(CREATE_OPT) logger.opt logger$(O) $(CRINOIDLIB)/LIB


CRINOID.opt    : configure.mms
             $(CREATE_OPT) CRINOID.opt CRINOID$(O) $(CRINOIDLIB)/LIB :threads :linkopt=$(DLQ)

$(SERVICE_NAME).com :
        @ write sys$output "Creating a new $(service_name).COM (in this directory)"
        @ open/write FD $(service_name).com
        @ write FD "$ f = f$parse(f$environ(""procedure""),,,""name"")"
        @ write FD "$ define CRINOID_DECNET_SERVICE 'f'"
        @ write FD "$ define CRINOID_MGR ""$(crinoid_mgr)"""
        @ write FD "$ @$(installdir)CRINOID.COM $(process_name)"
        @ write FD "$ exit"
        @ close FD

CRINOID$(O)       :     CRINOID.c CRINOID.h proc_mgr.h vms_data.h util.h pipe2.h msg.h CRINOID_types.h script.h version.h nfbdef.h
tentacle$(O)      :     tentacle.c tentacle.h perlxsi.c errlog_client.h vms_data.h util.h msg.h CRINOID_types.h mbxq.h pipe2.h
errlog_client$(O) :     errlog_client.c errlog_client.h vms_data.h msg.h util.h
proc_mgr$(O)      :     proc_mgr.c proc_mgr.h CRINOID_types.h util.h
util$(O)          :     util.c util.h CRINOID_types.h
parser$(O)        :     parser.c  parser.h util.h
stub$(O)          :     util.h  msg.h
script$(O)        :     util.h  script.h vms_data.h CRINOID_types.h
mbxq$(O)          :     mbxq.c mbxq.h  util.h
pipe2$(O)         :     pipe2.c  pipe2.h util.h errlog_client.h mbxq.h
startup$(O)       :     startup.c util.h
.ifdef NO_PERSONA
stub$(O)          :     pcbjibdef.h
pcbjibdef.h       :     sys$library:lib.mlb
                        $(CREATE_OPT) pcbjibdef.h  :PCBJIBDEF
.endif
nfbdef.h          :     sys$library:lib.mlb
                        $(CREATE_OPT) nfbdef.h  :NFBDEF

MODULES =-
        ERRLOG_CLIENT=ERRLOG_CLIENT$(O),-
        PROC_MGR=PROC_MGR$(O),-
        UTIL=UTIL$(O),-
        PARSER=PARSER$(O),-
        SCRIPT=SCRIPT$(O),-
        PIPE2=PIPE2$(O),-
        MBXQ=MBXQ$(O),-
        CRINOID_MSG=CRINOID_MSG$(O)

$(CRINOIDLIB)     :   $(CRINOIDLIB)($(MODULES))
                @ write sys$output "update of $(CRINOIDLIB) complete."

INSTALL_TARGETS = -
    $(INSTALLDIR)TENTACLE$(E),-
    $(INSTALLDIR)TENTACLE.OPT,-
    $(INSTALLDIR)TENTACLE.COM,-
    $(INSTALLDIR)LOGGER$(E),-
    $(INSTALLDIR)LOGGER.COM,-
    $(INSTALLDIR)CRINOID$(E),-
    $(INSTALLDIR)CRINOID.COM,-
    $(INSTALLDIR)STUB$(E),-
    $(INSTALLDIR)STUB.COM,-
    $(INSTALLDIR)STUB.OPT,-
    $(INSTALLDIR)CRINOID_MSG$(E),-
    $(INSTALLDIR)OYSTER.,-
    $(INSTALLLIB)SCRIPT.PM,-
    $(INSTALLLIB)TIEENV.PM,-
    $(INSTALLDIR)STARTUP.COM,-
    $(INSTALLDIR)STARTUP$(E),-
    $(INSTALLDIR)SHUTDOWN.COM,-
    $(INSTALLDIR)NEWLOG.COM   $(DEBUG_TARGETS)

$(INSTALLDIR)TENTACLE$(E) : TENTACLE$(E)
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)TENTACLE.OPT : TENTACLE.OPT
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)TENTACLE.COM : [.MISC]TENTACLE.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)LOGGER$(E)   : LOGGER$(E)
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)LOGGER.COM   : [.MISC]LOGGER.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)CRINOID$(E)  : CRINOID$(E)
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)CRINOID.COM  : [.MISC]CRINOID.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)STARTUP$(E)  : STARTUP$(E)
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)STUB$(E)     : STUB$(E)
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)STUB.COM     : [.MISC]STUB.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)STUB.OPT     : STUB.OPT
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)CRINOID_MSG$(E) : CRINOID_MSG$(E)
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)OYSTER.      : [.MISC]OYSTER.
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLLIB)SCRIPT.PM    : [.MISC]SCRIPT.PM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLLIB)TIEENV.PM    : [.MISC]TIEENV.PM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)STARTUP.COM  : [.MISC]STARTUP.COM
                            @ open/write fd1 $(MMS$TARGET)
                            @ write fd1 "$ CRINOID_manager  = ""$(CRINOID_MGR)"""
                            @ write fd1 "$ CRINOID_username = ""$(CRINOID_USR)"""
                            @ write fd1 "$ CRINOID_service  = ""$(SERVICE_NAME)"""
                            @ open/read fd2 $(MMS$SOURCE)
                            @ copy fd2: fd1:
                            @ close fd1
                            @ close fd2
                            @ WRITE SYS$OUTPUT "STARTUP.COM file written"
$(INSTALLDIR)SHUTDOWN.COM : [.MISC]SHUTDOWN.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)NEWLOG.COM   : [.MISC]NEWLOG.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)

$(INSTALLDIR)DO_DEBUG.COM  : [.DEBUG_SUPPORT]DO_DEBUG.COM
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)DBG$CRINOID.INI  : [.DEBUG_SUPPORT]DBG$CRINOID.INI
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)DBG$TENTACLE.INI  : [.DEBUG_SUPPORT]DBG$TENTACLE.INI
                            COPY $(MMS$SOURCE) $(MMS$TARGET)
$(INSTALLDIR)DBG$LOGGER.INI  : [.DEBUG_SUPPORT]DBG$LOGGER.INI
                            COPY $(MMS$SOURCE) $(MMS$TARGET)



INSTALL :  $(INSTALL_TARGETS)
            @ WRITE SYS$OUTPUT "Installation complete"

tidy :
    if f$search("*$(O)") .nes. "" then purge *$(O)
    if f$search("*.lis") .nes. "" then purge *.lis
    if f$search("*$(E)") .nes. "" then purge *$(E)
    if f$search("*.map") .nes. "" then purge *.map
    if f$search("*.opt") .nes. "" then purge *.opt
    if f$search("perlxsi.c") .nes. "" then purge perlxsi.c
    if f$search("configure.mms") .nes. "" then purge configure.mms
    if f$search("$(SERVICE_NAME).COM") .nes. "" then purge $(SERVICE_NAME).COM

clean :
    if f$search("*$(O)") .nes. "" then delete *$(O);*
    if f$search("*.lis") .nes. "" then delete *.lis;*
    if f$search("*$(E)") .nes. "" then delete *$(E);*
    if f$search("*.map") .nes. "" then delete *.map;*
    if f$search("*.opt") .nes. "" then delete *.opt;*
    if f$search("$(SERVICE_NAME).COM") .nes. "" then delete $(SERVICE_NAME).COM;*

realclean :  clean
    if f$search("*.olb") .nes. "" then delete *.olb;*
    if f$search("perlxsi.c") .nes. "" then delete perlxsi.c;*
    if f$search("configure.mms") .nes. "" then delete configure.mms;*
    if f$search("pcbjibdef.h") .nes. "" then delete pcbjibdef.h;*
    if f$search("nfbdef.h") .nes. "" then delete nfbdef.h;*
