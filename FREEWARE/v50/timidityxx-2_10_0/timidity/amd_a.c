/*-
 * Sound file for vms
 * Jouk Jansen <joukj@alpha.chem.uva.nl> contributed this
 * which he found at http://axp616.gsi.de:8080/www/vms/mzsw.html
 */

/** amd.c **/

#include <stdio.h>
#include <iodef.h>		/* needed for IO$_ functions */
#include <ssdef.h>		/* system condition codes */
#include <descrip.h>		/* VMS Descriptor functions */
#include <string.h>
#include <starlet.h>
#include <lib$routines.h>

#include "amd.h"		/* AMD access functions */

/* Timidity */
#include "config.h"
#include "timidity.h"
#include "output.h"
#include "controls.h"

static int open_output(void); /* 0=success, 1=warning, -1=fatal error */
static void close_output(void);
static int output_data(char *buf, int32 count);
static int acntl(int request, void *arg);


static int32 bufs1[2048];
static int32 bufs2[2048];
static int32 *bufsptr1=&bufs1[0]; 
static int32 *bufsptr2=&bufs2[0];
static int numbuf; 

extern int default_play_event(void *);

/* export the playback mode */

#define dpm amd_play_mode

PlayMode dpm = {
  DEFAULT_RATE, PE_MONO|PE_ULAW, PF_PCM_STREAM|PF_CAN_TRACE,
  -1,
  {0,0,0,0,0},
  "Amd audio device", 'd',
  "SO:",
  open_output,
  close_output,
  output_data,
  acntl
};


struct dsc$descriptor_s devname;	/* device we are using */
static short int so_chan;	/* Channel to SODRIVER */
char        ReadBuffer[512];	/* Asynchronous read buffer */
unsigned short ReadIOsb[4] =
{0, 0, 0, 0};
int         ReadPending = 0;
int         ReadCompleted = 0;
/* */
float soflushtime = 0.5; /* Wait time to flush internal SO buffer */

/*-
 *	Initialize access to the AMD chip
 */
/*unsigned long int AmdInitialize(char *device, int volume) */
static int open_output(void)
{
	unsigned long int status;
	unsigned long int iosb[2] =
	{0, 0};
	int         p1;
        int volume = 65;
        unsigned long int efn = 28;
#ifdef DEBUG
	printf("Address of AMD Read buffer is 0x%x\n", &ReadBuffer);
#endif
        numbuf = 2;
	/*
	 *    Create the descriptor
	 */
	devname.dsc$w_length = strlen(dpm.name);
	devname.dsc$a_pointer = dpm.name;
	devname.dsc$b_dtype = DSC$K_DTYPE_T;
	devname.dsc$b_class = DSC$K_CLASS_S;
	/*
	 *    Assign a channel to the device
	 */
	status = sys$assign(&devname, &so_chan, 0, 0, 0);
	if (!(status & 1))
		return (status);
	/*
	 *    Clear MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Clear MMR2
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR2,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set Volume to initial 30%
	 */
	p1 = 0x2ABD;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GER,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set MMR1 to 8
	 */
	p1 = 8;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set GR
	 */
	p1 = 0x92FB;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GR,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set MMR1 to 12 (0x0C)
	 */
	p1 = 12;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR2
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR2,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Put it back MMR2
	 */
/*        p1=  SO_EXTERNAL;   /* External audio output */
        p1 = SO_INTERNAL; /* Internal speaker */
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR2,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set GX to 0x0112
	 */
	p1 = 0x0112;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GX,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set MMR1 to 14 (0x0E)
	 */
	p1 = 14;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set GER (Volume to what the user wants)
	 */
	p1 = VolumeTable[volume - 1].ger_value;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GER,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set GR (Volume to what the user wants)
	 */
	p1 = VolumeTable[volume - 1].gr_value;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GR,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Return Success
	 */
        sys$setef (efn);  /* set event flag 28 for writing to output devive */
	return (0);
}

/*-
 *	Return the channel number associated with the device
 */
unsigned short
AmdGetChannel(void)
{
	return (so_chan);
}

/*-
 *	Increase or Decrease the Volume
 */
AmdSetVolume(int volume)
{
	unsigned long int status;
	unsigned short iosb[4] =
	{0, 0, 0, 0};
	int         p1;

	/*
	 *    Set GER (Volume to what the user wants)
	 */
	p1 = VolumeTable[volume - 1].ger_value;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GER,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set GR (Volume to what the user wants)
	 */
	p1 = VolumeTable[volume - 1].gr_value;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GR,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Get MMR1
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR1,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Return Success
	 */
	return (SS$_NORMAL);
}

