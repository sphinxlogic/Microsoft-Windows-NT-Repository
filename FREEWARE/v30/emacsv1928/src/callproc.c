/* Synchronous subprocess invocation for GNU Emacs.
   Copyright (C) 1985, 1986, 1987, 1988, 1993, 1994 Free Software Foundation, Inc.

This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include <signal.h>
#include <errno.h>

#include <config.h>
#include <stdio.h>

extern int errno;
extern char *strerror ();

#ifdef VMS
#define err_str(a) ((a != EVMSERR) ? strerror(a) : strerror(a,vaxc$errno))
#else
#define err_str(a) (strerror(a))
#endif

/* Define SIGCHLD as an alias for SIGCLD.  */

#if !defined (SIGCHLD) && defined (SIGCLD)
#define SIGCHLD SIGCLD
#endif /* SIGCLD */

#include <sys/types.h>

#include <sys/file.h>
#ifdef USG5
#define INCLUDED_FCNTL
#include <fcntl.h>
#endif

#ifdef MSDOS	/* Demacs 1.1.1 91/10/16 HIRANO Satoshi */
#include "msdos.h"
#define INCLUDED_FCNTL
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#endif /* MSDOS */

#ifndef O_RDONLY
#define O_RDONLY 0
#endif

#ifndef O_WRONLY
#define O_WRONLY 1
#endif

#include "lisp.h"
#include "commands.h"
#include "buffer.h"
#include <paths.h>
#include "process.h"
#include "syssignal.h"
#include "systty.h"

#ifdef VMS
#include <libdef.h>
#include <descrip.h>
#define	CLI$M_NOWAIT	1
#define CLI$M_WAIT	0
#define CLI$M_AUTHPRIV	128

#ifdef __GNUC__
#define	environ $$PsectAttributes_NOSHR$$environ
extern char **environ;
#else /* not __GNUC__ */
extern noshare char **environ;
#endif /* nto __GNUC__ */
#else /* not VMS */
extern char **environ;
#endif /* not VMS */

#define max(a, b) ((a) > (b) ? (a) : (b))

#ifdef MSDOS
/* When we are starting external processes we need to know whether they
   take binary input (no conversion) or text input (\n is converted to
   \r\n).  Similar for output: if newlines are written as \r\n then it's
   text process output, otherwise it's binary.  */
Lisp_Object Vbinary_process_input;
Lisp_Object Vbinary_process_output;
#endif

Lisp_Object Vexec_path, Vexec_directory, Vdata_directory, Vdoc_directory;
Lisp_Object Vconfigure_info_directory;

Lisp_Object Vshell_file_name;

Lisp_Object Vprocess_environment;

#ifdef MSDOS
Lisp_Object Qbuffer_file_type;
#endif

/* True iff we are about to fork off a synchronous process or if we
   are waiting for it.  */
int synch_process_alive;

/* Nonzero => this is a string explaining death of synchronous subprocess.  */
char *synch_process_death;

/* If synch_process_death is zero,
   this is exit code of synchronous subprocess.  */
#ifdef VMS
unsigned long synch_process_retcode;
#else
int synch_process_retcode;
#endif

extern Lisp_Object Vdoc_file_name;

/* Clean up when exiting Fcall_process.
   On MSDOS, delete the temporary file on any kind of termination.
   On Unix, kill the process and any children on termination by signal.  */

/* Nonzero if this is termination due to exit.  */
static int call_process_exited;

static Lisp_Object
call_process_kill (fdpid)
     Lisp_Object fdpid;
{
#ifdef VMS
  vms_close_fd (XFASTINT (Fcar (fdpid)));
#else
  close (XFASTINT (Fcar (fdpid)));
#endif
  EMACS_KILLPG (XFASTINT (Fcdr (fdpid)), SIGKILL);
  synch_process_alive = 0;
  return Qnil;
}

Lisp_Object
call_process_cleanup (fdpid)
     Lisp_Object fdpid;
{
#ifdef MSDOS
  /* for MSDOS fdpid is really (fd . tempfile)  */
  register Lisp_Object file;
  file = Fcdr (fdpid);
  close (XFASTINT (Fcar (fdpid)));
  if (strcmp (XSTRING (file)-> data, NULL_DEVICE) != 0)
    unlink (XSTRING (file)->data);
#else /* not MSDOS */
#ifdef VMS
  register int pid = (getpid () & 0xff000000) | (XFASTINT (Fcdr (fdpid)));
#else
  register int pid = XFASTINT (Fcdr (fdpid));
#endif /* VMS */

  if (call_process_exited)
    {
      close (XFASTINT (Fcar (fdpid)));
      return Qnil;
    }

  if (EMACS_KILLPG (pid, SIGINT) == 0)
    {
      int count = specpdl_ptr - specpdl;
      record_unwind_protect (call_process_kill, fdpid);
      message1 ("Waiting for process to die...(type C-g again to kill it instantly)");
      immediate_quit = 1;
      QUIT;
      wait_for_termination (pid);
      immediate_quit = 0;
      specpdl_ptr = specpdl + count; /* Discard the unwind protect.  */
      message1 ("Waiting for process to die...done");
    }
  synch_process_alive = 0;
#ifdef VMS
  vms_close_fd (XFASTINT (Fcar (fdpid)));
#else
  close (XFASTINT (Fcar (fdpid)));
#endif /* VMS */
#endif /* not MSDOS */
  return Qnil;
}

