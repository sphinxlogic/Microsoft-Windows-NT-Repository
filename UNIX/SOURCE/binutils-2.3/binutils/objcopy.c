/* objcopy.c -- copy object file from input to output, optionally massaging it.
   Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GNU Binutils.

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

#include "bfd.h"
#include "sysdep.h"
#include "bucomm.h"
#include <getopt.h>

asymbol       **isympp = NULL; /* Input symbols */
asymbol       **osympp = NULL; /* Output symbols that survive stripping */
char           *input_target = NULL;
char           *output_target = NULL;
char           *input_filename = NULL;
char           *output_filename = NULL;


static void     setup_sections();
static void     copy_sections();
static boolean verbose;

/* This flag distinguishes between strip and objcopy:
   1 means this is 'strip'; 0 means this is 'objcopy'.
   -1 means if we should use argv[0] to decide. */
extern int is_strip;

int		  show_version = 0;

enum strip_action
{
  strip_undef,
  strip_none,			/* don't strip */
  strip_debug,			/* strip all debugger symbols */
  strip_all			/* strip all symbols */
};

/* Which symbols to remove. */
enum strip_action strip_symbols;

enum locals_action
{
  locals_undef,
  locals_start_L,		/* discard locals starting with L */
  locals_all			/* discard all locals */
};

/* Which local symbols to remove. */
enum locals_action discard_locals;

/* Options to handle if running as "strip". */

struct option strip_options[] = {
    {"strip-all",	no_argument, 0, 's'},
    {"strip-debug",	no_argument, 0, 'S'},
    {"discard-all",	no_argument, 0, 'x'},
    {"discard-locals",	no_argument, 0, 'X'},
    {"help",		no_argument, 0, 'h'},
    {"input-format",	required_argument, 0, 'I'},
    {"output-format",	required_argument, 0, 'O'},
    {"format",		required_argument, 0, 'F'},
    {"target",		required_argument, 0, 'F'},

    {"version",         no_argument, 0, 'V'},
    {"verbose",         no_argument, 0, 'v'},
    {0, no_argument, 0, 0}
};

/* Options to handle if running as "objcopy". */

struct option copy_options[] = {
    {"strip-all",	no_argument, 0, 'S'},
    {"strip-debug",	no_argument, 0, 'g'},
    {"discard-all",	no_argument, 0, 'x'},
    {"discard-locals",	no_argument, 0, 'X'},
    {"help",		no_argument, 0, 'h'},
    {"input-format",	required_argument, 0, 'I'},
    {"output-format",	required_argument, 0, 'O'},
    {"format",		required_argument, 0, 'F'},
    {"target",		required_argument, 0, 'F'},
    
    {"version",         no_argument, 0, 'V'},
    {"verbose",         no_argument, 0, 'v'},
    {0,			no_argument, 0, 0}
};

/* IMPORTS */
extern char    *program_name;
extern char    *program_version;


static
void            
copy_usage(stream, status)
     FILE *stream;
     int status;
{
    fprintf(stream, "\
Usage: %s [-vVSgxX] [-I format] [-O format] [-F format]\n\
       [--format=format] [--target=format] [--input-format=format]\n\
       [--output-format=format] [--strip-all] [--strip-debug]\n\
       [--discard-all] [--discard-locals] [--verbose] [--version]\n\
       [--help] in-file [out-file]\n", program_name);
    exit(status);
}

static
void            
strip_usage(stream, status)
     FILE *stream;
     int status;
{
    fprintf(stream, "\
Usage: %s [-vVsSgxX] [-I format] [-O format] [-F format]\n\
       [--format=format] [--target=format] [--input-format=format]\n\
       [--output-format=format] [--strip-all] [--strip-debug] [--discard-all]\n\
       [--discard-locals] [--verbose] [--version] [--help] file...\n",
	    program_name);
    exit(status);
}


/* Create a temp file in the same directory as supplied */
static
char *
make_tempname(filename)
char *filename;
{
    static char template[] = "stXXXXXX";
    char *tmpname;
    char *      slash = strrchr( filename, '/' );
    if (slash != (char *)NULL){
	*slash = 0;
	tmpname = xmalloc(strlen(filename) + sizeof(template) + 1 );
	strcpy(tmpname, filename);
	strcat(tmpname, "/" );
	strcat(tmpname, template);
	mktemp(tmpname );
	*slash = '/';
    } else {
	tmpname = xmalloc(sizeof(template));
	strcpy(tmpname, template);
	mktemp(tmpname);
    }
    return tmpname;
}

