/*
 * mk_direntry.c
 * Make new directory entries, and handles name clashes
 *
 */

/*
 * This file is used by those commands that need to create new directory entries
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "vfat.h"
#include "nameclash.h"
#include "fs.h"

static inline int ask_rename(ClashHandling_t *ch,
			     char *longname, int isprimary, char *argname)
{
	char shortname[13];
	int mangled;

	/* TODO: Would be nice to suggest "autorenamed" version of name, press 
	 * <Return> to get it.
	 */
#if 0
	fprintf(stderr,"Entering ask_rename, isprimary=%d.\n", isprimary);
#endif

	if(!opentty(0))
		return 0;

#define maxsize (isprimary ?  MAX_VNAMELEN+1 : 11+1)
#define name (isprimary ? argname : shortname)

	mangled = 0;
	do {
		fprintf(stderr, "New %s name for \"%s\": ",
			isprimary ? "primary" : "secondary", longname);
		fflush(stderr);
		if (! fgets(name, maxsize, opentty(0)))
			return 0;

		/* Eliminate newline(s) in the file name */
		name[strlen(name)-1]='\0';
		if (!isprimary)
			ch->name_converter(shortname,0, &mangled, argname);
	} while (mangled & 1);
	return 1;
#undef maxsize
#undef name
}

static inline clash_action ask_namematch(char *name, int isprimary, 
					 ClashHandling_t *ch, int no_overwrite,
					 int reason)
{
	char ans[10];
	clash_action a;
	int perm;
	char unix_shortname[13];


#define EXISTS 0
#define RESERVED 1
#define ILLEGALS 2

	static const char *reasons[]= {
		"already exists",
		"is reserved",
		"contains illegal character(s)"};


	if (!isprimary)
		name = unix_normalize(unix_shortname, name, name+8);

	a = ch->action[isprimary];

	if(!opentty(1)) {
		/* no default, and no tty either . Skip the troublesome file */
		if(a == NAMEMATCH_NONE)
			a = NAMEMATCH_SKIP;
		return a;
	}

	perm = 0;
	while (a == NAMEMATCH_NONE) {
		fprintf(stderr, "%s file name \"%s\" %s.\n",
			isprimary ? "Long" : "Short", name, reasons[reason]);
		fprintf(stderr,
			"a)utorename A)utorename-all r)ename R)ename-all ");
		if(!no_overwrite)
			fprintf(stderr,"o)verwrite O)verwrite-all");
		fprintf(stderr,
			"\ns)kip S)kip-all q)uit (aArR");
		if(!no_overwrite)
			fprintf(stderr,"oO");
		fprintf(stderr,"sSq): ");
		fflush(stderr);
		fflush(opentty(1));
		if (mtools_raw_tty) {
			ans[0] = fgetc(opentty(1));
			fputs("\n", stderr);
		} else {
			fgets(ans, 9, opentty(0));
		}
		perm = isupper(ans[0]);
		switch(tolower(ans[0])) {
			case 'a':
				a = NAMEMATCH_AUTORENAME;
				break;
			case 'r':
				if(isprimary)
					a = NAMEMATCH_PRENAME;
				else
					a = NAMEMATCH_RENAME;
				break;
			case 'o':
				if(no_overwrite)
					continue;
				a = NAMEMATCH_OVERWRITE;
				break;
			case 's':
				a = NAMEMATCH_SKIP;
				break;
			case 'q':
				perm = 0;
				a = NAMEMATCH_QUIT;
				break;
			default:
				perm = 0;
		}
	}

	/* Keep track of this action in case this file collides again */
	ch->action[isprimary]  = a;
	if (perm)
		ch->namematch_default[isprimary] = a;

	/* if we were asked to overwrite be careful. We can't set the action
	 * to overwrite, else we get won't get a chance to specify another
	 * action, should overwrite fail. Indeed, we'll be caught in an
	 * infinite loop because overwrite will fail the same way for the
	 * second time */
	if(a == NAMEMATCH_OVERWRITE)
		ch->action[isprimary] = NAMEMATCH_NONE;
	return a;
}

/* Returns:
 * 2 if file is to be overwritten
 * 1 if file was renamed
 * 0 if it was skipped
 *
 * If a short name is involved, handle conversion between the 11-character
 * fixed-length record DOS name and a literal null-terminated name (e.g.
 * "COMMAND  COM" (no null) <-> "COMMAND.COM" (null terminated)).
 *
 * Also, immediately copy the original name so that messages can use it.
 */
