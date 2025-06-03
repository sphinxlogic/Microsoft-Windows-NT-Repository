/* @(#)slipd_tuucp.c	1.1 (30 Sep 1993) */
/*..........................................................................
 . Copyright 1993, Jim Avera.  All Rights Reserved.
 . 
 . This program contains new material owned by the above copyright holder,
 . and may also contain portions derived from existing works used according
 . to permission granted by the owners of those works.
 .
 . You are prohibited from copying, distributing, modifying, or using this
 . file (or the portions owned by the above copyright holder) except as 
 . described in the file "COPYRIGHT" which accompanies this program.
 ...........................................................................*/

/* Taylor UUCP is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version. */

/* 
 * SLIP DIAL MODULE USING Taylor UUCP (linked with slipd.o)
 *
 * This version of sldial uses the Taylor UUCP dialing routines to
 * reach the remote host.
 *
 * This file created by Jim Avera (jima@netcom.com) June 1993.
 * Most of the code was copied from the Taylor cu.c and related files, 
 * written by Ian Lance Taylor.
 *
 * This file must be compiled and linked after Taylor uucp has been built.
 */
#include "uucp.h"

#include "uudefs.h"
#include "uuconf.h"
#include "conn.h"
#include "prot.h"
#include "system.h"
#include "sysdep.h"
#include "getopt.h"

#include "slipd.h"

/* The program name, for taylor logging utilities */
char abProgram[] = "slipd-taylor";

/* Global SLIP network interface name (used in diagnostic message) */
char *zifng;

/* Global uuconf pointer.  */
static pointer pCuuuconf;

/* Connection.  */
static struct sconnection *qCuconn;

/* Whether to close the connection.  */
static boolean fCuclose_conn;

/* Dialer used to dial out.  */
static struct uuconf_dialer *qCudialer;

/* Whether we need to restore the terminal.  */
static boolean fCurestore_terminal;

/* Whether we are doing local echoing.  */
static boolean fCulocalecho;

/* Globals needed by ../../unix/cusub.o, but not actually used by slipd */
const char *zCuvar_escape = "";
const char *zCuvar_eol = "";

/* A structure used to pass information to icuport_lock.  */
struct sconninfo
{
  boolean fmatched;
  boolean flocked;
  struct sconnection *qconn;
  const char *zline;
};

/* Local functions.  */

static void ucuusage P((void));
static void ucuabort P((void));
static int icuport_lock P((struct uuconf_port *qport, pointer pinfo));
static void log_bad_paramstring ();

/* Long getopt options.  */
static const struct option asCulongopts[] = { { NULL, 0, NULL, 0 } };

/* Call state -- held across calls to slipd_dialout and slipd_undial */
static struct sconnection sconn;
static struct uuconf_dialer sdialer;

/*
 * DIAL OUT FOR SLIP
 *
 * RETURNS: TRUE (non-zero) on success, and stores file descriptor and
 *  undial state pointer by reference.
 *  
 * The device is later closed and unlocked by calling slipd_undial(pstate).
 */
