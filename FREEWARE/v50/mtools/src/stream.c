#include "sysincludes.h"
#include "msdos.h"
#include "stream.h"

int flush_stream(Stream_t *Stream)
{
	int ret=0;

	if(Stream->Class->flush)
		ret |= Stream->Class->flush(Stream);
	if(Stream->Next)
		ret |= flush_stream(Stream->Next);
	return ret;
}

Stream_t *copy_stream(Stream_t *Stream)
{
	if(Stream)
		Stream->refs++;
	return Stream;
}

int free_stream(Stream_t **Stream)
{
	int ret=0;

	if(!*Stream)
		return -1;
	if((*Stream)->Class->flush)
		ret |= (*Stream)->Class->flush(*Stream);
	if(! --(*Stream)->refs){
		if((*Stream)->Class->free)
			ret |= (*Stream)->Class->free(*Stream);
		if((*Stream)->Next)
			ret |= free_stream(&(*Stream)->Next);
		Free(*Stream);
	} else if ( (*Stream)->Next )
		ret |= flush_stream((*Stream)->Next);		
	*Stream = NULL;
	return ret;
}


#define GET_DATA(stream, date, size, type, address) \
(stream)->Class->get_data( (stream), (date), (size), (type), (address) )


int get_data_pass_through(Stream_t *Stream, long *date, size_t *size,
			  int *type, int *address)
{
       return GET_DATA(Stream->Next, date, size, type, address);
}

int read_pass_through(Stream_t *Stream, char *buf, off_t start, size_t len)
{
	return READS(Stream->Next, buf, start, len);
}

int write_pass_through(Stream_t *Stream, char *buf, off_t start, size_t len)
{
	return WRITES(Stream->Next, buf, start, len);
}