static inline clash_action process_namematch(char *name,
					     char *longname,
					     int isprimary,
					     ClashHandling_t *ch,
					     int no_overwrite,
					     int reason)
{
	clash_action action;

#if 0
	fprintf(stderr,
		"process_namematch: name=%s, default_action=%d, ask=%d.\n",
		name, default_action, ch->ask);
#endif

	action = ask_namematch(name, isprimary, ch, no_overwrite, reason);

	switch(action){
	case NAMEMATCH_QUIT:
		got_signal = 1;
		return NAMEMATCH_SKIP;
	case NAMEMATCH_SKIP:
		return NAMEMATCH_SKIP;
	case NAMEMATCH_RENAME:
	case NAMEMATCH_PRENAME:
		/* We need to rename the file now.  This means we must pass
		 * back through the loop, a) ensuring there isn't a potential
		 * new name collision, and b) finding a big enough VSE.
		 * Change the name, so that it won't collide again.
		 */
		ask_rename(ch, longname, isprimary, name);
		return action;
	case NAMEMATCH_AUTORENAME:
		/* Very similar to NAMEMATCH_RENAME, except that we need to
		 * first generate the name.
		 * TODO: Remember previous name so we don't
		 * keep trying the same one.
		 */
		if (isprimary) {
			autorename_long(name, 1);
			return NAMEMATCH_PRENAME;
		} else {
			autorename_short(name, 1);
			return NAMEMATCH_RENAME;
		}
	case NAMEMATCH_OVERWRITE:
		if(no_overwrite)
			return NAMEMATCH_SKIP;
		else
			return NAMEMATCH_OVERWRITE;
	default:
		return NAMEMATCH_NONE;
	}
}


static void clear_scan(char *longname, int use_longname, struct scan_state *s)
{
	s->shortmatch = s->longmatch = s->slot = -1;
	s->free_size = s->got_slots = s->free_start = 0;

	if (use_longname & 1)
                s->size_needed = 2 + (strlen(longname)/VSE_NAMELEN);
	else
                s->size_needed = 1;
}


static int scan_dir(Stream_t *Dir,
		    char *dosname,
		    char *longname,
		    struct vfat_state *vsp,
		    struct scan_state *ssp, 
		    int use_longname,
		    int ignore_entry)
{
	int entry;
	struct directory dir;
	int vse_start;
	int ignore_match;
	int ret;
	int address;
	char readlongname[VBUFSIZE];
	char readshortname[13];
	ignore_match = (ignore_entry == -2 );

	vse_start = -1;
	clear_scan(longname, use_longname, ssp);
	clear_vfat(vsp);
	entry = 0;
	ssp->match_free = 0;
	while(1){
		if(!ssp->got_slots){
			ssp->free_start = entry;
			ssp->free_size = 0;
		}
		if (ssp->longmatch > -1)
			return 1; /* Name match, process and try again */
		ret=vfat_lookup(Dir, &dir, &entry, &vse_start, 0,
				ACCEPT_PLAIN | ACCEPT_DIR | ACCEPT_LABEL |
				MATCH_ANY,
				0, readshortname, readlongname);
		if(!ssp->got_slots){
			ssp->free_size = vse_start - ssp->free_start;
			if(ssp->free_size >= ssp->size_needed){
				/* enough entries */
				ssp->got_slots = 1;
				ssp->slot = ssp->free_start + 
				    ssp->size_needed - 1;
			}
		}
		if(!ssp->got_slots && ssp->slot == -1 && ssp->free_size > 1)
			/* If there aren't enough consecutive entries
			 * to store the long file name, perhaps just
			 * store it with the short name? 
			 */
			ssp->slot = ssp->free_start;
		if(ret)
			break;

		/* labels never match, neither does the ignored entry */
		if( (dir.attr & 0x8) || (entry - 1 == ignore_entry) )
			continue;
		
		/* check long name */
		if((*readlongname && 
		    !strncasecmp(readlongname, longname, VBUFSIZE-1)) ||
		   (*readshortname &&
		    !strncasecmp(readshortname, longname, VBUFSIZE-1)))
			ssp->longmatch = entry - 1;

		/* Long name or not, always check for short name match */
		if (!ignore_match &&
		    !strncasecmp(dosname, dir.name, 8) &&
		    !strncasecmp(dosname+8, dir.ext, 3))
			ssp->shortmatch = entry - 1;
	}

	if (ssp->shortmatch > -1)
		return 1;
	ssp->max_entry = vse_start;
	if (ssp->got_slots)
		return 6;	/* Success */

	/* Need more room.  Can we grow the directory? */
	GET_DATA(Dir,0,0,0,&address);
	if (address)
		return 5;	/* OK, try to grow the directory */

	/* no '.' entry means root directory */
	fprintf(stderr, "No directory slots\n");
	return -1;

} /* scan_dir */


static int contains_illegals(const char *string, const char *illegals)
{
	for(; *string ; string++)
		if((*string < ' ' && *string != '\005' && !(*string & 0x80)) ||
		   strchr(illegals, *string))
			return 1;
	return 0;
}

