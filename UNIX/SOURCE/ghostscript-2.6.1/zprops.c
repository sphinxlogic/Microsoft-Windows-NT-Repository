/* Copyright (C) 1991, 1992 Aladdin Enterprises.  All rights reserved.

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

/* zprops.c */
/* Device property operators */
#include "memory_.h"
#include "ghost.h"
#include "alloc.h"
#include "dict.h"
#include "errors.h"
#include "iname.h"
#include "oper.h"
#include "state.h"
#include "store.h"
#include "gsprops.h"
#include "gsmatrix.h"			/* for gxdevice.h */
#include "gxdevice.h"
#include "gsstate.h"

/* Forward references */
private int props_to_stack(P3(const gs_prop_item *, os_ptr, int));
private int props_from_stack(P3(gs_prop_item *, const_os_ptr, int));

/* <device> getdeviceprops <mark> <name> <value> ... */
int
zgetdeviceprops(os_ptr op)
{	gx_device *dev;
	gs_prop_item *plist;
	int count;
	int code;
	check_type(*op, t_device);
	dev = op->value.pdevice;
	count = gs_getdeviceprops_size(dev);
	plist = (gs_prop_item *)alloc(count, sizeof(gs_prop_item), "getdeviceprops");
	if ( plist == 0 ) return_error(e_VMerror);
	code = gs_getdeviceprops(dev, plist);
	if ( code >= 0 ) code = props_to_stack(plist, op + 1, count);
	alloc_free((char *)plist, count, sizeof(gs_prop_item), "getdeviceprops");
	if ( code >= 0 )
	   {	make_mark(op);
		push(code);
		code = 0;
	   }
	return code;
}

/* <mark> <name> <value> ... <device> putdeviceprops <device> */
int
zputdeviceprops(os_ptr op)
{	gx_device *dev;
	gs_prop_item *plist;
	os_ptr mp;
	int count, acount = 0;
	int code;
	int old_width, old_height;
	check_type(*op, t_device);
	dev = op->value.pdevice;
	old_width = dev->width;
	old_height = dev->height;
	for ( mp = op - 1; !r_has_type(mp, t_mark); mp-- )
	   {	if ( mp <= osbot )
			return_error(e_unmatchedmark);
		if ( r_is_array(mp) )
			acount += r_size(mp);
	   }
	count = op - mp - 1;
	if ( count & 1 )
		return_error(e_rangecheck);
	count >>= 1;
	plist = (gs_prop_item *)alloc(count + acount, sizeof(gs_prop_item), "putdeviceprops");
	if ( plist == 0 ) return_error(e_VMerror);
	code = props_from_stack(plist, mp + 1, count);
	if ( code >= 0 )
		code = gs_putdeviceprops(dev, plist, count);
	alloc_free((char *)plist, count + acount, sizeof(gs_prop_item), "putdeviceprops");
	if ( code > 0 || (code == 0 && (dev->width != old_width || dev->height != old_height)) )
	{	/* The device was open and is now closed, */
		/* or its dimensions have changed. */
		/* If it was the current device, */
		/* call setdevice to reinstall it and erase the page. */
		/****** DOESN'T FIND ALL THE GSTATES THAT REFERENCE THE DEVICE. ******/
		if ( gs_currentdevice(igs) == dev )
		{	int was_open = dev->is_open;
			code = gs_setdevice(igs, dev);
			/* If the device wasn't closed, */
			/* setdevice won't erase the page. */
			if ( was_open && code >= 0 )
				code = gs_erasepage(igs);
		}
	}
	if ( code >= 0 )
	   {	*mp = *op;
		osp = op = mp;
		code = 0;
	   }
	return code;
}

/* ------ Initialization procedure ------ */

op_def zprops_op_defs[] = {
	{"1getdeviceprops", zgetdeviceprops},
	{"2putdeviceprops", zputdeviceprops},
	op_def_end(0)
};

/* ------ Internal routines ------ */