/*-
 *	Start recording sound
 */
unsigned long int
AmdInitRecord()
{
	unsigned long int status;
	unsigned long int iosb[2] =
	{0, 0};
	int         p1, p2, p3, p4, p5, p6;

	/*
	 *    Set recording level to 100%
	 */
	p1 = 0x0E;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* I/O status */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer Length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_GX,	/* AMD Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Start recording
	 */
	p3 = 2;
	p4 = 0x07D0;
	p5 = 0x14;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACCESS,	/* function code */
			  iosb,	/* I/O Status */
			  0,	/* ast routine */
			  0,	/* ast param */
			  0,	/* P1 */
			  0,	/* P2 */
			  p3,	/* Single freq tone */
			  p4,	/* frequency */
			  p5,	/* amplitude */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Retrieve the value of MMR2
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_GETREG,	/* IOCTL code (P3) */
			  MAP_MMR2,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Set MMR2 to 0
	 */
	p1 = 0;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR2,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Issue a stop
	 */
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* I/O status */
			  0,	/* ast routine */
			  0,	/* ast param */
			  0,	/* P1 */
			  0,	/* P2 */
			  AMD_STOP,	/* IOCTL function code */
			  0,	/* P4 */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Issue a read start to begin reading data
	 */
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* I/O status */
			  0,	/* ast routine */
			  0,	/* ast param */
			  0,	/* P1 */
			  0,	/* P2 */
			  AMD_READSTART,	/* IOCTL function code */
			  0,	/* P4 */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);
	/*
	 *    Return success
	 */
	return (0);
}

/*-
 *	Select the Internal or External Speaker
 */
unsigned long int
AmdSelect(int s)
{
	unsigned long int status;
	unsigned long int iosb[2];
	int         p1;

	/*
	 *    Set the MMR2 register to indicate speaker
	 */
	p1 = s;
	status = sys$qiow(0,	/* efn */
			  so_chan,	/* channel */
			  IO$_ACPCONTROL,	/* function */
			  iosb,	/* iosb */
			  0,	/* ast routine */
			  0,	/* ast param */
			  &p1,	/* Buffer (P1) */
			  sizeof (p1),	/* Buffer length (P2) */
			  AMD_SETREG,	/* IOCTL code (P3) */
			  MAP_MMR2,	/* AMD INDIRECT Register (P4) */
			  0,	/* P5 */
			  0);	/* P6 */

	if (!(status & 1))
		return (status);
	if (!(iosb[0] & 1))
		return (iosb[0]);

}

/*-
 *  Write a buffer of sound data to the device
 */
/* unsigned long int AmdWrite(char *buffer, int len) */

/* Asynchronous writes on AMD device with 2 flip/flop buffers. */
/* Event Flag 28 is used.    */
static int output_data(char *buf, int32 count)
{
  unsigned long int status;
  unsigned long int efn = 28;  /* event flag number */
  unsigned long iosb[2] =
  {0, 0};
  int i;
#ifdef DEBUG
  printf("AmdWrite: buffer %d, count %d \n",numbuf,count);
#endif
  if (numbuf == 2) 
    {
      memcpy(bufs1,buf,count);

  /*
   *    Write the data to the device
   */
        status = sys$waitfr(efn); /* Wait for Event Flag 28 */
        status = sys$clref(efn);  /* Clear Event flag 28 */
        status = sys$qio (efn,   
/*      status = sys$qiow(0,    */
        so_chan,
        IO$_WRITEVBLK,
        0,
        0,
        0,
        bufsptr1, count, 0, 0, 0, 0);
      numbuf = 1;   /* switch buffers */
    }
    else
     {
       memcpy(bufs2,buf,count);

  /*
   *    Write the data to the device
   */
        status = sys$waitfr(efn); /* Wait for Event Flag 28 */
        status = sys$clref(efn);  /* Clear Event flag 28 */
        status = sys$qio (efn,  
/*      status = sys$qiow(0, */
        so_chan,
        IO$_WRITEVBLK,
        0,
        0,
        0,
        bufsptr2, count, 0, 0, 0, 0);
      numbuf = 2;   /* switch buffers */
    }
 return 1;
}

/* Dummies */
static void close_output (void) { }
static int acntl(int request, void *arg)
{
    return -1;
}
