#ifndef _VMS_FD
#define _VMS_FD

typedef struct fd_set
{
  int fds_bits[1];
} fd_set;

#define FD_SETSIZE          (sizeof (fd_set) * 8)
#define FD_SET(f,s)         ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)         ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)       ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)          ((s)->fds_bits[0] = 0)

#endif /* _VMS_FD */
