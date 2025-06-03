/* Copyright (C) 1992 Aladdin Enterprises.  All rights reserved.

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

/* zimage2.c */
/* image operator extensions for Level 2 PostScript */
#include "memory_.h"
#include "ghost.h"
#include "errors.h"
#include "oper.h"
#include "gscolor.h"
#include "gxcolor.h"		/* for gscolor2.h */
#include "gscspace.h"
#include "gscolor2.h"
#include "gsmatrix.h"
#include "dict.h"
#include "dparam.h"
#include "ilevel.h"
#include "state.h"		/* for igs */

/* Imported from zpaint.c */
extern int zimage_setup(P10(int width, int height, gs_matrix *pmat,
  ref *sources, int bits_per_component,
  int spread, const gs_color_space *pcs, int masked,
  const float *decode, int npop));

/* Names of keys in image dictionaries: */
static ref name_ImageType;
static ref name_Width;
static ref name_Height;
static ref name_ImageMatrix;
static ref name_MultipleDataSources;
static ref name_DataSource;
static ref name_BitsPerComponent;
static ref name_Decode;
static ref name_Interpolate;

/* Initialization */
private void
zimage2_init(void)
{	static const names_def imn[] = {

	/* Create the names of the known entries in */
	/* an image dictionary. */
	   { "ImageType", &name_ImageType },
	   { "Width", &name_Width },
	   { "Height", &name_Height },
	   { "ImageMatrix", &name_ImageMatrix },
	   { "MultipleDataSources", &name_MultipleDataSources },
	   { "DataSource", &name_DataSource },
	   { "BitsPerComponent", &name_BitsPerComponent },
	   { "Decode", &name_Decode },
	   { "Interpolate", &name_Interpolate },

	/* Mark the end of the initialized name list. */
	    names_def_end
	};

	init_names(imn);
}


/* Define a structure for acquiring image parameters. */
typedef struct image_params_s {
	int Width;
	int Height;
	gs_matrix ImageMatrix;
	int /*boolean*/ MultipleDataSources;
	ref DataSource[4];
	int BitsPerComponent;
	float Decode[2*4];
	int /*boolean*/ Interpolate;
} image_params;

/* Common code for unpacking an image dictionary. */
/* Assume *op is a dictionary. */
private int
image_dict_unpack(os_ptr op, image_params *pip, int max_bits_per_component)
{	int code;
	int num_components;
	int decode_size;
	ref *pds;
	check_dict_read(*op);
	num_components = gs_currentcolorspace(igs)->type->num_components;
	if ( num_components < 1 )
		return_error(e_rangecheck);
	if ( (code = dict_int_param(op, &name_ImageType, 1, 1, 1,
				    &code)) < 0 ||
	     (code = dict_int_param(op, &name_Width, 0, 0x7fff, -1,
				    &pip->Width)) < 0 ||
	     (code = dict_int_param(op, &name_Height, 0, 0x7fff, -1,
				    &pip->Height)) < 0 ||
	     (code = dict_matrix_param(op, &name_ImageMatrix,
				    &pip->ImageMatrix)) < 0 ||
	     (code = dict_bool_param(op, &name_MultipleDataSources, 0,
				    &pip->MultipleDataSources)) < 0 ||
	     (code = dict_int_param(op, &name_BitsPerComponent, 0,
				    max_bits_per_component, -1,
				    &pip->BitsPerComponent)) < 0 ||
	     (code = decode_size = dict_float_array_param(op, &name_Decode,
				    num_components * 2,
				    &pip->Decode[0], NULL)) < 0 ||
	     (code = dict_bool_param(op, &name_Interpolate, 0,
				    &pip->Interpolate)) < 0
	   )
		return code;
	if ( decode_size != num_components * 2 )
		return_error(e_rangecheck);
	/* Extract and check the data sources. */
	if ( (code = dict_find(op, &name_DataSource, &pds)) < 0 )
		return code;
	if ( pip->MultipleDataSources )
	{	check_type(*pds, t_array);
		if ( r_size(pds) != num_components )
			return_error(e_rangecheck);
		memcpy(&pip->DataSource[0], pds->value.refs, sizeof(ref) * num_components);
	}
	else
		pip->DataSource[0] = *pds;
	return 0;
}

/* (<width> <height> <bits/sample> <matrix> <datasrc> image -) */
/* <dict> image - */
int
z2image(register os_ptr op)
{	if ( level2_enabled && r_has_type(op, t_dictionary) )
	{	image_params ip;
		int code = image_dict_unpack(op, &ip, 12);
		if ( code < 0 )
			return code;
		return zimage_setup(ip.Width, ip.Height, &ip.ImageMatrix,
			&ip.DataSource[0], ip.BitsPerComponent,
			0, gs_currentcolorspace(igs), 0, &ip.Decode[0], 1);
	}
	/* Level 1 image operator */
	check_op(5);
	return zimage(op);
}

/* (<width> <height> <paint_1s> <matrix> <datasrc> imagemask -) */
/* <dict> imagemask - */
int
z2imagemask(register os_ptr op)
{	if ( level2_enabled && r_has_type(op, t_dictionary) )
	{	image_params ip;
		gs_color_space cs;
		int code = image_dict_unpack(op, &ip, 1);
		if ( code < 0 )
			return code;
		if ( ip.MultipleDataSources )
			return_error(e_rangecheck);
		cs.type = &gs_color_space_type_DeviceGray;
		return zimage_setup(ip.Width, ip.Height, &ip.ImageMatrix,
			&ip.DataSource[0], 1, 0, &cs, 1, &ip.Decode[0], 1);
	}
	/* Level 1 imagemask operator */
	check_op(5);
	return zimagemask(op);
}

/* ------ Initialization procedure ------ */

/* Note that these override the definitions in zpaint.c. */
op_def zimage2_op_defs[] = {
	{"1image", z2image},
	{"1imagemask", z2imagemask},
	op_def_end(zimage2_init)
};
