/* Copyright (C) 1994, 1998 Aladdin Enterprises.  All rights reserved.

   This file is part of Aladdin Ghostscript.

   Aladdin Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author
   or distributor accepts any responsibility for the consequences of using it,
   or for whether it serves any particular purpose or works at all, unless he
   or she says so in writing.  Refer to the Aladdin Ghostscript Free Public
   License (the "License") for full details.

   Every copy of Aladdin Ghostscript must include a copy of the License,
   normally in a plain ASCII text file named PUBLIC.  The License grants you
   the right to copy, modify and redistribute Aladdin Ghostscript, but only
   under certain conditions described in the License.  Among other things, the
   License requires that the copyright notice and this notice be preserved on
   all copies.
 */

/*$Id: gxtmap.h,v 1.1 2000/03/09 08:40:43 lpd Exp $ */
/* Definition of transfer mapping function */
/* (also used for black generation and undercolor removal) */

#ifndef gxtmap_INCLUDED
#  define gxtmap_INCLUDED

/* Common definition for mapping procedures. */
/* These are used for transfer functions, black generation, */
/* and undercolor removal. */
/* gx_transfer_map should probably be renamed gx_mapping_cache.... */

/* Define an abstract type for a transfer map. */
typedef struct gx_transfer_map_s gx_transfer_map;

/*
 * Define the type of a mapping procedure.  There are two forms of this.
 * The original form passed only the transfer map itself as an argument:
 */
typedef float (*gs_mapping_proc) (P2(floatp, const gx_transfer_map *));

/*
 * Later, we recognized that this procedure should really be a general
 * closure:
 */
typedef float (*gs_mapping_closure_proc_t) (P3(floatp value,
					       const gx_transfer_map * pmap,
					       const void *proc_data));
typedef struct gs_mapping_closure_s {
    gs_mapping_closure_proc_t proc;
    const void *data;
} gs_mapping_closure_t;

#endif /* gxtmap_INCLUDED */
