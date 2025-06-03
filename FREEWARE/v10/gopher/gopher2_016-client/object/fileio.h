/********************************************************************
 Changes from Alan Coopersmith's patches:
     - include <fcntl.h> etc for O_RDONLY defn's. etc.
 ********************************************************************/
/********************************************************************
 * lindner
 * 3.8
 * 1994/04/25 03:40:07
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/fileio.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 92, 93, 94 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: fileio.h
 * Abstration of socket/file input output routines.
 *********************************************************************
 * Revision History:
 * fileio.h,v
 * Revision 3.8  1994/04/25  03:40:07  lindner
 * VMS socket stuff still wasn't working..
 *
 * Revision 3.7  1994/04/22  06:42:02  lindner
 * better compatibility checking
 *
 * Revision 3.6  1994/04/13  04:28:23  lindner
 * add vms compatible.h
 *
 * Revision 3.5  1994/04/01  04:43:55  lindner
 * fixes for VMS includes
 *
 * Revision 3.4  1994/03/17  04:36:32  lindner
 * Fix for UCX
 *
 * Revision 3.3  1994/03/11  00:09:14  lindner
 * Fix for UCX
 *
 * Revision 3.2  1994/03/08  03:21:18  lindner
 * Mods for opening processes without sh
 *
 * Revision 3.1  1994/02/20  16:20:50  lindner
 * New object based versions of buffered io routines
 *
 *
 *********************************************************************/

#ifndef FILEIO_H
#define FILEIO_H

#include "STRstring.h"
#include "boolean.h"
#include "Stdlib.h"

#ifdef VMS
#  include <processes.h>
#  include <unixio.h>
#  include <file.h>
#else              
#  ifdef  M_XENIX         /* SCO Xenix/UNIX */
#    include <sys/stream.h>            
#    include <sys/fcntl.h>
#    include <sys/ptem.h>
#  else
#    include <fcntl.h>
#  endif /* M_XENIX */
#endif /* VMS */
#include "compatible.h"

struct fileio_struct 
{
     int     bufindex;
     int     bufdatasize;
     char    *buf;

     boolean issocket;
     
     String  *filename;
     int     fd;
     pid_t   pid;
};

typedef struct fileio_struct FileIO;


#define FIOBUFSIZE   4096
#define FIOMAXOFILES 256

#define FIOisSocket(a)      ((a)->issocket)
#define FIOgetPid(a)        ((a)->pid)
#define FIOgetfd(a)         ((a)->fd)
#define FIOgetFilename(a)   (STRget((a)->filename)
#define FIOgetBufIndex(a)   ((a)->bufindex)
#define FIOgetBufDsize(a)   ((a)->bufdatasize)


#define FIOsetfd(a,b)       ((a)->fd=(b))
#define FIOsetPid(a,b)      ((a)->pid=(b))
#define FIOsetSocket(a,b)   ((a)->issocket=(b))
#define FIOsetFilename(a,b) (STRset((a)->filename,(b)))
#define FIOsetBufIndex(a,b) ((a)->bufindex = (b))
#define FIOsetBufDsize(a,b) ((a)->bufdatasize = (b))


/*
 * More elegant way of doing socket writes than ifdefs everywhere
 */

#if defined(VMS)
    int DCLsystem(/* char */);
#   define system(a) DCLsystem(a)

    /**  Note that Multinet already has socket_*  **/
#   if defined(WOLLONGONG) || defined(CMUIP) || defined(NETLIB)
#       define socket_write netwrite
#       define socket_read  netread
#       define socket_close netclose
#   endif
#   if defined(UCX)
#       define socket_write write
#       define socket_read  read
#       define socket_close close
#   endif
    /**  Note that Wollongong is like Multinet for EPIPE handling  **/
#   if defined(UCX) || defined(CMUIP) || defined(NETLIB)
#       define FIO_NOMULTIEOF
#   endif

#else
    /* Unix systems */
#   define socket_write write
#   define socket_read  read
#   define socket_close close
#endif


/** Forward Declarations **/

FileIO *FIOopenUFS();
FileIO *FIOopenfd();
FileIO *FIOopenProcess();
FileIO *FIOopenCmdline();
int     FIOclose();
int     FIOwriten();
int     FIOwritestring();
int     FIOreadbuf();
int     FIOreadn();
int     FIOreadline();
int     FIOwaitpid();
char  **FIOgetargv();
#endif
