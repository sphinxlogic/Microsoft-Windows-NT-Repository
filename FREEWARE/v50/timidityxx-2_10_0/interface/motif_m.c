/* 

    TiMidity -- Experimental MIDI to WAVE converter
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


    motif_mbx.c:  written by Arne Bergseth (Arne.Bergseth@dnv.com) 26/1-2000

    mailbox communication between motif interface and sound generator.
    This code is only for use on VMS, using the mailbox device.
    These routines has the same entry point names as motif_pipe.c,
    so this file must be linked instead.
    These routines use two mailbox devices for communication,
    MOTIF_MBX  from sound generator to motif interface, and 
    TIMIDITY_MBX  from motif interface to sound generator.

    Running Timidity with the Motif user interface on VMS is achived
    by giving command line option "-im".

    To run with the basic or VT100 command line interface use "-id" or "-iT".

    A special command line option  "-Y"  is used when starting 
    the subprocess to run the Motif user interface, this option should
    not be given by the user. 
    */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include <iodef.h>              /* needed for IO$_ functions */
#include <ssdef.h>              /* VMS system service condition codes */
#include <descrip.h>            /* VMS Descriptor functions */
#include <starlet.h>		/* VMS runtime library */

#include <string.h>
#include <sys/ioctl.h>
#include <climsgdef>
#include <perror>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include "timidity.h"
#include "controls.h"
#include "motif.h"

int pid;	               /* Pid for child process */
int status;

struct dsc$descriptor_s mbxname;        /* mailbox logical name descriptor. */

static	int	mbx_buflen = 512;
static	int	mbx_bufquo = 1056;
static	int	mi_chan;	/* mailbox input channel. */
static	int	mo_chan;	/* mailbox output channel. */
static	char	in_buffer[512];
static	char	out_buffer[512];

/* DATA VALIDITY CHECK */
#define INT_CODE 214
#define STRING_CODE 216

#undef DEBUGPIPE

/***********************************************************************/
/* MAILBOX COMUNICATION                                                */
/***********************************************************************/

int	tall;
unsigned long	mbx_efn = 20;
unsigned short	iosbr[4] = {0, 0, 0, 0};
unsigned short	mbx_iosb[4];

void mbx_error(char *st)
{
    fprintf(stderr,"CONNECTION PROBLEM WITH MOTIF PROCESS IN %s BECAUSE:%s\n",
	    st,
	    sys_errlist[errno]);
    exit(1);
}


/*****************************************
 *              INT                      *
 *****************************************/

/*  void mbx_int_write(int c) */
void m_pipe_int_write(int c)
{
    int len;
    int mbx_msglen;
    int code = INT_CODE;
    int func = IO$_WRITEVBLK + IO$M_NOW;

#ifdef DEBUGPIPE
    if (c == 0)
    { printf ("Write channel %d code %d value %d \n", mo_chan, code, c);
    }
#endif

    sprintf(&out_buffer[0],"  %d  %d ",  code, c);
    mbx_msglen = strlen(&out_buffer[0]);
    out_buffer[mbx_msglen]='\0';	/* Append a terminal 0 */
/*
 * Write to mailbox.
 */
	status = sys$qiow (0,		/* no event flag number */
                          mo_chan,	/* output channel */
			  func,		/* function write to buffer */
                          mbx_iosb,	/* io status block */
                          0,		/* ast routine */
                          0,		/* ast param */
                          &out_buffer[0],	/* Buffer (P1) */
                          mbx_msglen,	/* Buffer length (P2) */
			  0, 0, 0, 0);


        if (!(status & 1))
	mbx_error("MAILBOX_INT_WRITE");

        if (!(mbx_iosb[0] & 1))
	mbx_error("MAILBOX_INT_WRITE");
}

/*  void mbx_int_read(int *c) */
void m_pipe_int_read(int *c)
{
    int len;
    int code;
    if (iosbr[1] == 0)		/* Check if message already in buffer. */
    
/* Read from mailbox. */ 
      {	status = sys$qiow (0,    /* no event flag number */
                          mi_chan,      /* input channel */
                          IO$_READVBLK,	/* function */
                          iosbr,		/* io status block */
                          0,		/* ast routine */
                          0,		/* ast param */
                          &in_buffer[0],	/* Buffer (P1) */
                          mbx_buflen,  /* Buffer length (P2) */
			  0, 0, 0, 0);

        if (!(status & 1))
	mbx_error("MAILBOX_INT_READ");

        if (!(iosbr[0] & 1))
	mbx_error("MAILBOX_INT_READ");
      }
    else
        status = sys$clref (mbx_efn);

    len = iosbr[1];
    in_buffer[len]='\0';		/* Append a terminal 0 */
    sscanf (&in_buffer[0],"  %d  %d ", &code, &tall);
    iosbr[1] = 0;

#ifdef DEBUGPIPE
    if (code != INT_CODE) fprintf(stderr,"Bug alert on mbx read  %i\n",code);
    printf("Read channel %d code %d value %d status %d \n",
                 mi_chan,   code,   tall,    status);
#endif

    *c = tall ;
}