DEFUN ("call-process", Fcall_process, Scall_process, 1, MANY, 0,
  "Call PROGRAM synchronously in separate process.\n\
The program's input comes from file INFILE (nil means `/dev/null').\n\
Insert output in BUFFER before point; t means current buffer;\n\
 nil for BUFFER means discard it; 0 means discard and don't wait;\n\
 1 means give the lines as messages instead.\n\
Fourth arg DISPLAY non-nil means redisplay buffer as output is inserted.\n\
Remaining arguments are strings passed as command arguments to PROGRAM.\n\
If BUFFER is 0, returns immediately with value nil.\n\
Otherwise waits for PROGRAM to terminate\n\
and returns a numeric exit status or a signal description string.\n\
If you quit, the process is killed with SIGINT, or SIGKILL if you quit again.")
  (nargs, args)
     int nargs;
     register Lisp_Object *args;
{
  Lisp_Object infile, buffer, current_dir, display, path;
  int give_messages = 0;
  int fd[2];
  int filefd;
#ifndef VMS
  register int pid;
#else
  int pid;			/* Doesn't really matter, and it takes
				   away a stupid warning below, in the
				   call to lib$spawn().  RL  */
#endif
  char buf[1024];
  int count = specpdl_ptr - specpdl;
  register unsigned char **new_argv
    = (unsigned char **) alloca ((max (2, nargs - 2)) * sizeof (char *));
  struct buffer *old = current_buffer;
#ifdef MSDOS	/* Demacs 1.1.1 91/10/16 HIRANO Satoshi */
  char *outf, *tempfile;
  int outfilefd;
#endif
#ifdef VMS
  char out_dev_name[65];
  $DESCRIPTOR (dout,out_dev_name);
#endif
#if 0
  int mask;
#endif
  CHECK_STRING (args[0], 0);

#ifndef subprocesses
  /* Without asynchronous processes we cannot have BUFFER == 0.  */
  if (nargs >= 3 && XTYPE (args[2]) == Lisp_Int)
    error ("Operating system cannot handle asynchronous subprocesses");
#endif /* subprocesses */

  if (nargs >= 2 && ! NILP (args[1]))
    {
      infile = Fexpand_file_name (args[1], current_buffer->directory);
#ifdef VMS
      /* if the file name doesn't have an extension, add a period. */
      {
	extern unsigned char *strchr (), *strrchr ();
	unsigned char *p = strrchr (XSTRING (infile)->data, ']');
	if (p == 0)
	  p = strrchr (XSTRING (infile)->data, ':');
	if (p == 0)
	  p = XSTRING (infile)->data;
	
	if (strchr (p, '.') == 0)
	  infile = concat2 (infile, build_string ("."));
      }
#endif
      CHECK_STRING (infile, 1);
    }
  else
    infile = build_string (NULL_DEVICE);

  if (nargs >= 3)
    {
      register Lisp_Object tem;

      buffer = tem = args[2];
      if (XTYPE (tem) == Lisp_Int && XFASTINT (tem) == 1)
	{
	  buffer = Qnil;
	  give_messages = 1;
	}
      else
	if (!(EQ (tem, Qnil)
	      || EQ (tem, Qt)
	      || XFASTINT (tem) == 0))
	  {
	    buffer = Fget_buffer (tem);
	    CHECK_BUFFER (buffer, 2);
	  }
    }
  else 
    buffer = Qnil;

  /* Make sure that the child will be able to chdir to the current
     buffer's current directory, or its unhandled equivalent.  We
     can't just have the child check for an error when it does the
     chdir, since it's in a vfork.

     We have to GCPRO around this because Fexpand_file_name,
     Funhandled_file_name_directory, and Ffile_accessible_directory_p
     might call a file name handling function.  The argument list is
     protected by the caller, so all we really have to worry about is
     buffer.  */
  {
    struct gcpro gcpro1, gcpro2, gcpro3;

    current_dir = current_buffer->directory;

    GCPRO3 (infile, buffer, current_dir);

#ifdef VMS
    current_dir = Funhandled_file_name_directory (current_dir);
    if (NILP (Ffile_accessible_directory_p
	      (expand_and_dir_to_file (current_dir, Qnil))))
      report_file_error ("Setting current directory",
			 Fcons (current_buffer->directory, Qnil));
#else
    current_dir
      = expand_and_dir_to_file (Funhandled_file_name_directory (current_dir),
				Qnil);
    if (NILP (Ffile_accessible_directory_p (current_dir)))
      report_file_error ("Setting current directory",
			 Fcons (current_buffer->directory, Qnil));
#endif

    UNGCPRO;
  }

  display = nargs >= 4 ? args[3] : Qnil;

  filefd = open (XSTRING (infile)->data, O_RDONLY, 0);
  if (filefd < 0)
    {
      report_file_error ("Opening process input file", Fcons (infile, Qnil));
    }
#ifndef VMS
  /* Search for program; barf if not found.  */
  {
    struct gcpro gcpro1;

    GCPRO1 (current_dir);
    openp (Vexec_path, args[0], EXEC_SUFFIXES, &path, 1);
    UNGCPRO;
  }
  if (NILP (path))
    {
      close (filefd);
      report_file_error ("Searching for program", Fcons (args[0], Qnil));
    }
  new_argv[0] = XSTRING (path)->data;
#define START 4
#else
  {
    int start = 4;
    unsigned char *tem;
    CHECK_STRING (args[0], 0);
    if (strcmp (XSTRING (args[0])->data, "*dcl*") == 0
	&& nargs > start + 1)
      {
	CHECK_STRING (args[start], 1);
	if (strcmp (XSTRING (args[start])->data, "-c") == 0)
	  {
	    new_argv[0] = XSTRING (args[0])->data;
	    new_argv[1] = XSTRING (args[start])->data;
	    start++;
	    CHECK_STRING (args[start], start);
	    tem = alloca (XSTRING (args[start])->size + 256);
	    strcpy (tem, XSTRING (args[start])->data);
	    new_argv[2] = hack_vms_program_name (tem);
	    start++;
	  }
      }
    else
      {
	tem = alloca (XSTRING (args[0])->size + 256);
	strcpy (tem, XSTRING (args[0])->data);
	new_argv[0] = hack_vms_program_name (tem);
      }
#define START start
#endif /* not VMS */
  {
    register int i;
    for (i = START; i < nargs; i++)
      {
	CHECK_STRING (args[i], i);
	new_argv[i - 3] = XSTRING (args[i])->data;
      }
    new_argv[i - 3] = 0;
  }
#ifdef VMS
  }
#endif

#ifdef MSDOS /* MW, July 1993 */
  /* These vars record information from process termination.
     Clear them now before process can possibly terminate,
     to avoid timing error if process terminates soon.  */
  synch_process_death = 0;
  synch_process_retcode = 0;

  if ((outf = egetenv ("TMP")) || (outf = egetenv ("TEMP")))
    strcpy (tempfile = alloca (strlen (outf) + 20), outf);
  else
    {
      tempfile = alloca (20);
      *tempfile = '\0';
    }
  dostounix_filename (tempfile);
  if (*tempfile == '\0' || tempfile[strlen (tempfile) - 1] != '/') 
    strcat (tempfile, "/");
  strcat (tempfile, "detmp.XXX");
  mktemp (tempfile);

  outfilefd = creat (tempfile, S_IREAD | S_IWRITE);
  if (outfilefd < 0)
    {
      close (filefd);
      report_file_error ("Opening process output file", Fcons (tempfile, Qnil));
    }
#endif

  if (XTYPE (buffer) == Lisp_Int)
#ifndef VMS
    fd[1] = open (NULL_DEVICE, O_WRONLY), fd[0] = -1;
  else
    {
#ifndef MSDOS
      pipe (fd);
#endif
#if 0
      /* Replaced by close_process_descs */
      set_exclusive_use (fd[0]);
#endif
    }
#else
    {
      dout.dsc$b_dtype = DSC$K_DTYPE_T;
      dout.dsc$b_class = DSC$K_CLASS_S;
      dout.dsc$a_pointer = "NLA0:";
      dout.dsc$w_length = strlen (dout.dsc$a_pointer);
      fd[0] = -1;
    }
  else
    {
      if (vms_pipe (fd) < 0)
	error ("can't create mailboxes");
      vms_close_fd (fd[1]);
      vms_get_device_name (fd[0], &dout);
    }
#endif

#ifndef VMS
  {
    /* child_setup must clobber environ in systems with true vfork.
       Protect it from permanent change.  */
    register char **save_environ = environ;
    register int fd1 = fd[1];

#if 0  /* Some systems don't have sigblock.  */
    mask = sigblock (sigmask (SIGCHLD));
#endif

    /* Record that we're about to create a synchronous process.  */
    synch_process_alive = 1;

    /* These vars record information from process termination.
       Clear them now before process can possibly terminate,
       to avoid timing error if process terminates soon.  */
    synch_process_death = 0;
    synch_process_retcode = 0;

#ifdef MSDOS /* MW, July 1993 */
    /* ??? Someone who knows MSDOG needs to check whether this properly
       closes all descriptors that it opens.  */
    pid = run_msdos_command (new_argv, current_dir, filefd, outfilefd);
    close (outfilefd);
    fd1 = -1; /* No harm in closing that one!  */
    fd[0] = open (tempfile, NILP (Vbinary_process_output) ? O_TEXT : O_BINARY);
    if (fd[0] < 0)
      {
	unlink (tempfile);
	close (filefd);
	report_file_error ("Cannot re-open temporary file", Qnil);
      }
#else /* not MSDOS */
    pid = vfork ();

    if (pid == 0)
      {
	if (fd[0] >= 0)
	  close (fd[0]);
#ifdef USG
        setpgrp ();
#else
        setpgrp (pid, pid);
#endif /* USG */
	child_setup (filefd, fd1, fd1, new_argv, 0, current_dir);
      }
#endif /* not MSDOS */

    environ = save_environ;

    /* Close most of our fd's, but not fd[0]
       since we will use that to read input from.  */
    close (filefd);
    if (fd1 >= 0)
      close (fd1);
  }

  if (pid < 0)
    {
      if (fd[0] >= 0)
	close (fd[0]);
      report_file_error ("Doing vfork", Qnil);
    }
#else /* VMS */
  {
    struct dsc$descriptor_s dcmd, din;
    int spawn_flags = CLI$M_NOWAIT;
    int status;
    char oldDir[512];
    extern call_process_ast ();

    close (filefd);

    din.dsc$b_dtype = DSC$K_DTYPE_T;
    din.dsc$b_class = DSC$K_CLASS_S;
    din.dsc$a_pointer = (char *) XSTRING (infile)->data;
    din.dsc$w_length = strlen (XSTRING (infile)->data);

    dcmd.dsc$b_dtype = DSC$K_DTYPE_T;
    dcmd.dsc$b_class = DSC$K_CLASS_S;
    if (strcmp (*new_argv, "*dcl*") == 0)
      {
	if (strcmp (new_argv[1], "-c") == 0)
	  {
	    dcmd.dsc$a_pointer = hack_argv(new_argv + 2);
	    dcmd.dsc$w_length = strlen(dcmd.dsc$a_pointer);
	  }
	else
	  {
	    dcmd.dsc$w_length = 0;
	    dcmd.dsc$a_pointer = (char *)0;
	  }
      }
    else
      {
	dcmd.dsc$a_pointer = hack_argv(new_argv);
	dcmd.dsc$w_length = strlen(dcmd.dsc$a_pointer);
      }

    synch_process_alive = 1;
    /*
      On VMS we need to change the current directory
      of the parent process before forking so that
      the child inherit that directory.  We remember
      where we were before changing.
      */
    VMSgetwd (oldDir);
    chdir (XSTRING (current_dir)->data);

    do {
      spawn_flags = spawn_flags ^ CLI$M_AUTHPRIV;
    
      status = lib$spawn (&dcmd, &din, &dout, &spawn_flags, 0, &pid,
			  &synch_process_retcode, 0, call_process_ast, 0);
    }
    while (status == LIB$_INVARG && (spawn_flags & CLI$M_AUTHPRIV));

    chdir (oldDir);

    free (dcmd.dsc$a_pointer);

    if (!(status & 1))
      {
	char *msg = strerror (EVMSERR, status);
	vms_close_fd (fd[0]);
	if (msg != 0)
	  error ("Unable to spawn subprocess: %s", msg);
	else
	  error ("Unable to spawn subprocess");
      }
  }
#endif

  if (XTYPE (buffer) == Lisp_Int)
    {
      if (fd[0] >= 0)
	close (fd[0]);
#ifndef subprocesses
      /* If Emacs has been built with asynchronous subprocess support,
	 we don't need to do this, I think because it will then have
	 the facilities for handling SIGCHLD.  */
      wait_without_blocking ();
#endif /* subprocesses */
      return Qnil;
    }

  /* Enable sending signal if user quits below.  */
  call_process_exited = 0;

#ifdef MSDOS
  /* MSDOS needs different cleanup information.  */
  record_unwind_protect (call_process_cleanup,
			 Fcons (make_number (fd[0]), build_string (tempfile)));
#else
  record_unwind_protect (call_process_cleanup,
			 Fcons (make_number (fd[0]), make_number (pid)));
#endif /* not MSDOS */


  if (XTYPE (buffer) == Lisp_Buffer)
    Fset_buffer (buffer);

  immediate_quit = 1;
  QUIT;

  {
    register int nread;
    int status = 0;
    int first = 1;

#ifdef VMS
#define tmp_read vms_read_fd
#else
#define tmp_read read
#endif

    while ((nread = tmp_read (fd[0], buf, sizeof buf)) >= 0
	   && errno != EWOULDBLOCK)
      {
#ifdef VMS
	extern int call_process_check_end ();
#if 0
	printf ("nread = %d, errno = %d\n", nread, errno);
#endif
	if (nread == 0 && call_process_check_end ())
	  break;
#else
	if (nread == 0)
	  break;
#endif
	if (nread < 0)
	  continue;		/* EWOULDBLOCK */

	immediate_quit = 0;
#if 0
	if (nread)
	  printf ("buf = \"%*.*s\"\n", nread, nread, buf);
#endif
	if (!NILP (buffer))
	  insert (buf, nread);
	else if (give_messages && nread > 1 && !status)
	  /* output first output line as message line if */
	  /* output not sent to buffer */
          {
            status = 0;

	    /* read () will most probably return several lines.
	       At least on VMS, the first is the most interesting, so
	       let's skip the rest...
	       This should really be fdone a better way...
	       -- Richard Levitte */
	    while (status <= nread && buf[status] > 31) status++;
            if (status)
              {
		buf[status] = 0;
		message ("%s", buf);
              }
	  }
	if (!NILP (display) && INTERACTIVE)
	  {
	    if (first)
	      prepare_menu_bars ();
	    first = 0;
	    redisplay_preserve_echo_area ();
	  }
	immediate_quit = 1;
	QUIT;
      }
#if 0
    printf ("terminating: nread = %d, errno = %d\n", nread, errno);
#endif

#undef tmp_read
  }

  /* Wait for it to terminate, unless it already has.  */
  wait_for_termination (pid);

#ifdef VMS
  vms_close_fd (fd[0]);       /* Need to close output mailbox */
#endif

  immediate_quit = 0;

  set_buffer_internal (old);

  /* Don't kill any children that the subprocess may have left behind
     when exiting.  */
  call_process_exited = 1;

  unbind_to (count, Qnil);

  if (synch_process_death)
    return build_string (synch_process_death);
#ifdef VMS
  {
    extern int vms_split_status_code;
    extern Lisp_Object do_vms_split_status_code ();
    if (vms_split_status_code)
      return do_vms_split_status_code (synch_process_retcode);
  }
#endif
  return make_number (synch_process_retcode);
}

