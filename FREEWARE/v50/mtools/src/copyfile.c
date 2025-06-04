#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "file.h"

/*
 * Copy the data from source to target
 */

int copyfile(Stream_t *Source, Stream_t *Target)
{
	char buffer[16384];
	int pos;
	int ret, retw;

	if (!Source){
		fprintf(stderr,"Couldn't open source file\n");
		return -1;
	}

	if (!Target){
		fprintf(stderr,"Couldn't open target file\n");
		return -1;
	}
	
	pos = 0;
	while(1){
		ret = READS(Source, buffer, pos, 16384);
		if (ret < 0 ){
			perror("file read");
			return -1;
		}
		if(got_signal)
			return -1;
		if (ret == 0)
			break;
		if ((retw = force_write(Target, buffer, pos, ret)) != ret){
			if(retw < 0 )
				perror("write");
			else
				fprintf(stderr,
					"Short write %d instead of %d\n", retw,
					ret);
			return ret;
		}
		pos += ret;
	}
	return pos;
}