/*
   All the symbols have been read in and point to their owning input section.
   They have been relocated to that they are all relative to the base of
   their owning section. On the way out, all the symbols will be relocated to
   their new location in the output file, through some complex sums.

*/
static void
mangle_sections(ibfd, obfd)
    bfd            *ibfd;
    bfd            *obfd;
{
    asection       *current = ibfd->sections;
    for (; current != NULL; current = current->next) {
	current->output_section = bfd_get_section_by_name(obfd, current->name);
	current->output_offset = 0;
    }
}

/* Choose which symbol entries to copy; put the result in osyms.
   We don't copy in place, because that confuses the relocs.
   Return the number of symbols to be printed.  */
static unsigned int
filter_symbols (abfd, osyms, isyms, symcount)
     bfd *abfd;
     asymbol **osyms, **isyms;
     unsigned long symcount;
{
  register asymbol **from = isyms, **to = osyms;
  unsigned int dst_count = 0;
  asymbol *sym;
  char locals_prefix = bfd_get_symbol_leading_char(abfd) == '_' ? 'L' : '.';

  unsigned int src_count = 0;
  for (; src_count <symcount; src_count++) {
    int keep = 0;

    flagword flags = (from[src_count])->flags;
    sym = from[src_count];
    if ((flags & BSF_GLOBAL) /* Keep if external */
	|| (sym->section == &bfd_und_section)
	|| (bfd_is_com_section (sym->section)))
	keep = 1;
    else if ((flags & BSF_DEBUGGING) != 0) /* debugging symbol */
	keep = strip_symbols != strip_debug;
    else /* local symbol */
	keep = (discard_locals != locals_all)
                && !(discard_locals == locals_start_L &&
                     sym->name[0] == locals_prefix);


    if (keep) {
      to[dst_count++] = from[src_count];
    }
  }

  return dst_count;
}

static void
copy_object(ibfd, obfd)
bfd *ibfd;
bfd *obfd;
{

    unsigned int symcount;


    if (!bfd_set_format(obfd, bfd_get_format(ibfd)))
	bfd_fatal(output_filename);


    if (verbose)
	printf("copy from %s(%s) to %s(%s)\n",
	       ibfd->filename, ibfd->xvec->name,
	       obfd->filename, obfd->xvec->name);

    if (! bfd_set_start_address (obfd, bfd_get_start_address (ibfd))
	|| ! bfd_set_file_flags (obfd,
				 (bfd_get_file_flags (ibfd)
				  & bfd_applicable_file_flags (obfd))))
      bfd_fatal (bfd_get_filename (ibfd));

    /* Copy architecture of input file to output file */
    if (!bfd_set_arch_mach(obfd, bfd_get_arch(ibfd),
			   bfd_get_mach(ibfd))) {
	fprintf(stderr, "Output file cannot represent architecture %s\n",
		bfd_printable_arch_mach(bfd_get_arch(ibfd),
					bfd_get_mach(ibfd)));
    }
    if (!bfd_set_format(obfd, bfd_get_format(ibfd)))
	{
	    bfd_fatal(ibfd->filename);
	}

    if (isympp)
      free (isympp);
    if (osympp != isympp)
      free (osympp);

    if (strip_symbols == strip_all && discard_locals == locals_undef)
      {
	osympp = isympp = NULL;
	symcount = 0;
      }
    else
      {
	osympp = isympp = (asymbol **) xmalloc(get_symtab_upper_bound(ibfd));
	symcount = bfd_canonicalize_symtab(ibfd, isympp);

	if (strip_symbols == strip_debug || discard_locals != locals_undef)
	  {
	    osympp = (asymbol **) xmalloc(symcount * sizeof(asymbol*));
	    symcount = filter_symbols (ibfd, osympp, isympp, symcount);
	  }
      }

    bfd_set_symtab(obfd, osympp, symcount);
    
    /*
      bfd mandates that all output sections be created and sizes set before
      any output is done.  Thus, we traverse all sections twice.
      */
    bfd_map_over_sections(ibfd, setup_sections, (void *) obfd);
    bfd_map_over_sections(ibfd, copy_sections, (void *) obfd);
    mangle_sections(ibfd, obfd);
}
static
char *
cat(a,b,c)
char *a;
char *b;
char *c;
{
    int size = strlen(a) + strlen(b) + strlen(c);
    char *r = xmalloc(size+1);
    strcpy(r,a);
    strcat(r,b);
    strcat(r,c);
    return r;
}

