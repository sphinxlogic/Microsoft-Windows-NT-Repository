/* 
    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999,2000 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	Macintosh interface for TiMidity
	by T.Nogami	<t-nogami@happy.email.ne.jp>
		
    mac_readdir.c
    Macintosh readdir
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include "mac_readdir.h"

DIR *opendir(const char * /*name*/)
{
	return NULL; //dummy
}

int closedir(DIR * /*dir*/)
{
	return 0; //dummy
}

struct dirent* readdir( DIR * /*p*/)
{
	return NULL; //dummy
}