/* Get properties from a property list to the stack. */
private int
props_to_stack(const gs_prop_item *plist, os_ptr op0, int count)
{	const gs_prop_item *pi;
	os_ptr op;
	int i;
	int code;
	for ( op = op0, pi = plist, i = count; i != 0; pi++, i-- )
	   {	ref value;
		const char *nstr = pi->pname;
		int nlen = pi->name_size;
		if ( nstr == 0 ) continue;	/* no name, skip */
		if ( ostop - op < 2 )
			return_error(e_stackoverflow);
		if ( nlen < 0 ) nlen = strlen(nstr);
		code = name_ref((const byte *)nstr, nlen, op, 0);
		if ( code < 0 ) return code;
		switch ( pi->type )
		   {
		case (int)prt_int:
			make_int(&value, pi->value.i);
			break;
		case (int)prt_float:
			make_real(&value, pi->value.f);
			break;
		case (int)prt_bool:
			make_bool(&value, pi->value.b);
			break;
		case (int)prt_string:
		   {	ushort size = pi->value.a.size;
			char *str;
			if ( size == (ushort)(-1) )
				size = strlen(pi->value.a.p.s);
			str = alloc(size, 1, "props_to_stack(string)");
			if ( str == 0 )
				return_error(e_VMerror);
			memcpy(str, pi->value.a.p.s, size);
			make_string(&value, a_all, size, (byte *)str);
		   }	break;
		case (int)prt_int_array:
		case (int)prt_float_array:
		   {	uint size = pi->value.a.size;
			ref *arefs;
			uint j;
			gs_prop_item *pv = pi->value.a.p.v;
			code = alloc_array(&value, a_all, size, "props_to_stack(array)");
			if ( code < 0 )
				return code;
			arefs = value.value.refs;
			for ( j = 0; j < size; j++, arefs++, pv++ )
			  if ( pi->type == prt_int_array )
				make_int_new(arefs, pv->value.i);
			  else
				make_real_new(arefs, pv->value.f);
		   }	break;
		default:
			return_error(e_typecheck);
		   }
		ref_assign(op + 1, &value);
		op += 2;
	   }
	return op - op0;
}

/* Set properties from the stack. */
/* Returns the number of elements copied. */
/* Entries with non-name keys are not copied; */
/* entries with invalid values are copied with status = pv_typecheck. */
private int
props_from_stack(gs_prop_item *plist /* [count + acount] */, const_os_ptr op0,
  int count)
{	gs_prop_item *pi = plist;
	gs_prop_item *pai = plist + count;
	const_os_ptr op = op0 + 1;
	for ( ; count; op += 2, count-- )
	   {	ref sref;
		if ( !r_has_type(op - 1, t_name) )
			return_error(e_typecheck);
		name_string_ref(op - 1, &sref);
		pi->pname = (char *)sref.value.bytes;
		pi->name_size = r_size(&sref);
		pi->status = pv_set;
		switch ( r_type(op) )
		   {
		case t_null:
			pi->type = prt_null;
			break;
		case t_integer:
			pi->type = prt_int;
			pi->value.i = op->value.intval;
			break;
		case t_real:
			pi->type = prt_float;
			pi->value.f = op->value.realval;
			break;
		case t_boolean:
			pi->type = prt_bool;
			pi->value.b = op->value.index;
			break;
		case t_name:
			name_string_ref(op, &sref);
			goto nst;
		case t_string:
			ref_assign(&sref, op);
			pi->type = prt_string;
			pi->value.a.p.s = (char *)op->value.bytes;
nst:			pi->value.a.size = r_size(&sref);
			break;
		case t_array:
		case t_mixedarray:
		case t_shortarray:
		   {	uint size = r_size(op);
			uint i;
			gs_prop_item *pv;
			gs_prop_type tv = prt_int;
			pi->type = prt_int_array;
			pi->value.a.p.v = pai;
			pi->value.a.size = size;
top:			pv = pai;
			for ( i = 0; i < size; i++ )
			   {	ref rnum;
				array_get(op, (long)i, &rnum);
				pv->pname = 0;
				pv->type = tv;
				switch ( r_type(&rnum) )
				   {
				case t_real:
					if ( tv == prt_int )
					   {	tv = prt_float;
						pi->type = prt_float_array;
						goto top;
					   }
					pv++->value.f = rnum.value.realval;
					break;
				case t_integer:
					if ( tv == prt_int )
					  pv++->value.i = rnum.value.intval;
					else
					  pv++->value.f = rnum.value.intval;
					break;
				default:
					pi->status = pv_typecheck;
				   }
			   }

			pai = pv;
		   }	break;
		default:
			pi->status = pv_typecheck;
		   }
		pi++;
	   }
	return 0;
}
