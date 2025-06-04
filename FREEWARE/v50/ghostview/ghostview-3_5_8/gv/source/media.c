/*
**
** media.c
**
** Copyright (C) 1997 Johannes Plass
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

/*
#define MESSAGES
*/
#include "message.h"

#include <stdio.h>
#include <math.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_XMU(SysUtil.h)

#include "types.h"
#include "config.h"
#include "types.h"
#include "d_memdebug.h"
#include "options.h"
#include "media.h"

extern char *main_getResource();

/*##################################################
  media_freeMedias
##################################################*/

void media_freeMedias(medias)
  Media *medias;
{
  int i=0;
  BEGINMESSAGE(media_freeMedias)
  while (medias[i]) {
    if (medias[i]->name) GV_XtFree(medias[i]->name);
    GV_XtFree(medias[i]);
    i++;
  }
  if (medias) GV_XtFree(medias);
  ENDMESSAGE(media_freeMedias)
}

/*##################################################
  media_parseMedias
##################################################*/

static Media media_mallocMedia()
{
  Media media;
  media = (Media) GV_XtMalloc(sizeof(MediaStruct));
  memset((void*)media ,0,sizeof(MediaStruct));
  return media;
}

Media *media_parseMedias(s)
  char *s;
{
  char *c,*nl;
  Media *medias,*mmedias,media;
  int i,n,have_media=0,have_used_media=0,used,w,h;
  char name[100];

  BEGINMESSAGE(media_parseMedias)
  if (!s) s = "";
  s =options_squeezeMultiline(s);
  for (n=1,c=s; (c = strchr(c,'\n')); n++, c++);
  INFIMESSAGE(number of medias,n)
  mmedias = medias = (Media*) GV_XtMalloc((n+3)*sizeof(Media));
  media = media_mallocMedia();
  media->name = GV_XtNewString("BBox");
  media->width= 0;
  media->height= 0;
  media->used= 1;
  *medias++ = media;
  c=s;
  if (*s) while (n>0) {
    nl = strchr(c,'\n'); 
    if (nl) *nl='\0';
    name[0]='\0';
    used = 1;
    while (*c && (*c == '#' || *c == '!')) { used=0; c++; }
    i=sscanf(c," %[^,] , %d %d ",name,&w,&h);
    if (i==3 && w>0 && h>0) {
      media = media_mallocMedia();
      media->name = GV_XtNewString(name);
      media->width= w;
      media->height= h;
      media->used= used;
      if (used) have_used_media=1;
      have_media = 1;
      INFSMESSAGE(found media,media->name)
      IIMESSAGE(media->width,media->height)
      *medias++ = media;
    }
    n--;
    c=++nl;
  }
  if (!have_media) {
    media = media_mallocMedia();
    media->name = GV_XtNewString("A4");
    media->width= 595;
    media->height= 842;
    media->used= 1;
    *medias++ = media;
  }
  if (!have_used_media) mmedias[1]->used=1;
  *medias = (Media) NULL;

  GV_XtFree(s);
  ENDMESSAGE(media_parseMedias)
  return(mmedias);
}

/*##################################################
  media_numMedias
##################################################*/

int media_numMedias(medias)
  Media *medias;
{
  int i;
  for (i=1; medias[i]; i++);
  return i;
}


