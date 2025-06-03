/*
 * This module provides a routine to analyze a tar file.
 *
 *   int tar_init();
 *   void *tar_open ( char *archive, char errmsg[256] );
 *   int tar_close ( void *volume );
 *   int tar_set_module 
 *	( void *volume, char *name, int *size, char errmsg[256] );
 *   int tar_read ( void *volume, char *buffer, int maxlen, int *length );
 *   int tar readdir ( void *volume, char *name, int *size, unsigned *date );
 */
#include "pthread_np.h"
#include <stdlib.h>
#include <stdio.h>
#define strerror strerror_1
#include <string.h>
#undef strerror
char *strerror ( int errnum, ... );
#include <errno.h>
#include <stat.h>
#include "tarscan.h"
#include "tutil.h"

struct tar_header {
    char name[100], mode[8], uid[8], gid[8], size[12],
	mtime[12], chksum[8], linkflag, linkname[100], rdev[6];
    char fill[250];
};

struct tar_dir_block {		/* Matches first part of TAR file header */
    char name[100];
    int mode;
    int size;
    int location;		/* Offset from start of file */
    unsigned int mdate;
};

struct volume_control_block {
    struct volume_control_block *flink, *blink;
    char fname[256];			/* Tar file name */
    int size;				/* Total bytes in file */
    unsigned int cdate, mdate;		/* creat and modify times */
    int last_used[2];
    int ref_count;			/* Number of active contexts */
    int delete_pending;			/* Delete when ref_count -> 0 */
    int dir_size;			/* Number of entries in directory */
    int dir_used;
    struct tar_dir_block *directory;	/* Table of modules in file */
};
typedef struct volume_control_block *vcb_ptr;

struct volume_context {		/* Maintains context for access */
    struct volume_context *flink, *blink;
    struct volume_control_block *vcb;	/* back pointer to volume */
    int dir_position;			/* Index for current entry */
    int file_position;			/* Current virtual offset */
    int start, finish;
    FILE *fptr;				/* I/O stream to archive file */
};
typedef struct volume_context *volume_ctx;

/****************************************************************************/
static int master_sequence;
static struct volume_control_block volume_root;
static volume_ctx free_ctx;
static vcb_ptr active_vcb, free_vcb;
/*
 * The following macros protect shared data fields.
 */
static pthread_mutex_t volume_ctl, context_ctl;
static pthread_key_t tar_key;

#define LOCK_VOLUME_DB pthread_mutex_lock ( &volume_ctl );
#define UNLOCK_VOLUME_DB pthread_mutex_unlock ( &volume_ctl );
#define LOCK_CONTEXT_DB  pthread_mutex_lock ( &context_ctl );
#define UNLOCK_CONTEXT_DB pthread_mutex_unlock ( &context_ctl );
/****************************************************************************/
/* Called on thread exit to rundown any volume contexts left open by the
 * thread.
 */
static void tar_rundown ( volume_ctx ctx )
{
    int tar_close();
    volume_ctx cur;
    if ( !ctx ) return;			/* No active contexts. */

    for ( cur = ctx; cur; cur = cur->flink ) {
printf("Running down dangling tar stream\n");
	LOCK_C_RTL
	fclose ( cur->fptr );
	UNLOCK_C_RTL
	cur->fptr = (FILE *) 0;

	tar_close ( cur );		/* Disconnect from volume */
	if ( !cur->flink ) {
	    /* Last block, place bulk on free list */
	    LOCK_CONTEXT_DB
	    cur->flink = free_ctx;
	    free_ctx = ctx;
	    UNLOCK_CONTEXT_DB
	    break;
	}
    }
}
/****************************************************************************/
/*
 * Initialize volume control list.  We assume we are called from initial
 * thread and only called once.
 */
