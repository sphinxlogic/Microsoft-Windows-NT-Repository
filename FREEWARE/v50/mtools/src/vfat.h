#ifndef MTOOLS_VFAT_H
#define MTOOLS_VFAT_H

#include "msdos.h"
#include "stream.h"

/*
 * VFAT-related common header file
 */

#define VFAT_SUPPORT

struct unicode_char {
	char lchar;
	char uchar;
};


/* #define MAX_VFAT_SUBENTRIES 32 */ /* Theoretical max # of VSEs */
#define MAX_VFAT_SUBENTRIES 20		/* Max useful # of VSEs */
#define VSE_NAMELEN 13

#define VSE1SIZE 5
#define VSE2SIZE 6
#define VSE3SIZE 2

struct vfat_subentry {
	unsigned char id;		/* 0x40 = last; & 0x1f = VSE ID */
	struct unicode_char text1[VSE1SIZE] PACKED;
	unsigned char attribute;	/* 0x0f for VFAT */
	unsigned char hash1;		/* Always 0? */
	unsigned char sum;		/* Checksum of short name */
	struct unicode_char text2[VSE2SIZE] PACKED;
	unsigned char sector_l;		/* 0 for VFAT */
	unsigned char sector_u;		/* 0 for VFAT */
	struct unicode_char text3[VSE3SIZE] PACKED;
};

/* Enough size for a worst case number of full VSEs plus a null */
#define VBUFSIZE ((MAX_VFAT_SUBENTRIES*VSE_NAMELEN) + 1)

/* Max legal length of a VFAT long name */
#define MAX_VNAMELEN (255)

#define VSE_PRESENT 0x01
#define VSE_LAST 0x40
#define VSE_MASK 0x1f

struct vfat_state {
	char name[VBUFSIZE];
	int status; /* is now a bit map of 32 bits */
	int subentries;
	unsigned char sum; /* no need to remember the sum for each entry,
			    * it is the same anyways */
};


struct scan_state {
	int match_free;
	int shortmatch;
	int longmatch;
	int free_start;
	int free_size;
	int slot;
	int got_slots;
	int size_needed;
	int max_entry;
};


void clear_vfat(struct vfat_state  *);
int check_vfat(struct vfat_state *v, struct directory *dir);
int unicode_read(struct unicode_char *, char *, int num);
int unicode_write(char *, struct unicode_char *, int num, int *end);
unsigned char sum_shortname(char *);
int write_vfat(Stream_t *, char *, char *, int);
void clear_vses(Stream_t *, int, size_t);
void autorename_short(char *, int);
void autorename_long(char *, int);

int vfat_lookup(Stream_t *Dir,
		struct directory *dir, int *entry,
		int *beginpos,
		const char *filename,
		int flags, char *outname,
		char *shortname, char *longname);

struct directory *dir_read(Stream_t *Stream,
			   struct directory *dir, 
			   int num, 
			   struct vfat_state *v);

#define DO_OPEN 1
#define ACCEPT_NO_DOTS 0x4
#define ACCEPT_PLAIN 0x20
#define ACCEPT_DIR 0x10
#define ACCEPT_LABEL 0x08
#define SINGLE 2
#define MATCH_ANY 0x40
#define NO_MSG 0x80
#define NO_DOTS 0x100
#endif
