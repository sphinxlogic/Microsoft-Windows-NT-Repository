/*
 * Copyright © 1994, 2000 the Free Software Foundation, Inc.
 *
 * This file is a part of GNU VMSLIB, the GNU library for porting GNU
 * software to VMS.
 *
 * GNU VMSLIB is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU VMSLIB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU VMSLIB; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Revision history:
 *
 * 7-feb-2000	Craig A. Berry <craig.berry@metamorgs.com>
 *		- Removed private read() for 2.7.2 because block_read()
 *		  in io.c now already does what's necessary.
 *		- Implemented quote_system_arg().
 *		- Implemented my_exit().
 *		- Added VAXC$PATH code to vms_initialize_main.
 *
 * 15-jan-2000	Martin Vorlaender <mv@pdv-systeme.de>
 *		Inserted vms_initialize_main to implement I/O redirection.
 *
 * 4-jan-2000 	Craig A. Berry <craig.berry@metamorgs.com>
 *		Decided to put all VMS-specific routines in here, but only
 *		read() is needed at this point.
 */

/* standard includes */
#include <errno.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <unixio.h>
#include <varargs.h>

/* VMS-specific includes */
#include <descrip.h>
#include <fscndef.h>
#include <lib$routines.h>
#include <lnmdef.h>
#include <psldef.h>
#include <starlet.h>
#include <ssdef.h>
#include <stsdef.h>

/* diffutils-specific includes */
#include "quotesys.h"
#include "config.h"
#include "diff.h"

struct itmlst {
  unsigned short int buflen;
  unsigned short int itmcode;
  void *bufadr;
  unsigned short int *retlen;
  unsigned int terminator;
};

/* Macros to set errno using the VAX thread-safe calls, if present */
#if (defined(__DECC) || defined(__DECCXX)) && !defined(__ALPHA)
#  define set_errno(v)      (cma$tis_errno_set_value(v))
   void cma$tis_errno_set_value(int __value);  /* missing in some errno.h */
#  define set_vaxc_errno(v) (vaxc$errno = (v))
#else
#  define set_errno(v)      (errno = (v))
#  define set_vaxc_errno(v) (vaxc$errno = (v))
#endif

/* Handy way to vet calls to VMS system services and RTL routines. */
#define _ckvmssts(call) { \
  register unsigned long int __ckvms_sts; \
  if (!((__ckvms_sts=(call))&1)) { \
    set_errno(EVMSERR); \
    set_vaxc_errno(__ckvms_sts); \
    fprintf(stderr, "Fatal VMS error (status=%d) at %s, line %d", \
            __ckvms_sts,__FILE__,__LINE__); \
  } \
}

#define safemalloc malloc
#define saferealloc realloc

#define New(x,v,n,t)   (v = (t*)safemalloc((n)*sizeof(t)))
#define Renew(v,n,t)    (v = (t*)saferealloc((v),(n)*sizeof(t)))

struct itmlst_3 {
  unsigned short int buflen;
  unsigned short int itmcode;
  void *bufadr;
  unsigned short int *retlen;
};

static int my_trnlnm(char *lnm, char *eqv, unsigned long int idx)
{
    static char __my_trnlnm_eqv[LNM$C_NAMLENGTH+1];
    unsigned short int eqvlen;
    int retsts;
    unsigned int attr = LNM$M_CASE_BLIND;
    $DESCRIPTOR(tabdsc,"LNM$FILE_DEV");
    struct dsc$descriptor_s lnmdsc = {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,0};
    struct itmlst_3 lnmlst[3] = {{sizeof idx,      LNM$_INDEX,  0, 0},
                                 {LNM$C_NAMLENGTH, LNM$_STRING, 0, 0},
                                 {0, 0, 0, 0}};

    lnmlst[0].bufadr = (void *)&idx;
    lnmlst[1].retlen = &eqvlen;

    if (!lnm || idx > LNM$_MAX_INDEX) {
      set_errno(EINVAL); set_vaxc_errno(SS$_BADPARAM); return 0;
    }
    if (!eqv) eqv = __my_trnlnm_eqv;
    lnmlst[1].bufadr = (void *)eqv;
    lnmdsc.dsc$a_pointer = lnm;
    lnmdsc.dsc$w_length = strlen(lnm);
    retsts = sys$trnlnm(&attr,&tabdsc,&lnmdsc,0,lnmlst);
    if (retsts == SS$_NOLOGNAM || retsts == SS$_IVLOGNAM) {
      set_vaxc_errno(retsts); set_errno(EINVAL); return 0;
    }
    else if (retsts & 1) {
      eqv[eqvlen] = '\0';
      return eqvlen;
    }
    _ckvmssts(retsts);  /* Must be an error */
    return 0;      /* Not reached, assuming _ckvmssts() bails out */
}

