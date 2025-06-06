/* config.h.  Generated automatically by configure.  */
/* Template configuration file for screen.  -*- C -*-
 * Copyright (c) 1991
 *      Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
 *      Michael Schroeder (mlschroe@immd4.informatik.uni-erlangen.de)
 * Copyright (c) 1987 Oliver Laumann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Noteworthy contributors to screen's design and implementation:
 *	Wayne Davison (davison@borland.com)
 *	Patrick Wolfe (pat@kai.com, kailand!pat)
 *	Bart Schaefer (schaefer@cse.ogi.edu)
 *	Nathan Glasser (nathan@brokaw.lcs.mit.edu)
 *	Larry W. Virden (lwv27%cas.BITNET@CUNYVM.CUNY.Edu)
 *	Howard Chu (hyc@hanauma.jpl.nasa.gov)
 *	Tim MacKenzie (tym@dibbler.cs.monash.edu.au)
 *	Markku Jarvinen (mta@{cc,cs,ee}.tut.fi)
 *	Marc Boucher (marc@CAM.ORG)
 *
 ****************************************************************
 * $Id: config.all,v 1.2 92/02/03 02:30:36 jnweiger Exp $ FAU
 */

/*
 * This file has two parts:
 * 1.  Site configuration.  Preferences that depend on your tastes
 *     or on details of your individual installation.
 *     You should check these values by hand.
 *
 * 2.  O.S. configuration.  Variables that depend on what kind of
 *     operating system you are running.
 *     The configure script tries to set these correctly automatically
 *     when it creates config.h from config.h.in.
 *     If it messes up, you can set them by hand.
 */


/*
 *		Site Configuration Section
 */


/*
 * First, decide whether to install screen set-uid to root.
 * This isn't necessary to use screen, but it allows the pseudo-ttys
 * to be set to their proper owner (for security purposes), /etc/utmp to be
 * updated, and /dev/kmem to be accessed to read the load average values.
 *
 * Failing to install it suid root (e.g., if you fear a trojan horse) doesn't
 * have any major disadvantages, except that w(1) and some other utilities
 * will display only "screen" as the current process, and the pseudo ttys
 * (ptys) used for the virtual terminals won't have their owners set.  Screen
 * can provide you some pty security by opening the ptys exclusively, but
 * this has the unfortunate side-effect of keeping your own subprocesses from
 * being able to open /dev/tty.
 *
 * An alternative to installing screen set-uid root is to install it set-gid
 * utmp (with the file /etc/utmp installed to be group-utmp writable) or
 * set-gid kmem (with /dev/kmem set to be group-kmem readable) or some other
 * custom group to give you both.  The final alternative is to omit /etc/utmp
 * updating and the /dev/kmem reading (see the following defines) and simply
 * run screen as a regular program -- its major functions will be unaffected.
 *
 * If screen is going to be installed set-uid root, you MUST define SUIDROOT.
 */
#define SUIDROOT

/*
 * If screen is installed with permissions to update /etc/utmp (such as if
 * it is installed set-uid root), define UTMPOK.  Set LOGINDEFAULT to one (1)
 * if you want entries added to /etc/utmp by default, else set it to zero (0).
 */
#define UTMPOK
#define LOGINDEFAULT	1

/*
 * If UTMPOK is defined and your system (incorrectly) counts logins by
 * counting non-null entries in /etc/utmp (instead of counting non-null
 * entries with no hostname that are not on a pseudo tty), define USRLIMIT
 * to have screen put an upper-limit on the number of entries to write
 * into /etc/utmp.  This helps to keep you from exceeding a limited-user
 * license.
 */
#undef USRLIMIT

/*
 * If screen is NOT installed set-uid root, screen can provide tty security
 * by exclusively locking the ptys.  While this keeps other users from
 * opening your ptys, it also keeps your own subprocesses from being able
 * to open /dev/tty.  Define LOCKPTY to add this exclusive locking.
 */
#undef LOCKPTY

/*
 * If your version of NFS supports named sockets and you install screen
 * suid root, you may need to define NFS_HACK for screen to be able to
 * open the sockets.
 */
#undef NFS_HACK

/*
 * By default screen will create a directory named ".screen" in the user's
 * HOME directory to contain the named sockets.  If this causes you problems
 * (e.g., some user's HOME directories are NFS-mounted and don't support
 * named sockets) you can have screen create the socket directories in a
 * common subdirectory, such as /tmp or /usr/tmp.  It makes things a little
 * more secure if you choose a directory where the "sticky" bit is on, but
 * this isn't required.  Screen will name the subdirectories "S-$USER"
 * (e.g /tmp/S-davison).
 * Do not define TMPTEST unless it's for debugging purpose.
 * If you want to have your socket directory in "/tmp/screens" then
 * define LOCALSOCKDIR and change the definition of SOCKDIR below.
 */
#define LOCALSOCKDIR

#ifdef LOCALSOCKDIR
# ifndef TMPTEST
#  define SOCKDIR "/tmp/screens"
# else
#  define SOCKDIR "/tmp/testscreens"
# endif
#endif

/*
 * If you'd rather see the status line on the first line of your
 * terminal rather than the last, define TOPSTAT.
 * This should really be screenrc-settable, not a compile option.
 */
#undef TOPSTAT

/*
 * define LOCK if you want to use a lock program for a screenlock.
 * define PASSWORD for secure reattach of your screen.
 * define COPY_PASTE to use the famous hacker's treasure zoo.
 * define POW_DETACH to have a detach_and_logout key.
 * define REMOTE_DETACH (-d option) to move screen between terminals.
 */
