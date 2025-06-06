/* rec.c
   Routines to receive a file.

   Copyright (C) 1991, 1992 Ian Lance Taylor

   This file is part of the Taylor UUCP package.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   The author of the program may be contacted at ian@airs.com or
   c/o Infinity Development Systems, P.O. Box 520, Waltham, MA 02254.
   */

#include "uucp.h"

#if USE_RCS_ID
const char rec_rcsid[] = "$Id: rec.c,v 1.21 1993/01/20 05:26:30 ian Rel $";
#endif

#include <errno.h>

#include "uudefs.h"
#include "uuconf.h"
#include "system.h"
#include "prot.h"
#include "trans.h"

/* If the other side does not tell us the size of a file it wants to
   send us, we assume it is this long.  This is only used for free
   space checking.  */
#define CASSUMED_FILE_SIZE (10240)

/* We keep this information in the pinfo field of the stransfer
   structure.  */
struct srecinfo
{
  /* Local user to send mail to (may be NULL).  */
  char *zmail;
  /* Full file name.  */
  char *zfile;
  /* Temporary file name.  */
  char *ztemp;
  /* TRUE if this is a spool directory file.  */
  boolean fspool;
  /* TRUE if this was a local request.  */
  boolean flocal;
  /* TRUE if the file has been completely received.  */
  boolean freceived;
  /* TRUE if remote request has been replied to.  */
  boolean freplied;
  /* TRUE if we moved the file to the final destination.  */
  boolean fmoved;
};

/* This structure is kept in the pinfo field if we are refusing a
   remote request.  */
struct srecfailinfo
{
  /* Reason for refusal.  */
  enum tfailure twhy;
  /* TRUE if we have sent the reason for refusal.  */
  boolean fsent;
  /* TRUE if we have seen the end of the file.  */
  boolean freceived;
};

/* Local functions.  */

static void urrec_free P((struct stransfer *qtrans));
static boolean flocal_rec_fail P((struct stransfer *qtrans,
				  struct scmd *qcmd,
				  const struct uuconf_system *qsys,
				  const char *zwhy));
static boolean flocal_rec_send_request P((struct stransfer *qtrans,
					  struct sdaemon *qdaemon));
static boolean flocal_rec_await_reply P((struct stransfer *qtrans,
					 struct sdaemon *qdaemon,
					 const char *zdata,
					 size_t cdata));
static boolean fremote_send_reply P((struct stransfer *qtrans,
				     struct sdaemon *qdaemon));
static boolean fremote_send_fail P((struct sdaemon *qdaemon,
				    struct scmd *qcmd,
				    enum tfailure twhy,
				    int iremote));
static boolean fremote_send_fail_send P((struct stransfer *qtrans,
					 struct sdaemon *qdaemon));
static boolean fremote_discard P((struct stransfer *qtrans,
				  struct sdaemon *qdaemon,
				  const char *zdata, size_t cdata));
static boolean frec_file_end P((struct stransfer *qtrans,
				struct sdaemon *qdaemon,
				const char *zdata, size_t cdata));
static boolean frec_file_send_confirm P((struct stransfer *qtrans,
					 struct sdaemon *qdaemon));

/* Free up a receive stransfer structure.  */

static void
urrec_free (qtrans)
     struct stransfer *qtrans;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;

  if (qinfo != NULL)
    {
      ubuffree (qinfo->zmail);
      ubuffree (qinfo->zfile);
      ubuffree (qinfo->ztemp);
      xfree (qtrans->pinfo);
    }

  utransfree (qtrans);
}       

/* Set up a request for a file from the remote system.  This may be
   called before the remote system has been called.

   This is the order of function calls:

   flocal_rec_file_init --> fqueue_local
   flocal_rec_send_request (send R ...) --> fqueue_receive
   flocal_rec_await_reply (open file, call pffile) --> fqueue_receive
   receive file
   frec_file_end (close and move file, call pffile) --> fqueue_send
   frec_file_send_confirm (send CY)
   */

boolean
flocal_rec_file_init (qdaemon, qcmd)
     struct sdaemon *qdaemon;
     struct scmd *qcmd;
{
  const struct uuconf_system *qsys;
  boolean fspool;
  char *zfile;
  struct srecinfo *qinfo;
  struct stransfer *qtrans;

  qsys = qdaemon->qsys;

  /* Make sure we are permitted to transfer files.  */
  if (qdaemon->fcaller
      ? ! qsys->uuconf_fcall_transfer
      : ! qsys->uuconf_fcalled_transfer)
    {
      /* This case will have been checked by uucp or uux, but it could
	 have changed.  */
      if (! qsys->uuconf_fcall_transfer
	  && ! qsys->uuconf_fcalled_transfer)
	return flocal_rec_fail ((struct stransfer *) NULL, qcmd, qsys,
				"not permitted to request files");
      return TRUE;
    }

