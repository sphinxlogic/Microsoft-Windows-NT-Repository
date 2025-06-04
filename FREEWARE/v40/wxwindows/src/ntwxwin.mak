#
# File:		ntwxwin.env
# Author:	Ulrich Leodolter
# Created:	Wed May 17 08:36:42 1995
# Updated:
#
#	MSVC++ 32-bit makefile include file
#
!include <ntwin32.mak>

WIN95=1

!if "$(WIN95)" == "0"
# With 3.50, Win95 will use your existing icons to show smaller ones.
# With 4.0, you'll have to follow Win95 procedures for icons or you'll get the
# default Windows icon.
APPVER=3.50
WINVERSION=-DWINVER=0x0350 /DWIN95=$(WIN95) # Generic WIN32
!else
#APPVER=3.50
APPVER=4.0
# This means 'enable Windows 95 features' (in wxWindows and in VC++ 4.0).
WINVERSION=-DWINVER=0x0400 /DWIN95=$(WIN95)
!endif

CPU=i386

# Suffixes
OBJSUFF=obj
SRCSUFF=cpp

WINFLAGS=-c -W3 -Dtry=__try -Dexcept=__except -Dleave=__leave -Dfinally=__finally -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -nologo -D_X86_=1 -DWIN32 -D_WIN32 $(WINVERSION)
WINLINKFLAGS=/NODEFAULTLIB /INCREMENTAL:NO /NOLOGO -align:0x1000 -machine:$(CPU) -subsystem:windows,$(APPVER)
WINLIBS=kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib libc.lib oldnames.lib\
 comctl32.lib ctl3d32.lib # odbc32.lib # libci.lib # libci.lib required for VC++ 4.2 (or libc.lib?)

# Change this to your WXWIN directory
WXDIR=$(WXWIN)

WXSRC=$(WXDIR)\src\msw
WXINC=$(WXDIR)\include\msw
WXBASESRC=$(WXDIR)\src\base
WXBASEINC=$(WXDIR)\include\base

!if "$(WXUSINGDLL)" == "1" || "$(WXMAKINGDLL)" == "1"
WXLIB=$(WXDIR)\lib\wx166.lib
!else
WXLIB=$(WXDIR)\lib\wx.lib
!endif

!if "$(WXUSINGDLL)" == "1"
EXTRADLLFLAGS=/DWXUSINGDLL=1
!endif

!if "$(WXMAKINGDLL)" == "1"
EXTRADLLFLAGS=/DWXMAKINGDLL=1
!endif

!if "$(WXMAKINGDLL)" == "0" && "$(DLL)" == "1"
EXTRADLLFLAGS=
!endif

!if "$(NOMAIN)" == "1"
EXTRADLLFLAGS=$(EXTRADLLFLAGS) /DNOMAIN
!endif

INC=-I$(WXDIR)\include\msw -I$(WXDIR)\include\base -I$(WXDIR)\contrib\fafa $(EXTRAINC)
LIBS = $(EXTRALIBS) $(WXLIB) $(WINLIBS)

!ifndef FINAL
FINAL=0
!endif

!ifndef DLL
DLL=0
!endif

!ifndef DEBUG
DEBUG=1
!endif

# Set this to 1 if you don't want to use precompiled headers
NOPCH=0
MAKEPRECOMP=/YcWX_PREC.H
OPTIONS=

!if "$(FINAL)" == "0"
OPT = /Od /Gy
# ***N.B.*** to save space/time, comment out /FR to avoid browse info (.sbr files) being generated
DEBUG_FLAGS= /Zi # /FR
LINK_DEBUG_FLAGS=-debug:full -debugtype:cv # /PDB:NONE
!else
# /O1 - smallest code
# /O2 - fastest code
OPT = /O1 # /O2 # /Od
DEBUG_FLAGS=
LINK_DEBUG_FLAGS=/RELEASE
!endif

!if "$(DLL)" == "0"

!if "$(NOPCH)" == "1"
PCH=
PRECOMP=
MAKEPRECOMP=
!else
PCH=WX.PCH
PRECOMP=/YuWX_PREC.H /Fp$(WXDIR)\src\msw\$(PCH) /Fd$(WXDIR)\src\msw\wx.pdb
MAKEPRECOMP=/YcWX_PREC.H
!endif

CPPFLAGS=$(WINFLAGS) $(DEBUG_FLAGS) $(PRECOMP) $(EXTRAFLAGS) /Dwx_msw /DDEBUG=$(DEBUG) $(INC) $(OPT) $(EXTRADLLFLAGS)
# If you don't include wx_prec.h, use CPPFLAGS2
CPPFLAGS2=$(WINFLAGS) $(DEBUG_FLAGS) /Dwx_msw /DDEBUG=$(DEBUG) $(INC) $(EXTRAFLAGS) $(OPT) $(EXTRADLLFLAGS)
LINKFLAGS=$(LINK_DEBUG_FLAGS) $(WINLINKFLAGS) -entry:WinMainCRTStartup
DUMMY=dummy

!else

!if "$(MAKINGDLL)" == "1"
PCH=WXDUMMY.PCH
DUMMY=dummydll
!else
PCH=WX.PCH
DUMMY=dummy
!endif

PRECOMP=/YuWX_PREC.H /Fp$(WXDIR)\src\msw\$(PCH) /Fd$(WXDIR)\src\msw\wx.pdb
CPPFLAGS=$(WINFLAGS) $(DEBUG_FLAGS) $(PRECOMP) $(EXTRAFLAGS) /Dwx_msw /DDEBUG=$(DEBUG) $(INC) $(OPT) /D_DLL /MT $(EXTRADLLFLAGS) /D_WINDOWS /D_WINDLL
CPPFLAGS2=$(WINFLAGS) $(DEBUG_FLAGS) /Dwx_msw /DDEBUG=$(DEBUG) $(INC) $(EXTRAFLAGS) $(OPT) /D_DLL /MT $(EXTRADLLFLAGS) /D_WINDOWS /D_WINDLL
LINKFLAGS=$(LINK_DEBUG_FLAGS) -machine:i386 -subsystem:windows,$(APPVER) -dll # -entry:_DllMainCRTStartup$(DLLENTRY)
!endif

DUMMYOBJ=$(WXDIR)\src\msw\$(DUMMY).obj


