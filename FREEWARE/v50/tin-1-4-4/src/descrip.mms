# Makefile for tin - for tin compiler flag options read INSTALL and README.
#
VERSION	= 1.4.4

.IFDEF DEF_NNTP
.ELSE
# Edit the following line for your default nntp server.
# Remark:
# The environment variable NNTPSERVER will overwrite this option.
DEF_NNTP = NNTP_DEFAULT_SERVER="""news.rrzn.uni-hannover.de"""
.ENDIF

#
# The only tcp option currently supported is SOCKETSHR_TCP
#
# SOCKETSHR_STATIC is a development aid so we can link statically against a
# copy of socketshr in the "bin:" directory.

.IFDEF TCP
.ELSE
#TCP = UCX
TCP = SOCKETSHR_TCP
.IFDEF SOCKETSHR_STATIC
SOCKETSHR_LIBS = bin:socketlib/lib
SOCKETSHR_INCS = ,bin:
.ELSE
SOCKETSHR_LIBS = socketshr/share
SOCKETSHR_INCS =
.ENDIF
.ENDIF

.IFDEF ALPHA
OS_STR  = OpenVMS/ALPHA
OS_TYP	= ALPHA
.ENDIF

.IFDEF VAX
OS_STR  = OpenVMS/VAX
OS_TYP	= VAX
.ENDIF

.IFDEF OS_STR
.ELSE
.FIRST :
        @ WRITE SYS$OUTPUT "   Please start MMS (or MMK) at least with following switches:"
        @ WRITE SYS$OUTPUT ""
        @ WRITE SYS$OUTPUT "   on OpenVMS/Alpha:"
        @ WRITE SYS$OUTPUT "   $MMS /MACRO=(alpha=1)"
        @ WRITE SYS$OUTPUT ""
        @ WRITE SYS$OUTPUT "   on OpenVMS/Vax:"
        @ WRITE SYS$OUTPUT "   $MMS /MACRO=(vax=1)"
        @ WRITE SYS$OUTPUT ""
        @ WRITE SYS$OUTPUT "   other examples:"
        @ WRITE SYS$OUTPUT "   $MMS /MACRO=(vax=1,gnuc=1,tcp=socketshr_tcp)"
        @ WRITE SYS$OUTPUT "   $MMS /MACRO=(vax=1,debug=1)"
        @ WRITE SYS$OUTPUT "   $MMS /MACRO=(vax=1,debugger=1)"
        @ return 44  ! %SYSTEM-F-ABORT, abort

.ENDIF


DEF_OS   = OS="""$(OS_STR)"""

# Define "DEBUG" to compile-in runtime traces with the -"D" option.
.IFDEF DEBUG
DEF_TEST = ,DEBUG
.ELSE
DEF_TEST =
.ENDIF

DEFINES  = HAVE_ISPELL,DONT_HAVE_NNTP_EXTS,VMS,$(TCP),$(DEF_NNTP),$(DEF_OS)$(DEF_TEST)
INCLUDES = [-.include],[-.vms],[-.PCRE]$(SOCKETSHR_INCS)

# Comment/uncomment one of these to get no listings, or long listings.
# DEC's runtime environment reports errors based on the line numbers in these
# listings, so they're useful for analyzing fatal errors.
LIST_OPT = /list/show=expans
#LIST_OPT = /list/show=all

.IFDEF VAXC
CC = cc
CDEFS = /include=($(INCLUDES))/define=($(DEFINES))/nowarning$(LIST_OPT)
GLOMOD = $(TINLIB)/LIBRARY,
COMP = VAXC
.ELSE
.IFDEF  GNUC
CC	= gcc
CDEFS = /include=($(INCLUDES))/define=($(DEFINES))
GLOMOD = $(TINLIB)/LIBRARY,
COMP = GNUC
.ELSE
CC = cc/decc
CDEFS = /include=($(INCLUDES))/define=($(DEFINES))/nowarning$(LIST_OPT)
COMP = DECC
.ENDIF
.ENDIF

# Define "DEBUGGER" to make an executable that you can use the debug program
# to analyze problems.
.IFDEF DEBUGGER
DBG	= /debug /nooptim
LFLAGS	= /debug/map
TARGET	= tin_debug$(EXE)
PCRELIB	= LIBPCREDBG_$(COMP)_$(OS_TYP).OLB
TINLIB	= TINDBG.OLB
VMSLIB	= LIBVMSDBG_$(COMP)_$(OS_TYP).OLB
.ELSE # !DEBUGGER
TARGET	= tin$(EXE)
PCRELIB	= LIBPCRE_$(COMP)_$(OS_TYP).OLB
TINLIB	= TIN.OLB
VMSLIB	= LIBVMS_$(COMP)_$(OS_TYP).OLB
.IFDEF VAXC
DBG = /optim
.ELSE
.IFDEF GNUC
DBG = /optim=2
.ELSE
DBG = /optim
#DBG = /nooptim
.ENDIF
.ENDIF

