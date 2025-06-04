#include "include/copyright.h"

/* VMS DIVA Audio format */

/*
 *  Include file dependencies:
 */

#include <stdio.h>

#ifndef __DECC
/*typedef unsigned long   u_long; */
#define O_RDONLY        0x00
typedef int wchar_t;
#else
#include "sys$common:[decc$lib.include]types.h"
#define O_RDONLY        0x00

#ifdef __DECC_VER
#if (__DECC_VER < 50200000)
typedef	unsigned long	u_long;
#endif /* __DECC_VER < 50200000 , that is DEC C 5.0 or 5.1 */
#else    /* DEC C 4.0 , __DECC_VER is not defined */
typedef	unsigned long	u_long;
#endif /* __DECC_VER */
#endif  /* VMS */

#include <unixio.h>
#include <iodef.h>
#include <file.h>

#include "include/error.h"
#include "include/audio.h"
/*
 *  Internal macro definitions:
 */
#include "amd.h"
/*
 *  Internal type declarations:
 */

/*
 *  Internal variable declarations:
 */

#if NeedFunctionPrototypes
int SetUpAudioSystem(Display *display)
#else
int SetUpAudioSystem(display)
	Display *display;
#endif
{
        int volume = 65;
        int status;
        int speaker = SO_EXTERNAL;
        status = AmdInitialize("SO:",volume);
        AmdSelect(speaker);
	return True;
}

#if NeedFunctionPrototypes
void FreeAudioSystem(void)
#else
void FreeAudioSystem()
#endif
{
	/* Do nothing*/
}

#if NeedFunctionPrototypes
void setNewVolume(unsigned int Volume)
#else
void setNewVolume(Volume)
	unsigned int Volume;
#endif
{
       int levolume;
       int status;
       int speaker = SO_EXTERNAL;
       levolume = Volume;
       status = AmdSetVolume(levolume);
}

#if NeedFunctionPrototypes
void playSoundFile(char *filename, int volume)
#else
void playSoundFile(filename, volume)
	char *filename;
	int volume;
#endif
{       int speaker = SO_EXTERNAL;
        int status;
        int fp;
        int i;
        char buffer[2048];
        char *soundfile;

/*        status = AmdSetVolume(volume); */
        /*
         *      Open the file  (first construct the file path)
         */
    	sprintf(soundfile, "%s:%s.au", SOUNDS_DIR, filename);
       printf("Soundfile is: %s\n", soundfile);       
        fp = open(soundfile,O_RDONLY,0777);
        /*
         *      Read through it
         */
        i = read(fp, buffer, 2048);
        while(i) {
                status = AmdWrite(buffer, i);
                if (!(status&1)) exit(status);
                i = read(fp, buffer, 2048);
        }
        /*
         *      Close the file
         */
        close(fp);

}

#if NeedFunctionPrototypes
void audioDeviceEvents(void)
#else
void audioDeviceEvents()
#endif
{
    /* None to do */
}

#if NeedFunctionPrototypes
void SetMaximumVolume(int Volume)
#else
void SetMaximumVolume(Volume)
    int Volume;
#endif
{
}

#if NeedFunctionPrototypes
int GetMaximumVolume(void)
#else
int GetMaximumVolume()
#endif
{
    return 100;
}