int tar_init ( int max_streams )
{
    int i, status;
    void tar_rundown();
    /*
     * Initialize pthreads objects for module.
     */
    pthread_mutex_init ( &context_ctl, pthread_mutexattr_default );
    pthread_mutex_init ( &volume_ctl, pthread_mutexattr_default );
    pthread_keycreate ( &tar_key, tar_rundown );
    /*
     * Allocate a volume control block for every possible stream.
     */
    LOCK_VOLUME_DB
    master_sequence = 0;
    active_vcb = (vcb_ptr) 0;
    LOCK_C_RTL
    free_vcb = (vcb_ptr) malloc 
		( sizeof(struct volume_control_block) * (max_streams+1) );
    UNLOCK_C_RTL
    if ( !free_vcb ) return 0;
    for ( i = 0; i < max_streams; i++ ) free_vcb[i].flink = &free_vcb[i+1];
    free_vcb[max_streams].flink = (vcb_ptr) 0;
    /*
     * Allocate a context for every allowed stream.
     */
    LOCK_C_RTL
    free_ctx = (volume_ctx) malloc 
		( sizeof(struct volume_context) * max_streams );
    UNLOCK_C_RTL
    if ( !free_ctx ) return 0;		/* allocation error */
    for ( i = 0; i < (max_streams-1); i++ ) free_ctx[i].flink = &free_ctx[i+1];
    free_ctx[i].flink = (volume_ctx) 0;	/* terminate list */
    UNLOCK_VOLUME_DB
    return 1;
}
/****************************************************************************/
/*  Internal utility routines, convert octal field to int and compare name
 *  elements in to tar_dir_blocks (used to qsort).
 */
static int octal_value ( char *str, int maxlen )
{
    int value, i;
    value = 0;
    for ( i = 0; str[i] && (i < maxlen); i++ ) {
	if ( (str[i] >= '0') && (str[i] <= '7') ) value = (value*8) +
	    ((int) str[i]) - 48;
	else if ( (value > 0) && (str[i] == ' ') ) break;
    }
    return value;
}
static int tar_compare_names ( const void *b1, const void *b2 )
{
     struct tar_dir_block *d1, *d2;
     d1 = (struct tar_dir_block *) b1;
     d2 = (struct tar_dir_block *) b2;
     return tu_strncmp ( d1->name, d2->name, 100 ); 
}
/****************************************************************************/
/* Read tar file and build directory structure from scanned files.
 * We assume volume datbase is locked during load.
 */