static void 
copy_archive(ibfd, obfd)
bfd *ibfd;
bfd *obfd;
{
    bfd **ptr = &obfd->archive_head;
    bfd *this_element;
    /* Read each archive element in turn from the input, copy the
       contents to a temp file, and keep the temp file handle */
    char *dir = cat("./#",make_tempname(""),"cd");

    /* Make a temp directory to hold the contents */
    mkdir(dir,0777);
    obfd->has_armap = ibfd->has_armap;
    this_element = bfd_openr_next_archived_file(ibfd, NULL);
    ibfd->archive_head = this_element;
    while (this_element != (bfd *)NULL) {

	/* Create an output file for this member */
	char *output_name = cat(dir, "/",this_element->filename);
	bfd *output_bfd = bfd_openw(output_name, output_target);

	if (!bfd_set_format(obfd, bfd_get_format(ibfd)))
	    bfd_fatal(output_filename);

	if (output_bfd == (bfd *)NULL) {
	    bfd_fatal(output_name);
	}
	if (bfd_check_format(this_element, bfd_object) == true) {
	    copy_object(this_element, output_bfd);
	}

	bfd_close(output_bfd);
	/* Now open the newly output file and attatch to our list */
	output_bfd = bfd_openr(output_name, output_target);
	/* Mark it for deletion */

	*ptr = output_bfd;

	ptr = &output_bfd->next;
	this_element->next = bfd_openr_next_archived_file(ibfd, this_element);
	this_element = this_element->next;

    }
    *ptr = (bfd *)NULL;

    if (!bfd_close(obfd))
	bfd_fatal(output_filename);

    /* Now delete all the files that we opened.
       Construct their names again, unfortunately, but so what;
       we're about to exit anyway. */
    for (this_element = ibfd->archive_head;
	 this_element != (bfd *)NULL;
	 this_element = this_element->next) 
	{
	unlink(cat(dir,"/",this_element->filename));
    }
    rmdir(dir);
    if (!bfd_close(ibfd))
	bfd_fatal(input_filename);

}

static
void
copy_file(input_filename, output_filename)
    char           *input_filename;
    char           *output_filename;
{
  bfd            *ibfd;

  /* To allow us to do "strip *" without dying on the first
     non-object file, failures are nonfatal.  */

  ibfd = bfd_openr(input_filename, input_target);
  if (ibfd == NULL)
    {
      bfd_perror(input_filename);
      return;
    }

  if (bfd_check_format(ibfd, bfd_object)) {
    bfd * obfd = bfd_openw(output_filename, output_target);
    if (obfd == NULL)
      {
	bfd_perror(output_filename);
	return;
      }

    copy_object(ibfd, obfd);

    if (!bfd_close(obfd))
      {
	bfd_perror(output_filename);
	return;
      }

    if (!bfd_close(ibfd))
      {
	bfd_perror(input_filename);
	return;
      }
  }
  else if (bfd_check_format(ibfd, bfd_archive)) {
    bfd * obfd = bfd_openw(output_filename, output_target);
    if (obfd == NULL)
      {
	bfd_perror(output_filename);
	return;
      }
    copy_archive(ibfd, obfd);
  }
  else {
    /* Get the right error message.  */
    (void) bfd_check_format (ibfd, bfd_object);
    bfd_perror (input_filename);
  }
}



