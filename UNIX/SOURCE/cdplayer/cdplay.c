/* 
   Basic cd-rom player for UnixWare

	Eric Wallengren, Univel 1993

	Can be redistribiuted as long as Univel/UnixWare 
	and my name are left intact. 

   This allows a Toshiba CD-ROM to play audio cds. It requires 
   headphones or speakers to be pluggged into the front of the
   cd-rom drive.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/cdrom.h>
#include <sys/cd_ioctl.h>

/* Define the starting track and the device used for accessing the cdrom */
#define MIN_TRACK 1

/* Change device to suit your needs */
#define CD_ROM "/dev/rcdrom/c0t2l0\0" 

/* Gotta have structures */
struct cdrom_inq Inquiry;
struct cdrom_capacity Capacity;
struct cdrom_audio Audio;

/* Ugly kludge which everyone needs, one per program */
extern int Trackloop[99]={1,2,3,4,5,6,7,8,9, \
 			  16,17,18,19,20,21,22,23,24,25, \
			  32,33,34,35,36,37,38,39,40,41, \
			  48,49,50,51,52,53,54,55,56,57, \
			  64,65,66,67,68,69,70,71,72,73, \
			  80,81,82,83,84,85,86,87,88,89, \
			  96,97,98,99,100,101,102,103,104,105, \
			  112,113,114,115,116,117,118,119,120,121, \
			  128,129,130,131,132,133,134,135,136,137, \
			  144,145,146,147,148,149,150,151,152,153};
/* File for open */
int CdRom;

extern char ValidArgs[6]={'c', 'e', 'p', 'r', 's', 't'};

main(argc, argv)
int   argc;
char *argv[];
{

	int a, tracks;
	int b=0;

	for(a=1; a<argc; a++)
	{
		if(argv[a][0] != '-')
		{
			printf("Invalid switch\n");
			exit(1);
		}
		for(b=0; b<6; b++)
		{

			if(argv[a][1] == ValidArgs[b])
			{
				switch(ValidArgs[b]) {
					case 'c' :
						tracks=Count_Tracks();
						printf("Tracks: %d\n", tracks);
					break;
					case 'e' :
						Eject_Disk();
					break;
					case 'r' :
						Resume_Play();
					break;
					case 't' :
						a++;
						Select_Track(atoi(argv[a]));
					break;
					case 'p' :
						Stop_Play();
					break;
					case 's' :
						Quit_Play();
					break;
					default:
						Help_Message();
					break;	
				}
				exit(0);
			}
		}
		
	}
	Help_Message();
}

/* Open the device */
Open_Device()
{
	if((CdRom=open(CD_ROM, O_RDONLY)) < 0)
		perror("Opening Device"), exit(1);
}

/* Quit playing the CD */
Quit_Play()
{

	int Return;

	Open_Device();

	if((Return=ioctl(CdRom, C_STOPUNIT, NULL)) < 0)
		perror("Stopping Play"), exit(1);

}

/* How many tracks are there */
Count_Tracks()
{

	int Return;
	int tracks=0;

	Open_Device();

	if((Return=ioctl(CdRom, C_STARTUNIT, NULL)) < 0)
		perror("Starting CD-ROM");

	if((Return=ioctl(CdRom, C_REZERO, NULL)) < 0)
		perror("Seeking track 0");

	Audio.addr_track=Trackloop[tracks];

	Audio.play=0;
	Audio.type=2;

	while(Return > -1)
	{
		if((Return=ioctl(CdRom, C_AUDIOSEARCH, &Audio)) < 0)
			return tracks;
		++tracks;
		Audio.addr_track=Trackloop[tracks];
	}
}

/* Kick the disk out */
Eject_Disk()
{

	int Return;

	Open_Device();

	if((Return=ioctl(CdRom, C_TRAYOPEN, NULL)) < 0)
		perror("Opening Tray");

}

/* Select the track to play */
Select_Track(TrackNum)
int TrackNum;
{

	int Return;

	Open_Device();

	Audio.play=0;
	Audio.type=2;

	Audio.addr_track=Trackloop[TrackNum-1];

	if((Return=ioctl(CdRom, C_AUDIOSEARCH, &Audio)) < 0)
		printf("Track %d does not exist\n", TrackNum ), exit(1);
	
	Audio.play=3;
	Audio.type=3;

	if((Return=ioctl(CdRom, C_PLAYAUDIO, &Audio)) < 0)
		perror("Playing CD");
	
}

/* Pause the cd-rom */
Stop_Play()
{

	int Return;

	Open_Device();

	if((Return=ioctl(CdRom, C_STILL, NULL)) < 0)
		perror("Stopping Player");

}

/* Resume playing the cd-rom */
Resume_Play()
{

	int Return;

	Open_Device();

	Audio.play=3;
	Audio.type=2;

	if((Return=ioctl(CdRom, C_PLAYAUDIO, &Audio)) < 0)
		perror("Resuming Play");

}

/* How do ya use this thing anyway? */
Help_Message()
{

	printf("Usage:  -c Count tracks\n \
	-e Eject Disk\n\t-p Pause Play\n \
	-r Restart from current position\n \
	-s Stop Play\n \
	-t <int> Select track to start play from\n");

}