int /*ARGSUSED3*/
slipd_dialout(zdialparams, zifn, pifd, ppstate)
char *zdialparams;
char *zifn;
int *pifd;		/* ptr to place to return file descriptor */
void	**ppstate;	/* ptr to place to return undial state pointer */
{
  /* -c: phone number.  */
  char *zphone = NULL;
  /* -e: even parity.  */
  boolean feven = FALSE;
  /* -l: line.  */
  char *zline = NULL;
  /* -o: odd parity.  */
  boolean fodd = FALSE;
  /* -p: port name.  */
  const char *zport = NULL;
  /* -s: speed.  */
  long ibaud = 0L;
  /* -z: system.  */
  const char *zsystem = NULL;
  /* -I: configuration file name.  */
  const char *zconfig = NULL;

  int iopt;
  pointer puuconf;
  int iuuconf;
  char *z;
  int i;
  struct uuconf_system ssys;
  const struct uuconf_system *qsys = NULL;
  boolean flooped;
  struct uuconf_port sport;
  struct sconninfo sinfo;
  long ihighbaud;
  struct uuconf_dialer *qdialer;

  int iargc;
  char **pcargv = NULL;
  int iargv_alloc = 0;

  zifng = zifn;
  /*
   * SCAN THE ARGUMENT STRING
   *
   * The dial parameter string consists of args as with cu(1).
   * A shell is not used to parse this string, and so shell syntax
   * (e.g., quotes, embedded blanks) are NOT allowed.
   */
  z = xmalloc( strlen(abProgram) + strlen(zdialparams) + 2 );
  strcpy(z, abProgram);
  strcat(z," ");
  strcat(z,zdialparams);
  iargc = _uuconf_istrsplit (z, 0, &pcargv, &iargv_alloc);

  /* We want to accept -# as a speed.  It's easiest to look through
     the arguments, replace -# with -s#, and let getopt handle it.  */
  for (i = 1; i < iargc; i++)
    {
      if (pcargv[i][0] == '-'
	  && isdigit (BUCHAR (pcargv[i][1])))
	{
	  size_t clen;
	  char *z;

	  clen = strlen (pcargv[i]);
	  z = zbufalc (clen + 2);
	  z[0] = '-';
	  z[1] = 's';
	  memcpy (z + 2, pcargv[i] + 1, clen);
 	  pcargv[i] = z;
	}
    }

  while ((iopt = getopt_long (iargc, pcargv, "a:c:dehnI:l:op:s:S:tx:z:",
			      asCulongopts, (int *) NULL)) != EOF)
    {
      switch (iopt)
	{
	case 'c':
	  /* Phone number.  */
	  zphone = optarg;
	  break;

	case 'd':
	  /* Set debugging level to maximum.  */
#if DEBUG > 1
	  iDebug = DEBUG_MAX;
#endif
	  break;

	case 'e':
	  /* Even parity.  */
	  feven = TRUE;
	  break;

	case 'l':
	  /* Line name.  */
	  zline = optarg;
	  break;

	case 'o':
	  /* Odd parity.  */
	  fodd = TRUE;
	  break;

	case 'p':
	case 'a':
	  /* Port name (-a is for compatibility).  */
	  zport = optarg;
	  break;

	case 's':
	  /* Speed.  */
	  ibaud = strtol (optarg, (char **) NULL, 10);
	  break;

	case 'z':
	  /* System name.  */
	  zsystem = optarg;
	  break;

	case 'I':
	  /* Configuration file name.  */
	  if (fsysdep_other_config (optarg))
	    zconfig = optarg;
	  break;

	case 'x':
#if DEBUG > 1
	  /* Set debugging level.  */
	  iDebug |= idebug_parse (optarg);
#endif
	  break;

	case 0:
	  /* Long option found and flag set.  */
	  break;

	default:
	  log_bad_paramstring ();
	  return 0;
	}
    }

  /* There can be one more argument, which is either a system name, a
     phone number, or "dir".  We decide which it is based on the first
     character.  To call a UUCP system whose name begins with a digit,
     or one which is named "dir", you must use -z.  */
  if (optind != iargc)
    {
      if (optind != iargc - 1
	  || zsystem != NULL
	  || zphone != NULL)
        {
	  log_bad_paramstring ();
	  return 0;
	}
      if (strcmp (pcargv[optind], "dir") != 0)
	{
	  if (isdigit (BUCHAR (pcargv[optind][0])))
	    zphone = pcargv[optind];
	  else
	    zsystem = pcargv[optind];
	}
    }

  /* If the user doesn't give a system, port, line or speed, then
     there's no basis on which to select a port.  */
  if (zsystem == NULL
      && zport == NULL
      && zline == NULL
      && ibaud == 0L)
    {
      log_bad_paramstring ();
      return 0;
    }

  /*-----------------------------------*/

  iuuconf = uuconf_init (&puuconf, abProgram, zconfig);
  if (iuuconf != UUCONF_SUCCESS)
    ulog_uuconf (LOG_FATAL, puuconf, iuuconf);
  pCuuuconf = puuconf;

#if DEBUG > 1
  {
    const char *zdebug;

    iuuconf = uuconf_debuglevel (puuconf, &zdebug);
    if (iuuconf != UUCONF_SUCCESS)
      ulog_uuconf (LOG_FATAL, puuconf, iuuconf);
    if (zdebug != NULL)
      iDebug |= idebug_parse (zdebug);
  }
#endif

  usysdep_initialize (puuconf, INIT_NOCHDIR | INIT_NOCLOSEFILES);

  ulog_fatal_fn (ucuabort);

#ifdef SIGINT
  usysdep_signal (SIGINT);
#endif
#ifdef SIGHUP
  usysdep_signal (SIGHUP);
#endif
#ifdef SIGQUIT
  usysdep_signal (SIGQUIT);
#endif
#ifdef SIGTERM
  usysdep_signal (SIGTERM);
#endif
#ifdef SIGPIPE
  usysdep_signal (SIGPIPE);
#endif

  if (zsystem != NULL)
    {
      iuuconf = uuconf_system_info (puuconf, zsystem, &ssys);
      if (iuuconf != UUCONF_SUCCESS)
	{
	  if (iuuconf != UUCONF_NOT_FOUND)
	    ulog_uuconf (LOG_FATAL, puuconf, iuuconf);
	    {
              ulog (LOG_ERROR, "%s: System not found", zsystem);
              return 0;
	    }
	}
      qsys = &ssys;
    }

  /* This loop is used if a system is specified.  It loops over the
     various alternates until it finds one for which the dial
     succeeds.  This is an ugly spaghetti construction, and it should
     be broken up into different functions someday.  */
  flooped = FALSE;
  for (;;) 
    {
      enum tparitysetting tparity;
      enum tstripsetting tstrip;

      /* The uuconf_find_port function only selects directly on a port
	 name and a speed.  To select based on the line name, we use a
	 function.  If we can't find any defined port, and the user
	 specified a line name but did not specify a port name or a
	 system or a phone number, then we fake a direct port with
	 that line name (we don't fake a port if a system or phone
	 number were given because if we fake a port we have no way to
	 place a call; perhaps we should automatically look up a
	 particular dialer).  This permits users to say cu -lttyd0
	 without having to put ttyd0 in the ports file, provided they
	 have read and write access to the port.  */
      sinfo.fmatched = FALSE;
      sinfo.flocked = FALSE;
      sinfo.qconn = &sconn;
      sinfo.zline = zline;
      if (zport != NULL || zline != NULL || ibaud != 0L)
	{
	  iuuconf = uuconf_find_port (puuconf, zport, ibaud, 0L,
				      icuport_lock, (pointer) &sinfo,
				      &sport);
	  if (iuuconf != UUCONF_SUCCESS)
	    {
	      if (iuuconf != UUCONF_NOT_FOUND)
		{
		  if (sinfo.flocked)
		    {
		      (void) fconn_unlock (&sconn);
		      uconn_free (&sconn);
		    }
		  ulog_uuconf (LOG_FATAL, puuconf, iuuconf);
		}
	      if (zline == NULL
		  || zport != NULL
		  || zphone != NULL
		  || qsys != NULL)
		{
		  if (sinfo.fmatched)
		    ulog (LOG_FATAL, "All matching ports in use");
		  else
		    ulog (LOG_FATAL, "No matching ports");
		}

	      sport.uuconf_zname = zline;
	      sport.uuconf_ttype = UUCONF_PORTTYPE_DIRECT;
	      sport.uuconf_zprotocols = NULL;
	      sport.uuconf_qproto_params = NULL;
	      sport.uuconf_ireliable = 0;
	      sport.uuconf_zlockname = NULL;
	      sport.uuconf_palloc = NULL;
	      sport.uuconf_u.uuconf_sdirect.uuconf_zdevice = NULL;
	      sport.uuconf_u.uuconf_sdirect.uuconf_ibaud = ibaud;

	      if (! fconn_init (&sport, &sconn))
		ucuabort ();

	      if (! fconn_lock (&sconn, FALSE))
		ulog (LOG_FATAL, "%s: Line in use", zline);

	      qCuconn = &sconn;
	    }
	  ihighbaud = 0L;
	}
      else
	{
	  for (; qsys != NULL; qsys = qsys->uuconf_qalternate)
	    {
	      if (! qsys->uuconf_fcall)
		continue;
	      if (qsys->uuconf_qport != NULL)
		{
		  if (fconn_init (qsys->uuconf_qport, &sconn))
		    {
		      if (fconn_lock (&sconn, FALSE))
			{
			  qCuconn = &sconn;
			  break;
			}
		      uconn_free (&sconn);
		    }
		}
	      else
		{
		  sinfo.fmatched = FALSE;
		  sinfo.flocked = FALSE;
		  sinfo.qconn = &sconn;
		  iuuconf = uuconf_find_port (puuconf, qsys->uuconf_zport,
					      qsys->uuconf_ibaud,
					      qsys->uuconf_ihighbaud,
					      icuport_lock,
					      (pointer) &sinfo,
					      &sport);
		  if (iuuconf == UUCONF_SUCCESS)
		    break;
		  if (iuuconf != UUCONF_NOT_FOUND)
		    {
		      if (sinfo.flocked)
			{
			  (void) fconn_unlock (&sconn);
			  uconn_free (&sconn);
			}
		      ulog_uuconf (LOG_FATAL, puuconf, iuuconf);
		    }
		}
	    }

	  if (qsys == NULL)
	    {
	      const char *zrem;

	      if (flooped)
		zrem = "remaining ";
	      else
		zrem = "";
	      if (sinfo.fmatched)
		ulog (LOG_FATAL, "%s: All %smatching ports in use",
		      zsystem, zrem);
	      else
		ulog (LOG_FATAL, "%s: No %smatching ports", zsystem, zrem);
	    }

	  ibaud = qsys->uuconf_ibaud;
	  ihighbaud = qsys->uuconf_ihighbaud;
	}

      /* Here we have locked a connection to use.  */
      if (! fconn_open (&sconn, ibaud, ihighbaud, FALSE))
	ucuabort ();

      fCuclose_conn = TRUE;

      if (FGOT_SIGNAL ())
	ucuabort ();

      /* Set up the connection.  */
      if (fodd && feven)
	{
	  tparity = PARITYSETTING_NONE;
	  tstrip = STRIPSETTING_SEVENBITS;
	}
      else if (fodd)
	{
	  tparity = PARITYSETTING_ODD;
	  tstrip = STRIPSETTING_SEVENBITS;
	}
      else if (feven)
	{
	  tparity = PARITYSETTING_EVEN;
	  tstrip = STRIPSETTING_SEVENBITS;
	}
      else
	{
	  tparity = PARITYSETTING_DEFAULT;
	  tstrip = STRIPSETTING_DEFAULT;
	}

      if (! fconn_set (&sconn, tparity, tstrip, XONXOFF_ON))
	ucuabort ();

      if (qsys != NULL)
	zphone = qsys->uuconf_zphone;

      if (qsys != NULL || zphone != NULL)
	{
	  enum tdialerfound tdialer;

	  if (! fconn_dial (&sconn, puuconf, qsys, zphone, &sdialer,
			    &tdialer))
	    {
	      if (zport != NULL
		  || zline != NULL
		  || ibaud != 0L
		  || qsys == NULL)
		ucuabort ();

	      if (qsys->uuconf_qalternate == NULL)
		ulog (LOG_FATAL, "%s: No remaining alternates", zsystem);
	      else
		qsys= qsys->uuconf_qalternate;

	      fCuclose_conn = FALSE;
	      (void) fconn_close (&sconn, pCuuuconf, qCudialer, FALSE);
	      qCuconn = NULL;
	      (void) fconn_unlock (&sconn);
	      uconn_free (&sconn);

	      /* Loop around and try another alternate.  */
	      flooped = TRUE;
	      continue;
	    }
	  if (tdialer == DIALERFOUND_FALSE)
	    qdialer = NULL;
	  else
	    qdialer = &sdialer;
	}
      else
	{
	  /* If no system or phone number was specified, we connect
	     directly to the modem.  

	     At this point cu checks to see if the user has access to the
	     modem device file, but we skip this check because we aren't
	     suid (and are normally invoked as root). */
	  qdialer = NULL;
	  if (! fconn_carrier (&sconn, FALSE))
	    ulog (LOG_FATAL, "Can't turn off carrier");
	}

      break;
    }

  qCudialer = qdialer;

  if (FGOT_SIGNAL ())
    ucuabort ();

  /* Run the log-in chat, if present.
   * (This code was taken from uucico.c:fdo_call) */
  if (sconn.qport == NULL)
    zport = "unknown";
  else
    zport = sconn.qport->uuconf_zname;
  if (! fchat (&sconn, puuconf, &qsys->uuconf_schat, qsys,
	       (const struct uuconf_dialer *) NULL,
	       (const char *) NULL, FALSE, zport,
	       iconn_baud (&sconn)))
    {
      ucuabort ();
    }

  /* --- SUCCESS --- */
  ulog_close ();
  *pifd = ((struct ssysdep_conn *)(sconn.psysdep))->o;
  /* *ppstate = &sconn; */
  return 1;	/* TRUE */
}

