$!
$! @(#)Makevms.com	6.9 98/01/02
$!
$! OpenVMS "Makefile" for xmcd
$!
$!      xmcd - Motif(tm) CD Audio Player
$!
$!   Copyright (C) 1993-1998  Ti Kan
$!   E-mail: ti@amb.org
$!
$!   This program is free software; you can redistribute it and/or modify
$!   it under the terms of the GNU General Public License as published by
$!   the Free Software Foundation; either version 2 of the License, or
$!   (at your option) any later version.
$!
$!   This program is distributed in the hope that it will be useful,
$!   but WITHOUT ANY WARRANTY; without even the implied warranty of
$!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
$!   GNU General Public License for more details.
$!
$!   You should have received a copy of the GNU General Public License
$!   along with this program; if not, write to the Free Software
$!   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
$!
$ set verify
$ ! *** Compile Phase ***
$ define/nolog XMCD_D 'f$directory()'
$ cc cdfunc.c
$ cc dbprog.c
$ cc geom.c
$ cc help.c
$ cc hotkey.c
$ cc main.c
$ cc widget.c
$ lib/cre xmcd.olb *.obj
$ ! *** Link Phase ***
$ define/nolog  LNK$LIBRARY  SYS$SHARE:DECCRTL
$ link/exe=xmcd main.obj, xmcd.olb/lib,-
				LIBDI_D:LIBDI.olb/lib,-
				CDDB_D:LIBCDDB.olb/lib,-
				COMMON_D:LIBUTIL.olb/lib,-
				sys$input:/opt
sys$library:decc$shr.exe/share
sys$share:decw$xlibshr.exe/share
sys$share:decw$xtlibshrr5.exe/share
sys$share:decw$xmulibshrr5.exe/share
sys$share:decw$xmlibshr12.exe/share
sys$share:DECW$BKRSHR12.EXE/share
sys$share:DECW$DXMLIBSHR12.EXE/share
sys$share:DECW$MRMLIBSHR12.EXE/share
sys$share:ipc$share.exe/share
$ set noverify
$ ! 
$ ! create a foreign command for XMCD
$ !
$ xmcd :== $sys$login_device:'f$directory()'xmcd.exe
$ !
$ ! delete the logical names we set up
$ !
$ deass LNK$LIBRARY
$ deass decc$user_include
$ deass decc$system_include
$ deass COMMON_D
$ deass LIBDI_D
$ deass CDDB_D
$ deass XMCD_D
