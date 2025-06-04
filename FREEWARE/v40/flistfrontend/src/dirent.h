/* $Id: dirent.h,v 1.16 1995/11/15 20:21:50 tom Exp $ */

/*
 * Title:	dirent.h
 * Author:	Thomas E. Dickey
 * Created:	02 May 1984
 * Last update:
 *		28 May 1995, prototypes
 *		18 Feb 1995, ported to DEC Alpha.
 *		04 Nov 1988, added field ".fexpr", enlarged .f_mbm, .f_grp
 *		09 Sep 1985, added NAME_DOT
 *		30 Jul 1985, removed 'numreads'
 *		16 Jul 1985, changed 'filelist' to an array of pointers to
 *			     'filelink'.
 *		13 Jul 1985, added FLINK, filelink.
 *		05 Jul 1985, use FK, FK_ to absorb other references to filelist.
 *			     Added '.path_refs' member to PATHENT.
 *		02 Jul 1985, make 'filelist', etc., into import-definitions
 *		27 Jun 1985, corrected value of 'WILD_VER'
 *		16 Jun 1985, file-version (and WILD_CARD) are *signed*.
 *		15 Jun 1985, added 'FK' to absorb references to 'filelist'.
 *		13 Jun 1985, added 'dateflag', 'datechek'
 *		18 May 1985, added 'pcolumns'
 *		22 Mar 1985, added '.fidnum', '.f_recl' to FILENT
 *		10 Mar 1985, moved buffers out of PATHNT
 *		01 Feb 1985, added 'WILD_VER', change 'readlist' (cf: dirread)
 *		24 Dec 1984, added 'D_mode'
 *		22 Dec 1984, added '.fallc' (allocation)
 *		20 Dec 1984, added width-latch for FORMAT.
 *		11 Dec 1984, added 'NOPRIV' definition.
 *		05 Dec 1984, make filename, filetype pointers (not fixed size)
 *			     increase name,type lengths for VMS 4.0
 *		16 Nov 1984, added record-format stuff
 *		02 Sep 1984, convert to support "import"
 *		25 Aug 1984, use "names.h"
 *		14 Aug 1984, moved DELETED def here, added '.fmisc' to FILENT
 *		06 Aug 1984, added '.fhour' component to FILENT
 *		28 Jul 1984, added FILENT_????_size, re-ordered FILENT
 *		26 Jul 1984, added 'LOCKED', 'readlist'
 *		20 Jul 1984, added 'numreads'
 *		15 Jul 1984, added 'O_opt', also tag component to PATHNT
 *		03 Jul 1984, re-ordered FILENT to match GETPROT
 *		21 May 1984
 *
 *	Define structure used by FLIST to store information for a directory
 *	entry.
 */

#ifndef DIRENT_H
#define DIRENT_H

#include	<rms.h>

#include	"names.h"

#define	MAX_NAME	39	/* length of filename		*/
#define	MAX_TYPE	39	/* length of filetype		*/
#define	WILD_VER	(-32768)
#define	NAME_DOT	1	/* adjustment to get trailing '.' on name */

/*
 * Base of FILENT data, given index to 'filelist':
 */
#define	FK(k)	filelist[k]->fk
#define	FK_(k)	(&FK(k))	/* address of the FILENT-block */

/*
 * FLIST makes a linked-list of these, to be able to test the pathname
 * independently of the filename, and to save space:
 */
typedef	struct my_pathnt {
	struct my_pathnt *path_next;	/* => next entry in chain	*/
	unsigned
	char	path_refs;	/* level-reference flag (TEXTLINK) */
	short	path_sort;	/* sort key, used in 'dirsrt.c'	*/
	char	*path_text,	/* => pathname, include device	*/
		*path_trim;	/* => string used in sorting	*/
	} PATHNT;

#define	zPATHOF(z)	(z->fpath_->path_text)
#define	PATHOF(inx)	(FK(inx).fpath_->path_text)

