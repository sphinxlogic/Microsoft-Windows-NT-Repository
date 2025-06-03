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

typedef struct 
{
   int fd;			       /* handle */
   FILE *fp;			       /* kind of obvious */
   unsigned int flags;		       /* modes, etc... */
#ifdef HAS_SUBPROCESSES
   int pid;			       /* pid of child (if any) */
#endif
} SL_File_Table_Type;

#define SL_MAX_FILES 30
extern SL_File_Table_Type SL_File_Table[SL_MAX_FILES];

#define SL_READ		0x01
#define SL_WRITE	0x02
#define SL_BINARY	0x04
#define SL_SOCKET	0x08
#define SL_PROCESS	0x10

#ifdef HAS_SUBPROCESSES
extern int SLcreate_child_process (char *);
#endif

extern SL_File_Table_Type *get_file_table_entry(void);




