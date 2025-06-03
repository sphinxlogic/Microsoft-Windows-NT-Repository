/* coff object file format
   Copyright (C) 1989, 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "as.h"

#include "obstack.h"

#ifndef BFD_ASSEMBLER
lineno *lineno_rootP;

const short seg_N_TYPE[] =
{				/* in: segT   out: N_TYPE bits */
  C_ABS_SECTION,
  C_TEXT_SECTION,
  C_DATA_SECTION,
  C_BSS_SECTION,
  C_UNDEF_SECTION,		/* SEG_UNKNOWN */
  C_UNDEF_SECTION,		/* SEG_GOOF */
  C_UNDEF_SECTION,		/* SEG_EXPR */
  C_DEBUG_SECTION,		/* SEG_DEBUG */
  C_NTV_SECTION,		/* SEG_NTV */
  C_PTV_SECTION,		/* SEG_PTV */
  C_REGISTER_SECTION,		/* SEG_REGISTER */
};


/* Add 4 to the real value to get the index and compensate the negatives */

const segT N_TYPE_seg[32] =
{
  SEG_PTV,			/* C_PTV_SECTION	== -4 */
  SEG_NTV,			/* C_NTV_SECTION	== -3 */
  SEG_DEBUG,			/* C_DEBUG_SECTION	== -2 */
  SEG_ABSOLUTE,			/* C_ABS_SECTION	== -1 */
  SEG_UNKNOWN,			/* C_UNDEF_SECTION	== 0 */
  SEG_TEXT,			/* C_TEXT_SECTION	== 1 */
  SEG_DATA,			/* C_DATA_SECTION	== 2 */
  SEG_BSS,			/* C_BSS_SECTION	== 3 */
  SEG_REGISTER,			/* C_REGISTER_SECTION   == 4 */
  SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
  SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF,
  SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF, SEG_GOOF
};
#endif

char *s_get_name PARAMS ((symbolS * s));
static symbolS *tag_find_or_make PARAMS ((char *name));
static symbolS *tag_find PARAMS ((char *name));
#ifdef BFD_HEADERS
static void obj_coff_section_header_append PARAMS ((char **where, struct internal_scnhdr * header));
#else
static void obj_coff_section_header_append PARAMS ((char **where, SCNHDR * header));
#endif
static void obj_coff_def PARAMS ((int what));
static void obj_coff_dim PARAMS ((void));
static void obj_coff_endef PARAMS ((void));
static void obj_coff_line PARAMS ((void));
static void obj_coff_ln PARAMS ((int));
static void obj_coff_scl PARAMS ((void));
static void obj_coff_size PARAMS ((void));
static void obj_coff_tag PARAMS ((void));
static void obj_coff_type PARAMS ((void));
static void obj_coff_val PARAMS ((void));
static void tag_init PARAMS ((void));
static void tag_insert PARAMS ((char *name, symbolS * symbolP));

#ifdef BFD_ASSEMBLER
static void SA_SET_SYM_ENDNDX PARAMS ((symbolS *, symbolS *));
static void SA_SET_SYM_TAGNDX PARAMS ((symbolS *, symbolS *));
#endif

int line_base;

static struct hash_control *tag_hash;
static symbolS *def_symbol_in_progress;

const pseudo_typeS obj_pseudo_table[] =
{
#ifndef IGNORE_DEBUG
  {"def", obj_coff_def, 0},
  {"dim", obj_coff_dim, 0},
  {"endef", obj_coff_endef, 0},
  {"line", obj_coff_line, 0},
  {"ln", obj_coff_ln, 0},
  {"appline", obj_coff_ln, 1},
  {"scl", obj_coff_scl, 0},
  {"size", obj_coff_size, 0},
  {"tag", obj_coff_tag, 0},
  {"type", obj_coff_type, 0},
  {"val", obj_coff_val, 0},
#else
  {"def", s_ignore, 0},
  {"dim", s_ignore, 0},
  {"endef", s_ignore, 0},
  {"line", s_ignore, 0},
  {"ln", s_ignore, 0},
  {"scl", s_ignore, 0},
  {"size", s_ignore, 0},
  {"tag", s_ignore, 0},
  {"type", s_ignore, 0},
  {"val", s_ignore, 0},
#endif /* ignore debug */

  {"ident", s_ignore, 0},	/* we don't yet handle this. */

  {"optim", s_ignore, 0},	/* For sun386i cc (?) */
  /* other stuff */
  {"ABORT", s_abort, 0},

  {NULL}			/* end sentinel */
};				/* obj_pseudo_table */

#ifdef BFD_ASSEMBLER
struct line_no {
  struct line_no *next;
  fragS *frag;
  alent l;
};
#endif

#define GET_FILENAME_STRING(X) \
((char*)(&((X)->sy_symbol.ost_auxent->x_file.x_n.x_offset))[1])

/* obj dependant output values */
#ifndef BFD_ASSEMBLER
#ifdef BFD_HEADERS
static struct internal_scnhdr bss_section_header;
struct internal_scnhdr data_section_header;
struct internal_scnhdr text_section_header;
#else
static SCNHDR bss_section_header;
SCNHDR data_section_header;
SCNHDR text_section_header;
#endif
#endif

#ifdef BFD_ASSEMBLER

/* @@ Ick.  */
static segT
fetch_coff_debug_section ()
{
  static segT debug_section;
  if (!debug_section)
    {
      CONST asymbol *s;
      s = bfd_make_debug_symbol (stdoutput, (char *) 0, 0);
      assert (s != 0);
      debug_section = s->section;
    }
  return debug_section;
}

static void
SA_SET_SYM_ENDNDX (sym, val)
     symbolS *sym;
     symbolS *val;
{
  combined_entry_type *entry, *p;

  entry = &coffsymbol (sym->bsym)->native[1];
  p = coffsymbol (val->bsym)->native;
  entry->u.auxent.x_sym.x_fcnary.x_fcn.x_endndx.p = p;
  entry->fix_end = 1;
}

static void
SA_SET_SYM_TAGNDX (sym, val)
     symbolS *sym;
     symbolS *val;
{
  combined_entry_type *entry, *p;

  entry = &coffsymbol (sym->bsym)->native[1];
  p = coffsymbol (val->bsym)->native;
  entry->u.auxent.x_sym.x_tagndx.p = p;
  entry->fix_tag = 1;
}

#else /* ! BFD_ASSEMBLER */

/* Relocation. */

static int
reloc_compare (p1, p2)
#ifdef BFD_HEADERS
     struct internal_reloc *p1, *p2;
#else
     RELOC *p1, *p2;
#endif
{
  return (int) (p1->r_vaddr - p2->r_vaddr);
}

/*
 *		emit_relocations()
 *
 * Crawl along a fixS chain. Emit the segment's relocations.
 */

void
obj_emit_relocations (where, fixP, segment_address_in_file)
     char **where;
     fixS *fixP;		/* Fixup chain for this segment. */
     relax_addressT segment_address_in_file;
{
#ifdef BFD_HEADERS
  struct internal_reloc *ri_table;
#else
  RELOC *ri_table;
#endif
#ifdef TC_I960
  char *callj_table;
#endif
  symbolS *symbolP;
  int i, count;
  fixS *p;

  for (count = 0, p = fixP; p; p = p->fx_next)
    if (p->fx_addsy)
      count++;
  if (!count)
    return;

#ifdef BFD_HEADERS
  ri_table = (struct internal_reloc *) calloc (sizeof (*ri_table), count);
#else
  ri_table = (RELOC *) calloc (sizeof (*ri_table), count);
#endif
  if (!ri_table)
    as_fatal ("obj_emit_relocations: Could not malloc relocation table");

#ifdef TC_I960
  callj_table = (char *) malloc (sizeof (char) * count);
  if (!callj_table)
    as_fatal ("obj_emit_relocations: Could not malloc callj table");
#endif

  for (i = 0; fixP; fixP = fixP->fx_next)
    {
      if (symbolP = fixP->fx_addsy)
	{
	  int rtype_ok = 0;
#if defined(TC_M68K)
	  ri_table[i].r_type = (fixP->fx_pcrel ?
				(fixP->fx_size == 1 ? R_PCRBYTE :
				 fixP->fx_size == 2 ? R_PCRWORD :
				 R_PCRLONG) :
				(fixP->fx_size == 1 ? R_RELBYTE :
				 fixP->fx_size == 2 ? R_RELWORD :
				 R_RELLONG));
	  rtype_ok = 1;
#endif
#if defined(TC_I386)
	  /* FIXME-SOON R_OFF8 & R_DIR16 are a vague guess, completly
			   untested. */
	  ri_table[i].r_type = (fixP->fx_pcrel ?
				(fixP->fx_size == 1 ? R_PCRBYTE :
				 fixP->fx_size == 2 ? R_PCRWORD :
				 R_PCRLONG) :
				(fixP->fx_size == 1 ? R_OFF8 :
				 fixP->fx_size == 2 ? R_DIR16 :
				 R_DIR32));
	  rtype_ok = 1;
#endif
#if defined(TC_I960)
	  ri_table[i].r_type = (fixP->fx_pcrel
				? R_IPRMED
				: R_RELLONG);
	  callj_table[i] = fixP->fx_callj ? 1 : 0;
	  rtype_ok = 1;
#endif
#if defined(TC_A29K)
	  ri_table[i].r_type = tc_coff_fix2rtype (fixP);
	  rtype_ok = 1;
#endif
	  if (!rtype_ok)
	    abort ();
	  ri_table[i].r_vaddr = (fixP->fx_frag->fr_address
				 + fixP->fx_where);
	  /* If symbol associated to relocation entry is a bss symbol
	     or undefined symbol just remember the index of the symbol.
	     Otherwise store the index of the symbol describing the
	     section the symbol belong to. This heuristic speeds up ld.
	     */
	  /* Local symbols can generate relocation information. In case
	     of structure return for instance. But they have no symbol
	     number because they won't be emitted in the final object.
	     In the case where they are in the BSS section, this leads
	     to an incorrect r_symndx.
	     Under bsd the loader do not care if the symbol reference
	     is incorrect. But the SYS V ld complains about this. To
	     avoid this we associate the symbol to the associated
	     section, *even* if it is the BSS section. */
	  /* If someone can tell me why the other symbols of the bss
	     section are not associated with the .bss section entry,
	     I'd be gratefull. I guess that it has to do with the special
	     nature of the .bss section. Or maybe this is because the
	     bss symbols are declared in the common section and can
	     be resized later. Can it break code some where ? */
	  ri_table[i].r_symndx = (S_GET_SEGMENT (symbolP) == SEG_TEXT
				  ? dot_text_symbol->sy_number
				  : (S_GET_SEGMENT (symbolP) == SEG_DATA
				     ? dot_data_symbol->sy_number
				     : ((SF_GET_LOCAL (symbolP)
					 ? dot_bss_symbol->sy_number
					 : symbolP->sy_number))));	/* bss or undefined */

	  /* md_ri_to_chars((char *) &ri, ri); *//* Last step : write md f */

	  i++;
	}			/* if there's a symbol */
    }				/* for each fixP */

  /* AIX ld prefer to have the reloc table with r_vaddr sorted.
     But sorting it should not hurt any other ld.  */
  qsort (ri_table, count, sizeof (*ri_table), reloc_compare);

  for (i = 0; i < count; i++)
    {
#ifdef BFD_HEADERS
      *where += bfd_coff_swap_reloc_out (stdoutput, &ri_table[i], *where);
# ifdef TC_A29K
      /* The 29k has a special kludge for the high 16 bit reloc.
	 Two relocations are emmited, R_IHIHALF, and R_IHCONST.
	 The second one doesn't contain a symbol, but uses the
	 value for offset */
      if (ri_table[i].r_type == R_IHIHALF)
	{
	  /* now emit the second bit */
	  ri_table[i].r_type = R_IHCONST;
	  ri_table[i].r_symndx = fixP->fx_addnumber;
	  *where += bfd_coff_swap_reloc_out (stdoutput, &ri_table[i],
					     *where);
	}
# endif				/* TC_A29K */

#else /* not BFD_HEADERS */
      append (where, (char *) &ri_table[i], RELSZ);
#endif /* not BFD_HEADERS */

#ifdef TC_I960
      if (callj_table[i])
	{
	  ri_table[i].r_type = R_OPTCALL;
# ifdef BFD_HEADERS
	  *where += bfd_coff_swap_reloc_out (stdoutput, &ri_table[i],
					     *where);
# else
	  append (where, (char *) &ri_table[i], (unsigned long) RELSZ);
# endif				/* BFD_HEADERS */
	}			/* if it's a callj, do it again for the opcode */
#endif /* TC_I960 */
    }

  free (ri_table);
#ifdef TC_I960
  free (callj_table);
#endif

  return;
}				/* obj_emit_relocations() */

