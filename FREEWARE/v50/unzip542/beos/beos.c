/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  beos.c

  BeOS-specific routines for use with Info-ZIP's UnZip 5.30 and later.
  (based on unix/unix.c)

  Contains:  do_wild()           <-- generic enough to put in fileio.c?
             mapattr()
             mapname()
             checkdir()
             close_outfile()
             set_direc_attribs()
             stamp_file()
             version()
             scanBeOSexfield()
             set_file_attrs()
             setBeOSexfield()
             printBeOSexfield()
             assign_MIME()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"

#include "beos.h"
#include <errno.h>             /* Just make sure we've got a few things... */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <dirent.h>

/* For the new post-DR8 file attributes */
#include <kernel/fs_attr.h>
#include <support/byteorder.h>
#include <storage/Mime.h>

static uch *scanBeOSexfield  OF((const uch *ef_ptr, unsigned ef_len));
static int  set_file_attrs( const char *, const unsigned char *, const off_t );
static void setBeOSexfield   OF((const char *path, uch *extra_field));
#ifdef BEOS_USE_PRINTEXFIELD
static void printBeOSexfield OF((int isdir, uch *extra_field));
#endif
#ifdef BEOS_ASSIGN_FILETYPE
static void assign_MIME( const char * );
#endif

static int created_dir;        /* used in mapname(), checkdir() */
static int renamed_fullpath;   /* ditto */

#ifndef SFX

/**********************/
/* Function do_wild() */   /* for porting:  dir separator; match(ignore_case) */
/**********************/

char *do_wild(__G__ wildspec)
    __GDEF
    ZCONST char *wildspec;  /* only used first time on a given dir */
{
    static DIR *wild_dir = (DIR *)NULL;
    static ZCONST char *wildname;
    static char *dirname, matchname[FILNAMSIZ];
    static int notfirstcall=FALSE, have_dirname, dirnamelen;
    struct dirent *file;

    /* Even when we're just returning wildspec, we *always* do so in
     * matchname[]--calling routine is allowed to append four characters
     * to the returned string, and wildspec may be a pointer to argv[].
     */
    if (!notfirstcall) {    /* first call:  must initialize everything */
        notfirstcall = TRUE;

        if (!iswild(wildspec)) {
            strcpy(matchname, wildspec);
            have_dirname = FALSE;
            wild_dir = NULL;
            return matchname;
        }

        /* break the wildspec into a directory part and a wildcard filename */
        if ((wildname = strrchr(wildspec, '/')) == (ZCONST char *)NULL) {
            dirname = ".";
            dirnamelen = 1;
            have_dirname = FALSE;
            wildname = wildspec;
        } else {
            ++wildname;     /* point at character after '/' */
            dirnamelen = wildname - wildspec;
            if ((dirname = (char *)malloc(dirnamelen+1)) == (char *)NULL) {
                Info(slide, 0x201, ((char *)slide,
                  "warning:  cannot allocate wildcard buffers\n"));
                strcpy(matchname, wildspec);
                return matchname;   /* but maybe filespec was not a wildcard */
            }
            strncpy(dirname, wildspec, dirnamelen);
            dirname[dirnamelen] = '\0';   /* terminate for strcpy below */
            have_dirname = TRUE;
        }

        if ((wild_dir = opendir(dirname)) != (DIR *)NULL) {
            while ((file = readdir(wild_dir)) != (struct dirent *)NULL) {
                if (file->d_name[0] == '.' && wildname[0] != '.')
                    continue;  /* Unix:  '*' and '?' do not match leading dot */
                if (match(file->d_name, wildname, 0)) {  /* 0 == case sens. */
                    if (have_dirname) {
                        strcpy(matchname, dirname);
                        strcpy(matchname+dirnamelen, file->d_name);
                    } else
                        strcpy(matchname, file->d_name);
                    return matchname;
                }
            }
            /* if we get to here directory is exhausted, so close it */
            closedir(wild_dir);
            wild_dir = (DIR *)NULL;
        }

        /* return the raw wildspec in case that works (e.g., directory not
         * searchable, but filespec was not wild and file is readable) */
        strcpy(matchname, wildspec);
        return matchname;
    }

    /* last time through, might have failed opendir but returned raw wildspec */
    if (wild_dir == (DIR *)NULL) {
        notfirstcall = FALSE; /* nothing left to try--reset for new wildspec */
        if (have_dirname)
            free(dirname);
        return (char *)NULL;
    }

    /* If we've gotten this far, we've read and matched at least one entry
     * successfully (in a previous call), so dirname has been copied into
     * matchname already.
     */
    while ((file = readdir(wild_dir)) != (struct dirent *)NULL) {
        if (file->d_name[0] == '.' && wildname[0] != '.')
            continue;   /* Unix:  '*' and '?' do not match leading dot */
        if (match(file->d_name, wildname, 0)) {   /* 0 == don't ignore case */
            if (have_dirname) {
                /* strcpy(matchname, dirname); */
                strcpy(matchname+dirnamelen, file->d_name);
            } else
                strcpy(matchname, file->d_name);
            return matchname;
        }
    }

    closedir(wild_dir);     /* have read at least one entry; nothing left */
    wild_dir = (DIR *)NULL;
    notfirstcall = FALSE;   /* reset for new wildspec */
    if (have_dirname)
        free(dirname);
    return (char *)NULL;

} /* end function do_wild() */

