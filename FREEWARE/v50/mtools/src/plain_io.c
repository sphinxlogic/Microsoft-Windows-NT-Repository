/*
 * Io to a plain file or device
 *
 * written by:
 *
 * Alain L. Knaff			
 * Alain.Knaff@poboxes.com
 *
 */

#include "sysincludes.h"
#include "stream.h"
#include "mtools.h"
#include "msdos.h"
#include "plain_io.h"
#include "patchlevel.h"
#include "scsi.h"
#include "partition.h"

#ifdef VMS
#include <descrip.h>
#include <starlet.h>
#include <iodef.h>
#include <dvidef.h>
#include <mntdef.h>
#include <devdef.h>

typedef struct
{
    unsigned short  Length;
    unsigned short  Code;
    void	    *Address;
    unsigned short  *RetAddress;
} ITEM_S;

static struct dsc$descriptor_s Name;
static char	    DevName[32];
static long	    DevChar = 0;
static long long    MntFlags = MNT$M_FOREIGN|MNT$M_NOASSIST;
static long	    VMS_cylinders, VMS_tracks, VMS_sectors;

static ITEM_S	DiskInfo[] =
    {
    {sizeof(long), DVI$_DEVCHAR, &DevChar, NULL},
    {sizeof(long), DVI$_CYLINDERS, &VMS_cylinders, NULL},
    {sizeof(long), DVI$_TRACKS, &VMS_tracks, NULL},
    {sizeof(long), DVI$_SECTORS, &VMS_sectors, NULL},
    {0, 0, NULL, NULL}};

static ITEM_S	MntInfo[] =
    {{0, MNT$_DEVNAM, NULL, NULL},
     {sizeof(long long), MNT$_FLAGS, &MntFlags, NULL},
     {0, 0, NULL, NULL}};

#endif

typedef struct SimpleFile_t {
	Class_t *Class;
	int refs;
	Stream_t *Next;
	Stream_t *Buffer;
	struct stat stat;
	int fd;
	int offset;
	int lastwhere;
	int seekable;
	int privileged;
} SimpleFile_t;

static Class_t VMS_SimpleFileClass;

/*
 * Create an advisory lock on the device to prevent concurrent writes.
 * Uses either lockf, flock, or fcntl locking methods.  See the Makefile
 * and the Configure files for how to specify the proper method.
 */

int lock_dev(int fd, int mode, struct device *dev)
{
#if (defined(HAVE_FLOCK) && defined (LOCK_EX) && defined(LOCK_NB))
	/**/
#else /* FLOCK */

#if (defined(HAVE_LOCKF) && defined(F_TLOCK))
	/**/
#else /* LOCKF */

#if (defined(F_SETLK) && defined(F_WRLCK))
	struct flock flk;

#endif /* FCNTL */
#endif /* LOCKF */
#endif /* FLOCK */

	if(dev && dev->nolock)
		return 0;

    return 0;


#if (defined(HAVE_FLOCK) && defined (LOCK_EX) && defined(LOCK_NB))
	if (flock(fd, (mode ? LOCK_EX : LOCK_SH)|LOCK_NB) < 0)
#else /* FLOCK */

#if (defined(HAVE_LOCKF) && defined(F_TLOCK))
	if (mode && lockf(fd, F_TLOCK, 0) < 0)
#else /* LOCKF */

#if (defined(F_SETLK) && defined(F_WRLCK))
	flk.l_type = mode ? F_WRLCK : F_RDLCK;
	flk.l_whence = 0;
	flk.l_start = 0L;
	flk.l_len = 0L;

	if (fcntl(fd, F_SETLK, &flk) < 0)
#endif /* FCNTL */
#endif /* LOCKF */
#endif /* FLOCK */
	{
		if(errno == EINVAL
#ifdef  EOPNOTSUPP 
		   || errno ==  EOPNOTSUPP
#endif
		  )
			return 0;
		else
			return 1;
	}
	return 0;
}

typedef int (*iofn) (int, char *, int);