#ifndef	RMS$_PRV
#define	RMS$_PRV	0x1829a
#endif
#define	zNOPRIV(z)	(z->fstat == RMS$_PRV)
#define	NOPRIV(inx)	(FK(inx).fstat == RMS$_PRV)

#ifndef	RMS$_FLK
#define	RMS$_FLK	0x1828a
#endif
#define	zLOCKED(z)	(z->fstat == RMS$_FLK)
#define	LOCKED(inx)	(FK(inx).fstat == RMS$_FLK)

#ifndef	RMS$_FNF
#define	RMS$_FNF	0x18292
#endif
#define	zDELETED(z)	(z->fstat == RMS$_FNF)
#define	DELETED(inx)	(FK(inx).fstat == RMS$_FNF)

#include "datent.h"
#include "getprot.h"

/*
 * FLIST stores one of these for each filename:
 */
typedef	struct my_filent {
	/*
	 * Path,name,type and version are grouped for same-name lookups:
	 */
	PATHNT	*fpath_;	/* => PATHNT entry			*/
	char	*fname,		/* filename, less path, type		*/
		*ftype;		/* filetype, less "."			*/
	short	fvers;		/* File version number (1-32767)	*/
	/*
	 * Other data are unordered:
	 */
	DATENT	fdate;		/* Creation date (DIR/DATE)		*/
	DATENT	fback;		/* Backup date (DIR/FULL)		*/
	DATENT	frevi;		/* Revision date (DIR/FULL)		*/
	DATENT	fexpr;		/* Expiration date (DIR/FULL)		*/
	unsigned
	short	fhour;		/* Hour-in-day for "/sh" sorts		*/
	unsigned fsize;		/* File size (DIR/SIZE), -1=unknown	*/
	unsigned fallc;		/* File allocation			*/
	unsigned fstat;		/* RMS-status on open			*/
	unsigned
	char	fmisc,		/* temp bit-vector for list manipulation*/
		f_rat,		/* FAB: record attributes		*/
		f_rfm,		/* FAB: record format			*/
		ftext;		/* set iff 'INSPECT' says file is text	*/
	unsigned
	short	f_recl;		/* Record-length			*/
	unsigned
	short	fidnum[3];	/* File-identifier			*/
	GETPROT	f_getprot;
	char	*f_user;	/* user identifier (from f_getprot)	*/
	} FILENT;

#define	fprot	f_getprot.p_mask
#define	f_grp	f_getprot.p_grp
#define	f_mbm	f_getprot.p_mbm

/*
 * Patch: There appears to be no automatic way to get 'sizeof(FILENT.fpath_)'
 *	or 'sizeof(FILENT.fvers)', which are 4 and 2 respectively.
 */
#define	FILENT_vers_size	 (4 + 4 + 4)
#define	FILENT_name_size	 (FILENT_vers_size + 2)

/*
 * FLIST stores the FILENT blocks in a linked-list which is accessible to all
 * levels of the hierarchy.  A given level uses an array of pointers (for fast
 * access) to its own subset of the FILENT-blocks.  This list can be automatically
 * constructed from the '.file_refs' mask (see NAMEHEAP).
 */
typedef	struct my_flink {
	struct my_flink	*file_next; /* => next entry in chain	*/
	unsigned
	char	file_refs;	/* level-reference flag (TEXTLINK) */
	FILENT	fk;		/* file-data-block		*/
	}	FLINK;


/*
 * Define an 'import' structure, to permit me to do this in a structured
 * manner.  (One problem with the bulk-include is that the link-map grows
 * phenomenally).  This relies on the CC-preprocessor to perform string
 * concatenation.
 */
