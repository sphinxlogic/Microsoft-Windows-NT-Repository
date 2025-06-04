/*									*/
/*	Copyright (©) Ecole Nationale Supérieure des Télécommunications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-3)				*/
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
/*			PROCEDURES DE TCS			*/
/*								*/
/****************************************************************/

extern void LINCLR (long * INDEX);
extern void BEGPAT (long * IPATRN, long * IWIDE, long * IHIGH, long * IBITS);
extern void VBGPNL (float * X, float * Y, long * IBOUND);
extern void BEGPNL (int * IX, int *IY, int *IBOUND);
extern void ENDPAT ();
extern void ENDPNL ();
extern void FILMOD (int * IMODE, int * IFILL, int * IKEY);
extern void SELPAT (int * IPATRN);
extern void RASRIT (int * IPIXLS, int * ILEN, int * IBRAY);
extern void RUNRIT (int * ILEN , int * IRUN);
extern void ADERAY (int * LEN , int * IRAY);
extern void CMDOUT (int * IC1, int * IC2);
extern void INTRAY (int * LEN , int * IRAY);
extern void INTOUT (int * INT);
extern void XYOUT (int * IX, int * IY);
extern void XYTRN (int * IX, int * IY, int * LEN , int * ICHARS);
extern void ANMODE ();
extern void SEEFAC (int * IFACT);
extern void SEEMRG (int *  MLEFT, int * MRIGHT);
extern void SEETRM (int * IBAUDR, int * ITERM, int * ICSIZE, int * MAXSCR);
extern void SEETW (int *  MINX, int * MAXX, int * MINY, int * MAXY);
extern void SEEDW (float * XMIN, float * XMAX, float * YMIN, float * YMAX);
extern void SEEMOD (int *  LINE, int * IZAXIS, int * MODE);
extern void SEETRN (float * XFAC, float * YFAC, int *  KEY);
extern void SEEREL (float * RCOS, float * RSIN, float * SCALE);
extern void TTBLSZ (int *  ITBLSZ);
extern void TCSLEV (int *  LEVEL);
extern void TERM (int * ITERM, int * ISCAL);
extern void DASHSR (float * X, float * Y, int * L);
extern void DASHSA (float * X, float * Y, int * L);
extern void DRAWSR (float * X, float * Y);
extern void DRAWSA (float * X, float * Y);
extern void INCPLT (int * IONOFF, int * IDIR, int * NO);
extern void RROTAT (float * DEG);
extern void RSCALE (float * FACTOR);
extern void FINITT (int * IX, int * IY);
extern void BELL ();
extern void VWINDO (float * XMIN, float * XRANGE, float * YMIN, float * YRANGE);
extern void SWINDO (int *  MINX, int * LENX, int * MINY, int * LENY);
extern void TWINDO (int *  MINX, int * MAXX, int * MINY, int * MAXY);
extern void DWINDO (float * XMIN, float * XMAX, float * YMIN, float * YMAX);
extern void POINTR (float * X, float * Y);
extern void DASHR (float * X, float * Y, int * L);
extern void DASHA (float * X, float * Y, int * L);
extern void DSHREL (int * IX, int * IY, int * L);
extern void DSHABS (int * IX, int * IY, int * L);
extern void DRAWR (float * X, float * Y);
extern void POINTA (float * X, float * Y);
extern void DRAWA (float * X, float * Y);
extern void MOVER (float * X, float * Y);
extern void REL2AB (float * XIN, float * YIN, float * XOUT, float * YOUT);
extern void MOVEA (float * X, float * Y);
extern void VCURSR (int * ICHAR , float * X, float * Y);
extern void V2ST (int * I, float * X, float * Y, int * IX, int * IY);
extern void CLIPT (int * BUFIN, int * OUTBF);
extern void PARCLT (float * RL1, float * RL2, float * RM1,
		    float * RM2, float * RN1, float * RN2);
