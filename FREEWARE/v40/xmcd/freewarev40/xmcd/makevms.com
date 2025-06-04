$!
$! @(#)Makevms.com	6.10 98/06/22
$!
$! Top-level OpenVMS "Makefile" for xmcd, cda, libdi and cddb
$!
$!	xmcd  - Motif(tm) CD Audio Player
$!	cda   - Command-line CD Audio Player
$!	libdi - CD Audio Player Device Interface Library
$!	cddb  - CD Database Management Library
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
$ home_path = f$directory()
$ CC :== CC/WARNINGS=NOINFORMATIONALS
$ set def [.COMMON_D]
$ @MAKEVMS
$ set def 'home_path'
$ set def [.CDDB_D]
$ @MAKEVMS
$ set def 'home_path'
$ set def [.LIBDI_D]
$ @MAKEVMS
$ set def 'home_path'
$ set def [.XMCD_D]
$ @MAKEVMS
$ xmcd :== "$SYS$LOGIN_DEVICE:''f$directory()'XMCD.EXE"
$ set def 'home_path'
$ DELETE/SYMBOL/GLOBAL CC
$ exit
