/* Shared definitions for GNU DIFF
   Copyright 1988, 89, 91, 92, 93, 94, 95, 1998 Free Software Foundation, Inc.

   This file is part of GNU DIFF.

   GNU DIFF is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU DIFF is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "system.h"
#include <stdio.h>
#include "regex.h"

#define TAB_WIDTH 8

/* Variables for command line options */

#ifndef GDIFF_MAIN
# define XTERN extern
#else
# define XTERN
#endif

enum output_style {
  /* Default output style.  */
  OUTPUT_NORMAL,
  /* Output the differences with lines of context before and after (-c).  */
  OUTPUT_CONTEXT,
  /* Output the differences in a unified context diff format (-u).  */
  OUTPUT_UNIFIED,
  /* Output the differences as commands suitable for `ed' (-e).  */
  OUTPUT_ED,
  /* Output the diff as a forward ed script (-f).  */
  OUTPUT_FORWARD_ED,
  /* Like -f, but output a count of changed lines in each "command" (-n).  */
  OUTPUT_RCS,
  /* Output merged #ifdef'd file (-D).  */
  OUTPUT_IFDEF,
  /* Output sdiff style (-y).  */
  OUTPUT_SDIFF
};

/* True for output styles that are robust,
   i.e. can handle a file that ends in a non-newline.  */
#define ROBUST_OUTPUT_STYLE(S) ((S) != OUTPUT_ED && (S) != OUTPUT_FORWARD_ED)

XTERN enum output_style output_style;

/* Nonzero if output cannot be generated for identical files.  */
XTERN int no_diff_means_no_output;

/* Number of lines of context to show in each set of diffs.
   This is zero when context is not to be shown.  */
XTERN int context;

/* Consider all files as text files (-a).
   Don't interpret codes over 0177 as implying a "binary file".  */
XTERN int always_text_flag;

/* Number of lines to keep in identical prefix and suffix.  */
XTERN int horizon_lines;

/* Ignore changes in horizontal white space (-b).  */
XTERN int ignore_space_change_flag;

/* Ignore all horizontal white space (-w).  */
XTERN int ignore_all_space_flag;

/* Ignore changes that affect only blank lines (-B).  */
XTERN int ignore_blank_lines_flag;

/* 1 if files may match even if their contents are not byte-for-byte identical.
   This depends on various options.  */
XTERN int ignore_some_changes;

/* Ignore differences in case of letters (-i).  */
XTERN int ignore_case_flag;

/* File labels for `-c' output headers (-L).  */
XTERN char *file_label[2];

/* Regexp to identify function-header lines (-F).  */
XTERN struct re_pattern_buffer function_regexp;

/* Ignore changes that affect only lines matching this regexp (-I).  */
XTERN struct re_pattern_buffer ignore_regexp;

/* Say only whether files differ, not how (-q).  */
XTERN int no_details_flag;

/* Output the differences with exactly 8 columns added to each line
   so that any tabs in the text line up properly (-T).  */
XTERN int tab_align_flag;

/* Expand tabs in the output so the text lines up properly
   despite the characters added to the front of each line (-t).  */
XTERN int tab_expand_flag;

/* In directory comparison, specify file to start with (-S).
   All file names less than this name are ignored.  */
XTERN char *dir_start_file;

/* Pipe each file's output through pr (-l).  */
XTERN int paginate_flag;

enum line_class {
  /* Lines taken from just the first file.  */
  OLD,
  /* Lines taken from just the second file.  */
  NEW,
  /* Lines common to both files.  */
  UNCHANGED,
  /* A hunk containing both old and new lines (line groups only).  */
  CHANGED
};

/* Line group formats for old, new, unchanged, and changed groups.  */
XTERN char *group_format[CHANGED + 1];

/* Line formats for old, new, and unchanged lines.  */
XTERN char *line_format[UNCHANGED + 1];

/* If using OUTPUT_SDIFF print extra information to help the sdiff filter.  */
XTERN int sdiff_help_sdiff;

/* Tell OUTPUT_SDIFF to show only the left version of common lines.  */
XTERN int sdiff_left_only;

/* Tell OUTPUT_SDIFF to not show common lines.  */
XTERN int sdiff_skip_common_lines;

/* The half line width and column 2 offset for OUTPUT_SDIFF.  */
XTERN unsigned sdiff_half_width;
XTERN unsigned sdiff_column2_offset;

/* String containing all the command options diff received,
   with spaces between and at the beginning but none at the end.
   If there were no options given, this string is empty.  */
XTERN char *switch_string;

/* Nonzero means use heuristics for better speed.  */
XTERN int heuristic;

/* Nonzero means don't merge hunks.  */
XTERN int inhibit_hunk_merge;

/* For debugging: don't do discard_confusing_lines.  */
XTERN int no_discards;

/* Name of program the user invoked (for error messages).  */
XTERN char *program_name;

/* The result of comparison is an "edit script": a chain of `struct change'.
   Each `struct change' represents one place where some lines are deleted
   and some are inserted.

   LINE0 and LINE1 are the first affected lines in the two files (origin 0).
   DELETED is the number of lines deleted here from file 0.
   INSERTED is the number of lines inserted here in file 1.

   If DELETED is 0 then LINE0 is the number of the line before
   which the insertion was done; vice versa for INSERTED and LINE1.  */

struct change
{
  struct change *link;		/* Previous or next edit command  */
  int inserted;			/* # lines of file 1 changed here.  */
  int deleted;			/* # lines of file 0 changed here.  */
  int line0;			/* Line number of 1st deleted line.  */
  int line1;			/* Line number of 1st inserted line.  */
  char ignore;			/* Flag used in context.c */
};