  fspool = fspool_file (qcmd->zto);

  if (fspool)
    {
      pointer puuconf;
      int iuuconf;
      const char *zlocalname;
      struct uuconf_system slocalsys;

      /* Normal users are not allowed to request files to be received
	 into the spool directory.  To support uux forwarding, we use
	 the special option '9'.  This permits a file to be received
	 into the spool directory for the local system only without
	 the usual checking.  This is only done for local requests, of
	 course.  */
      if (qcmd->zto[0] != 'D'
	  || strchr (qcmd->zoptions, '9') == NULL)
	return flocal_rec_fail ((struct stransfer *) NULL, qcmd, qsys,
				"not permitted to receive");

      puuconf = qdaemon->puuconf;
      iuuconf = uuconf_localname (puuconf, &zlocalname);
      if (iuuconf == UUCONF_NOT_FOUND)
	{
	  zlocalname = zsysdep_localname ();
	  if (zlocalname == NULL)
	    return FALSE;
	}
      else if (iuuconf != UUCONF_SUCCESS)
	{
	  ulog_uuconf (LOG_ERROR, puuconf, iuuconf);
	  return FALSE;
	}

      iuuconf = uuconf_system_info (puuconf, zlocalname, &slocalsys);
      if (iuuconf == UUCONF_NOT_FOUND)
	{
	  iuuconf = uuconf_system_local (puuconf, &slocalsys);
	  if (iuuconf != UUCONF_SUCCESS)
	    {
	      ulog_uuconf (LOG_ERROR, puuconf, iuuconf);
	      return FALSE;
	    }
	}
      else if (iuuconf != UUCONF_SUCCESS)
	{
	  ulog_uuconf (LOG_ERROR, puuconf, iuuconf);
	  return FALSE;
	}

      zfile = zsysdep_spool_file_name (&slocalsys, qcmd->zto, qcmd->pseq);

      (void) uuconf_system_free (puuconf, &slocalsys);

      if (zfile == NULL)
	return FALSE;
    }
  else
    {
      zfile = zsysdep_add_base (qcmd->zto, qcmd->zfrom);
      if (zfile == NULL)
	return FALSE;

      /* Check permissions.  */
      if (! fin_directory_list (zfile, qsys->uuconf_pzlocal_receive,
				qsys->uuconf_zpubdir, TRUE,
				FALSE, qcmd->zuser))
	{
	  ubuffree (zfile);
	  return flocal_rec_fail ((struct stransfer *) NULL, qcmd, qsys,
				  "not permitted to receive");
	}

      /* The 'f' option means that directories should not
	 be created if they do not already exist.  */
      if (strchr (qcmd->zoptions, 'f') == NULL)
	{
	  if (! fsysdep_make_dirs (zfile, TRUE))
	    {
	      ubuffree (zfile);
	      return flocal_rec_fail ((struct stransfer *) NULL, qcmd,
				      qsys, "cannot create directories");
	    }
	}
    }

  qinfo = (struct srecinfo *) xmalloc (sizeof (struct srecinfo));
  if (strchr (qcmd->zoptions, 'm') == NULL)
    qinfo->zmail = NULL;
  else
    qinfo->zmail = zbufcpy (qcmd->zuser);
  qinfo->zfile = zfile;
  qinfo->ztemp = NULL;
  qinfo->fspool = fspool;
  qinfo->flocal = TRUE;
  qinfo->freceived = FALSE;
  qinfo->freplied = TRUE;

  qtrans = qtransalc (qcmd);
  qtrans->psendfn = flocal_rec_send_request;
  qtrans->pinfo = (pointer) qinfo;

  return fqueue_local (qdaemon, qtrans);
}

/* Report an error for a local receive request.  */

static boolean
flocal_rec_fail (qtrans, qcmd, qsys, zwhy)
     struct stransfer *qtrans;
     struct scmd *qcmd;
     const struct uuconf_system *qsys;
     const char *zwhy;
{
  if (zwhy != NULL)
    {
      ulog (LOG_ERROR, "%s: %s", qcmd->zfrom, zwhy);
      (void) fmail_transfer (FALSE, qcmd->zuser, (const char *) NULL, zwhy,
			     qcmd->zfrom, qsys->uuconf_zname,
			     qcmd->zto, (const char *) NULL,
			     (const char *) NULL);
      (void) fsysdep_did_work (qcmd->pseq);
    }
  if (qtrans != NULL)
    urrec_free (qtrans);
  return TRUE;
}

/* This is called when we are ready to send the actual request to the
   other system.  */

