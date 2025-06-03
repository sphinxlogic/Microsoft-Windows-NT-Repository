/*
 ===========================================================================
 =                                                                         =
 =  (C) Copyright 1991-1993 The Trustees of Indiana University             =
 =                                                                         =
 =  Permission to use, copy, modify, and distribute this program for       =
 =  non-commercial use and without fee is hereby granted, provided that    =
 =  this copyright and permission notice appear on all copies and          =
 =  supporting documentation, the name of Indiana University not be used   =
 =  in advertising or publicity pertaining to distribution of the program  =
 =  without specific prior permission, and notice be given in supporting   =
 =  documentation that copying and distribution is by permission of        =
 =  Indiana University.                                                    =
 =                                                                         =
 =  Indiana University makes no representations about the suitability of   =
 =  this software for any purpose. It is provided "as is" without express  =
 =  or implied warranty.                                                   =
 =                                                                         =
 ===========================================================================
 =                                                                         =
 = File:                                                                   =
 =   UCX_IOCTL.C                                                           =
 =                                                                         =
 = Synopsis:                                                               =
 =   This file contains a somewhat function IOCTL function which is        =
 =   not supplied with the UCX library.                                    =
 =                                                                         =
 = Authors:                                                                =
 =   Jacob Levanon & Larry Hughes                                          =
 =   Indiana University                                                    =
 =   University Computing Services, Network Applications                   =
 =                                                                         =
 ===========================================================================
*/

/* ======================================================================== */
/* Includes */
/* ======================================================================== */
#include "iufingerd.h"

#include <stdio.h>
#include <iodef.h>
#include <ucx$inetdef.h>

/* ======================================================================== */
/* Defines */
/* ======================================================================== */
#define IOCPARM_MASK 0x7f
#define IOC_VOID     (int)0x20000000
#define IOC_OUT      (int)0x40000000
#define IOC_IN       (int)0x80000000
#define IOC_INOUT    (int)(IOC_IN|IOC_OUT)
#define _IO(x,y)     (int)(IOC_VOID|('x'<<8)|y)
#define _IOR(x,y,t)  (int)(IOC_OUT|((sizeof(t)&IOCPARM_MASK<<16)|('x'<<8)|y)
#define _IOW(x,y,t)  (int)(IOC_IN|((sizeof(t)&IOCPARM_MASK<<16)|('x'<<8)|y)
#define _IOWR(x,y,t) (int)(IOC_INOUT|((sizeof(t)&IOCPARM_MASK<<16)|('x'<<8)|y)

/* ======================================================================== */
/* IOCTL */
/* ======================================================================== */
int ioctl(int sock, int request, char *argp)
{
  int event_flag;
  int channel;
  unsigned short function;
  struct IOSB iosb;
  char *p5, *p6;
  struct commands
  {
    int command;
    char *address;
  } ioctl_commands;
  struct 
  {
    unsigned short length;
    unsigned short options;
    struct commands *address;
  } ioctl_desc;
  int status;
  int retval = -1;

  if ((channel = vaxc$get_sdc(sock)) == 0)
  {
    errno = EBADF;
    return(retval);
  }

  status = lib$get_ef(&event_flag);
  if (VmsError(status))
  { 
    SystemLog("error: lib$get_ef: %s", VmsMessage(status));
    event_flag = 0;
  }

  ioctl_desc.options = UCX$C_IOCTL;
  ioctl_desc.length  = sizeof(struct commands);
  ioctl_desc.address = &ioctl_commands;
  if (request & IOC_OUT)
  {
    function = IO$_SENSEMODE;
    p5 = 0;
    (struct it2 *)p6 = &ioctl_desc;
  }
  else
  {
    function = IO$_SETMODE;
    (struct it2 *)p5 = &ioctl_desc;
  }

  ioctl_commands.command = request;
  ioctl_commands.address = argp;

  status = sys$qiow(event_flag, channel, function , &iosb, 0, 0,
                    0, 0, 0, 0, p5, p6);

  if (VmsError(status))
  {
    SystemLog("error: ioctl: %s", VmsMessage(status));
    errno = status;
  }
  else if (VmsError(iosb.status))
  {
    SystemLog("error: ioctl iosb: %s", VmsMessage(iosb.status));
    errno = iosb.status;
  }
  else
    retval = 0;

  (void) lib$free_ef(&event_flag);
  return(retval);
}
