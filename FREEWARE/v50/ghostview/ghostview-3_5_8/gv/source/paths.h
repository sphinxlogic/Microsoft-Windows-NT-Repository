/*
**
** paths.h
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

#ifndef _PATHS_H_
#define _PATHS_H_

#include "config.h"
#ifdef VMS
#   define INC_X11(aaa) <X11_DIRECTORY/aaa>
#   define INC_XMU(aaa) <XMU_DIRECTORY/aaa>
#   define INC_XAW(aaa) <XAW_DIRECTORY/aaa>
#else
#   define INC_X11(aaa) <X11/##aaa##>
#   define INC_XMU(aaa) <X11/Xmu/##aaa##>
#   define INC_XAW(aaa) <X11/Xaw3d/##aaa##>
#endif

#endif /* _PATHS_H_ */

