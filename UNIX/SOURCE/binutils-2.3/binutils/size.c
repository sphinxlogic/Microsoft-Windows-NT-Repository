/* size.c -- report size of various sections of an executable file.
   Copyright 1991, 1992 Free Software Foundation, Inc.

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


/* Extensions/incompatibilities:
   o - BSD output has filenames at the end.
   o - BSD output can appear in different radicies.
   o - SysV output has less redundant whitespace.  Filename comes at end.
   o - SysV output doesn't show VMA which is always the same as the PMA.
   o - We also handle core files.
   o - We also handle archives.
   If you write shell scripts which manipulate this info then you may be
   out of luck; there's no --predantic option.
*/

#include "bfd.h"
#include "sysdep.h"
#include "getopt.h"
#include "bucomm.h"

#ifndef BSD_DEFAULT
#define BSD_DEFAULT 1
#endif

/* Various program options */

enum {decimal, octal, hex} radix = decimal;
int berkeley_format = BSD_DEFAULT; /* 0 means use AT&T-style output */
int show_version = 0;
int show_help = 0;

int return_code = 0;

/* IMPORTS */
extern char *program_version;
extern char *target;

/* Forward declarations */

static void
display_file PARAMS ((char *filename));

static void
print_sizes PARAMS ((bfd *file));

static void
berkeley_sum PARAMS ((bfd *, sec_ptr, PTR));

/** main and like trivia */

void
usage (stream, status)
     FILE *stream;
     int status;
{
  fprintf (stream, "\
Usage: %s [-ABdoxV] [--format=berkeley|sysv] [--radix=8|10|16]\n\
       [--target=bfdname] [--version] [--help] [file...]\n", program_name);
#if BSD_DEFAULT
  fputs ("       (default is --format=berkeley)\n", stream);
#else
  fputs ("       (default is --format=sysv)\n", stream);
#endif
  exit (status);
}

struct option long_options[] = {
  {"format",  required_argument, 0, 200},
  {"radix",   required_argument, 0, 201},
  {"target",  required_argument, 0, 202},
  {"version", no_argument, &show_version, 1},
  {"help",    no_argument, &show_help, 1},
  {0, no_argument, 0, 0}
};

int
main (argc, argv)
     int argc;
     char **argv;
{
  int temp;
  int c;			/* sez which option char */
  extern int optind;		/* steps thru options */

  program_name = *argv;

  bfd_init();

  while ((c = getopt_long(argc, argv, "ABVdox", long_options,
			  (int *) 0)) != EOF)
    switch(c) {
    case 200:			/* --format */
	switch(*optarg) {
	case 'B': case 'b': berkeley_format = 1; break;
	case 'S': case 's': berkeley_format = 0; break;
	default: fprintf(stderr, "invalid argument to --format: %s\n", optarg);
	  usage(stderr, 1);
	}
	break;

      case 202:			/* --target */
	target = optarg;
	break;

      case 201:			/* --radix */
#ifdef ANSI_LIBRARIES
	temp = strtol(optarg, NULL, 10);
#else
	temp = atol(optarg);
#endif
	switch(temp) {
	case 10: radix = decimal; break;
	case 8:  radix = octal; break;
	case 16: radix = hex; break;
	default: printf("Unknown radix: %s\n", optarg);
	  usage(stderr, 1);
	}
	break;

    case 'A': berkeley_format = 0; break;
    case 'B': berkeley_format = 1; break;
    case 'V': show_version = 1; break;
    case 'd': radix = decimal; break;
    case 'x': radix = hex; break;
    case 'o': radix = octal; break;
    case 0: break;
    case '?': usage(stderr, 1);
    }

  if (show_version) {
    printf("GNU %s version %s\n", program_name, program_version);
    exit (0);
  }
  if (show_help)
    usage(stdout, 0);
	
  if (optind == argc)
    display_file ("a.out");
  else
    for (; optind < argc;)
      display_file (argv[optind++]);

  return return_code;
}

/** Display a file's stats */

void
display_bfd (abfd)
     bfd *abfd;
{
  CONST  char *core_cmd;

  if (bfd_check_format(abfd, bfd_archive)) return;

  if (bfd_check_format(abfd, bfd_object)) {
    print_sizes(abfd);
    goto done;
  }

  if (bfd_check_format(abfd, bfd_core)) {
    print_sizes(abfd);
    fputs(" (core file", stdout);

    core_cmd = bfd_core_file_failing_command(abfd);
    if (core_cmd) printf(" invoked as %s", core_cmd);

    puts(")");
    goto done;
  }
  
  printf("Unknown file format: %s.", bfd_get_filename(abfd));
  return_code = 3;

 done:


  printf("\n");
  return;
}

