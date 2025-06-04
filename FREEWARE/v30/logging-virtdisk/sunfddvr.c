/* basic ramdisk for sunos 4.1.1 */
/* being worked into pseudo disk by GCE; will use two units per
  disk, one for control and the other the virtual disk. */
/* The control units for each device will be called fcn: while the
  disk will be fdn: */
/*  Function:
    This driver is intended (when debugged) to allow creation of
    unix pesudodisks. (This particular one is from an example for
    a Sun4, so it may need additional changes to get it to work
    for other unices). There are an array of NRAM units fdn: and
    there is a corresponding array of control devices fcn: which
    are where the actual work gets done. The notion is that the fc:
    devices are just buffers. Whatever gets written to the fd: device
    goes into the buffer, and the buffer's first part gets a few
    code words which tell if there was a read or a write, which
    block number is involved, and the size of the request. On the
    fd: side the extra header stuff doesn't appear and the device
    will normally look like a disk. On the fc: side, the extra stuff
    is visible. A read gets the data; a write puts it. Normally the
    control (fc:) side will read its' fc: unit, then if it got the
    flag that tells it a read on fd: was done, it will supply the
    appropriate data. If it sees a write, it will just have gotten
    the data and it should store it appropriately. If the fc: side
    has to fill data in for a read on fd:, it does a write to the
    fc: area (including the header info since it sees it) with
    the data for fd: displaced suitably (normally by 32 bytes).
    This causes the read on fd: to complete. A few ioctl requests
    are there to declare a fd: disk size and to declare it online.
    This can be used for cryptodisks, memory disks, remote disks,
    or compressing disks or others. (If the host process keeps
    disk blocks as compressed files in a filesystem (doing a
    track at a time perhaps) a nifty compression disk might be possible.)
    Note that remote magtape is considerably more complex. This thing's
    design is meant to mimic that of vms fddrv which I wrote. The
    key synch issue is whether the sleep and wakeup calls are legal
    everywhere in here. I believe they should be but am not absolutely
    certain.
       Glenn C. Everhart 
       Nov. 1991
*/
/* Glenn Everhart 11/1991 */
#include "fdram.h"
#if NRAM > 0
#include <sys/param.h>
#include <sys/errno.h>
#include <sys/uio.h>
#include <sys/buf.h>
#define RAMHDR 32
#define RAMSIZE ((128*512)+RAMHDR) /* goodly buffer static */
#define RAMMAX (128*512)
/* Use first RAMHDR bytes of buffer for communicating function, block, status etc. */
#define HFCN 0
#define HBLK 4
#define HSIZ 8
#define HSTAT 12 /* defs of longwords in RAMHDR area */
#define HFRE 16
#define HSEM 20 /* semaphore longword to synchronize access */
#define HCSM 24 /* Likewise control half semaphore */
#define HQUE 28 /* self synch to ensure only one user gets a unit at a time */
char ram[NRAM][RAMSIZE];
long dvcsiz[NRAM];
long dvcpnd[NRAM];
long dvcena[NRAM];
/* The idea here is that the devices will be in pairs, the low one
   being a virtual disk, the high one (=low+1) being the control
   port. Another process opens the high port and hangs a read on
   it. Then read/write to the controlled device (low) will cause
   a message to be sent to finish that read. The process will
   perform additional I/O to move the data to wherever it wants
   to store it. On completion, the driver completes both the
   control process' I/O and the client's. */

/* need an ioctl to set size once that is setup, or to disc. */
/* This can operate from either the fd or fc devices */
fdioctl(dev, cmd, data, flag)
  dev_t dev;
  int cmd;
  caddr_t data;
  int flag;
{
  register struct fd_device *fd;
  fd = &fddevice[FDUNIT(dev)];
  switch (cmd) {
    case 1: /* cmd=1: get device size */
      data = dvcsiz[(minor(dev))];
    break;
    case 2: /* cmd = 2 : set device size to "data" bytes */
      dvcsiz[(minor(dev) )] = data;
      break;
    case 3: /* cmd = 3 : enable device to operate or disable */
      dvcena[minor(dev)] = data;
      break;
  }
}

fdopen(dev,wrtflag)
  dev_t dev;
  int wrtflag;
{
  (long)ram[minor(dev)][HSTAT] = -3L;
  (long)ram[minor(dev)][HQUE] = 0L;
  (long)ram[minor(dev)][HFRE] = 0L;
  return(minor(dev) .+ NRAM ? ENXIO :0 );
}