#endif /* !SFX */





/**********************/
/* Function mapattr() */
/**********************/

int mapattr(__G)
    __GDEF
{
    ulg tmp = G.crec.external_file_attributes;

    switch (G.pInfo->hostnum) {
        case AMIGA_:
            tmp = (unsigned)(tmp>>17 & 7);   /* Amiga RWE bits */
            G.pInfo->file_attr = (unsigned)(tmp<<6 | tmp<<3 | tmp);
            break;
        case THEOS_:
            tmp &= 0xF1FFFFFFL;
            if ((tmp & 0xF0000000L) != 0x40000000L)
                tmp &= 0x01FFFFFFL;     /* not a dir, mask all ftype bits */
            else
                tmp &= 0x41FFFFFFL;     /* leave directory bit as set */
            /* fall through! */
        case BEOS_:
        case UNIX_:
        case VMS_:
        case ACORN_:
        case ATARI_:
        case QDOS_:
        case TANDEM_:
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            if (G.pInfo->file_attr != 0 || !G.extra_field) {
                return 0;
            } else {
                /* Some (non-Info-ZIP) implementations of Zip for Unix and
                   VMS (and probably others ??) leave 0 in the upper 16-bit
                   part of the external_file_attributes field. Instead, they
                   store file permission attributes in some extra field.
                   As a work-around, we search for the presence of one of
                   these extra fields and fall back to the MSDOS compatible
                   part of external_file_attributes if one of the known
                   e.f. types has been detected.
                   Later, we might implement extraction of the permission
                   bits from the VMS extra field. But for now, the work-around
                   should be sufficient to provide "readable" extracted files.
                   (For ASI Unix e.f., an experimental remap of the e.f.
                   mode value IS already provided!)
                 */
                ush ebID;
                unsigned ebLen;
                uch *ef = G.extra_field;
                unsigned ef_len = G.crec.extra_field_length;
                int r = FALSE;

                while (!r && ef_len >= EB_HEADSIZE) {
                    ebID = makeword(ef);
                    ebLen = (unsigned)makeword(ef+EB_LEN);
                    if (ebLen > (ef_len - EB_HEADSIZE))
                        /* discoverd some e.f. inconsistency! */
                        break;
                    switch (ebID) {
                      case EF_ASIUNIX:
                        if (ebLen >= (EB_ASI_MODE+2)) {
                            G.pInfo->file_attr =
                              (unsigned)makeword(ef+(EB_HEADSIZE+EB_ASI_MODE));
                            /* force stop of loop: */
                            ef_len = (ebLen + EB_HEADSIZE);
                            break;
                        }
                        /* else: fall through! */
                      case EF_PKVMS:
                        /* "found nondecypherable e.f. with perm. attr" */
                        r = TRUE;
                      default:
                        break;
                    }
                    ef_len -= (ebLen + EB_HEADSIZE);
                    ef += (ebLen + EB_HEADSIZE);
                }
                if (!r)
                    return 0;
            }
            /* fall through! */
        /* all remaining cases:  expand MSDOS read-only bit into write perms */
        case FS_FAT_:
            /* PKWARE's PKZip for Unix marks entries as FS_FAT_, but stores the
             * Unix attributes in the upper 16 bits of the external attributes
             * field, just like Info-ZIP's Zip for Unix.  We try to use that
             * value, after a check for consistency with the MSDOS attribute
             * bits (see below).
             */
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            /* fall through! */
        case FS_HPFS_:
        case FS_NTFS_:
        case MAC_:
        case TOPS20_:
        default:
            /* Ensure that DOS subdir bit is set when the entry's name ends
             * in a '/'.  Some third-party Zip programs fail to set the subdir
             * bit for directory entries.
             */
            if ((tmp | 0x10) == 0) {
                extent fnlen = strlen(G.filename);
                if (fnlen > 0 && G.filename[fnlen-1] == '/')
                    tmp |= 0x10;
            }
            /* read-only bit --> write perms; subdir bit --> dir exec bit */
            tmp = !(tmp & 1) << 1  |  (tmp & 0x10) >> 4;
            if ((G.pInfo->file_attr & 0700) == (unsigned)(0400 | tmp<<6))
                /* keep previous G.pInfo->file_attr setting, when its "owner"
                 * part appears to be consistent with DOS attribute flags!
                 */
                return 0;
            G.pInfo->file_attr = (unsigned)(0444 | tmp<<6 | tmp<<3 | tmp);
            break;
    } /* end switch (host-OS-created-by) */

    /* for originating systems with no concept of "group," "other," "system": */
    umask( (int)(tmp=umask(0)) );    /* apply mask to expanded r/w(/x) perms */
    G.pInfo->file_attr &= ~tmp;

    return 0;

} /* end function mapattr() */