static boolean
flocal_rec_send_request (qtrans, qdaemon)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;
  long cbytes, cbytes2;
  size_t clen;
  char *zsend;
  boolean fret;

  qinfo->ztemp = zsysdep_receive_temp (qdaemon->qsys, qinfo->zfile,
				       (const char *) NULL);
  if (qinfo->ztemp == NULL)
    {
      urrec_free (qtrans);
      return FALSE;
    }

  /* Check the amount of free space available for both the temporary
     file and the real file.  */
  cbytes = csysdep_bytes_free (qinfo->ztemp);
  cbytes2 = csysdep_bytes_free (qinfo->zfile);
  if (cbytes < cbytes2)
    cbytes = cbytes2;
  if (cbytes != -1)
    {
      cbytes -= qdaemon->qsys->uuconf_cfree_space;
      if (cbytes < 0)
	cbytes = 0;
    }

  if (qdaemon->clocal_size != -1
      && (cbytes == -1 || qdaemon->clocal_size < cbytes))
    cbytes = qdaemon->clocal_size;

  /* We send the string
     R from to user options

     We put a dash in front of options.  If we are talking to a
     counterpart, we also send the maximum size file we are prepared
     to accept, as returned by esysdep_open_receive.  */
  clen = (strlen (qtrans->s.zfrom) + strlen (qtrans->s.zto)
	  + strlen (qtrans->s.zuser) + strlen (qtrans->s.zoptions) + 30);
  zsend = zbufalc (clen);
  if ((qdaemon->ifeatures & FEATURE_SIZES) == 0)
    sprintf (zsend, "R %s %s %s -%s", qtrans->s.zfrom, qtrans->s.zto,
	     qtrans->s.zuser, qtrans->s.zoptions);
  else if ((qdaemon->ifeatures & FEATURE_V103) == 0)
    sprintf (zsend, "R %s %s %s -%s 0x%lx", qtrans->s.zfrom, qtrans->s.zto,
	     qtrans->s.zuser, qtrans->s.zoptions, (unsigned long) cbytes);
  else
    sprintf (zsend, "R %s %s %s -%s %ld", qtrans->s.zfrom, qtrans->s.zto,
	     qtrans->s.zuser, qtrans->s.zoptions, cbytes);

  fret = (*qdaemon->qproto->pfsendcmd) (qdaemon, zsend, qtrans->ilocal,
					qtrans->iremote);
  ubuffree (zsend);
  if (! fret)
    {
      urrec_free (qtrans);
      return FALSE;
    }

  qtrans->fcmd = TRUE;
  qtrans->precfn = flocal_rec_await_reply;

  return fqueue_receive (qdaemon, qtrans);
}

/* This is called when a reply is received for the request.  */

/*ARGSUSED*/
static boolean
flocal_rec_await_reply (qtrans, qdaemon, zdata, cdata)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
     const char *zdata;
     size_t cdata;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;
  long crestart;
  const char *zlog;

  if (zdata[0] != 'R'
      || (zdata[1] != 'Y' && zdata[1] != 'N'))
    {
      ulog (LOG_ERROR, "%s: bad response to receive request: \"%s\"",
	    qtrans->s.zfrom, zdata);
      urrec_free (qtrans);
      return FALSE;
    }

  if (zdata[1] == 'N')
    {
      boolean fnever;
      const char *zerr;

      fnever = TRUE;
      if (zdata[2] == '2')
	zerr = "no such file";
      else if (zdata[2] == '6')
	{
	  /* We sent over the maximum file size we were prepared to
	     receive, and the remote system is telling us that the
	     file is larger than that.  Try again later.  It would be
	     better if we could know whether there will ever be enough
	     room.  */
	  zerr = "too large to receive now";
	  fnever = FALSE;
	}
      else
	zerr = "unknown reason";

      if (fnever)
	return flocal_rec_fail (qtrans, &qtrans->s, qdaemon->qsys, zerr);

      ulog (LOG_ERROR, "%s: %s", qtrans->s.zfrom, zerr);

      urrec_free (qtrans);

      return TRUE;
    }

  /* The mode should have been sent as "RY 0%o".  If it wasn't, we use
     0666.  */
  qtrans->s.imode = (unsigned int) strtol ((char *) (zdata + 2),
					   (char **) NULL, 8);
  if (qtrans->s.imode == 0)
    qtrans->s.imode = 0666;

  /* Open the file to receive into.  We just ignore any restart count,
     since we have no way to tell it to the other side.  SVR4 may have
     some way to do this, but I don't know what it is.  */
  qtrans->e = esysdep_open_receive (qdaemon->qsys, qinfo->zfile,
				    (const char *) NULL, qinfo->ztemp,
				    &crestart);
  if (! ffileisopen (qtrans->e))
    return flocal_rec_fail (qtrans, &qtrans->s, qdaemon->qsys,
			    "cannot open file");

  if (qinfo->fspool)
    zlog = qtrans->s.zto;
  else
    zlog = qinfo->zfile;
  qtrans->zlog = zbufalc (sizeof "Receiving " + strlen (zlog));
  sprintf (qtrans->zlog, "Receiving %s", zlog);

  if (qdaemon->qproto->pffile != NULL)
    {
      boolean fhandled;

      if (! (*qdaemon->qproto->pffile) (qdaemon, qtrans, TRUE, FALSE,
					(long) -1, &fhandled))
	{
	  (void) ffileclose (qtrans->e);
	  return flocal_rec_fail (qtrans, &qtrans->s, qdaemon->qsys,
				  (const char *) NULL);
	}
      if (fhandled)
	return TRUE;
    }

  qtrans->frecfile = TRUE;
  qtrans->psendfn = frec_file_send_confirm;
  qtrans->precfn = frec_file_end;

  return fqueue_receive (qdaemon, qtrans);
}