#define LOCK
#define PASSWORD
#define COPY_PASTE
#define REMOTE_DETACH
#define POW_DETACH

/*
 * As error messages are mostly meaningless to the user, we
 * try to throw out phrases that are somewhat more familiar
 * to ...well, at least familiar to us NetHack players.
 */
#define NETHACK


/*
 *		O.S. Configuration Section
 *		configure defines these symbols if appropriate
 */


/*
 * Define POSIX if your system supports IEEE Std 1003.1-1988 (POSIX).
 */
#define POSIX 1

/*
 * Define BSDJOBS if you have BSD-style job control (both process
 * groups and a tty that deals correctly with them).
 */
#undef BSDJOBS

/*
 * Define TERMIO if you have struct termio instead of struct sgttyb.
 * This is usually the case for SVID systems, whereas BSD uses sgttyb.
 * POSIX systems should define this anyway, even though they use
 * struct termios.
 */
#define TERMIO 1

/*
 * Define TERMINFO if your machine emulates the termcap routines
 * with the terminfo database.
 * Thus the .screenrc file is parsed for
 * the command 'terminfo' and not 'termcap'.
 */
#undef TERMINFO

/*
 * Define SYSV if your machine is SYSV complient (Sys V, HPUX, A/UX)
 */
#define SYSV 1

/*
 * Define SIGVOID if your signal handlers return void.  On older
 * systems, they return int, but on newer ones, they return void.
 */
#define SIGVOID 1

/*
 * Define DIRENT if your system has <dirent.h> instead of <sys/dir.h>
 */
#define DIRENT 1

/*
 * If your system has getutent(), pututline(), etc. to write to the
 * utmp file, define GETUTENT.
 */
#define GETUTENT 1

/*
 * Define UTHOST if the utmp file has a host field.
 */
#undef UTHOST

/*
 * If ttyslot() breaks getlogin() by returning indexes to utmp entries of
 * type DEAD_PROCESS, then our getlogin() replacement should be selected by
 * defining BUGGYGETLOGIN.  This is only known to happen on SVR4.
 * This is only used if UTMPOK is defined.
 */
#define BUGGYGETLOGIN 1

/*
 * If your system does not have the calls setreuid() and setregid(), define
 * NOREUID to force screen to use a forked process to safely create output
 * files without retaining any special privileges.  (Output logging will be
 * disabled, however.)
 */
#define NOREUID 1

/*
 * If your system has the new format /etc/ttys (like 4.3 BSD) and the
 * getttyent(3) library functions, define GETTTYENT.
 */
#undef GETTTYENT

/*
 * Define USEBCOPY if the bcopy() from your system's C library supports the
 * overlapping of source and destination blocks.  When undefined, screen
 * uses its own (probably slower) version of bcopy().
 */
#undef USEBCOPY

/*
 * If your system has vsprintf() and requires the use of the macros in
 * <varargs.h> to use functions with variable arguments, define USEVARARGS.
 */
#undef USEVARARGS

/*
 * Define this if your system supports named pipes
 * instead of sockets.
 */
#undef NAMEDPIPE

/*
 * if your system does not come with a putenv()/getenv() functions, 
 * you may bring in our own code by defining NEEDSETENV *and* adding 
 * putenv.o to OFILES in your Makefile.
 */
#undef NEEDSETENV

/*
 * Define if you have shadow passwords and <shadow.h>.
 */
#define SHADOWPW 1

/*
 * Define if you are not SYSV, POSIX, apollo, or sysV68,
 * yet your headers define pid_t.
 */
#define PID_T_DEFINED 1

/*
 * Define if your headers define sig_t.
 */
#undef SIG_T_DEFINED

/*
 * Define if you have nlist.h.  For getting the load average.
 */
#undef NLIST_STRUCT

/*
 * Define each of these if the appropriate function is declared in
 * your system header files.
 */
#undef CRYPT_DECLARED
#undef GETHOSTNAME_DECLARED
#undef KILLSTUFF_DECLARED /* kill and maybe killpg */
#undef MEMFUNCS_DECLARED /* bzero, bcopy or memset */
#define MKNOD_DECLARED 1 /* in sys/stat.h */
#undef NLIST_DECLARED
#undef PUTENV_DECLARED
#undef REUID_DECLARED /* setres?[ug]id */
#define SETPGID_DECLARED 1 /* in unistd.h */
#undef VPRNT_DECLARED /* vsprintf */
#undef WAITSTUFF_DECLARED /* wait3 */

/*
 * If you are on a SYS V machine that restricts filename length to 14 
 * characters, you may need to enforce that by defining this to 14.
 */
#ifndef NAME_MAX
#undef NAME_MAX
#endif

/*
 * The characters to try when finding pty names to open,
 * on some types of systems.
 * Try each char in PTY_FIRST_CHARS followed by each char in
 * PTY_SECOND_CHARS in succession until finding a free pty.
 */
#ifdef MIPS
#define PTY_FIRST_CHARS "zyxwvutsrqp"
#endif

#ifdef hpux
#define PTY_FIRST_CHARS "pqrstuvw"
#endif

#ifdef sun
#define PTY_FIRST_CHARS "qprstuvwxyzPQRST"
#endif

#ifndef PTY_FIRST_CHARS
#define PTY_FIRST_CHARS "qpr"
#endif

#ifndef PTY_SECOND_CHARS
#define PTY_SECOND_CHARS "0123456789abcdef"
#endif