/************************/
/*  Function mapname()  */
/************************/
                             /* return 0 if no error, 1 if caution (filename */
int mapname(__G__ renamed)   /*  truncated), 2 if warning (skip file because */
    __GDEF                   /*  dir doesn't exist), 3 if error (skip file), */
    int renamed;             /*  or 10 if out of memory (skip file) */
{                            /*  [also IZ_VOL_LABEL, IZ_CREATED_DIR] */
    char pathcomp[FILNAMSIZ];      /* path-component buffer */
    char *pp, *cp=(char *)NULL;    /* character pointers */
    char *lastsemi=(char *)NULL;   /* pointer to last semi-colon in pathcomp */
    int quote = FALSE;             /* flags */
    int error = 0;
    register unsigned workch;      /* hold the character being tested */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    if (G.pInfo->vollabel)
        return IZ_VOL_LABEL;    /* can't set disk volume labels in BeOS */

    /* can create path as long as not just freshening, or if user told us */
    G.create_dirs = (!uO.fflag || renamed);

    created_dir = FALSE;        /* not yet */

    /* user gave full pathname:  don't prepend rootpath */
    renamed_fullpath = (renamed && (*G.filename == '/'));

    if (checkdir(__G__ (char *)NULL, INIT) == 10)
        return 10;              /* initialize path buffer, unless no memory */

    *pathcomp = '\0';           /* initialize translation buffer */
    pp = pathcomp;              /* point to translation buffer */
    if (uO.jflag)               /* junking directories */
        cp = (char *)strrchr(G.filename, '/');
    if (cp == (char *)NULL)     /* no '/' or not junking dirs */
        cp = G.filename;        /* point to internal zipfile-member pathname */
    else
        ++cp;                   /* point to start of last component of path */

/*---------------------------------------------------------------------------
    Begin main loop through characters in filename.
  ---------------------------------------------------------------------------*/

    while ((workch = (uch)*cp++) != 0) {

        if (quote) {                 /* if character quoted, */
            *pp++ = (char)workch;    /*  include it literally */
            quote = FALSE;
        } else
            switch (workch) {
            case '/':             /* can assume -j flag not given */
                *pp = '\0';
                if ((error = checkdir(__G__ pathcomp, APPEND_DIR)) > 1)
                    return error;
                pp = pathcomp;    /* reset conversion buffer for next piece */
                lastsemi = (char *)NULL; /* leave directory semi-colons alone */
                break;

            case ';':             /* VMS version (or DEC-20 attrib?) */
                lastsemi = pp;
                *pp++ = ';';      /* keep for now; remove VMS ";##" */
                break;            /*  later, if requested */

            case '\026':          /* control-V quote for special chars */
                quote = TRUE;     /* set flag for next character */
                break;

            default:
                /* allow European characters in filenames: */
                if (isprint(workch) || (128 <= workch && workch <= 254))
                    *pp++ = (char)workch;
            } /* end switch */

    } /* end while loop */

    *pp = '\0';                   /* done with pathcomp:  terminate it */

    /* if not saving them, remove VMS version numbers (appended ";###") */
    if (!uO.V_flag && lastsemi) {
        pp = lastsemi + 1;
        while (isdigit((uch)(*pp)))
            ++pp;
        if (*pp == '\0')          /* only digits between ';' and end:  nuke */
            *lastsemi = '\0';
    }

/*---------------------------------------------------------------------------
    Report if directory was created (and no file to create:  filename ended
    in '/'), check name to be sure it exists, and combine path and name be-
    fore exiting.
  ---------------------------------------------------------------------------*/

    if (G.filename[strlen(G.filename) - 1] == '/') {
        checkdir(__G__ G.filename, GETPATH);
        if (created_dir) {
            if (QCOND2) {
                Info(slide, 0, ((char *)slide, "   creating: %s\n",
                  G.filename));
            }

            if (!uO.J_flag) {   /* Handle the BeOS extra field if present. */
                void *ptr = scanBeOSexfield( G.extra_field,
                                             G.lrec.extra_field_length );
                if (ptr) {
                    setBeOSexfield( G.filename, ptr );
                }
            }

#ifndef NO_CHMOD
            /* set approx. dir perms (make sure can still read/write in dir) */
            if (chmod(G.filename, (0xffff & G.pInfo->file_attr) | 0700))
                perror("chmod (directory attributes) error");
#endif

            return IZ_CREATED_DIR;   /* set dir time (note trailing '/') */
        }
        /* TODO: should we re-write the BeOS extra field data in case it's */
        /* changed?  The answer is yes. [Sept 1999 - cjh]                  */
        if (!uO.J_flag) {   /* Handle the BeOS extra field if present. */
            void *ptr = scanBeOSexfield( G.extra_field,
                                         G.lrec.extra_field_length );
            if (ptr) {
                setBeOSexfield( G.filename, ptr );
            }
        }

        return 2;   /* dir existed already; don't look for data to extract */
    }

    if (*pathcomp == '\0') {
        Info(slide, 1, ((char *)slide, "mapname:  conversion of %s failed\n",
          G.filename));
        return 3;
    }

    checkdir(__G__ pathcomp, APPEND_NAME);  /* returns 1 if truncated: care? */
    checkdir(__G__ G.filename, GETPATH);

    return error;

} /* end function mapname() */