.IFDEF VAX
GLOMOD = $(TINLIB)/LIBRARY,
.ELSE
GLOMOD =
.ENDIF

LFLAGS = /map
.ENDIF # !DEBUGGER

.IFDEF VAXC
OPTS    =
.ELSE
.IFDEF GNUC
OPTS    =
.ELSE
#OPTS    = /warning=(disable=implicitfunc)/prefix=all
OPTS    = /prefix=(all,except=ENDWIN)
#OPTS    = /prefix=all
.ENDIF
.ENDIF

CFLAGS = $(DBG) $(OPTS) $(CDEFS)

INCDIR  = [-.include]
LIBS	= [-.vms]$(VMSLIB)/libr,[-.pcre]$(PCRELIB)/libr

.IFDEF BISON
YACC = bison/fixed
YACCOUTPUT = y_tab.c
.ELSE
.IFDEF POSIX
YACC = POSIX/Run/Path=POSIX "/bin/yacc"
YACCOUTPUT = ytab.c
.ELSE
YACC = yacc
YACCOUTPUT = ytab.c
.ENDIF
.ENDIF

MODULES = active, art, attrib, -
          charset, config, curses, debug, -
	  envarg, feed, filter, getline, group, hashstr, -
          help, inews, init, lang, list, mail, main, -
          memory, misc, newsrc, nntplib, -
          open, page, pgp, post, prompt, -
          save, screen, search, select, sigfile, signal, -
          strftime, thread, wildmat, -
          xref, -
          rfc1522, rfc1521, string, header, -
          nrctbl, read, refs, regex, auth

CFILES = $(MODULES:=.c)
OFILES = $(CFILES:.c=.obj)
#HFILES  = $(INCDIR)config.h, $(INCDIR)tin.h, $(INCDIR)extern.h, $(INCDIR)nntplib.h, \
#          $(INCDIR)proto.h, $(INCDIR)stpwatch.h, \
#          $(INCDIR)os_2.h, $(INCDIR)win32.h
HFILES   = $(INCDIR)tin.h
TNNTP	 = $(INCDIR)tnntp.h

all : opt pcrelib vmslib  tincfg.h tin
    continue

#
# The MMS target opt is only for creating the right linker options file.
# With GNUC and DECC on both VAX and ALPHA and different TCP stacks the
# ammount of different option files with mostly identical lines would be
# very large.
#
opt :
	@ write sys$output "Creating linker options file on $(OS_STR) for $(TCP) with $(COMP)"
	@ open/write optfile [-.vms]tin.opt
        @ write optfile "!Options file for tin $(VERSION) on $(OS_STR) for $(TCP) with $(COMP)"
	@ if ("$(TCP)" .eqs. "SOCKETSHR_TCP") then write optfile "!Using Socketshr TCP/IP"
	@ if ("$(TCP)" .eqs. "SOCKETSHR_TCP") then write optfile "$(SOCKETSHR_LIBS)"
.ifdef VAXC
        @ write optfile "!VAXC options"
        @ write optfile "sys$share:vaxcrtl/share"
.else
.ifdef GNUC
        @ write optfile "!GNUC options"
.ifdef VAX
        @ write optfile "gnu_cc:[000000]gcclib/libr"
.else
        @ write optfile "gnu_cc_library:libgcc/libr"
.endif
        @ write optfile "sys$share:vaxcrtl/share"
.else
	@ write optfile "!DECC options"
.endif
.endif
        @ write optfile "sys$library:decw$dwtlibshr/share"
        @ write optfile "sys$library:decw$xlibshr/share"
        @ write optfile "sys$library:vaxccurse/libr"
.ifdef GNUC
.ifdef ALPHA
        @ write optfile "gnu_cc_library:crt0.obj"
.endif
.endif
	@ close optfile


tin : $(TINLIB)($(MODULES))
    link $(LFLAGS) /exec=$(TARGET) $(TINLIB)/LIBRARY/INCL=MAIN,$(GLOMOD) -
        [-.vms]tin.opt/option, $(LIBS)

vmslib :
    set def [-.vms]
    @ if f$getsyi("VERSION") .lts. "V6.2" then -
	modules = "vms,vmsdir,vmspwd,vmsfile,parse,parsdate,getopt,getopt1,getpass,qsort,isxterm"
    @ if f$getsyi("VERSION") .ges. "V6.2" then -
        modules = "vms,vmsdir,vmspwd,vmsfile,parse,parsdate,getpass,qsort,isxterm"
    mms /description=libvms.mms $(MMSQUALIFIERS) $(MMSTARGETS)
    set def [-.src]

pcrelib :
    set def [-.pcre]
    mms /description=pcre.mms $(MMSQUALIFIERS) $(MMSTARGETS)
    set def [-.src]