static Lisp_Object
delete_temp_file (name)
     Lisp_Object name;
{
  unlink (XSTRING (name)->data);
}

DEFUN ("call-process-region", Fcall_process_region, Scall_process_region,
  3, MANY, 0,
  "Send text from START to END to a synchronous process running PROGRAM.\n\
Delete the text if fourth arg DELETE is non-nil.\n\
Insert output in BUFFER before point; t means current buffer;\n\
 nil for BUFFER means discard it; 0 means discard and don't wait.\n\
Sixth arg DISPLAY non-nil means redisplay buffer as output is inserted.\n\
Remaining args are passed to PROGRAM at startup as command args.\n\
If BUFFER is nil, returns immediately with value nil.\n\
Otherwise waits for PROGRAM to terminate\n\
and returns a numeric exit status or a signal description string.\n\
If you quit, the process is killed with SIGINT, or SIGKILL if you quit again.")
  (nargs, args)
     int nargs;
     register Lisp_Object *args;
{
  struct gcpro gcpro1;
  Lisp_Object filename_string;
  register Lisp_Object start, end;
#ifdef MSDOS
  char *tempfile;
#else
  char tempfile[28];
#endif
  int count = specpdl_ptr - specpdl;
#ifdef MSDOS
  char *outf = '\0';

  if ((outf = egetenv ("TMP")) || (outf = egetenv ("TEMP")))
    strcpy (tempfile = alloca (strlen (outf) + 20), outf);
  else
    {
      tempfile = alloca (20);
      *tempfile = '\0';
    }
  dostounix_filename (tempfile);
  if (tempfile[strlen (tempfile) - 1] != '/')
    strcat (tempfile, "/");
  strcat (tempfile, "detmp.XXX");
#else /* not MSDOS */

#ifdef VMS
  strcpy (tempfile, "sys$scratch:emacsXXXXXX.tmp");
#else
  strcpy (tempfile, "/tmp/emacsXXXXXX");
#endif
#endif /* not MSDOS */

  mktemp (tempfile);

  filename_string = build_string (tempfile);
  GCPRO1 (filename_string);
  start = args[0];
  end = args[1];
#ifdef MSDOS
  specbind (Qbuffer_file_type, Vbinary_process_input);
  Fwrite_region (start, end, filename_string, Qnil, Qlambda);
  unbind_to (count, Qnil);
#else
  Fwrite_region (start, end, filename_string, Qnil, Qlambda);
#endif

  record_unwind_protect (delete_temp_file, filename_string);

  if (!NILP (args[3]))
    Fdelete_region (start, end);

  args[3] = filename_string;

  RETURN_UNGCPRO (unbind_to (count, Fcall_process (nargs - 2, args + 2)));
}