/***********************/
/* Function checkdir() */
/***********************/

int checkdir(__G__ pathcomp, flag)
    __GDEF
    char *pathcomp;
    int flag;
/*
 * returns:  1 - (on APPEND_NAME) truncated filename
 *           2 - path doesn't exist, not allowed to create
 *           3 - path doesn't exist, tried to create and failed; or
 *               path exists and is not a directory, but is supposed to be
 *           4 - path is too long
 *          10 - can't allocate memory for filename buffers
 */
{
    static int rootlen = 0;   /* length of rootpath */
    static char *rootpath;    /* user's "extract-to" directory */
    static char *buildpath;   /* full path (so far) to extracted file */
    static char *end;         /* pointer to end of buildpath ('\0') */

#   define FN_MASK   7
#   define FUNCTION  (flag & FN_MASK)



/*---------------------------------------------------------------------------
    APPEND_DIR:  append the path component to the path being built and check
    for its existence.  If doesn't exist and we are creating directories, do
    so for this one; else signal success or error as appropriate.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_DIR) {
        int too_long = FALSE;
#ifdef SHORT_NAMES
        char *old_end = end;
#endif

        Trace((stderr, "appending dir segment [%s]\n", pathcomp));
        while ((*end = *pathcomp++) != '\0')
            ++end;
#ifdef SHORT_NAMES   /* path components restricted to 14 chars, typically */
        if ((end-old_end) > FILENAME_MAX)  /* GRR:  proper constant? */
            *(end = old_end + FILENAME_MAX) = '\0';
#endif

        /* GRR:  could do better check, see if overrunning buffer as we go:
         * check end-buildpath after each append, set warning variable if
         * within 20 of FILNAMSIZ; then if var set, do careful check when
         * appending.  Clear variable when begin new path. */

        if ((end-buildpath) > FILNAMSIZ-3)  /* need '/', one-char name, '\0' */
            too_long = TRUE;                /* check if extracting directory? */
        if (stat(buildpath, &G.statbuf)) {  /* path doesn't exist */
            if (!G.create_dirs) { /* told not to create (freshening) */
                free(buildpath);
                return 2;         /* path doesn't exist:  nothing to do */
            }
            if (too_long) {
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  path too long: %s\n", buildpath));
                free(buildpath);
                return 4;         /* no room for filenames:  fatal */
            }
            if (mkdir(buildpath, 0777) == -1) {   /* create the directory */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  cannot create %s\n\
                 unable to process %s.\n", buildpath, G.filename));
                free(buildpath);
                return 3;      /* path didn't exist, tried to create, failed */
            }
            created_dir = TRUE;
        } else if (!S_ISDIR(G.statbuf.st_mode)) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  %s exists but is not directory\n\
                 unable to process %s.\n", buildpath, G.filename));
            free(buildpath);
            return 3;          /* path existed but wasn't dir */
        }
        if (too_long) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  path too long: %s\n", buildpath));
            free(buildpath);
            return 4;         /* no room for filenames:  fatal */
        }
        *end++ = '/';
        *end = '\0';
        Trace((stderr, "buildpath now = [%s]\n", buildpath));
        return 0;

    } /* end if (FUNCTION == APPEND_DIR) */

/*---------------------------------------------------------------------------
    GETPATH:  copy full path to the string pointed at by pathcomp, and free
    buildpath.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == GETPATH) {
        strcpy(pathcomp, buildpath);
        Trace((stderr, "getting and freeing path [%s]\n", pathcomp));
        free(buildpath);
        buildpath = end = (char *)NULL;
        return 0;
    }

/*---------------------------------------------------------------------------
    APPEND_NAME:  assume the path component is the filename; append it and
    return without checking for existence.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_NAME) {
#ifdef SHORT_NAMES
        char *old_end = end;
#endif

        Trace((stderr, "appending filename [%s]\n", pathcomp));
        while ((*end = *pathcomp++) != '\0') {
            ++end;
#ifdef SHORT_NAMES  /* truncate name at 14 characters, typically */
            if ((end-old_end) > FILENAME_MAX)      /* GRR:  proper constant? */
                *(end = old_end + FILENAME_MAX) = '\0';
#endif
            if ((end-buildpath) >= FILNAMSIZ) {
                *--end = '\0';
                Info(slide, 0x201, ((char *)slide,
                  "checkdir warning:  path too long; truncating\n\
                   %s\n                -> %s\n", G.filename, buildpath));
                return 1;   /* filename truncated */
            }
        }
        Trace((stderr, "buildpath now = [%s]\n", buildpath));
        return 0;  /* could check for existence here, prompt for new name... */
    }

/*---------------------------------------------------------------------------
    INIT:  allocate and initialize buffer space for the file currently being
    extracted.  If file was renamed with an absolute path, don't prepend the
    extract-to path.
  ---------------------------------------------------------------------------*/

