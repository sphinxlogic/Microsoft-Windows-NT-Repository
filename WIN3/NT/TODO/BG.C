A crude port of xvi for Windows NT.  Pipes (e.g. reading to/from other
processes) don't work, but otherwise it seems fully functional.
The other files here are:

   xvi.exe - the executable
   xvisrc.zip - the complete source and documentation.
   xvidiff.zip - contains only the files I had to change for NT

xvi.exe will create a new window.  Normally the window in which you
invoke xvi.exe will not be usable (because you won't be able to
invoke anything else until xvi.exe exits).  You can use the program
below to put it in the background (e.g. I have a batch file called vi.bat
which essentially invokes "bg.exe xvi.exe %1").
  
         ...Tim Thompson...tjt@blink.att.com...

---- cut here ----- ---- cut here ----- ---- cut here ----- ---- cut here ----
/*
 * bg - A utility for putting an NT program into the background.
 *
 * Intended to be compiled with a makefile like this:
 *
 * !include <ntwin32.mak>
 *
 * bg.exe:	bg.obj
 * 		$(link) $(conflags) -out:bg.exe bg.obj $(conlibs)
 * 
 * .c.obj:
 * 		$(cc) $(cflags) $(cvars) $*.c
 */

#include <stdio.h>
#include <fcntl.h>
#include <winsock.h>
#include <memory.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>

int Verbose = 0;

#define RCVSIZE 512

main(int argc,char **argv)
{
	char cmd[1024];
	SECURITY_ATTRIBUTES saPipe; 
	PROCESS_INFORMATION pi;
	STARTUPINFO si;  /* for CreateProcess call */
	int r, n;

	cmd[0] = '\0';
	for ( n=1; n<argc; n++ ) {
		strcat(cmd,argv[n]);
		strcat(cmd," ");
	}

	saPipe.nLength = sizeof(SECURITY_ATTRIBUTES);
	saPipe.lpSecurityDescriptor = NULL;
	saPipe.bInheritHandle = FALSE;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	r = CreateProcess(NULL,  /* filename */
		cmd,  /* full command line for child */
		NULL,  /* process security descriptor */
		NULL,  /* thread security descriptor */
		FALSE,  /* inherit handles? */
		0,  /* creation flags */
		NULL,  /* inherited environment address */
		NULL,  /* startup dir; NULL = start in current */
		&si,  /* pointer to startup info (input) */
		&pi);  /* pointer to process info (output) */
	if (!r) {
		fprintf(stderr,"CreateProcess() failed");
		return 1;
	}
	if ( Verbose )
		fprintf(stderr,"CreateProcess for (%s) succeeded\n",cmd);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}