/** Actually do the work */
static void
setup_sections(ibfd, isection, obfd)
    bfd            *ibfd;
    sec_ptr         isection;
    bfd            *obfd;
{
    sec_ptr         osection;
    char           *err;

    if ((bfd_get_section_flags (ibfd, isection) & SEC_DEBUGGING) != 0
	&& (strip_symbols == strip_debug
	    || strip_symbols == strip_all
	    || discard_locals == locals_all))
      return;

    osection = bfd_get_section_by_name(obfd, bfd_section_name(ibfd, isection));
    if (osection == NULL) {
	osection = bfd_make_section(obfd, bfd_section_name(ibfd, isection));
	if (osection == NULL) {
	    err = "making";
	    goto loser;
	}
    }

    if (!bfd_set_section_size(obfd,
			      osection,
			      bfd_section_size(ibfd, isection))) {
	err = "size";
	goto loser;
    }

    if (bfd_set_section_vma(obfd,
			    osection,
			    bfd_section_vma(ibfd, isection))
	== false) {
	err = "vma";
	goto loser;
    }				/* on error */

    if (bfd_set_section_alignment(obfd,
				  osection,
				  bfd_section_alignment(ibfd, isection))
	== false) {
	err = "alignment";
	goto loser;
    }				/* on error */

    if (!bfd_set_section_flags(obfd, osection,
			       bfd_get_section_flags(ibfd, isection))) {
	err = "flags";
	goto loser;
    }

    /* All went well */
    return;

loser:
    fprintf(stderr, "%s: file \"%s\", section \"%s\": error in %s: %s\n",
	    program_name,
	    bfd_get_filename(ibfd), bfd_section_name(ibfd, isection),
	    err, bfd_errmsg(bfd_error));
    exit(1);
}				/* setup_sections() */

/*
Copy all the section related data from an input section
to an output section

If stripping then don't copy any relocation info
*/
static void
copy_sections(ibfd, isection, obfd)
    bfd            *ibfd;
    sec_ptr         isection;
    bfd            *obfd;
{

  arelent       **relpp;
  int             relcount;
  sec_ptr         osection;
  bfd_size_type   size;

  if ((bfd_get_section_flags (ibfd, isection) & SEC_DEBUGGING) != 0
      && (strip_symbols == strip_debug
	  || strip_symbols == strip_all
	  || discard_locals == locals_all))
    return;

  osection = bfd_get_section_by_name(obfd,
				     bfd_section_name(ibfd, isection));

  size = bfd_get_section_size_before_reloc(isection);

  if (size == 0)
    return;

  if (strip_symbols == strip_all
      || bfd_get_reloc_upper_bound(ibfd, isection) == 0) 
    {
      bfd_set_reloc(obfd, osection, (arelent **)NULL, 0);
    } 
  else 
    {
      relpp = (arelent **) xmalloc(bfd_get_reloc_upper_bound(ibfd, isection));
      relcount = bfd_canonicalize_reloc(ibfd, isection, relpp, isympp);
      bfd_set_reloc(obfd, osection, relpp, relcount);
    }

  isection->_cooked_size = isection->_raw_size;
  isection->reloc_done =true;
  

  if (bfd_get_section_flags(ibfd, isection) & SEC_HAS_CONTENTS) 
    {
      PTR memhunk = (PTR) xmalloc((unsigned)size);

      if (!bfd_get_section_contents(ibfd, isection, memhunk, (file_ptr) 0, size))
	bfd_fatal(bfd_get_filename(ibfd));

      if (!bfd_set_section_contents(obfd, osection, memhunk, (file_ptr)0, size))
	bfd_fatal(bfd_get_filename(obfd));
      free(memhunk);
    }


}
int
main(argc, argv)
    int             argc;
    char           *argv[];
{
  int             i;
  int c;			/* sez which option char */

  program_name = argv[0];

  strip_symbols = strip_undef;	/* default is to strip everything.  */
  discard_locals = locals_undef;

  bfd_init();

  if (is_strip < 0) {
      i = strlen (program_name);
      is_strip = (i >= 5 && strcmp(program_name+i-5,"strip"));
  }

  if (is_strip) {
    
      while ((c = getopt_long(argc, argv, "I:O:F:sSgxXVv",
			      strip_options, (int *) 0))
	     != EOF) {
	  switch (c) {
	    case 'I':
	      input_target = optarg;
	    case 'O':
	      output_target = optarg;
	      break;
	    case 'F':
	      input_target = output_target = optarg;
	      break;

	    case 's':
	      strip_symbols = strip_all;
	      break;
	    case 'S':
	    case 'g':
	      strip_symbols = strip_debug;
	      break;
	    case 'x':
	      discard_locals = locals_all;
	      break;
	    case 'X':
	      discard_locals = locals_start_L;
	      break;
	    case 'v':
	      verbose = true;
	      break;
	    case 'V':
	      show_version = true;
	      break;
	    case  0:
	      break;		/* we've been given a long option */
	    case 'h':
	      strip_usage (stdout, 0);
	    default:
	      strip_usage (stderr, 1);
	  }
      }
      
      i = optind;

      /* Default is to strip all symbols.  */
      if (strip_symbols == strip_undef && discard_locals == locals_undef)
	  strip_symbols = strip_all;

      if (output_target == (char *) NULL)
	  output_target = input_target;

      if (show_version) {
	  printf ("GNU %s version %s\n", program_name, program_version);
	  exit (0);
	}
      else if (i == argc)
	  strip_usage(stderr, 1);
      for ( ; i < argc; i++) {
	    char *tmpname = make_tempname(argv[i]);
	    copy_file(argv[i], tmpname);
	    smart_rename(tmpname, argv[i]);
      }
      return 0;
    }

  /* Invoked as "objcopy", not "strip" */

  while ((c = getopt_long(argc, argv, "I:s:O:d:F:b:SgxXVv",
			  strip_options, (int *) 0))
	 != EOF) {
      switch (c) {
	case 'I':
	case 's': /* "source" - 'I' is preferred */
	  input_target = optarg;
	case 'O':
	case 'd': /* "destination" - 'O' is preferred */
	  output_target = optarg;
	  break;
	case 'F':
	case 'b': /* "both" - 'F' is preferred */
	  input_target = output_target = optarg;
	  break;
	  
	case 'S':
	  strip_symbols = strip_all;
	  break;
	case 'g':
	  strip_symbols = strip_debug;
	  break;
	case 'x':
	  discard_locals = locals_all;
	  break;
	case 'X':
	  discard_locals = locals_start_L;
	  break;
	case 'v':
	  verbose = true;
	  break;
	case 'V':
	  show_version = true;
	  break;
	case  0:
	  break;		/* we've been given a long option */
	case 'h':
	  copy_usage (stdout, 0);
	default:
	  copy_usage (stderr, 1);
      }
  }
      
  if (show_version) {
    printf ("GNU %s version %s\n", program_name, program_version);
    exit (0);
  }

  if (optind == argc || optind + 2 < argc)
    copy_usage(stderr, 1);

  input_filename = argv[optind];
  if (optind + 1 < argc)
      output_filename = argv[optind+1];

  /* Default is to strip no symbols.  */
  if (strip_symbols == strip_undef && discard_locals == locals_undef)
      strip_symbols = strip_none;

  if (input_filename == (char *) NULL)
    copy_usage(stderr, 1);

  if (output_target == (char *) NULL)
    output_target = input_target;

  /* If there is no  destination file then create a temp and rename
     the result into the input */

  if (output_filename == (char *)NULL) {
    char *	tmpname = make_tempname(input_filename);
    copy_file(input_filename, tmpname);
    output_filename = input_filename;
    smart_rename(tmpname, input_filename);
  }
  else {
    copy_file(input_filename, output_filename);
  }
  return 0;
}

