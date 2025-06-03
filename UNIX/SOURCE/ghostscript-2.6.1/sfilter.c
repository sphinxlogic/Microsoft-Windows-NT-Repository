/* Copyright (C) 1991, 1992, 1993 Aladdin Enterprises.  All rights reserved.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* sfilter.c */
/* Stream functions for filters */
#include "stdio_.h"	/* includes std.h */
#include "memory_.h"
#include "scanchar.h"
#include "stream.h"
#include "gscrypt1.h"

/* ------ Generic functions ------ */

/* Implement flushing for all encoding filters. */
int
s_filter_write_flush(register stream *s)
{	(*s->procs.write_buf)(s);
	return sflush(s->strm);
}

/* Close a filter.  If this is an encoding filter, flush it first. */
int
s_filter_close(register stream *s)
{	if ( s_is_writing(s) )
	   {	int code = sflush(s);
		if ( code == ERRC ) return code;
	   }
	return s_std_close(s);
}	

/* ------ ASCIIHexEncode ------ */

/* Flush the buffer */
private int
s_AXE_write_buf(register stream *s)
{	register stream *strm = s->strm;
	register byte *p = s->cbuf;
	register int count = s->cptr + 1 - p;
	register const char _ds *hex_digits = "0123456789abcdef";
	while ( --count >= 0 )
	  { char c1 = hex_digits[*p >> 4];
	    char c2 = hex_digits[*p & 0xf];
	    sputc(strm, c1);
	    sputc(strm, c2);
	    if ( !(count & 15) )
	      sputc(strm, '\n');
	    p++;
	  }
	s->cptr = s->cbuf - 1;
	return 0;
}

/* Close the stream */
private int
s_AXE_close(register stream *s)
{	(*s->procs.write_buf)(s);
	sputc(s->strm, '>');
	return s_std_close(s);
}

/* Stream procedures */
const stream_procs s_AXE_procs =
   {	s_std_noavailable, NULL, s_filter_write_flush, s_AXE_close,
	NULL, s_AXE_write_buf
   };

/* ------ ASCIIHexDecode ------ */

/* Initialize the stream */
void
s_AXD_init(register stream *s)
{	s->odd = -1;
}

/* Refill the buffer */
private int
s_AXD_read_buf(register stream *s)
{	stream *strm = s->strm;
	uint count;
	int code = sreadhex(strm, s->cbuf, s->bsize, &count, &s->odd, 0);
	s->cptr = s->cbuf - 1;
	s->endptr = s->cptr + count;
	if ( code != ERRC )
	{	if ( code )
			s->end_status = strm->end_status;
		return 0;
	}
	/* Check for EOD */
	if ( sgetc(strm) == '>' )	/* EOD */
		s->end_status = EOFC;
	else			/* syntax error */
	   {	s->end_status = ERRC;
		sputback(strm);
	   }
	return 0;
}

/* Stream procedures */
const stream_procs s_AXD_procs =
   {	s_std_noavailable, s_std_noseek, s_std_read_flush, s_filter_close,
	s_AXD_read_buf, NULL
   };

/* ------ eexecDecode ------ */

/* Initialize a stream for reading and decrypting another stream. */
/* Decrypting streams are not positionable. */
void
s_exD_init(register stream *s, ushort /*crypt_state*/ state)
{	s->cstate = state;
	s->odd = -1;
	s->binary = -1;			/* unknown */
}

/* Refill the buffer of a decrypting stream. */
private int
s_exD_read_buf(register stream *s)
{	byte *buf = s->cbuf;
	uint nread;
	int skip = (s->binary < 0 ? 4 : 0);
	s->cptr = s->endptr = buf - 1;
top:	nread = sgets(s->strm, buf, s->bsize);
	if ( nread == 0 )		/* end of stream */
	   {	s->end_status = EOFC;
		return 0;
	   }
	if ( s->binary < 0 )
	   {	/* This is the very first time we're filling the buffer. */
		/* Determine whether this is ASCII or hex encoding. */
		register byte _ds *decoder = scan_char_decoder;
		if ( nread < 4 ) return EOFC;
		if ( decoder[buf[0]] == ctype_space ||
		     (decoder[buf[0]] | decoder[buf[1]] | decoder[buf[2]] |
		      decoder[buf[3]]) <= 0xf )
		   {	/* Would be invalid if binary, hence must be hex. */
			s->binary = 0;
		   }
		else	s->binary = 1;
			
	   }
	if ( !s->binary )
	   {	/* Convert the buffer from binary to hex in place. */
		stream sst;
		sread_string(&sst, buf, nread);
		sreadhex(&sst, buf, nread, &nread, &s->odd, 0);
		if ( nread == 0 ) goto top;	/* try again */
	   }
	/* Now decrypt the buffer. */
	gs_type1_decrypt(buf, buf, nread, (crypt_state *)&s->cstate);
	if ( skip )
	   {	/* Very first buffer-load, strip off leading bytes. */
		if ( nread < skip ) return EOFC;
		s->cptr += skip;
		nread -= skip;
	   }
	s->endptr = s->cptr + nread;
	return 0;
}

/* Estimate the number of remaining bytes in a decrypting stream. */
private int
s_exD_available(stream *s, long *pl)
{	if ( savailable(s->strm, pl) < 0 ) return ERRC;
	if ( *pl >= 0 ) *pl /= 2;
	return 0;
}

/* Stream procedures */
const stream_procs s_exD_procs =
   {	s_exD_available, s_std_noseek, s_std_read_flush, s_filter_close,
	s_exD_read_buf, NULL
   };

/* ------ NullEncode ------ */

/* Flush the buffer */
private int
s_NullE_write_buf(register stream *s)
{	uint count = s->cptr - s->cbuf + 1;
	sputs(s->strm, s->cbuf, count);
	s->cptr = s->cbuf - 1;
	s->position += count;
	return 0;
}

