/* BFD backend for hp-ux 9000/300
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Glenn Engel.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*

    hpux native  ------------> |               |
                               | hp300hpux bfd | ----------> hpux w/gnu ext
    hpux w/gnu extension ----> |               |


    Support for the 9000/[34]00 has several limitations.
      1. Shared libraries are not supported.
      2. The output format from this bfd is not usable by native tools.

    The primary motivation for writing this bfd was to allow use of
    gdb and gcc for host based debugging and not to mimic the hp-ux tools
    in every detail.  This leads to a significant simplification of the
    code and a leap in performance.  The decision to not output hp native
    compatible objects was further strengthened by the fact that the richness
    of the gcc compiled objects could not be represented without loss of
    information.  For example, while the hp format supports the concept of
    secondary symbols, it does not support indirect symbols.  Another
    reason is to maintain backwards compatibility with older implementations
    of gcc on hpux which used 'hpxt' to translate .a and .o files into a
    format which could be readily understood by the gnu linker and gdb.
    This allows reading hp secondary symbols and converting them into
    indirect symbols but the reverse it not always possible.

    Another example of differences is that the hp format stores symbol offsets
    in the object code while the gnu utilities use a field in the
    relocation record for this.  To support the hp native format, the object
    code would need to be patched with the offsets when producing .o files.

    The basic technique taken in this implementation is to #include the code
    from aoutx.h and aout-target.h with appropriate #defines to override
    code where a unique implementation is needed:

    {
        #define a bunch of stuff
        #include <aoutx.h>

        implement a bunch of functions

        #include "aout-target.h"
    }

    The hp symbol table is a bit different than other a.out targets.  Instead
    of having an array of nlist items and an array of strings, hp's format
    has them mixed together in one structure.  In addition, the strings are
    not null terminated.  It looks something like this:

    nlist element 1
    string1
    nlist element 2
    string2
    ...

    The whole symbol table is read as one chunk and then we march thru it
    and convert it to canonical form.  As we march thru the table, we copy
    the nlist data into the internal form and we compact the strings and null
    terminate them, using storage from the already allocated symbol table:

    string1
    null
    string2
    null
 */

/* @@ Is this really so different from normal a.out that it needs to include
   aoutx.h?  We should go through this file sometime and see what can be made
   more dependent on aout32.o and what might need to be broken off and accessed
   through the backend_data field.  Or, maybe we really do need such a
   completely separate implementation.  I don't have time to investigate this
   much further right now.  [raeburn:19930428.2124EST] */
/* @@ Also, note that there wind up being two versions of some routines, with
   different names, only one of which actually gets used.  For example:
	slurp_symbol_table
	swap_std_reloc_in
	slurp_reloc_table
	get_symtab
	get_symtab_upper_bound
	canonicalize_reloc
	mkobject
   This should also be fixed.  */

#define ARCH 32
#define TARGETNAME "a.out-hp300hpux"
#define MY(OP) CAT(hp300hpux_,OP)

#define external_exec hp300hpux_exec_bytes
#define external_nlist hp300hpux_nlist_bytes

#include "aout/hp300hpux.h"

/* define these so we can compile unused routines in aoutx.h */
#define e_strx  e_shlib
#define e_other e_length
#define e_desc  e_almod

#define AR_PAD_CHAR '/'
#define TARGET_IS_BIG_ENDIAN_P
#define DEFAULT_ARCH bfd_arch_m68k

#define MY_get_section_contents aout_32_get_section_contents
#define MY_close_and_cleanup aout_32_close_and_cleanup
#define MY_slurp_armap bfd_slurp_bsd_armap_f2

/***********************************************/
/* provide overrides for routines in this file */
/***********************************************/
/* these don't use MY because that causes problems within JUMP_TABLE
   (CAT winds up being expanded recursively, which ANSI C compilers
   will not do).  */
#define MY_get_symtab hp300hpux_get_symtab
#define MY_get_symtab_upper_bound hp300hpux_get_symtab_upper_bound
#define MY_canonicalize_reloc hp300hpux_canonicalize_reloc
#define MY_write_object_contents hp300hpux_write_object_contents