/* Coff file generation & utilities */

#ifdef BFD_HEADERS
void
obj_header_append (where, headers)
     char **where;
     object_headers *headers;
{
  tc_headers_hook (headers);
  *where += bfd_coff_swap_filehdr_out (stdoutput, &(headers->filehdr), *where);
#ifndef OBJ_COFF_OMIT_OPTIONAL_HEADER
  *where += bfd_coff_swap_aouthdr_out (stdoutput, &(headers->aouthdr), *where);
#endif
  obj_coff_section_header_append (where, &text_section_header);
  obj_coff_section_header_append (where, &data_section_header);
  obj_coff_section_header_append (where, &bss_section_header);
}

#else /* ! BFD_HEADERS */

void
obj_header_append (where, headers)
     char **where;
     object_headers *headers;
{
  tc_headers_hook (headers);

#ifdef CROSS_COMPILE
  /* Eventually swap bytes for cross compilation for file header */
  md_number_to_chars (*where, headers->filehdr.f_magic, sizeof (headers->filehdr.f_magic));
  *where += sizeof (headers->filehdr.f_magic);
  md_number_to_chars (*where, headers->filehdr.f_nscns, sizeof (headers->filehdr.f_nscns));
  *where += sizeof (headers->filehdr.f_nscns);
  md_number_to_chars (*where, headers->filehdr.f_timdat, sizeof (headers->filehdr.f_timdat));
  *where += sizeof (headers->filehdr.f_timdat);
  md_number_to_chars (*where, headers->filehdr.f_symptr, sizeof (headers->filehdr.f_symptr));
  *where += sizeof (headers->filehdr.f_symptr);
  md_number_to_chars (*where, headers->filehdr.f_nsyms, sizeof (headers->filehdr.f_nsyms));
  *where += sizeof (headers->filehdr.f_nsyms);
  md_number_to_chars (*where, headers->filehdr.f_opthdr, sizeof (headers->filehdr.f_opthdr));
  *where += sizeof (headers->filehdr.f_opthdr);
  md_number_to_chars (*where, headers->filehdr.f_flags, sizeof (headers->filehdr.f_flags));
  *where += sizeof (headers->filehdr.f_flags);

#ifndef OBJ_COFF_OMIT_OPTIONAL_HEADER
  /* Eventually swap bytes for cross compilation for a.out header */
  md_number_to_chars (*where, headers->aouthdr.magic, sizeof (headers->aouthdr.magic));
  *where += sizeof (headers->aouthdr.magic);
  md_number_to_chars (*where, headers->aouthdr.vstamp, sizeof (headers->aouthdr.vstamp));
  *where += sizeof (headers->aouthdr.vstamp);
  md_number_to_chars (*where, headers->aouthdr.tsize, sizeof (headers->aouthdr.tsize));
  *where += sizeof (headers->aouthdr.tsize);
  md_number_to_chars (*where, headers->aouthdr.dsize, sizeof (headers->aouthdr.dsize));
  *where += sizeof (headers->aouthdr.dsize);
  md_number_to_chars (*where, headers->aouthdr.bsize, sizeof (headers->aouthdr.bsize));
  *where += sizeof (headers->aouthdr.bsize);
  md_number_to_chars (*where, headers->aouthdr.entry, sizeof (headers->aouthdr.entry));
  *where += sizeof (headers->aouthdr.entry);
  md_number_to_chars (*where, headers->aouthdr.text_start, sizeof (headers->aouthdr.text_start));
  *where += sizeof (headers->aouthdr.text_start);
  md_number_to_chars (*where, headers->aouthdr.data_start, sizeof (headers->aouthdr.data_start));
  *where += sizeof (headers->aouthdr.data_start);
  md_number_to_chars (*where, headers->aouthdr.tagentries, sizeof (headers->aouthdr.tagentries));
  *where += sizeof (headers->aouthdr.tagentries);
#endif /* OBJ_COFF_OMIT_OPTIONAL_HEADER */

#else /* CROSS_COMPILE */

  append (where, (char *) &headers->filehdr, sizeof (headers->filehdr));
#ifndef OBJ_COFF_OMIT_OPTIONAL_HEADER
  append (where, (char *) &headers->aouthdr, sizeof (headers->aouthdr));
#endif /* OBJ_COFF_OMIT_OPTIONAL_HEADER */

#endif /* CROSS_COMPILE */

  /* Output the section headers */
  obj_coff_section_header_append (where, &text_section_header);
  obj_coff_section_header_append (where, &data_section_header);
  obj_coff_section_header_append (where, &bss_section_header);

  return;
}				/* obj_header_append() */

#endif /* ! BFD_HEADERS */

void
obj_symbol_to_chars (where, symbolP)
     char **where;
     symbolS *symbolP;
{
  /* Move the value into the COFF symbol itself.  */
  symbolP->sy_symbol.ost_entry.n_value = S_GET_VALUE (symbolP);

#ifdef BFD_HEADERS
  unsigned int numaux = symbolP->sy_symbol.ost_entry.n_numaux;
  unsigned int i;

  if (S_GET_SEGMENT (symbolP) == SEG_REGISTER)
    {
      S_SET_SEGMENT (symbolP, SEG_ABSOLUTE);
    }
  *where += bfd_coff_swap_sym_out (stdoutput, &symbolP->sy_symbol.ost_entry,
				   *where);

  for (i = 0; i < numaux; i++)
    {
      *where += bfd_coff_swap_aux_out (stdoutput,
				       &symbolP->sy_symbol.ost_auxent[i],
				       S_GET_DATA_TYPE (symbolP),
				       S_GET_STORAGE_CLASS (symbolP),
				       *where);
    }

#else /* BFD_HEADERS */
  SYMENT *syment = &symbolP->sy_symbol.ost_entry;
  int i;
  char numaux = syment->n_numaux;
  unsigned short type = S_GET_DATA_TYPE (symbolP);

#ifdef CROSS_COMPILE
  md_number_to_chars (*where, syment->n_value, sizeof (syment->n_value));
  *where += sizeof (syment->n_value);
  md_number_to_chars (*where, syment->n_scnum, sizeof (syment->n_scnum));
  *where += sizeof (syment->n_scnum);
  md_number_to_chars (*where, 0, sizeof (short));	/* pad n_flags */
  *where += sizeof (short);
  md_number_to_chars (*where, syment->n_type, sizeof (syment->n_type));
  *where += sizeof (syment->n_type);
  md_number_to_chars (*where, syment->n_sclass, sizeof (syment->n_sclass));
  *where += sizeof (syment->n_sclass);
  md_number_to_chars (*where, syment->n_numaux, sizeof (syment->n_numaux));
  *where += sizeof (syment->n_numaux);
#else /* CROSS_COMPILE */
  append (where, (char *) syment, sizeof (*syment));
#endif /* CROSS_COMPILE */

  /* Should do the following:
     if (.file entry) MD(..)... else if (static entry) MD(..) */
  if (numaux > OBJ_COFF_MAX_AUXENTRIES)
    {
      as_bad ("Internal error? too many auxents for symbol");
    }				/* too many auxents */

  for (i = 0; i < numaux; ++i)
    {
#ifdef CROSS_COMPILE
#if 0				/* This code has never been tested */
      /* The most common case, x_sym entry. */
      if ((SF_GET (symbolP) & (SF_FILE | SF_STATICS)) == 0)
	{
	  md_number_to_chars (*where, auxP->x_sym.x_tagndx, sizeof (auxP->x_sym.x_tagndx));
	  *where += sizeof (auxP->x_sym.x_tagndx);
	  if (ISFCN (type))
	    {
	      md_number_to_chars (*where, auxP->x_sym.x_misc.x_fsize, sizeof (auxP->x_sym.x_misc.x_fsize));
	      *where += sizeof (auxP->x_sym.x_misc.x_fsize);
	    }
	  else
	    {
	      md_number_to_chars (*where, auxP->x_sym.x_misc.x_lnno, sizeof (auxP->x_sym.x_misc.x_lnno));
	      *where += sizeof (auxP->x_sym.x_misc.x_lnno);
	      md_number_to_chars (*where, auxP->x_sym.x_misc.x_size, sizeof (auxP->x_sym.x_misc.x_size));
	      *where += sizeof (auxP->x_sym.x_misc.x_size);
	    }
	  if (ISARY (type))
	    {
	      register int index;
	      for (index = 0; index < DIMNUM; index++)
		md_number_to_chars (*where, auxP->x_sym.x_fcnary.x_ary.x_dimen[index], sizeof (auxP->x_sym.x_fcnary.x_ary.x_dimen[index]));
	      *where += sizeof (auxP->x_sym.x_fcnary.x_ary.x_dimen[index]);
	    }
	  else
	    {
	      md_number_to_chars (*where, auxP->x_sym.x_fcnary.x_fcn.x_lnnoptr, sizeof (auxP->x_sym.x_fcnary.x_fcn.x_lnnoptr));
	      *where += sizeof (auxP->x_sym.x_fcnary.x_fcn.x_lnnoptr);
	      md_number_to_chars (*where, auxP->x_sym.x_fcnary.x_fcn.x_endndx, sizeof (auxP->x_sym.x_fcnary.x_fcn.x_endndx));
	      *where += sizeof (auxP->x_sym.x_fcnary.x_fcn.x_endndx);
	    }
	  md_number_to_chars (*where, auxP->x_sym.x_tvndx, sizeof (auxP->x_sym.x_tvndx));
	  *where += sizeof (auxP->x_sym.x_tvndx);
	}
      else if (SF_GET_FILE (symbolP))
	{			/* .file */
	  ;
	}
      else if (SF_GET_STATICS (symbolP))
	{			/* .text, .data, .bss symbols */
	  md_number_to_chars (*where, auxP->x_scn.x_scnlen, sizeof (auxP->x_scn.x_scnlen));
	  *where += sizeof (auxP->x_scn.x_scnlen);
	  md_number_to_chars (*where, auxP->x_scn.x_nreloc, sizeof (auxP->x_scn.x_nreloc));
	  *where += sizeof (auxP->x_scn.x_nreloc);
	  md_number_to_chars (*where, auxP->x_scn.x_nlinno, sizeof (auxP->x_scn.x_nlinno));
	  *where += sizeof (auxP->x_scn.x_nlinno);
	}
#endif /* 0 */
#else /* CROSS_COMPILE */
      append (where, (char *) &symbolP->sy_symbol.ost_auxent[i], sizeof (symbolP->sy_symbol.ost_auxent[i]));
#endif /* CROSS_COMPILE */

    };				/* for each aux in use */
#endif /* BFD_HEADERS */
  return;
}				/* obj_symbol_to_chars() */

