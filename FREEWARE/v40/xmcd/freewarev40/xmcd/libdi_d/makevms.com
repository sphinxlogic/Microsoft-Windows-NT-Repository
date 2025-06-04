$!
$! @(#)Makevms.com	6.8 98/01/02
$!
$! OpenVMS "Makefile" for libdi
$!
$!      libdi - CD Audio Player Device Interface Library
$!
$!   Copyright (C) 1993-1998  Ti Kan
$!   E-mail: ti@amb.org
$!
$!   This library is free software; you can redistribute it and/or
$!   modify it under the terms of the GNU Library General Public
$!   License as published by the Free Software Foundation; either
$!   version 2 of the License, or (at your option) any later version.
$!
$!   This library is distributed in the hope that it will be useful,
$!   but WITHOUT ANY WARRANTY; without even the implied warranty of
$!   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
$!   Library General Public License for more details.
$!
$!   You should have received a copy of the GNU Library General Public
$!   License along with this library; if not, write to the Free
$!   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
$!
$ set verify
$ define/nolog LIBDI_D 'f$directory()'
$ cc libdi.c
$ cc scsipt.c
$ cc os_vms.c
$ cc vu_chin.c
$ cc vu_hita.c
$ cc vu_nec.c
$ cc vu_pana.c
$ cc vu_pion.c
$ cc vu_sony.c
$ cc vu_tosh.c
$ lib/cre libdi.olb *.obj
$ set noverify
