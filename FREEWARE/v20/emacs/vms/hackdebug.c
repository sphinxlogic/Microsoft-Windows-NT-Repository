/*
 * Copyright © 1994 the Free Software Foundation, Inc.
 *
 * Author: Richard Levitte <levitte@e.kth.se>
 *
 * This file is a part of GNU VMSLIB, the GNU library for porting GNU
 * software to VMS.
 *
 * GNU VMSLIB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU VMSLIB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU VMSLIB; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

static char hackdebug_version[] = "VMS hackdebug version 1.1";

/* Now this is probably one of the dirtiest hacks you've ever seen */

/* For now, this is a VERY ugly hack, with everything hardcoded...
   What more do we need? */

#include <unixio.h>
#include <file.h>

#define FLAGS (O_RDWR|O_EXCL)
#define MODE (0755)

#define BLOCK_SIZE (512)

#ifdef VAX
#define offset (0x20)
#define mask (~0x01)
#define value (0x01)
#else
#define offset (0x50)
#define mask (~0x01)
#define value (0x01)
#endif

static char buffer[BLOCK_SIZE];

int main (argc, argv)
     int argc;
     char *argv[];
{
#define IMAGE argv[1]
  int fd = open (IMAGE, FLAGS, MODE);

  if (fd == -1)
    {
      perror ("Could not open TEMACS.EXE");
      return 4; /* This is fatal */
    }

  read (fd, buffer, BLOCK_SIZE);

  buffer[offset] &= mask;

  lseek (fd, 0, SEEK_SET);

  write (fd, buffer, BLOCK_SIZE);

  close (fd);

  return 1;
}