/* Make sure there is still enough disk space available to receive a
   file.  */

boolean
frec_check_free (qtrans, cfree_space)
     struct stransfer *qtrans;
     long cfree_space;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;
  long cfree1, cfree2;

  cfree1 = csysdep_bytes_free (qinfo->ztemp);
  cfree2 = csysdep_bytes_free (qinfo->zfile);
  if (cfree1 < cfree2)
    cfree1 = cfree2;
  if (cfree1 != -1 && cfree1 < cfree_space)
    {
      ulog (LOG_ERROR, "%s: too big to receive now", qinfo->zfile);
      return FALSE;
    }

  return TRUE;
}

/* A remote request to send a file to the local system, meaning that
   we are going to receive a file.

   If we are using a protocol which does not support multiple
   channels, the remote system will not start sending us the file
   until it has received our confirmation.  In that case, the order of
   functions is as follows:

   fremote_send_file_init (open file) --> fqueue_remote
   fremote_send_reply (send SY, call pffile) --> fqueue_receive
   receive file
   frec_file_end (close and move file, call pffile) --> fqueue_send
   frec_file_send_confirm (send CY)

   If the protocol supports multiple channels, then the remote system
   will start sending the file immediately after the send request.
   That means that the data may come in before remote_send_reply is
   called, so frec_file_end may be called before fremote_send_reply.
   Note that this means the pffile entry points may be called in
   reverse order for such a protocol.

   If the send request is rejected, via fremote_send_fail, and the
   protocol supports multiple channels, we must accept and discard
   data until a zero byte buffer is received from the other side,
   indicating that it has received our rejection.

   This code also handles execution requests, which are very similar
   to send requests.  */

