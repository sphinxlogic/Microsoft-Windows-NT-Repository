/* Copyright (C) 1989, 1992, 1993 Aladdin Enterprises.  All rights reserved.

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

/* gxfont.h */
/* Internal font definition for Ghostscript library */
/* Requires gsccode.h, gsmatrix.h, gxdevice.h */
#include "gsfont.h"
#include "gsuid.h"

/* A font object as seen by clients. */
/* See the PostScript Language Reference Manual for details. */

#ifndef gs_show_enum_s_DEFINED
struct gs_show_enum_s;
#endif

/* A client-supplied BuildChar/BuildGlyph procedure. */
/* The gs_char may be gs_no_char (for BuildGlyph), or the gs_glyph */
/* may be gs_no_glyph (for BuildChar), but not both. */
#define gs_proc_build_char(proc)\
  int proc(P5(struct gs_show_enum_s *, struct gs_state_s *,\
    gs_font *, gs_char, gs_glyph))

gs_proc_build_char(gs_no_build_char_proc);

/* A client-supplied character encoding procedure. */
#define gs_proc_encode_char(proc)\
  gs_glyph proc(P3(struct gs_show_enum_s *, gs_font *, gs_char *))

gs_proc_encode_char(gs_no_encode_char_proc);

/* Define the known font types. */
/* These numbers must be the same as the values of FontType */
/* in font dictionaries. */
typedef enum {
	ft_composite = 0,
	ft_encrypted = 1,
	ft_user_defined = 3
} font_type;

/* Define the bitmap font behaviors. */
/* These numbers must be the same as the values of the ExactSize, */
/* InBetweenSize, and TransformedChar entries in font dictionaries. */
typedef enum {
	fbit_use_outlines = 0,
	fbit_use_bitmaps = 1,
	fbit_transform_bitmaps = 2
} fbit_type;

/* Define the composite font mapping types. */
/* These numbers must be the same as the values of FMapType */
/* in type 0 font dictionaries. */
typedef enum {
	fmap_8_8 = 2,
	fmap_escape = 3,
	fmap_1_7 = 4,
	fmap_9_7 = 5,
	fmap_SubsVector = 6,
	fmap_double_escape = 7,
	fmap_shift = 8
} fmap_type;
#define fmap_type_min 2
#define fmap_type_max 8
#define fmap_type_is_modal(fmt)\
  ((fmt) == fmap_escape || (fmt) == fmap_double_escape || (fmt) == fmap_shift)

/* This is the type-specific information for a type 0 (composite) gs_font. */
typedef struct gs_type0_data_s gs_type0_data;
struct gs_type0_data_s {
	fmap_type FMapType;
	byte EscChar, ShiftIn, ShiftOut;
	byte *SubsVector;
	  uint subs_size;		/* bytes per entry */
	  uint subs_width;		/* # of entries */
	uint *Encoding;
	  uint encoding_size;
	gs_font **FDepVector;
	  uint fdep_size;
};

/* This is the type-specific information for a type 1 (encrypted) gs_font. */
/* The zone_table values should be ints, according to the Adobe */
/* specification, but some fonts have arbitrary floats here. */
#define zone_table(size)\
	struct {\
		int count;\
		float values[(size)*2];\
	}
#define stem_table(size)\
	struct {\
		int count;\
		float values[size];\
	}
typedef struct gs_type1_data_s gs_type1_data;
struct gs_type1_data_s {
	int PaintType;
	int (*subr_proc)(P3(gs_type1_data *pdata,
			    int index, const byte **pcharstring));
	int (*pop_proc)(P2(gs_type1_data *, fixed *));
	char *proc_data;		/* data for subr_proc */
	int lenIV;
	/* For a description of the following hint information, */
	/* see chapter 5 of the "Adobe Type 1 Font Format" book. */
	int BlueFuzz;
	float BlueScale;
	float BlueShift;
#define max_BlueValues 7
	zone_table(max_BlueValues) BlueValues;
	float ExpansionFactor;
	int ForceBold;
#define max_FamilyBlues 7
	zone_table(max_FamilyBlues) FamilyBlues;
#define max_FamilyOtherBlues 5
	zone_table(max_FamilyOtherBlues) FamilyOtherBlues;
	int LanguageGroup;
#define max_OtherBlues 5
	zone_table(max_OtherBlues) OtherBlues;
	int RndStemUp;
	stem_table(1) StdHW;
	stem_table(1) StdVW;
#define max_StemSnap 12
	stem_table(max_StemSnap) StemSnapH;
	stem_table(max_StemSnap) StemSnapV;
};
#define gs_type1_data_s_DEFINED

/* Even though it costs a little extra space, it's more convenient to */
/* include all the necessary information for >>all<< known font types */
/* (user-defined, encrypted, and composite) in the font structure. */
/* The font names are only needed for xfont lookup. */
typedef struct gs_font_name_s {
#define gs_font_name_max 47		/* must be >= 40 */
	/* The +1 is so we can null-terminate for debugging printout. */
	byte chars[gs_font_name_max+1];
	  uint size;
} gs_font_name;
struct gs_font_s {
	gs_font *next, *prev;		/* chain for original font list or */
					/* scaled font cache */
	gs_font_dir *dir;		/* directory where registered */
	gs_font *base;			/* original (unscaled) base font */
	char *client_data;		/* additional client data */
	gs_matrix FontMatrix;
	font_type FontType;
	int BitmapWidths;		/* boolean */
	fbit_type ExactSize, InBetweenSize, TransformedChar;
	int WMode;			/* 0 or 1 */
	gs_proc_build_char((*build_char_proc));	/* BuildChar/BuildGlyph */
	gs_proc_encode_char((*encode_char_proc));
	gs_proc_glyph_name((*glyph_name_proc));
	union _d {
		/* Composite (type 0) fonts */
		gs_type0_data type0_data;
		/* Base (non-type 0) fonts */
		struct _b {
			gs_rect FontBBox;
			gs_uid UID;
			int encoding_index;	/* 0=Std, 1=ISOLatin1, */
						/* 2=Symbol, 3=Dingbats, */
						/* -1=other */
			int nearest_encoding_index;	/* (may be >= 0 */
					/* even if encoding_index = -1) */
			/* Type 1 data */
			gs_type1_data type1_data;
		} base;
	} data;
	/* We store both the FontDirectory key (key_name) and, */
	/* if present, the FontName (font_name). */
	gs_font_name key_name, font_name;
};
