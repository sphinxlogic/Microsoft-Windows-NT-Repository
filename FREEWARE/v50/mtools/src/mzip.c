/*
 * mzip.c
 * Iomega Zip/Jaz drive tool
 * change protection mode and eject disk
 */

/* mzip.c by Markus Gyger <mgyger@itr.ch> */
/* This code is based on ftp://gear.torque.net/pub/ziptool.c */
/* by Grant R. Guenther with the following copyright notice: */

/*  (c) 1996   Grant R. Guenther,  based on work of Itai Nahshon  */
/*  http://www.torque.net/ziptool.html  */

#include "sysincludes.h"
#include "mtools.h"
#include "scsi.h"

#ifndef _PASSWORD_LEN
#define _PASSWORD_LEN 33
#endif

#ifdef OS_linux
#include <linux/fs.h>
#endif

static void usage(void)
{
	fprintf(stderr, 
		"Mtools version %s, dated %s\n", 
		mversion, mdate);
	fprintf(stderr, 
		"Usage: %s [-V] [-q] [-e] [-r|-w|-p|-x] [drive:]\n"
		"\t-q print status\n"
		"\t-e eject disk\n"
		"\t-f eject disk even when mounted\n"
		"\t-r write protected (read-only)\n"
		"\t-w not write-protected (read-write)\n"
		"\t-p password write protected\n"
		"\t-x password protected\n", 
		progname);
	exit(1);
}


static int get_zip_status(int fd)
{
	unsigned char status[128];
	unsigned char cdb[6] = { 0x06, 0, 0x02, 0, sizeof status, 0 };
	
	if (scsi_cmd(fd, cdb, 6, SCSI_IO_READ, 
		     status, sizeof status) == -1) {
		perror("status: ");
		exit(1);
	}
	return status[21] & 0xf;
}


static int short_command(int fd, int cmd1, int cmd2, int cmd3, char *data)
{
	unsigned char cdb[6] = { 0, 0, 0, 0, 0, 0 };

	cdb[0] = cmd1;
	cdb[1] = cmd2;
	cdb[4] = cmd3;

	return scsi_cmd(fd, cdb, 6, SCSI_IO_WRITE, 
			data, data ? strlen(data) : 0);
}


static int iomega_command(int fd, int mode, char *data)
{
	return short_command(fd, 
			     SCSI_IOMEGA, mode, data ? strlen(data) : 0,
			     data);
}

static int door_command(int fd, int cmd1, int cmd2)
{
	return short_command(fd, cmd1, 0, cmd2, 0);
}