boolean
fremote_send_file_init (qdaemon, qcmd, iremote)
     struct sdaemon *qdaemon;
     struct scmd *qcmd;
     int iremote;
{
  const struct uuconf_system *qsys;
  boolean fspool;
  char *zfile;
  openfile_t e;
  char *ztemp;
  long cbytes, cbytes2;
  long crestart;
  struct srecinfo *qinfo;
  struct stransfer *qtrans;
  const char *zlog;

  qsys = qdaemon->qsys;

  if (! qsys->uuconf_frec_request)
    {
      ulog (LOG_ERROR, "%s: not permitted to receive files from remote",
	    qcmd->zfrom);
      return fremote_send_fail (qdaemon, qcmd, FAILURE_PERM, iremote);
    }
		  
  fspool = fspool_file (qcmd->zto);

  /* We don't accept remote command files.  An execution request may
     only send a simple data file.  */
  if ((fspool && qcmd->zto[0] == 'C')
      || (qcmd->bcmd == 'E'
	  && (! fspool || qcmd->zto[0] != 'D')))
    {
      ulog (LOG_ERROR, "%s: not permitted to receive", qcmd->zfrom);
      return fremote_send_fail (qdaemon, qcmd, FAILURE_PERM, iremote);
    }

  /* See if we have already received this file in a previous
     conversation.  */
  if (fsysdep_already_received (qsys, qcmd->zto, qcmd->ztemp))
    return fremote_send_fail (qdaemon, qcmd, FAILURE_RECEIVED, iremote);

  if (fspool)
    {
      zfile = zsysdep_spool_file_name (qsys, qcmd->zto, (pointer) NULL);
      if (zfile == NULL)
	return FALSE;
    }
  else
    {
      zfile = zsysdep_local_file (qcmd->zto, qsys->uuconf_zpubdir);
      if (zfile != NULL)
	{
	  char *zadd;

	  zadd = zsysdep_add_base (zfile, qcmd->zfrom);
	  ubuffree (zfile);
	  zfile = zadd;
	}
      if (zfile == NULL)
	return FALSE;

      /* Check permissions.  */
      if (! fin_directory_list (zfile, qsys->uuconf_pzremote_receive,
				qsys->uuconf_zpubdir, TRUE,
				FALSE, (const char *) NULL))
	{
	  ulog (LOG_ERROR, "%s: not permitted to receive", zfile);
	  ubuffree (zfile);
	  return fremote_send_fail (qdaemon, qcmd, FAILURE_PERM, iremote);
	}

      if (strchr (qcmd->zoptions, 'f') == NULL)
	{
	  if (! fsysdep_make_dirs (zfile, TRUE))
	    {
	      ubuffree (zfile);
	      return fremote_send_fail (qdaemon, qcmd, FAILURE_OPEN,
					iremote);
	    }
	}
    }

  ztemp = zsysdep_receive_temp (qsys, zfile, qcmd->ztemp);

  /* Adjust the number of bytes we are prepared to receive according
     to the amount of free space we are supposed to leave available
     and the maximum file size we are permitted to transfer.  */
  cbytes = csysdep_bytes_free (ztemp);
  cbytes2 = csysdep_bytes_free (zfile);
  if (cbytes < cbytes2)
    cbytes = cbytes2;

  if (cbytes != -1)
    {
      cbytes -= qsys->uuconf_cfree_space;
      if (cbytes < 0)
	cbytes = 0;
    }

  if (qdaemon->cremote_size != -1
      && (cbytes == -1 || qdaemon->cremote_size < cbytes))
    cbytes = qdaemon->cremote_size;

  /* If the number of bytes we are prepared to receive is less than
     the file size, we must fail.  If the remote did not tell us the
     file size, arbitrarily assumed that it is 10240 bytes.  */
  if (cbytes != -1)
    {
      long csize;

      csize = qcmd->cbytes;
      if (csize == -1)
	csize = CASSUMED_FILE_SIZE;
      if (cbytes < csize)
	{
	  ulog (LOG_ERROR, "%s: too big to receive", zfile);
	  ubuffree (ztemp);
	  ubuffree (zfile);
	  return fremote_send_fail (qdaemon, qcmd, FAILURE_SIZE, iremote);
	}
    }

  /* Open the file to receive into.  This may find an old copy of the
     file, which will be used for file restart if the other side
     supports it.  */
  e = esysdep_open_receive (qsys, zfile, qcmd->ztemp, ztemp, &crestart);
  if (! ffileisopen (e))
    {
      ubuffree (ztemp);
      ubuffree (zfile);
      return fremote_send_fail (qdaemon, qcmd, FAILURE_OPEN, iremote);
    }

  if (crestart > 0)
    {
      if ((qdaemon->ifeatures & FEATURE_RESTART) == 0)
	crestart = -1;
      else
	{
	  DEBUG_MESSAGE1 (DEBUG_UUCP_PROTO,
			  "fremote_send_file_init: Restarting receive from %ld",
			  crestart);
	  if (! ffileseek (e, crestart))
	    {
	      ulog (LOG_ERROR, "seek: %s", strerror (errno));
	      (void) ffileclose (e);
	      ubuffree (ztemp);
	      ubuffree (zfile);
	      return FALSE;
	    }
	}
    }

  qinfo = (struct srecinfo *) xmalloc (sizeof (struct srecinfo));
  if (strchr (qcmd->zoptions, 'n') == NULL)
    qinfo->zmail = NULL;
  else
    qinfo->zmail = zbufcpy (qcmd->znotify);
  qinfo->zfile = zfile;
  qinfo->ztemp = ztemp;
  qinfo->fspool = fspool;
  qinfo->flocal = FALSE;
  qinfo->freceived = FALSE;
  qinfo->freplied = FALSE;

  qtrans = qtransalc (qcmd);
  qtrans->psendfn = fremote_send_reply;
  qtrans->precfn = frec_file_end;
  qtrans->iremote = iremote;
  qtrans->pinfo = (pointer) qinfo;
  qtrans->frecfile = TRUE;
  qtrans->e = e;
  if (crestart > 0)
    qtrans->ipos = crestart;

  if (qcmd->bcmd == 'E')
    zlog = qcmd->zcmd;
  else
    {
      if (qinfo->fspool)
	zlog = qcmd->zto;
      else
	zlog = qinfo->zfile;
    }
  qtrans->zlog = zbufalc (sizeof "Receiving " + strlen (zlog));
  sprintf (qtrans->zlog, "Receiving %s", zlog);

  return fqueue_remote (qdaemon, qtrans);
}