static char *do_tovmsspec(const char *path, char *buf, int ts) {
  static char __tovmsspec_retbuf[NAM$C_MAXRSS+1];
  char *rslt, *dirend;
  register char *cp1;
  register const char *cp2;
  unsigned long int infront = 0, hasdir = 1;

  if (path == NULL) return NULL;
  if (buf) rslt = buf;
  else if (ts) New(1316,rslt,strlen(path)+9,char);
  else rslt = __tovmsspec_retbuf;
  if (strpbrk(path,"]:>") ||
      (dirend = strrchr(path,'/')) == NULL) {
    if (path[0] == '.') {
      if (path[1] == '\0') strcpy(rslt,"[]");
      else if (path[1] == '.' && path[2] == '\0') strcpy(rslt,"[-]");
      else strcpy(rslt,path); /* probably garbage */
    }
    else strcpy(rslt,path);
    return rslt;
  }
  if (*(dirend+1) == '.') {  /* do we have trailing "/." or "/.." or "/..."? */
    if (!*(dirend+2)) dirend +=2;
    if (*(dirend+2) == '.' && !*(dirend+3)) dirend += 3;
    if (*(dirend+2) == '.' && *(dirend+3) == '.' && !*(dirend+4)) dirend += 4;
  }
  cp1 = rslt;
  cp2 = path;
  if (*cp2 == '/') {
    char trndev[NAM$C_MAXRSS+1];
    int islnm, rooted;
    size_t trnend;

    while (*(cp2+1) == '/') cp2++;  /* Skip multiple /s */
    if (!*(cp2+1)) {
      if (!buf & ts) Renew(rslt,18,char);
      strcpy(rslt,"sys$disk:[000000]");
      return rslt;
    }
    while (*(++cp2) != '/' && *cp2) *(cp1++) = *cp2;
    *cp1 = '\0';
    islnm =  my_trnlnm(rslt,trndev,0);
    trnend = islnm ? strlen(trndev) - 1 : 0;
    islnm =  trnend ? (trndev[trnend] == ']' || trndev[trnend] == '>') : 0;
    rooted = islnm ? (trndev[trnend-1] == '.') : 0;
    /* If the first element of the path is a logical name, determine
     * whether it has to be translated so we can add more directories. */
    if (!islnm || rooted) {
      *(cp1++) = ':';
      *(cp1++) = '[';
      if (cp2 == dirend) while (infront++ < 6) *(cp1++) = '0';
      else cp2++;
    }
    else {
      if (cp2 != dirend) {
        if (!buf && ts) Renew(rslt,strlen(path)-strlen(rslt)+trnend+4,char);
        strcpy(rslt,trndev);
        cp1 = rslt + trnend;
        *(cp1++) = '.';
        cp2++;
      }
      else {
        *(cp1++) = ':';
        hasdir = 0;
      }
    }
  }
  else {
    *(cp1++) = '[';
    if (*cp2 == '.') {
      if (*(cp2+1) == '/' || *(cp2+1) == '\0') {
        cp2 += 2;         /* skip over "./" - it's redundant */
        *(cp1++) = '.';   /* but it does indicate a relative dirspec */
      }
      else if (*(cp2+1) == '.' && (*(cp2+2) == '/' || *(cp2+2) == '\0')) {
        *(cp1++) = '-';                                 /* "../" --> "-" */
        cp2 += 3;
      }
      else if (*(cp2+1) == '.' && *(cp2+2) == '.' &&
               (*(cp2+3) == '/' || *(cp2+3) == '\0')) {
        *(cp1++) = '.'; *(cp1++) = '.'; *(cp1++) = '.'; /* ".../" --> "..." */
        if (!*(cp2+4)) *(cp1++) = '.'; /* Simulate trailing '/' for later */
        cp2 += 4;
      }
      if (cp2 > dirend) cp2 = dirend;
    }
    else *(cp1++) = '.';
  }
  for (; cp2 < dirend; cp2++) {
    if (*cp2 == '/') {
      if (*(cp2-1) == '/') continue;
      if (*(cp1-1) != '.') *(cp1++) = '.';
      infront = 0;
    }
    else if (!infront && *cp2 == '.') {
      if (cp2+1 == dirend || *(cp2+1) == '\0') { cp2++; break; }
      else if (*(cp2+1) == '/') cp2++;   /* skip over "./" - it's redundant */
      else if (*(cp2+1) == '.' && (*(cp2+2) == '/' || *(cp2+2) == '\0')) {
        if (*(cp1-1) == '-' || *(cp1-1) == '[') *(cp1++) = '-'; /* handle "../" */
        else if (*(cp1-2) == '[') *(cp1-1) = '-';
        else {  /* back up over previous directory name */
          cp1--;
          while (*(cp1-1) != '.' && *(cp1-1) != '[') cp1--;
          if (*(cp1-1) == '[') {
            memcpy(cp1,"000000.",7);
            cp1 += 7;
          }
        }
        cp2 += 2;
        if (cp2 == dirend) break;
      }
      else if ( *(cp2+1) == '.' && *(cp2+2) == '.' &&
                (*(cp2+3) == '/' || *(cp2+3) == '\0') ) {
        if (*(cp1-1) != '.') *(cp1++) = '.'; /* May already have 1 from '/' */
        *(cp1++) = '.'; *(cp1++) = '.'; /* ".../" --> "..." */
        if (!*(cp2+3)) {
          *(cp1++) = '.';  /* Simulate trailing '/' */
          cp2 += 2;  /* for loop will incr this to == dirend */
        }
        else cp2 += 3;  /* Trailing '/' was there, so skip it, too */
      }
      else *(cp1++) = '_';  /* fix up syntax - '.' in name not allowed */
    }
    else {
      if (!infront && *(cp1-1) == '-')  *(cp1++) = '.';
      if (*cp2 == '.')      *(cp1++) = '_';
      else                  *(cp1++) =  *cp2;
      infront = 1;
    }
  }
  if (*(cp1-1) == '.') cp1--; /* Unix spec ending in '/' ==> trailing '.' */
  if (hasdir) *(cp1++) = ']';
  if (*cp2) cp2++;  /* check in case we ended with trailing '..' */
  while (*cp2) *(cp1++) = *(cp2++);
  *cp1 = '\0';

  return rslt;
}