static int file_io(Stream_t *Stream, char *buf, int where, int len,
		   iofn io)
{
    DeclareThis(SimpleFile_t);
    int ret;

    where += This->offset;

#ifdef VMS
    if (This->Class == &VMS_SimpleFileClass)
    {
	int	Block, Func;
	long	Result;

	Block = where/512;

	if (io == (iofn)write)
	    Func = IO$_WRITELBLK;
	else
	    Func = IO$_READLBLK;

	Result = sys$qiow(0, This->fd, Func, NULL, NULL, NULL, buf, len, Block,
			    NULL, NULL, NULL, NULL, NULL);
	if (Result & 1)
	    ret = len;
	else
	    ret = -1;
    }
    else
#endif
    {
	if (This->seekable && where != This->lastwhere)
	{
	    if (lseek(This->fd, where-512, SEEK_SET) < 0)
	    {
		perror("seek");
		This->lastwhere = -1;
		return -1;
	    }
	}
	ret = io(This->fd, buf, len);
    }

    if ( ret == -1 ){
	    perror("read");
	    This->lastwhere = -1;
	    return -1;
    }
    This->lastwhere = where + ret;

    return ret;
}
	


static int file_read(Stream_t *Stream, char *buf, off_t where, size_t len)
{	
	return file_io(Stream, buf, where, len, (iofn) read);
}

static int file_write(Stream_t *Stream, char *buf, off_t where, size_t len)
{
	return file_io(Stream, buf, where, len, (iofn) write);
}

static int file_flush(Stream_t *Stream)
{
#if 0
	DeclareThis(SimpleFile_t);

	return fsync(This->fd);
#endif
	return 0;
}

static int file_free(Stream_t *Stream)
{
	DeclareThis(SimpleFile_t);

#ifdef VMS
	if (This->Class == &VMS_SimpleFileClass)
	{
	    long    Result;

	    Result = sys$dismou(&Name);
	}
	else
#endif

	if (This->fd > 2)
		return close(This->fd);
	else
		return 0;
}

static int file_geom(Stream_t *Stream, struct device *dev, 
		     struct device *orig_dev,
		     int media, struct bootsector *boot)
{
	int ret;
	DeclareThis(SimpleFile_t);
	size_t tot_sectors;
	int BootP, Infp0, InfpX, InfTm;
	int sectors, j;
	unsigned char sum;
	int sect_per_track;

	dev->ssize = 2; /* allow for init_geom to change it */
	dev->use_2m = 0x80; /* disable 2m mode to begin */

	if(media == 0xf0 || media >= 0x100){		
		dev->heads = CHAR(nheads);
		dev->sectors = CHAR(nsect);
		tot_sectors = DWORD(bigsect);
		SET_INT(tot_sectors, WORD(psect));
		sect_per_track = dev->heads * dev->sectors;
		tot_sectors += sect_per_track - 1; /* round size up */
		dev->tracks = tot_sectors / sect_per_track;

		BootP = WORD(ext.old.BootP);
		Infp0 = WORD(ext.old.Infp0);
		InfpX = WORD(ext.old.InfpX);
		InfTm = WORD(ext.old.InfTm);
		
		if (boot->descr >= 0xf0 &&
		    boot->ext.old.dos4 == 0x29 &&
		    WORD(fatlen) &&
		    strncmp( boot->banner,"2M", 2 ) == 0 &&
		    BootP < 512 && Infp0 < 512 && InfpX < 512 && InfTm < 512 &&
		    BootP >= InfTm + 2 && InfTm >= InfpX && InfpX >= Infp0 && 
		    Infp0 >= 76 ){
			for (sum=0, j=63; j < BootP; j++) 
				sum += boot->jump[j];/* checksum */
			dev->ssize = boot->jump[InfTm];
			if (!sum && dev->ssize <= 7){
				dev->use_2m = 0xff;
				dev->ssize |= 0x80; /* is set */
			}
		}
	} else if (media >= 0xf8){
		media &= 3;
		dev->heads = old_dos[media].heads;
		dev->tracks = old_dos[media].tracks;
		dev->sectors = old_dos[media].sectors;
		dev->ssize = 0x80;
		dev->use_2m = ~1;
	} else {
		fprintf(stderr,"Unknown media type\n");
		exit(1);
	}

	sectors = dev->sectors;
	dev->sectors = dev->sectors * WORD(secsiz) / 512;

#ifdef JPD
	printf("file_geom:media=%0X=>cyl=%d,heads=%d,sects=%d,ssize=%d,use2m=%X\n",
	       media, dev->tracks, dev->heads, dev->sectors, dev->ssize,
	       dev->use_2m);
#endif
	ret = init_geom(This->fd,dev, orig_dev, &This->stat);
	dev->sectors = sectors;
#ifdef JPD
	printf("f_geom: after init_geom(), sects=%d\n", dev->sectors);
#endif
	return ret;
}