/* GRR:  for VMS and TOPS-20, add up to 13 to strlen */

    if (FUNCTION == INIT) {
        Trace((stderr, "initializing buildpath to "));
        if ((buildpath = (char *)malloc(strlen(G.filename)+rootlen+1)) ==
            (char *)NULL)
            return 10;
        if ((rootlen > 0) && !renamed_fullpath) {
            strcpy(buildpath, rootpath);
            end = buildpath + rootlen;
        } else {
            *buildpath = '\0';
            end = buildpath;
        }
        Trace((stderr, "[%s]\n", buildpath));
        return 0;
    }

/*---------------------------------------------------------------------------
    ROOT:  if appropriate, store the path in rootpath and create it if neces-
    sary; else assume it's a zipfile member and return.  This path segment
    gets used in extracting all members from every zipfile specified on the
    command line.
  ---------------------------------------------------------------------------*/

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (FUNCTION == ROOT) {
        Trace((stderr, "initializing root path to [%s]\n", pathcomp));
        if (pathcomp == (char *)NULL) {
            rootlen = 0;
            return 0;
        }
        if (rootlen > 0)        /* rootpath was already set, nothing to do */
            return 0;
        if ((rootlen = strlen(pathcomp)) > 0) {
            char *tmproot;

            if ((tmproot = (char *)malloc(rootlen+2)) == (char *)NULL) {
                rootlen = 0;
                return 10;
            }
            strcpy(tmproot, pathcomp);
            if (tmproot[rootlen-1] == '/') {
                tmproot[--rootlen] = '\0';
            }
            if (rootlen > 0 && (stat(tmproot, &G.statbuf) ||
                !S_ISDIR(G.statbuf.st_mode)))       /* path does not exist */
            {
                if (!G.create_dirs /* || iswild(tmproot) */ ) {
                    free(tmproot);
                    rootlen = 0;
                    return 2;   /* skip (or treat as stored file) */
                }
                /* create the directory (could add loop here scanning tmproot
                 * to create more than one level, but why really necessary?) */
                if (mkdir(tmproot, 0777) == -1) {
                    Info(slide, 1, ((char *)slide,
                      "checkdir:  cannot create extraction directory: %s\n",
                      tmproot));
                    free(tmproot);
                    rootlen = 0;  /* path didn't exist, tried to create, and */
                    return 3; /* failed:  file exists, or 2+ levels required */
                }
            }
            tmproot[rootlen++] = '/';
            tmproot[rootlen] = '\0';
            if ((rootpath = (char *)realloc(tmproot, rootlen+1)) == NULL) {
                free(tmproot);
                rootlen = 0;
                return 10;
            }
            Trace((stderr, "rootpath now = [%s]\n", rootpath));
        }
        return 0;
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    END:  free rootpath, immediately prior to program exit.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == END) {
        Trace((stderr, "freeing rootpath\n"));
        if (rootlen > 0) {
            free(rootpath);
            rootlen = 0;
        }
        return 0;
    }

    return 99;  /* should never reach */

} /* end function checkdir() */






/****************************/
/* Function close_outfile() */
/****************************/