static int is_reserved(char *ans, int islong)
{
	int i;
	static const char *dev3[] = {"CON", "AUX", "PRN", "NUL", "   "};
	static const char *dev4[] = {"COM", "LPT" };

	for (i = 0; i < sizeof(dev3)/sizeof(*dev3); i++)
		if (!strncasecmp(ans, dev3[i], 3) &&
		    ((islong && !ans[3]) ||
		     (!islong && !strncmp(ans+3,"     ",5))))
			return 1;

	for (i = 0; i < sizeof(dev4)/sizeof(*dev4); i++)
		if (!strncasecmp(ans, dev4[i], 3) &&
		    (ans[3] >= '1' && ans[3] <= '4') &&
		    ((islong && !ans[4]) ||
		     (!islong && !strncmp(ans+4,"    ",4))))
			return 1;
	
	return 0;
}

static inline clash_action get_slots(Stream_t *Dir,
				     char *dosname, char *longname,
				     struct scan_state *ssp,
				     ClashHandling_t *ch)
{
	struct vfat_state vfat;
	clash_action ret;
	int match=0;
	struct directory dir;
	int isprimary;
	int no_overwrite;
	int reason;

	no_overwrite = 1;
	if((is_reserved(longname,1)) ||
	   longname[strspn(longname,". ")] == '\0'){
		reason = RESERVED;
		isprimary = 1;
	} else if(contains_illegals(longname,long_illegals)) {
		reason = ILLEGALS;
		isprimary = 1;
	} else if(is_reserved(dosname,0)) {
		reason = RESERVED;
		ch->use_longname = 1;
		isprimary = 0;
	} else if(contains_illegals(dosname,short_illegals)) {
		reason = ILLEGALS;
		ch->use_longname = 1;
		isprimary = 0;
	} else {
		reason = EXISTS;
		switch (scan_dir(Dir, dosname, longname, &vfat, 
				 ssp, ch->use_longname, ch->ignore_entry)) {
			case -1:
				return NAMEMATCH_ERROR;
				
			case 0:
				return NAMEMATCH_SKIP; 
				/* Single-file error error or skip request */
				
			case 5:
				return NAMEMATCH_GREW;
				/* Grew directory, try again */
				
			case 6:
				return NAMEMATCH_SUCCESS; /* Success */
		}	    
		match = -1;
		if (ssp->longmatch > -1) {
			/* Primary Long Name Match */
#ifdef debug
			fprintf(stderr,
				"Got longmatch=%d for name %s.\n", 
				longmatch, longname);
#endif			
			match = ssp->longmatch;
			isprimary = 1;
		} else if ((ch->use_longname & 1) && (ssp->shortmatch > -1)) {
			/* Secondary Short Name Match */
#ifdef debug
			fprintf(stderr,
				"Got secondary short name match for name %s.\n", 
				longname);
#endif

			match = ssp->shortmatch;
			isprimary = 0;
		} else if (ssp->shortmatch >= 0) {
			/* Primary Short Name Match */
#ifdef debug
			fprintf(stderr,
				"Got primary short name match for name %s.\n", 
				longname);
#endif
			match = ssp->shortmatch;
			isprimary = 1;
		} else 
			return NAMEMATCH_RENAME;
		
		dir_read(Dir, &dir, match, NULL);
		/* if we can't overwrite, don't propose it */
		no_overwrite = (match == ch->source || (dir.attr & 0x10));
	}
	ret = process_namematch(isprimary ? longname : dosname, longname,
				isprimary, ch, no_overwrite, reason);
	
	if (ret == NAMEMATCH_OVERWRITE && match > -1){
		if((dir.attr & 0x5) &&
		   (ask_confirmation("file is read only, overwrite anyway (y/n) ? ",0,0)))
			return NAMEMATCH_RENAME;
		
		/* Free up the file to be overwritten */
		if(fatFreeWithDir(Dir,&dir))
			return NAMEMATCH_ERROR;
		
#if 0
		if(isprimary &&
		   match - ssp->match_free + 1 >= ssp->size_needed){
			/* reuse old entry and old short name for overwrite */
			ssp->free_start = match - ssp->size_needed + 1;
			ssp->free_size = ssp->size_needed;
			ssp->slot = match;
			ssp->got_slots = 1;
			strncpy(dosname, dir.name, 3);
			strncpy(dosname + 8, dir.ext, 3);
			return ret;
		} else
#endif
			{
			dir.name[0] = DELMARK;
			dir_write(Dir, match, &dir);
			return NAMEMATCH_RENAME;
		}
	}

	return ret;
}