/* Reply to a send request, and prepare to receive the file.  */

static boolean
fremote_send_reply (qtrans, qdaemon)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;
  char ab[50];

  ab[0] = qtrans->s.bcmd;
  ab[1] = 'Y';
  if (qtrans->ipos <= 0)
    ab[2] = '\0';
  else
    sprintf (ab + 2, " 0x%lx", (unsigned long) qtrans->ipos);

  if (! (*qdaemon->qproto->pfsendcmd) (qdaemon, ab, qtrans->ilocal,
				       qtrans->iremote))
    {
      (void) ffileclose (qtrans->e);
      (void) remove (qinfo->ztemp);
      urrec_free (qtrans);
      return FALSE;
    }

  qinfo->freplied = TRUE;

  if (qdaemon->qproto->pffile != NULL)
    {
      boolean fhandled;

      if (! (*qdaemon->qproto->pffile) (qdaemon, qtrans, TRUE, FALSE,
					(long) -1, &fhandled))
	{
	  (void) ffileclose (qtrans->e);
	  (void) remove (qinfo->ztemp);
	  urrec_free (qtrans);
	  return FALSE;
	}
      if (fhandled)
	return TRUE;
    }

  /* If the file has been completely received, we just want to send
     the final confirmation.  Otherwise, we must wait for the file
     first.  */
  qtrans->psendfn = frec_file_send_confirm;
  if (qinfo->freceived)
    return fqueue_send (qdaemon, qtrans);
  else
    return fqueue_receive (qdaemon, qtrans);
}

/* If we can't receive a file, queue up a response to the remote
   system.  */

static boolean
fremote_send_fail (qdaemon, qcmd, twhy, iremote)
     struct sdaemon *qdaemon;
     struct scmd *qcmd;
     enum tfailure twhy;
     int iremote;
{
  struct srecfailinfo *qinfo;
  struct stransfer *qtrans;

  qinfo = (struct srecfailinfo *) xmalloc (sizeof (struct srecfailinfo));
  qinfo->twhy = twhy;
  qinfo->fsent = FALSE;

  /* If the protocol does not support multiple channels (cchans <= 1),
     then we have essentially already received the entire file.  */
  qinfo->freceived = qdaemon->qproto->cchans <= 1;

  qtrans = qtransalc (qcmd);
  qtrans->psendfn = fremote_send_fail_send;
  qtrans->precfn = fremote_discard;
  qtrans->iremote = iremote;
  qtrans->pinfo = (pointer) qinfo;

  return fqueue_remote (qdaemon, qtrans);
}

/* Send a failure string for a send command to the remote system;
   this is called when we are ready to reply to the command.  */

static boolean
fremote_send_fail_send (qtrans, qdaemon)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
{
  struct srecfailinfo *qinfo = (struct srecfailinfo *) qtrans->pinfo;
  char ab[4];
  boolean fret;

  ab[0] = qtrans->s.bcmd;
  ab[1] = 'N';

  switch (qinfo->twhy)
    {
    case FAILURE_PERM:
      ab[2] = '2';
      break;
    case FAILURE_OPEN:
      ab[2] = '4';
      break;
    case FAILURE_SIZE:
      ab[2] = '6';
      break;
    case FAILURE_RECEIVED:
      /* Remember this file as though we successfully received it;
	 when the other side acknowledges our rejection, we know that
	 we no longer have to remember that we received this file.  */
      usent_receive_ack (qdaemon, qtrans);
      ab[2] = '8';
      break;
    default:
      ab[2] = '\0';
      break;
    }
  
  ab[3] = '\0';

  fret = (*qdaemon->qproto->pfsendcmd) (qdaemon, ab, qtrans->ilocal,
					qtrans->iremote);

  qinfo->fsent = TRUE;

  /* Wait for the end of file marker if we haven't gotten it yet.  */
  if (! qinfo->freceived)
    {
      if (! fqueue_receive (qdaemon, qtrans))
	fret = FALSE;
    }
  else
    {
      xfree (qtrans->pinfo);
      utransfree (qtrans);
    }

  return fret;
}

/* Discard data until we reach the end of the file.  This is used for
   a protocol with multiple channels, since the remote system may
   start sending the file before the confirmation is sent.  If we
   refuse the file, the remote system will get us back in synch by
   sending an empty buffer, which is what we look for here.  */

/*ARGSUSED*/
static boolean
fremote_discard (qtrans, qdaemon, zdata, cdata)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
     const char *zdata;
     size_t cdata;
{
  struct srecfailinfo *qinfo = (struct srecfailinfo *) qtrans->pinfo;

  DEBUG_MESSAGE1 (DEBUG_UUCP_PROTO,
		  "fremote_discard: Discarding %lu bytes",
		  (unsigned long) cdata);

  if (cdata != 0)
    return TRUE;

  qinfo->freceived = TRUE;

  /* If we have already sent the denial, we are done.  */
  if (qinfo->fsent)
    {
      xfree (qtrans->pinfo);
      utransfree (qtrans);
    }

  return TRUE;
}

