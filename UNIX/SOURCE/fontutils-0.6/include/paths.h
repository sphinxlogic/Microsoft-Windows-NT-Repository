/* Generated from paths.h.in (Fri May  7 12:58:25 GMT 1993).  */
/* Paths.  */

/* If the environment variable `FONTUTIL_LIB' isn't set, use this
   path instead to search for auxiliary files.  */ 
#ifndef DEFAULT_LIB_PATH
#define DEFAULT_LIB_PATH ".:/source/cdrom/source/fontutils-0.6/data:/opt/lib/fontutil"
#endif

/* The meanings of these paths are described in `filename.h'.  They are
   exactly the same as those in the TeX distribution.  */

/* The directories listed in these paths are searched for the various
   font files.  The current directory is always searched first.  */
#ifndef DEFAULT_TFM_PATH
#define DEFAULT_TFM_PATH "/opt/lib/tex/fonts//:."
#endif

#ifndef DEFAULT_PK_PATH
#define DEFAULT_PK_PATH "/opt/lib/tex/fonts//:."
#endif

#ifndef DEFAULT_GF_PATH
#define DEFAULT_GF_PATH "/opt/lib/tex/fonts//:."
#endif