/*
 * my_exit
 * Replacement for exit() to do something reasonable with exit statuses.
 * We use the same codes VMS DIFFERENCES does to indicate whether the
 * files were the same or different.  Note that the GNU sources use
 * 0 both to indicate files are the same and as a generic success value,
 * so we have little choice but to propagate this conflation.
 * Note: sdiff may kill itself rather than exit, thus never getting here.
 */

#ifdef exit
#undef exit	/* get the real one back */
#endif

void
my_exit (status)
    int status;
{
    int my_status;
/* 
 * Child processes have to give Unix-style exit codes. Unfortunately this
 * means we lose our nice exit statuses if the main program happens to be
 * running in a subprocess.
 */
    if ( getppid() )
		exit(status);

    switch (status)
    {
      case 0:
        my_status = 7110665; 	/* SAMEFILE */
        break;

      case 1:
        my_status = 7110675; 	/* FILAREDIF */
        break;

      case 2:			/* error exit */
        my_status = SS$_ABORT;
        break;

      default:			/* else just pass on what we got */
        my_status = status;
        break;
    }

    exit (my_status);
}

/* The jacket for unlink() accomplishes the following:
 *  -- converts filenames to VMS format before sending them
 *     to the C RTL
 *  -- uses delete() since unlink() is not available on all
 *     VMS systems
 */

int my_unlink (const char *file_spec)
{
  char vmsspec[NAM$C_MAXRSS+1];

  if (do_tovmsspec(file_spec,vmsspec,0) == NULL) return -1;

  return delete( vmsspec );
}




/*
 * quote_system_arg
 * Replacement for highly Unix-specific version in quotesys.c.  It's not
 * clear we really need this for VMS but it may need to be operational
 * so here's an easy version.
 *          comments from quotesys.c included below
 */

/* Place into QUOTED a quoted version of ARG suitable for `system'.
   Return the length of the resulting string (which is not null-terminated).
   If QUOTED is null, return the length without any side effects.  */

size_t
quote_system_arg (quoted, arg)
     char *quoted;
     char const *arg;
{
  size_t len = strlen(arg);
  if (quoted) memcpy(quoted, arg, len);
  return (len);
}


/*
 * initialize_main
 * This function implements I/O redirection for VMS systems before 7.0
 * that do not have the PIPE command (or anyone who just prefers this
 * way of doing it).  The function also places a pointer to a shortened
 * version of the program name in argv[0], and sets a logical name so
 * auxiliary programs can find diff.
 */

#define REMOVE_2_ARGS(i,argc,argv) \
	(argc) -= 2; \
	for (j=(i); j <= (argc); ++j) \
	  (argv)[j] = (argv)[j+2];

/* "Short" program name; a pointer to it is inserted into argv[0]
   to make messages look nicer */
static char short_program_name[NAME_MAX+1];