#ifdef BFD_HEADERS
static void
obj_coff_section_header_append (where, header)
     char **where;
     struct internal_scnhdr *header;
{
  *where += bfd_coff_swap_scnhdr_out (stdoutput, header, *where);
}

#else
static void
obj_coff_section_header_append (where, header)
     char **where;
     SCNHDR *header;
{
#ifdef CROSS_COMPILE
  memcpy (*where, header->s_name, sizeof (header->s_name));
  *where += sizeof (header->s_name);

  md_number_to_chars (*where, header->s_paddr, sizeof (header->s_paddr));
  *where += sizeof (header->s_paddr);

  md_number_to_chars (*where, header->s_vaddr, sizeof (header->s_vaddr));
  *where += sizeof (header->s_vaddr);

  md_number_to_chars (*where, header->s_size, sizeof (header->s_size));
  *where += sizeof (header->s_size);

  md_number_to_chars (*where, header->s_scnptr, sizeof (header->s_scnptr));
  *where += sizeof (header->s_scnptr);

  md_number_to_chars (*where, header->s_relptr, sizeof (header->s_relptr));
  *where += sizeof (header->s_relptr);

  md_number_to_chars (*where, header->s_lnnoptr, sizeof (header->s_lnnoptr));
  *where += sizeof (header->s_lnnoptr);

  md_number_to_chars (*where, header->s_nreloc, sizeof (header->s_nreloc));
  *where += sizeof (header->s_nreloc);

  md_number_to_chars (*where, header->s_nlnno, sizeof (header->s_nlnno));
  *where += sizeof (header->s_nlnno);

  md_number_to_chars (*where, header->s_flags, sizeof (header->s_flags));
  *where += sizeof (header->s_flags);

#ifdef TC_I960
  md_number_to_chars (*where, header->s_align, sizeof (header->s_align));
  *where += sizeof (header->s_align);
#endif /* TC_I960 */

#else /* CROSS_COMPILE */

  append (where, (char *) header, sizeof (*header));

#endif /* CROSS_COMPILE */

  return;
}				/* obj_coff_section_header_append() */

#endif
void
obj_emit_symbols (where, symbol_rootP)
     char **where;
     symbolS *symbol_rootP;
{
  symbolS *symbolP;
  /*
	 * Emit all symbols left in the symbol chain.
	 */
  for (symbolP = symbol_rootP; symbolP; symbolP = symbol_next (symbolP))
    {
      /* Used to save the offset of the name. It is used to point
		   to the string in memory but must be a file offset. */
      register char *temp;

      tc_coff_symbol_emit_hook (symbolP);

      temp = S_GET_NAME (symbolP);
      if (SF_GET_STRING (symbolP))
	{
	  S_SET_OFFSET (symbolP, symbolP->sy_name_offset);
	  S_SET_ZEROES (symbolP, 0);
	}
      else
	{
	  memset (symbolP->sy_symbol.ost_entry.n_name, '\0', SYMNMLEN);
	  strncpy (symbolP->sy_symbol.ost_entry.n_name, temp, SYMNMLEN);
	}
      obj_symbol_to_chars (where, symbolP);
      S_SET_NAME (symbolP, temp);
    }
}				/* obj_emit_symbols() */

#endif /* ! BFD_ASSEMBLER */

/* Merge a debug symbol containing debug information into a normal symbol. */

void
c_symbol_merge (debug, normal)
     symbolS *debug;
     symbolS *normal;
{
  S_SET_DATA_TYPE (normal, S_GET_DATA_TYPE (debug));
  S_SET_STORAGE_CLASS (normal, S_GET_STORAGE_CLASS (debug));

  if (S_GET_NUMBER_AUXILIARY (debug) > S_GET_NUMBER_AUXILIARY (normal))
    /* take the most we have */
    S_SET_NUMBER_AUXILIARY (normal, S_GET_NUMBER_AUXILIARY (debug));

  if (S_GET_NUMBER_AUXILIARY (debug) > 0)
    {
      /* Move all the auxiliary information.  */
#ifdef BFD_ASSEMBLER
      /* @@ How many fields do we want to preserve?  Would it make more
	 sense to pick and choose those we want to copy?  Should look
	 into this further....  [raeburn:19920512.2209EST]  */
      alent *linenos;
      linenos = coffsymbol (normal->bsym)->lineno;
      memcpy ((char *) &coffsymbol (normal->bsym)->native,
	      (char *) &coffsymbol (debug->bsym)->native,
	      S_GET_NUMBER_AUXILIARY(debug) * AUXESZ);
      coffsymbol (normal->bsym)->lineno = linenos;
#else
      memcpy ((char *) &normal->sy_symbol.ost_auxent[0],
	      (char *) &debug->sy_symbol.ost_auxent[0],
	      S_GET_NUMBER_AUXILIARY (debug) * AUXESZ);
#endif
    }

  /* Move the debug flags. */
  SF_SET_DEBUG_FIELD (normal, SF_GET_DEBUG_FIELD (debug));
}				/* c_symbol_merge() */

static symbolS *previous_file_symbol;
void
c_dot_file_symbol (filename)
     char *filename;
{
  symbolS *symbolP;

#ifdef BFD_ASSEMBLER
  symbolP = symbol_new (filename, &bfd_abs_section, 0,
			&zero_address_frag);
#else
  symbolP = symbol_new (".file",
			SEG_DEBUG,
			0,
			&zero_address_frag);
#endif

  S_SET_STORAGE_CLASS (symbolP, C_FILE);
  S_SET_NUMBER_AUXILIARY (symbolP, 1);

#ifdef BFD_ASSEMBLER
  symbolP->bsym->flags = BSF_DEBUGGING;
#else
  if (strlen(filename) > 14) 
    {
      /* This won't fit into a 14 char space, it will go into the string
	 table.  */
      symbolP->sy_symbol.ost_auxent->x_file.x_n.x_zeroes = 0;
      (&(symbolP->sy_symbol.ost_auxent->x_file.x_n.x_offset))[0] = string_byte_count;
      (&(symbolP->sy_symbol.ost_auxent->x_file.x_n.x_offset))[1] = (int)filename;
    }
  else 
    {
      SA_SET_FILE_FNAME (symbolP, filename);
    }
  SF_SET_DEBUG (symbolP);
#endif

#ifndef NO_LISTING
  {
    extern int listing;
    if (listing)
      {
	listing_source_file (filename);
      }
  }
#endif

  S_SET_VALUE (symbolP, (long) previous_file_symbol);

  previous_file_symbol = symbolP;

  /* Make sure that the symbol is first on the symbol chain */
  if (symbol_rootP != symbolP)
    {
      if (symbolP == symbol_lastP)
	{
	  symbol_lastP = symbol_lastP->sy_previous;
	}			/* if it was the last thing on the list */

      symbol_remove (symbolP, &symbol_rootP, &symbol_lastP);
      symbol_insert (symbolP, symbol_rootP, &symbol_rootP, &symbol_lastP);
      symbol_rootP = symbolP;
    }				/* if not first on the list */
}

/*
 * Build a 'section static' symbol.
 */

char *
c_section_symbol (name, value, length, nreloc, nlnno)
     char *name;
     long value;
     long length;
     unsigned short nreloc;
     unsigned short nlnno;
{
  symbolS *symbolP;

  symbolP = symbol_new (name,
			(name[1] == 't'
			 ? text_section
			 : name[1] == 'd'
			 ? data_section
			 : bss_section),
			value,
			&zero_address_frag);

  S_SET_STORAGE_CLASS (symbolP, C_STAT);
  S_SET_NUMBER_AUXILIARY (symbolP, 1);

  SA_SET_SCN_SCNLEN (symbolP, length);
  SA_SET_SCN_NRELOC (symbolP, nreloc);
  SA_SET_SCN_NLINNO (symbolP, nlnno);

  SF_SET_STATICS (symbolP);

  return (char *) symbolP;
}				/* c_section_symbol() */

void
c_section_header (header,
		  name,
		  core_address,
		  size,
		  data_ptr,
		  reloc_ptr,
		  lineno_ptr,
		  reloc_number,
		  lineno_number,
		  alignment)
#ifdef BFD_HEADERS
     struct internal_scnhdr *header;
#else
     SCNHDR *header;