static int load_directory ( FILE *fptr, vcb_ptr vcb )
{
    struct tar_sheader {
        char name[100], mode[8], uid[8], gid[8], size[12],
	    mtime[12], chksum[8], linkflag, linkname[100], rdev[6];
        char fill [512-263];
    } hdr;
    unsigned char *raw;
    int j, dpos, remaining, count, size, checksum, status, first_zero_block;

    dpos = 0;
    vcb->dir_used = 0;
    first_zero_block = 0;

    for ( remaining = vcb->size; remaining > sizeof(hdr); ) {
	/*
	 * Read header block and check for EOF mark.
	 */
	count = fread ( &hdr, sizeof(hdr), 1, fptr );
	if ( count != 1 ) break;		/* read error */
	/* printf("Header read,. name: '%s'\n", hdr.name ); */
	remaining = remaining - sizeof(hdr);
	if ( hdr.name[0] == '\0' ) {
	    if ( first_zero_block ) break;		/* two zeros in a row */
	    first_zero_block = 1;
	    continue;	/* zero header */
	}
	else first_zero_block = 0;
	/*
	 * Validate checksum.  For checksum, treat checksum field as blanks.
	 */
	checksum = octal_value ( hdr.chksum, sizeof(hdr.chksum) );
	for ( j = 0; j < 8; j++ ) hdr.chksum[j] = ' ';
	raw = (unsigned char *) hdr.name;
	for ( j = 0; j < 512; j++ ) checksum = checksum - *raw++;
	if ( checksum ) {
	    LOCK_C_RTL
	    printf("Header checksum failure: %s, %d\n", hdr.name, checksum );
	    UNLOCK_C_RTL
	    break;
	}
	/*
	 * Expand directory if needed.
	 */
	if ( (dpos+1) >= vcb->dir_size ) {
	    int new_size, extension;
	    extension = (vcb->dir_size <= 200) ? 200 : 1200;
	    new_size = (vcb->dir_size+extension) * sizeof(struct tar_dir_block);
	    LOCK_C_RTL
	    if ( vcb->dir_size > 0 ) 
		vcb->directory = realloc ( vcb->directory, new_size );
	    else vcb->directory = malloc ( new_size );
	    UNLOCK_C_RTL

	    vcb->dir_size += extension;
	}
	/*
	 * Trim "./" from directory name, if present.
	 */
	if ( (hdr.name[0] == '.') && (hdr.name[1] == '/') )
	    tu_strcpy ( vcb->directory[dpos].name, &hdr.name[2] );
	else
	    tu_strcpy ( vcb->directory[dpos].name, hdr.name );
	/*
	 * Bug in DECC ftell() returns wrong result when used on fixed length
	 * record file, so compute it indirectly.
	 */
	vcb->directory[dpos].location = vcb->size - remaining;
	size = octal_value ( hdr.size, sizeof(hdr.size) );
	vcb->directory[dpos].size = size;
	vcb->directory[dpos].mode = octal_value ( hdr.mode, sizeof(hdr.mode) );
	vcb->directory[dpos].mdate = 
		octal_value ( hdr.mtime, sizeof(hdr.mtime) );
	/*
	 * Skip data, don't have faith in fseek, so read it.
	 */
	size = (size+511)&0xfffffe00;		/* Round up to integral blocks*/
	remaining = remaining - size;
	for ( ; size > 0; size = size-512 ) fread ( &hdr, 512, 1, fptr );
	dpos = dpos + 1;
    }
    LOCK_C_RTL
    printf("Volume %s mounted(a=%d) with %d/%d entries in index.\n",
		vcb->fname, vcb, dpos, vcb->dir_size );
    UNLOCK_C_RTL
    if ( vcb->dir_size > 0 ) {
        vcb->directory[dpos].name[0] = '\0';
        vcb->directory[dpos].size = -1;		/* mark end of list */
    }
    /*
     * Sort directory.
     */
    vcb->dir_used = dpos;
    if ( dpos > 1 ) {
	LOCK_C_RTL
	qsort ( vcb->directory, dpos, sizeof(struct tar_dir_block),
		tar_compare_names );
	UNLOCK_C_RTL
    }
    return dpos;
}
/****************************************************************************/
/* Internal function to locate/add VCB to volume list.  Return 0 on error.
 */