#ifndef VMS /* VMS version is in vmsproc.c.  */

/* This is the last thing run in a newly forked inferior
   either synchronous or asynchronous.
   Copy descriptors IN, OUT and ERR as descriptors 0, 1 and 2.
   Initialize inferior's priority, pgrp, connected dir and environment.
   then exec another program based on new_argv.

   This function may change environ for the superior process.
   Therefore, the superior process must save and restore the value
   of environ around the vfork and the call to this function.

   ENV is the environment for the subprocess.

   SET_PGRP is nonzero if we should put the subprocess into a separate
   process group.  

   CURRENT_DIR is an elisp string giving the path of the current
   directory the subprocess should have.  Since we can't really signal
   a decent error from within the child, this should be verified as an
   executable directory by the parent.  */

child_setup (in, out, err, new_argv, set_pgrp, current_dir)
     int in, out, err;
     register char **new_argv;
     int set_pgrp;
     Lisp_Object current_dir;
{
#ifdef MSDOS
  /* The MSDOS port of gcc cannot fork, vfork, ... so we must call system
     instead.  */
#else /* not MSDOS */
  char **env;
  char *pwd_var;

  int pid = getpid ();

#ifdef SET_EMACS_PRIORITY
  {
    extern int emacs_priority;

    if (emacs_priority < 0)
      nice (- emacs_priority);
  }
#endif

#ifdef subprocesses
  /* Close Emacs's descriptors that this process should not have.  */
  close_process_descs ();
#endif
  close_load_descs ();

  /* Note that use of alloca is always safe here.  It's obvious for systems
     that do not have true vfork or that have true (stack) alloca.
     If using vfork and C_ALLOCA it is safe because that changes
     the superior's static variables as if the superior had done alloca
     and will be cleaned up in the usual way.  */
  {
    register char *temp;
    register int i;

    i = XSTRING (current_dir)->size;
    pwd_var = (char *) alloca (i + 6);
    temp = pwd_var + 4;
    bcopy ("PWD=", pwd_var, 4);
    bcopy (XSTRING (current_dir)->data, temp, i);
    if (temp[i - 1] != '/') temp[i++] = '/';
    temp[i] = 0;

    /* We can't signal an Elisp error here; we're in a vfork.  Since
       the callers check the current directory before forking, this
       should only return an error if the directory's permissions
       are changed between the check and this chdir, but we should
       at least check.  */
    if (chdir (temp) < 0)
      exit (errno);

    /* Strip trailing slashes for PWD, but leave "/" and "//" alone.  */
    while (i > 2 && temp[i - 1] == '/')
      temp[--i] = 0;
  }

  /* Set `env' to a vector of the strings in Vprocess_environment.  */
  {
    register Lisp_Object tem;
    register char **new_env;
    register int new_length;

    new_length = 0;
    for (tem = Vprocess_environment;
	 (XTYPE (tem) == Lisp_Cons
	  && XTYPE (XCONS (tem)->car) == Lisp_String);
	 tem = XCONS (tem)->cdr)
      new_length++;

    /* new_length + 2 to include PWD and terminating 0.  */
    env = new_env = (char **) alloca ((new_length + 2) * sizeof (char *));

    /* If we have a PWD envvar, pass one down,
       but with corrected value.  */
    if (getenv ("PWD"))
      *new_env++ = pwd_var;

    /* Copy the Vprocess_environment strings into new_env.  */
    for (tem = Vprocess_environment;
	 (XTYPE (tem) == Lisp_Cons
	  && XTYPE (XCONS (tem)->car) == Lisp_String);
	 tem = XCONS (tem)->cdr)
      {
	char **ep = env;
	char *string = (char *) XSTRING (XCONS (tem)->car)->data;
	/* See if this string duplicates any string already in the env.
	   If so, don't put it in.
	   When an env var has multiple definitions,
	   we keep the definition that comes first in process-environment.  */
	for (; ep != new_env; ep++)
	  {
	    char *p = *ep, *q = string;
	    while (1)
	      {
		if (*q == 0)
		  /* The string is malformed; might as well drop it.  */
		  goto duplicate;
		if (*q != *p)
		  break;
		if (*q == '=')
		  goto duplicate;
		p++, q++;
	      }
	  }
	*new_env++ = string;
      duplicate: ;
      }
    *new_env = 0;
  }

  /* Make sure that in, out, and err are not actually already in
     descriptors zero, one, or two; this could happen if Emacs is
     started with its standard in, out, or error closed, as might
     happen under X.  */
  in = relocate_fd (in, 3);
  if (out == err)
    err = out = relocate_fd (out, 3);
  else
    {
      out = relocate_fd (out, 3);
      err = relocate_fd (err, 3);
    }

  close (0);
  close (1);
  close (2);

  dup2 (in, 0);
  dup2 (out, 1);
  dup2 (err, 2);
  close (in);
  close (out);
  close (err);

#ifdef USG
#ifndef SETPGRP_RELEASES_CTTY
  setpgrp ();			/* No arguments but equivalent in this case */
#endif
#else
  setpgrp (pid, pid);
#endif /* USG */
  /* setpgrp_of_tty is incorrect here; it uses input_fd.  */
  EMACS_SET_TTY_PGRP (0, &pid);

#ifdef vipc
  something missing here;
#endif /* vipc */

  /* execvp does not accept an environment arg so the only way
     to pass this environment is to set environ.  Our caller
     is responsible for restoring the ambient value of environ.  */
  environ = env;
  execvp (new_argv[0], new_argv);

  write (1, "Couldn't exec the program ", 26);
  write (1, new_argv[0], strlen (new_argv[0]));
  _exit (1);
#endif /* not MSDOS */
}

