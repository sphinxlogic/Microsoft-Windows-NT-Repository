/* vfat.c
 *
 * Miscellaneous VFAT-related functions
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "vfat.h"
#include "file.h"

/* #define DEBUG */

const char *short_illegals=";+=[]',\"*\\<>/?:|";
const char *long_illegals = "\"*\\<>/?:|\005";

/* Automatically derive a new name */
static void autorename(char *name,
		       char tilda, char dot, const char *illegals,
		       int limit, int bump)
{
	int tildapos, dotpos;
	unsigned int seqnum=0, maxseq=0;
	char tmp;
	char *p;
	
#ifdef DEBUG
	printf("In autorename for name=%s.\n", name);
#endif
	tildapos = -1;

	for(p=name; *p ; p++)
		if((*p < ' ' && *p != '\005') || strchr(illegals, *p)) {
			*p = '_';
			bump = 0;
		}

	for(dotpos=0;
	    name[dotpos] && dotpos < limit && name[dotpos] != dot ;
	    dotpos++) {
		if(name[dotpos] == tilda) {
			tildapos = dotpos;
			seqnum = 0;
			maxseq = 1;
		} else if (name[dotpos] >= '0' && name[dotpos] <= '9') {
			seqnum = seqnum * 10 + name[dotpos] - '0';
			maxseq = maxseq * 10;
		} else
			tildapos = -1; /* sequence number interrupted */
	}
	if(tildapos == -1) {
		/* no sequence number yet */
		if(dotpos > limit - 2) {
			tildapos = limit - 2;
			dotpos = limit;
		} else {
			tildapos = dotpos;
			dotpos += 2;
		}
		seqnum = 1;
	} else {
		if(bump)
			seqnum++;
		if(seqnum > 999999) {
			seqnum = 1;
			tildapos = dotpos - 2;
			/* this matches Win95's behavior, and also guarantees
			 * us that the sequence numbers never get shorter */
		}
		if (seqnum == maxseq) {
		    if(dotpos >= limit)
			tildapos--;
		    else
			dotpos++;
		}
	}

	tmp = name[dotpos];
	if((bump && seqnum == 1) || seqnum > 1 || mtools_numeric_tail)
		sprintf(name+tildapos,"%c%d",tilda, seqnum);
	if(dot)
	    name[dotpos]=tmp;
	/* replace the character if it wasn't a space */
}


void autorename_short(char *name, int bump)
{
	autorename(name, '~', ' ', short_illegals, 8, bump);
}

void autorename_long(char *name, int bump)
{
	autorename(name, '-', '\0', long_illegals, 255, bump);
}

void clear_vfat(struct vfat_state *v)
{
	v->subentries = 0;
	v->status = 0;
}


/* sum_shortname
 *
 * Calculate the checksum that results from the short name in *dir.
 *
 * The sum is formed by circularly right-shifting the previous sum
 * and adding in each character, from left to right, padding both
 * the name and extension to maximum length with spaces and skipping
 * the "." (hence always summing exactly 11 characters).
 * 
 * This exact algorithm is required in order to remain compatible
 * with Microsoft Windows-95 and Microsoft Windows NT 3.5.
 * Thanks to Jeffrey Richter of Microsoft Systems Journal for
 * pointing me to the correct algorithm.
 *
 * David C. Niemi (niemidc@erols.com) 95.01.19
 */
unsigned char sum_shortname(name)
	char *name;
{
	unsigned char sum;
	int j;

	for (j=sum=0; j<11; ++j)
		sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1)
			+ (name[j] ? name[j] : ' ');
	return(sum);
}

/* check_vfat
 *
 * Inspect a directory and any associated VSEs.
 * Return 1 if the VSEs comprise a valid long file name,
 * 0 if not.
 */
int check_vfat(struct vfat_state *v, struct directory *dir)
{
	char name[12];

	if (! v->subentries) {
#ifdef DEBUG
		fprintf(stderr, "check_vfat: no VSEs.\n");
#endif
		return 0;
	}

	strncpy((char *)name, (char *)dir->name, 8);
	strncpy((char *)name + 8, (char *)dir->ext, 3);
	name[11] = '\0';

	if (v->sum != sum_shortname(name))
		return 0;
	
	if( (v->status & ((1<<v->subentries) - 1)) != (1<<v->subentries) - 1)
		return 0; /* missing entries */

	/* zero out byte following last entry, for good measure */
	v->name[VSE_NAMELEN * v->subentries] = 0;

	return 1;
}


void clear_vses(Stream_t *Dir, int entry, size_t last)
{
	struct directory dir;

	maximize(&last, entry + MAX_VFAT_SUBENTRIES);
	for (; entry <= last; ++entry) {
#ifdef DEBUG
		fprintf(stderr,"Clearing entry %d.\n", entry);
#endif
		dir_read(Dir, &dir, entry, NULL);
		if(!dir.name[0] || dir.name[0] == DELMARK)
			break;
		dir.name[0] = DELMARK;
		if (dir.attr == 0xf)
			dir.attr = '\0';
		dir_write(Dir, entry, &dir);
	}
}

