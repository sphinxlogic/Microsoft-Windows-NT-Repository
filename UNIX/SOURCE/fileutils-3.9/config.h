/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if using alloca.c.  */
#define C_ALLOCA 1

/* Define to empty if the keyword does not work.  */
#define const 

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have dirent.h.  */
#define DIRENT

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
#define GETGROUPS_T int

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca.h and it should be used (not Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if your struct stat has st_blocks.  */
/* #undef HAVE_ST_BLOCKS */

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* #undef HAVE_UTIME_NULL */

/* Define if you have the vprintf function.  */
/* #undef HAVE_VPRINTF */

/* Define if int is 16 bits instead of 32.  */
/* #undef INT_16_BITS */

/* Define if major, minor, and makedev are declared in mkdev.h.  */
#define MAJOR_IN_MKDEV 1

/* Define if major, minor, and makedev are declared in sysmacros.h.  */
/* #undef MAJOR_IN_SYSMACROS */

/* Define if on MINIX.  */
/* #undef _MINIX */

/* Define if you don't have dirent.h, but have ndir.h.  */
/* #undef NDIR */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE int

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
#define STACK_DIRECTION 0

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
#define STAT_MACROS_BROKEN 1

/* Define if you have the ANSI C header files.  */
/* #undef STDC_HEADERS */

/* Define if you don't have dirent.h, but have sys/dir.h.  */
/* #undef SYSDIR */

/* Define if you don't have dirent.h, but have sys/ndir.h.  */
/* #undef SYSNDIR */

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
/* #undef TIME_WITH_SYS_TIME */

/* Define if your <sys/time.h> declares struct tm.  */
#define TM_IN_SYS_TIME 1

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define if the closedir function returns void instead of int.  */
#define VOID_CLOSEDIR 1

/* Define if you have the Andrew File System.  */
/* #undef AFS */

/* Define if your system has the ftruncate function or if it can
   emulate it.  */
/* #undef HAVE_FTRUNCATE */

/* Define if there is no specific function for reading the list of
   mounted filesystems.  fread will be used to read /etc/mnttab.  [SVR2]  */
/* #undef MOUNTED_FREAD */

/* Define if (like SVR2) there is no specific function for reading the
   list of mounted filesystems, and your system has these header files:
   <sys/fstyp.h> and <sys/statfs.h>.  [SVR3]  */
/* #undef MOUNTED_FREAD_FSTYP */

/* Define if there is a function named getfsstat for reading the list
   of mounted filesystems.  [DEC Alpha running OSF/1]  */
/* #undef MOUNTED_GETFSSTAT */

/* Define if there is a function named getmnt for reading the list of
   mounted filesystems.  [Ultrix]  */
/* #undef MOUNTED_GETMNT */

/* Define if there is a function named getmntent for reading the list
   of mounted filesystems, and that function takes a single argument.
   [4.3BSD, SunOS, HP-UX, Dynix, Irix]  */
/* #undef MOUNTED_GETMNTENT1 */

/* Define if there is a function named getmntent for reading the list of
   mounted filesystems, and that function takes two arguments.  [SVR4]  */
#define MOUNTED_GETMNTENT2 1

/* Define if there is a function named getmntinfo for reading the list
   of mounted filesystems.  [4.4BSD]  */
/* #undef MOUNTED_GETMNTINFO */

/* Define if there is a function named mntctl that can be used to read
   the list of mounted filesystems, and there is a system header file
   that declares `struct vmount.'  [AIX]  */
/* #undef MOUNTED_VMOUNT */

/*  Define if  statfs takes 3 args.  [DEC Alpha running OSF/1]  */
/* #undef STAT_STATFS3_OSF1 */

/* Define if there is no specific function for reading filesystems usage
   information;  Use `read'.  [SVR2]  */
/* #undef STAT_READ */

/* Define if statfs takes 2 args and struct statfs has a field named f_bsize.
   [4.3BSD, SunOS 4, HP-UX, AIX PS/2]  */
/* #undef STAT_STATFS2_BSIZE */

/* Define if statfs takes 2 args and struct statfs has a field named f_fsize.
   [4.4BSD]  */
/* #undef STAT_STATFS2_FSIZE */

/* Define if statfs takes 2 args and the second argument has
   type struct fs_data.  [Ultrix]  */
/* #undef STAT_STATFS2_FS_DATA */

/* Define if statfs takes 4 args.  [SVR3, Dynix, Irix, Dolphin]  */
/* #undef STAT_STATFS4 */

/* Define if there is a function named statvfs.  [SVR4]  */
#define STAT_STATVFS 1

/* Define if you have bcopy.  */
/* #undef HAVE_BCOPY */

/* Define if you have ftime.  */
/* #undef HAVE_FTIME */

/* Define if you have getcwd.  */
/* #undef HAVE_GETCWD */

/* Define if you have gettimeofday.  */
/* #undef HAVE_GETTIMEOFDAY */

/* Define if you have memcpy.  */
/* #undef HAVE_MEMCPY */

/* Define if you have mkfifo.  */
/* #undef HAVE_MKFIFO */

/* Define if you have strerror.  */
/* #undef HAVE_STRERROR */

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the dgc library (-ldgc).  */
/* #undef HAVE_DGC */

/* Define if you have the seq library (-lseq).  */
/* #undef HAVE_SEQ */

/* Define if you have the sun library (-lsun).  */
/* #undef HAVE_SUN */

/* Define if you have the ypsec library (-lypsec).  */
/* #undef HAVE_YPSEC */

/* Define if we have the rename(2) system call */
#define HAVE_RENAME