#define hp300hpux_write_syms aout_32_write_syms

#define MY_callback MY(callback)

#define NAME_swap_exec_header_in NAME(hp300hpux_32_,swap_exec_header_in)

#define HP_SYMTYPE_UNDEFINED	0x00
#define HP_SYMTYPE_ABSOLUTE	0x01
#define HP_SYMTYPE_TEXT		0x02
#define HP_SYMTYPE_DATA		0x03
#define HP_SYMTYPE_BSS		0x04
#define HP_SYMTYPE_COMMON	0x05

#define HP_SYMTYPE_TYPE		0x0F
#define HP_SYMTYPE_FILENAME	0x1F

#define HP_SYMTYPE_ALIGN	0x10
#define HP_SYMTYPE_EXTERNAL	0x20
#define HP_SECONDARY_SYMBOL     0x40

/* RELOCATION DEFINITIONS */
#define HP_RSEGMENT_TEXT	0x00
#define HP_RSEGMENT_DATA	0x01
#define HP_RSEGMENT_BSS		0x02
#define HP_RSEGMENT_EXTERNAL	0x03
#define HP_RSEGMENT_PCREL       0x04
#define HP_RSEGMENT_RDLT        0x05
#define HP_RSEGMENT_RPLT        0x06
#define HP_RSEGMENT_NOOP	0x3F

#define HP_RLENGTH_BYTE		0x00
#define HP_RLENGTH_WORD		0x01
#define HP_RLENGTH_LONG		0x02
#define HP_RLENGTH_ALIGN	0x03

#define NAME(x,y) CAT3(hp300hpux,_32_,y)
#include "aoutx.h"

/* Since the hpux symbol table has nlist elements interspersed with
   strings and we need to insert som strings for secondary symbols, we
   give ourselves a little extra padding up front to account for
   this.  Note that for each non-secondary symbol we process, we gain
   9 bytes of space for the discarded nlist element (one byte used for
   null).  SYM_EXTRA_BYTES is the extra space.  */
#define SYM_EXTRA_BYTES   1024

/* Set parameters about this a.out file that are machine-dependent.
   This routine is called from some_aout_object_p just before it returns.  */
static bfd_target *
DEFUN(MY(callback),(abfd),
      bfd *abfd)
{
  struct internal_exec *execp = exec_hdr (abfd);

  /* Calculate the file positions of the parts of a newly read aout header */
  obj_textsec (abfd)->_raw_size = N_TXTSIZE(*execp);

  /* The virtual memory addresses of the sections */
  obj_textsec (abfd)->vma = N_TXTADDR(*execp);
  obj_datasec (abfd)->vma = N_DATADDR(*execp);
  obj_bsssec  (abfd)->vma = N_BSSADDR(*execp);

  /* The file offsets of the sections */
  obj_textsec (abfd)->filepos = N_TXTOFF (*execp);
  obj_datasec (abfd)->filepos = N_DATOFF (*execp);

  /* The file offsets of the relocation info */
  obj_textsec (abfd)->rel_filepos = N_TRELOFF(*execp);
  obj_datasec (abfd)->rel_filepos = N_DRELOFF(*execp);

  /* The file offsets of the string table and symbol table.  */
  obj_sym_filepos (abfd) = N_SYMOFF (*execp);
  obj_str_filepos (abfd) = N_STROFF (*execp);

  /* Determine the architecture and machine type of the object file.  */
#ifdef SET_ARCH_MACH
  SET_ARCH_MACH(abfd, *execp);
#else
  bfd_default_set_arch_mach(abfd, DEFAULT_ARCH, 0);
#endif


  if (obj_aout_subformat(abfd) == gnu_encap_format)
  {
      /* The file offsets of the relocation info */
      obj_textsec (abfd)->rel_filepos = N_GNU_TRELOFF(*execp);
      obj_datasec (abfd)->rel_filepos = N_GNU_DRELOFF(*execp);

      /* The file offsets of the string table and symbol table.  */
      obj_sym_filepos (abfd) = N_GNU_SYMOFF(*execp);
      obj_str_filepos (abfd) = (obj_sym_filepos (abfd) + execp->a_syms);

      abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;
      bfd_get_symcount (abfd) = execp->a_syms / 12;
      obj_symbol_entry_size (abfd) = 12;
      obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
  }

  return abfd->xvec;
}

