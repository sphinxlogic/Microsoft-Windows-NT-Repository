/* ODS-2 reader. To use in VMS, 
  $ define disk$users dka0:
  $ getvms -l [000000]
... etc.
*/
/* Following sent by Carl Lydick of CalTech. */
/* note: original version knows nothing about filenames longer than
  9.3 chars, and this knows nothing about ACLs. */
/*% cc -O -o getvms %
 *
 *  Copy files from VMS (ODS-2) filesystem.  Files may be copied to
 *  disk or to standard output.  Transfer modes supported are "text"
 *  (RMS stuff is thrown away, newline is tacked on the end of each
 *  VMS record), and "image" (straight byte-by-byte transfer).	There
 *  were once plans to support a third mode ("binary"), but this has
 *  not yet been implemented.  Defaults for the output destination and
 *  transfer mode are set by #defines, but the destination/mode can be
 *  specified at runtime by using various flags (see "options()").
 *
 *  The input device and directory, if omitted, will default to that of
 *  the previous filespec.  Note that this means that the first filespec
 *  MUST have a directory specified, and (if DFLTDEV is not defined) also
 *  a device as well.  The filename syntax is the same as the standard
 *  VMS naming scheme, except that a "." may be used to separate the
 *  filetype from the version number, and some delimiters may be changed
 *  via #defines, if desired.  (This is all to avoid the possibility of
 *  having to escape some of the characters that the shell treats as
 *  special.)  The device name is the name of the UNIX special file in
 *  /dev, rather than what VMS thinks it would be.
 *
 *  If the first character of argv[0] is "l", or if the "-l" option is
 *  used, the program lists the contents of the directory rather than
 *  copying a file.  At present, only one directory may be listed per
 *  command.
 *
 *  Written by Mark Bartelt, based on an earlier program which copied
 *  files from ODS-1 volumes.
 *
 *	hacked 6-aug-82 norman wilson:
 *		in getvb, don't let h_use get sign extended
 *		added -T & line number stripping
 *		bwk'd putch a bit in the process
 *		added access checks to openout to plug a security hole
 *		this last should probably be commented out for non-unix
 *
 *	hacked 16-aug-84  Sam Sjogren:
 *		In gethdr(), check to see if the user is the
 *		superuser.  If so, ignore file protection.
 *
 *	hacked 26-feb-85  Carl J Lydick
 *		In getde(), check for end-of-file occurring as first word
 *		in the block returned by getvb().  Failure to do so causes
 *		garbage and core dumps.
 */


#define DFLTDEV "disk$users"

#ifdef VMS
/* sure this will run under VMS. It might even be the start of a useful hack */
#define index strchr
#define rindex strrchr
#include <descrip.h>
#include <ssdef.h>
#include <iodef.h>
#endif

#define err0(msg)	{ errmsg(msg); return(0); }
#define err1(msg,arg)	{ errmsg(msg,arg); return(0); }

#define alphnum(x)	( ( 'a'<=(x) && (x)<='z' ) || ( 'A'<=(x) && (x)<='Z' ) || ( '0'<=(x) && (x)<='9' ) )

#define decimal(x)	( '0'<=(x) && (x)<='9' )

#include <stdio.h>

typedef unsigned short	ushort;

struct filnam {
	char	f_nam[14];	/* File name (ASCII) */
	ushort	f_ver;		/* Version number */
};

struct uic {
	ushort	u_prog; 	/* Programmer number */
	ushort	u_proj; 	/* Project number */
};

struct fileid {
	ushort	f_num;		/* File number */
	ushort	f_seq;		/* File sequence number (worthless concept) */
	ushort	f_rvn;		/* Relative volume number (ditto and MBZ) */
};