#endif
     char *name;
     long core_address;
     long size;
     long data_ptr;
     long reloc_ptr;
     long lineno_ptr;
     long reloc_number;
     long lineno_number;
     long alignment;
{
  strncpy (header->s_name, name, 8);
  header->s_paddr = header->s_vaddr = core_address;
  header->s_scnptr = ((header->s_size = size) != 0) ? data_ptr : 0;
  header->s_relptr = reloc_ptr;
  header->s_lnnoptr = lineno_ptr;
  header->s_nreloc = reloc_number;
  header->s_nlnno = lineno_number;

#ifdef OBJ_COFF_SECTION_HEADER_HAS_ALIGNMENT
#ifdef OBJ_COFF_BROKEN_ALIGNMENT
  header->s_align = ((name[1] == 'b' || (size > 0)) ? 16 : 0);
#else
  header->s_align = ((alignment == 0)
		     ? 0
		     : (1 << alignment));
#endif /* OBJ_COFF_BROKEN_ALIGNMENT */
#endif /* OBJ_COFF_SECTION_HEADER_HAS_ALIGNMENT */

  header->s_flags = STYP_REG | (name[1] == 't'
				? STYP_TEXT
				: name[1] == 'd'
				? STYP_DATA
				: name[1] == 'b'
				? STYP_BSS
				: STYP_INFO);
}

/* Line number handling */

#ifdef BFD_ASSEMBLER

/* Symbol of last function, which we should hang line#s off of.  */
symbolS *function_lineoff;

#else

/* Offset in line#s where the last function started (the odd entry for
   line #0). */
int function_lineoff = -1;

int text_lineno_number;

/* We use this to build pointers from .bf's into the linetable.  It
   should match exactly the values that are later assigned in
   text_lineno_number by write.c. */
int our_lineno_number;

lineno *lineno_lastP;
#endif

#ifndef BFD_ASSEMBLER
int
c_line_new (paddr, line_number, frag)
     long paddr;
     unsigned short line_number;
     fragS *frag;
{
  lineno *new_line = (lineno *) xmalloc (sizeof (lineno));

  new_line->line.l_addr.l_paddr = paddr;
  new_line->line.l_lnno = line_number;
  new_line->frag = (char *) frag;
  new_line->next = (lineno *) 0;

  if (lineno_rootP == (lineno *) 0)
    lineno_rootP = new_line;
  else
    lineno_lastP->next = new_line;
  lineno_lastP = new_line;
  return LINESZ * our_lineno_number++;
}
#endif

void
obj_emit_lineno (where, line, file_start)
     char **where;
#ifndef BFD_ASSEMBLER /* sigh */
     lineno *line;
#endif
     char *file_start;
{
#ifndef BFD_ASSEMBLER
#ifdef BFD_HEADERS
  struct bfd_internal_lineno *line_entry;
#else
  LINENO *line_entry;
#endif
  for (; line; line = line->next)
    {
      line_entry = &line->line;

      /* FIXME-SOMEDAY Resolving the sy_number of function linno's used to be
	 done in write_object_file() but their symbols need a fileptr to the
	 lnno, so I moved this resolution check here.  xoxorich. */

      if (line_entry->l_lnno == 0)
	{
	  /* There is a good chance that the symbol pointed to
	     is not the one that will be emitted and that the
	     sy_number is not accurate. */
	  symbolS *symbolP;

	  symbolP = (symbolS *) line_entry->l_addr.l_symndx;

	  line_entry->l_addr.l_symndx = symbolP->sy_number;
	  symbolP->sy_symbol.ost_auxent[0].x_sym.x_fcnary.x_fcn.x_lnnoptr = *where - file_start;

	}			/* if this is a function linno */
#ifdef BFD_HEADERS
      *where += bfd_coff_swap_lineno_out (stdoutput, line_entry, *where);
#else
      /* No matter which member of the union we process, they are
	 both long. */
#ifdef CROSS_COMPILE
      md_number_to_chars (*where, line_entry->l_addr.l_paddr, sizeof (line_entry->l_addr.l_paddr));
      *where += sizeof (line_entry->l_addr.l_paddr);

      md_number_to_chars (*where, line_entry->l_lnno, sizeof (line_entry->l_lnno));
      *where += sizeof (line_entry->l_lnno);

#ifdef TC_I960
      **where = '0';
      ++*where;
      **where = '0';
      ++*where;
#endif /* TC_I960 */

#else /* CROSS_COMPILE */
      append (where, (char *) line_entry, LINESZ);
#endif /* CROSS_COMPILE */
#endif /* BFD_HEADERS */
    }				/* for each line number */
#else /* BFD_ASSEMBLER */
  abort ();
#endif /* BFD_ASSEMBLER */
}

void
obj_symbol_new_hook (symbolP)
     symbolS *symbolP;
{
  char underscore = 0;		/* Symbol has leading _ */

#ifdef BFD_ASSEMBLER
  {
    long sz = (OBJ_COFF_MAX_AUXENTRIES + 1) * sizeof (combined_entry_type);
    char *s = (char *) bfd_alloc_by_size_t (stdoutput, sz);
    memset (s, 0, sz);
    coffsymbol (symbolP->bsym)->native = (combined_entry_type *) s;
  }
#else
  /* Effective symbol */
  /* Store the pointer in the offset. */
  S_SET_ZEROES (symbolP, 0L);
  /* Additional information */
  symbolP->sy_symbol.ost_flags = 0;
  /* Auxiliary entries */
  memset ((char *) &symbolP->sy_symbol.ost_auxent[0], '\0', AUXESZ);
#endif
  S_SET_DATA_TYPE (symbolP, T_NULL);
  S_SET_STORAGE_CLASS (symbolP, 0);
  S_SET_NUMBER_AUXILIARY (symbolP, 0);

#ifdef STRIP_UNDERSCORE
  /* Remove leading underscore at the beginning of the symbol.
     This is to be compatible with the standard librairies.  */
  if (*S_GET_NAME (symbolP) == '_')
    {
      underscore = 1;
      S_SET_NAME (symbolP, S_GET_NAME (symbolP) + 1);
    }
#endif /* STRIP_UNDERSCORE */

  if (S_IS_STRING (symbolP))
    SF_SET_STRING (symbolP);
  if (!underscore && S_IS_LOCAL (symbolP))
    SF_SET_LOCAL (symbolP);
}

/* stack stuff */
stack *
stack_init (chunk_size, element_size)
     unsigned long chunk_size;
     unsigned long element_size;
{
  stack *st;

  st = (stack *) malloc (sizeof (stack));
  if (!st)
    return 0;
  st->data = malloc (chunk_size);
  if (!st->data)
    {
      free (st);
      return 0;
    }
  st->pointer = 0;
  st->size = chunk_size;
  st->chunk_size = chunk_size;
  st->element_size = element_size;
  return st;
}

void
stack_delete (st)
     stack *st;
{
  free (st->data);
  free (st);
}

char *
stack_push (st, element)
     stack *st;
     char *element;
{
  if (st->pointer + st->element_size >= st->size)
    {
      st->size += st->chunk_size;
      if ((st->data = xrealloc (st->data, st->size)) == (char *) 0)
	return (char *) 0;
    }
  memcpy (st->data + st->pointer, element, st->element_size);
  st->pointer += st->element_size;
  return st->data + st->pointer;
}				/* stack_push() */

char *
stack_pop (st)
     stack *st;
{
  if ((st->pointer -= st->element_size) < 0)
    {
      st->pointer = 0;
      return (char *) 0;
    }
  return st->data + st->pointer;
}

char *
stack_top (st)
     stack *st;
{
  return st->data + st->pointer - st->element_size;
}


/*
 * Handle .ln directives.
 */

#ifdef BFD_ASSEMBLER
static symbolS *current_lineno_sym;
static struct line_no *line_nos;

static void
add_lineno (frag, offset, num)
     fragS *frag;
     int offset;
     int num;
{
  struct line_no *new_line = (struct line_no *) bfd_alloc_by_size_t (stdoutput,
								     sizeof (struct line_no));
  if (!current_lineno_sym)
    {
      abort ();
    }
  new_line->next = line_nos;
  new_line->frag = frag;
  new_line->l.line_number = num;
  new_line->l.u.offset = offset;
  line_nos = new_line;
}

static void
add_linesym (sym)
     symbolS *sym;
{
  if (line_nos)
    {
      add_lineno (0, 0, 0);
      coffsymbol (current_lineno_sym->bsym)->lineno = (alent *) line_nos;
      line_nos = 0;
    }
  current_lineno_sym = sym;
}
#endif