void mzip(int argc, char **argv, int type)
{
	int c;
	char drive;
	device_t *dev;
	int fd = -1;
	char name[EXPAND_BUF];
	enum { ZIP_NIX    =      0,
	       ZIP_STATUS = 1 << 0,
	       ZIP_EJECT  = 1 << 1,
	       ZIP_RW     = 1 << 2,
	       ZIP_RO     = 1 << 3,
	       ZIP_RO_PW  = 1 << 4,
	       ZIP_PW     = 1 << 5,
	       ZIP_FORCE  = 1 << 6
	} request = ZIP_NIX;
	
	/* get command line options */
	while ((c = getopt(argc, argv, "efpqrwx")) != EOF) {
		switch (c) {
			case 'f':
				request |= ZIP_FORCE;
				break;
			case 'e': /* eject */
				request |= ZIP_EJECT;
				break;
			case 'p': /* password read-only */
				request |= ZIP_RO_PW;
				break;
			case 'q': /* status query */
				request |= ZIP_STATUS;
				break;
			case 'r': /* read-only */
				request |= ZIP_RO;
				break;
			case 'w': /* read-write */
				request |= ZIP_RW;
				break;
			case 'x': /* password protected */
				request |= ZIP_PW;
				break;
			default:  /* unrecognized */
				usage();
		}
	}
	
	if (request == ZIP_NIX) request = ZIP_STATUS;  /* default action */

	if(!!(request & ZIP_RW) + !!(request & ZIP_RO) +
	   !!(request & ZIP_RO_PW) + !!(request & ZIP_PW) > 1)
		usage();
	
	if (argc - optind > 1 || 
	    (argc - optind == 1 &&
	     (!argv[optind][0] || argv[optind][1] != ':')))
		usage();
	
	drive = toupper(argc - optind == 1 ? argv[argc - 1][0] : 'a');
	
	for (dev = devices; dev->name; dev++) {
		unsigned char cdb[6] = { 0, 0, 0, 0, 0, 0 };
		struct {
			char    type,
				type_modifier,
				scsi_version,
				data_format,
				length,
				reserved1[2],
				capabilities,
				vendor[8],
				product[16],
				revision[4],
				vendor_specific[20],
				reserved2[40];
		} inq_data;

		if (dev->drive != drive) 
			continue;
		expand(dev->name, name);
		precmd(dev);

		if(dev->privileged)
			reclaim_privs();
		fd = open(name, O_RDONLY | dev->mode); 
		if(dev->privileged)
			drop_privs();

		/* need readonly, else we can't
		 * open the drive on Solaris if
		 * write-protected */		
		if (fd == -1) 
			continue;
		closeExec(fd);


		if (!(request & (ZIP_RW | ZIP_RO | ZIP_RO_PW | 
				 ZIP_PW | ZIP_STATUS)))
			/* if no mode change or ZIP specific status is
			 * involved, the command (eject) is applicable
			 * on all drives */
			break;

		cdb[0] = SCSI_INQUIRY;
		cdb[4] = sizeof inq_data;
		if (scsi_cmd(fd, cdb, 6, SCSI_IO_READ, 
			     &inq_data, sizeof inq_data) != 0) {
			close(fd);
			continue;
		}
		
#ifdef DEBUG
		fprintf(stderr, "device: %s\n\tvendor: %.8s\n\tproduct: %.16s\n"
			"\trevision: %.4s\n", name, inq_data.vendor,
			inq_data.product, inq_data.revision);
#endif /* DEBUG */
		
		if (strncasecmp("IOMEGA  ", inq_data.vendor, 
				sizeof inq_data.vendor) ||
		    (strncasecmp("ZIP 100         ", 
				 inq_data.product, sizeof inq_data.product) &&
		     strncasecmp("JAZ 1GB         ", 
				 inq_data.product, sizeof inq_data.product))) {
			
			/* debugging */
			fprintf(stderr,"Skipping drive with vendor='");
			fwrite(inq_data.vendor,1, sizeof(inq_data.vendor), 
			       stderr);
			fprintf(stderr,"' product='");
			fwrite(inq_data.product,1, sizeof(inq_data.product), 
			       stderr);
			fprintf(stderr,"'\n");
			/* end debugging */
			close(fd);
			continue;
		}
		break;  /* found Zip/Jaz drive */
	}
	
	if (dev->drive == 0) {
		fprintf(stderr, "%s: drive '%c:' is not a Zip or Jaz drive\n",
			argv[0], drive);
		exit(1);
	}
	
	if (request & (ZIP_RW | ZIP_RO | ZIP_RO_PW | ZIP_PW)) {
		int ret;
		char *passwd, dummy[1];
		int newmode;

		/* unlock the disk by trying the tools disk password first */
		if (get_zip_status(fd) & 0x01) {  /* unlock first */
			char *s, *passwd;
			passwd = "APlaceForYourStuff";
			if ((s = strchr(passwd, '\n'))) *s = '\0';  /* chomp */
			iomega_command(fd, 0, passwd);
		}

		if (get_zip_status(fd) & 0x01) {  /* unlock first */
			char *s, *passwd;
			passwd = getpass("Password: ");
			if ((s = strchr(passwd, '\n'))) *s = '\0';  /* chomp */
			if((ret=iomega_command(fd, 0, passwd))){
				if (ret == -1) perror("passwd: ");
				else fprintf(stderr, "wrong password\n");
				exit(1);
			}
			if(get_zip_status(fd) & 0x01) {
				fprintf(stderr, "wrong password\n");
				exit(1);
			}
		}
		
		if (request & (ZIP_RO_PW | ZIP_PW)) {			
			char first_try[_PASSWORD_LEN];
			
			passwd = getpass("Enter new password:");
			strncpy(first_try, passwd,_PASSWORD_LEN);
			passwd = getpass("Re-type new password:");
			if(strncmp(first_try, passwd, _PASSWORD_LEN)) {
				fprintf(stderr,
					"You mispelled it. Password not set.\n");
				exit(1);
			}
		} else {
			passwd = dummy;
			dummy[0] = '\0';
		}
			
		switch (request & 
			(ZIP_RW | ZIP_RO | ZIP_RO_PW | ZIP_PW)) {
			case ZIP_RW:    
				newmode = 0x00;
				break;
			case ZIP_RO:   
				newmode = 0x02;
				break;
			case ZIP_RO_PW:
				newmode = 0x03;
				break;
			case ZIP_PW:  
				newmode = 0x05;
				break;
			default:
				newmode = 0;
				break;
		}
			
		if((ret=iomega_command(fd, newmode, passwd))){
			if (ret == -1) perror("set passwd: ");
			else fprintf(stderr, "password not changed\n");
			exit(1);
		}
#ifdef OS_linux
		ioctl(fd, BLKRRPART); /* revalidate the disk, so that the
					 kernel notices that its writable
					 status has changed */
#endif
	}
	
	if (request & ZIP_STATUS) {
		int status;
		
		switch (status = get_zip_status(fd)) {
			case 0:  
				printf("Drive '%c:' is not write-protected\n", 
				       drive);
				break;
			case 2:
				printf("Drive '%c:' is write-protected\n",
				       drive);
				break;
			case 3: 
				printf("Drive '%c:' is password write-protected\n", 
				       drive);
				break;
			case 5:  
				printf("Drive '%c:' is password protected\n", 
				       drive);
				break;
			default: 
				printf("Unknown protection mode %d of drive '%c:'\n",
				       status, drive);
				break;				
		}		
	}
	
	if (request & ZIP_EJECT) {
		if(request & ZIP_FORCE)
			if(door_command(fd, SCSI_ALLOW_MEDIUM_REMOVAL, 0) < 0) {
				perror("door unlock: ");
				exit(1);
			}

		if(door_command(fd, SCSI_START_STOP, 1) < 0) {
			perror("stop motor: ");
			exit(1);
		}

		if(door_command(fd, SCSI_START_STOP, 2) < 0) {
			perror("eject: ");
			exit(1);
		}
		if(door_command(fd, SCSI_START_STOP, 2) < 0) {
			perror("second eject: ");
			exit(1);
		}
	}
	
	close(fd);
	exit(0);
}