/* Stream procedures */
const stream_procs s_NullE_procs =
   {	s_std_noavailable, s_std_noseek, s_filter_write_flush, s_filter_close,
	NULL, s_NullE_write_buf
   };

/* ------ PFBDecode ------ */

/* Initialize the stream */
void
s_PFBD_init(register stream *s, int binary_to_hex)
{	s->record_type = -1;
	s->binary_to_hex = binary_to_hex;
}

/* Refill the buffer */
private int
s_PFBD_read_buf(register stream *s)
{	stream *strm = s->strm;
	register byte *ptr = s->cbuf;
	uint count;
	int c;
	s->cptr = s->endptr = s->cbuf - 1;
top:	count = s->bsize;
	switch ( s->record_type )
	   {
	case -1:			/* new record */
		c = sgetc(strm);
		if ( c != 0x80 ) goto err;
		c = sgetc(strm);
		switch ( c )
		   {
		case 1: case 2:
			s->record_type = c;
			break;
		case 3:
		case EOFC:
			s->end_status = EOFC;
			return 0;
		default:
			goto err;
		   }
		s->record_left = sgetc(strm);
		s->record_left += (ulong)sgetc(strm) << 8;
		s->record_left += (ulong)sgetc(strm) << 16;
		s->record_left += (ulong)sgetc(strm) << 24;
		goto top;
	case 1:				/* text data */
		/* Translate \r to \n. */
		if ( count > s->record_left ) count = s->record_left;
	   {	uint n;
		for ( n = count; n != 0 && (c = sgetc(strm)) != EOFC; n-- )
		   {	*ptr++ = (c == '\r' ? '\n' : c);
		   }
	   }	break;
	case 2:				/* binary data */
		if ( s->binary_to_hex )
		{	/* Translate binary to hex. */
			uint n;
			byte *inp;
			register const char _ds *hex_digits = "0123456789abcdef";
			count >>= 1;		/* 2 chars per input byte */
			if ( count > s->record_left ) count = s->record_left;
			inp = ptr + count;
			count = sgets(strm, inp, count);
			for ( n = count; n != 0; n-- )
			{	c = *inp++;
				ptr[0] = hex_digits[c >> 4];
				ptr[1] = hex_digits[c & 0xf];
				ptr += 2;
			}
		}
		else
		{	/* Just read binary data. */
			if ( count > s->record_left ) count = s->record_left;
			count = sgets(strm, ptr, count);
			ptr += count;
		}
		break;
	   }
	if ( count == 0 )
	   {	s->record_type = -1;
		goto top;
	   }
	s->record_left -= count;
	s->endptr = ptr - 1;
	return 0;
err:	s->end_status = ERRC;
	s->endptr = ptr - 1;
	return 0;
}

/* Stream procedures */
const stream_procs s_PFBD_procs =
   {	s_std_noavailable, s_std_noseek, s_std_read_flush, s_filter_close,
	s_PFBD_read_buf, NULL
   };

/* ------ SubFileDecode ------ */

/* Initialize the stream */
void
s_SFD_init(stream *s, long count, const byte *str, uint strlen)
{	s->state.sf.count = count;
	s->state.sf.eod_string = str;
	s->state.sf.string_size = strlen;
	s->state.sf.match = 0;
}

/* Refill the buffer */
private int
s_SFD_read_buf(register stream *s)
{	stream *strm = s->strm;
	if ( s->state.sf.string_size == 0 )
	   {	/* Just read, with no EOD pattern. */
		uint count;
		if ( s->state.sf.count == 0 )
		   {	/* Read with no EOD limit. */
			count = sgets(strm, s->cbuf, s->bsize);
			s->end_status = strm->end_status;
		   }
		else
		   {	/* Count bytes till EOD. */
			uint len = min(s->bsize, s->state.sf.count);
			count = sgets(strm, s->cbuf, len);
			s->end_status = ((s->state.sf.count -= count) == 0 ?
					 EOFC : strm->end_status);
		   }
		s->cptr = s->cbuf - 1;
		s->endptr = s->cptr + count;
	   }
	else
	   {	/* Read looking for an EOD pattern. */
		uint keep = s->endptr + 1 - s->cbuf;
		byte *p = s->cbuf - 1;
		byte *limit = p + s->bsize;
		uint match = s->state.sf.match;
		if ( keep > match ) keep = match;
		memcpy(s->cbuf, s->endptr + 1 - keep, keep);
		s->cptr = p + keep;
		while ( p < limit )
		   {	int c = sgetc(strm);
			if ( c < 0 )		/* premature EOF */
			   {	s->end_status = c;
				s->endptr = p;
				return 0;
			   }
			*++p = c;
			if ( c == s->state.sf.eod_string[match] )
			   {	if ( ++match == s->state.sf.string_size )
				   {	if ( s->state.sf.count <= 1 )
					   {	s->endptr =
						  (s->state.sf.count == 0 ?
						   p - s->state.sf.string_size :
						   p);
						s->end_status = EOFC;
						return 0;
					   }
					s->state.sf.count--;
					match = 0;
				   }
			   }
			/* No match here, back up to find the longest one. */
			/* This may be quadratic in string_size, but */
			/* we don't expect this to be a real problem. */
			for ( ; match != 0; match-- )
			  if ( !memcmp(s->state.sf.eod_string, p - match + 1,
				       match)
			     )
				break;
		   }
		s->endptr = p;
		s->state.sf.match = match;
	   }
	return 0;
}

/* Stream procedures */
const stream_procs s_SFD_procs =
   {	s_std_noavailable, s_std_noseek, s_std_read_flush, s_filter_close,
	s_SFD_read_buf, NULL
   };
