#define module_name	VMSTAR
#define module_version	"V2.1-1"
/*
 *	VMSTAR.C - a Un*x-like tar reader/writer for VMS
 *	           based on TAR2VMS and VMS2TAR
 *
 * Usage (VMS-style):
 *	$ VMSTAR tarfile file[,...]
 *        /HELP	     -- Write a description and exit.
 *	  /LIST      -- List contents of tarfile
 *	  /CREATE    -- Create a tarfile
 *	  /EXTRACT   -- Extract files from tarfile
 *	  /VERBOSE   -- Display processed file info
 *	  /CONFIRM   -- Prompt before store/extract
 *	  /BINARY    -- Create binary files
 *	  /AUTOMATIC -- Automatically determine file type
 *	  /DOTS      -- Maintain `.' usage
 *
 * Usage (Un*x-style):
 * 	tar h|x|t|c[v][w][b][z][d][D][f tarfile] [file [file...]]
 *      h - prints a description and exits.
 *	x - extract from tarfile
 *	t - type contents of tarfile
 *	c - create tarfile, archive VMS files
 *	v - verbose
 *	w - wait for confirmation before extracting/archiving
 *	b - force binary mode extract, create (rfm=fixed, rat=none, mrs=512)
 *          files
 *	z - automatic mode, try to guess if files are text or binary, extract
 *          text files as (rfm=stream-LF, rat=cr) binary files as
 *          (rfm=fixed, rat=none, mrs=512)
 *	d - When creating a tar file, keep trailing dots in file names.
 *	    When extracting from a tar file, keep dots in directory names.
 *	    Otherwise, the dots are converted to underscores.
 *	f - specify tarfile name, default is $TAPE
 *
 *	file - space-separated list of file names, can include VMS-style
 *	       string wildcards on extract, can be any VMS file name
 *             specification (except DECnet) on create archive.
 *
 * Original author of the VMS2TAR and TAR2VMS programs:
 * Copyright 1986, Sid Penstone,
 *  Department of Electrical Engineering,
 *  Queen's University,
 *  Kingston, Ontario, Canada K7L3N6
 * (613)-545-2925
 * BITNET:   PENSTONE@QUCDNEE1
 *
 * Deeply modified by:
 * Alain Fauconnet
 * System Manager
 * SIM - Public Health Research Laboratories
 * 91 Boulevard de l'Hopital
 * 75634 PARIS CEDEX 13 - FRANCE
 * Bitnet: FAUCONNE@FRSIM51
 *
 * Currently watched over by:
 *
 *	Hunter Goatley				Richard Levitte
 *	VMS Systems Programmer			GNU on VMS hacker
 *	Western Kentucky University		S�dra L�nggatan 39, II
 *	1 Big Red Way				S-171 49  Solna
 *	Bowling Green, KY 42101			SWEDEN
 *	E-mail: goathunter@WKUVX1.WKU.EDU	levitte@e.kth.se
 *	FAX: +1 502 745 6014			N/A
 *
 * PROBLEMS SHOULD BE REPORTED TO EITHER HUNTER OR RICHARD.
 *
 * Version 2.0-4 1-JUL-1994
 * Based on TAR2VMS V2.2 21-OCT-1986 and VMS2TAR V1.8 23-DEC-1986
 *
 * Sid Penstone did not include any copyright information in his program so
 * this only applies if Sid Penstone agrees: you may use VMSTAR, distribute it,
 * modify it freely provided you don't use it for commercial
 * or military purposes. Please include the two above author names in the
 * source file of any modified version of VMSTAR.
 *
 * Modification history in CHANGELOG.TXT
 */

#ifdef __DECC
#pragma module module_name module_version
#else
#module module_name module_version
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <unixlib.h>
#include <ssdef.h>
#include <stsdef.h>

#include "vmstar_cmdline.h"
#include "vmstarP.h"

/* forward declarations of routines */

int tar2vms();
int vms2tar();

/* main -- parses options, dispacthes to tar2vms and vms2tar */

int main(int argc, char **argv)
{
register int status;
register char *cp;

/* Decode the options and parameters: */

    status = vmstar_cmdline (&argc, &argv);
    if (!(status & 1)) return(status | STS$M_INHIB_MSG);

/* Set up the current directory names */

#if 0
    strcpy(temp, getenv("PATH"));
#ifdef DEBUG
    fputs(temp, stdout);fputc('\n',stdout);
#endif
#endif
    getcwd(temp, NAMSIZE, 1);
#ifdef DEBUG
    fputs(temp, stdout);fputc('\n',stdout);
#endif
    cp = strchr(temp, ':');    /* split into device and directory */
    *cp++ = '\0';
    strcpy(curdir, cp);

    for (cp = curdir; *cp != '\0'; ++cp)
        *cp = toupper(*cp);		/* map to uppercase */

    if (create == 0)
        tar2vms(argc,argv);
    else
    {
	if (argc == 0) {
	    printf("tar: input file(s) not specified.\n");
#if 0
	    usage ();
#endif
	    exit(SS$_NORMAL);
	}
        vms2tar(argc,argv);
    }
}
