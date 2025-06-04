/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  acorn.c

  RISCOS-specific routines for use with Info-ZIP's UnZip 5.2 and later.

  Contains:  do_wild()           <-- generic enough to put in fileio.c?
             mapattr()
             mapname()
             checkdir()
             mkdir()
             isRISCOSexfield()
             setRISCOSexfield()
             printRISCOSexfield()
             close_outfile()
             stamp_file()
             version()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "^.unzip.h"
#include "riscos.h"

#define FTYPE_FFF (1<<17)      /* set filetype to &FFF when extracting */

static int created_dir;        /* used in mapname(), checkdir() */
static int renamed_fullpath;   /* ditto */
static int has_mimemap = -1;   /* used in mimemap() */

extern int mkdir(const char *path, int mode);
static int has_NFS_ext(const char *name);
static int uxtime2acornftime(unsigned *pexadr, unsigned *pldadr, time_t ut);
static int mimemap(const char *name);


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

        /* break the wildspec into a directory part and a wildcard filename */
        if ((wildname = (ZCONST char *)strrchr(wildspec, '.')) ==
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
                if (file->d_name[0] == '/' && wildname[0] != '/')
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
    while ((file = readdir(wild_dir)) != (struct dirent *)NULL)
        if (match(file->d_name, wildname, 0)) {   /* 0 == don't ignore case */
            if (have_dirname) {
                /* strcpy(matchname, dirname); */
                strcpy(matchname+dirnamelen, file->d_name);
            } else
                strcpy(matchname, file->d_name);
            return matchname;
        }

    closedir(wild_dir); /* have read at least one dir entry; nothing left */
    wild_dir = (DIR *)NULL;
    notfirstcall = FALSE;   /* reset for new wildspec */
    if (have_dirname)
        free(dirname);
    return (char *)NULL;

} /* end function do_wild() */

#endif /* !SFX */



/**************************/
/* Function has_NFS_ext() */
/**************************/

static int has_NFS_ext(const char* name)
{
  int i = strlen(name) - 4;

  return (i >= 0 && name[i] == ',' && (i > 0 || name[i-1]=='/') &&
          isxdigit(name[i+1]) && isxdigit(name[i+2]) && isxdigit(name[i+3]));
} /* end function has_NFS_ext() */



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
        case ACORN_:
        case UNIX_:
        case VMS_:
        case ATARI_:
        case BEOS_:
        case QDOS_:
        case TANDEM_:
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            if (G.pInfo->file_attr != 0 || !G.extra_field) {
                break;
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
                    break;
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
                break;
            G.pInfo->file_attr = (unsigned)(0444 | tmp<<6 | tmp<<3 | tmp);
            break;
    } /* end switch (host-OS-created-by) */

    G.pInfo->file_attr&=0xFFFF;

    G.pInfo->file_attr|=(0xFFDu<<20);

    if (has_NFS_ext(G.filename)) {
      int ftype=strtol(G.filename+strlen(G.filename)-3,NULL,16)&0xFFF;

      G.pInfo->file_attr = (G.pInfo->file_attr & 0x000FFFFF) | (ftype<<20);
    } else {
      int type = mimemap(G.filename);
      if (type == -1)
        type = (G.crec.internal_file_attributes & 1) ? 0xFFF : 0xFFD;
      G.pInfo->file_attr = (G.pInfo->file_attr & 0x000FFFFF) | (type<<20);
    }

    return 0;

} /* end function mapattr() */



/************************/
/*  Function mimemap()  */
/************************/

static int mimemap(const char *name)
{
  const char *ext = name;
  int type;

  if (has_mimemap < 0)
    has_mimemap =
         !(SWI_OS_CLI("%RMEnsure MimeMap 0.05 RMLoad System:Modules.MimeMap")
           || SWI_OS_CLI("%RMEnsure MimeMap 0.05"));

  if (!has_mimemap)
    return -1; /* no MimeMap module; fall back on text flag test */

  do {
    while (*ext && *ext!='.')
      ext++;
    if (!*ext)
      return -1; /* no suitable extension; fallback */
    type = SWI_MimeMap_Translate(ext++);
  } while (type == -1);

  return type;
}



/************************/
/*  Function mapname()  */
/************************/
                             /* return 0 if no error, 1 if caution (filename */
