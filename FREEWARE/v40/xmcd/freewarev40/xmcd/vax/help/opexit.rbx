#
# @(#)OpExit.rbx	6.6 98/03/13
#
#   xmcd - Motif(tm) CD Audio Player
#
#   Copyright (C) 1993-1998  Ti Kan
#   E-mail: ti@amb.org
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#
On Exit Options Radio Box

There are three toggle buttons in this radio box, only
one of these can be selected at a time:

- none
   No feature is selected with this option.

- auto stop
   With this option, the CD will stop when you quit
   Xmcd.

- auto eject
   With this option, the CD will eject when you quit
   Xmcd.  This option will work only if the CD-ROM
   drive supports software-controlled tray eject.

The default start-up setting of these options is
configured via the stopOnExit and ejectOnExit
parameters in the device-specific configuration
files.
