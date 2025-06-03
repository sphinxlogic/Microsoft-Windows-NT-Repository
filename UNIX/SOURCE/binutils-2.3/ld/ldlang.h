/* ldlang.h - linker command language support
   Copyright 1991, 1992 Free Software Foundation, Inc.
   
   This file is part of GLD, the Gnu Linker.
   
   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.
   
   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

typedef enum {
  lang_input_file_is_l_enum,
  lang_input_file_is_symbols_only_enum,
  lang_input_file_is_marker_enum,
  lang_input_file_is_fake_enum,
  lang_input_file_is_search_file_enum,
  lang_input_file_is_file_enum } lang_input_file_enum_type;

typedef unsigned short fill_type;
typedef struct statement_list {
  union lang_statement_union *head;
  union lang_statement_union **tail;
} lang_statement_list_type;


typedef struct memory_region_struct {
  char *name;
  struct memory_region_struct *next;
  bfd_vma origin;
  bfd_size_type length;
  bfd_vma current;
  bfd_size_type old_length;
  int flags;
  boolean had_full_message;

} lang_memory_region_type ;

typedef struct lang_statement_header_struct 
  {
    union  lang_statement_union  *next;
    enum statement_enum {
      lang_output_section_statement_enum,
      lang_assignment_statement_enum,
      lang_input_statement_enum,
      lang_address_statement_enum,
      lang_wild_statement_enum,
      lang_input_section_enum,
      lang_object_symbols_statement_enum,
      lang_fill_statement_enum,
      lang_data_statement_enum,
      lang_target_statement_enum,
      lang_output_statement_enum,
      lang_padding_statement_enum,
      
      lang_afile_asection_pair_statement_enum,
	lang_constructors_statement_enum
      } type;
    
  } lang_statement_header_type;


typedef struct 
  {
    lang_statement_header_type header;
    union etree_union *exp;
  }  lang_assignment_statement_type;


typedef struct lang_target_statement_struct {
  lang_statement_header_type header;
  CONST char *target;
} lang_target_statement_type;


typedef struct lang_output_statement_struct {
  lang_statement_header_type header;
  CONST char *name;
} lang_output_statement_type;


typedef struct lang_output_section_statement_struct 
{
  lang_statement_header_type header;
  union etree_union *addr_tree;
  lang_statement_list_type children;
  CONST char *memspec;
  union lang_statement_union *next;
  CONST char *name;

  boolean processed;
    
  asection *bfd_section;
  int flags;			/* Or together of all input sections */
  int loadable;			/* set from NOLOAD flag in script */
  struct memory_region_struct *region;
  size_t block_value;
  fill_type fill;

  int subsection_alignment;  /* alignment of components */
  int section_alignment;  /* alignment of start of section */

  union etree_union *load_base;  
} lang_output_section_statement_type;


typedef struct {
  lang_statement_header_type header;
} lang_common_statement_type;

typedef struct {
  lang_statement_header_type header;
} lang_object_symbols_statement_type;

typedef struct {
  lang_statement_header_type header;
  fill_type fill;
  int size;
  asection *output_section;
} lang_fill_statement_type;

typedef struct {
  lang_statement_header_type header;
  unsigned int type;
  union  etree_union *exp;
  bfd_vma value;
  asection *output_section;
  bfd_vma output_vma;
} lang_data_statement_type;




typedef struct lang_input_statement_struct
  {
    lang_statement_header_type header;
    /* Name of this file.  */
    CONST char *filename;
    /* Name to use for the symbol giving address of text start */
    /* Usually the same as filename, but for a file spec'd with -l
       this is the -l switch itself rather than the filename.  */
    CONST char *local_sym_name;
    
    bfd *the_bfd;
    
    boolean closed;
    file_ptr passive_position;
    
    /* Symbol table of the file.  */
    asymbol **asymbols;
    unsigned int symbol_count;
    
    /* For library members only */
    
    /* For a library, points to chain of entries for the library members.  */
    struct lang_input_statement_struct *subfiles;

    /* Size of contents of this file, if library member.  */
    bfd_size_type total_size;

    /* For library member, points to the library's own entry.  */
    struct lang_input_statement_struct *superfile;

    /* For library member, points to next entry for next member.  */
    struct lang_input_statement_struct *chain;

    /* Point to the next file - whatever it is, wanders up and down
       archives */
    
    union lang_statement_union  *next;
    /* Point to the next file, but skips archive contents */
    union  lang_statement_union  *next_real_file;
    
    boolean is_archive;
    
    /* 1 means search a set of directories for this file.  */
    boolean search_dirs_flag;
    
    /* 1 means this is base file of incremental load.
       Do not load this file's text or data.
       Also default text_start to after this file's bss. */
    
    boolean just_syms_flag;
    
    boolean loaded;
    
    
    /*    unsigned int globals_in_this_file;*/
    CONST char *target;
    boolean real;
    asection *common_section;
    asection *common_output_section;
    boolean complained;
  } lang_input_statement_type;

typedef struct {
  lang_statement_header_type header;
  asection *section;
  lang_input_statement_type *ifile;
  
} lang_input_section_type;