struct rms {
	char	f_forg; 	/* Record format and file organization */
	char	f_ratt; 	/* Record attributes */
	ushort	f_rsiz; 	/* Record size */
	ushort	f_hvbn[2];	/* Highest VBN allocated */
	ushort	f_heof[2];	/* End of file block */
	ushort	f_ffby; 	/* First free byte */
	char	f_bksz; 	/* Bucket size */
	char	f_hdsz; 	/* Fixed header size */
	ushort	f_mrs;		/* Maximum record size */
	ushort	f_deq;		/* Default extend quantity */
};

struct ident {
	char	i_fnam[20];	/* File name */
	ushort	i_rvno; 	/* Revision number */
	char	i_crdt[8];	/* Creation date and time */
	char	i_rvdt[8];	/* Revision date and time */
	char	i_exdt[8];	/* Expiration date and time */
	char	i_bkdt[8];	/* Backup date and time */
	char	i_ulab[80];	/* User label */
};

struct header {
	char	h_idof; 	/* Ident area offset */
	char	h_mpof; 	/* Map area offset */
	char	h_acof; 	/* Access control list offset */
	char	h_rsof; 	/* Reserved area offset */
	ushort	h_fseg; 	/* Extension segment number */
	ushort	h_flev; 	/* Structure level and version */
	ushort	h_fnum; 	/* File number */
	ushort	h_fseq; 	/* File sequence number */
	ushort	h_frvn; 	/* Relative volume number */
	ushort	h_efnu; 	/* Extension file number */
	ushort	h_efsq; 	/* Extension file sequence number */
	ushort	h_ervn; 	/* Extension relative volume number */
	union {
		char	hu_ufat[32];	/* User file attributes */
		struct rms hu_rms;	/* RMS file attributes */
	} h_ufat;
#define h_rms	h_ufat.hu_rms
	char	h_fcha[4];	/* File characteristics */
#define h_ucha	h_fcha[0]	/* User controlled characteristics */
#define h_scha	h_fcha[1]	/* System controlled characteristics */
	char	h_UU1[2];	/* Unused 1 */
	char	h_use;		/* Map words in use */
	char	h_priv; 	/* Accessor privilege level */
	struct uic h_fown;	/* File owner UIC */
#define h_prog	h_fown.u_prog	/* Programmer (member) number */
#define h_proj	h_fown.u_proj	/* Project (group) number */
	ushort	h_fpro; 	/* File protection code */
	ushort	h_rpro; 	/* Record protection code */
	char	h_UU2[4];	/* Ununsed 2 */
	char	h_semk[4];	/* Security mask */
	struct ident h_ident;	/* Ident area */
	char	h_other[300];	/* Map area, access control area, etc */
};

struct homeblock {
	long	H_hblb; 	/* Home block LBN */
	long	H_ahlb; 	/* Alternate home block LBN */
	long	H_ihlb; 	/* Backup index file header LBN */
	char	H_vlev[2];	/* Structure level and version */
	ushort	H_sbcl; 	/* Storage bitmap cluster factor */
	ushort	H_hbvb; 	/* Home block VBN */
	ushort	H_ahvb; 	/* Backup home block VBN */
	ushort	H_ihvb; 	/* Backup index file header VBN */
	ushort	H_ibvb; 	/* Index file bitmap VBN */
	ushort	H_iblb[2];	/* Index file bitmap LBN */
	long	H_fmax; 	/* Maximum number of files */
	ushort	H_ibsz; 	/* Index file bitmap size */
	ushort	H_rsvf; 	/* Number of reserved files */
	ushort	H_dvty; 	/* Disk device type */
	ushort	H_rvn;		/* Relative volume number */
	ushort	H_nvol; 	/* Number of volumes */
	ushort	H_vcha; 	/* Volume characteristics */
	struct uic H_vown;	/* Volume owner UIC */
	long	H_vsmx; 	/* Volume security mask */
	ushort	H_vpro; 	/* Volume protection code */
	ushort	H_dfpr; 	/* Default file protection */
	ushort	H_drpr; 	/* Default record protection */
	ushort	H_chk1; 	/* First checksum */
	char	H_vdat[8];	/* Volume creation date */
	char	H_wisz; 	/* Default window size */
	char	H_lruc; 	/* Directory pre-access limit */
	ushort	H_fiex; 	/* Default file extend */
	char	H_UU1[388];	/* Unused 1 */
	char	H_snam[12];	/* Structure name */
	char	H_indn[12];	/* Volume name */
	char	H_indo[12];	/* Volume owner */
	char	H_indf[12];	/* Format type */
	char	H_UU2[2];	/* Unused 2 */
	ushort	H_chk2; 	/* Second checksum */
} hblock;

