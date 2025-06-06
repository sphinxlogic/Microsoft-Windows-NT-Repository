/*
 * SPP.H -- Language definitions for interfacing SPP to C and C to UNIX.
 * Note that many of the definitions must agree with those in the SPP
 * compiler and with <iraf.h> and <mach.h>.
 */

/* Assorted machine constants. [MACHDEP]
 * Use osb$zzeps.f to compute the machine epsilon.
 */
#define	OSOK		1		/* normal successful completion	*/

/* Oft used constants.
 */
#define	SZ_LINE		161
#define	SZ_FNAME	63
#define	SZ_PATHNAME	127
#define	SZ_COMMAND	1024
#define	EOS		'\0'
#define	ERR		(-1)
#define	OK		0
#define	YES		1
#define	NO		0
#define	MAX_DIGITS	25
#define	min(a,b)	(((a)<(b))?(a):(b))
#define	max(a,b)	(((a)>(b))?(a):(b))

# ifndef NULL
#define	NULL		0
# endif

# ifndef EOF
#define	EOF		(-1)
#endif

/* SPP constants.
 */
#define	XEOS		0
#define	XERR		(-1)
#define	XEOF		(-2)
#define XBOF		(-3)
#define	XOK		0
#define	XNO		0
#define	XYES		1

#define	BOFL		(-3L)
#define EOFL		(-2L)

/* SPP datatypes. (potentially MACHDEP)
 */
# ifndef XCHAR
#define	XCHAR		short
# endif

# ifndef XINT
#define	XINT		int
# endif

#define	PKCHAR		XCHAR
#define XUBYTE		unsigned char
#define	XBOOL		int	
#define	XSHORT		short
#define	XUSHORT		unsigned short
#define	XLONG		long
#define	XREAL		float
#define	XDOUBLE		double
#define XCOMPLEX	struct cplx
#define	XSTRUCT		int
#define	XPOINTER	int

struct cplx {
	float	r;
	float	i;
};

#define	TY_BOOL		1		/* SPP datatype codes		*/
#define	TY_CHAR		2
#define	TY_SHORT	3
#define	TY_INT		4
#define	TY_LONG		5
#define	TY_REAL		6
#define	TY_DOUBLE	7
#define	TY_COMPLEX	8
#define	TY_STRUCT	9
#define	TY_POINTER	10


/* File I/O constants.
 */
#define	READ_ONLY	1		/* file access modes		*/
#define	READ_WRITE	2
#define	WRITE_ONLY	3
#define	APPEND		4
#define	NEW_FILE	5
		
#define	TEXT_FILE	11		/* file types			*/
#define	BINARY_FILE	12
#define	DIRECTORY_FILE	13

#define	CLIN		1
#define	CLOUT		2
#define	STDIN		3
#define	STDOUT		4
#define	STDERR		5
#define	STDGRAPH	6
#define	STDIMAGE	7
#define	STDPLOT		8
#define	PSIOCTRL	9

/* Filename Mapping definitions.
 */

#define	VFN_READ	1		/* VFN access modes for VFNOPEN	*/
#define	VFN_WRITE	2
#define	VFN_UNMAP	3

#define	VFN_NOUPDATE	0		/* update flag for VFNCLOSE */
#define	VFN_UPDATE	1

/* Oft referenced functions.
 */
XCHAR	*c_sppstr();
XCHAR	*c_strupk();
char	*c_strpak();
#define	D_spp