/* Move the file descriptor FD so that its number is not less than MIN.
   If the file descriptor is moved at all, the original is freed.  */
int
relocate_fd (fd, min)
     int fd, min;
{
  if (fd >= min)
    return fd;
  else
    {
      int new = dup (fd);
      if (new == -1)
	{
	  char *message1 = "Error while setting up child: ";
	  char *errmessage = err_str (errno);
	  char *message2 = "\n";
	  write (2, message1, strlen (message1));
	  write (2, errmessage, strlen (errmessage));
	  write (2, message2, strlen (message2));
	  _exit (1);
	}
      /* Note that we hold the original FD open while we recurse,
	 to guarantee we'll get a new FD if we need it.  */
      new = relocate_fd (new, min);
      close (fd);
      return new;
    }
}

#endif /* not VMS */
static int
getenv_internal (var, varlen, value, valuelen)
     char *var;
     int varlen;
     char **value;
     int *valuelen;
{
  Lisp_Object scan;

  for (scan = Vprocess_environment; CONSP (scan); scan = XCONS (scan)->cdr)
    {
      Lisp_Object entry;

      entry = XCONS (scan)->car;
      if (XTYPE (entry) == Lisp_String
	  && XSTRING (entry)->size > varlen
	  && XSTRING (entry)->data[varlen] == '='
	  && ! bcmp (XSTRING (entry)->data, var, varlen))
	{
	  *value    = (char *) XSTRING (entry)->data + (varlen + 1);
	  *valuelen = XSTRING (entry)->size - (varlen + 1);
	  return 1;
	}
    }
#ifdef VMS
  /* The following piece of code is inspired from the function make_string()
     in alloc.c */
  /* This is a kludge, I know, but hey, you tell me a better way! */
  {
    extern Lisp_Object make_uninit_string ();
    char *tmp = sys_getenv (var);

    if (tmp != 0)
      {
	register int tmplen = strlen (tmp);
	register Lisp_Object val = make_uninit_string (tmplen + varlen + 1); /* 1 for '=' */

	bcopy (var, XSTRING (val)->data, varlen);
	(XSTRING (val)->data)[varlen] = '=';
	bcopy (tmp, (char *) XSTRING (val)->data + (varlen + 1), tmplen);
	Vprocess_environment = Fcons (val, Vprocess_environment);

	*value    = (char *) XSTRING (val)->data + (varlen + 1);
	*valuelen = XSTRING (val)->size - (varlen + 1);
	return 1;
      }
  }
#endif /* VMS */
  return 0;
}