#define	type_AnyXAB	int		/* TRUE if any of a,d,p options	*/
#define	type_A_opt	int		/* Show file allocation		*/
#define	type_D_opt	int		/* Show creation date		*/
#define	type_D_mode	int		/* Mode in which to show date	*/
#define	type_H_opt	int		/* Permit sort-by-hour		*/
#define	type_M_opt	int		/* Show protection mask		*/
#define	type_O_opt	int		/* Show owner code		*/
#define	type_P_opt	int		/* Show names of no-priv files	*/
#define	type_V_opt	int		/* Show only highest version	*/
#define	type_ccolumns	char		/* width(path,name,type,vers,rfm,user)*/
#define	type_pcolumns	char		/* printing-widths (override)	*/
#define	type_conv_list	char		/* display-format codes (alpha)	*/
#define	type_maxfiles	int		/* # of files (space allocated)	*/
#define	type_numfiles	int		/* # of items in 'filelist[]'	*/
#define	type_numdlets	int		/* # of those which are deleted	*/
#define	type_pathlist	PATHNT	*	/* pathnames (common)		*/
#define	type_filelink	FLINK	*	/* filenames and their data	*/
#define	type_filelist	FLINK	**	/* filenames and their data	*/
#define	type_readlist	FILENT	*	/* => list of filespecs read in	*/
#define	type_readllen	int		/* # of entries in 'readlist[]'	*/
#define	type_namelist	char	*	/* => list of filenames, types	*/
#define	type_dateflag	int		/* (AFTER,BEFORE) (D_opt)	*/
#define	type_datechek	DATENT		/* date-selection threshold	*/

#define	size_AnyXAB
#define	size_A_opt
#define	size_D_opt
#define	size_D_mode
#define	size_H_opt
#define	size_M_opt
#define	size_O_opt
#define	size_P_opt
#define	size_V_opt
#define	size_ccolumns	[9]		/* name,type,version,path,rfm,org,user,size,alloc*/
#define	size_pcolumns	[2]		/* name,type	*/
#define	size_conv_list	[27]
#define	size_maxfiles
#define	size_numfiles
#define	size_numdlets
#define	size_pathlist
#define	size_filelink
#define	size_filelist
#define	size_readlist
#define	size_readllen
#define	size_namelist
#define	size_dateflag	[2]		/* (AFTER,BEFORE) (D_opt)	*/
#define	size_datechek			/* date-selection threshold	*/

#ifndef	DIRENT
#define	DIRENT	extern
#endif

#if __STDC__
#define concat(a,b)  a ## b
#define import(x)    DIRENT	concat(type_,x)	x concat(size_,x)
#else
#define import(x)    DIRENT	type_/**/x	x size_/**/x
#endif

extern	int	dirent__datechek (FILENT *z);
extern	int	dirent_acc (FILENT *z, char *mode);
extern	int	dirent_add (void);
extern	void	dirent_all (char *filespec, int update);
extern	void	dirent_cat_n (char *s, FILENT *z);
extern	int	dirent_ccol (void);
extern	int	dirent_chk (FILENT *z, char *filespec);
extern	int	dirent_chk2 (int j);
extern	int	dirent_chk3 (int j, char *fullname);
extern	int	dirent_chop (FILENT *z, char *filespec, struct NAM *nam_);
extern	void	dirent_conv (char *bfr, FILENT *z);
extern	char*	dirent_dft (void);
extern	void	dirent_dlet (int j);
extern	char*	dirent_glue (char *filespec, FILENT *z);
extern	char*	dirent_glue2 (char *filespec, FILENT *z, int curfile);
extern	int	dirent_isdir (FILENT *z);
extern	unsigned dirent_look (char *longspec, char *filespec);
extern	void	dirent_misc (int inx, int bitv);
extern	void	dirent_nul (int inx);
extern	int	dirent_old (char *filespec, int need);
extern	int	dirent_old1 (FILENT *z, char *spec);
extern	int	dirent_old_any (FILENT *z, char *filespec, int need);
extern	int	dirent_width (FILENT *z);

#ifdef	DCLARG_H
extern	int	dirent (DCLARG *arg_);
#endif

extern	int	dirhigh (char *filespec);
extern	int	diropen (char *name_);
extern	int	diropen2 (FILENT *z);

extern	unsigned acplook (FILENT *z, char *filespec, struct NAM *nam_);
extern	unsigned rmslook (FILENT *z, struct FAB *fab_);

#endif	/* DIRENT_H */