void aout_32_write_syms PARAMS ((bfd *abfd));

static boolean
DEFUN(MY(write_object_contents),(abfd),
      bfd *abfd)
{
    struct external_exec exec_bytes;
    struct internal_exec *execp = exec_hdr (abfd);
    bfd_size_type text_size; /* dummy vars */			
    file_ptr text_end;

    memset (&exec_bytes, 0, sizeof (exec_bytes));
#if CHOOSE_RELOC_SIZE
    CHOOSE_RELOC_SIZE(abfd);
#else
    obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
#endif

    if (adata(abfd).magic == undecided_magic)			
        NAME(aout,adjust_sizes_and_vmas) (abfd, &text_size, &text_end);
    execp->a_syms = 0;

    execp->a_entry = bfd_get_start_address (abfd);			

    execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *		
                       obj_reloc_entry_size (abfd));		
    execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *		
                       obj_reloc_entry_size (abfd));		

    N_SET_MACHTYPE(*execp, 0xc);
    N_SET_FLAGS (*execp, 0x2);

    NAME(aout,swap_exec_header_out) (abfd, execp, &exec_bytes);	

    /* update fields not covered by default swap_exec_header_out */

    /* this is really the sym table size but we store it in drelocs */
    bfd_h_put_32 (abfd,  bfd_get_symcount (abfd) * 12, exec_bytes.e_drelocs);

    bfd_seek (abfd, 0L, false);					
    bfd_write ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd);

    /* Now write out reloc info, followed by syms and strings */	

    if (bfd_get_symcount (abfd) != 0) 				
    {								
        bfd_seek (abfd,	(long)(N_TRELOFF(*execp)), false);	

        if (!NAME(aout,squirt_out_relocs) (abfd, obj_textsec (abfd))) return false;
        bfd_seek (abfd, (long)(N_DRELOFF(*execp)), false);	

        if (!NAME(aout,squirt_out_relocs)(abfd, obj_datasec (abfd))) return false;
    }

    MY(write_syms)(abfd);
    return true;
}									

/* convert the hp symbol type to be the same as aout64.h usage so we */
/* can piggyback routines in aoutx.h.                                */

static void
DEFUN(convert_sym_type,(sym_pointer, cache_ptr, abfd),
      struct external_nlist *sym_pointer AND
      aout_symbol_type *cache_ptr AND
      bfd *abfd)
{
    int name_type;
    int new_type;

    name_type = (cache_ptr->type);
    new_type  = 0;

    if ((name_type & HP_SYMTYPE_ALIGN) != 0)
    {
        /* iou_error ("aligned symbol encountered: %s", name);*/
        name_type = 0;
    }

    if (name_type == HP_SYMTYPE_FILENAME)
      new_type = N_FN;
    else
      {
	switch (name_type & HP_SYMTYPE_TYPE)
	  {
	  case HP_SYMTYPE_UNDEFINED:
	    new_type = N_UNDF;
	    break;

	  case HP_SYMTYPE_ABSOLUTE:
	    new_type = N_ABS;
	    break;

	  case HP_SYMTYPE_TEXT:
	    new_type = N_TEXT;
	    break;

	  case HP_SYMTYPE_DATA:
	    new_type = N_DATA;
	    break;

	  case HP_SYMTYPE_BSS:
	    new_type = N_BSS;
	    break;

	  case HP_SYMTYPE_COMMON:
	    new_type = N_COMM;
	    break;

	  default:
	    fprintf (stderr, "unknown symbol type encountered: %x", name_type);
	  }
	if (name_type & HP_SYMTYPE_EXTERNAL)
	  new_type |= N_EXT;

        if (name_type & HP_SECONDARY_SYMBOL)
            new_type = (new_type & ~N_TYPE) | N_INDR;
      }
    cache_ptr->type = new_type;

}