void close_outfile(__G)    /* GRR: change to return PK-style warning level */
    __GDEF
{
    iztimes zt;
    ush z_uidgid[2];
    unsigned eb_izux_flg;

/*---------------------------------------------------------------------------
    If symbolic links are supported, allocate a storage area, put the uncom-
    pressed "data" in it, and create the link.  Since we know it's a symbolic
    link to start with, we shouldn't have to worry about overflowing unsigned
    ints with unsigned longs.
  ---------------------------------------------------------------------------*/

#ifdef SYMLINKS
    if (G.symlnk) {
        unsigned ucsize = (unsigned)G.lrec.ucsize;
        char *linktarget = (char *)malloc((unsigned)G.lrec.ucsize+1);

        fclose(G.outfile);                      /* close "data" file... */
        G.outfile = fopen(G.filename, FOPR);    /* ...and reopen for reading */
        if (!linktarget || fread(linktarget, 1, ucsize, G.outfile) !=
                           (int)ucsize)
        {
            Info(slide, 0x201, ((char *)slide,
              "warning:  symbolic link (%s) failed\n", G.filename));
            if (linktarget)
                free(linktarget);
            fclose(G.outfile);
            return;
        }
        fclose(G.outfile);                  /* close "data" file for good... */
        unlink(G.filename);                 /* ...and delete it */
        linktarget[ucsize] = '\0';
        if (QCOND2)
            Info(slide, 0, ((char *)slide, "-> %s ", linktarget));
        if (symlink(linktarget, G.filename))  /* create the real link */
            perror("symlink error");

        if (!uO.J_flag) {
            /* Symlinks can have attributes, too. */
            void *ptr = scanBeOSexfield( G.extra_field,
                                         G.lrec.extra_field_length );
            if (ptr) {
                setBeOSexfield( G.filename, ptr );
            }
        }

        free(linktarget);
        return;                             /* can't set time on symlinks */
    }
#endif /* SYMLINKS */

    fclose(G.outfile);

    /* handle the BeOS extra field if present */
    if (!uO.J_flag) {
        void *ptr = scanBeOSexfield( G.extra_field,
                                     G.lrec.extra_field_length );

        if (ptr) {
            setBeOSexfield( G.filename, ptr );
#ifdef BEOS_ASSIGN_FILETYPE
        } else {
            /* Otherwise, ask the system to try assigning a MIME type. */
            assign_MIME( G.filename );
#endif
        }
    }

/*---------------------------------------------------------------------------
    Change the file permissions from default ones to those stored in the
    zipfile.
  ---------------------------------------------------------------------------*/

#ifndef NO_CHMOD
    if (chmod(G.filename, 0xffff & G.pInfo->file_attr))
        perror("chmod (file attributes) error");
#endif

/*---------------------------------------------------------------------------
    Convert from MSDOS-format local time and date to Unix-format 32-bit GMT
    time:  adjust base year from 1980 to 1970, do usual conversions from
    yy/mm/dd hh:mm:ss to elapsed seconds, and account for timezone and day-
    light savings time differences.  If we have a Unix extra field, however,
    we're laughing:  both mtime and atime are ours.  On the other hand, we
    then have to check for restoration of UID/GID.
  ---------------------------------------------------------------------------*/

    eb_izux_flg = (G.extra_field ? ef_scan_for_izux(G.extra_field,
                   G.lrec.extra_field_length, 0, G.lrec.last_mod_dos_datetime,
#ifdef IZ_CHECK_TZ
                   (G.tz_is_valid ? &zt : NULL),
#else
                   &zt,
#endif
                   z_uidgid) : 0);
    if (eb_izux_flg & EB_UT_FL_MTIME) {
        TTrace((stderr, "\nclose_outfile:  Unix e.f. modif. time = %ld\n",
          zt.mtime));
    } else {
        zt.mtime = dos_to_unix_time(G.lrec.last_mod_dos_datetime);
    }
    if (eb_izux_flg & EB_UT_FL_ATIME) {
        TTrace((stderr, "close_outfile:  Unix e.f. access time = %ld\n",
          zt.atime));
    } else {
        zt.atime = zt.mtime;
        TTrace((stderr, "\nclose_outfile:  modification/access times = %ld\n",
          zt.mtime));
    }

    /* if -X option was specified and we have UID/GID info, restore it */
    if (uO.X_flag && eb_izux_flg & EB_UX2_VALID) {
        TTrace((stderr, "close_outfile:  restoring Unix UID/GID info\n"));
        if (chown(G.filename, (uid_t)z_uidgid[0], (gid_t)z_uidgid[1]))
        {
            if (uO.qflag)
                Info(slide, 0x201, ((char *)slide,
                  "warning:  cannot set UID %d and/or GID %d for %s\n",
                  z_uidgid[0], z_uidgid[1], G.filename));
            else
                Info(slide, 0x201, ((char *)slide,
                  " (warning) cannot set UID %d and/or GID %d",
                  z_uidgid[0], z_uidgid[1]));
        }
    }

    /* set the file's access and modification times */
    if (utime(G.filename, (struct utimbuf *)&zt)) {
        if (uO.qflag)
            Info(slide, 0x201, ((char *)slide,
              "warning:  cannot set time for %s\n", G.filename));
        else
            Info(slide, 0x201, ((char *)slide,
              " (warning) cannot set time"));
    }

} /* end function close_outfile() */




#ifdef SET_DIR_ATTRIB
/* messages of code for setting directory attributes */
static char Far DirlistUidGidFailed[] =
  "warning:  cannot set UID %d and/or GID %d for %s\n";
static char Far DirlistUtimeFailed[] =
  "warning:  cannot set modification, access times for %s\n";
#  ifndef NO_CHMOD
  static char Far DirlistChmodFailed[] =
    "warning:  cannot set permissions for %s\n";
#  endif


int set_direc_attribs(__G__ d)
    __GDEF
    dirtime *d;
{
    int errval = PK_OK;

    if (d->have_uidgid &&
        chown(d->fn, (uid_t)d->uidgid[0], (gid_t)d->uidgid[1]))
    {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(DirlistUidGidFailed),
          d->uidgid[0], d->uidgid[1], d->fn));
        if (!errval)
            errval = PK_WARN;
    }
    if (utime(d->fn, (const struct utimbuf *)&d->u.t2)) {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(DirlistUtimeFailed), d->fn));
        if (!errval)
            errval = PK_WARN;
    }
#ifndef NO_CHMOD
    if (chmod(d->fn, 0xffff & d->perms)) {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(DirlistChmodFailed), d->fn));
        /* perror("chmod (file attributes) error"); */
        if (!errval)
            errval = PK_WARN;
    }
#endif /* !NO_CHMOD */
    return errval;
} /* end function set_directory_attributes() */

#endif /* SET_DIR_ATTRIB */




#ifdef TIMESTAMP

/***************************/
/*  Function stamp_file()  */
/***************************/

