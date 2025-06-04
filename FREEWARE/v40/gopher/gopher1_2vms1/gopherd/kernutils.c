/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/21 20:46:12 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/kernutils.c,v $
 * $State: Exp $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: kernutils.c
 * Routines to implement kernel specific stuff
 *********************************************************************
 * Revision History:
 * $Log: kernutils.c,v $
 * Revision 1.1  1992/12/21  20:46:12  lindner
 * Initial revision
 *
 *
 *********************************************************************/


#include "gopherd.h"

#define LOADSYMBOL "_avenrun"    /* should work with most Unix flavors */

#define WHICHLOAD  2             /* 0 ==  1 min average           */
                                 /* 1 ==  5 min average           */
                                 /* 2 == 15 min average           */

#ifndef LOADRESTRICT

int LoadTooHigh() 
{
  return(0);
}

#else /* LOADRESTRICT */

int LoadTooHigh()
{
  int status;

  status = getload();
  if(DEBUG)
    printf("getload returns %d\n",status);
  return(status);

}
#ifdef VMS

extern int vaxc$errno_stv;
double sysload;

int getload()
{
    /* In VMS with Multinet, we'll assign a channel to $$VMS_LOAD_AVERAGE,
	and read in 36 bytes into the structure _averages.		    */
  struct {
	    float   load_avg[3];    /*	1 min, 5 min, 15 min	*/
	    float   prio_avg[3];    /*	1 min, 5 min, 15 min	*/
	    float   queue_avg[3];   /*	1 min, 5 min, 15 min	*/
	 }  _averages;

  FILE	*lav;

  sysload = 0.0;
  if (GDCgetMaxLoad(Config)==0.0)
    return(0);
  if ((lav=fopen("$$VMS_LOAD_AVERAGE","r"))==NULL) {
    if (DEBUG)
      printf("Cannot access load averages...%s%s%s",
	    strerror(EVMSERR,vaxc$errno),vaxc$errno_stv?"/ ":"",
		    vaxc$errno_stv?strerror(EVMSERR,vaxc$errno_stv):"");
    return(0);
  }
  if (fread((char *)&_averages,sizeof(_averages),1,lav)==0) {
    if (DEBUG)
      printf("Cannot read load averages...");
    fclose(lav);
    return(0);
  }
  if((sysload = _averages.load_avg[WHICHLOAD]) > GDCgetMaxLoad(Config)) {
    if(DEBUG)
      printf("System maxload %f exceeded (currently %f)\n",
		    GDCgetMaxLoad(Config),sysload);
    fclose(lav);
    return(1);
  }
  fclose(lav);
  return(0);
}
#else
#ifndef MAXLOAD
#define MAXLOAD 10.0
#endif
double maxload = MAXLOAD;
double sysload = 0.0;
double atof();
#include <nlist.h>
#include <kvm.h>
#define X_AVENRUN 0
long avenrun[3];
kvm_t * kd;
struct nlist nl[] = { {LOADSYMBOL}, {""}, };

int getload()
{
  if ((kd = kvm_open(NULL, NULL, NULL, O_RDONLY, NULL)) == NULL) 
    return(-1);
  if (kvm_nlist(kd, nl) != 0) 
    return(-1);
  if(nl[X_AVENRUN].n_type == 0) 
    return(-1);
  if(kvm_read(kd,nl[X_AVENRUN].n_value,avenrun,sizeof(avenrun)) 
     != sizeof(avenrun)) 
    return(-1);
  if((sysload = (((double) avenrun[WHICHLOAD]) / FSCALE)) > maxload) {
    if(DEBUG)
      printf("System maxload %f exceeded (currently %f)\n",maxload,sysload);
    return(1);
  }
  return(0);
}
#endif
#endif /* LOADRESTRICT */     