struct directory {
	ushort	d_rbc;		/* Record byte count */
	ushort	d_vrlm; 	/* Version limit */
	char	d_flags;	/* Flags */
	char	d_nbc;		/* Name byte count */
	char	d_fname[1];	/* File name string */
};

struct dirval {
	ushort	d_ver;		/* Version number */
	struct fileid d_fid;	/* File ID */
};

#define BUFSIZE 512

#define bit(x)	((01)<<(x))

#define DEV	bit(0)
#define DIR	bit(1)
#define FIL	bit(2)
#define EXT	bit(3)
#define VER	bit(4)

#define DIRBEG	'['
#define DIREND	']'

#define NULLCHR '\0'
#define NULLSTR ""

#define FSMAX	255
#define DEVMAX	10

#define TEXT	0
#define IMGRMS	1
#define IMGFULL 2
#define BINARY	3

#define DISK	0
#define STDOUT	1

#define DFLTMOD TEXT
#define DFLTOUT DISK

char	**av;				/* Global argv */
char	lsflag = 0;			/* Nonzero ==> list directory */
int	xfermode = DFLTMOD;		/* Transfer mode */
int	rmlineno = 0;			/* ntw - remove sos line numbers */
int	outdest = DFLTOUT;		/* Output destination */
char	filspec[FSMAX]; 		/* Full filename string being processed */
int	pflags; 			/* Flags returned by crack() */
char	*dev, *dir, *fil, *typ, *ver;	/* Pointers to cracked filename fields */
char	vmsdev[DEVMAX+6];		/* Special file name for VMS filesystem */
int	vmsfd = -1;			/* File descriptor for reading VMS filesystem */
FILE	*of;				/* Stream pointer for output file */
char	dirfound;			/* Directory found */

struct header	indexh, mfdh, dirh, fileh;	/* File headers for index file, MFD, UFD, and file */


main(argc,argv)
int	argc;
char	**argv;
{
	char	*basename();

	av = argv;

	if ( --argc == 0 )
		usage();

	if ( *basename(*argv) == 'l' )
		++lsflag;

	while ( argc-- ) {
		if ( **++av == '-' )
			options(*argv);
		else
			getvms();
	}
}


usage()
{
	fprintf(stderr,"usage: %s [-t] [-i] [-b] [-d] [-f] [-s] vmsfile\n",*av);
	exit(-1);
}


char *
basename(s)
register char	*s;
{
/* NOTE:  Some versions of UNIX use the name strrchr() rather than rindex() */
	char		*rindex();
	register char	*t;

	if ( (t=rindex(s,'/')) == NULL )
		return(s);
	else
		return(t+1);
}


/*
 *  Process option flags
 */

options()
{
	register char	*p;

	for ( p = *av; *++p; ) {

		switch ( *p ) {

		case 'd':
		case 'f':	outdest = DISK; break;
		case 's':	outdest = STDOUT; break;

		case 't':	xfermode = TEXT; break;
		case 'T':	xfermode = TEXT; rmlineno++; break;	/* ntw */
		case 'i':	xfermode = IMGRMS; break;
		case 'I':	xfermode = IMGFULL; break;
		case 'b':	xfermode = BINARY; break;

		case 'l':	++lsflag; break;
		case 'c':	lsflag = 0; break;

		default:	fprintf(stderr,"Invalid option (%c)\n",*p);

		}
	}
}