/* This is called when a file has been completely received.  It sends
   a response to the remote system.  */

/*ARGSUSED*/
static boolean
frec_file_end (qtrans, qdaemon, zdata, cdata)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
     const char *zdata;
     size_t cdata;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;
  const char *zerr;
  boolean fnever;

  DEBUG_MESSAGE3 (DEBUG_UUCP_PROTO, "frec_file_end: %s to %s (freplied %s)",
		  qtrans->s.zfrom, qtrans->s.zto,
		  qinfo->freplied ? "TRUE" : "FALSE");

  if (qdaemon->qproto->pffile != NULL)
    {
      boolean fhandled;

      if (! (*qdaemon->qproto->pffile) (qdaemon, qtrans, FALSE, FALSE,
					(long) -1, &fhandled))
	{
	  (void) ffileclose (qtrans->e);
	  (void) remove (qinfo->ztemp);
	  urrec_free (qtrans);
	  return FALSE;
	}
      if (fhandled)
	return TRUE;
    }

  qinfo->freceived = TRUE;

  fnever = FALSE;

  if (! ffileclose (qtrans->e))
    {
      zerr = strerror (errno);
      ulog (LOG_ERROR, "%s: close: %s", qtrans->s.zto, zerr);
    }
  else if (! fsysdep_move_file (qinfo->ztemp, qinfo->zfile, qinfo->fspool,
				FALSE, ! qinfo->fspool,
				(qinfo->flocal
				 ? qtrans->s.zuser
				 : (const char *) NULL)))
    {
      zerr = "could not move to final location";
      ulog (LOG_ERROR, "%s: %s", qinfo->zfile, zerr);
      fnever = TRUE;
    }
  else
    {
      if (! qinfo->fspool)
	{
	  unsigned int imode;

	  /* Unless we can change the ownership of the file, the only
	     choice to make about these bits is whether to set the
	     execute bit or not.  */
	  if ((qtrans->s.imode & 0111) != 0)
	    imode = 0777;
	  else
	    imode = 0666;
	  (void) fsysdep_change_mode (qinfo->zfile, imode);
	}
  
      zerr = NULL;
    }

  if (zerr != NULL)
    (void) remove (qinfo->ztemp);

  ustats (zerr == NULL, qtrans->s.zuser, qdaemon->qsys->uuconf_zname,
	  FALSE, qtrans->cbytes, qtrans->isecs, qtrans->imicros,
	  qdaemon->fmaster);

  if (zerr == NULL)
    {
      if (qinfo->zmail != NULL && *qinfo->zmail != '\0')
	(void) fmail_transfer (TRUE, qtrans->s.zuser, qinfo->zmail,
			       (const char *) NULL,
			       qtrans->s.zfrom, qdaemon->qsys->uuconf_zname,
			       qtrans->s.zto, (const char *) NULL,
			       (const char *) NULL);

      if (qtrans->s.pseq != NULL)
	(void) fsysdep_did_work (qtrans->s.pseq);

      if (! qinfo->flocal)
	{
	  /* Remember that we have received this file, so that if the
	     connection drops at this point we won't receive it again.
	     We could check the return value here, but if we return
	     FALSE we couldn't do anything but drop the connection,
	     which would hardly be reasonable.  Instead we trust that
	     the administrator will notice and handle any error
	     messages, which are very unlikely to occur if everything
	     is set up correctly.  */
	  (void) fsysdep_remember_reception (qdaemon->qsys, qtrans->s.zto,
					     qtrans->s.ztemp);
	}
    }
  else
    {
      /* If the transfer failed, we send mail if it was requested
	 locally and if it can never succeed.  */
      if (qinfo->flocal && fnever)
	{
	  (void) fmail_transfer (FALSE, qtrans->s.zuser, qinfo->zmail,
				 zerr, qtrans->s.zfrom,
				 qdaemon->qsys->uuconf_zname,
				 qtrans->s.zto, (const char *) NULL,
				 (const char *) NULL);
	  (void) fsysdep_did_work (qtrans->s.pseq);
	}
    }

  /* If this is an execution request, we must create the execution
     file itself.  */
  if (qtrans->s.bcmd == 'E' && zerr == NULL)
    {
      char *zxqt, *zxqtfile, *ztemp;
      FILE *e;
      boolean fbad;

      /* We get an execution file name by simply replacing the leading
	 D in the received file name with an X.  This pretty much
	 always has to work since we can always receive a file name
	 starting with X, so the system dependent code must be
	 prepared to see one.  */
      zxqt = zbufcpy (qtrans->s.zto);
      zxqt[0] = 'X';
      zxqtfile = zsysdep_spool_file_name (qdaemon->qsys, zxqt,
					  (pointer) NULL);
      ubuffree (zxqt);

      if (zxqtfile == NULL)
	{
	  urrec_free (qtrans);
	  return FALSE;
	}

      /* We have to write via a temporary file, because otherwise
	 uuxqt might pick up the file before we have finished writing
	 it.  */
      e = NULL;
      ztemp = zsysdep_receive_temp (qdaemon->qsys, zxqtfile, "D.0");
      if (ztemp != NULL)
	e = esysdep_fopen (ztemp, FALSE, FALSE, TRUE);

      if (e == NULL)
	{
	  ubuffree (zxqtfile);
	  ubuffree (ztemp);
	  urrec_free (qtrans);
	  return FALSE;
	}

      fprintf (e, "U %s %s\n", qtrans->s.zuser, qdaemon->qsys->uuconf_zname);
      fprintf (e, "F %s\n", qtrans->s.zto);
      fprintf (e, "I %s\n", qtrans->s.zto);
      if (strchr (qtrans->s.zoptions, 'N') != NULL)
	fprintf (e, "N\n");
      if (strchr (qtrans->s.zoptions, 'Z') != NULL)
	fprintf (e, "Z\n");
      if (strchr (qtrans->s.zoptions, 'R') != NULL)
	fprintf (e, "R %s\n", qtrans->s.znotify);
      if (strchr (qtrans->s.zoptions, 'e') != NULL)
	fprintf (e, "e\n");
      fprintf (e, "C %s\n", qtrans->s.zcmd);

      fbad = FALSE;
      if (fclose (e) == EOF)
	{
	  ulog (LOG_ERROR, "fclose: %s", strerror (errno));
	  (void) remove (ztemp);
	  fbad = TRUE;
	}

      if (! fbad)
	{
	  if (! fsysdep_move_file (ztemp, zxqtfile, TRUE, FALSE, FALSE,
				   (const char *) NULL))
	    fbad = TRUE;
	}

      ubuffree (zxqtfile);
      ubuffree (ztemp);

      if (fbad)
	{
	  urrec_free (qtrans);
	  return FALSE;
	}
    }

  /* Prepare to send the completion string to the remote system.  If
     we have not yet replied to the remote send request, we leave the
     transfer structure on the remote queue.  Otherwise we add it to
     the send queue.  The psendfn field will already be set.  */
  qinfo->fmoved = zerr == NULL;
  if (qinfo->freplied)
    return fqueue_send (qdaemon, qtrans);

  return TRUE;
}

