/* Hey Emacs, this is -*- C -*- code!  */

/* The default search path for Lisp function "load".
   This sets load-path.  */
#define PATH_LOADSEARCH "/opt/lib/emacs/site-lisp:/opt/lib/emacs/19.22/lisp"

/* Like PATH_LOADSEARCH, but used only when Emacs is dumping.  This
   path is usually identical to PATH_LOADSEARCH except that the entry
   for the directory containing the installed lisp files has been
   replaced with ../lisp.  */
#define PATH_DUMPLOADSEARCH "/cdcopy/SOURCE/emacs-19.22/lisp"

/* The extra search path for programs to invoke.  This is appended to
   whatever the PATH environment variable says to set the Lisp
   variable exec-path and the first file name in it sets the Lisp
   variable exec-directory.  exec-directory is used for finding
   executables and other architecture-dependent files.  */
#define PATH_EXEC "/opt/lib/emacs/19.22/i386-univel-sysv4.2"

/* Where Emacs should look for its architecture-independent data
   files, like the docstring file.  The lisp variable data-directory
   is set to this value.  */
#define PATH_DATA "/opt/lib/emacs/19.22/etc"

/* The name of the directory that contains lock files with which we
   record what files are being modified in Emacs.  This directory
   should be writable by everyone.  THE STRING MUST END WITH A
   SLASH!!!  */
#define PATH_LOCK "/opt/lib/emacs/lock/"

/* Where the configuration process believes the info tree lives.  The
   lisp variable configure-info-directory gets its value from this
   macro, and is then used to set the Info-default-directory-list.  */
#define PATH_INFO "/opt/info"