/*
DESCRIPTION
        Swaps the information in an executable header taken from a raw
        byte stream memory image, into the internal exec_header
        structure.
*/

void
DEFUN(NAME(aout,swap_exec_header_in),(abfd, raw_bytes, execp),
      bfd *abfd AND
      struct external_exec *raw_bytes AND
      struct internal_exec *execp)
{
  struct external_exec *bytes = (struct external_exec *)raw_bytes;

  /* The internal_exec structure has some fields that are unused in this
     configuration (IE for i960), so ensure that all such uninitialized
     fields are zero'd out.  There are places where two of these structs
     are memcmp'd, and thus the contents do matter. */
  memset (execp, 0, sizeof (struct internal_exec));
  /* Now fill in fields in the execp, from the bytes in the raw data.  */
  execp->a_info   = bfd_h_get_32 (abfd, bytes->e_info);
  execp->a_text   = GET_WORD (abfd, bytes->e_text);
  execp->a_data   = GET_WORD (abfd, bytes->e_data);
  execp->a_bss    = GET_WORD (abfd, bytes->e_bss);
  execp->a_syms   = GET_WORD (abfd, bytes->e_syms);
  execp->a_entry  = GET_WORD (abfd, bytes->e_entry);
  execp->a_trsize = GET_WORD (abfd, bytes->e_trsize);
  execp->a_drsize = GET_WORD (abfd, bytes->e_drsize);

  /***************************************************************/
  /* check the header to see if it was generated by a bfd output */
  /* this is detected rather bizarely by requiring a bunch of    */
  /* header fields to be zero and an old unused field (now used) */
  /* to be set.                                                  */
  /***************************************************************/
  do
  {
      long syms;
      struct aout_data_struct  *rawptr;
      if (bfd_h_get_32 (abfd, bytes->e_passize) != 0) break;
      if (bfd_h_get_32 (abfd, bytes->e_syms) != 0) break;
      if (bfd_h_get_32 (abfd, bytes->e_supsize) != 0) break;

      syms = bfd_h_get_32 (abfd, bytes->e_drelocs);
      if (syms == 0) break;

      /* OK, we've passed the test as best as we can determine */
      execp->a_syms = syms;

      /* allocate storage for where we will store this result */
      rawptr = (struct aout_data_struct  *) bfd_zalloc (abfd, sizeof (*rawptr));

      if (rawptr == NULL) {
          bfd_error = no_memory;
          return;
      }
      abfd->tdata.aout_data = rawptr;
      obj_aout_subformat(abfd) = gnu_encap_format;
  } while (0);
}


/* The hp symbol table is a bit different than other a.out targets.  Instead
   of having an array of nlist items and an array of strings, hp's format
   has them mixed together in one structure.  In addition, the strings are
   not null terminated.  It looks something like this:

   nlist element 1
   string1
   nlist element 2
   string2
   ...

   The whole symbol table is read as one chunk and then we march thru it
   and convert it to canonical form.  As we march thru the table, we copy
   the nlist data into the internal form and we compact the strings and null
   terminate them, using storage from the already allocated symbol table:

   string1
   null
   string2
   null
   ...
*/

