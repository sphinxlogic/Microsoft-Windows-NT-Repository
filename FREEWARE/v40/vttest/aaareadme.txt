NOTE for executables built for OpenVMS Freeware CDROM                1998-10-26
-----------------------------------------------------

This program was compiled using

	DEC C V5.7-004 on OpenVMS Alpha I7.2    

by Thomas Dickey <dickey@clark.net>


To run these programs, you should define a foreign command - a symbol
giving the full pathname of the executable, preceded by a '$'.  For
example, these are the foreign commands which I defined when building
these programs:

  BR*OWSE == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]BROWSE.EXE"
  E*DIT == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]VILE.EXE"
  FL*IST == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]FLIST.EXE"
  LYNX == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]LYNX.EXE"
  VILE == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]VILE.EXE"
  VTTEST == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]VTTEST.EXE"
  XVILE == "$ SYS$SYSDEVICE:[USERS.DICKEY.BIN]XVILE.EXE"

In general, I build most of my foreign commands automatically from the
contents of the [.bin] directory under my login directory.  I enclose
my LOGIN.COM file below, for reference.

The alias for EDIT is used in FLIST so that I run VILE rather than the
standard editor.

These are some useful aliases which I use in the FLIST program, for
changing file protections:

  _N == "set protection=(o:rwed,g:re,w:re)"
  _R == "set protection=(o:r,g:r,w:r)"
  _W == "set protection=(o:rwed)"
  _X == "set protection=(o:re,g:re,w:re)"


-------------------------------------------------------------------------------
-- My LOGIN.COM file
-------------------------------------------------------------------------------
$ VERIFY_STATE = 'F$VERIFY(0)'
$ IF F$MODE() .EQS. "OTHER" THEN $ GOTO EXIT
$ 
$!+
$! defines foreign-commands for testing, automatically from contents of [.bin]
$!-
$	path = F$ENVIRONMENT("procedure")
$	repath :== @ "''path'"
$	head = "''F$EXTRACT(0,F$LOCATE("]",path),path)'"
$	path = "''head'.bin]"
$	define/nolog bin 'path
$
$	name = ""
$ loop_exe:
$	last = name
$	name = F$SEARCH("''path'*.exe;")
$	if name .nes. "" .and. name .nes. last
$	then
$		name = F$EXTRACT(0,F$LOCATE(";",name),name)
$		'F$PARSE(name,,,"NAME","SYNTAX_ONLY") :== "$ ''name'"
$		goto loop_exe
$	endif
$
$	name = ""
$ loop_com:
$	last = name
$	name = F$SEARCH("''path'*.com;")
$	if name .nes. "" .and. name .nes. last
$	then
$		name = F$EXTRACT(0,F$LOCATE(";",name),name)
$		'F$PARSE(name,,,"NAME","SYNTAX_ONLY") :== " @ ''name'"
$		goto loop_com
$	endif
$
$! Define abbreviations
$ br*owse :== 'browse
$ fl*ist  :== 'flist
$
$! Common aliases
$ _n      :== "set protection=(o:rwed,g:re,w:re)"
$ _w      :== "set protection=(o:rwed)"
$ _x      :== "set protection=(o:re,g:re,w:re)"
$ _r      :== "set protection=(o:r,g:r,w:r)"
$ nodel   :== "set protection=(o:rwe,g:re,w:re)"
$ hide    :== "set protection=(g:e,w:e)"
$ _n/default
$ di      :== "dir/size/date"
$ dip     :== "dir/protection"
$
$! Stuff to use in development
$ e*dit   :== 'vile
$ w*ho    :== show users
$ what    :== show system
$ qb      :== show queue/batch
$ tf      :== $ axpguest_device:[tedickey.src.flist.bin]flist.exe
$
$ cd :== set default
$ pwd :== show default
$ home :== set default sys$login
$
$  define/nolog/trans=concealed dickey "''head'.]"
$  define/nolog dickey$exe dickey:[bin]
$  define/nolog lynx_dir dickey:[lynx2-8-1]
$! define/nolog/trans=concealed td_lib_root "''head'.src.td_lib.]"
$! mms :== 'mmk
$
$! FLEX/BISON
$ define/nolog/trans=concealed gnu_bison "''head'.src.bison.]"
$ define/nolog tools$$exe     bin
$ define/nolog tools$$library "''head'.lib]"
$ define/nolog tools$$manual  "''head'.info]"
$
$! Filter out loud noises, etc.
$ if "''f$mode()'" .eqs. "INTERACTIVE"
$ then
$!	set broadcast=noopcom
$	set terminal/line_editing
$ endif
$ 
$ EXIT:
$
$ IF VERIFY_STATE THEN $ SET VERIFY