void vms_initialize_main (int *argc, char **argv[])
{
  char *p;
  int i, j, k, cond, new_argc = 0;
  int append;
  char **vms_argv;
  FILE *f;

/* Path of program from argv[0] */
  char program_path[PATH_MAX+1];

  struct dsc$descriptor_s prgnam_dsc;
  $DESCRIPTOR( vaxc_path_dsc, "VAXC$PATH" );
  $DESCRIPTOR( proc_table_dsc, "LNM$PROCESS_TABLE" );

  struct itmlst lnmlst[] = {0, LNM$_STRING, 0, 0, 0};

  struct fscndef fscn_item[] =
  {
    {0, FSCN$_NAME, 0},
    {0, FSCN$_DEVICE, 0},
    {0, FSCN$_ROOT, 0},
    {0, FSCN$_DIRECTORY, 0},
    {0, 0, 0}
  };

  /* Get program name, device and directory from argv[0],
     copy to short_program_name[] and program_path[], respectively */
  prgnam_dsc.dsc$w_length  = strlen ((*argv)[0]);
  prgnam_dsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  prgnam_dsc.dsc$b_class   = DSC$K_CLASS_S;
  prgnam_dsc.dsc$a_pointer = (*argv)[0];

  cond = sys$filescan (&prgnam_dsc, fscn_item, NULL, NULL, NULL);
  if (! $VMS_STATUS_SUCCESS(cond))
    lib$signal (cond);

  if (fscn_item[0].fscn$w_length != 0)
  {
    /* Just to make messages look nicer */
    memcpy (short_program_name, (char*)fscn_item[0].fscn$l_addr, fscn_item[0].fscn$w_length);
    short_program_name[fscn_item[0].fscn$w_length] = '\0';
    (*argv)[0] = short_program_name;
  }

  p = program_path;
  *p = '\0';
  for (i = 1; i <= 3; ++i)
    if (fscn_item[i].fscn$w_length != 0)
    {
    	memcpy (p, (char*)fscn_item[i].fscn$l_addr, fscn_item[i].fscn$w_length);
    	p += fscn_item[i].fscn$w_length;
    }
  *p = '\0';

/*
 * Set the logical name VAXC$PATH to point to the path of the current
 * program. This will cause the auxiliary programs, when they invoke the
 * diff program with execvp(), to look for it in the same directory.
 * We have to use $crelnm instead of lib$set_logical in order to get a
 * user-mode logical name.
 */

  lnmlst->buflen = strlen (program_path);
  lnmlst->bufadr = &program_path;

  if (lnmlst->buflen != 0)
  {
    unsigned char accmode = PSL$C_USER;
    cond = sys$crelnm( NULL, 		/* take default attributes */
		       &proc_table_dsc,	/* use the process table */
		       &vaxc_path_dsc,  /* set VAXC$PATH */
		       &accmode,	/* user mode */
		       lnmlst );	/* item list w/ path as translation */

    if (! $VMS_STATUS_SUCCESS(cond))
      lib$signal (cond);
  }

  vms_argv = (char **) malloc((*argc+1) * sizeof(char*));

  vms_argv[new_argc++] = **argv;

  for (i = 1; i < *argc; i++) {
        if (argv[0][i][0] == '>') {
            k = 1;
            append = 0;
            if (argv[0][i][k] == '>') {
                k++;
                append++;
            }
            if (argv[0][i][k]) {
                f = freopen(argv[0][i]+k,append?"a":"w",stdout);
                if (!f) {
                    perror("stdout redirection");
                    exit(SS$_ABORT);
                }
            } else if (i+1 < *argc && argv[0][i+1] && *argv[0][i+1]) {
                f = freopen(argv[0][i+1],append?"a":"w",stdout);
                if (!f) {
                    perror("stdout redirection");
                    exit(SS$_ABORT);
                }
                i++;
            }
        } else if (argv[0][i][0] == '2' && argv[0][i][1] == '>') {
            k = 2;
            append = 0;
            if (argv[0][i][k] == '>') {
                k++;
                append++;
            }
            if (argv[0][i][k]) {
                if (argv[0][i][k] == '&') {
            	    if (argv[0][i][k+1] != '1') exit(SS$_BADPARAM);
                    stderr = stdout;
            	} else {
                    f = freopen(argv[0][i]+k,append?"a":"w",stderr);
                    if (!f) {
                        perror("stderr redirection");
                        exit(SS$_ABORT);
                    }
                }
            } else if (i+1 < *argc && argv[0][i+1] && *argv[0][i+1]) {
                if (argv[0][i+1][0] == '&') {
            	    if (argv[0][i+1][1] != '1') exit(SS$_BADPARAM);
                    stderr = stdout;
            	} else {
                    f = freopen(argv[0][i+1],append?"a":"w",stderr);
                    if (!f) {
                        perror("stderr redirection");
                        exit(SS$_ABORT);
                    }
                }
                i++;
            }
        } else if (*argv[0][i] == '<') {
            if (argv[0][i][1]) {
                f = freopen(argv[0][i]+1,"r",stdin);
                if (!f) {
                    perror("stdin redirection");
                    exit(SS$_ABORT);
                }
            } else if (i+1 < *argc && argv[0][i+1] && *argv[0][i+1]) {
                f = freopen(argv[0][i+1],"r",stdin);
                if (!f) {
                    perror("stdin redirection");
                    exit(SS$_ABORT);
                }
                i++;
            }
        } else {
            vms_argv[new_argc++] = argv[0][i];
        }
  }

  *argc = new_argc;
  vms_argv[new_argc] = NULL;
  *argv = vms_argv;
}

#ifdef NDIR_EMULATED
#define xfree    free
#include <rms.h>
char *file_name_as_directory (char *out, char *in);
static struct direct *vms_low_readdir ();

typedef struct
{
  DIR s_dir;
  unsigned long context;
  unsigned long uflags;
  struct dsc$descriptor_s dir_spec;
  struct dsc$descriptor_s file_spec;
  int version_flag;
  unsigned long status;
} VMS_DIR;