static void
obj_coff_ln (appline)
     int appline;
{
  int l;

  if (! appline && def_symbol_in_progress != NULL)
    {
      as_warn (".ln pseudo-op inside .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }

  l = get_absolute_expression ();
#ifdef BFD_ASSEMBLER
  add_lineno (frag_now, frag_now_fix (), l);
#else
  c_line_new (frag_now_fix (), l, frag_now);
#endif

#ifndef NO_LISTING
  {
    extern int listing;

    if (listing)
      {
	if (! appline)
	  l += line_base - 1;
	listing_source_line (l);
      }
  }
#endif

  demand_empty_rest_of_line ();
}				/* obj_coff_ln() */

/*
 *			def()
 *
 * Handle .def directives.
 *
 * One might ask : why can't we symbol_new if the symbol does not
 * already exist and fill it with debug information.  Because of
 * the C_EFCN special symbol. It would clobber the value of the
 * function symbol before we have a chance to notice that it is
 * a C_EFCN. And a second reason is that the code is more clear this
 * way. (at least I think it is :-).
 *
 */

#define SKIP_SEMI_COLON()	while (*input_line_pointer++ != ';')
#define SKIP_WHITESPACES()	while (*input_line_pointer == ' ' || \
				       *input_line_pointer == '\t') \
    input_line_pointer++;

static void
obj_coff_def (what)
     int what;
{
  char name_end;		/* Char after the end of name */
  char *symbol_name;		/* Name of the debug symbol */
  char *symbol_name_copy;	/* Temporary copy of the name */
  unsigned int symbol_name_length;

  if (def_symbol_in_progress != NULL)
    {
      as_warn (".def pseudo-op used inside of .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  SKIP_WHITESPACES ();

  symbol_name = input_line_pointer;
#ifdef STRIP_UNDERSCORE
  if (symbol_name[0] == '_' && symbol_name[1] != 0)
    symbol_name++;
#endif /* STRIP_UNDERSCORE */

  name_end = get_symbol_end ();
  symbol_name_length = strlen (symbol_name);
  symbol_name_copy = xmalloc (symbol_name_length + 1);
  strcpy (symbol_name_copy, symbol_name);

  /* Initialize the new symbol */
#ifdef BFD_ASSEMBLER
  def_symbol_in_progress = symbol_make (symbol_name_copy);
#else
  def_symbol_in_progress = (symbolS *) obstack_alloc (&notes, sizeof (*def_symbol_in_progress));
  memset (def_symbol_in_progress, '\0', sizeof (*def_symbol_in_progress));

  S_SET_NAME (def_symbol_in_progress, symbol_name_copy);
  def_symbol_in_progress->sy_name_offset = ~0;
  def_symbol_in_progress->sy_number = ~0;
#endif

  def_symbol_in_progress->sy_frag = &zero_address_frag;
  S_SET_VALUE (def_symbol_in_progress, 0);

  if (S_IS_STRING (def_symbol_in_progress))
    SF_SET_STRING (def_symbol_in_progress);

  *input_line_pointer = name_end;

  demand_empty_rest_of_line ();
}

unsigned int dim_index;
static void
obj_coff_endef ()
{
  symbolS *symbolP;
  /* DIM BUG FIX sac@cygnus.com */
  dim_index = 0;
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".endef pseudo-op used outside of .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  /* Set the section number according to storage class. */
  switch (S_GET_STORAGE_CLASS (def_symbol_in_progress))
    {
    case C_STRTAG:
    case C_ENTAG:
    case C_UNTAG:
      SF_SET_TAG (def_symbol_in_progress);
      /* intentional fallthrough */
    case C_FILE:
    case C_TPDEF:
      SF_SET_DEBUG (def_symbol_in_progress);
#ifdef BFD_ASSEMBLER
      S_SET_SEGMENT (def_symbol_in_progress, fetch_coff_debug_section ());
#else
      S_SET_SEGMENT (def_symbol_in_progress, SEG_DEBUG);
#endif
      break;

    case C_EFCN:
      SF_SET_LOCAL (def_symbol_in_progress);	/* Do not emit this symbol. */
      /* intentional fallthrough */
    case C_BLOCK:
      SF_SET_PROCESS (def_symbol_in_progress);	/* Will need processing before writing */
      /* intentional fallthrough */
    case C_FCN:
      {
	CONST char *name;
	S_SET_SEGMENT (def_symbol_in_progress, text_section);

#ifdef BFD_ASSEMBLER
	name = bfd_asymbol_name (def_symbol_in_progress->bsym);
#else
	name = def_symbol_in_progress->sy_symbol.ost_entry._n._n_nptr[1];
#endif
	if (name[1] == 'b' && name[2] == 'f')
	  {
	    if (function_lineoff < 0)
	      as_warn ("`%s' symbol without preceding function", name);
#ifdef BFD_ASSEMBLER
	    abort ();
#else
	    SA_GET_SYM_LNNOPTR (def_symbol_in_progress) = function_lineoff;
#endif
	    /* Will need relocating */
	    SF_SET_PROCESS (def_symbol_in_progress);
#ifdef BFD_ASSEMBLER
	    function_lineoff = 0;
#else
	    function_lineoff = -1;
#endif
	  }
      }
      break;

#ifdef C_AUTOARG
    case C_AUTOARG:
#endif /* C_AUTOARG */
    case C_AUTO:
    case C_REG:
    case C_MOS:
    case C_MOE:
    case C_MOU:
    case C_ARG:
    case C_REGPARM:
    case C_FIELD:
    case C_EOS:
      SF_SET_DEBUG (def_symbol_in_progress);
      S_SET_SEGMENT (def_symbol_in_progress, absolute_section);
      break;

    case C_EXT:
    case C_STAT:
    case C_LABEL:
      /* Valid but set somewhere else (s_comm, s_lcomm, colon) */
      break;

    case C_USTATIC:
    case C_EXTDEF:
    case C_ULABEL:
      as_warn ("unexpected storage class %d",
	       S_GET_STORAGE_CLASS (def_symbol_in_progress));
      break;
    }				/* switch on storage class */

  /* Now that we have built a debug symbol, try to find if we should
     merge with an existing symbol or not.  If a symbol is C_EFCN or
     SEG_ABSOLUTE or untagged SEG_DEBUG it never merges. */

  /* Two cases for functions.  Either debug followed by definition or
     definition followed by debug.  For definition first, we will
     merge the debug symbol into the definition.  For debug first, the
     lineno entry MUST point to the definition function or else it
     will point off into space when obj_crawl_symbol_chain() merges
     the debug symbol into the real symbol.  Therefor, let's presume
     the debug symbol is a real function reference. */

  /* FIXME-SOON If for some reason the definition label/symbol is
     never seen, this will probably leave an undefined symbol at link
     time. */

  if (S_GET_STORAGE_CLASS (def_symbol_in_progress) == C_EFCN
#ifdef BFD_ASSEMBLER
      || (!strcmp (bfd_get_section_name (stdoutput,
					 S_GET_SEGMENT (def_symbol_in_progress)),
		   "*DEBUG*")
	  && !SF_GET_TAG (def_symbol_in_progress))
#else
      || (S_GET_SEGMENT (def_symbol_in_progress) == SEG_DEBUG
	  && !SF_GET_TAG (def_symbol_in_progress))
#endif
      || S_GET_SEGMENT (def_symbol_in_progress) == absolute_section
      || (symbolP = symbol_find_base (S_GET_NAME (def_symbol_in_progress), DO_NOT_STRIP)) == NULL)
    {
#ifdef BFD_ASSEMBLER
      if (def_symbol_in_progress != symbol_lastP)
	symbol_append (def_symbol_in_progress, symbol_lastP, &symbol_rootP,
		       &symbol_lastP);
#else
      symbol_append (def_symbol_in_progress, symbol_lastP, &symbol_rootP,
		     &symbol_lastP);
#endif
    }
  else
    {
      /* This symbol already exists, merge the newly created symbol
	 into the old one.  This is not mandatory. The linker can
	 handle duplicate symbols correctly. But I guess that it save
	 a *lot* of space if the assembly file defines a lot of
	 symbols. [loic] */

      /* The debug entry (def_symbol_in_progress) is merged into the
	 previous definition. */

      c_symbol_merge (def_symbol_in_progress, symbolP);
      /* FIXME-SOON Should *def_symbol_in_progress be free'd? xoxorich. */
      def_symbol_in_progress = symbolP;

      if (SF_GET_FUNCTION (def_symbol_in_progress)
	  || SF_GET_TAG (def_symbol_in_progress))
	{
	  /* For functions, and tags, the symbol *must* be where the
	     debug symbol appears.  Move the existing symbol to the
	     current place. */
	  /* If it already is at the end of the symbol list, do nothing */
	  if (def_symbol_in_progress != symbol_lastP)
	    {
	      symbol_remove (def_symbol_in_progress, &symbol_rootP, &symbol_lastP);
	      symbol_append (def_symbol_in_progress, symbol_lastP, &symbol_rootP, &symbol_lastP);
	    }
	}
    }

  if (SF_GET_TAG (def_symbol_in_progress)
      && symbol_find_base (S_GET_NAME (def_symbol_in_progress), DO_NOT_STRIP) == NULL)
    {
      tag_insert (S_GET_NAME (def_symbol_in_progress), def_symbol_in_progress);
    }

  if (SF_GET_FUNCTION (def_symbol_in_progress))
    {
      know (sizeof (def_symbol_in_progress) <= sizeof (long));
#ifdef BFD_ASSEMBLER
      function_lineoff = def_symbol_in_progress;
      add_linesym (def_symbol_in_progress);
#else
      function_lineoff = c_line_new ((long) def_symbol_in_progress, 0,
				     &zero_address_frag);
#endif
      SF_SET_PROCESS (def_symbol_in_progress);

      if (symbolP == NULL)
	{
	  /* That is, if this is the first time we've seen the
	     function... */
	  symbol_table_insert (def_symbol_in_progress);
	}			/* definition follows debug */
    }				/* Create the line number entry pointing to the function being defined */

  def_symbol_in_progress = NULL;
  demand_empty_rest_of_line ();
  return;
}

static void
obj_coff_dim ()
{
  register int dim_index;

  if (def_symbol_in_progress == NULL)
    {
      as_warn (".dim pseudo-op used outside of .def/.endef: ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);

  for (dim_index = 0; dim_index < DIMNUM; dim_index++)
    {
      SKIP_WHITESPACES ();
      SA_SET_SYM_DIMEN (def_symbol_in_progress, dim_index, get_absolute_expression ());

      switch (*input_line_pointer)
	{

	case ',':
	  input_line_pointer++;
	  break;

	default:
	  as_warn ("badly formed .dim directive ignored");
	  /* intentional fallthrough */
	case '\n':
	case ';':
	  dim_index = DIMNUM;
	  break;
	}			/* switch on following character */
    }				/* for each dimension */

  demand_empty_rest_of_line ();
  return;
}				/* obj_coff_dim() */

static void
obj_coff_line ()
{
  int this_base;

  if (def_symbol_in_progress == NULL)
    {
      obj_coff_ln (0);
      return;
    }				/* if it looks like a stabs style line */

  this_base = get_absolute_expression ();
  if (this_base > line_base)
    {
      line_base = this_base;
    }


  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);
  SA_SET_SYM_LNNO (def_symbol_in_progress, line_base);

  demand_empty_rest_of_line ();
  return;
}				/* obj_coff_line() */

static void
obj_coff_size ()
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".size pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);
  SA_SET_SYM_SIZE (def_symbol_in_progress, get_absolute_expression ());
  demand_empty_rest_of_line ();
  return;
}				/* obj_coff_size() */

static void
obj_coff_scl ()
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".scl pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_STORAGE_CLASS (def_symbol_in_progress, get_absolute_expression ());
  demand_empty_rest_of_line ();
  return;
}				/* obj_coff_scl() */

static void
obj_coff_tag ()
{
  char *symbol_name;
  char name_end;

  if (def_symbol_in_progress == NULL)
    {
      as_warn (".tag pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_NUMBER_AUXILIARY (def_symbol_in_progress, 1);
  symbol_name = input_line_pointer;
  name_end = get_symbol_end ();

  /* Assume that the symbol referred to by .tag is always defined. */
  /* This was a bad assumption.  I've added find_or_make. xoxorich. */
  SA_SET_SYM_TAGNDX (def_symbol_in_progress,
		     (long) tag_find_or_make (symbol_name));
  if (SA_GET_SYM_TAGNDX (def_symbol_in_progress) == 0L)
    {
      as_warn ("tag not found for .tag %s", symbol_name);
    }				/* not defined */

  SF_SET_TAGGED (def_symbol_in_progress);
  *input_line_pointer = name_end;

  demand_empty_rest_of_line ();
}				/* obj_coff_tag() */

static void
obj_coff_type ()
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".type pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  S_SET_DATA_TYPE (def_symbol_in_progress, get_absolute_expression ());

  if (ISFCN (S_GET_DATA_TYPE (def_symbol_in_progress)) &&
      S_GET_STORAGE_CLASS (def_symbol_in_progress) != C_TPDEF)
    {
      SF_SET_FUNCTION (def_symbol_in_progress);
    }				/* is a function */

  demand_empty_rest_of_line ();
  return;
}				/* obj_coff_type() */

static void
obj_coff_val ()
{
  if (def_symbol_in_progress == NULL)
    {
      as_warn (".val pseudo-op used outside of .def/.endef ignored.");
      demand_empty_rest_of_line ();
      return;
    }				/* if not inside .def/.endef */

  if (is_name_beginner (*input_line_pointer))
    {
      char *symbol_name = input_line_pointer;
      char name_end = get_symbol_end ();

      if (!strcmp (symbol_name, "."))
	{
	  def_symbol_in_progress->sy_frag = frag_now;
	  S_SET_VALUE (def_symbol_in_progress, obstack_next_free (&frags) - frag_now->fr_literal);
	  /* If the .val is != from the .def (e.g. statics) */
	}
      else if (strcmp (S_GET_NAME (def_symbol_in_progress), symbol_name))
	{
	  def_symbol_in_progress->sy_value.X_op = O_symbol;
	  def_symbol_in_progress->sy_value.X_add_symbol =
	    symbol_find_or_make (symbol_name);
	  def_symbol_in_progress->sy_value.X_op_symbol = NULL;
	  def_symbol_in_progress->sy_value.X_add_number = 0;

	  /* If the segment is undefined when the forward reference is
	     resolved, then copy the segment id from the forward
	     symbol.  */
	  SF_SET_GET_SEGMENT (def_symbol_in_progress);
	}
      /* Otherwise, it is the name of a non debug symbol and its value will be calculated later. */
      *input_line_pointer = name_end;
    }
  else
    {
      S_SET_VALUE (def_symbol_in_progress, get_absolute_expression ());
    }				/* if symbol based */

  demand_empty_rest_of_line ();
  return;
}				/* obj_coff_val() */

/*
 * Maintain a list of the tagnames of the structres.
 */

static void
tag_init ()
{
  tag_hash = hash_new ();
  return;
}				/* tag_init() */

static void
tag_insert (name, symbolP)
     CONST char *name;
     symbolS *symbolP;
{
  register char *error_string;

  if ((error_string = hash_jam (tag_hash, name, (char *) symbolP)))
    {
      as_fatal ("Inserting \"%s\" into structure table failed: %s",
		name, error_string);
    }
  return;
}				/* tag_insert() */

static symbolS *
tag_find_or_make (name)
     char *name;
{
  symbolS *symbolP;

  if ((symbolP = tag_find (name)) == NULL)
    {
      symbolP = symbol_new (name, undefined_section,
			    0, &zero_address_frag);

      tag_insert (S_GET_NAME (symbolP), symbolP);
      symbol_table_insert (symbolP);
    }				/* not found */

  return (symbolP);
}				/* tag_find_or_make() */

static symbolS *
tag_find (name)
     char *name;
{
#ifdef STRIP_UNDERSCORE
  if (*name == '_')
    name++;
#endif /* STRIP_UNDERSCORE */
  return ((symbolS *) hash_find (tag_hash, name));
}				/* tag_find() */

void
obj_read_begin_hook ()
{
  /* These had better be the same.  Usually 18 bytes. */
#ifndef BFD_HEADERS
  know (sizeof (SYMENT) == sizeof (AUXENT));
  know (SYMESZ == AUXESZ);
#endif
  tag_init ();
}

void
obj_crawl_symbol_chain (headers)
     object_headers *headers;
{
  int symbol_number = 0;
  lineno *lineP;
  symbolS *last_functionP = NULL;
  symbolS *last_tagP;
  symbolS *symbolP;
  symbolS *symbol_externP = NULL;
  symbolS *symbol_extern_lastP = NULL;

  /* Initialize the stack used to keep track of the matching .bb .be */
  stack *block_stack = stack_init (512, sizeof (symbolS *));

  tc_crawl_symbol_chain (headers);

  /* The symbol list should be ordered according to the following sequence
   * order :
   * . .file symbol
   * . debug entries for functions
   * . fake symbols for .text .data and .bss
   * . defined symbols
   * . undefined symbols
   * But this is not mandatory. The only important point is to put the
   * undefined symbols at the end of the list.
   */

  if (symbol_rootP == NULL
      || S_GET_STORAGE_CLASS (symbol_rootP) != C_FILE)
    {
      know (!previous_file_symbol);
      c_dot_file_symbol ("fake");
    }				/* Is there a .file symbol ? If not insert one at the beginning. */

  /*
   * Build up static symbols for .text, .data and .bss
   */
  dot_text_symbol = (symbolS *)
    c_section_symbol (".text",
		      0,
		      H_GET_TEXT_SIZE (headers),
		      0 /*text_relocation_number */ ,
		      0 /*text_lineno_number */ );
#ifdef TE_I386AIX
  symbol_remove (dot_text_symbol, &symbol_rootP, &symbol_lastP);
  symbol_append (dot_text_symbol, previous_file_symbol,
		 &symbol_rootP, &symbol_lastP);
#endif /* TE_I386AIX */

  dot_data_symbol = (symbolS *)
    c_section_symbol (".data",
		      H_GET_TEXT_SIZE (headers),
		      H_GET_DATA_SIZE (headers),
		      0 /*data_relocation_number */ ,
		      0);	/* There are no data lineno entries */
#ifdef TE_I386AIX
  symbol_remove (dot_data_symbol, &symbol_rootP, &symbol_lastP);
  symbol_append (dot_data_symbol, dot_text_symbol,
		 &symbol_rootP, &symbol_lastP);
#endif /* TE_I386AIX */

  dot_bss_symbol = (symbolS *)
    c_section_symbol (".bss",
		      H_GET_TEXT_SIZE (headers) + H_GET_DATA_SIZE (headers),
		      H_GET_BSS_SIZE (headers),
		      0,	/* No relocation for a bss section. */
		      0);	/* There are no bss lineno entries */
#ifdef TE_I386AIX
  symbol_remove (dot_bss_symbol, &symbol_rootP, &symbol_lastP);
  symbol_append (dot_bss_symbol, dot_data_symbol,
		 &symbol_rootP, &symbol_lastP);
#endif /* TE_I386AIX */

#if defined(DEBUG)
  verify_symbol_chain (symbol_rootP, symbol_lastP);
#endif /* DEBUG */

  /* Three traversals of symbol chains here.  The
     first traversal yanks externals into a temporary
     chain, removing the externals from the global
     chain, numbers symbols, and does some other guck.
     The second traversal is on the temporary chain of
     externals and just appends them to the global
     chain again, numbering them as we go.  The third
     traversal patches pointers to symbols (using sym
     indexes).  The last traversal was once done as
     part of the first pass, but that fails when a
     reference preceeds a definition as the definition
     has no number at the time we process the
     reference. */

  /* Note that symbolP will be NULL at the end of a loop
     if an external was at the beginning of the list (it
     gets moved off the list).  Hence the weird check in
     the loop control.
     */
  for (symbolP = symbol_rootP;
       symbolP;
       symbolP = symbolP ? symbol_next (symbolP) : symbol_rootP)
    {
      if (!SF_GET_DEBUG (symbolP))
	{
	  /* Debug symbols do not need all this rubbish */
	  symbolS *real_symbolP;

	  /* L* and C_EFCN symbols never merge. */
	  if (!SF_GET_LOCAL (symbolP)
	      && (real_symbolP = symbol_find_base (S_GET_NAME (symbolP), DO_NOT_STRIP))
	      && real_symbolP != symbolP)
	    {
	      /* FIXME-SOON: where do dups come from?  Maybe tag references before definitions? xoxorich. */
	      /* Move the debug data from the debug symbol to the
	   real symbol. Do NOT do the oposite (i.e. move from
	   real symbol to debug symbol and remove real symbol from the
	   list.) Because some pointers refer to the real symbol
	   whereas no pointers refer to the debug symbol. */
	      c_symbol_merge (symbolP, real_symbolP);
	      /* Replace the current symbol by the real one */
	      /* The symbols will never be the last or the first
	   because : 1st symbol is .file and 3 last symbols are
	   .text, .data, .bss */
	      symbol_remove (real_symbolP, &symbol_rootP, &symbol_lastP);
	      symbol_insert (real_symbolP, symbolP, &symbol_rootP, &symbol_lastP);
	      symbol_remove (symbolP, &symbol_rootP, &symbol_lastP);
	      symbolP = real_symbolP;
	    }			/* if not local but dup'd */

	  if (flagseen['R'] && (S_GET_SEGMENT (symbolP) == SEG_DATA))
	    {
	      S_SET_SEGMENT (symbolP, SEG_TEXT);
	    }			/* push data into text */

	  resolve_symbol_value (symbolP);

	  if (!S_IS_DEFINED (symbolP) && !SF_GET_LOCAL (symbolP))
	    {
	      S_SET_EXTERNAL (symbolP);
	    }
	  else if (S_GET_STORAGE_CLASS (symbolP) == C_NULL)
	    {
	      if (S_GET_SEGMENT (symbolP) == SEG_TEXT)
		{
		  S_SET_STORAGE_CLASS (symbolP, C_LABEL);
		}
	      else
		{
		  S_SET_STORAGE_CLASS (symbolP, C_STAT);
		}
	    }			/* no storage class yet */

	  /* Mainly to speed up if not -g */
	  if (SF_GET_PROCESS (symbolP))
	    {
	      /* Handle the nested blocks auxiliary info. */
	      if (S_GET_STORAGE_CLASS (symbolP) == C_BLOCK)
		{
		  if (!strcmp (S_GET_NAME (symbolP), ".bb"))
		    stack_push (block_stack, (char *) &symbolP);
		  else
		    {		/* .eb */
		      register symbolS *begin_symbolP;
		      begin_symbolP = *(symbolS **) stack_pop (block_stack);
		      if (begin_symbolP == (symbolS *) 0)
			as_warn ("mismatched .eb");
		      else
			SA_SET_SYM_ENDNDX (begin_symbolP, symbol_number + 2);
		    }
		}
	      /* If we are able to identify the type of a function, and we
	   are out of a function (last_functionP == 0) then, the
	   function symbol will be associated with an auxiliary
	   entry. */
	      if (last_functionP == (symbolS *) 0 &&
		  SF_GET_FUNCTION (symbolP))
		{
		  last_functionP = symbolP;

		  if (S_GET_NUMBER_AUXILIARY (symbolP) < 1)
		    {
		      S_SET_NUMBER_AUXILIARY (symbolP, 1);
		    }		/* make it at least 1 */

		  /* Clobber possible stale .dim information. */
		  memset (symbolP->sy_symbol.ost_auxent[0].x_sym.x_fcnary.x_ary.x_dimen,
			  '\0', sizeof (symbolP->sy_symbol.ost_auxent[0].x_sym.x_fcnary.x_ary.x_dimen));
		}
	      /* The C_FCN doesn't need any additional information.
	   I don't even know if this is needed for sdb. But the
	   standard assembler generates it, so...
	   */
	      if (S_GET_STORAGE_CLASS (symbolP) == C_EFCN)
		{
		  if (last_functionP == (symbolS *) 0)
		    as_fatal ("C_EFCN symbol out of scope");
		  SA_SET_SYM_FSIZE (last_functionP,
				    (long) (S_GET_VALUE (symbolP) -
					    S_GET_VALUE (last_functionP)));
		  SA_SET_SYM_ENDNDX (last_functionP, symbol_number);
		  last_functionP = (symbolS *) 0;
		}
	    }
	}
      else if (SF_GET_TAG (symbolP))
	{
	  /* First descriptor of a structure must point to
	 the first slot after the structure description. */
	  last_tagP = symbolP;

	}
      else if (S_GET_STORAGE_CLASS (symbolP) == C_EOS)
	{
	  /* +2 take in account the current symbol */
	  SA_SET_SYM_ENDNDX (last_tagP, symbol_number + 2);
	}
      else if (S_GET_STORAGE_CLASS (symbolP) == C_FILE)
	{
	  if (symbolP->sy_symbol.ost_auxent->x_file.x_n.x_zeroes == 0)
	  {
	    symbolP->sy_symbol.ost_auxent->x_file.x_n.x_offset = string_byte_count;	    
	    string_byte_count +=
	     strlen(GET_FILENAME_STRING(symbolP)) + 1;


	  }
	    
	  if (S_GET_VALUE (symbolP))
	    {
	      S_SET_VALUE ((symbolS *) S_GET_VALUE (symbolP), symbol_number);
	      S_SET_VALUE (symbolP, 0);
	    }			/* no one points at the first .file symbol */
	}			/* if debug or tag or eos or file */

      /* We must put the external symbols apart. The loader
	 does not bomb if we do not. But the references in
	 the endndx field for a .bb symbol are not corrected
	 if an external symbol is removed between .bb and .be.
	 I.e in the following case :
	 [20] .bb endndx = 22
	 [21] foo external
	 [22] .be
	 ld will move the symbol 21 to the end of the list but
	 endndx will still be 22 instead of 21. */

      if (SF_GET_LOCAL (symbolP))
	{
	  /* remove C_EFCN and LOCAL (L...) symbols */
	  /* next pointer remains valid */
	  symbol_remove (symbolP, &symbol_rootP, &symbol_lastP);

	}
      else if (
#ifdef TE_I386AIX
	       S_GET_STORAGE_CLASS (symbolP) == C_EXT
	       && !SF_GET_FUNCTION (symbolP)
#else /* not TE_I386AIX */
	       !S_IS_DEFINED (symbolP)
	       && !S_IS_DEBUG (symbolP)
	       && !SF_GET_STATICS (symbolP)
#endif /* not TE_I386AIX */
	       )
	{
	  /* if external, Remove from the list */
	  symbolS *hold = symbol_previous (symbolP);

	  symbol_remove (symbolP, &symbol_rootP, &symbol_lastP);
	  symbol_clear_list_pointers (symbolP);
	  symbol_append (symbolP, symbol_extern_lastP, &symbol_externP, &symbol_extern_lastP);
	  symbolP = hold;
	}
      else
	{
	  if (SF_GET_STRING (symbolP))
	    {
	      symbolP->sy_name_offset = string_byte_count;
	      string_byte_count += strlen (S_GET_NAME (symbolP)) + 1;
	    }
	  else
	    {
	      symbolP->sy_name_offset = 0;
	    }			/* fix "long" names */

	  symbolP->sy_number = symbol_number;
	  symbol_number += 1 + S_GET_NUMBER_AUXILIARY (symbolP);
	}			/* if local symbol */
    }				/* traverse the symbol list */

  for (symbolP = symbol_externP; symbol_externP;)
    {
      symbolS *tmp = symbol_externP;

      /* append */
      symbol_remove (tmp, &symbol_externP, &symbol_extern_lastP);
      symbol_append (tmp, symbol_lastP, &symbol_rootP, &symbol_lastP);

      /* and process */
      if (SF_GET_STRING (tmp))
	{
	  tmp->sy_name_offset = string_byte_count;
	  string_byte_count += strlen (S_GET_NAME (tmp)) + 1;
	}
      else
	{
	  tmp->sy_name_offset = 0;
	}			/* fix "long" names */

      tmp->sy_number = symbol_number;
      symbol_number += 1 + S_GET_NUMBER_AUXILIARY (tmp);
    }				/* append the entire extern chain */

  /* When a tag reference preceeds the tag definition, the definition
     will not have a number at the time we process the reference
     during the first traversal.  Thus, a second traversal. */

  for (symbolP = symbol_rootP; symbolP; symbolP = symbol_next (symbolP))
    {
      if (SF_GET_TAGGED (symbolP))
	{
	  SA_SET_SYM_TAGNDX (symbolP, ((symbolS *) SA_GET_SYM_TAGNDX (symbolP))->sy_number);
	}
    }

  know (symbol_externP == NULL);
  know (symbol_extern_lastP == NULL);

  /* FIXME-SOMEDAY I'm counting line no's here so we know what to put
     in the section headers, and I'm resolving the addresses since I'm
     not sure how to do it later. I am NOT resolving the linno's
     representing functions.  Their symbols need a fileptr pointing to
     this linno when emitted.  Thus, I resolve them on emit.
     xoxorich. */

  for (lineP = lineno_rootP; lineP; lineP = lineP->next)
    {
      if (lineP->line.l_lnno > 0)
	{
	  lineP->line.l_addr.l_paddr += ((fragS *) lineP->frag)->fr_address;
	}
      else
	{
	  ;
	}
      text_lineno_number++;
    }				/* for each line number */

  H_SET_SYMBOL_TABLE_SIZE (headers, symbol_number);

  return;
}

/*
 * Find strings by crawling along symbol table chain.
 */

void
obj_emit_strings (where)
     char **where;
{
  symbolS *symbolP;

#ifdef CROSS_COMPILE
  /* Gotta do md_ byte-ordering stuff for string_byte_count first - KWK */
  md_number_to_chars (*where, string_byte_count, sizeof (string_byte_count));
  *where += sizeof (string_byte_count);
#else /* CROSS_COMPILE */
  append (where, (char *) &string_byte_count, (unsigned long) sizeof (string_byte_count));
#endif /* CROSS_COMPILE */

  for (symbolP = symbol_rootP; symbolP; symbolP = symbol_next (symbolP))
    {
      if (S_GET_STORAGE_CLASS(symbolP) == C_FILE) 
      {
	/* May need special treatment for this auxent */
	if (symbolP->sy_symbol.ost_auxent->x_file.x_n.x_zeroes == 0)
	{
	  char *p = GET_FILENAME_STRING(symbolP);
	  append 
	   (where,p, strlen(p)+1);
	}
      }
      if (SF_GET_STRING (symbolP))
	{
	  append (where, S_GET_NAME (symbolP),
		  (unsigned long) (strlen (S_GET_NAME (symbolP)) + 1));
	}			/* if it has a string */
    }				/* walk the symbol chain */
}

void
obj_pre_write_hook (headers)
     object_headers *headers;
{
  register int text_relocation_number = 0;
  register int data_relocation_number = 0;
  register fixS *fixP;

  /* FIXME-SOMEDAY this should be done at fixup_segment time but I'm
     going to wait until I do multiple segments.  xoxorich. */
  /* Count the number of relocation entries for text and data */
  for (fixP = text_fix_root; fixP; fixP = fixP->fx_next)
    {
      if (fixP->fx_addsy)
	{
	  ++text_relocation_number;
#ifdef TC_I960
	  /* two relocs per callj under coff. */
	  if (fixP->fx_callj)
	    {
	      ++text_relocation_number;
	    }			/* if callj and not already fixed. */
#endif /* TC_I960 */
#ifdef TC_A29K
	  /* Count 2 for a constH */
	  if (fixP->fx_r_type == RELOC_CONSTH)
	    {
	      ++text_relocation_number;
	    }
#endif

	}			/* if not yet fixed */
    }				/* for each fix */

  SA_SET_SCN_NRELOC (dot_text_symbol, text_relocation_number);
  /* Assign the number of line number entries for the text section */
  SA_SET_SCN_NLINNO (dot_text_symbol, text_lineno_number);
  /* Assign the size of the section */
  SA_SET_SCN_SCNLEN (dot_text_symbol, H_GET_TEXT_SIZE (headers));

  for (fixP = data_fix_root; fixP; fixP = fixP->fx_next)
    {
      if (fixP->fx_addsy)
	{
	  ++data_relocation_number;
	}			/* if still relocatable */
#ifdef TC_A29K
      /* Count 2 for a constH */
      if (fixP->fx_r_type == RELOC_CONSTH)
	{
	  ++data_relocation_number;
	}
#endif

    }				/* for each fix */


  SA_SET_SCN_NRELOC (dot_data_symbol, data_relocation_number);
  /* Assign the size of the section */
  SA_SET_SCN_SCNLEN (dot_data_symbol, H_GET_DATA_SIZE (headers));

  /* Assign the size of the section */
  SA_SET_SCN_SCNLEN (dot_bss_symbol, H_GET_BSS_SIZE (headers));

  /* pre write hook can add relocs (for 960 and 29k coff) so */
  headers->relocation_size = text_relocation_number * RELSZ +
    data_relocation_number * RELSZ;



  /* Fill in extra coff fields */

  /* Initialize general line number information. */
  H_SET_LINENO_SIZE (headers, text_lineno_number * LINESZ);

  /* filehdr */
  H_SET_FILE_MAGIC_NUMBER (headers, FILE_HEADER_MAGIC);
  H_SET_NUMBER_OF_SECTIONS (headers, 3);	/* text+data+bss */
#ifndef OBJ_COFF_OMIT_TIMESTAMP
  H_SET_TIME_STAMP (headers, (long) time ((long *) 0));
#else /* OBJ_COFF_OMIT_TIMESTAMP */
  H_SET_TIME_STAMP (headers, 0);
#endif /* OBJ_COFF_OMIT_TIMESTAMP */
  H_SET_SYMBOL_TABLE_POINTER (headers, H_GET_SYMBOL_TABLE_FILE_OFFSET (headers));
#if 0
  printf ("FILHSZ %x\n", FILHSZ);
  printf ("OBJ_COFF_AOUTHDRSZ %x\n", OBJ_COFF_AOUTHDRSZ);
  printf ("section headers %x\n", H_GET_NUMBER_OF_SECTIONS (headers) * SCNHSZ);
  printf ("get text size %x\n", H_GET_TEXT_SIZE (headers));
  printf ("get data size %x\n", H_GET_DATA_SIZE (headers));
  printf ("get relocation size %x\n", H_GET_RELOCATION_SIZE (headers));
  printf ("get lineno size %x\n", H_GET_LINENO_SIZE (headers));
#endif
  /* symbol table size allready set */
  H_SET_SIZEOF_OPTIONAL_HEADER (headers, OBJ_COFF_AOUTHDRSZ);

  /* do not added the F_RELFLG for the standard COFF.
	 * The AIX linker complain on file with relocation info striped flag.
	 */
#ifdef KEEP_RELOC_INFO
  H_SET_FLAGS (headers, (text_lineno_number == 0 ? F_LNNO : 0)
	       | BYTE_ORDERING);
#else
  H_SET_FLAGS (headers, (text_lineno_number == 0 ? F_LNNO : 0)
	| ((text_relocation_number + data_relocation_number) ? 0 : F_RELFLG)
	       | BYTE_ORDERING);
#endif
  /* aouthdr */
  /* magic number allready set */
  H_SET_VERSION_STAMP (headers, 0);
  /* Text, data, bss size; entry point; text_start and data_start are already set */

  /* Build section headers */

  c_section_header (&text_section_header,
		    ".text",
		    0,
		    H_GET_TEXT_SIZE (headers),
		    H_GET_TEXT_FILE_OFFSET (headers),
		    (SA_GET_SCN_NRELOC (dot_text_symbol)
		     ? H_GET_RELOCATION_FILE_OFFSET (headers)
		     : 0),
		    (text_lineno_number
		     ? H_GET_LINENO_FILE_OFFSET (headers)
		     : 0),
		    SA_GET_SCN_NRELOC (dot_text_symbol),
		    text_lineno_number,
		    section_alignment[(int) SEG_TEXT]);

  c_section_header (&data_section_header,
		    ".data",
		    H_GET_TEXT_SIZE (headers),
		    H_GET_DATA_SIZE (headers),
		    (H_GET_DATA_SIZE (headers)
		     ? H_GET_DATA_FILE_OFFSET (headers)
		     : 0),
		    (SA_GET_SCN_NRELOC (dot_data_symbol)
		     ? (H_GET_RELOCATION_FILE_OFFSET (headers)
			+ text_section_header.s_nreloc * RELSZ)
		     : 0),
		    0,		/* No line number information */
		    SA_GET_SCN_NRELOC (dot_data_symbol),
		    0,		/* No line number information */
		    section_alignment[(int) SEG_DATA]);

  c_section_header (&bss_section_header,
		    ".bss",
		    H_GET_TEXT_SIZE (headers) + H_GET_DATA_SIZE (headers),
		    H_GET_BSS_SIZE (headers),
		    0,		/* No file offset */
		    0,		/* No relocation information */
		    0,		/* No line number information */
		    0,		/* No relocation information */
		    0,		/* No line number information */
		    section_alignment[(int) SEG_BSS]);
}

#ifdef BFD_ASSEMBLER
static
unsigned long
align (val, exp)
{
  int n = (1 << exp) - 1;
  printf ("align (%x, %x)\n", val, exp);
  val = (val + n) & ~n;
  return val;
}

void
coff_check_file_symbols (symp, punt)
     symbolS *symp;
     int *punt;
{
  static symbolS *last_functionP, *last_tagP;
  static stack *block_stack;

  if (!block_stack)
    block_stack = stack_init (512, sizeof (symbolS*));

#if 1
  if (!S_IS_DEFINED (symp) && S_GET_STORAGE_CLASS (symp) != C_STAT)
    S_SET_STORAGE_CLASS (symp, C_EXT);
#endif

  if (!SF_GET_DEBUG (symp))
    {
      symbolS *real;
      if (!SF_GET_LOCAL (symp)
	  && (real = symbol_find_base (S_GET_NAME (symp), DO_NOT_STRIP))
	  && real != symp)
	{
	  c_symbol_merge (symp, real);
	  *punt = 1;
	}
      if (!S_IS_DEFINED (symp) && !SF_GET_LOCAL (symp))
	{
	  assert (S_GET_VALUE (symp) == 0);
	  S_SET_EXTERNAL (symp);
	}
      else if (S_GET_STORAGE_CLASS (symp) == C_NULL)
	{
	  if (S_GET_SEGMENT (symp) == text_section)
	    S_SET_STORAGE_CLASS (symp, C_LABEL);
	  else
	    S_SET_STORAGE_CLASS (symp, C_STAT);
	}
      if (SF_GET_PROCESS (symp))
	{
	  if (S_GET_STORAGE_CLASS (symp) == C_BLOCK)
	    {
	      if (!strcmp (S_GET_NAME (symp), ".bb"))
		stack_push (block_stack, (char *) &symp);
	      else
		{
		  symbolS *begin;
		  begin = *(symbolS **) stack_pop (block_stack);
		  if (begin == 0)
		    as_warn ("mismatched .eb");
		  else
		    SA_SET_SYM_ENDNDX (begin, begin);
		}
	    }
	  if (last_functionP == 0 && SF_GET_FUNCTION (symp))
	    {
	      union internal_auxent *auxp;
	      last_functionP = symp;
	      if (S_GET_NUMBER_AUXILIARY (symp) < 1)
		S_SET_NUMBER_AUXILIARY (symp, 1);
	      auxp = &coffsymbol (symp->bsym)->native[1].u.auxent;
	      memset (auxp->x_sym.x_fcnary.x_ary.x_dimen, 0,
		      sizeof (auxp->x_sym.x_fcnary.x_ary.x_dimen));
	    }
	  if (S_GET_STORAGE_CLASS (symp) == C_EFCN)
	    {
	      if (last_functionP == 0)
		as_fatal ("C_EFCN symbol out of scope");
	      SA_SET_SYM_FSIZE (last_functionP,
				(long) (S_GET_VALUE (symp)
					- S_GET_VALUE (last_functionP)));
	      SA_SET_SYM_ENDNDX (last_functionP, symp);
	      last_functionP = 0;
	    }
	}
      else if (SF_GET_TAG (symp))
	last_tagP = symp;
      else if (S_GET_STORAGE_CLASS (symp) == C_EOS)
	SA_SET_SYM_ENDNDX (last_tagP, symp);
      else if (S_GET_STORAGE_CLASS (symp) == C_FILE)
	{
	  if (S_GET_VALUE (symp))
	    {
	      S_SET_VALUE ((symbolS *) S_GET_VALUE (symp), 0xdeadbeef);
	      S_SET_VALUE (symp, 0);
	    }
	}
      if (SF_GET_LOCAL (symp))
	*punt = 1;
      /* more ... */
    }
  if (coffsymbol (symp->bsym)->lineno)
    {
      int i, n;
      struct line_no *lptr;
      alent *l;

      lptr = (struct line_no *) coffsymbol (symp->bsym)->lineno;
      for (i = 0; lptr; lptr = lptr->next)
	i++;
      n = i + 1;
      lptr = (struct line_no *) coffsymbol (symp->bsym)->lineno;
      l = (alent *) bfd_alloc_by_size_t (stdoutput, n * sizeof (alent));
      coffsymbol (symp->bsym)->lineno = l;
      for (i = n - 1; i > 0; i--)
	{
	  if (lptr->frag)
	    lptr->l.u.offset += lptr->frag->fr_address;
	  l[i] = lptr->l;
	  lptr = lptr->next;
	}
    }
}

void 
DEFUN_VOID(obj_coff_section)
{
  /* Strip out the section name */
  char *section_name ;
  char *section_name_end;
  char c;

  unsigned int len;
  unsigned int exp;

  section_name =  input_line_pointer;
  c =   get_symbol_end();
  section_name_end =  input_line_pointer;

  len = section_name_end - section_name ;
  input_line_pointer++;
  SKIP_WHITESPACE();
  if (c == ',')
    exp = get_absolute_expression();
  else if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      exp = get_absolute_expression();
    }
  else
    {
      exp = 0;
    }

  printf ("change_to_section(`%s',%d,%x)\n", section_name, len,exp);
  *section_name_end = c;
}

void
coff_frob_file ()
{
  if (symbol_rootP == NULL
      || S_GET_STORAGE_CLASS (symbol_rootP) != C_FILE)
    {
      assert (previous_file_symbol == 0);
      c_dot_file_symbol ("fake");
    }
  if (current_lineno_sym)
    add_linesym ((symbolS *) 0);
}
#endif /* BFD_ASSEMBLER */

#ifdef DEBUG
/* for debugging */
CONST char *
s_get_name (s)
     symbolS *s;
{
  return ((s == NULL) ? "(NULL)" : S_GET_NAME (s));
}				/* s_get_name() */

void
symbol_dump ()
{
  symbolS *symbolP;

  for (symbolP = symbol_rootP; symbolP; symbolP = symbol_next (symbolP))
    {
#ifdef BFD_ASSEMBLER
      printf("0x%lx: \"%s\" type = %ld, class = %d, segment = %d\n",
	     (unsigned long) symbolP,
	     S_GET_NAME(symbolP),
	     (long) S_GET_DATA_TYPE(symbolP),
	     S_GET_STORAGE_CLASS(symbolP),
	     (int) S_GET_SEGMENT(symbolP));
#else
      printf ("%3ld: 0x%lx \"%s\" type = %ld, class = %d, segment = %d\n",
	      symbolP->sy_number,
	      (unsigned long) symbolP,
	      S_GET_NAME (symbolP),
	      (long) S_GET_DATA_TYPE (symbolP),
	      S_GET_STORAGE_CLASS (symbolP),
	      (int) S_GET_SEGMENT (symbolP));
#endif
    }
}

#endif /* DEBUG */

/* end of obj-coff.c */