DEFUN ("getenv", Fgetenv, Sgetenv, 1, 1, 0,
  "Return the value of environment variable VAR, as a string.\n\
VAR should be a string.  Value is nil if VAR is undefined in the environment.\n\
This function consults the variable ``process-environment'' for its value.")
  (var)
     Lisp_Object var;
{
  char *value;
  int valuelen;

  CHECK_STRING (var, 0);
  if (getenv_internal (XSTRING (var)->data, XSTRING (var)->size,
		       &value, &valuelen))
    return make_string (value, valuelen);
  else
    return Qnil;
}

/* A version of getenv that consults process_environment, easily
   callable from C.  */
char *
egetenv (var)
     char *var;
{
  char *value;
  int valuelen;

  if (getenv_internal (var, strlen (var), &value, &valuelen))
    return value;
  else
    return 0;
}


/* This is run before init_cmdargs.  */
  
init_callproc_1 ()
{
  char *data_dir = egetenv ("EMACSDATA");
  char *doc_dir = egetenv ("EMACSDOC");

  Vdata_directory
    = Ffile_name_as_directory (build_string (data_dir ? data_dir 
					     : PATH_DATA));
  Vdoc_directory
    = Ffile_name_as_directory (build_string (doc_dir ? doc_dir
					     : PATH_DOC));

  /* Check the EMACSPATH environment variable, defaulting to the
     PATH_EXEC path from paths.h.  */
  Vexec_path = decode_env_path ("EMACSPATH", PATH_EXEC);
  Vexec_directory = Ffile_name_as_directory (Fcar (Vexec_path));
  Vexec_path = nconc2 (decode_env_path ("PATH", ""), Vexec_path);
}

