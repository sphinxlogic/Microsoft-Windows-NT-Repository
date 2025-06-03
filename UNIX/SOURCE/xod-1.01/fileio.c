static char *RcsId = "$Id: fileio.c,v 1.9 1993/03/02 02:56:05 rfs Exp $";

/*
 * $Log: fileio.c,v $
 * Revision 1.9  1993/03/02  02:56:05  rfs
 * Better ahndling of opening bogus ilfe names
 *
 * Revision 1.8  1993/03/02  00:46:04  rfs
 * Added several routines, and comments.
 *
 * Revision 1.7  1993/02/26  21:36:59  rfs
 * *** empty log message ***
 *
 * Revision 1.6  1993/02/25  00:44:11  rfs
 * *** empty log message ***
 *
 * Revision 1.5  1993/02/24  23:14:10  rfs
 * Added ReadNbytes, for use in decoding.
 *
 * Revision 1.4  1993/02/13  23:23:51  rfs
 * Fixed bug in computation of the offeset on End.
 *
 * Revision 1.3  1993/02/13  17:59:19  rfs
 * fixed problem with byte offsets when user hits End
 *
 * Revision 1.2  1993/02/13  13:40:35  rfs
 * Mouse coordinates reset on new file load.
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:20:18  rfs
 * Initial revision
 *
*/

#include <stdio.h>
#include <malloc.h>

#define True 1
#define False 0;
#define SRCHBUF 4096
typedef int Boolean;

static unsigned char *data = NULL;
static char *FileName;
static long FileSize = 0L;
static FILE *fp = (FILE *)NULL;
static Boolean AtEOF = 0;
static long DataOffset = 0L;
static int Dwidth, Dheight, Dblksize;

/* returns line # and offset from Dwidth of left corner byte */
void
DataLocation(int *line, int *moff) {
	int Dwidth = GetBytesHoriz();

	*line = DataOffset/Dwidth;
	*moff = DataOffset % Dwidth;
}

long
GetDataOffset(void) {
	return DataOffset;
}

char *
GetFileName(void) {
	return FileName;
}

Boolean
IsAtEOF(void) {
	return AtEOF;
}

/* 
	search for specified pattern starting 1 byte past cursor...
	wrap if required.
*/
DoSearch(unsigned char *pattern, int len) {
	unsigned char buffer[SRCHBUF];
	long begin = DataOffset;
	long start;
	long end = DataOffset+(Dwidth*Dheight)-1;
	long offset, where;
	int  pass = 0;
	int  x, y;
	int  line, moff;
	int  i, j, bytes;

	offset = begin;
	GetCursorOnData(&x, &y);
	offset += (y-1)*Dwidth + (x-1) + 1;
	start = offset-1;
	while (1) {
		fseek(fp, offset, 0);
		bytes = fread(buffer, sizeof(unsigned char), SRCHBUF, fp);
		for (i=0; i < bytes; i++) {
			if (buffer[i] == *pattern) {
				for (j=0; j < len; j++) {
					if (buffer[i+j] != *(pattern+j)) break;
				}
				/* did we find a match? */
				if (j == len) {
					where = offset+i;
					line = where/Dwidth;
					if (line < 0) line = 1;
					moff = begin % Dwidth;
					if (where > end || pass) {
						begin = line*Dwidth + moff;
						DisplayFullPage(line, moff);
					}
					y = (where-begin)/Dwidth + 1;
					x = (where-begin)-((y-1)*Dwidth) + 1;

					if (where <= end) {
						SetCursorOnData(x, y);
					}
					else {
						DisplayFullPage(line, moff);
						SetCursorOnData(x, y);
					}
					return;
				}
			}
		}
		offset += SRCHBUF-len+1;
		if (bytes < SRCHBUF) {
			if (start == 0L)
				break;
			else {
				pass++;
				offset = 0;
				start =0;
				continue;
			}
		}
		if (pass > 0 && offset > begin)
			break;
	}
}

unsigned char *
ReadBlock(int startline, int width, int height, int moff) {
	int blksize = width*height;
	int offset = startline*width+moff;
	int i, diff, bytes;

	/* implies end-of-file */
	if (startline < 0) {
		/* compute offset, and be sure offset % width is 0 */
		offset = FileSize-blksize;
		if (offset < 0) offset = moff;
		diff = offset % width;
		if (diff)
			offset += (width-diff);
	}
	DataOffset = offset;
	Dwidth = width;
	Dheight = height;
	Dblksize = blksize;

	/* always read into the same place */
	if (!data)
		data = (unsigned char *)malloc(blksize*sizeof(unsigned char));
	fseek(fp, offset, 0);
	bytes = fread(data, sizeof(unsigned char), blksize, fp);
	AtEOF = (bytes < blksize) ? True : False;
	if ((offset+bytes+1) >= FileSize)
		AtEOF = True;
	/* ensure we return a full block.  pad w/ zeros if necessary */
	for (i=bytes; i < blksize; i++) {
		*(data+i) = (unsigned char)0;
	}
	return data;
}

/* read n bytes starting at begin... this is for decoding */
ReadNbytes(long begin, int n, unsigned char *where) {
	fseek(fp, begin, 0);
	fread(where, sizeof(unsigned char), n, fp);
}

/* return byte that is at specified position */
unsigned char
ByteAtPosition(int line, int chpos) {
	int  off = ((line-1)*Dwidth)+chpos-1;

	return *(data+off);
}

/* Returns True if the specified address is on the screen, False otherwise */
Boolean
AddressOnScreen(long addr) {
	long begin = DataOffset;
	long end = begin + Dblksize -1;

	return (addr >= begin && addr <= end) ? True : False;
}


Boolean
FileOpen(char *filename) {
	FILE *oldfp = fp;
	FILE *newfp;

	if (filename)
		newfp = fopen(filename, "r");
	else
		return False;

	if (newfp) {
		if (oldfp) fclose(oldfp);
		fp = newfp;
	}
	else
		return False;

	ResetMouseCoord();
	/* save the file name being looked at */
	if (FileName)
		free(FileName);
	FileName = (char *)malloc(strlen(filename)+1);
	strcpy(FileName, filename);

	/* figure out the size of the file we're looking at */
	fseek(fp, 0, 2);
	FileSize = ftell(fp);
	fseek(fp, 0, 0);
	return True;
}

/* return largest allowed line #, knowing size of file */
LargestLine(void) {
	return FileSize/GetBytesHoriz();
}

long
CurrentFileSize(void) {
	return FileSize;
}