tincfg.h :	makecfg.exe
	CURRENT_OUTPUT = f$trnlnm("SYS$OUTPUT")
	PROG_NAME = f$search("makecfg.exe")
	DEFINE SYS$OUTPUT tincfg.h
	mc 'PROG_NAME' TINCFG.TBL
	DEASS SYS$OUTPUT
	DEFINE SYS$OUTPUT 'CURRENT_OUTPUT'           !nun wieder aufs Terminal

makecfg.exe : makecfg.obj
    link $(LFLAGS) /exec=makecfg.exe makecfg.obj,[-.vms]tin.opt/option

active.obj :    active.c, $(HFILES)
actived.obj :   actived.c, $(HFILES)
art.obj :       art.c, $(HFILES), $(INCDIR)stpwatch.h
attrib.obj :    attrib.c, $(HFILES)
charset.obj :   charset.c, $(HFILES)
config.obj :    config.c, $(HFILES)
curses.obj :    curses.c, $(HFILES)
tcurses.obj :    tcurses.c, $(HFILES)
debug.obj :     debug.c, $(HFILES)
envarg.obj :    envarg.c, $(HFILES)
feed.obj :      feed.c, $(HFILES)
filter.obj :    filter.c, $(HFILES)
getline.obj :   getline.c, $(HFILES)
group.obj :     group.c, $(HFILES)
hashstr.obj :   hashstr.c, $(HFILES)
help.obj :      help.c, $(HFILES)
inews.obj :     inews.c, $(TNNTP),$(HFILES)
init.obj :      init.c, $(TNNTP), $(HFILES)
lang.obj :      lang.c, $(HFILES)
list.obj :      list.c, $(HFILES)
mail.obj :      mail.c, $(HFILES)
main.obj :      main.c, $(HFILES)
makecfg.obj :   makecfg.c, $(HFILES)
memory.obj :    memory.c, $(HFILES)
misc.obj :      misc.c, $(HFILES)
msmail.obj :    msmail.c, $(HFILES)
newsrc.obj :    newsrc.c, $(HFILES)
nntplib.obj :   nntplib.c, $(TNNTP), $(HFILES)
nntpw32.obj :   nntpw32.c, $(HFILES)
open.obj :      open.c, $(HFILES)
os_2.obj :      os_2.c, $(HFILES), $(INCDIR)os_2.h
page.obj :      page.c, $(HFILES)
parsdate.obj :  parsdate.c, $(HFILES)
post.obj :      post.c, $(HFILES)
prompt.obj :    prompt.c, $(HFILES)
save.obj :      save.c, $(HFILES)
screen.obj :    screen.c, $(HFILES)
search.obj :    search.c, $(HFILES)
select.obj :    select.c, $(HFILES)
sigfile.obj :   sigfile.c, $(HFILES)
signal.obj :    signal.c, $(HFILES)
strftime.obj :  strftime.c, $(HFILES)
thread.obj :    thread.c, $(HFILES)
wildmat.obj :   wildmat.c, $(HFILES)
win32.obj :     win32.c, $(HFILES)
xref.obj :      xref.c, $(HFILES)
vms.obj :       vms.c, $(HFILES)
rfc1522.obj :       rfc1522.c, $(HFILES)
rfc1521.obj :       rfc1521.c, $(HFILES)
string.obj :        string.c, $(HFILES)
header.obj :        header.c, $(TNNTP), $(HFILES)
nrctbl.obj :        nrctbl.c, $(HFILES)
read.obj :        read.c, $(HFILES)
refs.obj :        refs.c, $(HFILES)
regex.obj :        regex.c, $(HFILES)
auth.obj :        auth.c, $(HFILES)

parsdate.c : parsdate.y
    $(YACC) $?
    rename/log $(YACCOUTPUT) $@

clean : vmslib pcrelib
    @- WRITE SYS$OUTPUT "Cleaning tin directory"
    @- if f$search("*.obj;*") .nes. "" then Delete/Log *.obj;*
    @- if f$search("*.lis;*") .nes. "" then Delete/Log *.lis;*
    @- if f$search("makecfg.exe;*") .nes. "" then Delete/Log makecfg.exe;*
    @- if f$search("tincfg.h;*") .nes. "" then Delete/Log tincfg.h;*
    @- if f$search("tin.*;*") .nes. "" then Delete/Log tin.*;*
    @- !Delete/Log parsdate.c;*



.IFDEF VAXC

.IFDEF MULTINET
.FIRST :
    Define SYS 		Multinet_Common_Root:[Multinet.Include.Sys],Sys$Library
    Define VAXC$INCLUDE	Multinet_Common_Root:[Multinet.Include.Sys],Sys$Library
.ELSE
.FIRST :
    Define SYS 		Sys$Library
    Define VAXC$INCLUDE	Sys$Library
.ENDIF

.LAST :
    deassign sys
    deassing vaxc$include

.ELSE
.IFDEF GNUC

.FIRST :
   define sys sys$share

.LAST :
    deassign sys

.ENDIF
.ENDIF