boolean
DEFUN(MY(slurp_symbol_table),(abfd),
      bfd *abfd)
{
  bfd_size_type symbol_bytes;
  struct external_nlist *syms;
  struct external_nlist *sym_pointer;
  struct external_nlist *sym_end;
  char *strings;
  aout_symbol_type *cached;
  unsigned num_syms = 0;
  unsigned num_secondary = 0;

  /* If there's no work to be done, don't do any */
  if (obj_aout_symbols (abfd) != (aout_symbol_type *)NULL) return true;
  symbol_bytes = exec_hdr(abfd)->a_syms;
  if (symbol_bytes == 0) {
    bfd_error = no_symbols;
    return false;
  }

  strings = (char *) bfd_alloc(abfd,
                               symbol_bytes + SYM_EXTRA_BYTES);
  syms = (struct external_nlist *) (strings + SYM_EXTRA_BYTES);
  bfd_seek (abfd, obj_sym_filepos (abfd), SEEK_SET);
  if (bfd_read ((PTR)syms, symbol_bytes, 1, abfd) != symbol_bytes)
  {
      bfd_release (abfd, syms);
      return false;
  }


  sym_end = (struct external_nlist *) (((char *)syms) + symbol_bytes);

  /* first, march thru the table and figure out how many symbols there are */
  for (sym_pointer = syms; sym_pointer < sym_end; sym_pointer++, num_syms++)
  {
      if (bfd_get_8(abfd,  sym_pointer->e_type) & HP_SECONDARY_SYMBOL)
          num_secondary++;
      /* skip over the embedded symbol. */
      sym_pointer = (struct external_nlist *) (((char *)sym_pointer) +
                                                     sym_pointer->e_length[0]);
  }

  /* now that we know the symbol count, update the bfd header */
  bfd_get_symcount (abfd) = num_syms+num_secondary;

  cached = (aout_symbol_type *)
    bfd_zalloc(abfd, (bfd_size_type)(bfd_get_symcount (abfd) *
                                     sizeof(aout_symbol_type)));

  /* as we march thru the hp symbol table, convert it into a list of
     null terminated strings to hold the symbol names.  Make sure any
     assignment to the strings pointer is done after we're thru using
     the nlist so we don't overwrite anything important. */
  num_secondary = 0;

  /* OK, now walk the new symtable, cacheing symbol properties */
    {
      aout_symbol_type *cache_ptr = cached;
      aout_symbol_type cache_save;
      /* Run through table and copy values */
      for (sym_pointer = syms, cache_ptr = cached;
	   sym_pointer < sym_end; sym_pointer++, cache_ptr++)
	  {
            unsigned int length;
	    cache_ptr->symbol.the_bfd = abfd;
	    cache_ptr->symbol.value = GET_SWORD(abfd,  sym_pointer->e_value);
	    cache_ptr->desc = bfd_get_16(abfd, sym_pointer->e_almod);
	    cache_ptr->type = bfd_get_8(abfd,  sym_pointer->e_type);
	    cache_ptr->symbol.udata = 0;
            length = bfd_get_8(abfd, sym_pointer->e_length);
	    cache_ptr->other = length;  /* other not used, save length here */

            cache_save = *cache_ptr;
            convert_sym_type(sym_pointer, cache_ptr, abfd);
	    translate_from_native_sym_flags (sym_pointer, cache_ptr, abfd);

            /********************************************************/
            /* for hpux, the 'lenght' value indicates the length of */
            /* the symbol name which follows the nlist entry.       */
            /********************************************************/
            if (length)
            {
                /**************************************************************/
                /* the hp string is not null terminated so we create a new one*/
                /* by copying the string to overlap the just vacated nlist    */
                /* structure before it in memory.                             */
                /**************************************************************/
                cache_ptr->symbol.name = strings;
                memcpy(strings, sym_pointer+1, length);
                strings[length] = '\0';
                strings += length + 1;
            }
	    else
	      cache_ptr->symbol.name = (char *)NULL;

            /**********************************************************/
            /* this is a bit of a kludge, but a secondary hp symbol   */
            /* gets translated into a gnu indirect symbol.  When this */
            /* happens, we need to create a "dummy" record to which   */
            /* we can point the indirect symbol to.                   */
            /**********************************************************/
            if ((cache_ptr->type | N_EXT) == (N_INDR | N_EXT))
            {
                aout_symbol_type *cache_ptr2 = cached + num_syms+num_secondary;

                num_secondary++;

                /* aoutx.h assumes the "next" value is the indirect sym  */
                /* since we don't want to disturb the order by inserting */
                /* a new symbol, we tack on the created secondary syms   */
                /* at the end.                                           */
                cache_ptr->symbol.value = (bfd_vma)(cache_ptr2);
                *cache_ptr2 = cache_save;
                cache_ptr2->symbol.name = strings;
                memcpy(strings, cache_ptr->symbol.name, length);
                strcpy(strings+length,":secondry"); /* 9 max chars + null */
                strings += length+10;
                cache_ptr2->type &= ~HP_SECONDARY_SYMBOL;  /* clear secondary */
                convert_sym_type(sym_pointer, cache_ptr2, abfd);
                translate_from_native_sym_flags (sym_pointer, cache_ptr2,
						 abfd);
            }

            /* skip over the embedded symbol. */
            sym_pointer = (struct external_nlist *) (((char *)sym_pointer) +
                                                 length);
	  }
    }

  obj_aout_symbols (abfd) =  cached;

  return true;
}