/* This is run after init_cmdargs, so that Vinvocation_directory is valid.  */

init_callproc ()
{
  char *data_dir = egetenv ("EMACSDATA");
    
  register char * sh;
  Lisp_Object tempdir;

  if (initialized && !NILP (Vinstallation_directory))
    {
      /* Add to the path the lib-src subdir of the installation dir.  */
      Lisp_Object tem;
      tem = Fexpand_file_name (build_string ("lib-src"),
			       Vinstallation_directory);
      if (NILP (Fmember (tem, Vexec_path)))
	{
#ifndef MSDOS
	  /* MSDOS uses wrapped binaries, so don't do this.  */
	  Vexec_path = nconc2 (Vexec_path, Fcons (tem, Qnil));
	  Vexec_directory = Ffile_name_as_directory (tem);
#endif

	  /* If we use ../lib-src, maybe use ../etc as well.
	     Do so if ../etc exists and has our DOC-... file in it.  */
	  if (data_dir == 0)
	    {
	      tem = Fexpand_file_name (build_string ("etc"),
				       Vinstallation_directory);
	      Vdoc_directory = Ffile_name_as_directory (tem);
	    }
	}
    }

  /* Look for the files that should be in etc.  We don't use
     Vinstallation_directory, because these files are never installed
     in /bin near the executable, and they are never in the build
     directory when that's different from the source directory.

     Instead, if these files are not in the nominal place, we try the
     source directory.  */
  if (data_dir == 0)
    {
      Lisp_Object tem, tem1, newdir;

      tem = Fexpand_file_name (build_string ("GNU"), Vdata_directory);
      tem1 = Ffile_exists_p (tem);
      if (NILP (tem1))
	{
	  newdir = Fexpand_file_name (build_string ("../etc/"),
				      build_string (PATH_DUMPLOADSEARCH));
	  tem = Fexpand_file_name (build_string ("GNU"), newdir);
	  tem1 = Ffile_exists_p (tem);
	  if (!NILP (tem1))
	    Vdata_directory = newdir;
	}
    }

  tempdir = Fdirectory_file_name (Vexec_directory);
  if (access (XSTRING (tempdir)->data, 0) < 0)
    {
      fprintf (stderr,
	       "Warning: arch-dependent data dir (%s) does not exist.\n",
	       XSTRING (Vexec_directory)->data);
      sleep (2);
    }

  tempdir = Fdirectory_file_name (Vdata_directory);
  if (access (XSTRING (tempdir)->data, 0) < 0)
    {
      fprintf (stderr,
	       "Warning: arch-independent data dir (%s) does not exist.\n",
	       XSTRING (Vdata_directory)->data);
      sleep (2);
    }

#ifdef VMS
  Vshell_file_name = build_string ("*dcl*");
#else
  sh = (char *) getenv ("SHELL");
  Vshell_file_name = build_string (sh ? sh : "/bin/sh");
#endif
}

