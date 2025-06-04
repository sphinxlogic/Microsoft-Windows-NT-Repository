/*
**
** SwitchP.c
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

#ifndef _SwitchP_h_
#define _SwitchP_h_

#include "paths.h"
#include "Switch.h"
#include "ButtonP.h"

typedef struct _SwitchClass 
{
  int makes_compiler_happy;  /* not used */
} SwitchClassPart;

typedef struct _MenuButtonClassRec {
  CoreClassPart	     core_class;
  SimpleClassPart    simple_class;
  ThreeDClassPart    threeD_class;
  LabelClassPart     label_class;
  CommandClassPart   command_class;
  ButtonClassPart    button_class;
  SwitchClassPart    switch_class;
} SwitchClassRec;

extern SwitchClassRec switchClassRec;

typedef struct {
  int makes_compiler_happy;  /* not used */
} SwitchPart;

typedef struct _SwitchRec {
    CorePart         core;
    SimplePart	     simple;
    ThreeDPart       threeD;
    LabelPart	     label;
    CommandPart	     command;
    ButtonPart	     button;
  /* switch won't work :-) */
    SwitchPart       swidch;
} SwitchRec;

#endif /* _SwitchP_h_ */