static int file_data(Stream_t *Stream, long *date, size_t *size,
		     int *type, int *address)
{
	DeclareThis(SimpleFile_t);

	if(date)
		*date = This->stat.st_mtime;
	if(size)
		*size = This->stat.st_size;
	if(type)
		*type = S_ISDIR(This->stat.st_mode);
	if(address)
		*address = 0;
	return 0;
}

/* ZIP or other scsi device on Solaris or SunOS system.
   Since Sun won't accept a non-Sun label on a scsi disk, we must
   bypass Sun's disk interface and use low-level SCSI commands to read
   or write the ZIP drive.  We thus replace the file_read and file_write
   routines with our own scsi_read and scsi_write routines, that use the
   uscsi ioctl interface.  By James Dugal, jpd@usl.edu, 11-96.  Tested
   under Solaris 2.5 and SunOS 4.3.1_u1 using GCC.

   Note: the mtools.conf entry for a ZIP drive would look like this:
(solaris) drive C: file="/dev/rdsk/c0t5d0s2" partition=4  FAT=16 nodelay  exclusive scsi=&
(sunos) drive C: file="/dev/rsd5c" partition=4  FAT=16 nodelay  exclusive scsi=1

   Note 2: Sol 2.5 wants mtools to be suid-root, to use the ioctl.  SunOS is
   happy if we just have access to the device, so making mtools sgid to a
   group called, say, "ziprw" which has rw permission on /dev/rsd5c, is fine.
 */

#define MAXBLKSPERCMD 255

static void scsi_init(int);
static scsi_sector_size=512;

static void scsi_init(int fd)
{
   unsigned char cdb[6],buf[8];

   memset(cdb, 0, sizeof cdb);
   memset(buf,0, sizeof(buf));
   cdb[0]=SCSI_READ_CAPACITY;
   if (scsi_cmd(fd, (unsigned char *)cdb, 
		sizeof(cdb), SCSI_IO_READ, buf, sizeof(buf))==0)
   {
       scsi_sector_size=
	       ((unsigned)buf[5]<<16)|((unsigned)buf[6]<<8)|(unsigned)buf[7];
       if (scsi_sector_size != 512)
	   fprintf(stderr,"  (scsi_sector_size=%d)\n",scsi_sector_size);
   }
}

int scsi_io(Stream_t *Stream, char *buf,  off_t where, size_t len, int rwcmd)
{
	unsigned int firstblock, nsect;
	int clen,r,max,offset;
	unsigned char cdb[10];
	DeclareThis(SimpleFile_t);
		
	firstblock=(where + This->offset)/scsi_sector_size;
	/* 512,1024,2048,... bytes/sector supported */
	offset=where + This->offset - firstblock*scsi_sector_size;
	nsect=(offset+len+scsi_sector_size-1)/scsi_sector_size;
#if defined(OS_sun) && defined(OS_i386)
	if (scsi_sector_size>512)
		firstblock*=scsi_sector_size/512; /* work around a uscsi bug */
#endif /* sun && i386 */

	if (len>512) {
		/* avoid buffer overruns. The transfer MUST be smaller or
		* equal to the requested size! */
		while (nsect*scsi_sector_size>len)
			--nsect;
		if(!nsect) {			
			fprintf(stderr,"Scsi buffer too small\n");
			exit(1);
		}
		if(SCSI_IO_WRITE && offset) {
			/* there seems to be no memmove before a write */
			fprintf(stderr,"Unaligned write\n");
			exit(1);
		}
		/* a better implementation should use bounce buffers.
		 * However, in normal operation no buffer overruns or
		 * unaligned writes should happen anyways, as the logical
		 * sector size is (hopefully!) equal to the physical one
		 */
	}


	max = scsi_max_length();
	
	if (nsect > max)
		nsect=max;
	
	/* set up SCSI READ/WRITE command */
	memset(cdb, 0, sizeof cdb);

	switch(rwcmd) {
		case SCSI_IO_READ:
			cdb[0] = SCSI_READ;
			break;
		case SCSI_IO_WRITE:
			cdb[0] = SCSI_WRITE;
			break;
	}

	cdb[1] = 0;

	if (firstblock > 0x1fffff || nsect > 0xff) {
		/* I suspect that the ZIP drive also understands Group 1
		 * commands. If that is indeed true, we may chose Group 1
		 * more agressively in the future */

		cdb[0] |= SCSI_GROUP1;
		clen=10; /* SCSI Group 1 cmd */

		/* this is one of the rare case where explicit coding is
		 * more portable than macros... The meaning of scsi command
		 * bytes is standardised, whereas the preprocessor macros
		 * handling it might be not... */

		cdb[2] = (unsigned char) (firstblock >> 24) & 0xff;
		cdb[3] = (unsigned char) (firstblock >> 16) & 0xff;
		cdb[4] = (unsigned char) (firstblock >> 8) & 0xff;
		cdb[5] = (unsigned char) firstblock & 0xff;
		cdb[6] = 0;
		cdb[7] = (unsigned char) (nsect >> 8) & 0xff;
		cdb[8] = (unsigned char) nsect & 0xff;
		cdb[9] = 0;
	} else {
		clen = 6; /* SCSI Group 0 cmd */
		cdb[1] |= (unsigned char) ((firstblock >> 16) & 0x1f);
		cdb[2] = (unsigned char) ((firstblock >> 8) & 0xff);
		cdb[3] = (unsigned char) firstblock & 0xff;
		cdb[4] = (unsigned char) nsect;
		cdb[5] = 0;
	}
	
	if(This->privileged)
		reclaim_privs();

	r=scsi_cmd(This->fd, (unsigned char *)cdb, clen, rwcmd, buf,
		   nsect*scsi_sector_size);

	if(This->privileged)
		drop_privs();

	if(r) {
		perror(rwcmd == SCSI_IO_READ ? "SCMD_READ" : "SCMD_WRITE");
		return -1;
	}
#ifdef JPD
	printf("finished %u for %u\n", firstblock, nsect);
#endif

#ifdef JPD
	printf("zip: read or write OK\n");
#endif
	if (offset>0) memmove(buf,buf+offset,nsect*scsi_sector_size-offset);
	if (len==256) return 256;
	else if (len==512) return 512;
	else return nsect*scsi_sector_size-offset;
}