/*****************************************
 *              STRINGS                  *
 *****************************************/

/*void mbx_string_write(char *str)*/
void m_pipe_string_write(char *str)
{
    int len, slen;
    int mbx_msglen;
    int code = STRING_CODE;
    int func = IO$_WRITEVBLK + IO$M_NOW;

#ifdef DEBUGPIPE
    printf("Write channel %d code %d string %s \n",
		  mo_chan,   code,   str );
#endif

    slen = strlen(str);
    sprintf(&out_buffer[0],"  %d  %d  %s ", code, slen, str);
    mbx_msglen = strlen(&out_buffer[0]);
/*
 * Write to mailbox.
 */
	status = sys$qiow (0,		/* no event flag number */
                          mo_chan,	/* output channel */
			  func,		/* function write to buffer */
                          mbx_iosb,	/* io status block */
                          0,		/* ast routine */
                          0,		/* ast param */
                          &out_buffer[0],	/* Buffer (P1) */
                          mbx_msglen,	/* Buffer length (P2) */
			  0, 0, 0, 0);

        if (!(status & 1))
	mbx_error("MAILBOX_INT_WRITE");

        if (!(mbx_iosb[0] & 1))
	mbx_error("MAILBOX_INT_WRITE");
}

/* void mbx_string_read(char *str) */
void m_pipe_string_read(char *str)
{
    int len, slen, sind;
    int mbx_msglen;
    int code;

    if (iosbr[1] == 0)			/* Check read buffer. */

					/* Read from mailbox. */ 
      {	status = sys$qiow (0,		/* no event flag number */
                          mi_chan,      /* input channel */
                          IO$_READVBLK,	/* function */
                          iosbr,		/* io status block */
                          0,		/* ast routine */
                          0,		/* ast param */
                          &in_buffer[0],	/* Buffer (P1) */
                          mbx_buflen,	/* Buffer length (P2) */
			  0, 0, 0, 0);

        if (!(status & 1))
	mbx_error("MAILBOX_STRING_READ");

        if (!(iosbr[0] & 1))
	mbx_error("MAILBOX_STRING_READ");
      }
    else
        status = sys$clref (mbx_efn);

    len = iosbr[1];
    in_buffer[len]='\0';		/* Append a terminal 0 */
    sscanf (&in_buffer[0],"  %d  %d  ", &code, &slen );

#ifdef DEBUGPIPE
    if (code != STRING_CODE) fprintf(stderr,"Bug alert on mbx read  %i\n",code);
#endif

    sind = len - slen -1;
    strncpy (str, &in_buffer[sind], slen);
    str[slen]='\0';			/* Append a terminal 0 */

    iosbr[1] = 0;			/* Clear IOSB */

#ifdef DEBUGPIPE
    printf("Read mailbox channel %d code %d string %s status %d \n",
                        mi_chan,   code,   str,      status);
#endif
}

/*  int mbx_read_ready() */
int m_pipe_read_ready()
{
    int len;
    int num = 0;
    int status ;
    int func = IO$_READVBLK + IO$M_NOW;
/*
 * Do a non-Blocking read to see if any message pending.
 */
	status = sys$qiow (0,	/* event flag number */
                          mi_chan,      /* input channel */
                          func,		/* function */
                          iosbr,	/* io status block */
                          0,		/* ast routine */
                          0,		/* ast param */
                          &in_buffer[0],	/* Buffer (P1) */
                          mbx_buflen,	/* Buffer length (P2) */
			  0, 0, 0, 0) ;

        if (!(status & 1))
		mbx_error("MAILBOX_INT_READ");

        if (iosbr[0] == SS$_ENDOFFILE)
		return num;

        if (!(iosbr[0] & 1))
		mbx_error("MAILBOX_INT_READ");
        num = iosbr[1];
    return num; 
}

void mbx_read_async()
{
    int len;
    int status ;
static    int flags ;
    int func = IO$_READVBLK;

    if (iosbr[1] == 0)			/* Check if message in read buffer. */
/*
 * Start an asynchronous read so that we get control while Motif is running.
 */
      {	status = sys$qio (mbx_efn,	/* event flag number */
			mi_chan,	/* input channel */
			func,		/* function */
			iosbr,		/* io status block */
			0,		/* ast routine */
			0,		/* ast param */
			&in_buffer[0],	/* Buffer (P1) */
			mbx_buflen,	/* Buffer length (P2) */
			0, 0, 0, 0) ;

        if (!(status & 1))
		mbx_error("MAILBOX_ASY_READ");
      }
    else		/* Message already in buffer. */
      { status = sys$readef (mbx_efn, &flags);
      }
    return ; 
}