extern void LINTRN ();
extern void LOGTRN (int * ITYPE);
extern void POLTRN (float * ANGMIN, float * ANGMAX, float * RSUPRS);
extern void RESCAL ();
extern void PSCAL ();
extern void WINCOT (float * X, float * Y, int * IX, int * IY);
extern void LVLCHT ();
extern void REVCOT (int * IX, int * IY, float * X, float * Y);
extern void PCLIPT (float * X, float * Y);
extern void PNTREL (int * IX, int * IY);
extern void PNTABS (int * IX, int * IY);
extern void DRWREL (int * IX, int * IY);
extern void DRWABS (int * IX, int * IY);
extern void SETKEY (int * IKEY);
extern void SETMRG (int * MLEFT, int * MRIGHT);
extern void MOVREL (int * IX, int * IY);
extern void SETTAB (int * ITAB, int * ITBTBL);
extern void RSTTAB (int * ITAB, int * ITBTBL);
extern void TABHOR (int * ITBTBL);
extern void TABVER (int * ITBTBL);
extern void DSHMOD (int * L);
extern void TKDASH (int * IX, int * IY);
extern void HDCOPY (int * IARG);
extern void DCURSR (int * ICHAR, int * IX, int * IY);
extern void SCURSR (int * ICHAR, int * IX, int * IY);
extern void A1IN (int * NCHAR, int * IA1);
extern void AINST (int *  NCHAR, int * IAM);
extern void TINPUT (int * IN1);
extern void TINSTR (int *  NCHAR, int * IADE);
extern void SVSTAT (float * RARRAY);
extern void RESTAT (float * RARRAY);
extern void INITT (int * IBAUD, int * ITERM);
extern void SEELOC (int * IX, int * IY);
#define SETBUF tcs$setbuf
extern void SETBUF (int * KFORM);
extern void RESET ();
extern void TKPNT (int * IX, int * IY);
extern void ANCHO (int * ICHAR);
extern void A1OUT (int * NCHAR, int * IA1);
extern void AOUTST (int * NCHAR, int * IAM);
extern void ANSTR (int * NCHAR, int * IADE);
extern void NEWLIN ();
extern void CARTN ();
extern void LINEF ();
extern void BAKSP ();
extern void HOME ();
extern void ERASE ();
extern void RECOVR ();
extern void NEWPAG ();
extern void ALFMOD ();
extern void MOVABS (int * IX, int * IY);
extern void IOWAIT (int * ITIME);
extern void VECMOD ();
extern void CSIZE (int * IHORZ, int * IVERT);
extern void PNTMOD ();
extern void IPMOD ();
extern void XYCNVT (int * IX, int * IY);
extern void CHRSIZ (int * KX, int * KY);
extern void CZAXIS (int * ICODE);
extern void CWSEND ();
extern void TOUTEND ();
extern void TOUTPT (int * KKOUT);
extern void TOUTST (int * LEN, int * IADE);
extern void BUFFPK (int * NCHAR, int * IOUT);
extern void PLTCHR (int * IX, int * IY, int * ICHAR);
extern void KAS2A1 (int * NCHAR, int * KADE, int * KA1);
extern void KA12AS (int * NCHAR, int * KA1, int * KADE);
extern void KAS2AM (int * NCHAR, int * KADE, int * KAM);
extern void KAM2AS (int * NCHAR, int * KAM, int * KADE);
extern void ADEIN (int * LENOUT, int * IDATA);
extern void ADEOUT (int * LENGTH, int * IDATA_DESC); 
extern void DEBPLOT (int * IARG);
extern void DEBCOPY (int * IARG);
extern void FINPLOT ();
extern void MALPHA ();
extern void MGRAPH ();
extern void DBLSIZ (int * IK);
extern void NRMSIZ ();
extern void ITALIC ();     
extern void ITALIR ();
extern void NORMAL ();
extern void RUBOUT ();
extern void SETRMW ();
extern void PAV4X4 ();
extern void BLKABS (int * IXMIN, int * IXMAX, int * IYMIN, int * IYMAX);
extern void BLKREL (int * IXREL, int * IXSIZ, int * IYREL, int * IYSIZ);
extern void BLOCKA (float * XMIN, float * XMAX, float * YMIN, float * YMAX);
extern void BLOCKR (float * XREL, float * XSIZ, float * YREL, float * YSIZ);
extern void LWINDO (float * XMIN, float * XMAX, float * YMIN, float * YMAX,
			int * KEYLW);
extern void PWINDO (float * THETA1, float * THETA2, float * RMIN, float * RMAX,
			float * RSUP);
extern int KIN (float * RI);
extern int KCM (float * RC);
extern int LEFTIO (int * IOBUF);
extern int GENFLG (int * ITEM);	/* Fortran Boolean? */
extern int LINWDT (int * NUMCHR); 
extern int LINHGT (int * NUMLIN);