DIR *
opendir (filename, filepattern)
     char *filename;   /* name of directory */
     char *filepattern;
{
  register VMS_DIR *dirp;      /* -> malloc'ed storage */
  register unsigned int length = strlen (filename);
  register int fd;             /* file descriptor for read */
  struct stat sbuf;            /* result of fstat */

  if ((filename[length-1] != ']'
       && filename[length-1] != '>'
       && filename[length-1] != ':'
       && (stat (filename, &sbuf) < 0
          || (sbuf.st_mode & S_IFMT) != S_IFDIR)))
    {
      errno = ENOTDIR;
      return 0;                /* bad luck today */
    }

  if ((dirp = (VMS_DIR *) xmalloc (sizeof (VMS_DIR))) == 0)
    {
      errno = ENOMEM;
      return 0;                /* bad luck today */
    }

  {
    int count;
    va_count(count);
    if (count == 2)
      {
       dirp->file_spec.dsc$a_pointer = 
         (char *) xmalloc (strlen (filepattern) + 1);
       strcpy (dirp->file_spec.dsc$a_pointer, filepattern);
      }
    else
      {
       dirp->file_spec.dsc$a_pointer = 
         (char *) xmalloc (4);
       strcpy (dirp->file_spec.dsc$a_pointer, "*.*");
      }
    dirp->file_spec.dsc$w_length = strlen (dirp->file_spec.dsc$a_pointer);
    dirp->file_spec.dsc$b_dtype = DSC$K_DTYPE_T;
    dirp->file_spec.dsc$b_class = DSC$K_CLASS_S;
    dirp->version_flag = strchr (dirp->file_spec.dsc$a_pointer, ';') != 0;
  }
  dirp->dir_spec.dsc$a_pointer = (char *) xmalloc (strlen (filename) + 10);
  file_name_as_directory (dirp->dir_spec.dsc$a_pointer, filename);
  dirp->dir_spec.dsc$w_length = strlen (dirp->dir_spec.dsc$a_pointer);
  dirp->dir_spec.dsc$b_dtype = DSC$K_DTYPE_T;
  dirp->dir_spec.dsc$b_class = DSC$K_CLASS_S;
  dirp->context = 0;
  dirp->uflags = 2;
  dirp->s_dir.dd_fd = 0;
  dirp->s_dir.dd_loc = dirp->s_dir.dd_size = 0;        /* refill needed */

  /* In the cases where the filename ended with `]', `>' or `:',
     we never checked if it really was a directory, so let's do that
     now, by trying to read the first entry.  */
  if (vms_low_readdir ((DIR *) dirp) == (struct direct *) -1)
    {
      closedir (dirp);         /* was: xfree (dirp);  */
      errno = ENOENT;
      return 0;
    }
  dirp->s_dir.dd_loc = 0;      /* Make sure the entry just read is
                                  reused at the next call to readdir.  */

  return (DIR *) dirp;         /* I had to cast, for VMS sake.  */
}

int
closedir (dirp)
     register DIR *dirp;               /* stream from opendir */
{
  {
    VMS_DIR *vms_dirp = (VMS_DIR *) dirp;

    if (vms_dirp->context != 0)
      lib$find_file_end (&(vms_dirp->context));
    xfree (vms_dirp->dir_spec.dsc$a_pointer);
    xfree (vms_dirp->file_spec.dsc$a_pointer);
  }

  xfree ((char *) dirp);
  return 0;
}

struct direct dir_static;      /* simulated directory contents */

static struct direct *
vms_low_readdir (dirp)
     register DIR *dirp;
{
  static char rbuf[257];
  static struct dsc$descriptor_s rdsc =
    { sizeof (rbuf), DSC$K_DTYPE_T, DSC$K_CLASS_S, rbuf };
  VMS_DIR * vms_dirp = (VMS_DIR *) dirp;

  if (dirp->dd_size == 0)
    {
      char *cp, *cp2;
      unsigned long status;

      status = lib$find_file (&vms_dirp->file_spec, &rdsc, &vms_dirp->context,
                             &vms_dirp->dir_spec, 0, 0, &vms_dirp->uflags);
      vms_dirp->status = status;
      if (status == RMS$_NMF || status == RMS$_FNF)
       return 0;
      if (status != RMS$_NORMAL)
       return (struct direct *) -1;

      rbuf [256] = '\0';
      if (cp = strchr (rbuf, ' '))
       *cp = '\0';
      if ((cp = strchr (rbuf, ';')) != 0
         && !vms_dirp->version_flag)
       *cp = '\0';

      for (cp2 = rbuf - 1; cp2 != 0;)
       {
         char *cp2tmp = 0;
         cp = cp2 + 1;
         cp2 = strchr (cp, ']');
         if (cp2 != 0)
           cp2tmp = strchr (cp2 + 1, '>');
         if (cp2tmp != 0)
           cp2 = cp2tmp;
       }
      strcpy (dirp->dd_buf, cp);
      dirp->dd_size = strlen (dirp->dd_buf);
      dirp->dd_loc = 0;
    }

  if (vms_dirp->status != RMS$_NORMAL)
    return 0;

  /* remove .DIR or .DIR;1 extension, if any */
  {
    int ext_point = 0;

    if (dirp->dd_size > 4 && (! strcmp (&dirp->dd_buf[dirp->dd_size - 4], ".DIR")
                     || ! strcmp (&dirp->dd_buf[dirp->dd_size - 4], ".dir")))
      ext_point = 4;
    else
      if (dirp->dd_size > 6 && (! strncmp (&dirp->dd_buf[dirp->dd_size - 6], ".DIR", 4)
                       || ! strncmp (&dirp->dd_buf[dirp->dd_size - 6], ".dir", 4))
          && (dirp->dd_buf[dirp->dd_size - 1] == '.' || dirp->dd_buf[dirp->dd_size - 1] == ';')
          && dirp->dd_buf[dirp->dd_size] == '1')
        ext_point = 6;

    if (ext_point != 0)
      {
      	dirp->dd_size -= ext_point;
      	dirp->dd_buf[dirp->dd_size] = '\0';
      }
  }

  dir_static.d_ino = -1;       /* Couldn't care less...  */
  dir_static.d_namlen = strlen (dirp->dd_buf);
  dir_static.d_reclen = sizeof (struct direct)
    - MAXNAMLEN + 3
      + dir_static.d_namlen - dir_static.d_namlen % 4;
  strcpy (dir_static.d_name, dirp->dd_buf);
  dir_static.d_name[dir_static.d_namlen] = '\0';
  dirp->dd_loc = dirp->dd_size; /* only one record at a time */

  return &dir_static;
}

