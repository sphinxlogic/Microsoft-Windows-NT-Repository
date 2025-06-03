/*									*/
/*	Copyright (©) Ecole Nationale Supérieure des Télécommunications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-2)				*/
/*									*/
/*	20-fev-1994:	Guillaume gérard				*/
/*									*/
/*	Ce logiciel est fourni gratuitement et ne peut faire		*/
/*		l'objet d'aucune commercialisation			*/
/*									*/
/*	Aucune garantie d'utilisation, ni implicite,			*/
/*		ni explicite, n'est fournie avec ce logiciel.		*/
/*		Utilisez-le à vos risques et périls			*/
/*									*/
/*	This freeware is given freely and cannot be sold		*/
/*	No warranty is given for reliability of this software		*/
/*	Use at your own risk						*/
/*									*/
/****************************************************************/
/*								*/
/*			PROCEDURES AG2T				*/
/*								*/
/****************************************************************/

void extern CHECK (float * X , float * Y);
void extern BSYMS (float * X, float * Y , int * ISYM);
void extern MNMX (float * ARRAY0 , float * AMIN, float * AMAX);
void extern WIDTH (int * NBASE);
void extern CPLOT (float * X, float * Y);
/*********************  nom a changer **********************/
/*void extern LABEL(int * NBASE);*/
/***********************************************************/
void extern REMLAB (int * NBASE, int * ILOC, int * LABTYP, int * IRX, int *IRY);
void extern EXPOUT (int * NBASE, int * IEXP, int * IARRAY, int * NCHARS, int * IFILL);
void extern AUTOAXC (int * XMIN, int * XMAX, int * YMIN, int * YMAX,
                    float * TICX, float * TICY,
                    int * ITX, int * ITY, int * IGRDX, int * IGRDY);
void extern AXIS (float * XMIN, float * YMIN, float * TICX, float * TICY,
		  float * XLEN, float * YLEN, int * ITX, int * ITY);
void extern FILBOX (int * MINX, int * MINY, int * MAXX,int * MAXY,
		    int * ISYMB, int * LSPACE);
void extern VLABEL (int * ILEN, int * ILAB);
void extern SOFTEK ();
void extern USERS ();
void extern STEPS (int * IVALUE);
void extern STEPL (int * IVALUE);
void extern SIZES (float * VALUE);
void extern SIZEL (float * VALUE);
void extern LINE (int * IVALUE);
void extern SYMBL (int * IVALUE);
void extern INFIN (float * VALUE);
void extern NPTS (int * IVALUE);
void extern SLIMX (int * IXMIN, int * IXMAX);
void extern SLIMY (int * IYMIN, int * IYMAX);
void extern DLIMX (float * XMIN, float * XMAX);
void extern DLIMY (float * YMIN, float * YMAX);
/*******************  nom a changer *************************/
/*void extern XTYNE XTICS(int * IVALUE);*/
/************************************************************/
void extern XLEN (int * IVALUE);
void extern XFRM (int * IVALUE);
void extern XMTCS (int * IVALUE);
void extern XMFRM (int * IVALUE);
void extern YTICS (int * IVALUE);
void extern YLEN (int * IVALUE);
void extern YFRM (int * IVALUE);
void extern YMTCS (int * IVALUE);
void extern YMFRM (int * IVALUE);
void extern YNEAT (int * IVALUE);
void extern YZERO (int * IVALUE);
void extern XNEAT (int * IVALUE);
void extern XZERO (int * IVALUE);
void extern XTYPE (int * IVALUE);
void extern YTYPE (int * IVALUE);
void extern DINITX ();
void extern XWDTH (int * IV);
void extern FRAME ();
void extern YDEN (int * IVALUE);
void extern YLOCRT (int * IRAST);
void extern DSPLAY (float * X , float * Y);
void extern TSET2 (int * NEWLOC, int * NFAR, int * NLEN, int * NFRM,
		   int * KSTART, int * KEND);
void extern LOPTIM (int * NBASE);
void extern GRID ();
void extern LOGTIX (int * NBASE, float * START, float * TINTVL,
		   int * MSTART, int * MEND);
void extern TEKSYM (int * ISYM);
void extern SETWIN ();
void extern LWIDTH (int * NBASE);
void extern SPREAD (int * NBASE);
void extern TSET (int * NBASE);
void extern KEYSET (float * ARRAY0, int * KEY);
void extern RGCHEK (int * NBASE, float * ARRAY0);
void extern EFORM (float * FNUM, int * IWIDTH, int * IDEC, int * IARRAY, 
		   int * IFILL);
void extern FONLY (float * FNUM, int * IWIDTH, int * IDEC, float * IARRAY,
			int * IFILL);
void extern ESPLIT (float * FNUM, int * IWIDTH, int * IDEC, int * IEXPON);
void extern IFORM (float * FNUM, int * IWIDTH, int * IARRAY, int * IFILL);
void extern JUSTER (int * LENSTR, int * IARRAY, 
                    int * IPOSIT, int * IFILL, int * LENCHR, int * IOFF);
void extern NUMSET (float * FNUM, int * IWIDTH, int * NBASE, int * IARRAY, 
		   int * IFILL);
void extern FFORM (float * FNUM, int * IWIDTH, int * IDEC, int * IARRAY,
                   int * IFILL);
void extern WRT1A1 (int * ITXT);
void extern COMDMP ();
void extern NOTATE (int * IX, int * IY, int * LENCHR, int * IARRAY);
void extern HLABEL (int * LEN, int * IARRAY);
void extern HSTRIN (int * IARRAY);
void extern PLACE (int * ICODE);
void extern BINITT ();
void extern COMSET (int * ITEM, float * VALUE);
void extern RESCOM (float * ARRAY0);
void extern SAVCOM (float * ARRAY0);
void extern ERREC (int * I);
void extern ULINE ();
void extern UMNMX ();
void extern CUPOINT ();
void extern USESET ();
void extern AXES (float * XMIN, float * XMAX, float * YMIN, float * YMAX);
float extern DATGET (float * ARR, int * I, int * KEY);
float extern FINDGE (float * VALUE, float * TABLE,int * IPOINT);
float extern ROUNDD (float * VALUE, float * FINT);
float extern ROUNDU (float * VALUE, float * FINT);
int extern IOTHER (int * NBASE);
int extern IBASEC (int * IOFF0);
int extern IBASEX (int * IOFF1);
int extern IBASEY (int * IOFF2);
float extern COMGET (int * ITEM0);