void
DEFUN(MY(swap_std_reloc_in), (abfd, bytes, cache_ptr, symbols),
  bfd *abfd AND
  struct hp300hpux_reloc *bytes AND
  arelent *cache_ptr AND
  asymbol **symbols)
{
  int r_index;
  int r_extern = 0;
  unsigned int r_length;
  int r_pcrel = 0;
  struct aoutdata  *su = &(abfd->tdata.aout_data->a);

  cache_ptr->address = bfd_h_get_32 (abfd, bytes->r_address);
  r_index = bfd_h_get_16(abfd, bytes->r_index);

  switch (bytes->r_type[0])
  {
  case HP_RSEGMENT_TEXT:
      r_index = N_TEXT;
      break;
  case HP_RSEGMENT_DATA:
      r_index = N_DATA;
      break;
  case HP_RSEGMENT_BSS:
      r_index = N_BSS;
      break;
  case HP_RSEGMENT_EXTERNAL:
      r_extern = 1;
      break;
  case HP_RSEGMENT_PCREL:
      r_extern = 1;
      r_pcrel = 1;
      break;
  case HP_RSEGMENT_RDLT:
      break;
  case HP_RSEGMENT_RPLT:
      break;
  case HP_RSEGMENT_NOOP:
      break;
  default:
      fprintf (stderr, "illegal relocation segment type: %x\n",
	       (bytes->r_type[0]));
  }

  switch (bytes->r_length[0])
    {
    case HP_RLENGTH_BYTE:
      r_length = 0;
      break;
    case HP_RLENGTH_WORD:
      r_length = 1;
      break;
    case HP_RLENGTH_LONG:
      r_length = 2;
      break;
    default:
      fprintf (stderr, "illegal relocation length: %x\n",bytes->r_length[0] );
      r_length = 0;
    }

  cache_ptr->howto =  howto_table_std + r_length + 4 * r_pcrel;
  /* FIXME-soon:  Roll baserel, jmptable, relative bits into howto setting */

  /* This macro uses the r_index value computed above */
  if (r_pcrel && r_extern)
  {
    /* The GNU linker assumes any offset from beginning of section */
    /* is already incorporated into the image while the HP linker  */
    /* adds this in later.  Add it in now...                       */
    MOVE_ADDRESS( - cache_ptr->address);
  }
  else
  {
    MOVE_ADDRESS(0);
  }
}