int mapname(__G__ renamed)   /*  truncated), 2 if warning (skip file because */
    __GDEF                   /*  dir doesn't exist), 3 if error (skip file), */
    int renamed;             /*  or 10 if out of memory (skip file) */
{                            /*  [also IZ_VOL_LABEL, IZ_CREATED_DIR] */
    char pathcomp[FILNAMSIZ];    /* path-component buffer */
    char *pp, *cp=(char *)NULL;  /* character pointers */
    char *lastsemi=(char *)NULL; /* pointer to last semi-colon in pathcomp */
    int quote = FALSE;           /* flags */
    int error = 0;
    register unsigned workch;    /* hold the character being tested */
    char *checkswap=NULL;        /* pointer the the extension to check or NULL */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    if (G.pInfo->vollabel)
        return IZ_VOL_LABEL;    /* can't set disk volume labels in RISCOS */

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
                checkswap=NULL;  /* reset checking when starting a new leafname */
                break;

            case ';':             /* VMS version (or DEC-20 attrib?) */
                lastsemi = pp;
                *pp++ = ';';      /* keep for now; remove VMS ";##" */
                break;            /*  later, if requested */

            case '\026':          /* control-V quote for special chars */
                quote = TRUE;     /* set flag for next character */
                break;

            case ' ':             /* change spaces to hard-spaces */
                *pp++ = 160;      /* (ISO 8859-1 Latin-1 codepage) */
                break;

            case ':':             /* change ':' to '�' (vertical broken bar) */
                *pp++ = '\xA6';   /* (position 166 in ISO 8859-1 code table) */
                break;

            case '&':             /* change '&' to 'E' */
                *pp++ = 'E';
                break;

            case '@':             /* change '@' to 'A' */
                *pp++ = 'A';
                break;

            case '.':
                *pp++ = '/';
                checkswap=pp;
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
            return IZ_CREATED_DIR;   /* set dir time (note trailing '/') */
        }
        return 2;   /* dir existed already; don't look for data to extract */
    }

    if (*pathcomp == '\0') {
        Info(slide, 1, ((char *)slide, "mapname:  conversion of %s failed\n",
          G.filename));
        return 3;
    }

    if (checkswap!=NULL) {
        if (checkext(checkswap)) {
            if ((error = checkdir(__G__ checkswap, APPEND_DIR)) > 1)
                return error;
            *(checkswap-1)=0;    /* remove extension from pathcomp */
        }
    }

    if (!uO.acorn_nfs_ext && has_NFS_ext(pathcomp)) {
      /* remove the filetype extension unless requested otherwise */
      /* the filetype should be already set by mapattr() */
      pathcomp[strlen(pathcomp)-4]=0;
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
                fflush(stderr);
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
        *end++ = '.';    /************* was '/' *************/
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
            if (tmproot[rootlen-1] == '.') {    /****** was '/' ********/
                tmproot[--rootlen] = '\0';
            }
            if (rootlen > 0 && (stat(tmproot, &G.statbuf) ||
                                !S_ISDIR(G.statbuf.st_mode)))
            {   /* path does not exist */
                if (!G.create_dirs /* || isshexp(tmproot) */ ) {
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
            tmproot[rootlen++] = '.';   /*********** was '/' *************/
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





/********************/
/* Function mkdir() */
/********************/

int mkdir(path, mode)
    const char *path;
    int mode;   /* ignored */
/*
 * returns:   0 - successful
 *           -1 - failed (errno not set, however)
 */
{
    return (SWI_OS_File_8((char *)path) == NULL)? 0 : -1;
}




/*********************************/
/* extra_field-related functions */
/*********************************/

int isRISCOSexfield(void *extra_field)
{
  if (extra_field!=NULL) {
    extra_block *block=(extra_block *)extra_field;
    return (block->ID==EF_SPARK && (block->size==24 || block->size==20) &&
            block->ID_2==SPARKID_2);
  } else
    return FALSE;
}

void setRISCOSexfield(char *path, void *extra_field)
{
  if (extra_field!=NULL) {
    extra_block *block=(extra_block *)extra_field;
    SWI_OS_File_1(path,block->loadaddr,block->execaddr,block->attr);
  }
}

void printRISCOSexfield(int isdir, void *extra_field)
{
 extra_block *block=(extra_block *)extra_field;
 printf("\n  This file has RISC OS file informations in the local extra field.\n");

 if (isdir) {
/*   I prefer not to print this string... should change later... */
/*   printf("  The file is a directory.\n");*/
 } else if ((block->loadaddr & 0xFFF00000) != 0xFFF00000) {
   printf("  Load address: %.8X\n",block->loadaddr);
   printf("  Exec address: %.8X\n",block->execaddr);
 } else {
   /************* should change this to use OS_FSControl 18 to get filetype string ************/
   char tmpstr[16];
   char ftypestr[32];
   int flen;
   sprintf(tmpstr,"File$Type_%03x",(block->loadaddr & 0x000FFF00) >> 8);
   if (SWI_OS_ReadVarVal(tmpstr,ftypestr,32,&flen)==NULL) {
     ftypestr[flen]=0;
     printf("  Filetype: %s (&%.3X)\n",ftypestr,(block->loadaddr & 0x000FFF00) >> 8);
   } else {
     printf("  Filetype: &%.3X\n",(block->loadaddr & 0x000FFF00) >> 8);
   }
 }
 printf("  Access: ");
 if (block->attr & (1<<3))
   printf("L");
 if (block->attr & (1<<0))
   printf("W");
 if (block->attr & (1<<1))
   printf("R");
 printf("/");
 if (block->attr & (1<<4))
   printf("w");
 if (block->attr & (1<<5))
   printf("r");
 printf("\n\n");
}


/**********************************************/
/* internal help function for time conversion */
/**********************************************/
static int uxtime2acornftime(unsigned *pexadr, unsigned *pldadr, time_t ut)
{
   unsigned timlo;      /* 3 lower bytes of acorn file-time plus carry byte */
   unsigned timhi;      /* 2 high bytes of acorn file-time */

   timlo = ((unsigned)ut & 0x00ffffffU) * 100 + 0x00996a00U;
   timhi = ((unsigned)ut >> 24);
   timhi = timhi * 100 + 0x0000336eU + (timlo >> 24);
   if (timhi & 0xffff0000U)
       return 1;        /* calculation overflow, do not change time */

   /* insert the five time bytes into loadaddr and execaddr variables */
   *pexadr = (timlo & 0x00ffffffU) | ((timhi & 0x000000ffU) << 24);
   *pldadr = (*pldadr & 0xffffff00U) | ((timhi >> 8) & 0x000000ffU);

   return 0;            /* subject to future extension to signal overflow */
}


/****************************/
/* Function close_outfile() */
/****************************/

void close_outfile(__G)
    __GDEF
{
  fclose(G.outfile);

  if (isRISCOSexfield(G.extra_field)) {
    setRISCOSexfield(G.filename, G.extra_field);
  } else {
    unsigned int loadaddr, execaddr;
    int attr;
    int mode=G.pInfo->file_attr&0xffff;   /* chmod equivalent mode */

    time_t m_time;
#ifdef USE_EF_UT_TIME
    iztimes z_utime;
#endif

#ifdef USE_EF_UT_TIME
    if (G.extra_field &&
#ifdef IZ_CHECK_TZ
        G.tz_is_valid &&
#endif
        (ef_scan_for_izux(G.extra_field, G.lrec.extra_field_length, 0,
                          G.lrec.last_mod_dos_datetime, &z_utime, NULL)
         & EB_UT_FL_MTIME))
    {
        TTrace((stderr, "close_outfile:  Unix e.f. modif. time = %ld\n",
          z_utime.mtime));
        m_time = z_utime.mtime;
    } else
#endif /* USE_EF_UT_TIME */
        m_time = dos_to_unix_time(G.lrec.last_mod_dos_datetime);

    /* set the file's modification time */
    SWI_OS_File_5(G.filename, NULL, &loadaddr, NULL, NULL, &attr);

    uxtime2acornftime(&execaddr, &loadaddr, m_time);

    loadaddr = (loadaddr & 0xfff000ffU) |
               ((G.pInfo->file_attr&0xfff00000) >> 12);

    attr=(attr&0xffffff00) | ((mode&0400) >> 8) | ((mode&0200) >> 6) |
                             ((mode&0004) << 2) | ((mode&0002) << 4);

    SWI_OS_File_1(G.filename, loadaddr, execaddr, attr);
  }

} /* end function close_outfile() */




#ifdef TIMESTAMP

/***************************/
/*  Function stamp_file()  */
/***************************/

int stamp_file(fname, modtime)
    ZCONST char *fname;
    time_t modtime;
{
    unsigned int loadaddr, execaddr;
    int attr;

    /* set the file's modification time */
    if (SWI_OS_File_5((char *)fname, NULL, &loadaddr, NULL, NULL, &attr)
        != NULL)
        return -1;

    if (uxtime2acornftime(&execaddr, &loadaddr, modtime) != 0)
        return -1;

    return (SWI_OS_File_1((char *)fname, loadaddr, execaddr, attr) == NULL) ?
           0 : -1;

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
#ifdef __GNUC__
      "gcc ", __VERSION__,
#else
#  ifdef __CC_NORCROFT
      "Norcroft ", "cc",
#  else
      "cc", "",
#  endif
#endif

      "RISC OS",

      " (Acorn Computers Ltd)",

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
      );

    (*G.message)((zvoid *)&G, slide, (ulg)strlen((char *)slide), 0);

} /* end function version() */

#endif /* !SFX */
