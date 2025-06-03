static char rcsid[] = "$Header: /usr/people/sam/tiff/libtiff/RCS/tif_fax3.c,v 1.51 91/08/22 18:17:45 sam Exp $";

 

 
 

 

 
 

 

 

 
 
 

 
 
 
 

 

#ident	"@(#)/usr/include/stdio.h.sl 1.4 SVR4.2 05/28/92 1866 USL"

typedef unsigned int 	size_t;

typedef long	fpos_t;



			 











typedef struct	 
{
	int		_cnt;	 
	unsigned char	*_ptr;	 
	unsigned char	*_base;	 
	unsigned char	_flag;	 
	unsigned char	_file;	 
} FILE;

extern FILE		__iob[ 60 ] ;
extern FILE		*_lastbuf;
extern unsigned char 	*_bufendtab[];
extern unsigned char	 _sibuf[], _sobuf[];


extern int	remove(const char *);
extern int	rename(const char *, const char *);
extern FILE	*tmpfile(void);
extern char	*tmpnam(char *);
extern int	fclose(FILE *);
extern int	fflush(FILE *);
extern FILE	*fopen(const char *, const char *);
extern FILE	*freopen(const char *, const char *, FILE *);
extern void	setbuf(FILE *, char *);
extern int	setvbuf(FILE *, char *, int, size_t);
 
extern int	fprintf(FILE *, const char *, ...);
 
extern int	fscanf(FILE *, const char *, ...);
 
extern int	printf(const char *, ...);
 
extern int	scanf(const char *, ...);
 
extern int	sprintf(char *, const char *, ...);
 
extern int	sscanf(const char *, const char *, ...);


extern int	vfprintf(FILE *, const char *,  void * ) ;
extern int	vprintf(const char *,  void * ) ;
extern int	vsprintf(char *, const char *,  void * ) ;
extern int	fgetc(FILE *);
extern char	*fgets(char *, int, FILE *); 
extern int	fputc(int, FILE *);
extern int	fputs(const char *, FILE *);
extern int	getc(FILE *);
extern int	getchar(void);
extern char	*gets(char *);
extern int	putc(int, FILE *);
extern int	putchar(int);
extern int	puts(const char *);
extern int	ungetc(int, FILE *);
extern size_t	fread(void *, size_t, size_t, FILE *);
#pragma	int_to_unsigned fread
extern size_t	fwrite(const void *, size_t, size_t, FILE *);
#pragma	int_to_unsigned fwrite
extern int	fgetpos(FILE *, fpos_t *);
extern int	fseek(FILE *, long, int);
extern int	fsetpos(FILE *, const fpos_t *);
extern long	ftell(FILE *);
extern void	rewind(FILE *);
extern void	clearerr(FILE *);
extern int	feof(FILE *);
extern int	ferror(FILE *);
extern void	perror(const char *);

extern int	__filbuf(FILE *);
extern int	__flsbuf(int, FILE *);



extern FILE	*fdopen(int, const char *);
extern char	*ctermid(char *);
extern int	fileno(FILE *);




extern FILE	*popen(const char *, const char *);
extern char	*cuserid(char *);
extern char	*tempnam(const char *, const char *);
extern int	getw(FILE *);
extern int	putw(int, FILE *);
extern int	pclose(FILE *);
extern int	system(const char *);


 
 
 

 
 
 
 


#ident	"@(#)/usr/include/sys/types.h.sl 1.7 SVR4.2 07/07/92 28935 USL"
#ident	"$Header: $"

typedef char *		addr_t; 	 
typedef char *		caddr_t;	 
typedef long		daddr_t;	 
typedef char *		faddr_t;	 
typedef long		off_t;		 
typedef short		cnt_t;		 
typedef unsigned long	paddr_t;	 
typedef unsigned char	use_t;		 
typedef short		sysid_t;	 
typedef short		index_t;	 
typedef short		lock_t;		 
typedef	unsigned short	sel_t;		 
typedef unsigned long	k_sigset_t;	 
typedef unsigned long	k_fltset_t;	 

typedef struct _label { int val[6]; } label_t;	 

typedef enum boolean { B_FALSE, B_TRUE } boolean_t;

 

typedef unsigned char	uchar_t;
typedef unsigned short	ushort_t;
typedef unsigned int	uint_t;
typedef unsigned long	ulong_t;

 
typedef long		id_t;		 

 

typedef unsigned long	pvec_t;		 

 

typedef unsigned long	lid_t;		 
typedef lid_t		level_t;	 

 


typedef unsigned long	adtemask_t[ 8 ] ;  


 

typedef unsigned long	major_t;	 
typedef unsigned long	minor_t;	 

 

typedef unsigned short	o_mode_t;	 
typedef short		o_dev_t;	 
typedef unsigned short	o_uid_t;	 
typedef o_uid_t		o_gid_t;	 
typedef short		o_nlink_t;	 
typedef short		o_pid_t;	 
typedef unsigned short	o_ino_t;	 

 

typedef int		key_t;		 
typedef unsigned long	mode_t;		 
typedef long		uid_t;		 
typedef uid_t		gid_t;		 
typedef unsigned long	nlink_t;	 
typedef unsigned long	dev_t;		 
typedef unsigned long	ino_t;		 
typedef long		pid_t;		 


typedef int		ssize_t;	 

typedef long		time_t;		 

typedef long		clock_t;	 


typedef struct { int r[1]; } * physadr;
typedef unsigned char	unchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef int		spl_t;


 




 
typedef long		hostid_t;

 

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
typedef struct _quad { long val[2]; } quad;	 

 


 
 
 

 
 
 
 


#ident	"@(#)/usr/include/sys/select.h.sl 1.2 SVR4.2 01/31/92 26057 USL"
#ident	"$Header: $"

 


typedef	long	fd_mask;

typedef	struct fd_set {
	fd_mask	fds_bits[ ( ( ( 1024 ) + ( ( ( sizeof ( fd_mask ) * 8 ) ) - 1 ) ) / ( ( sizeof ( fd_mask ) * 8 ) ) ) ] ;
} fd_set;





 




 
 
 

 
 
 
 


#ident	"@(#)/usr/include/fcntl.h.sl 1.3 SVR4.2 05/28/92 64404 USL"

 
 
 

 
 
 
 

 
 
 

 
 
 
 


#ident	"@(#)/usr/include/sys/fcntl.h.sl 1.5 SVR4.2 06/18/92 64885 USL"


 
 
 

 
 
 
 



 

 

 

 








 


typedef struct flock {
	short	l_type;		 
	short	l_whence;	 
	off_t	l_start;	 
	off_t	l_len;		 
	long	l_sysid;	 
        pid_t	l_pid;		 
	long	l_pad[4];	 
} flock_t;



 

 




extern int fcntl(int, int, ...);
extern int open(const char *, int, ...);
extern int creat(const char *, mode_t);



 
 
 
 

 
 
 
 


#ident	"@(#)/usr/include/unistd.h.sl 1.4 SVR4.2 05/28/92 14284 USL"


 




 

 

 
 
 

 
 
 
 


#ident	"@(#)/usr/include/sys/unistd.h.sl 1.3 SVR4.2 05/08/92 1220 USL"
#ident	"$Header: $"

 


 


 




 




 





 

 


 
 
 

 
 
 
 


extern int access(const char *, int);
extern int acct(const char *);
extern unsigned alarm(unsigned);
extern int brk(void *);
extern int chdir(const char *);
extern int chown(const char *, uid_t, gid_t);
extern int chroot(const char *);
extern int close(int);
extern char *ctermid(char *);
extern char *cuserid(char *);
extern int dup(int);
extern int dup2(int, int);
extern int execl(const char *, const char *, ...);
extern int execle(const char *, const char *, ...);
extern int execlp(const char *, const char *, ...);
extern int execv(const char *, char *const *);
extern int execve(const char *, char *const *, char *const *);
extern int execvp(const char *, char *const *);
extern void exit(int);
extern void _exit(int);
extern int fattach(int, const char *);
extern int fchdir(int);
extern int fchown(int,uid_t, gid_t);
extern int fdetach(const char *);
extern pid_t fork(void);
extern long fpathconf(int, int);
extern int fsync(int);
extern int ftruncate(int, off_t);
extern char *getcwd(char *, size_t);
extern gid_t getegid(void);
extern uid_t geteuid(void);
extern gid_t getgid(void);
extern int getgroups(int, gid_t *);
extern char *getlogin(void);
extern pid_t getpgid(pid_t);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern pid_t getpgrp(void);
char *gettxt(const char *, const char *);
extern pid_t getsid(pid_t);
extern uid_t getuid(void);
extern int ioctl(int, int, ...);
extern int isatty(int);
extern int link(const char *, const char *);
extern int lchown(const char *, uid_t, gid_t);
extern int lockf(int, int, long);
extern off_t lseek(int, off_t, int);
extern int mincore(caddr_t, size_t, char *);
extern int nice(int);
extern long pathconf(const char *, int);
extern int pause(void);
extern int pipe(int *);
extern void profil(unsigned short *, unsigned int, unsigned int, unsigned int);
extern int ptrace(int, pid_t, int, int);
extern ssize_t read(int, void *, size_t);
extern int readlink(const char *, void *, int);
extern int rename(const char *, const char *);
extern int rmdir(const char *);
extern void *sbrk(int);
extern int setgid(gid_t);
extern int setgroups(int, const gid_t *);
extern int setpgid(pid_t, pid_t);
extern pid_t setpgrp(void);
extern pid_t setsid(void);
extern int setuid(uid_t);
extern unsigned sleep(unsigned);
extern int stime(const time_t *);
extern int symlink(const char *, const char *);
extern void sync(void);
extern long sysconf(int);
extern pid_t tcgetpgrp(int);
extern int tcsetpgrp(int, pid_t);
extern int truncate(const char *, off_t);
extern char *ttyname(int);
extern int unlink(const char *);
extern pid_t vfork(void);
extern ssize_t write(int, const void *, size_t);



 

 

 

 
extern	long TIFFGetFileSize(int fd);


 
extern	long lseek();

 

extern	char *malloc();
extern	char *realloc();

 
typedef double dblparam_t;

 

 
 
 

 
 
 
 


#ident	"@(#)/usr/include/varargs.h.sl 1.3 SVR4.2 05/28/92 35255 USL"



typedef char *va_list;



 

 

 


typedef	struct {
	unsigned short tiff_magic;	 
	unsigned short tiff_version;	 
	unsigned long  tiff_diroff;	 
} TIFFHeader;

 
typedef	struct {
	unsigned short tdir_tag;	 
	unsigned short tdir_type;	 
	unsigned long  tdir_count;	 
	unsigned long  tdir_offset;	 
} TIFFDirEntry;

typedef	enum {
	TIFF_NOTYPE = 0,	 
	TIFF_BYTE = 1,		 
	TIFF_ASCII = 2,		 
	TIFF_SHORT = 3,		 
	TIFF_LONG = 4,		 
	TIFF_RATIONAL = 5	 
} TIFFDataType;

 
 
 

 
typedef	struct {
	u_long	td_imagewidth, td_imagelength, td_imagedepth;
	u_long	td_tilewidth, td_tilelength, td_tiledepth;
	u_short	td_subfiletype;
	u_short	td_bitspersample;
	u_short	td_datatype;
	u_short	td_compression;
	u_short	td_photometric;
	u_short	td_threshholding;
	u_short	td_fillorder;
	u_short	td_orientation;
	u_short	td_samplesperpixel;
	u_short	td_predictor;
	u_long	td_rowsperstrip;
	u_long	td_minsamplevalue, td_maxsamplevalue;	 
	float	td_xresolution, td_yresolution;
	u_short	td_resolutionunit;
	u_short	td_planarconfig;
	float	td_xposition, td_yposition;
	u_long	td_group3options;
	u_long	td_group4options;
	u_short	td_pagenumber[2];
	u_short	td_grayresponseunit;
	u_short	td_colorresponseunit;
	u_short	td_matteing;
	u_short	td_inkset;
	u_short	td_cleanfaxdata;
	u_short	td_badfaxrun;
	u_long	td_badfaxlines;
	u_short	*td_grayresponsecurve;
	u_short	*td_redresponsecurve;
	u_short	*td_greenresponsecurve;
	u_short	*td_blueresponsecurve;
	u_short	*td_redcolormap;
	u_short	*td_greencolormap;
	u_short	*td_bluecolormap;
	char	*td_documentname;
	char	*td_artist;
	char	*td_datetime;
	char	*td_hostcomputer;
	char	*td_imagedescription;
	char	*td_make;
	char	*td_model;
	char	*td_software;
	char	*td_pagename;
	u_long	td_fieldsset[2];	 
	u_long	td_stripsperimage;
	u_long	td_nstrips;		 
	u_long	*td_stripoffset;
	u_long	*td_stripbytecount;
} TIFFDirectory;

 
 
 


struct tiff {
	char	*tif_name;		 
	short	tif_fd;			 
	short	tif_mode;		 
	char	tif_fillorder;		 
	char	tif_options;		 
	short	tif_flags;
	long	tif_diroff;		 
	long	tif_nextdiroff;		 
	TIFFDirectory tif_dir;		 
	TIFFHeader tif_header;		 
	int	tif_typeshift[6];	 
	long	tif_typemask[6];	 
	long	tif_row;		 
	int	tif_curstrip;		 
	long	tif_curoff;		 
 
	long 	tif_col;		 
	int 	tif_curtile;		 
	long 	tif_tilesize;		 
 
	int	(*tif_predecode)();	 
	int	(*tif_preencode)();	 
	int	(*tif_postencode)();	 
	int	(*tif_decoderow)();	 
	int	(*tif_encoderow)();	 
	int	(*tif_decodestrip)();	 
	int	(*tif_encodestrip)();	 
	int	(*tif_decodetile)();	 
	int	(*tif_encodetile)();	 
	int	(*tif_close)();		 
	int	(*tif_seek)();		 
	int	(*tif_cleanup)();	 
	char	*tif_data;		 
 
	int	tif_scanlinesize;	 
	int	tif_scanlineskew;	 
	char	*tif_rawdata;		 
	long	tif_rawdatasize;	 
	char	*tif_rawcp;		 
	long	tif_rawcc;		 
};


 

 

 


 
 

 


 
typedef	struct tiff TIFF;


 

extern unsigned char TIFFBitRevTable[256];
extern unsigned char TIFFNoBitRevTable[256];

 
 
 

 
 
 
 

 
 
 
 

 
 
 
 


#ident	"@(#)/usr/include/stdarg.h.sl 1.3 SVR4.2 05/28/92 17726 USL"



typedef  void *	va_list ;





extern void  ;




typedef	void (*TIFFErrorHandler)(char* module, char* fmt, va_list ap);

