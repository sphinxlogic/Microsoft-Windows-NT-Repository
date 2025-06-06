/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  theos.c

  Theos-specific routines for use with Info-ZIP's UnZip 5.41 and later.

  Contains:  do_wild()           <-- generic enough to put in fileio.c?
             mapattr()
             mapname()
             checkdir()
             close_outfile()
             set_direc_attribs()
             stamp_file()
             version()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"
#include <direct.h>
#include <sc.h>
#include <fdb.h>
#include <nuc.h>
#include <peek.h>

/* standard function doesn't work with a trailing / */
#define opendir(a) _opendir(a)
extern DIR* _opendir(const char* dirpath);

static int created_dir;        /* used in mapname(), checkdir() */
static int renamed_fullpath;   /* ditto */

#define _FDB_SHARED_EXECUTE_PROTECT _FDB_MODIFIED

uch _um2tm_(ush mask);

int chmodv2_3(const char *fname, short mask)
{
    return _filechange(fname,'p',(size_t) _um2tm_(mask)|0x80);
}

int chlen(char *fname, unsigned short reclen, unsigned short keylen)
{
    size_t a = reclen + ((size_t) keylen << 16);
    return _filechange(fname,'ma',(size_t) &a);
}

#define chgrow(a,b) ((int) _filechange(a,'g',(size_t)(b)))
#define chorg(a,b)  ((int) _filechange(a,'m',(size_t)(b)))

#ifndef SFX

/*************************/
/* Function dateformat() */
/*************************/

int dateformat()
{
/*---------------------------------------------------------------------------
    For those operating systems that support it, this function returns a
    value that tells how national convention says that numeric dates are
    displayed.  Return values are DF_YMD, DF_DMY and DF_MDY (the meanings
    should be fairly obvious).
  ---------------------------------------------------------------------------*/

    switch (peeknuc(&NUC->dateopt) & (aform|eform|iform)) {
    case aform: return DF_MDY;
    case eform: return DF_DMY;
    }
    return DF_YMD;
}


/* usual THEOS match function for filenames */

/* match from Phase One Systems */