void /*ARGSUSED*/
slipd_undial(pstate)
void *pstate;
{
  struct sconnection *qconn;
  if (fCuclose_conn) {
      fCuclose_conn = FALSE;
      (void) fconn_close (qCuconn, pCuuuconf, qCudialer, FALSE);
  }
  qconn = qCuconn;
  qCuconn = NULL;
  (void) fconn_unlock (qconn);
  uconn_free (qconn);
  
  ulog_close ();
}


/* Check to see if this port has the desired line, to handle the -l
   option.  If it does, or if no line was specified, set up a
   connection and lock it.  */

static int
icuport_lock (qport, pinfo)
     struct uuconf_port *qport;
     pointer pinfo;
{
  struct sconninfo *q = (struct sconninfo *) pinfo;

  if (q->zline != NULL
      && ! fsysdep_port_is_line (qport, q->zline))
    return UUCONF_NOT_FOUND;

  q->fmatched = TRUE;

  if (! fconn_init (qport, q->qconn))
    return UUCONF_NOT_FOUND;
  else if (! fconn_lock (q->qconn, FALSE))
    {
      uconn_free (q->qconn);
      return UUCONF_NOT_FOUND;
    }
  else
    {
      qCuconn = q->qconn;
      q->flocked = TRUE;
      return UUCONF_SUCCESS;
    }
}