static int tar_mount ( volume_ctx ctx, char *volname, char errmsg[256] )
{
    vcb_ptr vcb, lru_vcb;
    int sequence;
    stat_t statblk;
    /*
     * look for an existing vcb with the same volname.  As we scan, keep
     * track of oldest vcb that has a refcount of zero.
     */
    fstat ( fileno(ctx->fptr), &statblk );
    lru_vcb = (vcb_ptr) 0;
    LOCK_VOLUME_DB		/* Serialize access to list */
    sequence = master_sequence + 1;
    for ( vcb = active_vcb; vcb; vcb = vcb->flink ) {
	if ( tu_strncmp ( vcb->fname, volname, sizeof(vcb->fname) ) == 0 ) {
#ifdef DEBUG
printf("Comparing '%s' with vcb/%d '%s'\n", volname, vcb, vcb->fname );
printf("Found active vcb for %s, dir_size: %d\n", vcb->fname, vcb->dir_size );
#endif
	    /*
	     * Check if date still valid and either update it or skip.
	     */
	    if ( statblk.st_mtime > vcb->mdate ) {
#ifdef DEBUG
printf("vcb stale: %d > %d(vcb)\n", statblk.st_mtime, vcb->mdate );
#endif
		/*
		 * Tar file newer that file used to build this vcb, either
		 * mark it for delete or delete it.
		 */
		if ( vcb->ref_count > 0 ) {
		    vcb->delete_pending = 1;
		} else {
		    /* Deallocate */
		    if ( vcb->dir_size > 0 ) free ( vcb->directory );
		    vcb->dir_size = 0;		/* force rescan */

		    if ( vcb->flink ) {
	    		vcb->flink->blink = vcb->blink;
		    }
		    if ( vcb->blink ) {
			vcb->blink->flink = vcb->flink;
		    } else {
			active_vcb = vcb->flink;
		    }
		    vcb->blink = (vcb_ptr) 0;
		    vcb->flink = free_vcb;
		    free_vcb = vcb;
		}
		continue;
	    }
	    /*
	     * Use this vcb.
	     */
	    ctx->vcb = vcb;
	    vcb->ref_count++;
	    master_sequence++;
	    vcb->last_used[0] = master_sequence;
	    if ( vcb->dir_size == 0 ) {		/* should never happen */
		vcb->size = statblk.st_size;
	        vcb->cdate = statblk.st_ctime;
	        vcb->mdate = statblk.st_mtime;		    
	        load_directory ( ctx->fptr, vcb );
	    }
	    UNLOCK_VOLUME_DB
	    return 1;
	}
	if ( vcb->ref_count == 0 ) {
	    if ( sequence > vcb->last_used[0] ) {
		sequence = vcb->last_used[0];
		lru_vcb = vcb;
	    }
	}
    }
    /*
     * Reaching this point means we must allocate a new volume.
     */
    if ( !free_vcb && lru_vcb ) {
	/*
	 * No VCBs on free list but active list scan found idle VCB that
	 * we can reuse.  Move lru_vcb from active list to free list.
	 */
	if ( lru_vcb->flink ) {
	    lru_vcb->flink->blink = lru_vcb->blink;
	}
	if ( lru_vcb->blink ) {
	    lru_vcb->blink->flink = lru_vcb->flink;
	} else {
	    active_vcb = lru_vcb->flink;
	}
	lru_vcb->blink = (vcb_ptr) 0;
	lru_vcb->flink = free_vcb;
	free_vcb = lru_vcb;
    }

    if ( !free_vcb ) {
	UNLOCK_VOLUME_DB
        tu_strcpy ( errmsg, "Internal error, no free volume control blocks" );
	 return 0;
    }
    /*
     * Remove vcb from free list.
     */
    vcb = free_vcb;
    free_vcb = vcb->flink;
    UNLOCK_VOLUME_DB
    /*
     * Fulling initialize block and load directory.
     */
    tu_strcpy ( vcb->fname, volname );
    /* printf("initialized vcb/%d fname to '%s'\n", vcb, vcb->fname ); */
    vcb->size = statblk.st_size;
    vcb->cdate = statblk.st_ctime;
    vcb->dir_size = 0;
    vcb->mdate = statblk.st_mtime;
    ctx->vcb = vcb;
    vcb->ref_count = 1;
    vcb->delete_pending = 0;
    load_directory ( ctx->fptr, vcb );
    /*
     * Place initialized vcb on active list.
     */
    LOCK_VOLUME_DB
    vcb->blink = (vcb_ptr) 0;
    vcb->flink = active_vcb;
    if ( vcb->flink ) active_vcb->blink = vcb;
    active_vcb = vcb;
    UNLOCK_VOLUME_DB

    return 1;
}
/****************************************************************************/
/* 'Mount' tar archive for access.
 */