/* Returns non-zero if string matches the literal mask */
int match(string, pattern, ignore_case)
    ZCONST char *string;
    ZCONST char *pattern;
    int ignore_case;            /* unused in this variant of match()! */
{
    int matched, k;

    if (!(*pattern))
        return 1;
    for(;;) {
        if ( (!(*string)) && (!(*pattern)) )
            return(1);
        else if ( !(*pattern) )
            return(0);
        else if (*pattern == '*') {
            if (!*(pattern+1))
                return(1);
            k=0;
            do {
                matched = match(string+k, pattern+1, ignore_case);
                k++;
            } while ( (!matched) && *(string+k));
            return(matched);
        } else if (*pattern == '@') {
            if (!((*string >= 'a' && *string <= 'z')
                ||(*string >= 'A' && *string <= 'Z')))
                return(0);
        } else if (*pattern == '#') {
            if (*string < '0' || *string > '9')
                return(0);
        } else if (*pattern != '?') {
            if (toupper(*string) != toupper(*pattern))
                return(0);

        }
        string++; pattern++;
    }
} /* end function match() */



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
        if ((wildname = (ZCONST char *)strrchr(wildspec, '/')) ==
            (ZCONST char *)NULL)
        {
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
                Trace((stderr, "do_wild:  readdir returns %s\n", file->d_name));
                if (match(file->d_name, wildname, 1)) {   /* 1 == ignore case */
                    Trace((stderr, "do_wild:  match() succeeds\n"));
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
        Trace((stderr, "do_wild:  readdir returns %s\n", file->d_name));
        if (file->d_name[0] == '.' && wildname[0] != '.')
            continue;   /* Unix:  '*' and '?' do not match leading dot */
        if (match(file->d_name, wildname, 1)) {   /* 1 == ignore case */
            Trace((stderr, "do_wild:  match() succeeds\n"));
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

    G.pInfo->file_attr = 0;
    /* initialized to 0 for check in "default" branch below... */

    switch (G.pInfo->hostnum) {
        case AMIGA_:
            tmp = (unsigned)(tmp>>17 & 7);   /* Amiga RWE bits */
            G.pInfo->file_attr = (unsigned)(tmp<<6 | tmp<<3 | tmp);
            break;
        case UNIX_:
        case VMS_:
        case ACORN_:
        case ATARI_:
        case BEOS_:
        case QDOS_:
        case TANDEM_:
        case THEOS_:
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            if (G.pInfo->file_attr != 0 || !G.extra_field) {
                return 0;
            } else {
                /* Some (non-Info-ZIP) implementations of Zip for Unix and
                 * VMS (and probably others ??) leave 0 in the upper 16-bit
                 * part of the external_file_attributes field. Instead, they
                 * store file permission attributes in some extra field.
                 * As a work-around, we search for the presence of one of
                 * these extra fields and fall back to the MSDOS compatible
                 * part of external_file_attributes if one of the known
                 * e.f. types has been detected.
                 * Later, we might implement extraction of the permission
                 * bits from the VMS extra field. But for now, the work-around
                 * should be sufficient to provide "readable" extracted files.
                 * (For ASI Unix e.f., an experimental remap from the e.f.
                 * mode value IS already provided!)
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
    G.pInfo->file_attr &= ~tmp;

    return 0;

} /* end function mapattr() */



/* portabilibity functions to ensure access to port 2.0 and new ports */

int isv2_3()
{
    return G.extra_field[0] == 'T' && G.extra_field[1] == 'h';
}

int isv2_3lib()
{
    return isv2_3() && S_ISLIB((G.pInfo->file_attr >> 8) & 0xFF);
}

int isv2_3dir()
{
    return isv2_3() && S_ISDIR((G.pInfo->file_attr >> 8) & 0xFF);
}

#ifdef OLD_THEOS_EXTRA
#define S_IFMT_     0xf800      /* type of file */
#define S_IFLIB_    0x8000      /* library */
#define S_IFDIR_    0x4000      /* directory */

#define S_ISLIB_(a) (((a) & 0xff) == _FDB_STAT_LIBRARY)
#define S_ISDIR_(a) (((a) & 0xff) == _FDB_STAT_DIRECTORY)

struct extra_block
{
    ush     signature;
    ush     size;
    ush     flags;
    ulg     filesize;
    ush     reclen;
    ush     keylen;
    uch     filegrow;
    uch     reserved[3];
};

struct extra_block *v2_0extra()
{
    return (struct extra_block *) G.extra_field;
}

int isv2_0()
{
    return ((struct extra_block *) G.extra_field)->signature == 0x4854;
}

int isv2_0lib()
{
  return isv2_0() && S_ISLIB_(G.pInfo->file_attr);
}

int isv2_0dir()
{
  return isv2_0() && S_ISDIR_(G.pInfo->file_attr);
}

#define islib() (isv2_0lib() || isv2_3lib())
#define isdir() (isv2_0dir() || isv2_3dir())
#define chmodv2_0(a,b)  ((int) _sc_168(a,'p',(size_t)(b)|0x80))
#undef chmod
#define chmod(a,b) (isv2_0() && chmodv2_0(a,b))||(isv2_3() && chmodv2_3(a,b))
#else
#define islib() isv2_3lib()
#define isdir() isv2_3dir()
#undef chmod
#define chmod(a,b) chmodv2_0(a,b)
#endif

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
#ifdef ACORN_FTYPE_NFS
    char *lastcomma=(char *)NULL;  /* pointer to last comma in pathcomp */
#endif
    int error = 0;
    register unsigned workch;      /* hold the character being tested */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    if (G.pInfo->vollabel)
        return IZ_VOL_LABEL;    /* can't set disk volume labels in Unix */

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

        switch (workch) {
        case '/':             /* can assume -j flag not given */
            *pp = '\0';
            if ((error = checkdir(__G__ pathcomp, APPEND_DIR)) > 1)
                return error;
            pp = pathcomp;    /* reset conversion buffer for next piece */
            lastsemi = (char *)NULL; /* leave directory semi-colons alone */
            break;
        default:
            if (isfnsym(workch) || workch == '.')
                *pp++ = (char)workch;
            else
                *pp++ = '_';
        } /* end switch */

    } /* end while loop */

    *pp = '\0';                   /* done with pathcomp:  terminate it */

/*---------------------------------------------------------------------------
    Report if directory was created (and no file to create:  filename ended
    in '/'), check name to be sure it exists, and combine path and name be-
    fore exiting.
  ---------------------------------------------------------------------------*/

    if (G.filename[strlen(G.filename) - 1] == '/') {
        checkdir(__G__ G.filename, GETPATH);
        if (islib() && G.filename[strlen(G.filename) - 1] == '/')
            G.filename[strlen(G.filename) - 1] = '\0';

        if (created_dir) {
            if (QCOND2) {
                Info(slide, 0, ((char *)slide, "   creating: %s\n",
                  G.filename));
            }
            return IZ_CREATED_DIR;   /* set dir time (note trailing '/') */
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




#if 0  /*========== NOTES ==========*/

  extract-to dir:      a:path/
  buildpath:           path1/path2/ ...   (NULL-terminated)
  pathcomp:                filename

  mapname():
    loop over chars in zipfile member name
      checkdir(path component, COMPONENT | CREATEDIR) --> map as required?
        (d:/tmp/unzip/)                    (disk:[tmp.unzip.)
        (d:/tmp/unzip/jj/)                 (disk:[tmp.unzip.jj.)
        (d:/tmp/unzip/jj/temp/)            (disk:[tmp.unzip.jj.temp.)
    finally add filename itself and check for existence? (could use with rename)
        (d:/tmp/unzip/jj/temp/msg.outdir)  (disk:[tmp.unzip.jj.temp]msg.outdir)
    checkdir(name, GETPATH)     -->  copy path to name and free space

#endif /* 0 */




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
    static char rootdisk[3];  /* user's "extract-to" disk */
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
        Trace((stderr, "appending disk segment [%s]\n", rootdisk));
        strcat(buildpath, rootdisk);
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
            if (islib()) {
                ulg size;
#ifdef OLD_THEOS_EXTRA
                if (isv2_0lib())
                    size = v2_0extra()->filesize;
                else
#endif
                {
                    size = (ulg) G.extra_field[5] |
                           ((ulg) G.extra_field[6] << 8) |
                           ((ulg) G.extra_field[7] << 16) |
                           ((ulg) G.extra_field[8] << 24);
                }
                if (makelib(buildpath, size / 64)) {
                    Info(slide, 1, ((char*)slide,
                      "checkdir error:  can't create library %s\n\
                     unable to process %s.\n", buildpath, G.filename);
                    free(buildpath);
                    return 3; /* path didn't exist, tried to create, failed */
                }
            } else if (mkdir(buildpath) == -1) {   /* create the directory */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  cannot create %s\n\
                 unable to process %s.\n", buildpath, G.filename));
                free(buildpath);
                return 3;      /* path didn't exist, tried to create, failed */
            }
            created_dir = TRUE;
        } else if (!S_ISDIR(G.statbuf.st_mode) && isdir()) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  %s exists but is not directory\n\
                 unable to process %s.\n", buildpath, G.filename));
            free(buildpath);
            return 3;          /* path existed but wasn't dir */
        } else if (!S_ISLIB(G.statbuf.st_mode) && islib()) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  %s exists but is not library\n\
                 unable to process %s.\n", buildpath, G.filename));
            free(buildpath);
            return 3;          /* path existed but wasn't lib */
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
        strcat(buildpath, rootdisk);
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
#ifdef ACORN_FTYPE_NFS
        if ((buildpath = (char *)malloc(strlen(G.filename)+rootlen+
                                        (uO.acorn_nfs_ext ? 5 : 1)))
#else
        if ((buildpath = (char *)malloc(strlen(G.filename)+rootlen+1))
#endif
            == (char *)NULL)
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
        if (pathcomp == (char *)NULL) {
            rootlen = 0;
            return 0;
        }
        if (rootlen > 0)        /* rootpath was already set, nothing to do */
            return 0;
        if ((rootlen = strlen(pathcomp)) > 0) {
            int prepend_slash = 0;
            char *tmproot, *p;

            if (*pathcomp == ':') {
                preprend_slash = 1;
                rootlen++;
            }
            if ((tmproot = (char *)malloc(rootlen+2)) == (char *)NULL) {
                rootlen = 0;
                return 10;
            }
            if (prepend_slash)
                strcpy(tmproot, "/");
            else
                *tmproot = '\0';
            strcat(tmproot, pathcomp);
            Trace((stderr, "initializing root path to [%s]\n", tmproot));
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
                if (mkdir(tmproot) == -1) {
                    Info(slide, 1, ((char *)slide,
                      "checkdir:  cannot create extraction directory: %s\n",
                      tmproot));
                    free(tmproot);
                    rootlen = 0;  /* path didn't exist, tried to create, and */
                    return 3; /* failed:  file exists, or 2+ levels required */
                }
            }
            /* split rootpath in path and disk */
            if ((p = strchr(tmproot, ':')) != NULL) {
                strncpy(rootdisk, p, 2);
                rootdisk[2] = '\0';
                *p = '\0';
                rootlen = p - tmproot;
            } else
                rootdisk[0] = '\0';
            if (rootpath[rootlen - 1] != '/') {
                rootpath[rootlen++] = '/';
                rootpath[rootlen] = '\0';
            }
            if ((rootpath = (char *)realloc(tmproot, rootlen+1)) == NULL) {
                free(tmproot);
                rootlen = 0;
                return 10;
            }
            Trace((stderr, "rootpath now = [%s], rootdisk now = [%s]\n",
                rootpath, rootdisk));
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




#ifdef MORE

/**************************/
/* Function screenlines() */
/**************************/

int screenlines()
{
    return getpl(27) + 1;
}

#endif /* MORE */





#ifndef MTS

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

    fclose(G.outfile);

/*---------------------------------------------------------------------------
    Convert from MSDOS-format local time and date to Unix-format 32-bit GMT
    time:  adjust base year from 1980 to 1970, do usual conversions from
    yy/mm/dd hh:mm:ss to elapsed seconds, and account for timezone and day-
    light savings time differences.  If we have a Unix extra field, however,
    we're laughing:  both mtime and atime are ours.
  ---------------------------------------------------------------------------*/

#ifdef USE_EF_UT_TIME
    eb_izux_flg = (G.extra_field ? ef_scan_for_izux(G.extra_field,
                   G.lrec.extra_field_length, 0, G.lrec.last_mod_dos_datetime,
#ifdef IZ_CHECK_TZ
                   (G.tz_is_valid ? &zt : NULL),
#else
                   &zt,
#endif
                   z_uidgid) : 0);
    if (eb_izux_flg & EB_UT_FL_MTIME) {
        TTrace((stderr, "\nclose_outfile:  Theos e.f. modif. time = %ld\n",
          zt.mtime));
    } else {
        zt.mtime = dos_to_unix_time(G.lrec.last_mod_dos_datetime);
    }
    if (eb_izux_flg & EB_UT_FL_ATIME) {
        TTrace((stderr, "close_outfile:  Theos e.f. access time = %ld\n",
          zt.atime));
    } else {
        zt.atime = zt.mtime;
        TTrace((stderr, "\nclose_outfile:  modification/access times = %ld\n",
          zt.mtime));
    }
#else
    zt.mtime = dos_to_unix_time(G.lrec.last_mod_dos_datetime);
#endif
    /* set the file's access and modification times */
    if (utime(G.filename, (ztimbuf *)&zt)) {
        if (uO.qflag)
            Info(slide, 0x201, ((char *)slide,
              "warning:  cannot set times for %s\n", G.filename));
        else
            Info(slide, 0x201, ((char *)slide,
              " (warning) cannot set times"));
    }

/*---------------------------------------------------------------------------
    Change the file permissions from default ones to those stored in the
    zipfile.
  ---------------------------------------------------------------------------*/

    if (chmod(G.filename, 0xffff & G.pInfo->file_attr))
        perror("chmod (file attributes) error");

/*---------------------------------------------------------------------------
    Change the file structure and set native .
  ---------------------------------------------------------------------------*/

    if (isv2_3()) {
        chorg(G.filename, G.extra_field[9]);
        chlen(G.filename,
            (ush) G.extra_field[10] | ((ush) G.extra_field[11] << 8),
            (ush) G.extra_field[12] | ((ush) G.extra_field[13] << 8));
        chgrow(G.filename, G.extra_field[14]);
    }
#if OLD_THEOS_EXTRA
     else if (isv2_0()) {
        chorg(G.filename, G.pInfo->file_attr & 0xFF);
        chlen(G.filename, v2_0extra()->reclen, v2_0extra()->keylen);
        chgrow(G.filename, v2_0extra()->filegrow);
    }
#endif
} /* end function close_outfile() */

#endif /* !MTS */




#ifdef SET_DIR_ATTRIB
/* messages of code for setting directory attributes */
static ZCONST char Far DirlistUidGidFailed[] =
  "warning:  cannot set UID %d and/or GID %d for %s\n";
static ZCONST char Far DirlistUtimeFailed[] =
  "warning:  cannot set modification, access times for %s\n";
#  ifndef NO_CHMOD
  static ZCONST char Far DirlistChmodFailed[] =
    "warning:  cannot set permissions for %s\n";
#  endif


int set_direc_attribs(__G__ d)
    __GDEF
    dirtime *d;
{
    int errval = PK_OK;

    if (utime(d->fn, &d->u.t2)) {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(DirlistUtimeFailed), d->fn));
        if (!errval)
            errval = PK_WARN;
    }
    if (chmod(d->fn, 0xffff & d->perms)) {
        Info(slide, 0x201, ((char *)slide,
          LoadFarString(DirlistChmodFailed), d->fn));
        /* perror("chmod (file attributes) error"); */
        if (!errval)
            errval = PK_WARN;
    }
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
    ztimbuf tp;

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
    char buf1[40];
    extern char Far  CompiledWith[];

    sprintf(slide, CompiledWith,
        "THEOS C ","5.28", "THEOS ", "4.x", " on ", __DATE__);
    (*G.message)((zvoid *)&G, slide, (ulg)strlen((char *)slide), 0);

} /* end function version() */

#endif /* !SFX */