/* ARGUSED */
struct direct *
readdir (dirp)
     register DIR *dirp;       /* stream from opendir */
{
  register struct direct *dp;

  for (; ;)
    {
      if (dirp->dd_loc >= dirp->dd_size)
       dirp->dd_loc = dirp->dd_size = 0;

      dp = vms_low_readdir (dirp);
      if (dp == 0 || dp == (struct direct *) -1)
       return 0;

      return dp;
    }
}

/* file_name_as_directory was snarfed from src/fileio.c in GNU Emacs.  */
/* modified by Martin Vorlaender */

char *
file_name_as_directory (char *out, char *in)
{
  int size = strlen (in) - 1;
  int ext_point = 1;
  char *inp, in2[MAXNAMLEN+1];

  strcpy (out, in);

  /* Is it already a directory string? */
  if (in[size] == ':' || in[size] == ']' || in[size] == '>')
    return out;

  if (strchr (in, '/'))
    {
      /* It's a (pseudo-)Unix directory name. Convert. */
      char tem[MAXNAMLEN+1];

      strcpy(tem,in);
      strcat(tem,".DIR");
      do_tovmsspec(tem, in2, 0);
      size = strlen (in2) - 1;
      inp = in2;
    }
  else
    {
      inp = in;
    }
  {
    /* It is a VMS directory name now. Hack VMS syntax, */

    register char *p, *dot;
    char brack;
    static char tem[256];

    if (size > 3 && (! strcmp (&inp[size - 3], ".DIR")
                     || ! strcmp (&inp[size - 3], ".dir")))
      ext_point = -3;
    else
      if (size > 5 && (! strncmp (&inp[size - 5], ".DIR", 4)
                       || ! strncmp (&inp[size - 5], ".dir", 4))
          && (inp[size - 1] == '.' || inp[size - 1] == ';')
          && inp[size] == '1')
        ext_point = -5;

    /* dir:[000000]x.dir --> dir:x.dir --> dir:[x]
      dir:[000000.x]y.dir --> dir:[x]y.dir --> dir:[x.y]
      but dir:[000000.000000]x.dir --> dir:[000000.000000.x]
          dir:[000000.000000.x]y.dir --> dir:[000000.000000.x.y] */

    p = dot = strchr(inp,':');
    if (p != 0 && (p[1] == '[' || p[1] == '<'))
     {
       p += 2;
       if (strncmp(p,"000000",6) == 0)
         {
           p += 6;
           if (strncmp(p,".000000",7) != 0
               && (*p == ']' || *p == '>' || *p == '.'))
             {
               size = dot - inp + 1;
               strncpy(tem, inp, size);
               if (*p == '.')
                 tem[size++] = '[';
               strcpy(tem + size, p + 1);
               inp = tem;
               size = strlen(inp) - 1;
             }
         }
     }
    /* x.dir -> [.x]
      dir:x.dir --> dir:[x]
      dir:[x]y.dir --> dir:[x.y] */
    p = inp + size;
    while (p != inp && *p != ':' && *p != '>' && *p != ']') p--;
    {
     char *emergency_dir = 0;
     int emergency_point = 0; /* relative to the end of `out' */

     if (p != inp)
       {
         strncpy (out, inp, p - inp);
         out[p - inp] = '\0';
         if (*p == ':')
           {
             brack = ']';
             strcat (out, ":[");
             emergency_dir = "000000";
             emergency_point = 0;
           }
         else
           {
             brack = *p;
             strcat (out, ".");
             emergency_dir = "";
             emergency_point = -1;
           }
         p++;
       }
     else
       {
         brack = ']';
         strcpy (out, "[.");
         emergency_dir = "";
         emergency_point = -2;
       }
     if (strncmp (p, "000000.", 7) == 0
         && (strncmp (p+7, "000000", 6) != 0
             || (p[13] != ']' && p[13] != '>' && p[13] != '.')))
       p += 7;
     if (p < (inp + size + ext_point))
       {
         register copy_len = ((inp + size + ext_point) - p);
         size = strlen (out) + copy_len;
         strncat (out, p, copy_len);
       }
     else
       {
         size = strlen (out) + emergency_point;
         strcpy (out + size, emergency_dir);
         size += strlen (emergency_dir);
       }
    }
    out[size++] = brack;
    out[size] = '\0';
  }
  return out;
}





#endif


#ifdef PIPE_EMULATED
#include <iodef.h>
#include <dvidef.h>
#include <syidef.h>
#include <clidef.h>

/*
 * Miscellaneous VMS types that are not normally defined
 * in any consistent fashion.
 */

/* VMS I/O status block */
struct IOSB
{
  short status, count;
  long devinfo;
};

