/*
 * Buffer read/write module
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "buffer.h"

typedef struct Buffer_t {
	Class_t *Class;
	int refs;
	Stream_t *Next;
	Stream_t *Buffer;
	
	size_t size;     	/* size of read/write buffer */
	int dirty;	       	/* is the buffer dirty? */
	int grain;
	int grain2;
	int ever_dirty;	       	/* was the buffer ever dirty? */
	int dirty_pos;
	int dirty_end;
	off_t current;		/* first sector in buffer */
	size_t cur_size;		/* the current size */
	char *buf;		/* disk read/write buffer */
} Buffer_t;

/*
 * Flush a dirty buffer to disk.  Resets Buffer->dirty to zero.
 * All errors are fatal.
 */

static int _buf_flush(Buffer_t *Buffer)
{
	int ret;

	if (!Buffer->Next || !Buffer->dirty)
		return 0;
	if(Buffer->current < 0L) {
		fprintf(stderr,"Should not happen\n");
		return 0;
	}
	ret = force_write(Buffer->Next, 
			  Buffer->buf + Buffer->dirty_pos,
			  Buffer->current + Buffer->dirty_pos,
			  Buffer->dirty_end - Buffer->dirty_pos);
	if(ret != Buffer->dirty_end - Buffer->dirty_pos) {
		if(ret < 0)
			perror("buffer_flush: write");
		else
			fprintf(stderr,"buffer_flush: short write\n");
		return -1;
	}
	Buffer->dirty = 0;
	return 0;
}

static void invalidate_buffer(Buffer_t *Buffer, off_t start)
{
	_buf_flush(Buffer);
	Buffer->current = start - start % Buffer->grain;
	Buffer->cur_size = 0;
}

static int buf_read(Stream_t *Stream, char *buf, off_t start, size_t len)
{
	int length, offset;
	char *disk_ptr;
	int ret;
	DeclareThis(Buffer_t);	

	if(!len)
		return 0;	
	/* a "cache" miss, load buffer */
	if (start < This->current || start >= This->current + This->cur_size) {
		invalidate_buffer(This, start);

		/* always load aligned */
		length = This->size - (This->current % This->grain2);

		/* read it! */
		ret=This->Next->Class->read(This->Next,
					    This->buf,
					    This->current,
					    length);
		if ( ret < 0 )
			return ret;
		This->cur_size = ret;
	}

	offset = start - This->current;
	disk_ptr = This->buf + offset;
	maximize(&len, This->cur_size - offset);
	memcpy(buf, disk_ptr, len);
	return len;
}

static int buf_write(Stream_t *Stream, char *buf, off_t start, size_t len)
{
	char *disk_ptr;
	DeclareThis(Buffer_t);	
	int offset, ret;

	if(!len)
		return 0;

	This->ever_dirty = 1;

	/* a cache miss... */
	if (start < This->current ||
	    start >= This->current + This->size ||
	    start > This->current + This->cur_size ||
	    (start == This->current + This->cur_size &&
	     (len < This->grain || !(start % This->grain2)))) {
		invalidate_buffer(This, start);
	       	if(start % This->grain || len < This->grain) {
			/* read it! */
			ret=This->Next->Class->read(This->Next,
						    This->buf,
						    This->current,
						    This->grain);
			if ( ret < 0 )
				return ret;
			This->cur_size = ret;
			/* for dosemu. Autoextend size */
			if(!This->cur_size) {
				memset(This->buf, 0, This->grain);
				This->cur_size = This->grain;
			}
		}
	}

	maximize(&len,This->size - (start % This->size));
	offset = start - This->current;
	if(len + offset > This->cur_size) {
		/* enforce aligned writes */
		len -= (len + offset ) % This->grain;
		This->cur_size = len + offset;
	}
	disk_ptr = This->buf + offset;
	memcpy(disk_ptr, buf, len);
	if(!This->dirty || disk_ptr - This->buf < This->dirty_pos){
		This->dirty_pos = disk_ptr - This->buf;
		This->dirty_pos -= This->dirty_pos % This->grain;
	}
	if(!This->dirty || disk_ptr - This->buf + len > This->dirty_end){
		This->dirty_end = disk_ptr - This->buf + len;
		This->dirty_end += This->grain - 1;
		This->dirty_end -= This->dirty_end % This->grain;
	}
	This->dirty = 1;
	return len;
}

static int buf_flush(Stream_t *Stream)
{
	int ret;
	DeclareThis(Buffer_t);	

	if (!This->ever_dirty)
		return 0;
	ret = _buf_flush(This);
	This->ever_dirty = 0;
	return ret;
}

static Class_t BufferClass = {
	buf_read,
	buf_write,
	buf_flush,
	0, /* buf_free */
	0, /* set_geom */
	get_data_pass_through, /* get_data */
};

Stream_t *buf_init(Stream_t *Next, int size, int grain, int grain2)
{
	Buffer_t *Buffer;
	Stream_t *Stream;


	if(Next->Buffer){
		Next->refs--;
		Next->Buffer->refs++;
		return Next->Buffer;
	}

	Stream = (Stream_t *) malloc (sizeof(Buffer_t));
	Buffer = (Buffer_t *) Stream;
	Buffer->buf = malloc(size);
	Buffer->size = size;
	Buffer->grain = grain;
	Buffer->grain2 = grain2;
	Buffer->cur_size = 0; /* buffer currently empty */
	Buffer->dirty = 0;
	Buffer->ever_dirty = 0;
	if ( !Buffer->buf){
		Free(Stream);
		return 0;
	}
	Buffer->Next = Next;
	Buffer->Class = &BufferClass;
	Buffer->refs = 1;
	Buffer->Buffer = 0;
	Buffer->Next->Buffer = (Stream_t *) Buffer;
	return Stream;
}