int write_vfat(Stream_t *Dir, char *shortname, char *longname, int start)
{
	struct vfat_subentry vse;
	int vse_id, num_vses;
	char *c;

#ifdef DEBUG
printf("Entering write_vfat with longname=\"%s\", start=%d.\n",longname,start);
#endif
	/* Fill in invariant part of vse */
	vse.attribute = 0x0f;
	vse.hash1 = vse.sector_l = vse.sector_u = 0;
	vse.sum = sum_shortname(shortname);
#ifdef DEBUG
	printf("Wrote checksum=%d for shortname %s.\n", vse.sum,shortname);
#endif

	num_vses = strlen(longname)/VSE_NAMELEN + 1;
	for (vse_id = num_vses; vse_id; --vse_id) {
		int end = 0;

		c = longname + (vse_id - 1) * VSE_NAMELEN;
		

		c += unicode_write(c, vse.text1, VSE1SIZE, &end);
		c += unicode_write(c, vse.text2, VSE2SIZE, &end);
		c += unicode_write(c, vse.text3, VSE3SIZE, &end);


		vse.id = (vse_id == num_vses) ? (vse_id | VSE_LAST) : vse_id;
#ifdef DEBUG
printf("Writing longname=(%s), VSE %d (%13s) at %d, end = %d.\n",
longname, vse_id, longname + (vse_id-1) * VSE_NAMELEN,
start + num_vses - vse_id, start + num_vses);
#endif
		dir_write(Dir, start + num_vses - vse_id,
			(struct directory *)&vse);
	}
	return start + num_vses;
}

/*
 * vfat_lookup looks for filenames in directory dir.
 * if a name if found, it is returned in outname
 * if applicable, the file is opened and its stream is returned in File
 */

int vfat_lookup(Stream_t *Dir, struct directory *dir,
		int *entry, int *vfat_start,
		const char *filename, 
		int flags, char *outname, char *shortname, char *longname)
{
	int found;
	struct vfat_state vfat;
	char newfile[13];
	int vfat_present = 0; /* zeroed by clear_vfat, but make GCC happy */

	if (*entry == -1)
		return -1;

	found = 0;
	clear_vfat(&vfat);
	while(1){
		if(!dir_read(Dir, dir, *entry, &vfat)){
			if(vfat_start)
				*vfat_start = *entry;
			break;
		}
		(*entry)++;

		/*---------------- Empty ---------------*/
		if (dir->name[0] == '\0'){
			if(!vfat_start)
				break;
			continue;
		}

		if (dir->attr == 0x0f) {
			/* VSE, keep going */
			continue;
		}
		
		/* If we get here, it's a short name FAT entry, maybe erased.
		 * thus we should make sure that the vfat structure will be
		 * cleared before the next loop run */

		/* deleted file */
		if ( (dir->name[0] == DELMARK) ||
		     ((dir->attr & 0x8) && !(flags & ACCEPT_LABEL))){
			clear_vfat(&vfat);
			continue;
		}

		vfat_present = check_vfat(&vfat, dir);
		if (vfat_start) {
			*vfat_start = *entry - 1;
			if(vfat_present)
				*vfat_start -= vfat.subentries;
		}

		if (dir->attr & 0x8){
			strncpy(newfile, dir->name,8);
			newfile[8]='\0';
			strncat(newfile, dir->ext,3);
			newfile[11]='\0';
		} else
			unix_name(dir->name, dir->ext, dir->Case, newfile);
		

		/*---------- multiple files ----------*/
#ifdef DEBUG
		printf(
	"!single, vfat_present=%d, vfat.name=%s, newfile=%s, filename=%s.\n",
			vfat_present, vfat.name, newfile, filename);
#endif						
		if(!((flags & MATCH_ANY) ||
		     (vfat_present && match(vfat.name, filename, outname, 0)) ||
		     match(newfile, filename, outname, 1))) {
			clear_vfat(&vfat);
			continue;
		}


		/* entry of non-requested type */
		if((dir->attr & 0x10) && !(flags & ACCEPT_DIR)) {
			if(!(flags & (ACCEPT_LABEL|MATCH_ANY|NO_MSG)))
				fprintf(stderr,
					"Skipping \"%s\", is a directory\n",
					newfile);
			clear_vfat(&vfat);
			continue;
		}


		if((!(dir->attr & 0x18)) && !(flags & ACCEPT_PLAIN)) {
			if(!(flags & (ACCEPT_LABEL|MATCH_ANY|NO_MSG)))
				fprintf(stderr,
					"Skipping \"%s\", is not a directory\n",
					newfile);
			clear_vfat(&vfat);
			continue;
		}


		found = 1;
		break;
	}

	if(found){
#if 0
		if((flags & DO_OPEN) && Fs && File){
			*File = open_file(COPY(Fs), dir);
			if (! *File)
				FREE( &Fs);
		}
#endif
		if(longname){
			if(vfat_present)
				strcpy(longname, vfat.name);
			else
				*longname ='\0';
		}
		if(shortname)
			strcpy(shortname, newfile);
			
		return 0; /* file found */
	} else {
		*entry = -1;
		return -1; /* no file found */
	}
}

/* End vfat.c */
