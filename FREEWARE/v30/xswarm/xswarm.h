/* Constants */
#define TRUE 1
#define FALSE 0
#define STD_STR	100
#define BEES	20	/* number of bees */
#define REPEAT 3	/* number of time positions recorded */
#define BEEACC	3	/* acceleration of bees */
#define WASPACC 5	/* maximum acceleration of wasp */
#define BEEVEL	11	/* maximum bee velocity */
#define WASPVEL 12	/* maximum wasp velocity */
#define DELAY	40000	/* microsecond delay between updates */
#define WINWIDTH 512	/* default window width */
#define WINHEIGHT 512	/* default window height */
#define BORDER	50	/* wasp won't go closer than this to the edges */

/* Error Codes */
#define FATAL	-1
#define WARNING	-2

/* Macros */
#define X(t,b)	(x[t*bees + b])		/* addressing into dynamic array */
#define Y(t,b)	(y[t*bees + b])		/* addressing into dynamic array */

#ifdef VMS
#define RAND(v) ((rand() % v) - (v/2)) /* random number around 0 */
#else
#define RAND(v)	((random() % v) - (v/2)) /* random number around 0 */
#endif

/* Type Definitions */
typedef struct _disp
{
	Window	win;
	Display	*dpy;
	char	*dname;
	long	wasp, bee, bg; /* colors */
	GC	wasp_gc;
	GC	bee_gc;
	GC	erase_gc;
	GC	wasp_xor_gc;
	GC	bee_xor_gc;
	Atom	kill_atom, protocol_atom;
	Colormap cmap;
} disp;

/* Function Prototypes */
void    nap();
void	Usage();
void	HandleError();
long	GetColor();

/* Global Variables */

/* X related */
int		winX, winY;
unsigned int	winW, winH;
disp		display;

/* animation related */
int	repeat = REPEAT;	/* number of time steps recorded  */
int	bees = BEES;		/* number of bees */
int	wasp_vel = WASPVEL;	/* maximum wasp speed */
int	bee_vel = BEEVEL;	/* maximum bee speed */
int	wasp_acc = WASPACC;	/* maximum wasp acceleration */
int	bee_acc = BEEACC;	/* bee acceleration */
int	delay = DELAY;		/* delay between updates, in microseconds */
int	border = BORDER;	/* border limiting wasp travel */
char	*wasp_color = NULL;
char	*bee_color = NULL;
char	*bg_color = NULL;
char	stop = FALSE;
int	xor = FALSE;		/* use GXxor if TRUE */
int	verbose = FALSE;	/* display settings if TRUE */