/* Send the final confirmation string to the remote system.  */

static boolean
frec_file_send_confirm (qtrans, qdaemon)
     struct stransfer *qtrans;
     struct sdaemon *qdaemon;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;
  const char *zsend;
  boolean fret;

  if (! qinfo->fmoved)
    zsend = "CN5";
  else if (! qdaemon->frequest_hangup)
    zsend = "CY";
  else
    {
#if DEBUG > 0
      if (qdaemon->fmaster)
	ulog (LOG_FATAL, "frec_file_send_confirm: Can't happen");
#endif

      DEBUG_MESSAGE0 (DEBUG_UUCP_PROTO,
		      "frec_send_file_confirm: Requesting remote to transfer control");
      zsend = "CYM";
    }

  fret = (*qdaemon->qproto->pfsendcmd) (qdaemon, zsend,
					qtrans->ilocal, qtrans->iremote);

  /* Now, if that was a remote command, then when the confirmation
     message is acked we no longer have to remember that we received
     that file.  */
  if (! qinfo->flocal && qinfo->fmoved)
    usent_receive_ack (qdaemon, qtrans);

  urrec_free (qtrans);
  return fret;
}

/* Discard a temporary file if it is not useful.  A temporary file is
   useful if it could be used to restart a receive.  This is called if
   the connection is lost.  It is only called if qtrans->frecfile is
   TRUE.  */

boolean
frec_discard_temp (qdaemon, qtrans)
     struct sdaemon *qdaemon;
     struct stransfer *qtrans;
{
  struct srecinfo *qinfo = (struct srecinfo *) qtrans->pinfo;

  if ((qdaemon->ifeatures & FEATURE_RESTART) == 0
      || qtrans->s.ztemp == NULL
      || qtrans->s.ztemp[0] != 'D'
      || strcmp (qtrans->s.ztemp, "D.0") == 0)
    (void) remove (qinfo->ztemp);
  return TRUE;
}