boolean
DEFUN(MY(slurp_reloc_table),(abfd, asect, symbols),
      bfd *abfd AND
      sec_ptr asect AND
      asymbol **symbols)
{
  unsigned int count;
  bfd_size_type reloc_size;
  PTR relocs;
  arelent *reloc_cache;
  size_t each_size;
  struct hp300hpux_reloc *rptr;
  unsigned int counter;
  arelent *cache_ptr;

  if (asect->relocation) return true;

  if (asect->flags & SEC_CONSTRUCTOR) return true;

  if (asect == obj_datasec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_drsize;
    goto doit;
  }

  if (asect == obj_textsec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_trsize;
    goto doit;
  }

  bfd_error = invalid_operation;
  return false;

 doit:
  bfd_seek (abfd, asect->rel_filepos, SEEK_SET);
  each_size = obj_reloc_entry_size (abfd);

  count = reloc_size / each_size;


  reloc_cache = (arelent *) bfd_zalloc (abfd, (size_t)(count * sizeof
						       (arelent)));
  if (!reloc_cache) {
nomem:
    bfd_error = no_memory;
    return false;
  }

  relocs = (PTR) bfd_alloc (abfd, reloc_size);
  if (!relocs) {
    bfd_release (abfd, reloc_cache);
    goto nomem;
  }

  if (bfd_read (relocs, 1, reloc_size, abfd) != reloc_size) {
    bfd_release (abfd, relocs);
    bfd_release (abfd, reloc_cache);
    bfd_error = system_call_error;
    return false;
  }

    rptr = (struct hp300hpux_reloc*) relocs;
    counter = 0;
    cache_ptr = reloc_cache;

    for (; counter < count; counter++, rptr++, cache_ptr++) {
	MY(swap_std_reloc_in)(abfd, rptr, cache_ptr, symbols);
    }


  bfd_release (abfd,relocs);
  asect->relocation = reloc_cache;
  asect->reloc_count = count;
  return true;
}


/************************************************************************/
/* The following functions are identical to functions in aoutx.h except */
/* they refer to MY(func) rather than NAME(aout,func) and they also     */
/* call aout_32 versions if the input file was generated by gcc         */
/************************************************************************/

unsigned int aout_32_get_symtab PARAMS((bfd *abfd, asymbol **location));
unsigned int aout_32_get_symtab_upper_bound PARAMS ((bfd *abfd));

unsigned int aout_32_canonicalize_reloc PARAMS((bfd *abfd, sec_ptr section,
                                                arelent **relptr,
                                                asymbol **symbols));

unsigned int
DEFUN(MY(get_symtab),(abfd, location),
      bfd *abfd AND
      asymbol **location)
{
    unsigned int counter = 0;
    aout_symbol_type *symbase;

    if (obj_aout_subformat(abfd) == gnu_encap_format)
        return aout_32_get_symtab(abfd,location);

    if (!MY(slurp_symbol_table)(abfd)) return 0;

    for (symbase = obj_aout_symbols(abfd); counter++ < bfd_get_symcount (abfd);)
      *(location++) = (asymbol *)( symbase++);
    *location++ =0;
    return bfd_get_symcount (abfd);
}

 unsigned int
DEFUN(MY(get_symtab_upper_bound),(abfd),
     bfd *abfd)
{
    if (obj_aout_subformat(abfd) == gnu_encap_format)
        return aout_32_get_symtab_upper_bound(abfd);
    if (!MY(slurp_symbol_table)(abfd)) return 0;

    return (bfd_get_symcount (abfd)+1) * (sizeof (aout_symbol_type *));
}




unsigned int
DEFUN(MY(canonicalize_reloc),(abfd, section, relptr, symbols),
      bfd *abfd AND
      sec_ptr section AND
      arelent **relptr AND
      asymbol **symbols)
{
  arelent *tblptr = section->relocation;
  unsigned int count;
  if (obj_aout_subformat(abfd) == gnu_encap_format)
      return aout_32_canonicalize_reloc(abfd,section,relptr,symbols);

  if (!(tblptr || MY(slurp_reloc_table)(abfd, section, symbols)))
    return 0;

  if (section->flags & SEC_CONSTRUCTOR) {
    arelent_chain *chain = section->constructor_chain;
    for (count = 0; count < section->reloc_count; count ++) {
      *relptr ++ = &chain->relent;
      chain = chain->next;
    }
  }
  else {
    tblptr = section->relocation;
    if (!tblptr) return 0;

    for (count = 0; count++ < section->reloc_count;)
      {
	*relptr++ = tblptr++;
      }
  }
  *relptr = 0;

  return section->reloc_count;
}


#include "aout-target.h"
