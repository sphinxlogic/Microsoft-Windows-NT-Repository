$!
$! @(#)Makevms.com	6.8 98/01/02
$!
$! OpenVMS "Makefile" for libutil (common utility routines
$! for xmcd, cda, libdi and cddb)
$!
$!      xmcd  - Motif(tm) CD Audio Player
$!      cda   - Command-line CD Audio Player
$!      libdi - CD Audio Player Device Interface Library
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
$ set verify
$ common_path = f$directory()
$ define/nolog COMMON_D 'common_path'
$ define/nolog decc$user_include [], decw$include:
$ define/nolog decc$system_include decc$user_include
$ cc util
$ lib/cre libutil.olb *.obj
$ set noverify