/* VMS Item List 3 structure */
struct itm$list3
{
  short buflen;
  short itemcode;
  void *buffer;
  size_t *retlen;
};

/* VMS Lock status block with value block */
struct LOCK
{
  short status, reserved;
  long lockid;
  long value[4];
};

/* VMS Exit Handler Control block */
struct EXHCB
{
  struct exhcb *exh$a_link;
  int (*exh$a_routine)();
  long exh$l_argcount;
  long *exh$a_status;
  long exh$l_status;
};


/* A linked list of pipes, for internal use only */
struct PIPE
{
  struct PIPE *next;            /* next pipe in the chain */
  struct PIPE *prev;            /* previous pipe in the chain */
  struct PIPE *self;            /* self reference */
  int mode;                     /* pipe I/O mode (read or write) */
  long status;                  /* subprocess completion status */
  struct IOSB iosb;             /* pipe I/O status block */
  FILE *file;                   /* pipe file structure */
  int pid;                      /* pipe process id */
  short chan;                   /* pipe channel */
  jmp_buf jmpbuf;              /* jump buffer, if needed */
  int has_jmpbuf;              /* flag */
};

/* Head of the pipe chain */
static struct PIPE *phead = NULL, *ptail = NULL;

static unsigned char evf = 1;

/*
 * Exit handler for current process, established by popen().
 * Force the current process to wait for the completion of children
 *   which were started via popen().
 * Since
 */
static int
pwait (status)
  int status;
{
  struct IOSB iosb;
  struct PIPE *this;
  int ret = 0;

  this = phead;
  while (this)
    {
      if (this->self != this)
        {
         ret = -1;
         continue;
       }
      if (!this->iosb.status)
       {
         fflush (this->file);
         if (this->mode == O_WRONLY)
           sys$qio (0, this->chan, IO$_WRITEOF, &iosb,
                    0, 0, 0, 0, 0, 0, 0, 0);
         fclose (this->file);
         sys$synch (evf, &this->iosb);
       }
      else
       fclose(this->file);
      sys$dassgn (this->chan);
      this = this->next;
    }
  return ret;
}

/*
 * Close a "pipe" created by popen()
 * Return codes
 * >0  VMS exit status of process
 *  0  success, pipe was closed
 * -1  stream not found in list of pipes
 * -2  memory corruption detected
 */
int
pclose (stream)
  FILE *stream;
{
  struct IOSB iosb;
  struct PIPE *this = phead;

  while (this && this->self == this && this->file != stream)
    this = this->next;

  /* Pipe not found or failed sanity check */
  if (!this)
    return -1;
  else if (this->self != this)
    return -2;

  /* Flush the I/O buffer and wait for the close to complete */
  if (!this->iosb.status)
    {
      fflush (this->file);
      if (this->mode == O_WRONLY)
       sys$qio (0, this->chan, IO$_WRITEOF, &iosb,
                0, 0, 0, 0, 0, 0, 0, 0);
      fclose (this->file);
      sys$synch (evf, &this->iosb);
    }
  else
    fclose (this->file);
  sys$dassgn (this->chan);

  /* Remove `this' from the list of pipes and free its storage */
  if (this == ptail)
    ptail = this->prev;
  if (this == phead)
    phead = this->next;
  if (this->prev)
    this->prev->next = this->next;
  if (this->next)
    this->next->prev = this->prev;
  free (this);

  if (this->status & STS$M_SUCCESS != STS$M_SUCCESS)
    return this->status;
  else
    return 0;
}

/*
 * Subprocess AST completion routine
 * Indicate successful completion in the iosb and clear the pid.
 * Note that the channel is *not* deassigned and the file is
 *   *not* closed.
 */
void
pdone (this)
  struct PIPE *this;
{
  struct IOSB iosb;

  if (this->self != this)
    return;
  this->iosb.status = 1;
  this->pid  = 0;
  if (this->has_jmpbuf)
    {
      this->has_jmpbuf = 0;
      longjmp (this->jmpbuf, 1);
    }
}

int
pipe_set_fd_jmpbuf (fd, jmpbuf)
     int fd;
     jmp_buf jmpbuf;
{
  struct PIPE *this = phead;

  while (this)
    if (fileno (this->file) == fd)
      {
       memcpy (this->jmpbuf, jmpbuf, sizeof (jmp_buf));
       this->has_jmpbuf = 1;
       if (this->pid == 0)
         {
           this->has_jmpbuf = 0;
           longjmp (this->jmpbuf, 1);
         }
       return 0;
      }
    else
      this = this->next;
  return 1;
}

pipe_unset_fd_jmpbuf (fd)
     int fd;
{
  struct PIPE *this = phead;

  while (this)
    if (fileno (this->file) == fd)
      {
       this->has_jmpbuf = 0;
       return 0;
      }
    else
      this = this->next;
  return 1;
}

/* Exit handler control block for the current process. */
static struct EXHCB pexhcb = { 0, pwait, 1, &pexhcb.exh$l_status, 0 };

struct Vstring
{
  short length;
  char body[NAM$C_MAXRSS+1];
};

