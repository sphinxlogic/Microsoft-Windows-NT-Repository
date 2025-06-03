/* Unix system calls */
/* 
 * Copyright (c) 1992, 1994 John E. Davis 
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#ifndef sequent
# include <unistd.h>
#endif

#include <errno.h>
#include <string.h>

#include "slang.h"

/* map value of errno to standard slang return values. */
static int map_errno(void)
{
   switch (errno)
     {
      case EACCES: return -1;	       /* insufficient privilege */
      case ENOENT: return -2;	       /* invalid path */
      default: return -50;
     }
}


#define EQS(a, b) ((*(a) == *(b)) && (!strcmp(a + 1, b + 1)))

static struct stat stat_buf;

static int parse_stat(struct stat *s, char *f)
{
   if (!strncmp(f, "st_", 3)) f += 3;
   
   if (EQS("dev", f)) return s->st_dev;
   if (EQS("ino", f)) return s->st_ino;
   if (EQS("mode", f)) return s->st_mode;
   if (EQS("nlink", f)) return s->st_nlink;
   if (EQS("uid", f)) return s->st_uid;
   if (EQS("gid", f)) return s->st_gid;
   if (EQS("rdev", f)) return s->st_rdev;
   if (EQS("size", f)) return s->st_size;
   if (EQS("atime", f)) return s->st_atime;
   if (EQS("mtime", f)) return s->st_mtime;
   if (EQS("ctime", f)) return s->st_ctime;
   SLang_doerror("Unknown stat field.");
   return (0);
}


static int unix_stat_file(char *file)
{
   if (!stat (file, &stat_buf)) return 0;
   return map_errno();
}

#ifndef __GO32__
static int unix_lstat_file(char *file)
{
   if (!lstat (file, &stat_buf)) return 0;
   return map_errno();
}
#endif

static int unix_stat_struct(char *field)
{
   return parse_stat(&stat_buf, field);
}

static int unix_chmod(char *file, int *mode)
{
   if (!chmod(file, (mode_t) *mode)) return 0;
   return map_errno();
}

static int unix_chown(char *file, int *owner, int *group)
{
   if (!chown(file, (uid_t) *owner, (gid_t) *group)) return 0;
   return map_errno();
}

/* This is mainly designed to check pids, but who knows.... */
static int unix_kill(void)
{
   int pid, sig;
   
   if (SLang_pop_integer(&sig) || SLang_pop_integer(&pid)) return (-1);
   return kill ((pid_t) pid, sig);
}


static char *unix_ctime(int *tt)
{
   char *t;
   
   t = ctime ((time_t *) tt);
   t[24] = 0;  /* knock off \n */
   return (t);
}


static SLang_Name_Type slunix_table[] =
{
   MAKE_INTRINSIC(".unix_kill", unix_kill, INT_TYPE, 0),
   /*Prototype: Integer unix_kill(Integer pid, Integer sig);
    */
   MAKE_INTRINSIC(".unix_ctime", unix_ctime, STRING_TYPE, 1),
   /*Prototype: String unix_ctime(Integer secs);
     Returns a string representation of the time as given by 'secs' seconds
     since 1970. */
#ifndef __GO32__
   MAKE_INTRINSIC(".lstat_file", unix_lstat_file, INT_TYPE, 1),
   /*Prototype: Integer lstat_file(String file);
     This function is like 'stat_file' but it returns information about 
     the link itself. See 'stat_file' for usage.
     See also: stat_file */
#endif
   MAKE_INTRINSIC(".stat_file", unix_stat_file, INT_TYPE, 1),
   /*Prototype: Integer stat_file(String file);
     This function returns information about 'file' through the use of the 
     system 'stat' call.  If the stat call fails, the function returns a 
     negative integer.  If it is successful, it returns zero.  Upon failure it 
     returns a negative number.

     To retrieve information obtained by this call, use the 'stat_struct'
     function.
     See also: lstat_file, stat_struct */

   MAKE_INTRINSIC(".stat_struct", unix_stat_struct, INT_TYPE, 1),
   /*Prototype Integer stat_struct(String field);
     This functions returns information previously obtained by a call to the
     'stat_file' or 'lstat_file' functions.  The 'field' argument specifies
     what piece of information to return.   Valid values for 'field' are:
     
         "dev"
	 "ino"
	 "mode"
	 "nlink"
	 "uid"
	 "gid"
	 "rdev"
	 "size"
	 "atime"
	 "mtime"
	 "ctime"
	 
    See the man page for 'stat' for a discussion of these fields.
    The following example returns the size of the file "jed.rc":
    
        variable size;
        if (stat_file("jed.rc") < 0) error ("Unable to stat file!");
	size = stat_struct("size");
	*/

   MAKE_INTRINSIC(".chown", unix_chown, INT_TYPE, 3),
   /*Prototype Integer chown(String file, Integer uid, Integer gid);
     Change ownership of 'file' to that of user id 'uid' and group id 'gid'.
     This function returns 0 upon success and a negative number up failure.
     It returns -1 if the process does not have sufficent privileges and -2
     if the file does not exist. 
     See also: chmod, stat_file */
   MAKE_INTRINSIC(".chmod", unix_chmod, INT_TYPE, 2),
   /*Prototype Integer chmod(String file, Integer mode);
     'chmod' changes the permissions of 'file' to those specified by 'mode'.
     It returns 0 upon success, -1 if the process lacks sufficient privilege
     for the operation, or -2 if the file does not exist.
     See also: chown, stat_file
     */
   SLANG_END_TABLE
};

int init_SLunix()
{
   return SLang_add_table(slunix_table, "_Unix");
}