static void
log_bad_paramstring ()
{
  fprintf (stderr, "%s: Invalid auto-dial parameter string for slip interface", abProgram);
  if (zifng) fprintf(stderr," %s", zifng);
  fprintf(stderr,".\n");
  fprintf (stderr,
	   "\nThis SLIP dialer uses part of Taylor UUCP,\ncopyright (C) 1991, 1992 Ian Lance Taylor\n");
  fprintf (stderr,
	   "\nParameter string usage: [options] [system or phone-number]\n");
  fprintf (stderr,
	   " -a port, -p port: Use named port\n");
  fprintf (stderr,
	   " -l line: Use named device (e.g. tty00)\n");
  fprintf (stderr,
	   " -s speed, -#: Use given speed\n");
  fprintf (stderr,
	   " -c phone: Phone number to call\n");
  fprintf (stderr,
	   " -z system: System to call\n");
  fprintf (stderr,
	   " -e: Set even parity\n");
  fprintf (stderr,
	   " -o: Set odd parity\n");
  fprintf (stderr,
	   " -d: Set maximum debugging level\n");
  fprintf (stderr,
	   " -x debug: Set debugging type\n");
#if HAVE_TAYLOR_CONFIG
  fprintf (stderr,
	   " -I file: Set configuration file to use\n");
#endif /* HAVE_TAYLOR_CONFIG */
}

/* This function is called when a fatal error occurs */
static void
ucuabort ()
{
  if (fCurestore_terminal)
    {
      fCurestore_terminal = FALSE;
      (void) fsysdep_terminal_restore ();
    }

  if (qCuconn != NULL)
    {
      struct sconnection *qconn;

      if (fCuclose_conn)
	{
	  fCuclose_conn = FALSE;
	  (void) fconn_close (qCuconn, pCuuuconf, qCudialer, FALSE);
	}
      qconn = qCuconn;
      qCuconn = NULL;
      (void) fconn_unlock (qconn);
      uconn_free (qconn);
    }

  ulog_close ();
  usysdep_exit (FALSE);
}
