
/*
 * This file is part of the Seyon, Copyright (c) 1992-1993 by Muhammad M.
 * Saggaf. All rights reserved.
 *
 * See the file COPYING (1-COPYING) or the manual page seyon(1) for a full
 * statement of rights and permissions for this program.
*/

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

#if HAVE_TERMIOS
#include <termios.h>
#else
#if HAVE_TERMIO
#include <termio.h>
#endif
#endif

#include <X11/Intrinsic.h>

#include "SeDecl.h"

int
io_set_attr(fd, io)
     int             fd;

#if HAVE_TERMIOS
     struct termios *io;
{
  int             res;

  res = tcsetattr(fd, TCSADRAIN, io);
  if (res < 0)
    SePError("tcsetattr");

  return res;

#else
#if HAVE_TERMIO
     struct termio  *io;
{
  int             res;

  res = ioctl(fd, TCSETAW, io);
  if (res < 0)
    SePError("ioctl-set");

  return res;
#else
#if HAVE_SGTTYB
     struct sgttyb *io;
{
  int res;

  res = ioctl(fd, TIOCSETP, io);
  if (res < 0)
    SePError("ioctl-set");

  return res;
#endif
#endif
#endif
}

int
io_get_attr(fd, io)
     int             fd;

#if HAVE_TERMIOS
     struct termios *io;
{
  int             res;

  res = tcgetattr(fd, io);
  if (res < 0)
    SePError("tcgetattr");

  return res;

#else
#if HAVE_TERMIO
     struct termio  *io;
{
  int             res;

  res = ioctl(fd, TCGETA, io);
  if (res < 0)
    SePError("ioctl-get");

  return res;
#else
#if HAVE_SGTTYB
     struct sgttyb *io;
{
  int res;

  res = ioctl(fd, TIOCGETP, io);
  if (res < 0)
    SePError("ioctl-get");

  return res;
#endif
#endif
#endif
}

int
io_flush(fd)
     int             fd;
{
  int             res;

#if HAVE_TERMIOS
  res = tcflush(fd, TCIFLUSH);
  if (res < 0)
    SePError("tcflush");

#else
#if HAVE_TERMIO
  res = ioctl(fd, TCFLSH, 0);
  if (res < 0)
    SePError("ioctl-flush");
#else
#if HAVE_SGTTYB
  res = ioctl(fd, TIOCFLUSH, 0);
  if (res < 0)
    SePError("ioctl-flush");
#endif
#endif
#endif

  return res;
}

int
io_send_break(fd)
     int             fd;
{
  int             res;

#if HAVE_TERMIOS
  res = tcsendbreak(fd, 0);

#else
#if HAVE_TERMIO
  res = ioctl(fd, TCSBRK, 0);
#else
#if HAVE_SGTTYB
  res = ioctl(fd, TIOCSBRK);
  if (!res) {
    sleep(1);
    ioctl(fd, TIOCCBRK);
  }
#endif
#endif
#endif

  return res;
}

void
io_set_speed(io, speed)
#if HAVE_TERMIOS
     struct termios *io;
     speed_t         speed;
{
  cfsetospeed(io, speed);
  cfsetispeed(io, speed);

#else
#if HAVE_TERMIO
     struct termio  *io;
     speed_t         speed;
{
  io->c_cflag &= ~CBAUD;
  io->c_cflag |= speed;
#else
#if HAVE_SGTTYB
     struct sgttyb *io;
     speed_t speed;
{
  io->sg_ispeed = io->sg_ospeed = speed;
#endif
#endif
#endif
}

speed_t
io_get_speed(io)
#if HAVE_TERMIOS
     struct termios *io;
{
  return cfgetospeed(io);

#else
#if HAVE_TERMIO
     struct termio  *io;
{
  return io->c_cflag & CBAUD;
#else
#if HAVE_SGTTYB
     struct sgttyb *io;
{
  return io->sg_ispeed;
#endif
#endif
#endif
}

int
IoGetModemStat(fd)
	 int fd;
{
  int rawStat, retStat, res;

#if HAVE_MODEM_CONTROL
#ifndef HPUX
  res = ioctl(fd, TIOCMGET, &rawStat);
#else
  res = ioctl(fd, MCGETA, &rawStat);
#endif
  if (res < 0) {
    SePError("ioctl-getmdm");
	return -1;
  }
#endif

  retStat = 0;

#if HAVE_MODEM_CONTROL
#ifndef HPUX
#ifdef TIOCM_CAR
  if (rawStat & TIOCM_CAR) retStat |= MDM_DCD;
#endif
#ifdef TIOCM_DTR
  if (rawStat & TIOCM_DTR) retStat |= MDM_DTR;
#endif
#ifdef TIOCM_DSR
  if (rawStat & TIOCM_DSR) retStat |= MDM_DSR;
#endif
#ifdef TIOCM_RTS
  if (rawStat & TIOCM_RTS) retStat |= MDM_RTS;
#endif
#ifdef TIOCM_CTS
  if (rawStat & TIOCM_CTS) retStat |= MDM_CTS;
#endif
#ifdef TIOCM_RNG
  if (rawStat & TIOCM_RNG) retStat |= MDM_RNG;
#endif
#else /* HPUX */
  /* Note: I'm note sure about the symbol names of HPUX. I used the same 
     symbols as those used by kermit, but even there there is a comment
	 that the author is not sure about the symbol names. Someone who has
	 HPUX please let me know */
#ifdef MDCD
  if (rawStat & MDCD) retStat |= MDM_DCD;
#endif
#ifdef MDTR
  if (rawStat & MDTR) retStat |= MDM_DTR;
#endif
#ifdef MDSR
  if (rawStat & MDSR) retStat |= MDM_DSR;
#endif
#ifdef MRTS
  if (rawStat & MRTS) retStat |= MDM_RTS;
#endif
#ifdef MCTS
  if (rawStat & MCTS) retStat |= MDM_CTS;
#endif
#ifdef MRNG
  if (rawStat & MRNG) retStat |= MDM_RNG;
#endif
#endif /* HPUX */
#endif /* HAVE_MODEM_CONTROL */
  
  return retStat;
}

