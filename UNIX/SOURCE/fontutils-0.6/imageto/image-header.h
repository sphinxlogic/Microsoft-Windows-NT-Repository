/* image-header.h: declarations for a generic image header.

Copyright (C) 1992 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef IMAGE_HEADER_H
#define IMAGE_HEADER_H

#include "types.h"

/* The important general information about the image data.  
   See `get_{img,pbm}_header' for the full details of the headers for
   the particular formats.  */
typedef struct
{
  two_bytes hres, vres;		/* In pixels per inch.  */
  two_bytes width, height;	/* In bits.  */
  two_bytes depth;		/* Perhaps the depth?  */
  unsigned format;		/* (for pbm) Whether packed or not.  */
} image_header_type;

#endif /* not IMAGE_HEADER_H */
