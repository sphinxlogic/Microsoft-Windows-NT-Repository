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

/* zfilter.c */
/* Filter creation for Ghostscript */
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "alloc.h"
#include "stream.h"

/* Forward references */
int filter_read(P4(os_ptr, const stream_procs _ds *, stream **, uint));
int filter_write(P4(os_ptr, const stream_procs _ds *, stream **, uint));

/* Imported from zfile.c */
extern const uint file_default_buffer_size;

/* <source> .filter_ASCIIHexEncode <file> */
extern const stream_procs s_AXE_procs;
int
zAXE(os_ptr op)
{	return filter_write(op, &s_AXE_procs, NULL, 0);
}

/* <target> .filter_ASCIIHexDecode <file> */
extern const stream_procs s_AXD_procs;
extern void s_AXD_init(P1(stream *));
int
zAXD(os_ptr op)
{	stream *s;
	int code = filter_read(op, &s_AXD_procs, &s, 0);
	if ( code < 0 ) return code;
	s_AXD_init(s);
	return code;
}

/* <source> <seed> .filter_eexecDecode <file> */
extern const stream_procs s_exD_procs;
extern void s_exD_init(P2(stream *, ushort));
int
zexD(register os_ptr op)
{	stream *s;
	ushort state;
	int code;
	check_type(*op, t_integer);
	state = op->value.intval;
	if ( op->value.intval != state )
		return_error(e_rangecheck);	/* state value was truncated */
	/* The buffer size of 512 is part of the specification of eexec. */
	code = filter_read(op - 1, &s_exD_procs, &s, 512);
	if ( code < 0 ) return code;
	s_exD_init(s, state);
	pop(1);
	return 0;
}

/* <target> .filter_NullEncode <file> */
extern const stream_procs s_NullE_procs;
int
zNullE(os_ptr op)
{	stream *s;
	int code = filter_write(op, &s_NullE_procs, &s, 0);
	if ( code >= 0 )
	{	/* NullEncode filters can report their position */
		s->modes |= s_mode_seek;
	}
	return code;
}

/* <source> <bool> .filter_PFBDecode <file> */
extern const stream_procs s_PFBD_procs;
extern void s_PFBD_init(P2(stream *, int));
int
zPFBD(os_ptr op)
{	stream *s;
	int code;
	check_type(*op, t_boolean);
	code = filter_read(op - 1, &s_PFBD_procs, &s, 0);
	if ( code < 0 ) return code;
	s_PFBD_init(s, op->value.index);
	pop(1);
	return 0;
}

/* <source> <EODcount> <EODstring> .filter_SubFileDecode <file> */
extern const stream_procs s_SFD_procs;
extern void s_SFD_init(P4(stream *, long, const byte *, uint));
int
zSFD(os_ptr op)
{	stream *s;
	int code;
	check_type(op[-1], t_integer);
	check_read_type(*op, t_string);
	if ( op[-1].value.intval < 0 )
		return_error(e_rangecheck);
	code = filter_read(op - 2, &s_SFD_procs, &s, r_size(op) * 2);
	if ( code < 0 ) return code;
	s_SFD_init(s, op[-1].value.intval, op->value.const_bytes, (uint)r_size(op));
	pop(2);
	return 0;
}

/* ------ Utilities ------ */

/* Free the underlying string stream if needed. */
private void
filter_free_null(stream *s)
{
}
private void
filter_free_stream(stream *s)
{	alloc_free((char *)s->strm, 1, sizeof(stream),
		   "filter_free_stream(stream)");
}

/* Set up an input filter. */
int
filter_read(os_ptr op, const stream_procs _ds *procs, stream **ps,
  uint min_size)
{	stream *s;
	stream *sstrm;
	int is_temp;
	int code;
	/* Check to make sure that the underlying data */
	/* can function as a source for reading. */
	switch ( r_type(op) )
	   {
	case t_string:
		check_read(*op);
		sstrm = (stream *)alloc(1, sizeof(stream),
					"filter_read(string stream)");
		if ( sstrm == 0 )
			return_error(e_VMerror);
		sread_string(sstrm, op->value.bytes, r_size(op));
		is_temp = 1;
		break;
	case t_file:
		check_read(*op);
		sstrm = op->value.pfile;
		is_temp = 0;
		break;
	default:
		return_error(e_typecheck);
	   }
	code = filter_open("r", max(min_size, file_default_buffer_size),
			   (ref *)op, procs, &s);
	if ( code < 0 ) return code;
	s->strm = sstrm;
	s->strm_is_temp = is_temp;
	if ( ps != NULL ) *ps = s;
	return 0;
}

/* Set up an output filter. */
int
filter_write(os_ptr op, const stream_procs _ds *procs, stream **ps,
  uint min_size)
{	stream *s;
	stream *sstrm;
	int is_temp;
	int code;
	/* Check to make sure that the underlying data */
	/* can function as a sink for writing. */
	switch ( r_type(op) )
	   {
	case t_string:
		check_write(*op);
		sstrm = (stream *)alloc(1, sizeof(stream),
					"filter_write(string)");
		if ( sstrm == 0 )
			return_error(e_VMerror);
		swrite_string(sstrm, op->value.bytes, r_size(op));
		is_temp = 1;
		break;
	case t_file:
		check_write(*op);
		sstrm = op->value.pfile;
		is_temp = 0;
		break;
	default:
		return_error(e_typecheck);
	   }
	code = filter_open("w", max(min_size, file_default_buffer_size),
			   (ref *)op, procs, &s);
	if ( code < 0 )
		return code;
	s->strm = sstrm;
	s->strm_is_temp = is_temp;
	if ( ps != NULL ) *ps = s;
	return 0;
}

/* ------ Initialization procedure ------ */

op_def zfilter_op_defs[] = {
	{"1.filter_ASCIIHexEncode", zAXE},
	{"1.filter_ASCIIHexDecode", zAXD},
	{"2.filter_eexecDecode", zexD},
	{"1.filter_NullEncode", zNullE},
	{"2.filter_PFBDecode", zPFBD},
	{"1.filter_SubFileDecode", zSFD},
	op_def_end(0)
};