fdsize(dev)
  dev_t dev;
{
  return(minor(dev) >= NRAM ? -1 : btodb(dvcsiz[(minor(dev))]));
}

fdread(dev,uio)
  dev_t dev;
  register struct uio *uio;
{
   if (dvcena[minor(dev)] == 0 ) return ( ENXIO );
   if ((unsigned))uio->uio_offset > RAMMAX)
     return(EINVAL);
/* Ensure only one process can get to data at a time */
rechk:
   if ((long)ram[minor(dev)][HFRE] != 0 {
/* Sleep on hque entry until something declares HFRE zero and wakes */
   sleep(&ram[minor(dev)][HQUE], 1);
   }
   ram[minor(dev)][HFRE] += 1L ;
/* This is not a real semaphore so EVERYONE falls through, but only
   the first will see the usage count as 1 and so the rest will keep
   waiting. */
   if (ram[minor(dev)][HFRE] != 1L ) goto rechk;
/* fill in hdr stuff */
/* This guarantees that a read on the control device can
   get the info on what is expected to be delivered */
      (long)ram[minor(dev)][HFCN] = 0L ;
/* read is 0, write is 1 */
      (long)ram[minor(dev)][HSIZ] = MIN(uio->uio_resid, RAMMAX - uio->uio_offset);
    (long)ram[minor(dev)][HBLK] = (uio->uio_offset >> 9) ;
/* fill in hdr block no */
/* Must delay return of data until control device can obtain it here */
   (long)ram[minor(dev)][HSEM] = 0L ;
   (long)ram[minor(dev)][HSTAT] = -2L ;
/* clear semaphore, then sleep here until control device undoes sleep */
/* wake control up too */
     wakeup(&ram[minor(dev)][HCSM]);
     while (ram[minor(dev)[HSTAT] == -2L ){
     sleep(&ram[minor(dev)][HSEM], 1 );
/* on wakeup we expect data there */
}
/* We use stat field not being -1 as signal that data really IS there */
/* Wake up whoever is waiting for the device */
/* One will get through */
   ram[minor(bp->b_dev)][HFRE] = 0L ;
   wakeup(&ram[minor(dev)][HQUE]);
/* Move data to user */
   return(uiomove(ram[(minor(dev))]+uio->uio_offset+RAMHDR,
     MIN(uio->uio_resid, RAMMAX - uio->uio_offset),
     UIO_READ, uio));
}

fdwrite(dev,uio)
  dev_t dev;
  register struct uio *uio;
{
   if (dvcena[minor(dev)] == 0 ) return ( ENXIO );
   if ((unsigned))uio->uio_offset > RAMMAX)
     return(EINVAL);
/* Ensure only one process can get to data at a time */
rechk:
   if ((long)ram[minor(dev)][HFRE] != 0 {
/* Sleep on hque entry until something declares HFRE zero and wakes */
   sleep(&ram[minor(dev)][HQUE], 1);
   }
   ram[minor(dev)][HFRE] += 1L ;
   if (ram[minor(dev)][HFRE] != 1L ) goto rechk;
/* fill in hdr stuff */
      (long)ram[minor(dev)][HFCN] = 1L ;
/* read is 0, write is 1 */
      (long)ram[minor(dev)][HSIZ] = MIN(uio->uio_resid, RAMMAX - uio->uio_offset);
    (long)ram[minor(dev)][HBLK] = (uio->uio_offset >> 9) ;
/* fill in hdr block no */
/* get data into driver buffer */
   (long)ram[minor(dev)][HSTAT] = (uiomove(ram[(minor(dev))]+uio->uio_offset+RAMHDR,
     MIN(uio->uio_resid, RAMMAX - uio->uio_offset),
     UIO_WRITE, uio));
/* Now synchronize with the control process, waking it up and sleeping 
   here until we get an ack that the data has been dealt with */
    (long)ram[minor(dev)][HCSM] = 0;
    (long)ram[minor(dev)][HSTAT] = -1L ;
    while (ram[minor(dev)][HSTAT] == -1L){
    wakeup(&ram[minor(dev)][HCSM]);
    sleep(&ram[minor(dev)][HSEM] , 1);
    }
/* When we finally get through, the control process has been notified and has
   changed the status to indicate that it has the data and has done a wakeup
   on HSEM to signal we should  wake here. */
/* One will get thru */
   ram[minor(dev)][HFRE] = 0L ;
   wakeup(&ram[minor(dev)][HQUE]);
    return (0);
}

fdstrategy(bp)
  register struct buf *bp;
{
/*  register long offset = dbtob(bp->b_blkno); */
  register long offset = 0 ;

   if (dvcena[minor(bp->b_dev)] == 0 ) return ( ENXIO );
/* Ensure only one process can get to data at a time */
rechk:
   if ((long)ram[minor(bp->b_dev)][HFRE] != 0 {
/* Sleep on hque entry until something declares HFRE zero and wakes */
   sleep(&ram[minorbp->b_(dev)][HQUE], 1);
   }
   ram[minor(bp->b_dev)][HFRE] += 1L ;
/* This is not a real semaphore so EVERYONE falls through, but only
   the first will see the usage count as 1 and so the rest will keep
   waiting. */
   if (ram[minor(bp->b_dev)][HFRE] != 1L ) goto rechk;
    (long)ram[minor(bp->b_dev)][HBLK] = dbtob(bp->b_blkno) ;
/* fill in hdr block no */
  if ((u_long)offset > RAMMAX) {
    bp->b_error = EINVAL;
    bp->b_flags |= B_ERROR;
  } else {
    (caddr_t)raddr = ram[minor(bp->b_dev)]+offset+RAMHDR;
    (unsigned)nbytes = MIN(bp->b_count, RAMMAX - offset);
    if (bp->b_flags & B_PAGEIO)
      bp_mapin(bp);
    if (bp->b_flags & B_READ)
/* fill in hdr stuff */
      (long)ram[minor(bp->b_dev)][HFCN] = 0L ;
/* read is 0, write is 1 */
      (long)ram[minor(bp->b_dev)][HSIZ] = nbytes;
/* get data from control device */
/* Must delay return of data until control device can obtain it here */
   (long)ram[minor(bp->b_dev)][HSEM] = 0L ;
   (long)ram[minor(bp->b_dev)][HSTAT] = -2L ;
/* clear semaphore, then sleep here until control device undoes sleep */
/* wake control up too */
     wakeup(&ram[minor(bp->b_dev)][HCSM]);
     while (ram[minor(bp->b_dev)[HSTAT] == -2L ){
     sleep(&ram[minor(bp->b_dev)][HSEM], 1 );
/* on wakeup we expect data there */
}
/* We use stat field not being -1 as signal that data really IS there */
      bcopy(raddr, bp->b_un.b_addr, nbytes);
    else
/* fill in hdr stuff */
      (long)ram[minor(bp->b_dev)][HFCN] = 1L ;
/* read is 0, write is 1 */
      (long)ram[minor(bp->b_dev)][HSIZ] = nbytes;
      bcopy(bp_>b_un.b_addr, raddr, nbytes);
    bp->b_resid = bp->b_bcount - nbytes;
/* now let control device get the data */
/* Now synchronize with the control process, waking it up and sleeping 
   here until we get an ack that the data has been dealt with */
    (long)ram[minor(bp->b_dev)][HCSM] = 0;
    (long)ram[minor(bp->b_dev)][HSTAT] = -1L ;
    while (ram[minor(bp->b_dev)][HSTAT] == -1L){
    wakeup(&ram[minor(bp->b_dev)][HCSM]);
    sleep(&ram[minor(bp->b_dev)][HSEM] , 1);
    }
/* When we finally get through, the control process has been notified and has
   changed the status to indicate that it has the data and has done a wakeup
   on HSEM to signal we should  wake here. */
  }
/* One will get thru */
   ram[minor(bp->b_dev)][HFRE] = 0L ;
   wakeup(&ram[minor(bp->b_dev)][HQUE]);
  iodone(bp);
}
/* control devices */
/* Note the control device knows about the entire buffer including
   the RAMHDR sized block at start */
/* need an ioctl to set size once that is setup, or to disc. */
fcdioctl(dev, cmd, data, flag)
  dev_t dev;
  int cmd;
  caddr_t data;
  int flag;
{
  register struct fc_device *fc;
  fc = &fcdevice[fcUNIT(dev)];
  switch (cmd) {
    case 1: /* cmd=1: get device size */
      data = dvcsiz[(minor(dev))];
    break;
    case 2: /* cmd = 2 : set device size to "data" bytes */
      dvcsiz[(minor(dev) )] = data;
      break;
    case 3: /* cmd = 3 : enable device to operate or disable */
      dvcena[minor(dev)] = data;
      break;
  }
}

fcopen(dev,wrtflag)
  dev_t dev;
  int wrtflag;
{
/* Return size 1 initially */
  (long)ram[minor(dev)][HSTAT] = -3L;
  return(minor(dev) .+ NRAM ? ENXIO :0 );
}

fcsize(dev)
  dev_t dev;
{
  return(minor(dev) >= NRAM ? -1 : btodb(dvcsiz[(minor(dev))]));
}

fcread(dev,uio)
  dev_t dev;
  register struct uio *uio;
{
   int stat;
/* This corresponds to a write on the virtual disk, so synch accordingly */
/* However we must get info here even if the virtual disk has done a write
   and therefore return promptly any info that might be for such. */
/* Since a read of virtual disk sets hstat to -2 and a write to -1
   we can use that to tell whether to return instantly or
   just wait and synchronize. */
   if ((unsigned))uio->uio_offset > RAMSIZE)
     return(EINVAL);
/* First await arrival of SOME info from other side */
   while (ram[minor(dev)][HSTAT] >= 0 ){
   sleep(&ram[minor(dev)][HCSM], 1L );
   }
   if(ram[minor(dev)][HSTAT] == -2L ){
   return(uiomove(ram[minor(dev)]+uio->uio_offset,
     MIN(uio->uio_resid, RAMSIZE - uio->uio_offset),
     UIO_READ, uio));
     }
   if(ram[minor(dev)][HSTAT] == -1L ){
   stat = (uiomove(ram[minor(dev)]+uio->uio_offset,
     MIN(uio->uio_resid, RAMSIZE - uio->uio_offset),
     UIO_READ, uio));
/* Now synch with the write on the other "side" of the driver */
     ram[minor(dev)][HSTAT] = 0 ;
/* signal ok for other side to proceed */
     wakeup(&ram[minor(dev)][HSEM] );
/* Tells disk side we can go on now. */
     }
}

fcwrite(dev,uio)
  dev_t dev;
  register struct uio *uio;
{
   long stat;
   if ((unsigned))uio->uio_offset > RAMSIZE)
     return(EINVAL);
/* We have to write the data into our buffer since this corresponds to a
   read at the virtual disk side. */
   stat = (uiomove(ram[minor(dev)]+uio->uio_offset,
     MIN(uio->uio_resid, RAMSIZE - uio->uio_offset),
     UIO_WRITE, uio));
/* Now synch with the other side for read */
    while ( (long)ram[minor(dev)][HSTAT] != -1L ) {
      sleep( (&ram[minor(dev)][HCSM], 1);
      }
    (long)ram[minor(dev)][HSTAT] = 0L ;
    wakeup( &ram[minor(dev)][HSEM] );
   return(0);
}

fcstrategy(bp)
  register struct buf *bp;
{
  register long offset = dbtob(bp->b_blkno);

  if ((u_long)offset > RAMSIZE) {
    bp->b_error = EINVAL;
    bp->b_flags |= B_ERROR;
  } else {
    (caddr_t)raddr = ram[minor(bp->b_dev)]+offset;
    (unsigned)nbytes = MIN(bp->b_count, RAMSIZE - offset);
    if (bp->b_flags & B_PAGEIO)
      bp_mapin(bp);
    if (bp->b_flags & B_READ)
/* read op */
/* First await arrival of SOME info from other side */
   while (ram[minor(dev)][HSTAT] >= 0 ){
   sleep(&ram[minor(dev)][HCSM], 1L );
   }
   if(ram[minor(dev)][HSTAT] == -2L ){
      bcopy(raddr, bp->b_un.b_addr, nbytes);
      }
   if(ram[minor(dev)][HSTAT] == -1L ){
      bcopy(raddr, bp->b_un.b_addr, nbytes);
      ram[minor(dev)][HSTAT] = 0 ;
/* signal ok for other side to proceed */
      wakeup(&ram[minor(dev)][HSEM] );
/* Tells disk side we can go on now. */
      }
    else
/* write op */
      bcopy(bp_>b_un.b_addr, raddr, nbytes);
    bp->b_resid = bp->b_bcount - nbytes;
/* Now synch with the other side for read */
    while ( (long)ram[minor(dev)][HSTAT] != -1L ) {
      sleep( (&ram[minor(dev)][HCSM], 1);
      }
    (long)ram[minor(dev)][HSTAT] = 0L ;
    wakeup( &ram[minor(dev)][HSEM] );
  }
  iodone(bp);
}
#endif