set_process_environment ()
{
  register char **envp;

  Vprocess_environment = Qnil;
#ifndef VMS
#ifndef CANNOT_DUMP
  if (initialized)
#endif
    for (envp = environ; *envp; envp++)
      Vprocess_environment = Fcons (build_string (*envp),
				    Vprocess_environment);
#endif
}

syms_of_callproc ()
{
#ifdef MSDOS
  Qbuffer_file_type = intern ("buffer-file-type");
  staticpro (&Qbuffer_file_type);

  DEFVAR_LISP ("binary-process-input", &Vbinary_process_input,
    "*If non-nil then new subprocesses are assumed to take binary input.");
  Vbinary_process_input = Qnil;

  DEFVAR_LISP ("binary-process-output", &Vbinary_process_output,
    "*If non-nil then new subprocesses are assumed to produce binary output.");
  Vbinary_process_output = Qnil;
#endif

  DEFVAR_LISP ("shell-file-name", &Vshell_file_name,
    "*File name to load inferior shells from.\n\
Initialized from the SHELL environment variable.");

  DEFVAR_LISP ("exec-path", &Vexec_path,
    "*List of directories to search programs to run in subprocesses.\n\
Each element is a string (directory name) or nil (try default directory).");

  DEFVAR_LISP ("exec-directory", &Vexec_directory,
    "Directory of architecture-dependent files that come with GNU Emacs,\n\
especially executable programs intended for Emacs to invoke.");

  DEFVAR_LISP ("data-directory", &Vdata_directory,
    "Directory of architecture-independent files that come with GNU Emacs,\n\
intended for Emacs to use.");

  DEFVAR_LISP ("doc-directory", &Vdoc_directory,
    "Directory containing the DOC file that comes with GNU Emacs.\n\
This is usually the same as data-directory.");

  DEFVAR_LISP ("configure-info-directory", &Vconfigure_info_directory,
    "For internal use by the build procedure only.\n\
This is the name of the directory in which the build procedure installed\n\
Emacs's info files; the default value for Info-default-directory-list\n\
includes this.");
  Vconfigure_info_directory = build_string (PATH_INFO);

  DEFVAR_LISP ("process-environment", &Vprocess_environment,
    "List of environment variables for subprocesses to inherit.\n\
Each element should be a string of the form ENVVARNAME=VALUE.\n\
The environment which Emacs inherits is placed in this variable\n\
when Emacs starts.");

  defsubr (&Scall_process);
  defsubr (&Sgetenv);
  defsubr (&Scall_process_region);
}