void *tar_open ( char *archive, char errmsg[256] )
{
    volume_ctx ctx, cur;
    FILE *tarf;
    /*
     * Allocate context client is to use.
     */
    ctx = (volume_ctx) 0;
    LOCK_CONTEXT_DB
    if ( free_ctx ) {
	ctx = free_ctx;
	free_ctx = ctx->flink;
    }
    UNLOCK_CONTEXT_DB
    if ( !ctx ) {
	tu_strcpy ( errmsg, "No streams available" );
	return (void *) 0;
    }
    *errmsg = '\0';			/* make err message string empty */
    /*
     * Attempt to open file.
     */
    tarf = fopen ( archive, "rb", "mbc=64" );
    if ( !tarf ) {
	char *err;
	ctx->flink = free_ctx; free_ctx = ctx;	/* deallocate context */
	err = strerror ( errno, vaxc$errno );
	tu_strcpy ( errmsg, err ? err : "File open error (unknown)" );
	return (void *) 0;
    }
    /*
     * File valid, Attempt to locate VCB for this file.
     */
    ctx->fptr = tarf;
    if ( !tar_mount ( ctx, archive, errmsg ) ) {
	LOCK_CONTEXT_DB
	ctx->flink = free_ctx; free_ctx = ctx;	/* deallocate context */
	UNLOCK_CONTEXT_DB
	return (void *) 0;
    }
   ctx->dir_position = 0;
    /*
     * Place context in thread's list of active streams.
     */
    if ( pthread_getspecific ( tar_key, (void *) &cur ) >= 0 ) {
	if ( cur ) {
	    /* Insert after current so we don't have to call setspecific */
	    ctx->flink = cur->flink;
	    ctx->blink = cur;
	    cur->flink = ctx;
	    if ( ctx->flink ) ctx->flink->blink = ctx;

	} else {
	    /* Empy list */
	    ctx->flink = ctx->blink = (volume_ctx) 0;
	    pthread_setspecific ( tar_key, ctx );
	}
    }

    return (void *) ctx;
}
/*******************************************************************/
int tar_close ( void *client_ctx )
{
    volume_ctx ctx;
    vcb_ptr vcb;
    /*
     * Disassociate context with volume control block.
     */
    ctx = (volume_ctx) client_ctx;
    vcb = ctx->vcb;
    ctx->vcb = (vcb_ptr) 0;

    LOCK_VOLUME_DB
    vcb->ref_count--;
    if ( vcb->ref_count <= 0 ) {
	if ( vcb->delete_pending ) {
#ifdef DEBUG
printf("Removing VCB\n");
#endif
	    vcb->delete_pending = 0;
	    vcb->fname[0] = '\0';	/* remove name */
	}
    }
    UNLOCK_VOLUME_DB
    /*
     * If we are called by thread (fptr non-null), remove from list of
     * active contexts for this thread.  If fptr is null, let rundown
     * routine do it.
     */
    if ( ctx->fptr ) {
	volume_ctx cur, first;
	LOCK_C_RTL
        fclose ( ctx->fptr );
	UNLOCK_C_RTL
	ctx->fptr = (FILE *) 0;

	if ( pthread_getspecific ( tar_key, (void *) &first ) < 0 ) return 0;
	for ( cur = first; cur; cur = cur->flink ) {
	    if ( cur == ctx ) {
		/* Delete from threads list of active streams. */
		if ( cur->flink ) cur->flink->blink = cur->blink;
		if ( cur->blink ) cur->blink->flink = cur->flink;
		else pthread_setspecific (tar_key, cur->flink);
		break;
	    }
	}
	
        LOCK_CONTEXT_DB
	ctx->fptr = (FILE *) 0;
        ctx->flink = free_ctx; free_ctx = ctx;	/* deallocate context */
        UNLOCK_CONTEXT_DB
    }

    return 1;
}
/*******************************************************************/
/*
 * Return current directory entry and advance to next.  Set context to
 * that a tar_read following a read_dir will read the contents.
 *
 * Name array must be at least 100 chars.
 */
int tar_read_dir ( void *client_ctx, char *name, int *size, unsigned *date )
{
    int i, status, dpos;
    volume_ctx ctx;
    vcb_ptr vcb;

    ctx = (volume_ctx) client_ctx;
    vcb = ctx->vcb;
    /*
     * Validate current directory position.
     */
    dpos = ctx->dir_position;
    if ( (dpos < vcb->dir_size) && (vcb->directory[dpos].size >= 0) ) {
	/*
	 * Everything OK, return info and update pointer.
	 */
	ctx->dir_position = dpos + 1;
	ctx->file_position = vcb->directory[dpos].location;
	ctx->start = vcb->directory[dpos].size;
	ctx->finish = -1;
	tu_strcpy ( name, vcb->directory[dpos].name );
	*size = vcb->directory[dpos].size;
	*date = vcb->directory[dpos].mdate;
	    /* (unsigned int) vcb->directory[dpos].location; */
	return 1;
    }
    return 0;
}
/***********************************************************************/
/* Read data from virtual file.
 */
