/*
**
** scale.c
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
#include "resource.h"
#include "scale.h"

#ifdef VMS
#   include "strcasecmp.h"
#else
#   include <string.h>
#endif

/*##################################################
  scale_freeScales
##################################################*/

void scale_freeScales(scales)
  Scale *scales;
{
  int i=0;
  BEGINMESSAGE(scale_freeScales)
  while (scales[i]) {
    if (scales[i]->name) GV_XtFree(scales[i]->name);
    GV_XtFree(scales[i]);
    i++;
  }
  if (scales) GV_XtFree(scales);
  ENDMESSAGE(scale_freeScales)
}

/*##################################################
  scale_parseScales
##################################################*/

static Scale scale_mallocScale()
{
  Scale scale;
  scale = (Scale) GV_XtMalloc(sizeof(ScaleStruct));
  memset((void*)scale ,0,sizeof(ScaleStruct));
  return scale;
}

Scale *scale_parseScales(s)
  char *s;
{
  char *c,*nl;
  Scale *scales,*mscales;
  Scale scale;
  int i,n,have_base=0,have_center=0;
  char name[50],kind[50];
  float f;

  BEGINMESSAGE(scale_parseScales)
  if (!s) s = "";
  s =options_squeezeMultiline(s);
  for (n=1,c=s; (c = strchr(c,'\n')); n++, c++);
  INFIMESSAGE(number of scales,n)
  mscales = scales = (Scale*) GV_XtMalloc((n+3)*sizeof(Scale));
  c=s;
  if (*s) while (n>0) {
    nl = strchr(c,'\n'); 
    if (nl) *nl='\0';
    name[0]=kind[0]='\0';
    f=0.0;
    if (*c=='#' || *c=='!') i=0; 
    else i=sscanf(c," %[^,] , %f , %[^,] ",name,&f,kind);
    if (i>=2 && f>0) {
      scale = scale_mallocScale();
      scale->name = GV_XtNewString(name);
      scale->scale = sqrt(f);
      if (i==3) {
        if (!strcasecmp(kind,"screen")) { scale->is_base = SCALE_IS_REAL_BASED; have_base=1;}
        else if (!strcasecmp(kind,"pixel")) { scale->is_base = SCALE_IS_PIXEL_BASED; have_base=1;}
      }
      if (!scale->is_base && f==1.0) { scale->is_center=1; have_center=1; }
      INFSMESSAGE(found scale,scale->name);
      FMESSAGE(scale->scale)
      IMESSAGE(scale->is_base)
      *scales++ = scale;
    }
    n--;
    c=++nl;
  }
  if (!have_center) {
    scale = scale_mallocScale();
    scale->name = GV_XtNewString("1.000");
    scale->scale = 1.0;
    scale->is_center=1;
    *scales++ = scale;
  }
  if (!have_base) {
    scale = scale_mallocScale();
    scale->name = GV_XtNewString("Natural size");
    scale->scale = 1.0;
    scale->is_base=SCALE_IS_REAL_BASED;
    *scales++ = scale;
  }
  *scales = (Scale) NULL;

  /* sort the scales: first the scale bases, then the relative ones. */
  /* Also sort relative scales according to their scale value. */
  scales=mscales;
  n=0;
  while (scales[n]) {
    if (n>0 &&
       ((scales[n]->is_base && !(scales[n-1]->is_base)) ||
        (!(scales[n]->is_base) && !(scales[n-1]->is_base) &&
	 scales[n-1]->scale > scales[n]->scale))) {
      scale=scales[n];
      scales[n]=scales[n-1];
      scales[n-1]=scale;
      n=0;
    }
    else n++;
  }
  GV_XtFree(s);
  ENDMESSAGE(scale_parseScales)
  return(mscales);
}

/*##################################################################*/
/* scale_numOfScale */
/*##################################################################*/