/*
 *  Get the next requested file from the VMS filesystem
 */

getvms()
{
	if ( strlen(*av) > FSMAX )
		err0("Filespec too long");
	strcpy(filspec,*av);

	if ( lsflag ) {
		if ( openin() )
			listdir();
	} else {
		if ( openin() && openout() ) {
			copyfile();
			if ( of != stdout )
				fclose(of);
		}
	}
}


/*
 *  Open VMS file for input
 */

openin()
{
	static int	filecnt = 0;
	struct filnam	fn;
	ushort		fnum;
	ushort		search();
	int		gh;

	++filecnt;
	if (crack() == 0)
		return (0);
	if ( pflags&DEV && !openvms(dev) )
		return(0);
#ifdef DFLTDEV
	if ( !(pflags&DEV) && filecnt==1 && !openvms(DFLTDEV) )
		return(0);
#endif
	if ( vmsfd < 0 )
		err0("No device specified");
	if ( pflags&(DEV|DIR) && !finddir() )
		return(0);
	if ( !dirfound )
		err0("No directory specified");
	if ( lsflag ) {
		if ( pflags & (FIL|EXT|VER) )
			err0("Invalid directory specification");
		return(1);
	}
	if ( !(pflags&EXT) )
		typ = NULLSTR;
	if ( !(pflags&VER) )
		ver = NULLSTR;
	if ( !convert(fil,typ,ver,&fn) )
		return(0);
	if ( !(fnum=search(&dirh,&fn)) )
		err0("File does not exist");
	if ( !(gh=gethdr(fnum,&fileh)) )
		err0("Can't get file header for file");
	if ( gh == -1 )
		err0("No access privilege for file");
	return(1);
}


/*
 *  Crack the filename string -- First step in parsing it; just
 *  locates the fields, doesn't do much real validity checking
 */

crack()
{
	register char	*p = filspec;
	register char	*q;

	for ( pflags=0; *p; ) {

		if ( *p == DIRBEG ) {
			if ( pflags & (DIR|FIL|EXT|VER) )
				err0("Bad filename syntax");
			dir = p+1;
			while ( *p != DIREND ) {
				if ( 'a' <= *p && *p <= 'z' )	/* SHOUT the directory */
					*p += 'A' - 'a';	/* name in UPPER CASE */
				if ( *p++ == NULLCHR )
					err0("Bad filename syntax");
			}
			*p++ = NULLCHR;
			pflags |= DIR;
			continue;
		}

		for ( q=p; alphnum(*q); ++q )
			;

		if ( *q == ':' ) {
			if ( pflags&(DEV|DIR|FIL|EXT|VER) )
				err0("Bad filename syntax");
			dev = p;
			pflags |= DEV;
			*q = NULLCHR;
			p = q + 1;
			continue;
		}

		if ( *q == '.' || *q == ';' || *q == NULLCHR ) {

			if ( !(pflags&FIL) ) {
				if ( p == q )
					err0("Filename missing");
				fil = p;
				pflags |= FIL;
				if ( *q == ';' ) {
					typ = NULLSTR;
					pflags |= EXT;
				}
			} else if ( !(pflags&EXT) ) {
				typ = p;
				pflags |= EXT;
			} else if ( !(pflags&VER) ) {
				ver = p;
				pflags |= VER;
			} else
				err0("Bad filename syntax");

			if ( *q == NULLCHR ) {
				if ( !(pflags&EXT) )
					typ = NULLSTR;
				if ( !(pflags&VER) )
					ver = NULLSTR;
				break;
			}
			*q = NULLCHR;
			p = q + 1;
			continue;
		}

		err0("Bad filename syntax");
	}

	return(1);
}


/*
 *  Open a disk containing an VMS filesystem
 */

