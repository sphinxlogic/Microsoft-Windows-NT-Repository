/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:13:27 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/gopherd/RCS/ext.c,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: ext.c
 * These fns define mapping of file extensions to gopher objects.
 *********************************************************************
 * Revision History:
 * $Log: ext.c,v $
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/

#include "ext.h"
#include "Malloc.h"
#include <ctype.h>

extern boolean DEBUG;

/*
 * Some functions to initialize and destroy sites and extensions..
 * (Needed for use with DynArrays...)
 */

static Ext *
ExtNew()
{
     Ext *temp;

     temp = (Ext *) malloc(sizeof(Ext));
     
     if (temp == NULL)
	  return(NULL);

     temp->Objtype   = '\0';
     temp->GplusType = STRnew();
     STRinit(temp->GplusType);
     temp->Prefix    = STRnew();
     STRinit(temp->Prefix);
     temp->ext       = STRnew();
     STRinit(temp->ext);

     return(temp);
}

static void ExtDestroy(ext)
  Ext *ext;
{
     STRdestroy(ext->GplusType);
     STRdestroy(ext->Prefix);
     STRdestroy(ext->ext);
     
     free(ext);
}

static void
Extcpy(ext1, ext2)
  Ext *ext1, *ext2;
{
     ext1->Objtype  = ext2->Objtype;
     STRcpy(ext1->GplusType, ext2->GplusType);
     STRcpy(ext1->Prefix, ext2->Prefix);
     STRcpy(ext1->ext, ext2->ext);
}

static void
ExtSet(ext, objtype, gplustype, prefix, fileext)
  Ext *ext;
  char       objtype;
  char       *gplustype;
  char       *prefix;
  char       *fileext;
{
     ext->Objtype = objtype;
     STRset(ext->GplusType, gplustype);
     STRset(ext->Prefix, prefix);
     STRset(ext->ext, fileext);
}

/******************************************************/
/* Functions external routines can call.    */

ExtArray *
ExtArrayNew()
{
     ExtArray *temp;
     
     temp = DAnew(20, ExtNew, NULL, ExtDestroy, Extcpy);
     
     return(temp);
}

void
ExtAdd(extarr, objtype, gplustype, prefix, fileext)
  ExtArray *extarr;
  char objtype;
  char *gplustype;
  char *prefix;
  char *fileext;
{
     Ext *ext;

     if (DEBUG) printf("Adding extension '%s', objtype %c, prefix %s, gplustype '%s'\n",
		       fileext, objtype, prefix, gplustype);
     
     ext = ExtNew();
     
     ExtSet(ext, objtype, gplustype, prefix, fileext);
     
     DApush(extarr, ext);
     
     ExtDestroy(ext);
     
}

/*
 * Get the parameters associated with a particular extension
 */

void
ExtGet(extarr, fileext, objtype, gplustype, prefix)
  ExtArray *extarr;
  char *fileext;
  char *objtype;
  char **gplustype;
  char **prefix;
{
     int i;
     Ext *temp;

     *objtype   = '\0';
     *gplustype = NULL;
     *prefix    = NULL;

     /*** Linear search.  Ick. ***/
     
     for (i=0; i< DAgetTop(extarr); i++) {

	  temp = ExtgetEntry(extarr,i);
	  
	  if (strcasecmp(fileext+strlen(fileext)-strlen(STRget(temp->ext)), STRget(temp->ext))==0) {

	       *objtype   = temp->Objtype;
	       *gplustype = STRget(temp->GplusType);
	       *prefix    = STRget(temp->Prefix);
	       
	       return;
	  }
     }
}

boolean 
ExtProcessLine(extarr, inputline)
  ExtArray *extarr;
  char     *inputline;
{
     int i;
     char ext[64];
     char Gtype;
     char prefix[64];
     char Gplustype[64];

     /** Strip out the white space **/
     while (isspace(*inputline))
	  inputline++;
     if (*inputline == '\0') return(FALSE);

     /*** The extension itself ***/
     i=0;
     while (!isspace(*inputline))
	  ext[i++] = *inputline++;
     if (*inputline == '\0') return(FALSE);

     ext[i] = '\0';
     
     while (isspace(*inputline))
	  inputline++;
     if (*inputline == '\0') return(FALSE);

     /*** The Gophertype ***/
     Gtype = *inputline++;
     
     while (isspace(*inputline))
	  inputline++;
     if (*inputline == '\0') return(FALSE);

     /*** Prefix ***/
     i=0;
     while (!isspace(*inputline))
	  prefix[i++] = *inputline++;
     if (*inputline == '\0') return(FALSE);
     prefix[i]='\0';

     while (isspace(*inputline))
	  inputline++;
     if (*inputline == '\0') return(FALSE);

     /*** Gopher + type ***/
     i=0;
     while (!isspace(*inputline))
	  Gplustype[i++]= *inputline++;

     Gplustype[i] = '\0';

     ExtAdd(extarr, Gtype, Gplustype, prefix, ext);
     return(TRUE);
}
