#ifndef MTOOLS_MTOOLS_H
#define MTOOLS_MTOOLS_H

#include "msdos.h"

#if defined(OS_sco3)
#define MAXPATHLEN 1024
#include <signal.h>
extern int lockf(int, int, off_t);  /* SCO has no proper include file for lockf */
#endif 


typedef struct device {
	const char *name;       /* full path to device */

	char drive;	       	/* the drive letter */
	int fat_bits;		/* FAT encoding scheme */

	int mode;	       	/* any special open() flags */
	unsigned int tracks;	/* tracks */
	unsigned int heads;	/* heads */
	unsigned int sectors;	/* sectors */

	off_t offset;	       	/* skip this many bytes */

	int partition;
	int scsi;

	int privileged;

	/* rarely used stuff */
	int nolock;	

	/* Solaris only stuff */
	int vold;


	/* Linux only stuff */
	unsigned int ssize;
	unsigned int use_2m;
	int use_xdf;

	char *precmd;		/* command to be executed before opening
				 * the drive */

	/* internal variables */
	int file_nr;		/* used during parsing */
	int hidden;		/* number of hidden sectors. Used for
				 * mformatting partitioned devices */
} device_t;

#include "stream.h"


extern const char *short_illegals, *long_illegals;

size_t maximize(size_t *target, long max);
size_t minimize(size_t *target, long max);

int init_geom(int fd, struct device *dev, struct device *orig_dev,
	      struct stat *stat);

int readwrite_sectors(int fd, /* file descriptor */
		      int *drive,
		      int rate,
		      int seektrack,
		      int track, int head, int sector, int size, /* address */
		      char *data, 
		      int bytes,
		      int direction,
		      int retries);

int lock_dev(int fd, int mode, struct device *dev);

char *unix_normalize (char *ans, char *name, char *ext);
void dir_write(Stream_t *Dir, int num, struct directory *dir);
char *dos_name(char *filename, int verbose, int *mangled, char *buffer);
struct directory *mk_entry(const char *filename, char attr,
			   unsigned int fat, size_t size, long date,
			   struct directory *ndir);
int copyfile(Stream_t *Source, Stream_t *Target);
unsigned long getfree(Stream_t *Stream);
unsigned long getfreeMin(Stream_t *Stream, size_t ref);

FILE *opentty(int mode);

int is_dir(Stream_t *Dir, char *path);
Stream_t *descend(Stream_t *Dir, char *path, int barf,char *outname, int lock);

int dir_grow(Stream_t *Dir, int size);
int match(const char *, const char *, char *, int);
int hasWildcards(const char *string);

Stream_t *new_file(char *filename, char attr,
		   unsigned int fat, size_t size, long date,
		   struct directory *ndir);
char *unix_name(char *name, char *ext, char Case, char *answer);
void *safe_malloc(size_t size);
Stream_t *open_filter(Stream_t *Next);

extern int got_signal;
void setup_signal(void);


#define SET_INT(target, source) \
if(source)target=source


//UNUSED(static inline int compare (long ref, long testee))
static int compare (long ref, long testee)
{
	return (ref && ref != testee);
}

Stream_t *GetFs(Stream_t *Fs);
Stream_t *find_device(char drive, int mode, struct device *out_dev,
		      struct bootsector *boot,
		      char *name, int *media);

struct directory *labelit(char *dosname,
			  char *longname,
			  void *arg0,
			  struct directory *dir);

char *label_name(char *filename, int verbose, 
		 int *mangled, char *ans);

/* environmental variables */
extern int mtools_skip_check;
extern int mtools_fat_compatibility;
extern int mtools_ignore_short_case;
extern int mtools_no_vfat;
extern int mtools_numeric_tail;
extern int mtools_dotted_dir;
extern int mtools_twenty_four_hour_clock;
extern char *mtools_date_string;
extern int mtools_rate_0, mtools_rate_any, mtools_raw_tty;

void read_config(void);
extern struct device *devices;
extern struct device const_devices[];
extern const int nr_const_devices;

#define New(type) ((type*)(malloc(sizeof(type))))
#define Grow(adr,n,type) ((type*)(realloc((char *)adr,n*sizeof(type))))
#define Free(adr) (free((char *)adr));
#define NewArray(size,type) ((type*)(calloc((size),sizeof(type))))

void mattrib(int argc, char **argv, int type);
void mbadblocks(int argc, char **argv, int type);
void mcd(int argc, char **argv, int type);
void mcopy(int argc, char **argv, int type);
void mdel(int argc, char **argv, int type);
void mdir(int argc, char **argv, int type);
void mformat(int argc, char **argv, int type);
void minfo(int argc, char **argv, int type);
void mlabel(int argc, char **argv, int type);
void mmd(int argc, char **argv, int type);
void mmount(int argc, char **argv, int type);
void mmove(int argc, char **argv, int type);
void mpartition(int argc, char **argv, int type);
void mtoolstest(int argc, char **argv, int type);
void mzip(int argc, char **argv, int type);

void init_privs(void);
void reclaim_privs(void);
void drop_privs(void);
void destroy_privs(void);
void closeExec(int fd);

extern const char *progname;

Stream_t *subdir(Stream_t *, char *pathname, int lock);
void precmd(struct device *dev);

void print_sector(char *message, unsigned char *data);

#ifdef USING_NEW_VOLD
char *getVoldName(struct device *dev, char *name);
#endif


#endif