openvms(devname)
char	*devname;
{
	long	ifhbn;

	if ( strlen(devname) > DEVMAX )
		err1("Device name too long (%s)",devname);
/*
	strcpy(vmsdev,"/dev/");
	if ( strncmp(devname,"disk$",5) == 0 )
		devname += 5;
	if ( strncmp(devname,"vms",3) != 0 )
		strcat(vmsdev,"vms");
	strcat(vmsdev,devname);
	if ( (vmsfd=open(vmsdev,0)) < 0 )
		err1("Can't open %s",vmsdev);
*/
#ifdef VMS
        strcpy(vmsdev,"");
#else
        strcpy(vmsdev,"/dev/");
        if ( strncmp(devname,"disk$",5) == 0 )
                devname += 5;
        if ( strncmp(devname,"vms",3) != 0 )
                strcat(vmsdev,"vms");
#endif
        strcat(vmsdev,devname);
#ifdef VMS
        {int retcode;
         struct dsc$descriptor devd;
         devd.dsc$w_length = strlen(vmsdev);
         devd.dsc$a_pointer = vmsdev;
         devd.dsc$b_class = DSC$K_CLASS_S;
         devd.dsc$b_dtype = DSC$K_DTYPE_T;
         vmsfd = 0;
         retcode = sys$assign(&devd,&vmsfd,0,0);
         if (retcode != SS$_NORMAL) vmsfd = -1;}
#else
        vmsfd=open(vmsdev,0);
#endif
        if ( vmsfd < 0 )
                err1("Can't open %s",vmsdev);

	if ( !getlb(1L,&hblock) )
		err1("Can't read homeblock on %s",vmsdev);

	ifhbn = ((long)hblock.H_iblb[1]<<16) + (long)hblock.H_iblb[0] + hblock.H_ibsz;
	if ( !getlb(ifhbn,(char *)&indexh) )
		err1("Can't read index file header on %s\n",vmsdev);

	if ( !getlb(ifhbn+3,(char *)&mfdh) )
		err1("Can't read mfd header on %s",vmsdev);

	return(1);
}


/*
 *  Locate the directory whose name is pointed to by "dir"
 */

finddir()
{
#define direrr(msg,dirname,ptr) { dirmsg(msg,dirname,ptr); return(0); }
	struct header	*hp = &mfdh;
	register char	*p = dir;
	register char	*q;
	char		*strchr();
	int		nch;
	struct filnam	dirfn;
	ushort		dirfnum;
	ushort		search();
	int		gh;

	do {
		for ( q=p; alphnum(*q); ++q )
			;
		if ( ( *q && *q!='.' ) || (nch=q-p) == 0 || nch > 9 )
			err1("Invalid directory ([%s])",dir);
		strncpy(dirfn.f_nam,p,nch);
		dirfn.f_nam[nch] = '\0';
		strcat(dirfn.f_nam,".DIR");
		dirfn.f_ver = 1;
		if ( !(dirfnum=search(hp,&dirfn)) )
			direrr("Directory [%s] does not exist",dir,q);
		if ( !(gh=gethdr(dirfnum,(hp=(&dirh)))) )
			direrr("Can't get file header for directory [%s]",dir,q);
		if ( gh == -1 )
			direrr("No access privilege for directory [%s]",dir,q);
		p = q + 1;
	} while (*q);
	dirfound = 1;
	return(1);
}


/*
 *  Error accessing a directory
 */

dirmsg(msg,dirname,ptr)
char	*msg;
char	*dirname;
char	*ptr;
{
	char	c;

	c = *ptr;
	*ptr = '\0';
	errmsg(msg,dirname);
	*ptr = c;
}


/*
 *  Convert file name, type, and version number to "struct filnam" format
 */