extern	void TIFFClose(TIFF*);
extern	int TIFFFlush(TIFF*);
extern	int TIFFFlushData(TIFF*);
extern	int TIFFGetField(TIFF*, int, ...);
extern	int TIFFVGetField(TIFF*, int, va_list);
extern	int TIFFReadDirectory(TIFF*);
extern	int TIFFScanlineSize(TIFF*);
extern	unsigned long TIFFStripSize(TIFF*);
extern	unsigned long TIFFTileRowSize(TIFF*);
extern	unsigned long TIFFTileSize(TIFF*);
extern	int TIFFFileno(TIFF*);
extern	int TIFFGetMode(TIFF*);
extern	int TIFFIsTiled(TIFF*);
extern	long TIFFCurrentRow(TIFF*);
extern	int TIFFCurrentStrip(TIFF*);
extern	int TIFFCurrentTile(TIFF*);
extern	int TIFFReadBufferSetup(TIFF*, char*, unsigned);
extern	int TIFFSetDirectory(TIFF*, int);
extern	int TIFFSetField(TIFF*, int, ...);
extern	int TIFFVSetField(TIFF*, int, va_list);
extern	int TIFFWriteDirectory(TIFF *);
extern	TIFF* TIFFOpen(char*, char*);
extern	TIFF* TIFFFdOpen(int, char*, char*);
extern	char* TIFFFileName(TIFF*);
extern	void TIFFError(char*, char*, ...);
extern	TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler handler);
extern	void TIFFWarning(char*, char*, ...);
extern	TIFFErrorHandler TIFFSetWarningHandler(TIFFErrorHandler handler);
extern	void TIFFPrintDirectory(TIFF*, FILE*, long);
extern	int TIFFReadScanline(TIFF*, unsigned char*, unsigned, unsigned);
extern	int TIFFWriteScanline(TIFF*, unsigned char*, unsigned, unsigned);
extern	unsigned int TIFFComputeTile(TIFF*, unsigned long, unsigned long, unsigned int, unsigned long);
extern	int TIFFCheckTile(TIFF*, unsigned long, unsigned long, unsigned long, unsigned);
extern	unsigned int TIFFNumberOfTiles(TIFF*);
extern	int TIFFReadTile(TIFF*, unsigned char*, unsigned long, unsigned long, unsigned long, unsigned);
extern	unsigned int TIFFComputeStrip(TIFF*, unsigned long, unsigned int);
extern	unsigned int TIFFNumberOfStrips(TIFF*);
extern	int TIFFReadEncodedStrip(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFReadRawStrip(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFReadEncodedTile(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFReadRawTile(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFWriteEncodedStrip(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFWriteRawStrip(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFWriteEncodedTile(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFWriteRawTile(TIFF*, unsigned, unsigned char*, unsigned);
extern	int TIFFSwabShort(unsigned short *);
extern	int TIFFSwabLong(unsigned long *);
extern	int TIFFSwabArrayOfShort(unsigned short *, int);
extern	int TIFFSwabArrayOfLong(unsigned long *, int);
extern	int TIFFReverseBits(unsigned char *, int);

 

extern	int TIFFNoEncode(TIFF*, u_char*, int, u_int);
extern	int TIFFNoDecode(TIFF*, u_char*, int, u_int);
 
 
 

 
 
 
 

 
 
 
 

 
 
 
 

#ident	"@(#)/usr/include/assert.h.sl 1.4 SVR4.2 05/28/92 25177 USL"


extern void __assert(const char *, const char *, int);


 

 

 
 

 
typedef struct {
	short	data;			 
	short	bit;			 
	short	white;			 
	u_long	rowbytes;		 
	u_long	rowpixels;		 
	enum { 				 
	    G3_1D,			 
	    G3_2D			 
	} tag;
	u_char	*bitmap;		 
	u_char	*refline;		 
} Fax3BaseState;

 
extern	int Fax3Decode2DRow(TIFF*, u_char *, int);
extern	int Fax3Encode2DRow(TIFF*, u_char *, u_char *, int);
extern	void Fax3PutEOL(TIFF*);

 

 

 
typedef struct tableentry {
    unsigned short length;	 
    unsigned short code;	 
    short	runlen;		 
} tableentry;


 

 
tableentry TIFFFaxWhiteCodes[] = {
    { 8, 0x35, 0 },	 
    { 6, 0x7, 1 },	 
    { 4, 0x7, 2 },	 
    { 4, 0x8, 3 },	 
    { 4, 0xB, 4 },	 
    { 4, 0xC, 5 },	 
    { 4, 0xE, 6 },	 
    { 4, 0xF, 7 },	 
    { 5, 0x13, 8 },	 
    { 5, 0x14, 9 },	 
    { 5, 0x7, 10 },	 
    { 5, 0x8, 11 },	 
    { 6, 0x8, 12 },	 
    { 6, 0x3, 13 },	 
    { 6, 0x34, 14 },	 
    { 6, 0x35, 15 },	 
    { 6, 0x2A, 16 },	 
    { 6, 0x2B, 17 },	 
    { 7, 0x27, 18 },	 
    { 7, 0xC, 19 },	 
    { 7, 0x8, 20 },	 
    { 7, 0x17, 21 },	 
    { 7, 0x3, 22 },	 
    { 7, 0x4, 23 },	 
    { 7, 0x28, 24 },	 
    { 7, 0x2B, 25 },	 
    { 7, 0x13, 26 },	 
    { 7, 0x24, 27 },	 
    { 7, 0x18, 28 },	 
    { 8, 0x2, 29 },	 
    { 8, 0x3, 30 },	 
    { 8, 0x1A, 31 },	 
    { 8, 0x1B, 32 },	 
    { 8, 0x12, 33 },	 
    { 8, 0x13, 34 },	 
    { 8, 0x14, 35 },	 
    { 8, 0x15, 36 },	 
    { 8, 0x16, 37 },	 
    { 8, 0x17, 38 },	 
    { 8, 0x28, 39 },	 
    { 8, 0x29, 40 },	 
    { 8, 0x2A, 41 },	 
    { 8, 0x2B, 42 },	 
    { 8, 0x2C, 43 },	 
    { 8, 0x2D, 44 },	 
    { 8, 0x4, 45 },	 
    { 8, 0x5, 46 },	 
    { 8, 0xA, 47 },	 
    { 8, 0xB, 48 },	 
    { 8, 0x52, 49 },	 
    { 8, 0x53, 50 },	 
    { 8, 0x54, 51 },	 
    { 8, 0x55, 52 },	 
    { 8, 0x24, 53 },	 
    { 8, 0x25, 54 },	 
    { 8, 0x58, 55 },	 
    { 8, 0x59, 56 },	 
    { 8, 0x5A, 57 },	 
    { 8, 0x5B, 58 },	 
    { 8, 0x4A, 59 },	 
    { 8, 0x4B, 60 },	 
    { 8, 0x32, 61 },	 
    { 8, 0x33, 62 },	 
    { 8, 0x34, 63 },	 
    { 5, 0x1B, 64 },	 
    { 5, 0x12, 128 },	 
    { 6, 0x17, 192 },	 
    { 7, 0x37, 256 },	 
    { 8, 0x36, 320 },	 
    { 8, 0x37, 384 },	 
    { 8, 0x64, 448 },	 
    { 8, 0x65, 512 },	 
    { 8, 0x68, 576 },	 
    { 8, 0x67, 640 },	 
    { 9, 0xCC, 704 },	 
    { 9, 0xCD, 768 },	 
    { 9, 0xD2, 832 },	 
    { 9, 0xD3, 896 },	 
    { 9, 0xD4, 960 },	 
    { 9, 0xD5, 1024 },	 
    { 9, 0xD6, 1088 },	 
    { 9, 0xD7, 1152 },	 
    { 9, 0xD8, 1216 },	 
    { 9, 0xD9, 1280 },	 
    { 9, 0xDA, 1344 },	 
    { 9, 0xDB, 1408 },	 
    { 9, 0x98, 1472 },	 
    { 9, 0x99, 1536 },	 
    { 9, 0x9A, 1600 },	 
    { 6, 0x18, 1664 },	 
    { 9, 0x9B, 1728 },	 
    { 11, 0x8, 1792 },	 
    { 11, 0xC, 1856 },	 
    { 11, 0xD, 1920 },	 
    { 12, 0x12, 1984 },	 
    { 12, 0x13, 2048 },	 
    { 12, 0x14, 2112 },	 
    { 12, 0x15, 2176 },	 
    { 12, 0x16, 2240 },	 
    { 12, 0x17, 2304 },	 
    { 12, 0x1C, 2368 },	 
    { 12, 0x1D, 2432 },	 
    { 12, 0x1E, 2496 },	 
    { 12, 0x1F, 2560 },	 
    { 12, 0x1,  - 3 } ,	 
    { 9, 0x1,  - 1 } ,	 
    { 10, 0x1,  - 1 } ,	 
    { 11, 0x1,  - 1 } ,	 
    { 12, 0x0,  - 1 } ,	 
};

tableentry TIFFFaxBlackCodes[] = {
    { 10, 0x37, 0 },	 
    { 3, 0x2, 1 },	 
    { 2, 0x3, 2 },	 
    { 2, 0x2, 3 },	 
    { 3, 0x3, 4 },	 
    { 4, 0x3, 5 },	 
    { 4, 0x2, 6 },	 
    { 5, 0x3, 7 },	 
    { 6, 0x5, 8 },	 
    { 6, 0x4, 9 },	 
    { 7, 0x4, 10 },	 
    { 7, 0x5, 11 },	 
    { 7, 0x7, 12 },	 
    { 8, 0x4, 13 },	 
    { 8, 0x7, 14 },	 
    { 9, 0x18, 15 },	 
    { 10, 0x17, 16 },	 
    { 10, 0x18, 17 },	 
    { 10, 0x8, 18 },	 
    { 11, 0x67, 19 },	 
    { 11, 0x68, 20 },	 
    { 11, 0x6C, 21 },	 
    { 11, 0x37, 22 },	 
    { 11, 0x28, 23 },	 
    { 11, 0x17, 24 },	 
    { 11, 0x18, 25 },	 
    { 12, 0xCA, 26 },	 
    { 12, 0xCB, 27 },	 
    { 12, 0xCC, 28 },	 
    { 12, 0xCD, 29 },	 
    { 12, 0x68, 30 },	 
    { 12, 0x69, 31 },	 
    { 12, 0x6A, 32 },	 
    { 12, 0x6B, 33 },	 
    { 12, 0xD2, 34 },	 
    { 12, 0xD3, 35 },	 
    { 12, 0xD4, 36 },	 
    { 12, 0xD5, 37 },	 
    { 12, 0xD6, 38 },	 
    { 12, 0xD7, 39 },	 
    { 12, 0x6C, 40 },	 
    { 12, 0x6D, 41 },	 
    { 12, 0xDA, 42 },	 
    { 12, 0xDB, 43 },	 
    { 12, 0x54, 44 },	 
    { 12, 0x55, 45 },	 
    { 12, 0x56, 46 },	 
    { 12, 0x57, 47 },	 
    { 12, 0x64, 48 },	 
    { 12, 0x65, 49 },	 
    { 12, 0x52, 50 },	 
    { 12, 0x53, 51 },	 
    { 12, 0x24, 52 },	 
    { 12, 0x37, 53 },	 
    { 12, 0x38, 54 },	 
    { 12, 0x27, 55 },	 
    { 12, 0x28, 56 },	 
    { 12, 0x58, 57 },	 
    { 12, 0x59, 58 },	 
    { 12, 0x2B, 59 },	 
    { 12, 0x2C, 60 },	 
    { 12, 0x5A, 61 },	 
    { 12, 0x66, 62 },	 
    { 12, 0x67, 63 },	 
    { 10, 0xF, 64 },	 
    { 12, 0xC8, 128 },	 
    { 12, 0xC9, 192 },	 
    { 12, 0x5B, 256 },	 
    { 12, 0x33, 320 },	 
    { 12, 0x34, 384 },	 
    { 12, 0x35, 448 },	 
    { 13, 0x6C, 512 },	 
    { 13, 0x6D, 576 },	 
    { 13, 0x4A, 640 },	 
    { 13, 0x4B, 704 },	 
    { 13, 0x4C, 768 },	 
    { 13, 0x4D, 832 },	 
    { 13, 0x72, 896 },	 
    { 13, 0x73, 960 },	 
    { 13, 0x74, 1024 },	 
    { 13, 0x75, 1088 },	 
    { 13, 0x76, 1152 },	 
    { 13, 0x77, 1216 },	 
    { 13, 0x52, 1280 },	 
    { 13, 0x53, 1344 },	 
    { 13, 0x54, 1408 },	 
    { 13, 0x55, 1472 },	 
    { 13, 0x5A, 1536 },	 
    { 13, 0x5B, 1600 },	 
    { 13, 0x64, 1664 },	 
    { 13, 0x65, 1728 },	 
    { 11, 0x8, 1792 },	 
    { 11, 0xC, 1856 },	 
    { 11, 0xD, 1920 },	 
    { 12, 0x12, 1984 },	 
    { 12, 0x13, 2048 },	 
    { 12, 0x14, 2112 },	 
    { 12, 0x15, 2176 },	 
    { 12, 0x16, 2240 },	 
    { 12, 0x17, 2304 },	 
    { 12, 0x1C, 2368 },	 
    { 12, 0x1D, 2432 },	 
    { 12, 0x1E, 2496 },	 
    { 12, 0x1F, 2560 },	 
    { 12, 0x1,  - 3 } ,	 
    { 9, 0x1,  - 1 } ,	 
    { 10, 0x1,  - 1 } ,	 
    { 11, 0x1,  - 1 } ,	 
    { 12, 0x0,  - 1 } ,	 
};
 

 


 
 
 
 
 

u_char	TIFFFax2DMode[20][256] = {
  {
    12,12,11, 0, 3, 3, 9, 9, 4, 4, 4, 4, 8, 8, 8, 8,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
},
  {
    12, 0, 3, 9, 4, 4, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
    12, 0, 3, 9, 4, 4, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
},
  {
     0, 0, 4, 8, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 4, 8, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 4, 8, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 4, 8, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
},
  {
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 0, 1, 1, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
},
  {
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     0, 1, 2, 2, 5, 5, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
},
  {
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
     0, 2, 5, 7, 6, 6, 6, 6, 0, 2, 5, 7, 6, 6, 6, 6,  
},
  {
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
     0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6, 0, 0, 6, 6,  
},
  {
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
     0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6,  
},
  {
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
},
  {
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
},
  {
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
},
  {
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
},
  {
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,10,10,10,10,10,10,10,10,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
},
  {
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
},
  {
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
},
  {
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
    12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,10,  
     3, 3, 3, 3, 3, 3, 3, 3, 9, 9, 9, 9, 9, 9, 9, 9,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
},
  {
    12,12,12,12,11,11,11, 0, 3, 3, 3, 3, 9, 9, 9, 9,  
     4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
},
  {
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
}
};
u_char	TIFFFax2DNextState[20][256] = {
  {
     7, 7, 0, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     0, 9, 0, 0, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 9, 0, 0, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
    10,11, 0, 0, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
    10,11, 0, 0, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
    10,11, 0, 0, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
    10,11, 0, 0, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
},
  {
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    12,13, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
},
  {
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
    14, 0, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5, 5,  
},
  {
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
    15, 0, 0, 0, 6, 6, 6, 6,15, 0, 0, 0, 6, 6, 6, 6,  
},
  {
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
    16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,16,17, 7, 7,  
},
  {
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
    18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,18, 0,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 0, 0,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     6, 6, 6, 6, 7, 7, 0,19, 6, 6, 6, 6, 6, 6, 6, 6,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
}
};
u_char	TIFFFaxUncompAction[20][256] = {
  {
     0, 9, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     0, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
     0, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
     0, 4, 3, 3, 2, 2, 2, 2, 0, 4, 3, 3, 2, 2, 2, 2,  
},
  {
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
     0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2, 0, 3, 2, 2,  
},
  {
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
     0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,  
},
  {
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
},
  {
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
},
  {
    14,14,14,14,14,14,14,14,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
},
  {
    14,14,14,14,12,12,12,12,11,11,11,11,11,11,11,11,  
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
},
  {
    14,14,12,12,11,11,11,11,10,10,10,10,10,10,10,10,  
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
},
  {
    14,12,11,11,10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9,  
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
},
  {
     0,11,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8,  
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
},
  {
     0,10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7,  
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
},
  {
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
},
  {
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,  
},
  {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
},
  {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
}
};
u_char	TIFFFaxUncompNextState[20][256] = {
  {
     8, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     9, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     9, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
},
  {
    10, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
    10, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
    10, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
    10, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
},
  {
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
    11, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
},
  {
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
    12, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
},
  {
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
    13, 0, 7, 7, 6, 6, 6, 6,13, 0, 7, 7, 6, 6, 6, 6,  
},
  {
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
    14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,14, 0, 7, 7,  
},
  {
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
    15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,15, 0,  
},
  {
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     0, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
    16, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
    17, 0, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,  
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  
},
  {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
},
  {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
}
};
u_char	TIFFFax1DAction[230][256] = {
  {
      0,  0, 31, 32, 47, 48, 24, 24, 25, 25, 49, 50, 15, 15,  
     15, 15, 22, 22, 35, 36, 37, 38, 39, 40, 21, 21, 33, 34,  
      3,  3,  3,  3, 14, 14, 14, 14, 55, 56, 28, 28, 41, 42,  
     43, 44, 45, 46, 23, 23, 30, 30, 63, 64, 65,  2, 70, 71,  
     12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13,  
     13, 13, 29, 29, 61, 62,  0,  0, 20, 20, 26, 26, 51, 52,  
     53, 54, 27, 27, 57, 58, 59, 60, 68, 68, 68, 68, 91, 91,  
     91, 91, 72, 73,  0, 75, 74,  0,  0,  0,  0,  0, 69, 69,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5, 67, 67, 67, 67, 67, 67, 67, 67, 10, 10,  
     10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11,  
     18, 18, 18, 18, 19, 19, 19, 19,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 16, 16,  
     16, 16, 17, 17, 17, 17, 66, 66, 66, 66, 66, 66, 66, 66,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9
},
  {
      0,  0,  0, 24, 25,  0, 15, 15, 22,  0,  0,  0, 21,  0,  
      3,  3, 14, 14,  0, 28,  0,  0,  0, 23, 30,  0,  0,  0,  
     12, 12, 12, 12, 13, 13, 13, 13, 29,  0,  0, 20, 26,  0,  
      0, 27,  0,  0, 68, 68, 91, 91,  0,  0,  0,  0,  0, 69,  
      4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  
      5,  5, 67, 67, 67, 67, 10, 10, 10, 10, 11, 11, 11, 11,  
     18, 18, 19, 19,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  
      7,  7,  7,  7,  7,  7, 16, 16, 17, 17, 66, 66, 66, 66,  
      8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  
      9,  9,  0,  0,  0, 24, 25,  0, 15, 15, 22,  0,  0,  0,  
     21,  0,  3,  3, 14, 14,  0, 28,  0,  0,  0, 23, 30,  0,  
      0,  0, 12, 12, 12, 12, 13, 13, 13, 13, 29,  0,  0, 20,  
     26,  0,  0, 27,  0,  0, 68, 68, 91, 91,  0,  0,  0,  0,  
      0, 69,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5, 67, 67, 67, 67, 10, 10, 10, 10, 11, 11,  
     11, 11, 18, 18, 19, 19,  6,  6,  6,  6,  6,  6,  6,  6,  
      7,  7,  7,  7,  7,  7,  7,  7, 16, 16, 17, 17, 66, 66,  
     66, 66,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  
      9,  9,  9,  9
},
  {
      0,  0,  0, 15,  0,  0,  0,  3, 14,  0,  0,  0,  0,  0,  
     12, 12, 13, 13,  0,  0,  0,  0,  0, 68, 91,  0,  0,  0,  
      4,  4,  4,  4,  5,  5,  5,  5, 67, 67, 10, 10, 11, 11,  
     18, 19,  6,  6,  6,  6,  7,  7,  7,  7, 16, 17, 66, 66,  
      8,  8,  8,  8,  9,  9,  9,  9,  0,  0,  0, 15,  0,  0,  
      0,  3, 14,  0,  0,  0,  0,  0, 12, 12, 13, 13,  0,  0,  
      0,  0,  0, 68, 91,  0,  0,  0,  4,  4,  4,  4,  5,  5,  
      5,  5, 67, 67, 10, 10, 11, 11, 18, 19,  6,  6,  6,  6,  
      7,  7,  7,  7, 16, 17, 66, 66,  8,  8,  8,  8,  9,  9,  
      9,  9,  0,  0,  0, 15,  0,  0,  0,  3, 14,  0,  0,  0,  
      0,  0, 12, 12, 13, 13,  0,  0,  0,  0,  0, 68, 91,  0,  
      0,  0,  4,  4,  4,  4,  5,  5,  5,  5, 67, 67, 10, 10,  
     11, 11, 18, 19,  6,  6,  6,  6,  7,  7,  7,  7, 16, 17,  
     66, 66,  8,  8,  8,  8,  9,  9,  9,  9,  0,  0,  0, 15,  
      0,  0,  0,  3, 14,  0,  0,  0,  0,  0, 12, 12, 13, 13,  
      0,  0,  0,  0,  0, 68, 91,  0,  0,  0,  4,  4,  4,  4,  
      5,  5,  5,  5, 67, 67, 10, 10, 11, 11, 18, 19,  6,  6,  
      6,  6,  7,  7,  7,  7, 16, 17, 66, 66,  8,  8,  8,  8,  
      9,  9,  9,  9
},
  {
      0,  0,  0,  0,  0,  0,  0, 12, 13,  0,  0,  0,  0,  0,  
      4,  4,  5,  5, 67, 10, 11,  0,  6,  6,  7,  7,  0, 66,  
      8,  8,  9,  9,  0,  0,  0,  0,  0,  0,  0, 12, 13,  0,  
      0,  0,  0,  0,  4,  4,  5,  5, 67, 10, 11,  0,  6,  6,  
      7,  7,  0, 66,  8,  8,  9,  9,  0,  0,  0,  0,  0,  0,  
      0, 12, 13,  0,  0,  0,  0,  0,  4,  4,  5,  5, 67, 10,  
     11,  0,  6,  6,  7,  7,  0, 66,  8,  8,  9,  9,  0,  0,  
      0,  0,  0,  0,  0, 12, 13,  0,  0,  0,  0,  0,  4,  4,  
      5,  5, 67, 10, 11,  0,  6,  6,  7,  7,  0, 66,  8,  8,  
      9,  9,  0,  0,  0,  0,  0,  0,  0, 12, 13,  0,  0,  0,  
      0,  0,  4,  4,  5,  5, 67, 10, 11,  0,  6,  6,  7,  7,  
      0, 66,  8,  8,  9,  9,  0,  0,  0,  0,  0,  0,  0, 12,  
     13,  0,  0,  0,  0,  0,  4,  4,  5,  5, 67, 10, 11,  0,  
      6,  6,  7,  7,  0, 66,  8,  8,  9,  9,  0,  0,  0,  0,  
      0,  0,  0, 12, 13,  0,  0,  0,  0,  0,  4,  4,  5,  5,  
     67, 10, 11,  0,  6,  6,  7,  7,  0, 66,  8,  8,  9,  9,  
      0,  0,  0,  0,  0,  0,  0, 12, 13,  0,  0,  0,  0,  0,  
      4,  4,  5,  5, 67, 10, 11,  0,  6,  6,  7,  7,  0, 66,  
      8,  8,  9,  9
},
  {
      0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  
      8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  
      7,  0,  8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  
      0,  6,  7,  0,  8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  
      5,  0,  0,  6,  7,  0,  8,  9,  0,  0,  0,  0,  0,  0,  
      0,  4,  5,  0,  0,  6,  7,  0,  8,  9,  0,  0,  0,  0,  
      0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  8,  9,  0,  0,  
      0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  8,  9,  
      0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  
      8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  
      7,  0,  8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  
      0,  6,  7,  0,  8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  
      5,  0,  0,  6,  7,  0,  8,  9,  0,  0,  0,  0,  0,  0,  
      0,  4,  5,  0,  0,  6,  7,  0,  8,  9,  0,  0,  0,  0,  
      0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  8,  9,  0,  0,  
      0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  8,  9,  
      0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  7,  0,  
      8,  9,  0,  0,  0,  0,  0,  0,  0,  4,  5,  0,  0,  6,  
      7,  0,  8,  9
},
  {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      0,  0,  0,  0,119,  0,  0,120,116,116,117,117,  0,  0,  
    118,118,115,115,115,115,114,114,114,114,113,113,113,113,  
    113,113,113,113,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108
},
  {
      0,  0,  0,  0,116,117,  0,118,115,115,114,114,113,113,  
    113,113,112,112,112,112,112,112,112,112,111,111,111,111,  
    111,111,111,111,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,  0,  0,  0,  0,116,117,  0,118,115,115,114,114,  
    113,113,113,113,112,112,112,112,112,112,112,112,111,111,  
    111,111,111,111,111,111,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108
},
  {
      0,  0,  0,  0,115,114,113,113,112,112,112,112,111,111,  
    111,111,107,107,107,107,107,107,107,107,110,110,110,110,  
    110,110,110,110,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,  0,  0,  0,  0,115,114,  
    113,113,112,112,112,112,111,111,111,111,107,107,107,107,  
    107,107,107,107,110,110,110,110,110,110,110,110,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,  0,  0,  0,  0,115,114,113,113,112,112,112,112,  
    111,111,111,111,107,107,107,107,107,107,107,107,110,110,  
    110,110,110,110,110,110,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,  0,  0,  0,  0,  
    115,114,113,113,112,112,112,112,111,111,111,111,107,107,  
    107,107,107,107,107,107,110,110,110,110,110,110,110,110,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108
},
  {
      0,  0,  0,113,112,112,111,111,107,107,107,107,110,110,  
    110,110,109,109,109,109,109,109,109,109,108,108,108,108,  
    108,108,108,108,  0,  0,  0,113,112,112,111,111,107,107,  
    107,107,110,110,110,110,109,109,109,109,109,109,109,109,  
    108,108,108,108,108,108,108,108,  0,  0,  0,113,112,112,  
    111,111,107,107,107,107,110,110,110,110,109,109,109,109,  
    109,109,109,109,108,108,108,108,108,108,108,108,  0,  0,  
      0,113,112,112,111,111,107,107,107,107,110,110,110,110,  
    109,109,109,109,109,109,109,109,108,108,108,108,108,108,  
    108,108,  0,  0,  0,113,112,112,111,111,107,107,107,107,  
    110,110,110,110,109,109,109,109,109,109,109,109,108,108,  
    108,108,108,108,108,108,  0,  0,  0,113,112,112,111,111,  
    107,107,107,107,110,110,110,110,109,109,109,109,109,109,  
    109,109,108,108,108,108,108,108,108,108,  0,  0,  0,113,  
    112,112,111,111,107,107,107,107,110,110,110,110,109,109,  
    109,109,109,109,109,109,108,108,108,108,108,108,108,108,  
      0,  0,  0,113,112,112,111,111,107,107,107,107,110,110,  
    110,110,109,109,109,109,109,109,109,109,108,108,108,108,  
    108,108,108,108
},
  {
      0,  0,112,111,107,107,110,110,109,109,109,109,108,108,  
    108,108,  0,  0,112,111,107,107,110,110,109,109,109,109,  
    108,108,108,108,  0,  0,112,111,107,107,110,110,109,109,  
    109,109,108,108,108,108,  0,  0,112,111,107,107,110,110,  
    109,109,109,109,108,108,108,108,  0,  0,112,111,107,107,  
    110,110,109,109,109,109,108,108,108,108,  0,  0,112,111,  
    107,107,110,110,109,109,109,109,108,108,108,108,  0,  0,  
    112,111,107,107,110,110,109,109,109,109,108,108,108,108,  
      0,  0,112,111,107,107,110,110,109,109,109,109,108,108,  
    108,108,  0,  0,112,111,107,107,110,110,109,109,109,109,  
    108,108,108,108,  0,  0,112,111,107,107,110,110,109,109,  
    109,109,108,108,108,108,  0,  0,112,111,107,107,110,110,  
    109,109,109,109,108,108,108,108,  0,  0,112,111,107,107,  
    110,110,109,109,109,109,108,108,108,108,  0,  0,112,111,  
    107,107,110,110,109,109,109,109,108,108,108,108,  0,  0,  
    112,111,107,107,110,110,109,109,109,109,108,108,108,108,  
      0,  0,112,111,107,107,110,110,109,109,109,109,108,108,  
    108,108,  0,  0,112,111,107,107,110,110,109,109,109,109,  
    108,108,108,108
},
  {
      0,  0,107,110,109,109,108,108,  0,  0,107,110,109,109,  
    108,108,  0,  0,107,110,109,109,108,108,  0,  0,107,110,  
    109,109,108,108,  0,  0,107,110,109,109,108,108,  0,  0,  
    107,110,109,109,108,108,  0,  0,107,110,109,109,108,108,  
      0,  0,107,110,109,109,108,108,  0,  0,107,110,109,109,  
    108,108,  0,  0,107,110,109,109,108,108,  0,  0,107,110,  
    109,109,108,108,  0,  0,107,110,109,109,108,108,  0,  0,  
    107,110,109,109,108,108,  0,  0,107,110,109,109,108,108,  
      0,  0,107,110,109,109,108,108,  0,  0,107,110,109,109,  
    108,108,  0,  0,107,110,109,109,108,108,  0,  0,107,110,  
    109,109,108,108,  0,  0,107,110,109,109,108,108,  0,  0,  
    107,110,109,109,108,108,  0,  0,107,110,109,109,108,108,  
      0,  0,107,110,109,109,108,108,  0,  0,107,110,109,109,  
    108,108,  0,  0,107,110,109,109,108,108,  0,  0,107,110,  
    109,109,108,108,  0,  0,107,110,109,109,108,108,  0,  0,  
    107,110,109,109,108,108,  0,  0,107,110,109,109,108,108,  
      0,  0,107,110,109,109,108,108,  0,  0,107,110,109,109,  
    108,108,  0,  0,107,110,109,109,108,108,  0,  0,107,110,  
    109,109,108,108
},
  {
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,  
    109,108,  0,  0,109,108,  0,  0,109,108,  0,  0,109,108,  
      0,  0,109,108
},
  {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105
},
  {
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89
},
  {
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92
},
  {
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77
},
  {
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79
},
  {
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81
},
  {
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83
},
  {
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85
},
  {
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 96, 96, 96, 96, 96, 96, 96, 96, 97, 97,  
     97, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 98, 98, 98,  
     99, 99, 99, 99, 99, 99, 99, 99,100,100,100,100,100,100,  
    100,100,101,101,101,101,101,101,101,101, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
    102,102,102,102,102,102,102,102,103,103,103,103,103,103,  
    103,103,104,104,104,104,104,104,104,104,105,105,105,105,  
    105,105,105,105
},
  {
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32
},
  {
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48
},
  {
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50
},
  {
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36
},
  {
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38
},
  {
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40
},
  {
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34
},
  {
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56
},
  {
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42
},
  {
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44
},
  {
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46
},
  {
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64
},
  {
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71
},
  {
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62
},
  {
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92
},
  {
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52
},
  {
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54
},
  {
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58
},
  {
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60
},
  {
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73
},
  {
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75
},
  {
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79
},
  {
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83
},
  {
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87
},
  {
      0,210,210,210,210,210,210,210,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1, 93, 93, 93, 93, 93, 93,  
     93, 93, 96, 96, 96, 96, 97, 97, 97, 97, 98, 98, 98, 98,  
     99, 99, 99, 99,100,100,100,100,101,101,101,101, 94, 94,  
     94, 94, 94, 94, 94, 94, 95, 95, 95, 95, 95, 95, 95, 95,  
    102,102,102,102,103,103,103,103,104,104,104,104,105,105,  
    105,105, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32
},
  {
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24
},
  {
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50
},
  {
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36
},
  {
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40
},
  {
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34
},
  {
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28
},
  {
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44
},
  {
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23
},
  {
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64
},
  {
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71
},
  {
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62
},
  {
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 89, 89, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 92,  
     92, 92, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20
},
  {
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52
},
  {
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27
},
  {
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60
},
  {
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75
},
  {
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 83, 83
},
  {
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87,  
     87, 87, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69
},
  {
      0,210,210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1, 93, 93, 93, 93, 96, 96, 97, 97, 98, 98,  
     99, 99,100,100,101,101, 94, 94, 94, 94, 95, 95, 95, 95,  
    102,102,103,103,104,104,105,105, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24
},
  {
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,  
     50, 50, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15
},
  {
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 40, 40
},
  {
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  
     34, 34,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28
},
  {
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,  
     44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23
},
  {
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 71, 71
},
  {
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,  
     62, 62, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88, 88,  
     88, 88, 88, 88, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89,  
     89, 89, 89, 89, 89, 89, 90, 90, 90, 90, 90, 90, 90, 90,  
     90, 90, 90, 90, 90, 90, 90, 90, 92, 92, 92, 92, 92, 92,  
     92, 92, 92, 92, 92, 92, 92, 92, 92, 92, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20
},
  {
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27
},
  {
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,  
     60, 60, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68
},
  {
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 76, 76, 76, 76,  
     76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 77, 77,  
     77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 75, 75
},
  {
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78,  
     78, 78, 78, 78, 78, 78, 79, 79, 79, 79, 79, 79, 79, 79,  
     79, 79, 79, 79, 79, 79, 79, 79, 80, 80, 80, 80, 80, 80,  
     80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 81, 81, 81, 81,  
     81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 82, 82,  
     82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,  
     83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83,  
     83, 83, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84,  
     84, 84, 84, 84, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85,  
     85, 85, 85, 85, 85, 85, 86, 86, 86, 86, 86, 86, 86, 86,  
     86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 87,  
     87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69
},
  {
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19
},
  {
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17
},
  {
      0,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1, 93, 93, 96, 97, 98, 99,100,101, 94, 94, 95, 95,  
    102,103,104,105, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,  
     31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32,  
     32, 32, 32, 32, 32, 32, 32, 32, 47, 47, 47, 47, 47, 47,  
     47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48,  
     48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 49, 49, 49, 49, 49, 49, 49, 49,  
     49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 50,  
     50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15
},
  {
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,  
     35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 36,  
     36, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37,  
     37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38,  
     38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39,  
     39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,  
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,  
     40, 40, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 33, 33, 33, 33, 33, 33, 33, 33,  
     33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34,  
     34, 34, 34, 34, 34, 34, 34, 34, 34, 34,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 55, 55, 55, 55, 55, 55,  
     55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56,  
     56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,  
     41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,  
     42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 43, 43,  
     43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44,  
     44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45,  
     45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46,  
     46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 23, 23
},
  {
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63,  
     63, 63, 63, 63, 63, 63, 64, 64, 64, 64, 64, 64, 64, 64,  
     64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65,  
     65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 70, 70,  
     70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70,  
     71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,  
     71, 71, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12
},
  {
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 61, 61, 61, 61, 61, 61, 61, 61,  
     61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62,  
     62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 88, 88, 88, 88,  
     88, 88, 88, 88, 89, 89, 89, 89, 89, 89, 89, 89, 90, 90,  
     90, 90, 90, 90, 90, 90, 92, 92, 92, 92, 92, 92, 92, 92,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 20, 20
},
  {
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,  
     51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52,  
     52, 52, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53,  
     53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 54, 54, 54, 54,  
     54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,  
     27, 27, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,  
     57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,  
     58, 58, 58, 58, 58, 58, 59, 59, 59, 59, 59, 59, 59, 59,  
     59, 59, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60,  
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68
},
  {
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 72, 72, 72, 72, 72, 72,  
     72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 73, 73, 73, 73,  
     73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 76, 76,  
     76, 76, 76, 76, 76, 76, 77, 77, 77, 77, 77, 77, 77, 77,  
     75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,  
     75, 75, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74, 74,  
     74, 74, 74, 74, 78, 78, 78, 78, 78, 78, 78, 78, 79, 79,  
     79, 79, 79, 79, 79, 79, 80, 80, 80, 80, 80, 80, 80, 80,  
     81, 81, 81, 81, 81, 81, 81, 81, 82, 82, 82, 82, 82, 82,  
     82, 82, 83, 83, 83, 83, 83, 83, 83, 83, 84, 84, 84, 84,  
     84, 84, 84, 84, 85, 85, 85, 85, 85, 85, 85, 85, 86, 86,  
     86, 86, 86, 86, 86, 86, 87, 87, 87, 87, 87, 87, 87, 87,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69, 69, 69
},
  {
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10
},
  {
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19
},
  {
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66
},
  {
      0,  1,  1,  1,  1,  1,  1,  1, 93,  0,  0,  0, 94, 95,  
      0,  0, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32,  
     32, 32, 32, 32, 47, 47, 47, 47, 47, 47, 47, 47, 48, 48,  
     48, 48, 48, 48, 48, 48, 24, 24, 24, 24, 24, 24, 24, 24,  
     24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25,  
     25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 49, 49, 49, 49,  
     49, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50, 50, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,  
     22, 22, 22, 22, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36,  
     36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37,  
     38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39,  
     39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 21, 21, 21, 21,  
     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 33, 33,  
     33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 34,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56,  
     56, 56, 56, 56, 56, 56, 28, 28, 28, 28, 28, 28, 28, 28,  
     28, 28, 28, 28, 28, 28, 28, 28, 41, 41, 41, 41, 41, 41,  
     41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43,  
     43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 44, 44, 45, 45,  
     45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46, 46, 46,  
     23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,  
     23, 23, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,  
     30, 30, 30, 30, 63, 63, 63, 63, 63, 63, 63, 63, 64, 64,  
     64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65,  
      2,  2,  2,  2,  2,  2,  2,  2, 70, 70, 70, 70, 70, 70,  
     70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12
},
  {
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 29, 29, 29, 29, 29, 29,  
     29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 61, 61, 61, 61,  
     61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 88, 88,  
     88, 88, 89, 89, 89, 89, 90, 90, 90, 90, 92, 92, 92, 92,  
     20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,  
     20, 20, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,  
     26, 26, 26, 26, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52,  
     52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 53, 53,  
     54, 54, 54, 54, 54, 54, 54, 54, 27, 27, 27, 27, 27, 27,  
     27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 57, 57, 57, 57,  
     57, 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 59, 59,  
     59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 60, 60,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 68, 68
},
  {
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 72, 72, 72, 72, 72, 72, 72, 72, 73, 73,  
     73, 73, 73, 73, 73, 73, 76, 76, 76, 76, 77, 77, 77, 77,  
     75, 75, 75, 75, 75, 75, 75, 75, 74, 74, 74, 74, 74, 74,  
     74, 74, 78, 78, 78, 78, 79, 79, 79, 79, 80, 80, 80, 80,  
     81, 81, 81, 81, 82, 82, 82, 82, 83, 83, 83, 83, 84, 84,  
     84, 84, 85, 85, 85, 85, 86, 86, 86, 86, 87, 87, 87, 87,  
     69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69,  
     69, 69,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10
},
  {
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  
     19, 19,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6
},
  {
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66
},
  {
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9
},
  {
      0,  1,  1,  1,  0,  0,  0,  0, 31, 31, 31, 31, 32, 32,  
     32, 32, 47, 47, 47, 47, 48, 48, 48, 48, 24, 24, 24, 24,  
     24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 49, 49,  
     49, 49, 50, 50, 50, 50, 15, 15, 15, 15, 15, 15, 15, 15,  
     15, 15, 15, 15, 15, 15, 15, 15, 22, 22, 22, 22, 22, 22,  
     22, 22, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37,  
     38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 21, 21,  
     21, 21, 21, 21, 21, 21, 33, 33, 33, 33, 34, 34, 34, 34,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,  
     14, 14, 14, 14, 55, 55, 55, 55, 56, 56, 56, 56, 28, 28,  
     28, 28, 28, 28, 28, 28, 41, 41, 41, 41, 42, 42, 42, 42,  
     43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46,  
     46, 46, 23, 23, 23, 23, 23, 23, 23, 23, 30, 30, 30, 30,  
     30, 30, 30, 30, 63, 63, 63, 63, 64, 64, 64, 64, 65, 65,  
     65, 65,  2,  2,  2,  2, 70, 70, 70, 70, 71, 71, 71, 71,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 12, 12
},
  {
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 29, 29, 29, 29, 29, 29, 29, 29, 61, 61,  
     61, 61, 62, 62, 62, 62, 88, 88, 89, 89, 90, 90, 92, 92,  
     20, 20, 20, 20, 20, 20, 20, 20, 26, 26, 26, 26, 26, 26,  
     26, 26, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53,  
     54, 54, 54, 54, 27, 27, 27, 27, 27, 27, 27, 27, 57, 57,  
     57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60,  
     68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,  
     68, 68, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,  
     91, 91, 91, 91, 72, 72, 72, 72, 73, 73, 73, 73, 76, 76,  
     77, 77, 75, 75, 75, 75, 74, 74, 74, 74, 78, 78, 79, 79,  
     80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86,  
     87, 87, 69, 69, 69, 69, 69, 69, 69, 69,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 18, 18, 18, 18, 18, 18, 18, 18,  
     18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19,  
     19, 19, 19, 19, 19, 19, 19, 19, 19, 19,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6
},
  {
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7, 16, 16, 16, 16, 16, 16,  
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17,  
     17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  
     66, 66,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9
},
  {
      0,  1,  0,  0, 31, 31, 32, 32, 47, 47, 48, 48, 24, 24,  
     24, 24, 25, 25, 25, 25, 49, 49, 50, 50, 15, 15, 15, 15,  
     15, 15, 15, 15, 22, 22, 22, 22, 35, 35, 36, 36, 37, 37,  
     38, 38, 39, 39, 40, 40, 21, 21, 21, 21, 33, 33, 34, 34,  
      3,  3,  3,  3,  3,  3,  3,  3, 14, 14, 14, 14, 14, 14,  
     14, 14, 55, 55, 56, 56, 28, 28, 28, 28, 41, 41, 42, 42,  
     43, 43, 44, 44, 45, 45, 46, 46, 23, 23, 23, 23, 30, 30,  
     30, 30, 63, 63, 64, 64, 65, 65,  2,  2, 70, 70, 71, 71,  
     12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,  
     12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,  
     13, 13, 13, 13, 29, 29, 29, 29, 61, 61, 62, 62, 88, 89,  
     90, 92, 20, 20, 20, 20, 26, 26, 26, 26, 51, 51, 52, 52,  
     53, 53, 54, 54, 27, 27, 27, 27, 57, 57, 58, 58, 59, 59,  
     60, 60, 68, 68, 68, 68, 68, 68, 68, 68, 91, 91, 91, 91,  
     91, 91, 91, 91, 72, 72, 73, 73, 76, 77, 75, 75, 74, 74,  
     78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 69, 69, 69, 69,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67,  
     67, 67, 67, 67, 67, 67, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 18, 18, 18, 18,  
     18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7, 16, 16, 16, 16, 16, 16, 16, 16,  
     17, 17, 17, 17, 17, 17, 17, 17, 66, 66, 66, 66, 66, 66,  
     66, 66, 66, 66, 66, 66, 66, 66, 66, 66,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209
},
  {
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,179,179,179,179,  
    179,179,179,179,180,180,180,180,180,180,180,180,181,181,  
    181,181,181,181,181,181,182,182,182,182,182,182,182,182,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,189,189,189,189,189,189,189,189,190,190,  
    190,190,190,190,190,190,191,191,191,191,191,191,191,191,  
    192,192,192,192,192,192,192,192,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,193,193,  
    193,193,193,193,193,193,194,194,194,194,194,194,194,194,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130
},
  {
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,195,195,195,195,195,195,195,195,196,196,  
    196,196,196,196,196,196,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,177,177,  
    177,177,177,177,177,177,178,178,178,178,178,178,178,178,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,183,183,183,183,183,183,183,183,184,184,  
    184,184,184,184,184,184,185,185,185,185,185,185,185,185,  
    186,186,186,186,186,186,186,186,187,187,187,187,187,187,  
    187,187,188,188,188,188,188,188,188,188,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170
},
  {
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122
},
  {
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128
},
  {
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125
},
  {
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,200,200,200,200,200,200,200,200,201,201,  
    201,201,201,201,201,201,202,202,202,202,202,202,202,202,  
    203,203,203,203,203,203,203,203,204,204,204,204,204,204,  
    204,204,205,205,205,205,205,205,205,205,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    206,206,206,206,206,206,206,206,207,207,207,207,207,207,  
    207,207,208,208,208,208,208,208,208,208,209,209,209,209,  
    209,209,209,209
},
  {
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,158,158,158,158,158,158,158,158,179,179,  
    179,179,180,180,180,180,181,181,181,181,182,182,182,182,  
    161,161,161,161,161,161,161,161,162,162,162,162,162,162,  
    162,162,189,189,189,189,190,190,190,190,191,191,191,191,  
    192,192,192,192,165,165,165,165,165,165,165,165,166,166,  
    166,166,166,166,166,166,193,193,193,193,194,194,194,194,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,195,195,195,195,196,196,196,196,174,174,  
    174,174,174,174,174,174,175,175,175,175,175,175,175,175,  
    176,176,176,176,176,176,176,176,177,177,177,177,178,178,  
    178,178,159,159,159,159,159,159,159,159,160,160,160,160,  
    160,160,160,160,183,183,183,183,184,184,184,184,185,185,  
    185,185,186,186,186,186,187,187,187,187,188,188,188,188,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170
},
  {
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,156,156,156,156,156,156,156,156,157,157,  
    157,157,157,157,157,157,150,150,150,150,150,150,150,150,  
    151,151,151,151,151,151,151,151,152,152,152,152,152,152,  
    152,152,153,153,153,153,153,153,153,153,163,163,163,163,  
    163,163,163,163,164,164,164,164,164,164,164,164,167,167,  
    167,167,167,167,167,167,173,173,173,173,173,173,173,173,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122
},
  {
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,154,154,154,154,154,154,154,154,155,155,  
    155,155,155,155,155,155,168,168,168,168,168,168,168,168,  
    169,169,169,169,169,169,169,169,136,136,136,136,136,136,  
    136,136,137,137,137,137,137,137,137,137,138,138,138,138,  
    138,138,138,138,139,139,139,139,139,139,139,139,146,146,  
    146,146,146,146,146,146,147,147,147,147,147,147,147,147,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120
},
  {
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,171,171,171,171,171,171,  
    171,171,172,172,172,172,172,172,172,172,132,132,132,132,  
    132,132,132,132,133,133,133,133,133,133,133,133,134,134,  
    134,134,134,134,134,134,135,135,135,135,135,135,135,135,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,140,140,140,140,140,140,140,140,141,141,  
    141,141,141,141,141,141,142,142,142,142,142,142,142,142,  
    143,143,143,143,143,143,143,143,144,144,144,144,144,144,  
    144,144,145,145,145,145,145,145,145,145,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,148,148,  
    148,148,148,148,148,148,149,149,149,149,149,149,149,149,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106
},
  {
      0,210,210,210,210,210,210,210,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,197,197,197,197,197,197,  
    197,197,200,200,200,200,201,201,201,201,202,202,202,202,  
    203,203,203,203,204,204,204,204,205,205,205,205,198,198,  
    198,198,198,198,198,198,199,199,199,199,199,199,199,199,  
    206,206,206,206,207,207,207,207,208,208,208,208,209,209,  
    209,209,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,158,158,158,158,179,179,180,180,181,181,  
    182,182,161,161,161,161,162,162,162,162,189,189,190,190,  
    191,191,192,192,165,165,165,165,166,166,166,166,193,193,  
    194,194,130,130,130,130,130,130,130,130,131,131,131,131,  
    131,131,131,131,195,195,196,196,174,174,174,174,175,175,  
    175,175,176,176,176,176,177,177,178,178,159,159,159,159,  
    160,160,160,160,183,183,184,184,185,185,186,186,187,187,  
    188,188,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170
},
  {
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,129,129,129,129,129,129,  
    129,129,156,156,156,156,157,157,157,157,150,150,150,150,  
    151,151,151,151,152,152,152,152,153,153,153,153,163,163,  
    163,163,164,164,164,164,167,167,167,167,173,173,173,173,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,154,154,154,154,155,155,155,155,168,168,  
    168,168,169,169,169,169,136,136,136,136,137,137,137,137,  
    138,138,138,138,139,139,139,139,146,146,146,146,147,147,  
    147,147,128,128,128,128,128,128,128,128,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120
},
  {
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117
},
  {
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,171,171,171,171,172,172,172,172,132,132,  
    132,132,133,133,133,133,134,134,134,134,135,135,135,135,  
    125,125,125,125,125,125,125,125,126,126,126,126,126,126,  
    126,126,140,140,140,140,141,141,141,141,142,142,142,142,  
    143,143,143,143,144,144,144,144,145,145,145,145,127,127,  
    127,127,127,127,127,127,148,148,148,148,149,149,149,149,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118
},
  {
      0,210,210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,197,197,197,197,200,200,201,201,202,202,  
    203,203,204,204,205,205,198,198,198,198,199,199,199,199,  
    206,206,207,207,208,208,209,209,124,124,124,124,124,124,  
    124,124,158,158,179,180,181,182,161,161,162,162,189,190,  
    191,192,165,165,166,166,193,194,130,130,130,130,131,131,  
    131,131,195,196,174,174,175,175,176,176,177,178,159,159,  
    160,160,183,184,185,186,187,188,170,170,170,170,170,170,  
    170,170,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,119,119,119,119,  
    119,119,119,119,119,119,129,129,129,129,156,156,157,157,  
    150,150,151,151,152,152,153,153,163,163,164,164,167,167,  
    173,173,122,122,122,122,122,122,122,122,123,123,123,123,  
    123,123,123,123,154,154,155,155,168,168,169,169,136,136,  
    137,137,138,138,139,139,146,146,147,147,128,128,128,128,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,120,120
},
  {
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,121,121,121,121,121,121,121,121,121,121,121,121,  
    121,121,121,121,171,171,172,172,132,132,133,133,134,134,  
    135,135,125,125,125,125,126,126,126,126,140,140,141,141,  
    142,142,143,143,144,144,145,145,127,127,127,127,148,148,  
    149,149,106,106,106,106,106,106,106,106,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118
},
  {
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114
},
  {
      0,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,197,197,200,201,202,203,204,205,198,198,199,199,  
    206,207,208,209,124,124,124,124,158,  0,  0,161,162,  0,  
      0,165,166,  0,130,130,131,131,  0,174,175,176,  0,159,  
    160,  0,  0,  0,170,170,170,170,119,119,119,119,119,119,  
    119,119,119,119,119,119,119,119,119,119,129,129,156,157,  
    150,151,152,153,163,164,167,173,122,122,122,122,123,123,  
    123,123,154,155,168,169,136,137,138,139,146,147,128,128,  
    120,120,120,120,120,120,120,120,120,120,120,120,120,120,  
    120,120,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,116,116,116,116,  
    116,116,116,116,116,116,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,121,121,121,121,  
    121,121,121,121,171,172,132,133,134,135,125,125,126,126,  
    140,141,142,143,144,145,127,127,148,149,106,106,106,106,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,118,118
},
  {
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113
},
  {
      0,  1,  1,  1,  1,  1,  1,  1,197,  0,  0,  0,198,199,  
      0,  0,124,124,  0,  0,  0,  0,  0,130,131,  0,  0,  0,  
      0,  0,170,170,119,119,119,119,119,119,119,119,129,  0,  
      0,  0,  0,  0,122,122,123,123,  0,  0,  0,  0,  0,128,  
    120,120,120,120,120,120,120,120,116,116,116,116,116,116,  
    116,116,116,116,116,116,116,116,116,116,117,117,117,117,  
    117,117,117,117,117,117,117,117,117,117,117,117,121,121,  
    121,121,  0,  0,  0,125,126,  0,  0,  0,127,  0,106,106,  
    118,118,118,118,118,118,118,118,118,118,118,118,118,118,  
    118,118,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,115,115,115,115,  
    115,115,115,115,115,115,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113
},
  {
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111
},
  {
      0,  1,  1,  1,  0,  0,  0,  0,124,  0,  0,  0,  0,  0,  
      0,170,119,119,119,119,  0,  0,  0,122,123,  0,  0,  0,  
    120,120,120,120,116,116,116,116,116,116,116,116,117,117,  
    117,117,117,117,117,117,121,121,  0,  0,  0,  0,  0,106,  
    118,118,118,118,118,118,118,118,115,115,115,115,115,115,  
    115,115,115,115,115,115,115,115,115,115,114,114,114,114,  
    114,114,114,114,114,114,114,114,114,114,114,114,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,113,113,113,113,113,113,  
    113,113,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111
},
  {
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110
},
  {
      0,  1,  0,  0,  0,  0,  0,  0,119,119,  0,  0,  0,  0,  
    120,120,116,116,116,116,117,117,117,117,121,  0,  0,  0,  
    118,118,118,118,115,115,115,115,115,115,115,115,114,114,  
    114,114,114,114,114,114,113,113,113,113,113,113,113,113,  
    113,113,113,113,113,113,113,113,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,112,112,  
    112,112,112,112,112,112,112,112,112,112,112,112,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,111,111,111,111,111,111,111,111,111,111,111,111,  
    111,111,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,107,107,107,107,  
    107,107,107,107,107,107,107,107,107,107,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110,110,110,110,110,110,110,110,110,110,110,  
    110,110,110,110
},
  {
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,109,109,109,109,109,109,109,109,109,109,109,109,  
    109,109,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108,108,108,108,108,108,108,108,108,108,108,  
    108,108,108,108
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210
},
  {
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97
},
  {
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99
},
  {
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101
},
  {
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103
},
  {
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97
},
  {
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101
},
  {
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95
},
  {
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93, 93,  
     93, 93, 93, 93, 93, 93, 93, 93, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,  
     96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  
     97, 97, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98, 98,  
     98, 98, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  
     99, 99, 99, 99, 99, 99, 99, 99, 99, 99,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101,101,101,101,101,101,101,101,101,101,101,  
    101,101,101,101
},
  {
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94, 94,  
     94, 94, 94, 94, 94, 94, 94, 94, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,  
     95, 95,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,102,102,102,102,102,102,102,102,  
    102,102,102,102,102,102,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,103,103,103,103,  
    103,103,103,103,103,103,103,103,103,103,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    104,104,104,104,104,104,104,104,104,104,104,104,104,104,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105,105,105,105,105,105,105,105,105,105,105,  
    105,105,105,105
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210
},
  {
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180
},
  {
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182
},
  {
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190
},
  {
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192
},
  {
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194
},
  {
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196
},
  {
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178
},
  {
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184
},
  {
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186
},
  {
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188
},
  {
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201
},
  {
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203
},
  {
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205
},
  {
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207
},
  {
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209
},
  {
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180
},
  {
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161
},
  {
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190
},
  {
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165
},
  {
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194
},
  {
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174
},
  {
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176
},
  {
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159
},
  {
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184
},
  {
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188
},
  {
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157
},
  {
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151
},
  {
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153
},
  {
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164
},
  {
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173
},
  {
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155
},
  {
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169
},
  {
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137
},
  {
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139
},
  {
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147
},
  {
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172
},
  {
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133
},
  {
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135
},
  {
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141
},
  {
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143
},
  {
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145
},
  {
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201
},
  {
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205
},
  {
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199
},
  {
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209
},
  {
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,179,179,179,179,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,180,180,180,180,  
    180,180,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,181,181,  
    181,181,181,181,181,181,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    182,182,182,182,182,182,182,182,182,182,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161
},
  {
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,189,189,189,189,189,189,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,190,190,190,190,190,190,  
    190,190,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,191,191,191,191,  
    191,191,191,191,191,191,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165
},
  {
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,193,193,193,193,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,194,194,194,194,  
    194,194,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130
},
  {
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,195,195,195,195,  
    195,195,195,195,195,195,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174
},
  {
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,177,177,  
    177,177,177,177,177,177,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    178,178,178,178,178,178,178,178,178,178,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159
},
  {
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,183,183,183,183,183,183,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,184,184,184,184,184,184,  
    184,184,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,185,185,185,185,  
    185,185,185,185,185,185,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,188,188
},
  {
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157
},
  {
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153
},
  {
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173
},
  {
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169
},
  {
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139
},
  {
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128
},
  {
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133
},
  {
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125
},
  {
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141
},
  {
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145
},
  {
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149
},
  {
      0,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,210,210,210,210,210,210,  
    210,210,210,210,210,210,210,210,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,197,197,197,197,197,197,  
    197,197,197,197,197,197,197,197,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,200,200,  
    200,200,200,200,200,200,200,200,200,200,200,200,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,201,201,201,201,201,201,201,201,201,201,201,201,  
    201,201,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,202,202,202,202,202,202,202,202,  
    202,202,202,202,202,202,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,203,203,203,203,  
    203,203,203,203,203,203,203,203,203,203,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    204,204,204,204,204,204,204,204,204,204,204,204,204,204,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205,205,205,205,205,205,205,205,205,205,205,  
    205,205,205,205
},
  {
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,198,198,198,198,198,198,  
    198,198,198,198,198,198,198,198,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,199,199,199,199,199,199,199,199,199,199,199,199,  
    199,199,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,206,206,206,206,206,206,206,206,  
    206,206,206,206,206,206,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,207,207,207,207,  
    207,207,207,207,207,207,207,207,207,207,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    208,208,208,208,208,208,208,208,208,208,208,208,208,208,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209,209,209,209,209,209,209,209,209,209,209,  
    209,209,209,209
},
  {
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,124,124,124,124,124,124,124,124,124,124,124,124,  
    124,124,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,158,158,158,158,158,158,158,158,  
    158,158,158,158,158,158,179,179,179,179,179,179,179,179,  
    179,179,179,179,179,179,179,179,180,180,180,180,180,180,  
    180,180,180,180,180,180,180,180,180,180,181,181,181,181,  
    181,181,181,181,181,181,181,181,181,181,181,181,182,182,  
    182,182,182,182,182,182,182,182,182,182,182,182,182,182,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161,161,161,161,161,161,161,161,161,161,161,  
    161,161,161,161
},
  {
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,162,162,162,162,162,162,162,162,162,162,  
    162,162,162,162,189,189,189,189,189,189,189,189,189,189,  
    189,189,189,189,189,189,190,190,190,190,190,190,190,190,  
    190,190,190,190,190,190,190,190,191,191,191,191,191,191,  
    191,191,191,191,191,191,191,191,191,191,192,192,192,192,  
    192,192,192,192,192,192,192,192,192,192,192,192,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,165,165,165,165,165,165,165,165,165,165,165,165,  
    165,165,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,166,166,166,166,166,166,166,166,  
    166,166,166,166,166,166,193,193,193,193,193,193,193,193,  
    193,193,193,193,193,193,193,193,194,194,194,194,194,194,  
    194,194,194,194,194,194,194,194,194,194,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130,130,130,130,130,130,130,130,130,130,130,  
    130,130,130,130
},
  {
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,131,131,131,131,131,131,  
    131,131,131,131,131,131,131,131,195,195,195,195,195,195,  
    195,195,195,195,195,195,195,195,195,195,196,196,196,196,  
    196,196,196,196,196,196,196,196,196,196,196,196,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,174,174,174,174,174,174,174,174,174,174,174,174,  
    174,174,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,175,175,175,175,175,175,175,175,  
    175,175,175,175,175,175,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,176,176,176,176,  
    176,176,176,176,176,176,176,176,176,176,177,177,177,177,  
    177,177,177,177,177,177,177,177,177,177,177,177,178,178,  
    178,178,178,178,178,178,178,178,178,178,178,178,178,178,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159,159,159,159,159,159,159,159,159,159,159,  
    159,159,159,159
},
  {
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,160,160,160,160,160,160,160,160,160,160,  
    160,160,160,160,183,183,183,183,183,183,183,183,183,183,  
    183,183,183,183,183,183,184,184,184,184,184,184,184,184,  
    184,184,184,184,184,184,184,184,185,185,185,185,185,185,  
    185,185,185,185,185,185,185,185,185,185,186,186,186,186,  
    186,186,186,186,186,186,186,186,186,186,186,186,187,187,  
    187,187,187,187,187,187,187,187,187,187,187,187,187,187,  
    188,188,188,188,188,188,188,188,188,188,188,188,188,188,  
    188,188,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170,170,170,170,170,170,170,170,170,170,170,  
    170,170,170,170
},
  {
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,129,129,129,129,129,129,  
    129,129,129,129,129,129,129,129,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,156,156,  
    156,156,156,156,156,156,156,156,156,156,156,156,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,157,157,157,157,157,157,157,157,157,157,157,157,  
    157,157,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,150,150,150,150,150,150,150,150,  
    150,150,150,150,150,150,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,151,151,151,151,  
    151,151,151,151,151,151,151,151,151,151,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    152,152,152,152,152,152,152,152,152,152,152,152,152,152,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153,153,153,153,153,153,153,153,153,153,153,  
    153,153,153,153
},
  {
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,163,163,163,163,163,163,163,163,163,163,  
    163,163,163,163,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,164,164,164,164,164,164,  
    164,164,164,164,164,164,164,164,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,167,167,  
    167,167,167,167,167,167,167,167,167,167,167,167,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,173,173,173,173,173,173,173,173,173,173,173,173,  
    173,173,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,  
    122,122,122,122
},
  {
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,123,123,123,123,123,123,123,123,123,123,123,123,  
    123,123,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,154,154,154,154,154,154,154,154,  
    154,154,154,154,154,154,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,155,155,155,155,  
    155,155,155,155,155,155,155,155,155,155,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    168,168,168,168,168,168,168,168,168,168,168,168,168,168,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169,169,169,169,169,169,169,169,169,169,169,  
    169,169,169,169
},
  {
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,136,136,136,136,136,136,136,136,136,136,  
    136,136,136,136,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,137,137,137,137,137,137,  
    137,137,137,137,137,137,137,137,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,138,138,  
    138,138,138,138,138,138,138,138,138,138,138,138,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,139,139,139,139,139,139,139,139,139,139,139,139,  
    139,139,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,146,146,146,146,146,146,146,146,  
    146,146,146,146,146,146,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,147,147,147,147,  
    147,147,147,147,147,147,147,147,147,147,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128,128,128,128,128,128,128,128,128,128,128,  
    128,128,128,128
},
  {
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,171,171,171,171,171,171,171,171,171,171,  
    171,171,171,171,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,172,172,172,172,172,172,  
    172,172,172,172,172,172,172,172,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,132,132,  
    132,132,132,132,132,132,132,132,132,132,132,132,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,133,133,133,133,133,133,133,133,133,133,133,133,  
    133,133,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,134,134,134,134,134,134,134,134,  
    134,134,134,134,134,134,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,135,135,135,135,  
    135,135,135,135,135,135,135,135,135,135,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125,125,125,125,125,125,125,125,125,125,125,  
    125,125,125,125
},
  {
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,126,126,126,126,126,126,  
    126,126,126,126,126,126,126,126,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,140,140,  
    140,140,140,140,140,140,140,140,140,140,140,140,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,141,141,141,141,141,141,141,141,141,141,141,141,  
    141,141,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,142,142,142,142,142,142,142,142,  
    142,142,142,142,142,142,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,143,143,143,143,  
    143,143,143,143,143,143,143,143,143,143,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    144,144,144,144,144,144,144,144,144,144,144,144,144,144,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145,145,145,145,145,145,145,145,145,145,145,  
    145,145,145,145
},
  {
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,  
    127,127,127,127,127,127,127,127,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,148,148,  
    148,148,148,148,148,148,148,148,148,148,148,148,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,149,149,149,149,149,149,149,149,149,149,149,149,  
    149,149,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106,106,106,106,106,106,106,106,106,106,106,  
    106,106,106,106
}
};
u_char	TIFFFax1DNextState[230][256] = {
  {
     16, 17,  0,  0,  0,  0,  7,  7,  7,  7,  0,  0,  6,  6,  
      6,  6,  7,  7,  0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  
      6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  7,  7,  0,  0,  
      0,  0,  0,  0,  7,  7,  7,  7,  0,  0,  0,  0,  0,  0,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  7,  7,  0,  0, 18, 19,  7,  7,  7,  7,  0,  0,  
      0,  0,  7,  7,  0,  0,  0,  0,  6,  6,  6,  6,  6,  6,  
      6,  6,  0,  0, 20,  0,  0, 21, 22, 23, 24, 25,  7,  7,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      6,  6,  6,  6,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  
      6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
     26, 27, 28,  0,  0, 29,  7,  7,  0, 30, 31, 32,  0, 33,  
      7,  7,  7,  7, 34,  0, 35, 36, 37,  0,  0, 38, 39, 40,  
      6,  6,  6,  6,  6,  6,  6,  6,  0, 41, 42,  0,  0, 43,  
     44,  0, 45, 46,  7,  7,  7,  7, 47, 48, 49, 50, 51,  0,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  7,  7,  7,  7,  6,  6,  6,  6,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5, 26, 27, 28,  0,  0, 29,  7,  7,  0, 30, 31, 32,  
      0, 33,  7,  7,  7,  7, 34,  0, 35, 36, 37,  0,  0, 38,  
     39, 40,  6,  6,  6,  6,  6,  6,  6,  6,  0, 41, 42,  0,  
      0, 43, 44,  0, 45, 46,  7,  7,  7,  7, 47, 48, 49, 50,  
     51,  0,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  7,  7,  7,  7,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5
},
  {
     52, 53, 54,  0, 55, 56, 57,  0,  0, 58, 59, 60, 61, 62,  
      7,  7,  7,  7, 63, 64, 65, 66, 67,  0,  0, 68, 69, 70,  
      6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  
      0,  0,  6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  7,  7,  
      6,  6,  6,  6,  6,  6,  6,  6, 52, 53, 54,  0, 55, 56,  
     57,  0,  0, 58, 59, 60, 61, 62,  7,  7,  7,  7, 63, 64,  
     65, 66, 67,  0,  0, 68, 69, 70,  6,  6,  6,  6,  6,  6,  
      6,  6,  7,  7,  7,  7,  7,  7,  0,  0,  6,  6,  6,  6,  
      6,  6,  6,  6,  0,  0,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6, 52, 53, 54,  0, 55, 56, 57,  0,  0, 58, 59, 60,  
     61, 62,  7,  7,  7,  7, 63, 64, 65, 66, 67,  0,  0, 68,  
     69, 70,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  0,  0,  6,  6,  6,  6,  6,  6,  6,  6,  0,  0,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6, 52, 53, 54,  0,  
     55, 56, 57,  0,  0, 58, 59, 60, 61, 62,  7,  7,  7,  7,  
     63, 64, 65, 66, 67,  0,  0, 68, 69, 70,  6,  6,  6,  6,  
      6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  0,  0,  6,  6,  
      6,  6,  6,  6,  6,  6,  0,  0,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6
},
  {
     71, 72, 73, 74, 75, 76, 77,  0,  0, 78, 79, 80, 81, 82,  
      7,  7,  7,  7,  0,  0,  0, 83,  7,  7,  7,  7, 84,  0,  
      7,  7,  7,  7, 71, 72, 73, 74, 75, 76, 77,  0,  0, 78,  
     79, 80, 81, 82,  7,  7,  7,  7,  0,  0,  0, 83,  7,  7,  
      7,  7, 84,  0,  7,  7,  7,  7, 71, 72, 73, 74, 75, 76,  
     77,  0,  0, 78, 79, 80, 81, 82,  7,  7,  7,  7,  0,  0,  
      0, 83,  7,  7,  7,  7, 84,  0,  7,  7,  7,  7, 71, 72,  
     73, 74, 75, 76, 77,  0,  0, 78, 79, 80, 81, 82,  7,  7,  
      7,  7,  0,  0,  0, 83,  7,  7,  7,  7, 84,  0,  7,  7,  
      7,  7, 71, 72, 73, 74, 75, 76, 77,  0,  0, 78, 79, 80,  
     81, 82,  7,  7,  7,  7,  0,  0,  0, 83,  7,  7,  7,  7,  
     84,  0,  7,  7,  7,  7, 71, 72, 73, 74, 75, 76, 77,  0,  
      0, 78, 79, 80, 81, 82,  7,  7,  7,  7,  0,  0,  0, 83,  
      7,  7,  7,  7, 84,  0,  7,  7,  7,  7, 71, 72, 73, 74,  
     75, 76, 77,  0,  0, 78, 79, 80, 81, 82,  7,  7,  7,  7,  
      0,  0,  0, 83,  7,  7,  7,  7, 84,  0,  7,  7,  7,  7,  
     71, 72, 73, 74, 75, 76, 77,  0,  0, 78, 79, 80, 81, 82,  
      7,  7,  7,  7,  0,  0,  0, 83,  7,  7,  7,  7, 84,  0,  
      7,  7,  7,  7
},
  {
     85, 86, 87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  
      0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  
      0, 94,  0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  0, 92,  
     93,  0,  0, 94,  0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  
      0, 92, 93,  0,  0, 94,  0,  0, 85, 86, 87, 88, 89, 90,  
     91,  0,  0, 92, 93,  0,  0, 94,  0,  0, 85, 86, 87, 88,  
     89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  0,  0, 85, 86,  
     87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  0,  0,  
     85, 86, 87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  
      0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  
      0, 94,  0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  0, 92,  
     93,  0,  0, 94,  0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  
      0, 92, 93,  0,  0, 94,  0,  0, 85, 86, 87, 88, 89, 90,  
     91,  0,  0, 92, 93,  0,  0, 94,  0,  0, 85, 86, 87, 88,  
     89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  0,  0, 85, 86,  
     87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  0,  0,  
     85, 86, 87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  0, 94,  
      0,  0, 85, 86, 87, 88, 89, 90, 91,  0,  0, 92, 93,  0,  
      0, 94,  0,  0
},
  {
     95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,  
    101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98,  
     99,100,101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96,  
     97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,101,102,  
     95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,  
    101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98,  
     99,100,101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96,  
     97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,101,102,  
     95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,  
    101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98,  
     99,100,101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96,  
     97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,101,102,  
     95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,  
    101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98,  
     99,100,101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96,  
     97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,101,102,  
     95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98, 99,100,  
    101,102, 95, 96, 97, 98, 99,100,101,102, 95, 96, 97, 98,  
     99,100,101,102
},
  {
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106,103,104,105,106,103,104,105,106,103,104,  
    105,106,103,104,105,106,103,104,105,106,103,104,105,106,  
    103,104,105,106
},
  {
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108,107,108,107,108,107,108,107,108,107,108,  
    107,108,107,108
},
  {
    109,110,111,112,  0,113,114,  0,  7,  7,  7,  7,115,116,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  
      5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
    117,118,119,120,  0,  0,121,  0,  7,  7,  7,  7,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,117,118,119,120,  0,  0,121,  0,  7,  7,  7,  7,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
    122,123,124,125,  0,  0,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,122,123,124,125,  0,  0,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,122,123,124,125,  0,  0,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,122,123,124,125,  
      0,  0,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
    126,127,128,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,126,127,128,  0,  7,  7,  7,  7,  6,  6,  
      6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,126,127,128,  0,  7,  7,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,126,127,  
    128,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,126,127,128,  0,  7,  7,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,126,127,128,  0,  7,  7,  7,  7,  
      6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,126,127,128,  0,  
      7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
    126,127,128,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5
},
  {
    129,130,  0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,129,130,  0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6,129,130,  0,  0,  7,  7,  7,  7,  6,  6,  
      6,  6,  6,  6,  6,  6,129,130,  0,  0,  7,  7,  7,  7,  
      6,  6,  6,  6,  6,  6,  6,  6,129,130,  0,  0,  7,  7,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,129,130,  0,  0,  
      7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,129,130,  
      0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  
    129,130,  0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,129,130,  0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6,129,130,  0,  0,  7,  7,  7,  7,  6,  6,  
      6,  6,  6,  6,  6,  6,129,130,  0,  0,  7,  7,  7,  7,  
      6,  6,  6,  6,  6,  6,  6,  6,129,130,  0,  0,  7,  7,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,129,130,  0,  0,  
      7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,129,130,  
      0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  
    129,130,  0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,129,130,  0,  0,  7,  7,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6
},
  {
    131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  
      7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  
      7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  
      0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,  
    131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  
      7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  
      7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  
      0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,  
    131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  
      7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  
      7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  
      0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,  
    131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  
      7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  
      7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  
      0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  7,  7,  
    131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  7,  7,  
      7,  7,131,132,  0,  0,  7,  7,  7,  7,131,132,  0,  0,  
      7,  7,  7,  7
},
  {
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  
      0,  0,133,134,  0,  0,133,134,  0,  0,133,134,  0,  0,  
    133,134,  0,  0
},
  {
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136,135,136,135,136,135,136,135,136,135,136,  
    135,136,135,136
},
  {
    137,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    137,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
    137,  0,  7,  7,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    137,  0,  7,  7,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  
      7,  7,  7,  7,  7,  7,  7,  7,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    137,  0,  6,  6,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  7,  7,  0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  
      0,  0,  0,  0,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    137,  7,  6,  6,  5,  5,  5,  5,  0,138,139,140,  0,  0,  
    141,142,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    143,  7,  6,  6,144,145,146,147,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  
      6,  6,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  
      6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
    148,  7,149,150,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  
      6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  5,  5,  5,  5,  
      5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  7,  7,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  6,  6,  6,  6,  7,  7,  7,  7,  0,  0,  
      0,  0,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  
      7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  7,  7,  7,  7,  0,  0,  7,  7,  7,  7,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,  6,  6,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
    151,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
    151,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
    151,  0,  7,  7,  6,  6,  6,  6, 12, 12, 12, 12, 12, 12,  
     12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  
     10, 10, 10, 10, 10, 10, 10, 10,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  
      7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  6,  6,  6,  6,  
      6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  
      5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    151,  0,  7,  7, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12,  
     12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,  
     11, 11, 11, 11,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  
      7,  7,  7,  7,  7,  7,  7,  7,  5,  5,  5,  5,  5,  5,  
      5,  5,  7,  7,  0,  0,  0,  0,  7,  7,  7,  7,  0,  0,  
      0,  0,  7,  7,  7,  7,  0,  0,  6,  6,  6,  6,  6,  6,  
      6,  6,  0,  0,  7,  7,  7,  7,  7,  7,  0,  0,  7,  7,  
      7,  7,  0,  0,  0,  0,  0,  0,  5,  5,  5,  5,  5,  5,  
      5,  5,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  
      7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  7,  7,  
      7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    151,  0, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12,  
     12, 12,  7,  7,  0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  
      0,  0,  0,  0,  6,  6,  6,  6,  0,152,153,  0,  0,154,  
    155,  0,  0,156,  7,  7,  7,  7,157,  0,  0,  0,158,  0,  
      0,159,160,161,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  7,  7,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  6,  6,  6,  6,  6,  6,  
      6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  5,  5,  5,  5,  
      5,  5,  5,  5,  0,  0,  0,  0,  0,  0,  7,  7,  7,  7,  
      0,  0,  0,  0,  0,  0,  7,  7,  0,  0,  6,  6,  6,  6,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    151, 15, 14, 14, 13, 13, 13, 13,  0,162,163,164,  0,  0,  
    165,166,  7,  7,167,168,169,170,171,  0,  0,172,173,174,  
    175,176,  7,  7,  5,  5,  5,  5,  5,  5,  5,  5,  0,177,  
    178,179,180,181,  7,  7,  7,  7,182,183,184,185,186,  0,  
      5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  
      6,  6,187,188,189,  0,  0,190,191,192,  0,193,  7,  7,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    194, 15, 14, 14,195,196,197,198,  0,199,200,201,202,203,  
    204,  0,  6,  6,  6,  6,205,206,207,  0,  0,208,209,210,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  7,  7,211,212,213,214,215,  0,  
      5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    216, 15,217,218,219,220,221,222,  7,  7,223,224,225,226,  
      7,  7,  6,  6,  6,  6,  6,  6,  6,  6,  0,227,228,229,  
      6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
      5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    137,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    137,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
    137,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
      0,  0,  0,  0
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
    151,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
    151,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
    151,  0,  7,  7,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  
      5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  
      9,  9,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  
      8,  8,  8,  8
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
      4,  4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
},
  {
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3
},
  {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  
      2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  
      3,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  
      1,  1,  1,  1
}
};

typedef struct {
	Fax3BaseState b;
} Fax3DecodeState;

typedef struct {
	Fax3BaseState b;
	u_char	*wruns;
	u_char	*bruns;
	short	k;			 
	short	maxk;			 
} Fax3EncodeState;

static	Fax3PreDecode(TIFF *);
static	Fax3Decode(TIFF*, u_char *, int, u_int);
static	int Fax3Decode1DRow(TIFF*, u_char *, int);
static	Fax3PreEncode(TIFF *);
static	Fax3PostEncode(TIFF *);
static	Fax3Encode(TIFF*, u_char *, int, u_int);
static	int Fax3Encode1DRow(TIFF *, u_char *, int);
static	Fax3Close(TIFF *);
static	Fax3Cleanup(TIFF *);
static	void *Fax3SetupState(TIFF *, int);
static	void fillspan(char *, int, int);
static	int findspan(u_char **, int, int, u_char *);
static	int finddiff(u_char *, int, int);
static	void skiptoeol(TIFF *, int);
static	void putbits(TIFF *, u_int, u_int);
static	void putcode(TIFF *, tableentry *);
static	void putspan(TIFF *, int, tableentry *);
extern	int TIFFFlushData1(TIFF *);

TIFFInitCCITTFax3(tif)
	TIFF *tif;
{
	tif->tif_predecode = Fax3PreDecode;
	tif->tif_decoderow = Fax3Decode;
	tif->tif_decodestrip = Fax3Decode;
	tif->tif_decodetile = Fax3Decode;
	tif->tif_preencode = Fax3PreEncode;
	tif->tif_postencode = Fax3PostEncode;
	tif->tif_encoderow = Fax3Encode;
	tif->tif_encodestrip = Fax3Encode;
	tif->tif_encodetile = Fax3Encode;
	tif->tif_close = Fax3Close;
	tif->tif_cleanup = Fax3Cleanup;
	tif->tif_options |=  0x1 ;	 
	tif->tif_flags |=  0x20 ;	 
	return (1);
}

TIFFModeCCITTFax3(tif, isClassF)
	TIFF *tif;
	int isClassF;
{
	if (isClassF)
		tif->tif_options |=  0x1 ;
	else
		tif->tif_options &= ~ 0x1 ;
}

static u_char bitMask[8] =
    { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };



 
static void
skiptoeol(tif, len)
	TIFF *tif;
	int len;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	register int bit = sp->b.bit;
	register int data = sp->b.data;
	int code = 0;

	 
	if (bit == 0)			 
		bit = 8;
	for (;;) {
		switch (bit) {
	again:	 case 0 : code <<= 1 ; if ( data & ( 1 << ( 7 - 0 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 0 + 1 ; break ; } ;
		 case 1 : code <<= 1 ; if ( data & ( 1 << ( 7 - 1 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 1 + 1 ; break ; } ;
		 case 2 : code <<= 1 ; if ( data & ( 1 << ( 7 - 2 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 2 + 1 ; break ; } ;
		 case 3 : code <<= 1 ; if ( data & ( 1 << ( 7 - 3 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 3 + 1 ; break ; } ;
		 case 4 : code <<= 1 ; if ( data & ( 1 << ( 7 - 4 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 4 + 1 ; break ; } ;
		 case 5 : code <<= 1 ; if ( data & ( 1 << ( 7 - 5 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 5 + 1 ; break ; } ;
		 case 6 : code <<= 1 ; if ( data & ( 1 << ( 7 - 6 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 6 + 1 ; break ; } ;
		 case 7 : code <<= 1 ; if ( data & ( 1 << ( 7 - 7 ) ) ) code |= 1 ; len ++ ; if ( code > 0 ) { bit = 7 + 1 ; break ; } ;
		default:
			if (tif->tif_rawcc <= 0)
				return;
			data =  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
			goto again;
		}
		if (len >= 12 && code ==  0x001 )
			break;
		code = len = 0;
	}
	sp->b.bit = bit > 7 ? 0 : bit;	 
	sp->b.data = data;
}

 
static int
nextbit(tif)
	TIFF *tif;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	int bit;

	if (sp->b.bit == 0 && tif->tif_rawcc > 0)
		sp->b.data =  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
	bit =  ( ( sp ) -> b . data & bitMask [ ( sp ) -> b . bit ] ) ;
	if (++(sp->b.bit) > 7)
		sp->b.bit = 0;
	return (bit);
}

static void
invert(cp, n)
	register unsigned char *cp;
	register int n;
{
	while (n-- > 0) {
		*cp = ~*cp;
		cp++;
	}
}

 
static void *
Fax3SetupState(tif, space)
	TIFF *tif;
	int space;
{
	TIFFDirectory *td = &tif->tif_dir;
	Fax3BaseState *sp;
	int cc = space;

	if (td->td_bitspersample != 1) {
		TIFFError(tif->tif_name,
		    "Bits/sample must be 1 for Group 3/4 encoding/decoding");
		return (0);
	}
	if ( ( tif -> tif_dir . td_group3options & 0x1 ) || td -> td_compression == 4 )
		cc += tif->tif_scanlinesize;
	tif->tif_data = malloc(cc);
	if (tif->tif_data ==  0 ) {
		TIFFError(tif->tif_name, "No space for Fax3 state block");
		return (0);
	}
	sp = (Fax3BaseState *)tif->tif_data;
	sp->bitmap = (tif->tif_fillorder != td->td_fillorder ? 
	    TIFFBitRevTable : TIFFNoBitRevTable);
	sp->white = (td->td_photometric ==  1 ) ;
	if ( ( tif -> tif_dir . td_group3options & 0x1 ) || td -> td_compression == 4 ) {
		 
		sp->refline = (u_char *)tif->tif_data + space;
		 memset ( sp -> refline , 0 , tif -> tif_scanlinesize ) ;
		if (sp->white == 1)
			invert(sp->refline, tif->tif_scanlinesize);
	} else
		sp->refline = 0;
	 
	if ( ( ( ( tif ) -> tif_flags & 0x80 ) != 0 ) ) {
		sp->rowbytes = TIFFTileRowSize(tif);
		sp->rowpixels = tif->tif_dir.td_tilewidth;
	} else {
		sp->rowbytes = TIFFScanlineSize(tif);
		sp->rowpixels = tif->tif_dir.td_imagewidth;
	}
	return (sp);
}

 
static
Fax3PreDecode(tif)
	TIFF *tif;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;

	if (sp ==  0 ) {
		sp = (Fax3DecodeState *)Fax3SetupState(tif, sizeof (*sp));
		if (!sp)
			return (0);
	}
	sp->b.bit = 0;			 
	sp->b.data = 0;
	sp->b.tag = G3_1D;
	 
	if ((tif->tif_options &  0x2 ) == 0 ) {
		skiptoeol(tif, 0);
		if ( ( tif -> tif_dir . td_group3options & 0x1 ) )
			 
			sp->b.tag = nextbit(tif) ? G3_1D : G3_2D;
	}
	return (1);
}

 
static void
fillspan(cp, x, count)
	register char *cp;
	register int x, count;
{
	static unsigned char masks[] =
	    { 0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

	if (count <= 0)
		return;
	cp += x>>3;
	if (x &= 7) {			 
		if (count < 8 - x) {
			*cp++ |= masks[count] >> x;
			return;
		}
		*cp++ |= 0xff >> x;
		count -= 8 - x;
	}
	while (count >= 8) {
		*cp++ = 0xff;
		count -= 8;
	}
	*cp |= masks[count];
}

 
static
Fax3Decode(tif, buf, occ, s)
	TIFF *tif;
	u_char *buf;
	int occ;
	u_int s;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;

	 memset ( buf , 0 , occ ) ;		 
	while (occ > 0) {
		if (sp->b.tag == G3_1D) {
			if (!Fax3Decode1DRow(tif, buf, sp->b.rowpixels))
				return (0);
		} else {
			if (!Fax3Decode2DRow(tif, buf, sp->b.rowpixels))
				return (0);
		}
		if ( ( tif -> tif_dir . td_group3options & 0x1 ) ) {
			 
			sp->b.tag = nextbit(tif) ? G3_1D : G3_2D;
			if (sp->b.tag == G3_2D)
				 memcpy ( sp -> b . refline , buf , sp -> b . rowbytes ) ;
		}
		buf += sp->b.rowbytes;
		occ -= sp->b.rowbytes;
	}
	return (1);
}

 
static int
decode_white_run(tif)
	TIFF *tif;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	short state = sp->b.bit;
	short action;
	int runlen = 0;

	for (;;) {
		if (sp->b.bit == 0) {
	nextbyte:
			if (tif->tif_rawcc <= 0)
				return ( - 4 ) ;
			sp->b.data =  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
		}
		action = TIFFFax1DAction[state][sp->b.data];
		state = TIFFFax1DNextState[state][sp->b.data];
		if (action ==  0 )
			goto nextbyte;
		if (action ==  1 )
			return ( - 1 ) ;
		if (action ==  210 )
			return ( - 3 ) ;
		sp->b.bit = state;
		action =  ( TIFFFaxWhiteCodes [ action - 2 ] . runlen ) ;
		runlen += action;
		if (action < 64)
			return (runlen);
	}
	 
}

 
static int
decode_black_run(tif)
	TIFF *tif;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	short state = sp->b.bit + 8;
	short action;
	int runlen = 0;

	for (;;) {
		if (sp->b.bit == 0) {
	nextbyte:
			if (tif->tif_rawcc <= 0)
				return ( - 4 ) ;
			sp->b.data =  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
		}
		action = TIFFFax1DAction[state][sp->b.data];
		state = TIFFFax1DNextState[state][sp->b.data];
		if (action ==  0 )
			goto nextbyte;
		if (action ==  1 )
			return ( - 1 ) ;
		if (action ==  210 )
			return ( - 3 ) ;
		sp->b.bit = state;
		action =  ( TIFFFaxWhiteCodes [ action - 106 ] . runlen ) ;
		runlen += action;
		if (action < 64)
			return (runlen);
		state += 8;
	}
	 
}

 
static int
Fax3Decode1DRow(tif, buf, npels)
	TIFF *tif;
	u_char *buf;
	int npels;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	int x = 0;
	int runlen;
	short action;
	short color = sp->b.white;
	static char module[] = "Fax3Decode1D";

	for (;;) {
		if (color == sp->b.white)
			runlen = decode_white_run(tif);
		else
			runlen = decode_black_run(tif);
		switch (runlen) {
		case  - 4 :
			TIFFError(module,
			    "%s: Premature EOF at scanline %d (x %d)",
			    tif->tif_name, tif->tif_row, x);
			return (0);
		case  - 1 :	 
			 
			TIFFError(tif->tif_name,
			   "%s: Bad code word at scanline %d (x %d)",
			   module, tif->tif_row, x);
			goto done;
		case  - 3 :	 
			TIFFWarning(tif->tif_name,
			    "%s: Premature EOL at scanline %d (x %d)",
			    module, tif->tif_row, x);
			return (1);	 
		}
		if (x+runlen > npels)
			runlen = npels-x;
		if (runlen > 0) {
			if (color)
				fillspan((char *)buf, x, runlen);
			x += runlen;
			if (x >= npels)
				break;
		}
		color = !color;
	}
done:
	 
	if ((tif->tif_options &  0x2 ) == 0 )
		skiptoeol(tif, 0);
	if (tif->tif_options &  0x4 )
		sp->b.bit = 0;
	if ((tif->tif_options &  0x8 ) && ( ( long ) tif -> tif_rawcp & 1 ) )
		(void)  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
	return (x == npels);
}

 

 
static int
decode_uncomp_code(tif)
	TIFF *tif;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	short code;

	do {
		if (sp->b.bit == 0 || sp->b.bit > 7) {
			if (tif->tif_rawcc <= 0)
				return ( 13 ) ;
			sp->b.data =  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
		}
		code = TIFFFaxUncompAction[sp->b.bit][sp->b.data];
		sp->b.bit = TIFFFaxUncompNextState[sp->b.bit][sp->b.data];
	} while (code ==  0 ) ;
	return (code);
}

 
int
Fax3Decode2DRow(tif, buf, npels)
	TIFF *tif;
	u_char *buf;
	int npels;
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	int a0 = 0;
	int b1 = 0;
	int b2 = 0;
	int run1, run2;		 
	short mode;
	short color = sp->b.white;
	static char module[] = "Fax3Decode2D";

	do {
		if (sp->b.bit == 0 || sp->b.bit > 7) {
			if (tif->tif_rawcc <= 0) {
			    TIFFError(module,
				"%s: Premature EOF at scanline %d",
			        tif->tif_name, tif->tif_row);
			    return (0);
			}
			sp->b.data =  ( ( tif ) -> tif_rawcc -- , ( sp ) -> b . bitmap [ * ( u_char * ) ( tif ) -> tif_rawcp ++ ] ) ;
		}
		mode = TIFFFax2DMode[sp->b.bit][sp->b.data];
		sp->b.bit = TIFFFax2DNextState[sp->b.bit][sp->b.data];
		switch (mode) {
		case  0 :
			break;
		case  1 :
			if (a0 || color || ! ( ( ( ( sp -> b . refline ) [ ( 0 ) >> 3 ] ) >> ( 7 - ( ( 0 ) & 7 ) ) ) & 1 ) ) {
				b1 = finddiff(sp->b.refline, a0, npels);
				if (color ==  ( ( ( ( sp -> b . refline ) [ ( b1 ) >> 3 ] ) >> ( 7 - ( ( b1 ) & 7 ) ) ) & 1 ) )
					b1 = finddiff(sp->b.refline, b1, npels);
			} else
				b1 = 0;
			b2 = finddiff(sp->b.refline, b1, npels);
			if (color)
				fillspan((char *)buf, a0, b2 - a0);
			a0 += b2 - a0;
			break;
		case  2 :
			if (color == sp->b.white) {
				run1 = decode_white_run(tif);
				run2 = decode_black_run(tif);
			} else {
				run1 = decode_black_run(tif);
				run2 = decode_white_run(tif);
			}
			 
			if (a0 + run1 > npels)
				run1 = npels - a0;
			if (color)
				fillspan((char *)buf, a0, run1);
			a0 += run1;
			if (a0 + run2 > npels)
				run2 = npels - a0;
			if (!color)
				fillspan((char *)buf, a0, run2);
			a0 += run2;
			break;
		case  6 :
		case  7 :
		case  8 :
		case  9 :
		case  5 :
		case  4 :
		case  3 :
			if (a0 || color || ! ( ( ( ( sp -> b . refline ) [ ( 0 ) >> 3 ] ) >> ( 7 - ( ( 0 ) & 7 ) ) ) & 1 ) ) {
				b1 = finddiff(sp->b.refline, a0, npels);
				if (color ==  ( ( ( ( sp -> b . refline ) [ ( b1 ) >> 3 ] ) >> ( 7 - ( ( b1 ) & 7 ) ) ) & 1 ) )
					b1 = finddiff(sp->b.refline, b1, npels);
			} else
				b1 = 0;
			b1 += mode -  6 ;
			if (color)
				fillspan((char *)buf, a0, b1 - a0);
			color = !color;
			a0 += b1 - a0;
			break;
	        case  10 :
			 
			do {
				mode = decode_uncomp_code(tif);
				switch (mode) {
				case  2 :
				case  3 :
				case  4 :
				case  5 :
				case  6 :
					run1 = mode -  1 ;
					fillspan((char *)buf, a0+run1-1, 1);
					a0 += run1;
					break;
				case  7 :
					a0 += 5;
					break;
				case  8 :
				case  9 :
				case  10 :
				case  11 :
				case  12 :
					run1 = mode -  8 ;
					a0 += run1;
					color = nextbit(tif) ?
					    !sp->b.white : sp->b.white;
					break;
				case  14 :
					TIFFError(module,
				"%s: Bad uncompressed code word at scanline %d",
					    tif->tif_name, tif->tif_row);
					goto bad;
				case  13 :
					TIFFError(module,
					    "%s: Premature EOF at scanline %d",
					    tif->tif_name, tif->tif_row);
					return (0);
				}
			} while (mode <  8 ) ;
			break;
	        case  12 :
			if ((tif->tif_options &  0x2 ) == 0 ) {
				TIFFWarning(tif->tif_name,
				    "%s: Premature EOL at scanline %d (x %d)",
				    module, tif->tif_row, a0);
				skiptoeol(tif, 7);	 
				return (1);		 
			}
			 
	        case  11 :
			TIFFError(tif->tif_name,
			    "%s: Bad 2D code word at scanline %d",
			    module, tif->tif_row);
			goto bad;
	        default:
			TIFFError(tif->tif_name,
			    "%s: Panic, bad decoding state at scanline %d",
			    module, tif->tif_row);
			return (0);
		}
	} while (a0 < npels);
bad:
	 
	if ((tif->tif_options &  0x2 ) == 0 )
		skiptoeol(tif, 0);
	return (a0 >= npels);
}

 

 
static void
putbits(tif, bits, length)
	TIFF *tif;
	u_int bits, length;
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;
	static int mask[9] =
	    { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

	while (length > sp->bit) {
		sp->data |= bits >> (length - sp->bit);
		length -= sp->bit;
		 { if ( ( tif ) -> tif_rawcc >= ( tif ) -> tif_rawdatasize ) ( void ) TIFFFlushData1 ( tif ) ; * ( tif ) -> tif_rawcp ++ = ( sp ) -> bitmap [ ( sp ) -> data ] ; ( tif ) -> tif_rawcc ++ ; ( sp ) -> data = 0 ; ( sp ) -> bit = 8 ; } ;
	}
	sp->data |= (bits & mask[length]) << (sp->bit - length);
	sp->bit -= length;
	if (sp->bit == 0)
		 { if ( ( tif ) -> tif_rawcc >= ( tif ) -> tif_rawdatasize ) ( void ) TIFFFlushData1 ( tif ) ; * ( tif ) -> tif_rawcp ++ = ( sp ) -> bitmap [ ( sp ) -> data ] ; ( tif ) -> tif_rawcc ++ ; ( sp ) -> data = 0 ; ( sp ) -> bit = 8 ; } ;
}

 
static void
putcode(tif, te)
	TIFF *tif;
	tableentry *te;
{
	putbits(tif, te->code, te->length);
}

 
static void
putspan(tif, span, tab)
	TIFF *tif;
	int span;
	tableentry *tab;
{
	while (span >= 2624) {
		tableentry *te = &tab[63 + (2560>>6)];
		putcode(tif, te);
		span -= te->runlen;
	}
	if (span >= 64) {
		tableentry *te = &tab[63 + (span>>6)];
		 ( void ) ( ( te -> runlen == 64 * ( span >> 6 ) ) || ( __assert ( "te->runlen == 64*(span>>6)" , "tif_fax3.c" , 783 ) , 0 ) ) ;
		putcode(tif, te);
		span -= te->runlen;
	}
	putcode(tif, &tab[span]);
}

 
void
Fax3PutEOL(tif)
	TIFF *tif;
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

	if (tif->tif_dir.td_group3options &  0x4 ) {
		 
		int align = 8 - ( ( tif -> tif_dir . td_group3options & 0x1 ) ? 3 : 4 ) ;
		if (align != sp->bit) {
			if (align > sp->bit)
				align = sp->bit + (8 - align);
			else
				align = sp->bit - align;
			putbits(tif, 0, align);
		}
	}
	putbits(tif,  0x001 , 12 ) ;
	if ( ( tif -> tif_dir . td_group3options & 0x1 ) )
		putbits(tif, sp->tag == G3_1D, 1);
}

static u_char zeroruns[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,	 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
};
static u_char oneruns[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	 
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,	 
};

 
static
Fax3PreEncode(tif)
	TIFF *tif;
{
	Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;

	if (sp ==  0 ) {
		sp = (Fax3EncodeState *)Fax3SetupState(tif, sizeof (*sp));
		if (!sp)
			return (0);
		if (sp->b.white == 0) {
			sp->wruns = zeroruns;
			sp->bruns = oneruns;
		} else {
			sp->wruns = oneruns;
			sp->bruns = zeroruns;
		}
	}
	sp->b.bit = 8;
	sp->b.data = 0;
	sp->b.tag = G3_1D;
	if ( ( tif -> tif_dir . td_group3options & 0x1 ) ) {
		float res = tif->tif_dir.td_yresolution;
		 
		if (tif->tif_dir.td_resolutionunit ==  3 )
			res = (res * .3937) / 2.54;	 
		sp->maxk = (res > 150 ? 4 : 2);
		sp->k = sp->maxk-1;
	} else
		sp->k = sp->maxk = 0;
	return (1);
}

 
static int
Fax3Encode1DRow(tif, bp, bits)
	TIFF *tif;
	u_char *bp;
	int bits;
{
	Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;
	int bs = 0, span;

	for (;;) {
		span = findspan(&bp, bs, bits, sp->wruns);	 
		putspan(tif, span, TIFFFaxWhiteCodes);
		bs += span;
		if (bs >= bits)
			break;
		span = findspan(&bp, bs, bits, sp->bruns);	 
		putspan(tif, span, TIFFFaxBlackCodes);
		bs += span;
		if (bs >= bits)
			break;
	}
	return (1);
}

static tableentry horizcode =
    { 3, 0x1 };		 
static tableentry passcode =
    { 4, 0x1 };		 
static tableentry vcodes[7] = {
    { 7, 0x03 },	 
    { 6, 0x03 },	 
    { 3, 0x03 },	 
    { 1, 0x1 },		 
    { 3, 0x2 },		 
    { 6, 0x02 },	 
    { 7, 0x02 }		 
};

 
int
Fax3Encode2DRow(tif, bp, rp, bits)
	TIFF *tif;
	u_char *bp, *rp;
	int bits;
{
	short white = ((Fax3BaseState *)tif->tif_data)->white;
	int a0 = 0;
	int a1 = ( ( ( ( ( bp ) [ ( 0 ) >> 3 ] ) >> ( 7 - ( ( 0 ) & 7 ) ) ) & 1 ) != white ? 0 : finddiff ( bp , 0 , bits ) ) ;
	int a2 = 0;
	int b1 = ( ( ( ( ( rp ) [ ( 0 ) >> 3 ] ) >> ( 7 - ( ( 0 ) & 7 ) ) ) & 1 ) != white ? 0 : finddiff ( rp , 0 , bits ) ) ;
	int b2 = 0;

	for (;;) {
		b2 = finddiff(rp, b1, bits);
		if (b2 >= a1) {
			int d = b1 - a1;
			if (!(-3 <= d && d <= 3)) {	 
				a2 = finddiff(bp, a1, bits);
				putcode(tif, &horizcode);
				if (a0+a1 == 0 ||  ( ( ( ( bp ) [ ( a0 ) >> 3 ] ) >> ( 7 - ( ( a0 ) & 7 ) ) ) & 1 ) == white ) {
					putspan(tif, a1-a0, TIFFFaxWhiteCodes);
					putspan(tif, a2-a1, TIFFFaxBlackCodes);
				} else {
					putspan(tif, a1-a0, TIFFFaxBlackCodes);
					putspan(tif, a2-a1, TIFFFaxWhiteCodes);
				}
				a0 = a2;
			} else {			 
				putcode(tif, &vcodes[d+3]);
				a0 = a1;
			}
		} else {				 
			putcode(tif, &passcode);
			a0 = b2;
		}
		if (a0 >= bits)
			break;
		a1 = finddiff(bp, a0, bits);
		b1 = finddiff(rp, a0, bits);
		if ( ( ( ( ( rp ) [ ( b1 ) >> 3 ] ) >> ( 7 - ( ( b1 ) & 7 ) ) ) & 1 ) == ( ( ( ( bp ) [ ( a0 ) >> 3 ] ) >> ( 7 - ( ( a0 ) & 7 ) ) ) & 1 ) )
			b1 = finddiff(rp, b1, bits);
	}
	return (1);
}

 
static int
Fax3Encode(tif, bp, cc, s)
	TIFF *tif;
	u_char *bp;
	int cc;
	u_int s;
{
	Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;

	while (cc > 0) {
		Fax3PutEOL(tif);
		if ( ( tif -> tif_dir . td_group3options & 0x1 ) ) {
			if (sp->b.tag == G3_1D) {
				if (!Fax3Encode1DRow(tif, bp, sp->b.rowpixels))
					return (0);
				sp->b.tag = G3_2D;
			} else {
				if (!Fax3Encode2DRow(tif, bp, sp->b.refline, sp->b.rowpixels))
					return (0);
				sp->k--;
			}
			if (sp->k == 0) {
				sp->b.tag = G3_1D;
				sp->k = sp->maxk-1;
			} else
				 memcpy ( sp -> b . refline , bp , sp -> b . rowbytes ) ;
		} else {
			if (!Fax3Encode1DRow(tif, bp, sp->b.rowpixels))
				return (0);
		}
		bp += sp->b.rowbytes;
		cc -= sp->b.rowbytes;
	}
	return (1);
}

static int
Fax3PostEncode(tif)
	TIFF *tif;
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

	if (sp->bit != 8)
		 { if ( ( tif ) -> tif_rawcc >= ( tif ) -> tif_rawdatasize ) ( void ) TIFFFlushData1 ( tif ) ; * ( tif ) -> tif_rawcp ++ = ( sp ) -> bitmap [ ( sp ) -> data ] ; ( tif ) -> tif_rawcc ++ ; ( sp ) -> data = 0 ; ( sp ) -> bit = 8 ; } ;
	return (1);
}

static
Fax3Close(tif)
	TIFF *tif;
{
	if ((tif->tif_options &  0x1 ) == 0 ) {	 
		int i;
		for (i = 0; i < 6; i++)
			Fax3PutEOL(tif);
		(void) Fax3PostEncode(tif);
	}
}

static
Fax3Cleanup(tif)
	TIFF *tif;
{
	if (tif->tif_data) {
		free(tif->tif_data);
		tif->tif_data =  0 ;
	}
}

 

 
static int
findspan(bpp, bs, be, tab)
	u_char **bpp;
	int bs, be;
	register u_char *tab;
{
	register u_char *bp = *bpp;
	register int bits = be - bs;
	register int n, span;

	 
	if (bits > 0 && (n = (bs & 7))) {
		span = tab[(*bp << n) & 0xff];
		if (span > 8-n)		 
			span = 8-n;
		if (n+span < 8)		 
			goto done;
		bits -= span;
		bp++;
	} else
		span = 0;
	 
	while (bits >= 8) {
		n = tab[*bp];
		span += n;
		bits -= n;
		if (n < 8)		 
			goto done;
		bp++;
	}
	 
	if (bits > 0) {
		n = tab[*bp];
		span += (n > bits ? bits : n);
	}
done:
	*bpp = bp;
	return (span);
}

 
static int
finddiff(cp, bs, be)
	u_char *cp;
	int bs, be;
{
	cp += bs >> 3;			 
	return (bs + findspan(&cp, bs, be,
	    (*cp & (0x80 >> (bs&7))) ? oneruns : zeroruns));
}
#ident "acomp: (CCS) 2.0  07/24/92 "