void mbx_open(char *program)
{
    int res, status;
    char *str_in = "TIMIDITY_MBX";
    char *str_out = "MOTIF_MBX";

#ifdef DEBUGPIPE
    printf ("Timidity parent - mbx_open called. \n");
#endif

  if (mi_chan == 0)		/* Open VMS mailbox device only once. */
  { 
/*
 *	Create descriptor for output mailbox name.
 */
        mbxname.dsc$w_length = strlen(str_in);
        mbxname.dsc$a_pointer = str_in;
        mbxname.dsc$b_dtype = DSC$K_DTYPE_T;
        mbxname.dsc$b_class = DSC$K_CLASS_S;
/*
 *      Create mailbox to read from Motif interface process.
 */
	status = sys$crembx( 0,
			    &mi_chan,
			     mbx_buflen,
			     mbx_bufquo,0,0,
			    &mbxname);
/*
 *	Create descriptor for output mailbox name.
 */
        mbxname.dsc$w_length = strlen(str_out);
        mbxname.dsc$a_pointer = str_out;
        mbxname.dsc$b_dtype = DSC$K_DTYPE_T;
        mbxname.dsc$b_class = DSC$K_CLASS_S;
/*
 *      Create mailbox to write to Motif interface process.
 */
        status = sys$crembx( 0,
                            &mo_chan,
                             mbx_buflen,
                             mbx_bufquo,0,0,
			    &mbxname);
/*
 *      Start a subprocess to run the Motif user interface.
 */  
#ifndef VMS
    pid = fork()    /*child*/
#else
    pid = vfork() ;   /*child*/
#endif

#ifdef DEBUGPIPE
    printf ("Timidity parent - starting process %d \n", pid);
#endif
    if (pid == 0 )   /*child*/
      {
#ifdef DEBUGPIPE
	printf ("Timidity parent - input mailbox  %s \n", str_in);
	printf ("Timidity parent - output mailbox %s \n", str_out);
#endif

        status = execl(program,
                       program, "-Y", NULL); 
	if (status == -1)
          { printf("Parent - Execl failed");
            exit(EXIT_FAILURE);
          }
        printf("Execl status %d \n", status);
      }
    else
      { if (pid < 0)
	    printf ("Parent - Child process failed\n");
#ifdef DEBUGPIPE
	else
	{   printf ("Parent - waiting for Child process\n");
	}
#endif
      }
#ifdef DEBUGPIPE
    printf ("Timidity parent - input channel %d \n", mi_chan);
    printf ("Timidity parent - output channel %d \n", mo_chan);
#endif
  }
/*
 * Also try to set process priority to avoid disruptions.
 * DECW processes running with priority 6 
 * If the process does not have privilege to change priority,
 * then the request is ignored. 
 */
  status = sys$setpri (0, 0, 6, 0);
}

int mbx_exists()
{
    int res, status;
    char *str_in  = "MOTIF_MBX";
    char *str_out = "TIMIDITY_MBX";

#ifdef DEBUGPIPE
    printf ("Timidity Motif interface input mailbox  %s \n", str_in);
    printf ("Timidity Motif interface output mailbox %s \n", str_out);
#endif

/*
 * Set default process priority to avoid disturbing Timidity computations.
 * If the process does not have privilege to change priority,
 * then the request is ignored. 
 */
  status = sys$setpri (0, 0, 4, 0);
/*
 *	Create descriptor for output mailbox name.
 */
        mbxname.dsc$w_length = strlen(str_in);
        mbxname.dsc$a_pointer = str_in;
        mbxname.dsc$b_dtype = DSC$K_DTYPE_T;
        mbxname.dsc$b_class = DSC$K_CLASS_S;

        status = sys$assign(&mbxname, &mi_chan, 0, 0, 0);
        if (!(status & 1))	return (status);
/*
 *	Create descriptor for output mailbox name.
 */
        mbxname.dsc$w_length = strlen(str_out);
        mbxname.dsc$a_pointer = str_out;
        mbxname.dsc$b_dtype = DSC$K_DTYPE_T;
        mbxname.dsc$b_class = DSC$K_CLASS_S;

        status = sys$assign(&mbxname, &mo_chan, 0, 0, 0);
        if (!(status & 1))
		mbx_error("MAILBOX_INT_WRITE");

#ifdef DEBUGPIPE
	printf ("Starting Motif interface\n");
#endif
	Launch_Motif_Process(mbx_efn);

					/* Won't come back from here */
	fprintf(stderr,"WARNING: come back from MOTIF\n");
	exit(0); 
}