convert(fl,tp,vr,f)
char		*fl;
char		*tp;
char		*vr;
struct filnam	*f;
{
	register char *p;

/*	if ( strlen(fl) > 9 )
		err0("Filename longer than 9 characters"); */
/*	if ( strlen(tp) > 3 )
		err0("File type longer than 3 characters"); */
/* Kludge-o addition by glenn everhart to handle longer names. Also bumped
   array size. */
	if ( strlen(fl) > 39 )
		err0("Filename longer than 39 characters");
	if ( strlen(tp) > 39 )
		err0("File type longer than 39 characters");
	strcpy(f->f_nam,fl);
	strcat(f->f_nam,".");
	strcat(f->f_nam,tp);
	for ( p=f->f_nam; *p; ++p )		/* This code is needed since */
		if ( 'a' <= *p && *p <= 'z' )	/* VMS loves to SHOUT at you */
			*p += 'A' - 'a';	/* in UPPER CASE all the time */
/* (this is a holdover from the old RAD50 code on RSX and DOS-11) */
	for ( f->f_ver=0; *vr; ) {
		if ( !decimal(*vr) )
			err0("Non-digit in version number");
		f->f_ver *= 10;
		f->f_ver += *vr++ - '0';
	}
	return(1);
}


/*
 *  Search a directory (identified by dhp) for a filename
 */

ushort
search(dhp,fn)
register struct header	*dhp;
register struct filnam	*fn;
{
	int				len;
	int				bod;
	register struct directory	*de;
	struct directory		*getde();
	register struct dirval		*vp;
	register struct dirval		*vplim;

	len = strlen(fn->f_nam);
	for ( bod=1; de=getde(dhp,bod); bod=0 ) {
		if ( de->d_nbc!=len || strncmp(de->d_fname,fn->f_nam,len)!=0 )
			continue;
		vp = (struct dirval *) ( de->d_fname + ((de->d_nbc+1)&0376) );
		if ( !fn->f_ver )
			return(vp->d_fid.f_num);
		for ( vplim=(struct dirval *)((char *)(&de->d_vrlm)+de->d_rbc); vp<vplim; ++vp ) {
			if ( vp->d_ver > fn->f_ver )
				continue;
			if ( vp->d_ver == fn->f_ver )
				return(vp->d_fid.f_num);
			return(0);
		}
		return(0);
	}
	return(0);
}


/*
 *  Open output file
 */

openout()
{
/* used to be dimensioned 14 next. Changed to 84 for LONG names. */
	char	outfile[84];

	if ( outdest == STDOUT ) {
		of = stdout;
		return(1);
	}

	strcpy(outfile,fil);
	strcat(outfile,".");
	strcat(outfile,typ);
	if (okwrite(outfile) == 0		/* ntw */
	||   (of=fopen(outfile,"w")) == NULL )
		err0("Can't open output file");
	return(1);
}


/*
 * see if ok to write/create this file
 * needed because we might be setuid or setgid
 * to get at the special files for disks
 * nb we assume the file is in the working directory
 * always true at the moment;  might neeed more mess in future
 */

int
okwrite(file)
char *file;
{
	if (access(file, 02) == 0)
		return (1);		/* exists and is writeable */
	if (access(file, 0) == 0)
		return (0);		/* exists although not writeable */
	if (index(file, '/') == 0)
		return (0);		/* snh */
	if (access(".", 02) == 0)
		return (1);		/* file doesn't exist and can create it */
	return (0);
}


/*
 *  Copy input file to output destination
 */

copyfile()
{
	long		eofblk;
	register long	block = 0;
	register long	b = 0;
	char		buf[BUFSIZE];
	int		nbytes = BUFSIZE;
	register char	*p;

	if ( xfermode == BINARY )
		err0("Binary mode not yet supported");
	if ( xfermode != IMGFULL )
		eofblk = ( (long)fileh.h_rms.f_heof[0] << 16 ) + fileh.h_rms.f_heof[1];
	while ( getvb(++block,buf,&fileh) ) {
		if ( xfermode == IMGFULL ) {
			if ( fwrite(buf,1,BUFSIZE,of) == BUFSIZE )
				continue;
			err0("write error");
		}
		if ( ++b > eofblk )
			return(1);
		if ( b == eofblk )
			nbytes = fileh.h_rms.f_ffby;
		if ( xfermode == IMGRMS ) {
			if ( fwrite(buf,1,nbytes,of) == nbytes )
				continue;
			err0("write error");
		}
		for ( p=buf; p<buf+nbytes; )
			putch(*p++);
	}
	return(1);
}