/* Structures that describe the input files.  */

/* Data on one input file being compared.  */

struct file_data {
    int             desc;	/* File descriptor  */
    char const      *name;	/* File name  */
    struct stat     stat;	/* File status */

    /* Buffer in which text of file is read.  */
    char *	    buffer;
    /* Allocated size of buffer.  */
    size_t	    bufsize;
    /* Number of valid characters now in the buffer.  */
    size_t	    buffered_chars;

    /* Array of pointers to lines in the file.  */
    char const **linbuf;

    /* linbuf_base <= buffered_lines <= valid_lines <= alloc_lines.
       linebuf[linbuf_base ... buffered_lines - 1] are possibly differing.
       linebuf[linbuf_base ... valid_lines - 1] contain valid data.
       linebuf[linbuf_base ... alloc_lines - 1] are allocated.  */
    int linbuf_base, buffered_lines, valid_lines, alloc_lines;

    /* Pointer to end of prefix of this file to ignore when hashing.  */
    char const *prefix_end;

    /* Count of lines in the prefix.
       There are this many lines in the file before linbuf[0].  */
    int prefix_lines;

    /* Pointer to start of suffix of this file to ignore when hashing.  */
    char const *suffix_begin;

    /* Vector, indexed by line number, containing an equivalence code for
       each line.  It is this vector that is actually compared with that
       of another file to generate differences.  */
    int		   *equivs;

    /* Vector, like the previous one except that
       the elements for discarded lines have been squeezed out.  */
    int		   *undiscarded;

    /* Vector mapping virtual line numbers (not counting discarded lines)
       to real ones (counting those lines).  Both are origin-0.  */
    int		   *realindexes;

    /* Total number of nondiscarded lines.  */
    int		    nondiscarded_lines;

    /* Vector, indexed by real origin-0 line number,
       containing 1 for a line that is an insertion or a deletion.
       The results of comparison are stored here.  */
    char	   *changed_flag;

    /* 1 if file ends in a line with no final newline.  */
    int		    missing_newline;

    /* 1 more than the maximum equivalence value used for this or its
       sibling file.  */
    int equiv_max;
};

/* Data on two input files being compared.  */

struct comparison
  {
    struct file_data file[2];
    struct comparison const *parent;  /* parent, if a recursive comparison */
  };

/* Describe the two files currently being compared.  */

XTERN struct file_data files[2];

/* Stdio stream to output diffs to.  */

XTERN FILE *outfile;

/* Declare various functions.  */

/* analyze.c */
int diff_2_files PARAMS((struct comparison *));

/* context.c */
void print_context_header PARAMS((struct file_data[], int));
void print_context_script PARAMS((struct change *, int));

/* diff.c */
int excluded_filename PARAMS((char const *));

/* dir.c */
int diff_dirs PARAMS((struct comparison const *, int (*) PARAMS((struct comparison const *, char const *, char const *))));

/* ed.c */
void print_ed_script PARAMS((struct change *));
void pr_forward_ed_script PARAMS((struct change *));

/* error.c */
#if __STDC__ && (HAVE_VPRINTF || HAVE_DOPRNT)
void error (int, int, char const *, ...) __attribute__((format (printf, 3, 4)));
#else
void error ();
#endif

/* freesoft.c */
extern char const free_software_msgid[];

/* ifdef.c */
void print_ifdef_script PARAMS((struct change *));

/* io.c */
int read_files PARAMS((struct file_data[], int));
int sip PARAMS((struct file_data *, int));
void slurp PARAMS((struct file_data *));

/* normal.c */
void print_normal_script PARAMS((struct change *));

/* rcs.c */
void print_rcs_script PARAMS((struct change *));

/* side.c */
void print_sdiff_script PARAMS((struct change *));

/* util.c */
extern char const pr_program[];
char *concat PARAMS((char const *, char const *, char const *));
char *dir_file_pathname PARAMS((char const *, char const *));
int change_letter PARAMS((int, int));
int line_cmp PARAMS((char const *, char const *));
int translate_line_number PARAMS((struct file_data const *, int));
struct change *find_change PARAMS((struct change *));
struct change *find_reverse_change PARAMS((struct change *));
void analyze_hunk PARAMS((struct change *, int *, int *, int *, int *, int *, int *));
void begin_output PARAMS((void));
void debug_script PARAMS((struct change *));
void fatal PARAMS((char const *)) __attribute__((noreturn));
void finish_output PARAMS((void));
void message PARAMS((char const *, char const *, char const *));
void message5 PARAMS((char const *, char const *, char const *, char const *, char const *));
void output_1_line PARAMS((char const *, char const *, char const *, char const *));
void perror_with_name PARAMS((char const *));
void pfatal_with_name PARAMS((char const *)) __attribute__((noreturn));
void print_1_line PARAMS((char const *, char const * const *));
void print_message_queue PARAMS((void));
void print_number_range PARAMS((int, struct file_data *, int, int));
void print_script PARAMS((struct change *, struct change * (*) PARAMS((struct change *)), void (*) PARAMS((struct change *))));
void setup_output PARAMS((char const *, char const *, int));
void translate_range PARAMS((struct file_data const *, int, int, int *, int *));

/* xmalloc.c */
VOID *xmalloc PARAMS((size_t));
VOID *xrealloc PARAMS((VOID *, size_t));
extern int xmalloc_exit_failure;

/* version.c */
extern char const version_string[];