/*
 * Emulate a unix popen() call using lib$spawn
 *
 * if mode == "w", lib$spawn uses the mailbox for sys$input
 * if mode == "r", lib$spawn uses the mailbox for sys$output
 *
 * Don't now how to handle both read and write
 *
 * Returns
 *   FILE *  file pointer to the pipe
 *   NULL    indicates an error ocurred, check errno value
 */
FILE *
popen (cmd, mode)
  const char *cmd;
  const char *mode;
{
#ifdef __DECC
#pragma message save
#pragma message disable ADDRCONSTEXT
#endif
  int i, status, flags, mbxsize;
  struct IOSB iosb;
  struct dsc$descriptor_s cmddsc, mbxdsc;
  struct Vstring mbxname = { sizeof(mbxname.body) };
  struct itm$list3 mbxlist[2] = {
    { sizeof(mbxname.body)-1, DVI$_DEVNAM, &mbxname.body, (size_t *) &mbxname.length },
    { 0, 0, 0, 0} };
  struct itm$list3 syilist[2] = {
    { sizeof(mbxsize), SYI$_MAXBUF, &mbxsize, (size_t *) 0 },
    { 0, 0, 0, 0} };
  static int noExitHandler = 1;
  struct PIPE *this;
#ifdef __DECC
#pragma message restore
#endif

  /* First allocate space for the new pipe */
  this = (struct PIPE *) calloc (1, sizeof(struct PIPE));
  if (!this)
    {
      errno = ENOMEM;
      return NULL;
    }

  /* Sanity check value */
  this->self = this;

  /* Use the smaller of SYI$_MAXBUF and 2048 for the mailbox size */
  status = sys$getsyiw(0, 0, 0, syilist, &iosb, 0, 0, 0);
  if (status != SS$_NORMAL && !(iosb.status & STS$M_SUCCESS))
    {
      vaxc$errno = iosb.status;
      errno = EVMSERR;
      free (this);
      perror ("popen, $GETSYIW failure for SYI$_MAXBUF");
      return NULL;
    }

  if (mbxsize > 2048)
    mbxsize = 2048;

  status = sys$crembx (0, &this->chan, mbxsize, mbxsize, 0, 0, 0, 0);
  if (status != SS$_NORMAL)
    {
      vaxc$errno = status;
      errno = EVMSERR;
      free (this);
      perror ("popen, $CREMBX failure");
      return NULL;
    }

  /* Retrieve mailbox name, use for fopen */
  status = sys$getdviw (0, this->chan, 0, &mbxlist, &iosb, 0, 0, 0);
  if (status != SS$_NORMAL && !(iosb.status & STS$M_SUCCESS))
    {
      vaxc$errno = iosb.status;
      errno = EVMSERR;
      sys$dassgn (this->chan);
      free (this);
      perror ("popen, $GETDVIW failure");
      return NULL;
    }

  /* Spawn the command using the mailbox as the name for sys$input */
  mbxname.body[mbxname.length] = 0;
  mbxdsc.dsc$w_length  = mbxname.length;
  mbxdsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  mbxdsc.dsc$b_class   = DSC$K_CLASS_S;
  mbxdsc.dsc$a_pointer = mbxname.body;

  cmddsc.dsc$w_length  = strlen(cmd);
  cmddsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  cmddsc.dsc$b_class   = DSC$K_CLASS_S;
  cmddsc.dsc$a_pointer = (char *)cmd;
  flags = CLI$M_NOWAIT;
  if (strcmp(mode,"w") == 0)
    {
      status = lib$spawn (&cmddsc, &mbxdsc, 0, &flags, 0, &this->pid,
                          &this->status, &evf, &pdone, this->self);
      this->mode = O_WRONLY;
    }
  else
    {
      status = lib$spawn (&cmddsc, 0, &mbxdsc, &flags, 0, &this->pid,
                          &this->status, &evf, &pdone, this->self);
      this->mode = O_RDONLY;
    }
  if (status != SS$_NORMAL)
    {
      vaxc$errno = status;
      errno = EVMSERR;
      sys$dassgn (this->chan);
      free (this);
      perror("popen, LIB$SPAWN failure");
      return NULL;
    }

  /* Set up an exit handler so the subprocess isn't prematurely killed */
  if (noExitHandler)
    {
      status = sys$dclexh (&pexhcb);
      if (status != SS$_NORMAL)
        {
          vaxc$errno = status;
          errno = EVMSERR;
          sys$dassgn (this->chan);
          sys$delprc (&this->pid, 0);
          free (this);
          perror("popen, $DCLEXH failure");
          return NULL;
        }
      noExitHandler = 0;
    }

  /* Pipes are always binary mode devices */
  if (this->mode == O_WRONLY)
    this->file = fopen (mbxname.body, "w");
  else
    this->file = fopen (mbxname.body, "r");

  /* Paranoia, check for failure again */
  if (!this->file)
    {
      sys$dassgn (this->chan);
      sys$delprc (this->pid);
      free (this);
      perror ("popen, fopen failure");
      return NULL;
    }

  this->has_jmpbuf = 0;

  /* Insert the new pipe into the list of open pipes */
  if (phead)
    {
      ptail->next = this;
      this->prev = ptail;
      ptail = this;
    }
  else
    phead = ptail = this;

  return (this->file);
}


#endif /* PIPE_EMULATED */

