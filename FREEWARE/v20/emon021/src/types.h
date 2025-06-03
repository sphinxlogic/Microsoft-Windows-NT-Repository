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
#ifndef GG_TYPES
#define GG_TYPES True
#ifndef __DECC
#define malloc VAXC$MALLOC_OPT
#define free   VAXC$FREE_OPT
#define calloc	VAXC$CALLOC_OPT
#define cfree	VAXC$CFREE_OPT
#define realloc	VAXC$REALLOC_OPT
#endif
typedef struct {unsigned long low, high;} vmstime_t;
typedef unsigned char  ubyte;
typedef 	 char  byte;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef struct {ushort iosb_status,iosb_count; ulong iosb_devdep;} iosb_t;

typedef enum {false,true} boolean;
#define istrue(x) (((int) x) & 1)
#define isfalse(x) (~((int) x) & 1)
#endif