/*
 *  Process next character from input file
 *  for text mode
 */

/*
 * possible states of the machine:
 */

#define INIT	0	/* waiting for the beginning of a record */
#define COUNT	1	/* in byte count */
#define LINENO	2	/* in line number */
#define DATA	3	/* in data */
#define NULLPAD	4	/* eating the padding null at the end */

putch(c)
register char	c;
{
	static unsigned	count;
	static int	state = INIT;
	static int	nextstate;
	static int	lnbytes;

	switch (state) {
	case INIT:
		count = (c&0377);
		state = COUNT;
		break;

	case COUNT:
		if ( (count+=((c&0377)<<8)) == 0 ) {
			putc('\n',of);
			state = INIT;
		} else {
			if (rmlineno == 0)
				state = DATA;
			else {
				lnbytes = 0;
				state = LINENO;
			}
			nextstate = INIT;
			if ( count&1 )
				nextstate = NULLPAD;
		}
		break;

	case LINENO:
		if (lnbytes == 0)
			lnbytes++;
		else
			state = DATA;
		if (--count == 0) {
			putc('\n', of);
			state = INIT;
		}
		break;

	case DATA:
		putc(c,of);
		if ( --count == 0 ) {
			state = nextstate;
			putc('\n',of);
		}
		break;

	case NULLPAD:
		state = INIT;
		break;

	default:
		errmsg("internal error in putch");
		abort();
	}
}


/*
 *  List contents of a UFD
 */

listdir()
{
	register int			bod;
	register struct directory	*de;
	struct directory		*getde();
	register struct dirval		*vp;
	register struct dirval		*vplim;

	for ( bod=1; de=getde(&dirh,bod); bod=0 ) {
		vp = (struct dirval *) ( de->d_fname + ((de->d_nbc+1)&0376) );
		vplim = (struct dirval *) ((char *)(&de->d_vrlm)+de->d_rbc);
		for ( ; vp<vplim; ++vp )
			prtfn(de,vp);
	}
}


/*
 *  Write filename to standard output
 */

prtfn(de,vp)
register struct directory	*de;
register struct dirval		*vp;
{
	register char	*p;
	register int	i;

	for ( p=de->d_fname, i=de->d_nbc; i>0; --i )
		putc(*p++,stdout);
	fprintf(stdout,";%d\n",vp->d_ver);
}


/*
 *  Return pointer to next directory entry
 */

struct directory *
getde(dhp,bod)
register struct header	*dhp;
int			bod;
{
#define recsize (*((ushort *)de))
#define STOP	((ushort)0177777)
	static long		vb;
	static long		eofblk;
	static char		*limit;
	static char		dirbuf[BUFSIZE];
	static char		*de;

	if ( bod ) {
		vb = 0;
		eofblk = ( (long)dhp->h_rms.f_heof[0] << 16 ) + dhp->h_rms.f_heof[1];
		limit = &dirbuf[BUFSIZE];
	}
	if ( bod || (de+=(recsize+2))>=limit || recsize==STOP ) {
		if ( ++vb == eofblk )
			limit = &dirbuf[dhp->h_rms.f_ffby];
		if ( !getvb(vb,dirbuf,dhp) || (*((ushort *)dirbuf)) == STOP)
			return((struct directory *)0);
		de = dirbuf;
	}
	if ( de >= limit )
		return((struct directory *)0);
	return((struct directory *)de);
}


/*
 *  Get a file header, given the file number; check access privilege
 */