typedef struct {
  lang_statement_header_type header;
  asection *section;
  union lang_statement_union *file;
} lang_afile_asection_pair_statement_type;

typedef struct lang_wild_statement_struct {
  lang_statement_header_type header;
  CONST char *section_name;
  CONST char *filename;
  lang_statement_list_type children;
} lang_wild_statement_type;

typedef struct lang_address_statement_struct {
  lang_statement_header_type header;
  CONST  char *section_name;
  union  etree_union *address;
} lang_address_statement_type;

typedef struct {
  lang_statement_header_type header;
  bfd_vma output_offset;
  size_t size;
  asection *output_section;
  fill_type fill;
} lang_padding_statement_type;

typedef union lang_statement_union 
  {
    lang_statement_header_type header;
    union lang_statement_union *next;
    lang_wild_statement_type wild_statement;
    lang_data_statement_type data_statement;
    lang_address_statement_type address_statement;
    lang_output_section_statement_type output_section_statement;
    lang_afile_asection_pair_statement_type afile_asection_pair_statement;
    lang_assignment_statement_type assignment_statement;
    lang_input_statement_type input_statement;
    lang_target_statement_type target_statement;
    lang_output_statement_type output_statement;
    lang_input_section_type input_section;
    lang_common_statement_type common_statement;
    lang_object_symbols_statement_type object_symbols_statement;
    lang_fill_statement_type fill_statement;
    lang_padding_statement_type padding_statement;
  } lang_statement_union_type;



void lang_init PARAMS ((void));
struct memory_region_struct *
     lang_memory_region_lookup PARAMS ((CONST char *CONST));

void lang_map PARAMS ((void));
void lang_set_flags PARAMS ((int *, CONST char *));
void lang_add_output PARAMS ((CONST char *, int from_script));

void lang_final PARAMS ((void));
struct symbol_cache_entry *
     create_symbol PARAMS ((CONST char *, unsigned int, struct sec *));
void lang_process PARAMS ((void));
void lang_section_start PARAMS ((CONST char *, union etree_union *));
void lang_add_entry PARAMS ((CONST char *));
void lang_add_target PARAMS ((CONST char *));
void lang_add_wild PARAMS ((CONST char *CONST , CONST char *CONST));
void lang_add_map PARAMS ((CONST char *));
void lang_add_fill PARAMS ((int));
void lang_add_assignment PARAMS ((union etree_union *));
void lang_add_attribute PARAMS ((enum statement_enum));
void lang_startup PARAMS ((CONST char *));
void lang_float PARAMS ((enum bfd_boolean));
void lang_leave_output_section_statement PARAMS ((bfd_vma, CONST char *));
void lang_abs_symbol_at_end_of PARAMS ((CONST char *, CONST char *));
void lang_abs_symbol_at_beginning_of PARAMS ((CONST char *, CONST char *));
void lang_statement_append PARAMS ((struct statement_list *,
	union lang_statement_union *, union lang_statement_union **));
void lang_for_each_file PARAMS ((void (*dothis)(lang_input_statement_type *)));


#define LANG_FOR_EACH_INPUT_STATEMENT(statement)               \
  extern lang_statement_list_type file_chain;			\
  lang_input_statement_type *statement;				\
  for (statement = (lang_input_statement_type *)file_chain.head;\
       statement != (lang_input_statement_type *)NULL;		\
       statement = (lang_input_statement_type *)statement->next)\
  
#define LANG_FOR_EACH_INPUT_SECTION(statement, abfd, section, x) \
  { extern lang_statement_list_type file_chain;			\
      lang_input_statement_type *statement;				\
	for (statement = (lang_input_statement_type *)file_chain.head;\
	     statement != (lang_input_statement_type *)NULL;		\
	     statement = (lang_input_statement_type *)statement->next)\
		 {								\
	  asection *section;					\
    bfd *abfd = statement->the_bfd;				\
      for (section = abfd->sections;				\
	   section != (asection *)NULL;				\
	   section = section->next) {				\
	  x;							\
  }								\
  }								\
 }		

#define LANG_FOR_EACH_OUTPUT_SECTION(section, x)		\
  { extern bfd *output_bfd;					\
      asection *section;      					\
	for (section = output_bfd->sections;				\
	     section != (asection *)NULL;				\
	     section = section->next)				\
		 { x; }							\
		 }


void lang_process PARAMS ((void));
void ldlang_add_file PARAMS ((lang_input_statement_type *));

lang_output_section_statement_type
      *lang_output_section_find PARAMS ((CONST char * CONST));

lang_input_statement_type *
      lang_add_input_file PARAMS ((CONST char *name,
			   lang_input_file_enum_type file_type,
			  CONST char *target));
lang_output_section_statement_type *
      lang_output_section_statement_lookup PARAMS ((CONST char * CONST name));

void ldlang_add_undef PARAMS ((CONST char *CONST name));
void lang_add_output_format PARAMS ((CONST char *, int from_script));


void lang_list_init PARAMS ((lang_statement_list_type*));

void lang_add_data PARAMS ((int type, union etree_union *));

void lang_for_each_statement PARAMS ((void (*func)()));

PTR stat_alloc PARAMS ((size_t size));