static void
display_file(filename)
     char *filename;
{
  bfd *file, *arfile = (bfd *) NULL;

  file = bfd_openr (filename, target);
  if (file == NULL) {
    fprintf (stderr, "%s: ", program_name);
    bfd_perror (filename);
    return_code = 1;
    return;
  }

  if (bfd_check_format(file, bfd_archive) == true) {
    for(;;) {
      
      bfd_error = no_error;

       arfile = bfd_openr_next_archived_file (file, arfile);
      if (arfile == NULL) {
	if (bfd_error != no_more_archived_files) {
	  fprintf (stderr, "%s: ", program_name);
	  bfd_perror (bfd_get_filename (file));
	  return_code = 2;
        }
	return;
      }

      display_bfd (arfile);
      /* Don't close the archive elements; we need them for next_archive */
    }
  }
  else
    display_bfd (file);

  bfd_close (file);
}

/* This is what lexical functions are for */
void
lprint_number (width, num)
     int width;
     bfd_size_type num;
{
  printf ((radix == decimal ? "%-*lu\t" :
	   ((radix == octal) ? "%-*lo\t" : "%-*lx\t")),
	  width, (unsigned long)num);
}

void
rprint_number(width, num)
     int width;
     bfd_size_type num;
{
  printf ((radix == decimal ? "%*lu\t" :
	   ((radix == octal) ? "%*lo\t" : "%*lx\t")),
	  width, (unsigned long)num);
}

static bfd_size_type bsssize;
static bfd_size_type datasize;
static bfd_size_type textsize;

static void
berkeley_sum (abfd, sec, ignore)
     bfd *abfd;
     sec_ptr sec;
     PTR ignore;
{
  bfd_size_type size;

  size = bfd_get_section_size_before_reloc (sec);
  if (bfd_get_section_flags (abfd, sec) & SEC_CODE)
    textsize += size;
  else if (bfd_get_section_flags (abfd, sec) & SEC_DATA)
    datasize += size;
  else if (bfd_get_section_flags (abfd, sec) & SEC_ALLOC)
    bsssize += size;
}

void print_berkeley_format(abfd)
bfd *abfd;
{
  static int files_seen = 0;
  bfd_size_type total;

  bsssize = 0;
  datasize = 0;
  textsize = 0;

  bfd_map_over_sections (abfd, berkeley_sum, (PTR) NULL);

  if (files_seen++ == 0)
#if 0	/* intel doesn't like bss/stk b/c they don't gave core files */
    puts((radix == octal) ? "text\tdata\tbss/stk\toct\thex\tfilename" :
	 "text\tdata\tbss/stk\tdec\thex\tfilename");
#else
    puts((radix == octal) ? "text\tdata\tbss\toct\thex\tfilename" :
	 "text\tdata\tbss\tdec\thex\tfilename");
#endif
	
  total = textsize + datasize + bsssize;
	
  lprint_number (7, textsize);
  lprint_number (7, datasize);
  lprint_number (7, bsssize);
  printf (((radix == octal) ? "%-7lo\t%-7lx\t" : "%-7lu\t%-7lx\t"),
	  (unsigned long)total, (unsigned long)total);

  fputs(bfd_get_filename(abfd), stdout);
  if (abfd->my_archive) printf (" (ex %s)", abfd->my_archive->filename);
}

/* I REALLY miss lexical functions! */
bfd_size_type svi_total = 0;

void
sysv_internal_printer(file, sec, ignore)
     bfd *file;
     sec_ptr sec;
     PTR ignore;
{
  bfd_size_type size = bfd_section_size (file, sec);
  if (sec!= &bfd_abs_section 
      && ! bfd_is_com_section (sec)
      && sec!=&bfd_und_section) 
  {
  
    svi_total += size;
	
    printf ("%-12s", bfd_section_name(file, sec));
    rprint_number (8, size);
    printf(" ");
    rprint_number (8, bfd_section_vma(file, sec));
    printf ("\n");
  }

}

void
print_sysv_format(file)
     bfd *file;
{
  svi_total = 0;

  printf ("%s  ", bfd_get_filename (file));
  if (file->my_archive) printf (" (ex %s)", file->my_archive->filename);

  puts(":\nsection\t\tsize\t     addr");
  bfd_map_over_sections (file, sysv_internal_printer, (PTR)NULL);

  printf("Total       ");
  rprint_number(8, svi_total);
  printf("\n");  printf("\n");
}

static void
print_sizes(file)
     bfd *file;
{
  if (berkeley_format)
    print_berkeley_format(file);
  else print_sysv_format(file);
}
