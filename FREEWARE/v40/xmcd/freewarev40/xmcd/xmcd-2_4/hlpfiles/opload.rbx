#
# @(#)OpLoad.rbx	6.5 98/01/02
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
On Load Options Radio Box

There are three toggle buttons in this radio box, only
one of these can be selected at a time:

- none
   No feature is selected with this option.

- spin down
   With this option, the CD will spin down after being
   loaded.  Starting audio playback will automatically
   spin up the CD again.  This is intended to reduce wear
   and tear on the CD-ROM laser and motors, since most
   CD-ROM drives remain spinning after CD insertion.

- auto play
   With this option, the CD will automatically start
   audio playback after being loaded.  If the shuffle
   mode is enabled, the playback will begin a new
   shuffle sequence.  If a program is defined, the
   program sequence is played.

The default start-up setting of these options is
configured via the spinDownOnLoad and playOnLoad
parameters in the device-specific configuration
files.