#ifndef S_ISLNK
#ifdef S_IFLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#else
#define S_ISLNK(m) 0
#define lstat stat
#endif
#endif

/* Rename FROM to TO, copying if TO is a symlink.
   Return 0 if ok, -1 if error.  */

int
smart_rename (from, to)
     char *from, *to;
{
  struct stat s;

  if (lstat(to, &s))
    return -1;
  if (!S_ISLNK(s.st_mode))
    return rename(from, to);
  else
    return simple_copy(from, to);
}

/* The number of bytes to copy at once.  */
#define COPY_BUF 8192

/* Copy file FROM to file TO, performing no translations.
   Return 0 if ok, -1 if error.  */

int
simple_copy (from, to)
     char *from, *to;
{
  int fromfd, tofd, nread;
  char buf[COPY_BUF];

  fromfd = open(from, O_RDONLY);
  if (fromfd < 0)
    return -1;
  tofd = open(to, O_WRONLY|O_CREAT|O_TRUNC);
  if (tofd < 0)
    {
      close(fromfd);
      return -1;
    }
  while ((nread = read(fromfd, buf, sizeof buf)) > 0)
    {
      if (write(tofd, buf, nread) != nread)
	{
	  close(fromfd);
	  close(tofd);
	  return -1;
	}
    }
  close (fromfd);
  close (tofd);
  if (nread < 0)
    return -1;
  return 0;
}