int tar_read ( void *client_ctx, char *buffer, int maxlen, int *length )
{
    int status, dpos, segment;
    volume_ctx ctx;
    vcb_ptr vcb;

    ctx = (volume_ctx) client_ctx;
    vcb = ctx->vcb;

    segment = ctx->finish - ctx->start;
    if ( segment <= 0 ) {
	/*
	 * Either at end-of-file or beginning of file.
	 */
	if ( ctx->finish < 0 ) {
	    status = fseek ( ctx->fptr, ctx->file_position, 0 );
	    ctx->finish = ctx->start;
	    ctx->start = 0;
	} else {
	    *length = 0;
	    return 0;
	}
    }
    /*
     * Read chunk of date from file, minimum of maxlen or remaining.
     */
    if ( segment > maxlen ) segment = maxlen;
    status = fread ( buffer, segment, 1, ctx->fptr );
    ctx->start += segment;
    *length = segment;
    return status;
}
/*******************************************************************/
int tar_set_module ( void *client_ctx, char *name, int *size, char errmsg[256] )
{
    int i, status, high, low, mid;
    volume_ctx ctx;
    vcb_ptr vcb;

    ctx = (volume_ctx) client_ctx;
    vcb = ctx->vcb;
    errmsg[0] = '\0';
    /* Trim redundant "./" from query name */
    if ( name[0] == '.' ) if ( name[1] == '/' ) name = &name[2];
    /*
     * search directory, use binary search.
     */
    low = 0; high = vcb->dir_used - 1;
    while ( low <= high ) {
	mid = (low+high) / 2;
	i = tu_strncmp ( name, vcb->directory[mid].name, 100 );
	if ( i < 0 ) high = mid - 1;
	else if ( i > 0 ) low = mid + 1;
	else {
	    /*
	     * we have a match, fill in context.
	     */
	    status = fseek ( ctx->fptr, vcb->directory[mid].location, 0 );
	    ctx->dir_position = mid;
	    ctx->file_position = vcb->directory[mid].location;
	    ctx->start = 0;
	    ctx->finish = vcb->directory[mid].size;
	    *size = ctx->finish;
	    return 1;
	}
    }
    /*
     * File not found.
     */
    tu_strcpy ( errmsg, "Could not find module" );
    return 0;
}
/*********************************************************************/
#ifdef TESTIT
int main ( int argc, char **argv )
{
    void *volume;
    int i, length, status, size, LIB$INIT_TIMER(), LIB$SHOW_TIMER();
    unsigned mtime;
    char errmsg[256];
    char contents[40000];
    if ( argc < 2 ) {
	printf ( "usage: tarscan archive [module]" );
	exit ( 1 );
    }

    status = tar_init ( 32 );
    printf("Status of tar init: %d\n", status );

    LIB$INIT_TIMER();
    volume = tar_open ( argv[1], errmsg );
    LIB$SHOW_TIMER();
    if ( !volume ) {
	printf ( "Error mounting archive: %s\n", errmsg );
	exit ( 1 );
    }
    for ( i = 2; i < argc; i++ ) {
	size = 0;
	status = tar_set_module ( volume, argv[i], &size, errmsg );
	printf("Status of set_module: %d, size: %d\n", status, size );
	if ( !status ) { printf ( "errmsg: '%s'\n", errmsg ); exit(1); }
	else {
	    status = tar_read ( volume, contents, 40000-1, &length );
	    printf("Status of read and length: %d %d %d\n", status, length,
		contents );
	    contents[length] = '\0';
	    if ( status > 0 ) printf("%s\n", contents );
	}
    }
    for ( i = 0; tar_read_dir ( volume, contents, &size, &mtime ) > 0;  i++ ) {
	printf ( "Module '%s' has size %d, loc: %d\n", contents, size, mtime );
    }
    status = tar_close ( volume );
    printf("Number of modules listed: %d, close status: %d\n", i, status );
    return 1;
}
#endif