int stamp_file(fname, modtime)
    ZCONST char *fname;
    time_t modtime;
{
    struct utimbuf tp;

    tp.modtime = tp.actime = modtime;
    return (utime(fname, &tp));

} /* end function stamp_file() */

#endif /* TIMESTAMP */




#ifndef SFX

/************************/
/*  Function version()  */
/************************/

void version(__G)
    __GDEF
{
    sprintf((char *)slide, LoadFarString(CompiledWith),
#if defined(__MWERKS__)
      "Metrowerks CodeWarrior", "",
#elif defined(__GNUC__)
      "GNU C ", __VERSION__,
#endif
      "BeOS ",

#ifdef __POWERPC__
      "(PowerPC)",
#else
# ifdef __INTEL__
      "(x86)",
# else
      "(unknown)",   /* someday we may have other architectures... */
# endif
#endif

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
    );

    (*G.message)((zvoid *)&G, slide, (ulg)strlen((char *)slide), 0);

} /* end function version() */

#endif /* !SFX */



/******************************/
/* Extra field functions      */
/******************************/

/*
** Scan the extra fields in extra_field, and look for a BeOS EF; return a
** pointer to that EF, or NULL if it's not there.
*/
static uch *scanBeOSexfield( const uch *ef_ptr, unsigned ef_len )
{
    while( ef_ptr != NULL && ef_len >= EB_HEADSIZE ) {
        unsigned eb_id  = makeword(EB_ID + ef_ptr);
        unsigned eb_len = makeword(EB_LEN + ef_ptr);

        if (eb_len > (ef_len - EB_HEADSIZE)) {
            Trace((stderr,
              "scanBeOSexfield: block length %u > rest ef_size %u\n", eb_len,
              ef_len - EB_HEADSIZE));
            break;
        }

        if( eb_id == EF_BEOS && eb_len >= EB_BEOS_HLEN ) {
            return (uch *)ef_ptr;
        }

        ef_ptr += (eb_len + EB_HEADSIZE);
        ef_len -= (eb_len + EB_HEADSIZE);
    }

    return NULL;
}

/* Used by setBeOSexfield():

Set a file/directory's attributes to the attributes passed in.

If set_file_attrs() fails, an error will be returned:

     EOK - no errors occurred

(other values will be whatever the failed function returned; no docs
yet, or I'd list a few)
*/
static int set_file_attrs( const char *name,
                           const unsigned char *attr_buff,
                           const off_t attr_size )
{
    int                  retval = EOK;
    unsigned char       *ptr;
    const unsigned char *guard;
    int                  fd;

    ptr   = (unsigned char *)attr_buff;
    guard = ptr + attr_size;

    fd = open( name, O_RDWR | O_NOTRAVERSE );
    if( fd < 0 ) {
        return errno; /* should it be -fd ? */
    }

    while( ptr < guard ) {
        ssize_t              wrote_bytes;
        struct attr_info     fa_info;
        const char          *attr_name;
        unsigned char       *attr_data;

        attr_name  = (char *)&(ptr[0]);
        ptr       += strlen( attr_name ) + 1;

        /* The attr_info data is stored in big-endian format because the */
        /* PowerPC port was here first.                                  */
        memcpy( &fa_info, ptr, sizeof( struct attr_info ) );
        fa_info.type = (uint32)B_BENDIAN_TO_HOST_INT32( fa_info.type );
        fa_info.size = (off_t)B_BENDIAN_TO_HOST_INT64( fa_info.size );
        ptr     += sizeof( struct attr_info );

        if( fa_info.size < 0LL ) {
            Info(slide, 0x201, ((char *)slide,
                 "warning: skipping attribute with invalid length (%Ld)\n",
                 fa_info.size));
            break;
        }

        attr_data  = ptr;
        ptr       += fa_info.size;

        if( ptr > guard ) {
            /* We've got a truncated attribute. */
            Info(slide, 0x201, ((char *)slide,
                 "warning: truncated attribute\n"));
            break;
        }

        /* Wave the magic wand... this will swap Be-known types properly. */
        (void)swap_data( fa_info.type, attr_data, fa_info.size,
                         B_SWAP_BENDIAN_TO_HOST );

        wrote_bytes = fs_write_attr( fd, attr_name, fa_info.type, 0,
                                     attr_data, fa_info.size );
        if( wrote_bytes != fa_info.size ) {
            Info(slide, 0x201, ((char *)slide,
                 "warning: wrote %ld attribute bytes of %ld\n",
                 (unsigned long)wrote_bytes,(unsigned long)fa_info.size));
        }
    }

    close( fd );

    return retval;
}