int scsi_read(Stream_t *Stream, char *buf, off_t where, size_t len)
{
	
#ifdef JPD
	printf("zip: to read %d bytes at %d\n", len, where);
#endif
	return scsi_io(Stream, buf, where, len, SCSI_IO_READ);
}

int scsi_write(Stream_t *Stream, char *buf, off_t where, size_t len)
{
#ifdef JPD
	Printf("zip: to write %d bytes at %d\n", len, where);
#endif
	return scsi_io(Stream, buf, where, len, SCSI_IO_WRITE);
}

static Class_t ScsiClass = {
	scsi_read, 
	scsi_write,
	file_flush,
	file_free,
	file_geom,
	file_data
};


static Class_t SimpleFileClass = {
	file_read, 
	file_write,
	file_flush,
	file_free,
	file_geom,
	file_data
};

#ifdef VMS
static Class_t VMS_SimpleFileClass = {
	file_read, 
	file_write,
	file_flush,
	file_free,
	file_geom,
	file_data
};
#endif

Stream_t *SimpleFileOpen(struct device *dev, struct device *orig_dev,
			 const char *name, int mode, char *errmsg, int mode2)
{
	SimpleFile_t *This;

	This = New(SimpleFile_t);
	if (!This){
		fprintf(stderr,"Out of memory error\n");
		exit(1);
	}
	This->seekable = 1;

	This->Class = &SimpleFileClass;
	if (!name || strcmp(name,"-") == 0 ){
		if (mode == O_RDONLY)
			This->fd = 0;
		else
			This->fd = 1;
		This->seekable = 0;
		This->refs = 1;
		This->Next = 0;
		This->Buffer = 0;
		return (Stream_t *) This;
	}

#ifdef VMS
	if (dev)
	    This->Class = &VMS_SimpleFileClass;
#endif
	
	if(dev) {
		if(!(mode2 & NO_PRIV))
			This->privileged = dev->privileged;
		mode |= dev->mode;
	}

	precmd(dev);
	if(dev && dev->privileged && !(mode2 & NO_PRIV))
		reclaim_privs();

#ifdef VMS
	if (dev)
	{
	    long    Result;

	    Name.dsc$b_dtype = DSC$K_DTYPE_T;
	    Name.dsc$b_class = DSC$K_CLASS_S;
	    Name.dsc$w_length = strlen(name);
	    Name.dsc$a_pointer = DevName;
	    memcpy(DevName, name, Name.dsc$w_length);

	    Result = sys$getdviw(0, 0, &Name, &DiskInfo,
			    NULL, NULL, NULL, NULL);
	    if (!(Result & 1))
		This->fd = -1;

	    if (This->fd != -1 && !(DevChar & DEV$M_FOR))
	    {
		// Let's try to mount
		MntInfo[0].Length = Name.dsc$w_length;
		MntInfo[0].Address = (void *)name;
		Result = sys$mount(&MntInfo);
		if (!(Result & 1))
		    This->fd = -1;
	    }
	    if (This->fd != -1)
	    {
		Result = sys$assign(&Name, &This->fd, 0, NULL, 0);
		if (!(Result & 1))
		    This->fd = -1;
	    }
	    if (orig_dev)
	    {
		if (!orig_dev->tracks)
		    orig_dev->tracks = VMS_cylinders;
		if (!orig_dev->heads)
		    orig_dev->heads = VMS_tracks;
		if (!orig_dev->sectors)
		    orig_dev->sectors = VMS_sectors;
	    }
	}
	else
#endif
	This->fd = open(name, mode, 0666);

	if(dev && dev->privileged && !(mode2 & NO_PRIV))
		drop_privs();
		
	if (This->fd < 0) {
		Free(This);
		if(errmsg)
			sprintf(errmsg, "Can't open %s: %s",
				name, strerror(errno));
		return NULL;
	}

	closeExec(This->fd);

#ifdef VMS
	if (!dev)
	    if (fstat(This->fd, &This->stat) < 0)
	    {
		Free(This);
		if (errmsg)
		    sprintf(errmsg,"Can't stat %s: %s", 
			    		name, strerror(errno));
		return NULL;
	    }
#else
	if (fstat(This->fd, &This->stat) < 0){
		Free(This);
		if(errmsg)
			sprintf(errmsg,"Can't stat %s: %s", 
				name, strerror(errno));

		return NULL;
	}
#endif

	/* lock the device on writes */
	if (lock_dev(This->fd, mode == O_RDWR, dev)) {
		close(This->fd);
		Free(This);
		if(errmsg)
			sprintf(errmsg,
				"plain floppy: device \"%s\" busy:",
				dev ? dev->name : "unknown");
		return NULL;
	}
		
	/* set default parameters, if needed */
	if (dev){
		if (init_geom(This->fd, dev, orig_dev, &This->stat)){
			close(This->fd);
			Free(This);
			if(errmsg)
				sprintf(errmsg,"init: set default params");
			return NULL;
		}
		This->offset = dev->offset;
	} else
		This->offset = 0;

	This->lastwhere = -This->offset;
	/* provoke a seek on those devices that don't start on a partition
	 * boundary */

	This->refs = 1;
	This->Next = 0;
	This->Buffer = 0;

	/* partitioned drive */

	/* jpd@usl.edu: assume a partitioned drive on these 2 systems is a ZIP*/
	/* or similar drive that must be accessed by low-level scsi commands */
	/* AK: introduce new "scsi=1" statement to specifically set
	 * this option. Indeed, there could conceivably be partitioned
	 * devices where low level scsi commands will not be needed */
	if(dev && dev->scsi) {
		This->Class = &ScsiClass;
		scsi_init(This->fd);
	}
	while(!(mode2 & NO_OFFSET) &&
	      dev && dev->partition && dev->partition <= 4) {
		int has_activated, last_end, j;
		unsigned char buf[2048];
		struct partition *partTable=(struct partition *)(buf+ 0x1ae);
		
		/* read the first sector, or part of it */
		if (force_read((Stream_t *)This, (char*) buf, 0, 512) != 512)
			break;
		if( _WORD(buf+510) != 0xaa55)
			break;
		This->offset += BEGIN(partTable[dev->partition]) << 9;
		if(!partTable[dev->partition].sys_ind) {
			if(errmsg)
				sprintf(errmsg,
					"init: non-existant partition");
			close(This->fd);
			Free(This);
			return NULL;
		}

		if(!dev->tracks) {
			dev->heads = head(partTable[dev->partition].end)+1;
			dev->sectors = sector(partTable[dev->partition].end);
			dev->tracks = cyl(partTable[dev->partition].end) -
				cyl(partTable[dev->partition].start)+1;
		}
		dev->hidden=dev->sectors*head(partTable[dev->partition].start);
		if(consistencyCheck((struct partition *)(buf+0x1ae), 0, 0,
				    &has_activated, &last_end, &j, dev, 0)) {
			fprintf(stderr,
				"Warning: inconsistent partition table\n");
			fprintf(stderr,
				"Possibly unpartitioned device\n");
			fprintf(stderr,
				"\n*** Maybe try without partition=%d in device definition ***\n\n",
				dev->partition);
		}
		break;
		/* NOTREACHED */
	}
	return (Stream_t *) This;
}

int get_fd(Stream_t *Stream)
{
	DeclareThis(SimpleFile_t);
	
	return This->fd;
}

