@echo Windows/Dos batch makefile for MingW32 and lynx.exe
@echo Remember to precede this by "command /E:8192" and to set the
@echo MingW32 C_INCLUDE_PATH and %C_INCLUDE_PATH%..\..\bin paths
@echo
@echo Usage: makelynx [all|src|link]
@echo Default option: all
@echo Specifying "src" causes the libwww code to be skipped.
@echo Specifying "link" causes the batch file to skip to the final
@echo linking phase.
@echo
PAUSE
@echo Note that you have to edit i386-mingw32\include\stdlib.h to put
@echo an "#ifndef WIN_EX" around the declaration for `sleep', or the
@echo compile won't work.  There is also an "#ifndef PDCURSES" around
@echo the declaration for `beep' for the same reason.  To change the
@echo console library from libslang to libpdcurses, uncomment the
@echo `SET LIBRARY' line below.

PAUSE

REM SET LIBRARY=PDCURSES

set CC=gcc

echo /* Generated lynx_cfg.h file in the lynx directory: */ > lynx_cfg.h
echo. >> lynx_cfg.h
echo #define _WIN_CC		 1 >> lynx_cfg.h
echo #define USE_ALT_BLAT_MAILER 1 >> lynx_cfg.h
echo #define USE_BLAT_MAILER	 1 >> lynx_cfg.h
echo #define ANSI_VARARGS	 1 >> lynx_cfg.h
echo #define HAVE_GETCWD	 1 >> lynx_cfg.h
echo #define LY_MAXPATH       1024 >> lynx_cfg.h
rem echo #define USE_SCROLLBAR	 1 >> lynx_cfg.h

SET DEFINES=-DCJK_EX
SET DEFINES=%DEFINES% -DHAVE_CONFIG_H
SET DEFINES=%DEFINES% -DNO_CONFIG_INFO
SET DEFINES=%DEFINES% -DSH_EX
SET DEFINES=%DEFINES% -DWIN_EX
SET DEFINES=%DEFINES% -D_WINDOWS
SET DEFINES=%DEFINES% -DUSE_EXTERNALS
SET DEFINES=%DEFINES% -DEXP_JUSTIFY_ELTS
SET DEFINES=%DEFINES% -DDIRED_SUPPORT
SET DEFINES=%DEFINES% -DDOSPATH
SET DEFINES=%DEFINES% -DEXP_ALT_BINDINGS
SET DEFINES=%DEFINES% -DEXP_PERSISTENT_COOKIES
SET DEFINES=%DEFINES% -DHAVE_DIRENT_H
SET DEFINES=%DEFINES% -DHAVE_KEYPAD
SET DEFINES=%DEFINES% -DXMOSAIC_HACK
SET DEFINES=%DEFINES% -DACCESS_AUTH
SET DEFINES=%DEFINES% -DNO_FILIO_H
SET DEFINES=%DEFINES% -DNO_UNISTD_H
SET DEFINES=%DEFINES% -DNO_UTMP
SET DEFINES=%DEFINES% -DNO_CUSERID
SET DEFINES=%DEFINES% -DNO_TTYTYPE
SET DEFINES=%DEFINES% -DNOSIGHUP
SET DEFINES=%DEFINES% -DNOUSERS
SET DEFINES=%DEFINES% -DLONG_LIST
SET DEFINES=%DEFINES% -DDISP_PARTIAL
SET DEFINES=%DEFINES% -DSOURCE_CACHE
SET DEFINES=%DEFINES% -DUSE_PRETTYSRC
SET DEFINES=%DEFINES% -DVC="2.14FM"
if "%LIBRARY%" == "PDCURSES" goto else1
SET DEFINES=%DEFINES% -DUSE_SLANG
goto endif1
:else1
SET DEFINES=%DEFINES% -DFANCY_CURSES
SET DEFINES=%DEFINES% -DCOLOR_CURSES
SET DEFINES=%DEFINES% -DPDCURSES
SET DEFINES=%DEFINES% -DUSE_MULTIBYTE_CURSES
:endif1

if not "%1" == "src" goto else
	cd src
	goto src
:else
if not "%1" == "link" goto endif
	cd src
	goto link
:endif

SET INCLUDES=-I. -I..\..\.. -I..\..\..\src

REM Your compiler may not support -mpentium.
REM In that case, replace -mpentium with -m486 or nothing.

set CFLAGS=-O3 -mpentium %INCLUDES% %DEFINES%
set COMPILE_CMD=%CC% -c %CFLAGS%

cd WWW\Library\Implementation
erase *.o

%COMPILE_CMD% HTAABrow.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTAAProt.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTAAUtil.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTAccess.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTAnchor.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTAssoc.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTAtom.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTBTree.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTChunk.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTDOS.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTFile.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTFinger.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTFormat.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTFTP.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTFWriter.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTGopher.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTGroup.c
if errorlevel 1 PAUSE

%COMPILE_CMD% HTLex.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTList.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTMIME.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTMLDTD.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTMLGen.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTNews.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTParse.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTPlain.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTRules.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTString.c
if errorlevel 1 PAUSE

%COMPILE_CMD% HTStyle.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTTCP.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTTelnet.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTTP.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTUU.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTWSRC.c
if errorlevel 1 PAUSE
%COMPILE_CMD% SGML.c
if errorlevel 1 PAUSE

ar crv libwww.a *.o

if errorlevel 1 PAUSE

cd ..\..\..\src\chrtrans
erase *.o

SET INCLUDES=-I. -I.. -I..\.. -I..\..\WWW\Library\Implementation
SET CFLAGS=-O3 -mpentium %INCLUDES% %DEFINES%
SET COMPILE_CMD=%CC% -c %CFLAGS%

%COMPILE_CMD% makeuctb.c
if errorlevel 1 PAUSE
%CC% -o makeuctb.exe makeuctb.o
if errorlevel 1 PAUSE

call makew32.bat
if errorlevel 1 PAUSE
cd ..\

:src
SET INCLUDES=-I. -I.. -I.\chrtrans -I..\WWW\Library\Implementation
SET CFLAGS=-O3 -mpentium %INCLUDES% %DEFINES%
SET COMPILE_CMD=%CC% -c %CFLAGS%
SET PATH=..\WWW\Library\Implementation;%PATH%
erase *.o

%COMPILE_CMD% DefaultStyle.c
if errorlevel 1 PAUSE
%COMPILE_CMD% GridText.c
if errorlevel 1 PAUSE
if not exist TRSTable.c goto notrstable
%COMPILE_CMD% TRSTable.c
if errorlevel 1 PAUSE
:notrstable
if not exist Xsystem.c goto noxsystem
%COMPILE_CMD% Xsystem.c
if errorlevel 1 PAUSE
:noxsystem
%COMPILE_CMD% HTAlert.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTFWriter.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTInit.c
if errorlevel 1 PAUSE
%COMPILE_CMD% HTML.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYBookmark.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYCgi.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYCharSets.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYCharUtils.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYClean.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYCookie.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYCurses.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYDownload.c
if errorlevel 1 PAUSE

%COMPILE_CMD% LYEdit.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYEditmap.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYexit.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYExtern.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYForms.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYGetFile.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYHash.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYHistory.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYJump.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYKeymap.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYLeaks.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYList.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYLocal.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYMail.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYMain.c
if errorlevel 1 PAUSE
%COMPILE_CMD%  LYMainLoop.c
if errorlevel 1 PAUSE

%COMPILE_CMD% LYMap.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYNews.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYOptions.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYPrettySrc.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYPrint.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYrcFile.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYReadCFG.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYSearch.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYShowInfo.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYStrings.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYStyle.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYTraversal.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYUpload.c
if errorlevel 1 PAUSE
%COMPILE_CMD% LYUtils.c
if errorlevel 1 PAUSE
%COMPILE_CMD% UCAuto.c
if errorlevel 1 PAUSE
%COMPILE_CMD% UCAux.c
if errorlevel 1 PAUSE
%COMPILE_CMD% UCdomap.c
if errorlevel 1 PAUSE

:link
if not "%LIBRARY%" == "PDCURSES" goto else2
SET LIBS=-L..\WWW\Library\Implementation -lwww -lpdcurses -lpanel -lwsock32 -luser32
goto endif2
:else2
SET LIBS=-L..\WWW\Library\Implementation -lwww -lslang -lwsock32 -luser32
:endif2

%CC% -s -o lynx *.o %LIBS%
if exist lynx.exe ECHO "Welcome to lynx!"