int scale_checkScaleNum(scales,s)
  Scale *scales;
  int s;
{
  int j,n=-1;
  int m;

  m = s & (~SCALE_VAL);
  s = s & SCALE_VAL; if (m&SCALE_MIN) s = -s;
  BEGINMESSAGE(scale_checkScaleNum)
  if (m&SCALE_REL) {
    for (j=0; scales[j] && !scales[j]->is_center; j++);
    if (scales[j]) {
      n = j;
      j += s; if (j<0) j = 0;
      while (n>j) if (scales[n-1]->is_base) break; else n--;
      while (n<j) if (!scales[n+1]) break; else n++;
    }
  } else {
    n = s;
    for (j=0; scales[j] && (n != j); j++); if (!scales[j]) n=-1;
  }
  if (n>=0) {
    if (m&SCALE_BAS) { if (!(scales[n]->is_base)) n=-1; }
    else if (scales[n]->is_base) n = -1;
  }
  if (n>=0) {
    if (m&SCALE_BAS) n = n|SCALE_BAS;
    else             n = n|SCALE_ABS;
  }
  ENDMESSAGE(scale_checkScaleNum)
  return n;
}

/*##################################################################*/
/* scale_getScreenSize */
/*##################################################################*/

void scale_getScreenSize(display,screen,db,app_class,app_name,widthP,heightP)
  Display *display;
  Screen *screen;
  XrmDatabase db;
  char *app_class;
  char *app_name;
  int *widthP;
  int *heightP;
{
  int hmm=0,wmm=0;
  char v[255];
  char *s;
  XrmDatabase sdb=NULL;

  BEGINMESSAGE(scale_getScreenSize)
  s=XScreenResourceString(screen);
  if (s) {
    sdb=XrmGetStringDatabase(s);
    s=resource_getResource(sdb,app_class,app_name,"screenSize",NULL);
    INFSMESSAGE(found in SCREEN_RESOURCES,s)
  }
#ifndef VMS
  if (!s) {
    scale_getScreenResourceName(display,v);
    s=resource_getResource(db,app_class,app_name,v,NULL);
    INFSMESSAGE(found in screen specific resource,s)
  }
#endif
  if (!s) {
    s=resource_getResource(db,app_class,app_name,"screenSize",NULL);
    INFSMESSAGE(found,s)
  }
  if (s) strcpy(v,s); else v[0]='\0';
  if (v[0]) sscanf(v," %d x %d ",&wmm,&hmm);
  if (hmm > 0 && wmm > 0) {
     *widthP=wmm;
     *heightP=hmm;
  } else {
    *widthP=WidthMMOfScreen(screen);
    *heightP=HeightMMOfScreen(screen);
  }
  if (sdb) XrmDestroyDatabase(sdb);
  IIMESSAGE(*widthP,*heightP)
  ENDMESSAGE(scale_getScreenSize)
}

#ifndef VMS

/*##################################################
  scale_getScreenResourceName  
##################################################*/

void scale_getScreenResourceName(display,name)
  Display *display;
  char *name;
{
  char server[255],*s;
  int disp=0,scr=0;

  BEGINMESSAGE(scale_getScreenResourceName)
  server[0]='\0';
  s=XDisplayName(XDisplayString(display));
  if (s) {
    strcpy(server,s);
    s=strrchr(server,':');
    if (s) {
      *s++ = '\0';
      if (*s == ':') s++;
      if (*s=='.') sscanf(++s, "%d", &scr);
      else         sscanf(s, "%d.%d", &disp,&scr);
    }
  }
  if (!*server || !strcmp(server, "unix") || !strcmp(server, "localhost"))
    XmuGetHostname(server, 255);
  s=strrchr(server,'.');
  if (s) *s='\0';
  SMESSAGE(server)
  if (server[0]) sprintf(name, "screenSize_%s_%d_%d",server,disp,scr);
  else *name='\0';
  SMESSAGE(name)
  ENDMESSAGE(scale_getScreenResourceName)
}
#endif /* ifndef VMS */