gethdr(fnum,hp)
register ushort 	fnum;
register struct header	*hp;
{
#define G_DENY	bit(8)
#define W_DENY	bit(12)
	register long	bn;
	int		grp;
	int		ogrp;

	bn = (long)fnum + hblock.H_ibvb + hblock.H_ibsz -1;
	if ( !getvb(bn,(char *)hp,&indexh) )
		return(0);
	if ( !(hp->h_fpro&W_DENY) || !getuid())
		return(1);
	grp = getgid();
	ogrp = 64*(grp/100) + 8*((grp/10)%10) + (grp%10);
#ifdef VMS
        /* the user must have LOGIO priv to run this anyway. */
        return(1);
#endif
	if ( ogrp != hp->h_proj || hp->h_fpro&G_DENY )
		return(-1);
	else
		return(1);
}


/*
 *  Routine to get specified virtual block from a file.  Returns 0
 *  on EOF, 1 otherwise.  Note that vbn is 1-based, not 0-based.
 */

getvb(vbn,buf,hp)
register long		vbn;
char			*buf;
register struct header	*hp;
{
#define WTPMASK 0140000
#define WTP00	0000000
#define WTP01	0040000
#define WTP10	0100000
#define WTP11	0140000
	register ushort 	*rp;
	register long		block;
	register ushort 	*limit;
	register ushort 	wtype;
	register long		lbn;
	register long		size;
	ushort			getsize();

	rp = (ushort *)hp + (hp->h_mpof&0377);
	block = 1;
	limit = rp + (hp->h_use & 0377);		/* ntw */
	while ( rp < limit && vbn >= ( block + (size=getsize(rp)) ) ) {
		wtype = (*rp) & WTPMASK;
		switch (wtype) {
			case WTP00:	rp += 1; break;
			case WTP01:	rp += 2; break;
			case WTP10:	rp += 3; break;
			case WTP11:	rp += 4; break;
		}
		block += size;
	}
	if ( rp >= limit )
		return(0);
	lbn = lbnbase(rp) + vbn - block;
	return(getlb(lbn,buf));
}


/*
 *  Return number of blocks mapped by the current window
 */

ushort
getsize(rp)
register ushort *rp;
{
	register ushort wtype;

	wtype = (*rp) & WTPMASK;
	switch (wtype) {
		case WTP00:	return(0);
		case WTP01:	return(((*((char *)rp))&0377)+1);
		case WTP10:	return(((*rp)&037777)+1);
		case WTP11:	return(((((long)(*rp)&037777)<<16)+rp[1])+1);
	}
}


/*
 *  Return base lbn mapped by the current window
 */

long
lbnbase(rp)
register ushort *rp;
{
	register ushort wtype;

	wtype = (*rp)&WTPMASK;
	switch ( wtype ) {
		case WTP00:	return(0L);
		case WTP01:	return(((((char *)rp)[1]&077L)<<16)+rp[1]);
		case WTP10:	return((((long)rp[2])<<16)+(long)rp[1]);
		case WTP11:	return((((long)rp[3])<<16)+(long)rp[2]);
	}
}


/*
 *  Get block from the filesystem, given the logical block number
 */

getlb(lbn,buf)
long    lbn;
char    *buf;
{
        if ( lbn == 0L )
                err0("Bad block in file");
#ifdef VMS
        {int retcode;
         short iosb[4];
         retcode = sys$qiow(0,vmsfd,IO$_READLBLK,&iosb,0,0,
                            buf,BUFSIZ,lbn,
                            0,0,0);
         if (retcode != SS$_NORMAL) err1("QIO error %d",retcode);
         if (iosb[0] != SS$_NORMAL) err1("READLBLK error %d",iosb[0]);}
#else
        if ( lseek(vmsfd,BUFSIZE*lbn,0) == -1L ||  read(vmsfd,buf,BUFSIZE) != BUFSIZE )
                err0("Read error");
#endif
        return(1);
}


/*
 *  Issue an error message
 */

/* VARARGS1 */

errmsg(msg,arg)
char	*msg;
int	arg;
{
	fprintf(stderr,"%s -- ",*av);
	fprintf(stderr,msg,arg);
	fprintf(stderr,"\n");
}
