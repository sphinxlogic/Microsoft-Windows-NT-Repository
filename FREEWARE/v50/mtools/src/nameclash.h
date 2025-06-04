#ifndef MTOOLS_NAMECLASH_H
#define MTOOLS_NAMECLASH_H

#include "stream.h"

typedef enum clash_action {
	NAMEMATCH_NONE,
	NAMEMATCH_AUTORENAME,
	NAMEMATCH_QUIT,
	NAMEMATCH_SKIP,
	NAMEMATCH_RENAME,
	NAMEMATCH_PRENAME, /* renaming of primary name */
	NAMEMATCH_OVERWRITE,
	NAMEMATCH_ERROR,
	NAMEMATCH_SUCCESS,
	NAMEMATCH_GREW
} clash_action;

/* clash handling structure */
typedef struct ClashHandling_t {
	clash_action action[2];
	clash_action namematch_default[2];
		
	int nowarn;	/* Don't ask, just do default action if name collision*/
	int got_slots;
	int mod_time;
	/* unsigned int dot; */
	char *myname;
	unsigned char *dosname;
	int single;

	int use_longname;
	int ignore_entry;
	int source; /* to prevent the source from overwriting itself */
	char * (*name_converter)(char *filename, int verbose, 
				 int *mangled, char *ans);
} ClashHandling_t;

/* write callback */
typedef struct directory * (write_data_callback)(char *,char *,
						  void *, struct directory *);

int mwrite_one(Stream_t *Dir,
	       char *argname,
	       char *shortname,
	       write_data_callback *cb,
	       void *arg,
	       ClashHandling_t *ch);

int handle_clash_options(ClashHandling_t *ch, char c);
void init_clash_handling(ClashHandling_t *ch);

#endif