static void setBeOSexfield( const char *path, uch *extra_field )
{
    uch *ptr       = extra_field;
    ush  id        = 0;
    ush  size      = 0;
    ulg  full_size = 0;
    uch  flags     = 0;
    uch *attrbuff  = NULL;
    int retval;

    if( extra_field == NULL ) {
        return;
    }

    /* Collect the data from the extra field buffer. */
    id        = makeword( ptr );    ptr += 2;   /* we don't use this... */
    size      = makeword( ptr );    ptr += 2;
    full_size = makelong( ptr );    ptr += 4;
    flags     = *ptr;               ptr++;

    /* Do a little sanity checking. */
    if( flags & EB_BE_FL_BADBITS ) {
        /* corrupted or unsupported */
        Info(slide, 0x201, ((char *)slide,
             "Unsupported flags set for this BeOS extra field, skipping.\n"));
        return;
    }
    if( size <= EB_BEOS_HLEN ) {
        /* corrupted, unsupported, or truncated */
        Info(slide, 0x201, ((char *)slide,
             "BeOS extra field is %d bytes, should be at least %d.\n", size,
             EB_BEOS_HLEN));
        return;
    }
    if( full_size < ( size - EB_BEOS_HLEN ) ) {
        /* possible old archive? will this screw up on valid archives? */
        Info(slide, 0x201, ((char *)slide,
             "Skipping attributes: BeOS extra field is %d bytes, "
             "data size is %ld.\n", size - EB_BEOS_HLEN, full_size));
        return;
    }

    /* Find the BeOS file attribute data. */
    if( flags & EB_BE_FL_UNCMPR ) {
        /* Uncompressed data */
        attrbuff = ptr;
    } else {
        /* Compressed data */
        attrbuff = (uch *)malloc( full_size );
        if( attrbuff == NULL ) {
            /* No memory to uncompress attributes */
            Info(slide, 0x201, ((char *)slide,
                 "Can't allocate memory to uncompress file attributes.\n"));
            return;
        }

        retval = memextract( __G__ attrbuff, full_size,
                             ptr, size - EB_BEOS_HLEN );
        if( retval != PK_OK ) {
            /* error uncompressing attributes */
            Info(slide, 0x201, ((char *)slide,
                 "Error uncompressing file attributes.\n"));

            /* Some errors here might not be so bad; we should expect */
            /* some truncated data, for example.  If the data was     */
            /* corrupt, we should _not_ attempt to restore the attrs  */
            /* for this file... there's no way to detect what attrs   */
            /* are good and which are bad.                            */
            free( attrbuff );
            return;
        }
    }

    /* Now attempt to set the file attributes on the extracted file. */
    retval = set_file_attrs( path, attrbuff, (off_t)full_size );
    if( retval != EOK ) {
        Info(slide, 0x201, ((char *)slide,
             "Error writing file attributes.\n"));
    }

    /* Clean up, if necessary */
    if( attrbuff != ptr ) {
        free( attrbuff );
    }

    return;
}

#ifdef BEOS_USE_PRINTEXFIELD
static void printBeOSexfield( int isdir, uch *extra_field )
{
    uch *ptr       = extra_field;
    ush  id        = 0;
    ush  size      = 0;
    ulg  full_size = 0;
    uch  flags     = 0;

    /* Tell picky compilers to be quiet. */
    isdir = isdir;

    if( extra_field == NULL ) {
        return;
    }

    /* Collect the data from the buffer. */
    id        = makeword( ptr );    ptr += 2;
    size      = makeword( ptr );    ptr += 2;
    full_size = makelong( ptr );    ptr += 4;
    flags     = *ptr;               ptr++;

    if( id != EF_BEOS ) {
        /* not a 'Be' field */
        printf("\t*** Unknown field type (0x%04x, '%c%c')\n", id,
               (char)(id >> 8), (char)id);
    }

    if( flags & EB_BE_FL_BADBITS ) {
        /* corrupted or unsupported */
        printf("\t*** Corrupted BeOS extra field:\n");
        printf("\t*** unknown bits set in the flags\n");
        printf("\t*** (Possibly created by an old version of zip for BeOS.\n");
    }

    if( size <= EB_BEOS_HLEN ) {
        /* corrupted, unsupported, or truncated */
        printf("\t*** Corrupted BeOS extra field:\n");
        printf("\t*** size is %d, should be larger than %d\n", size,
               EB_BEOS_HLEN );
    }

    if( flags & EB_BE_FL_UNCMPR ) {
        /* Uncompressed data */
        printf("\tBeOS extra field data (uncompressed):\n");
        printf("\t\t%ld data bytes\n", full_size);
    } else {
        /* Compressed data */
        printf("\tBeOS extra field data (compressed):\n");
        printf("\t\t%d compressed bytes\n", size - EB_BEOS_HLEN);
        printf("\t\t%ld uncompressed bytes\n", full_size);
    }
}
#endif

#ifdef BEOS_ASSIGN_FILETYPE
/* Note: This will no longer be necessary in BeOS PR4; update_mime_info()    */
/* will be updated to build its own absolute pathname if it's not given one. */
static void assign_MIME( const char *file )
{
    char *fullname;
    char buff[PATH_MAX], cwd_buff[PATH_MAX];
    int retval;

    if( file[0] == '/' ) {
        fullname = (char *)file;
    } else {
        sprintf( buff, "%s/%s", getcwd( cwd_buff, PATH_MAX ), file );
        fullname = buff;
    }

    retval = update_mime_info( fullname, FALSE, TRUE, TRUE );
}
#endif