static inline int write_slots(Stream_t *Dir,
			      char *dosname, 
			      char *longname,
			      struct scan_state *ssp,
			      write_data_callback *cb,
			      void *arg,
			      int Case)
{
	struct directory dir;

	/* write the file */
	if (fat_error(Dir))
		return 0;

	if (cb(dosname, longname, arg, &dir)) {
		if ((ssp->size_needed > 1) &&
		    (ssp->free_size >= ssp->size_needed)) {
			ssp->slot = write_vfat(Dir, dosname, longname,
					       ssp->free_start);
			clear_vses(Dir, ssp->free_start + ssp->size_needed,
				   ssp->free_start + ssp->free_size - 1);
		}
		dir.Case = Case & (EXTCASE | BASECASE);
		dir_write(Dir, ssp->slot, &dir);
	} else
		return 0;

	return 1;	/* Successfully wrote the file */
}

static void stripspaces(char *name)
{
	char *p,*non_space;

	non_space = name - 1;
	for(p=name; *p; p++)
		if (*p != ' ')
			non_space = p;
	non_space[1] = '\0';
}


int mwrite_one(Stream_t *Dir,
	       char *argname,
	       char *shortname,
	       write_data_callback *cb,
	       void *arg,
	       ClashHandling_t *ch)
{
	char longname[VBUFSIZE];
	const char *dstname;
	char dosname[13];
	int expanded;
	struct scan_state scan;
	clash_action ret;

	expanded = 0;

	if(ch->name_converter == dos_name) {
		if(shortname)
			stripspaces(shortname);
		if(argname)
			stripspaces(argname);
	}

	if(shortname){
		ch->name_converter(shortname,0, &ch->use_longname, dosname);
		if(ch->use_longname & 1){
			/* short name mangled, treat it as a long name */
			argname = shortname;
			shortname = 0;
		}
	}
						
	if (argname[0] && (argname[1] == ':')) {
		/* Skip drive letter */
		dstname = argname + 2;
	} else {
		dstname = argname;
	}

	/* Copy original argument dstname to working value longname */
	strcpy(longname, dstname);

	if(shortname) {
		ch->name_converter(shortname,0, &ch->use_longname, dosname);
		if(strcmp(shortname, longname))
			ch->use_longname |= 1;
	} else
		ch->name_converter(longname,0, &ch->use_longname, dosname);

	ch->action[0] = ch->namematch_default[0];
	ch->action[1] = ch->namematch_default[1];

	while (1) {
		switch((ret=get_slots(Dir, dosname, longname,
				      &scan, ch))){
			case NAMEMATCH_ERROR:
				return -1;	/* Non-file-specific error, 
						 * quit */
				
			case NAMEMATCH_SKIP:
				return 0;	/* Skip file (user request or 
						 * error) */

			case NAMEMATCH_PRENAME:
				ch->name_converter(longname,0,
						   &ch->use_longname, dosname);
				continue;
			case NAMEMATCH_RENAME:
				continue;	/* Renamed file, loop again */

			case NAMEMATCH_GREW:
				/* No collision, and not enough slots.
				 * Try to grow the directory
				 */
				if (expanded) {	/* Already tried this 
						 * once, no good */
					fprintf(stderr, 
						"%s: No directory slots\n",
						progname);
					return -1;
				}
				expanded = 1;
				
				if (dir_grow(Dir, scan.max_entry)) {
					fprintf(stderr, "%s: Disk full\n",
						progname);
					return -1;
				}
				continue;
			case NAMEMATCH_OVERWRITE:
			case NAMEMATCH_SUCCESS:
				return write_slots(Dir, dosname, longname,
						   &scan, cb, arg,
						   ch->use_longname);
			default:
				fprintf(stderr,
					"Internal error: clash_action=%d\n",
					ret);
				return -1;
		}

	}
}

void init_clash_handling(ClashHandling_t *ch)
{
	ch->ignore_entry = -1;
	ch->nowarn = 0;	/*Don't ask, just do default action if name collision */
	ch->namematch_default[0] = NAMEMATCH_AUTORENAME;
	ch->namematch_default[1] = NAMEMATCH_NONE;
	ch->name_converter = dos_name; /* changed by mlabel */
	ch->source = -2;
}

int handle_clash_options(ClashHandling_t *ch, char c)
{
	int isprimary;
	if(isupper(c))
		isprimary = 0;
	else
		isprimary = 1;
	c = tolower(c);
	switch(c) {
		case 'o':
			/* Overwrite if primary name matches */
			ch->namematch_default[isprimary] = NAMEMATCH_OVERWRITE;
			return 0;
		case 'r':
				/* Rename primary name interactively */
			ch->namematch_default[isprimary] = NAMEMATCH_RENAME;
			return 0;
		case 's':
			/* Skip file if primary name collides */
			ch->namematch_default[isprimary] = NAMEMATCH_SKIP;
			return 0;
		case 'm':
			ch->namematch_default[isprimary] = NAMEMATCH_NONE;
			return 0;
		case 'a':
			ch->namematch_default[isprimary] = NAMEMATCH_AUTORENAME;
			return 0;
		default:
			return -1;
	}
}
