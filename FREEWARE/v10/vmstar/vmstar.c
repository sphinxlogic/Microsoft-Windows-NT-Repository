/*
 *	VMSTAR.C - a Un*x-like tar reader/writer for VMS
 *	           based on TAR2VMS and VMS2TAR
 *
 * Usage:
 * 	tar x|t|c[v][w][b[d]][f tarfile] [file [file...]]
 *	x - extract from tarfile
 *	t - type contents of tarfile
 *	c - create tarfile, archive VMS files
 *	v - verbose
 *	w - wait for confirmation before extracting/archiving
 *	b - binary mode extract, create (rfm=fixed, rat=none, mrs=512) files
 *	d - keep trailing dots in file names
 *	f - specify tarfile name, default is $TAPE
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
 * PROBLEMS SHOULD BE REPORTED TO ME. PLEASE DON'T BOTHER SID PENSTONE
 * WITH MY OWN BUGS !
 *
 * Version 1.5-3.1 03-NOV-1994
 * Version 1.5-3   26-SEP-1991
 * Based on TAR2VMS V2.2 21-OCT-1986 and VMS2TAR V1.8 23-DEC-1986
 *
 * Sid Penstone did not include any copyright information in his program so
 * this only applies if Sid Penstone agrees: you may use VMSTAR, distribute it,
 * modify it freely provided you don't use it for commercial
 * or military purposes. Please include the two above author names in the
 * source file of any modified version of VMSTAR.
 *
 * Modification history:
 *
 * 1.5-3.1 Changed "wait" to "waitp", for some reason, DECC barfs on it
 *         and thinks it refers to the wait function call.  It seemed
 *         easier to hack, than to try and figure out the magic incantation
 *         for DECC.  FGK @DEC 03-Nov-1994.  To build for Alpha, use
 *         CC/STAND=VAXC VMSTAR   and then  LINK VMSTAR  - no options needed.
 * 1.5-3 - removed duplicate error message
 * 1.5-2 - removed duplicate #include iodef
 *       - added write status checking in copyfile and cleaned up error
 *         handling (avoids duplicate messages)
 * 1.5-1 - fix by Mark Parisi <MPARISI@RIPPLE.JPL.NASA.GOV>
 *         fixed bug in out_file: if the length of a text file was an
 *         exact multiple of DSIZE, flushout was called an additional
 *         time.
 *       - added some code for empty files handling.
 * 1.5  - when archiving a non-text file with rfm=stream_lf, rat=cr
 *        attributes, VMSTAR truncated the file. Modified out_file
 *        to more cleanly handle various RMS file formats: only
 *        variable and stream_cr record formats need two passes to compute
 *        the actual file size and need to be read record by record.
 *        All other formats should by read buffer by buffer and written
 *        as-is in the tar archive, thus out_file now fopens the file
 *        in binary mode and freads buffers.
 *        In the case of a stream_cr file, if a single record cannot fit in
 *        our buffers (probably because the file is non-text and has
 *        incorrect RMS attributes) out_file now error exits.
 * 1.4	- fixed a bug in scan_title that caused VMSTAR to fail on
 *	  extraction of absolute tarfiles (thanks to Tom Allebrandi
 *	  for this one)
 *	- added some code in scan_title to correctly handle dots in
 *	  directory names found in tarfile (replaced by "_")
 * 1.3  - changed tar2vms to use standard IO calls (fopen, fread) to
 *        read input tarfile in binary mode i.e. no translation of
 *        RMS record attributes done by C RTL. This fixes problem reading
 *        tarfiles created with rfm=fix, rat=cr
 *      - more room for file size in output formats
 * 1.2  - fixed bug in out_file not closing input VMS file, limited
 *        maximum number of files archived to FILLM quota
 *      - added mapping to underscores of some more characters found
 *        in Un*x file names illegal in VMS file names
 * 1.1  - reworked handling of current VMS default
 *      - will now create *relative* tarfiles (i.e. files known
 *        as "./..." in tar archive directory) except when
 *        device name is specified or wilcard search gives filenames
 *        located above current default (tar cvf foo.tar [-...]*.* will
 *        lead to this kind of situation)
 *      - attempt to handle more than 8 levels of directories upon
 *        extract : .../d7/d8/d9 will be mapped to [...D7.D8$D9]
 *      - greatly simplified make_new() because mkdir() creates
 *        intermediate levels of directories if missing
 * 1.0	Original version from Sid Penstone's code
 *	- merged VMS2TAR & TAR2VMS into a single source file
 *	- code reworked, messages cleaned up
 *	- added support for 'f tarfile' option, changed default to $TAPE
 *	- added support for VMS style wildcard file names on extract
 *	- added support for 'b' (binary file extract) option
 *	- suppressed usage of intermediate scratch file for create operation
 *	- file list on create should now be space separated (removed difficult
 *	  support of comma-separated list with context "a la BACKUP")
 *	- global code simplification attempt, may have broken some
 *	  odd case handling
 *	- added some error handling in tarfile write operations
 *	- probably millions of bugs introduced... sorry.
 */

#include stdio
#include time
#include ssdef
#include iodef
#include descrip
#include ctype
#include strdef
#include rms
#include stsdef
#include file
#include stat
#include types
#include string
#include errno

#define ERROR1 -1
#define BUFFSIZE 512
#define ISDIRE 1
#define ISFILE 0
#define NAMSIZE 100
#define BLKSIZE 10240           /* Block size */
#define DSIZE 512               /* Data block size */

struct tarhdr                   /* A tar header */
{
    char title[NAMSIZE];
    char protection[8];
    char uid[8];                /* this is the user id */
    char gid[8];                /* this is the group id */
    char count[12];             /* was 11 in error */
    char time[12];              /* UNIX format date  */
    char chksum[8];             /* header checksum */
    char linkcount;             /* hope this is right */
    char linkname[NAMSIZE];     /* Space for the name of the link */
    char dummy[255];            /* and the rest */
} header;

char buffer[DSIZE];             /* buffer for a tarfile record */

/* Function flags, options */

int extract,            /* x option, extract */
    list,               /* t option, list tape contents */
    verbose,            /* v option, report actions */
    waitp,               /* w option, prompt */
    dot,                /* d option, suppress dots */
    create,             /* c option, create */
    binmode,            /* z option, binary mode */
    foption;		/* f option, specify tarfile */

/* Miscellaneous globals, etc. */

char tarfile[NAMSIZE],  /* Tarfile name  */
    pathname[NAMSIZE],  /* File name as found on tape (UNIX) */
    curdir[NAMSIZE],    /* Current directory */
    topdir[NAMSIZE],    /* Top level directory of current default */
    curdev[NAMSIZE],    /* Current device */
    new_directory[NAMSIZE],     /* Directory of current file */
    newfile[NAMSIZE],   /* VMS format of file name */
    outfile[NAMSIZE],   /* Complete output file specification */
    temp[NAMSIZE],      /* Scratch */
    creation[NAMSIZE],  /* Date as extracted from the TAR file */
    *ctime(),           /* System function */
    linkname[NAMSIZE],  /* Linked file name  */
    searchname[NAMSIZE], /* used in the NAM block for SYS$SEARCH */
    dbuffer[DSIZE],     /* input file buffer for create operation */
    badchars[] = {",+~`@#%^*?|\&[]{}"};
                        /* Chars found in Un*x file names, illegal in VMS */

struct stat sblock;     /* structure returned from stat() */
struct FAB fblock;      /* File attribute block */
struct NAM nblock;      /* Name attribute block for rms */

int bytecount,  mode, uic1, uic2, linktype; /* Data from header */
int uid, gid, bufferpointer;    /* current values used by create */
FILE *tarfp;                      /* The input file pointer */

/* Global file characteristics */

FILE *vmsfile;
int vmsfd, outfd;
unsigned int vmsmrs, vmstime;       /* maximum record size */
int vmsrat,vmsorg,vmsrfm;           /* other format (as integers) */
char default_name[] = {"*.*;"};     /* only get the most recent version */

/* main -- parses options, dispacthes to tar2vms and vms2tar */

main(argc,argv)
int argc;
char *argv[];
{
char *cp, c;

/* Decode the options and parameters: */

    if(argc ==1)
        usage();
    extract = 0;            /* Defaults for now */
    verbose = 0;
    waitp = 0;               /* Don't wait for prompt */
    binmode = 0;
    create = 0;
    dot = 0;
    foption = 0;
    strcpy(tarfile,"$TAPE");

        cp = argv[1];
        while(c = *cp++)
            {
            switch(c)
            {
            case 't':
                list=1;
                break;
            case 'x':
                extract=1;
                break;
            case 'c':
                create=1;
                break;
            case 'v':
                verbose=1;
                break;
            case 'w':
                waitp=1;
                break;
            case 'd':
                dot=1;
                break;
            case 'b':
                binmode=1;
                break;
            case 'f':
                if (*cp != '\0' || argc < 3)
                    usage();
                else
                    foption = 1;
                    strcpy(tarfile,argv[2]);
                break;
            case '-':
                    break;
            default:
                printf("tar: option '%c' not recognized.\n",c);
                usage();
            }
       }

/* Check options are coherent */

    if (extract + list + create == 0)
    {
        printf("tar: no action specified.\n");
        exit(1);
    }
    if (extract + create == 2)
    {
        printf("tar: incompatible options specified.\n");
        exit(1);
    }

/* Set up directory names - current and top level */

    strcpy(curdev, getenv("PATH"));
    for (cp = curdev; *cp != '\0'; ++cp)
        *cp = toupper(*cp);		/* map to uppercase */

    cp = strchr(curdev, ':');    /* split into device and directory */
    *cp++ = '\0';
    strcpy(curdir, cp);

    strcpy(topdir, curdir);
    cp = strchr(topdir,'.');		/* isolate top level directory */
    if (cp != NULL)
    {
        *cp = ']';
        *++cp = '\0';
    }

    if (create == 0)
        tar2vms(argc,argv);
    else
        vms2tar(argc,argv);
}

/* tar2vms -- handles extract and list options */

tar2vms(argc,argv)
int argc;
char **argv;

{
int status,file_type,j, flag, argi, process;
char *make_directory(), *argp, *ptr;
struct dsc$descriptor pattern, candidate;
FILE *opentar();

/* open the file for reading */

    if((tarfp = opentar()) == NULL)
        {
        printf("tar: error opening tarfile.\n");
        exit(1);
        }

/* Now keep reading headers from this file, and decode the names, etc. */

    while((status=hdr_read(&header))==DSIZE)    /* 0 on end of file */
    {
        process = 0;
        if(strlen(header.title)!=0)     /* Valid header */
        {
            decode_header();
            process = 1;

/* Now if file names were specified on the command line, check if they
   match the current one */

            if ((foption == 0 && argc > 2) || argc > 3)
            {
                process = 0;
                argi = foption ? 3 : 2;
                while (argi < argc)
                {
                    pattern.dsc$w_length = strlen(argv[argi]);
                    pattern.dsc$a_pointer = argv[argi];
                    pattern.dsc$b_dtype = DSC$K_DTYPE_T;
                    pattern.dsc$b_class = DSC$K_CLASS_S;
                    candidate.dsc$w_length = strlen(pathname);
                    candidate.dsc$a_pointer = pathname;
                    candidate.dsc$b_dtype = DSC$K_DTYPE_T;
                    candidate.dsc$b_class = DSC$K_CLASS_S;

                    ++argi;

                    if (STR$MATCH_WILD(&candidate, &pattern) == STR$_MATCH)
                    {
                        process = 1;
                        break;
                    }
                }
            }
        }
        else
        {
             status = 1;
             break;
        }
        if (process && waitp)
        {
            *temp = '\0';
            while (*temp != 'y' && *temp != 'n')
            {
                printf("%s   (y/n) ? ", pathname);
                scanf("%s", temp);
                *temp = tolower(*temp);
            }
            process = (*temp == 'y');
        }
        if (process && extract)
        {
                file_type=scan_title(pathname,new_directory,newfile);
                cleanup_dire(new_directory);
                if( make_new(new_directory)!=0)
                    printf("tar: error creating %s\n",new_directory);
                if(file_type == ISDIRE)
                    {}
                if(file_type == ISFILE)

/*  Now move the data into the output file */

                    if(bytecount>0)
                    {
                        strcpy(outfile,new_directory);
                        strcat(outfile,newfile);
                        copyfile(outfile,bytecount);
                    }
        }
        else
        {
            if (process && list)               /* listing only */
            {
                printf("%6o %8d %s %s\n",
                    mode,bytecount,creation+4,pathname);
                if (linktype == 1 || linktype == 2)
                 printf("                                --->  %s\n",linkname);
            }
            if(linktype == 0)
                tarskip(bytecount);
        }
    }       /* end while  */
    if(status == 1)                     /* Empty header */
    {

/*        printf("Do you wish to move past the EOF mark (y/n) ? ");
**        fflush(stdout);
**        gets(temp);
**        if(tolower(*temp) == 'y')
**            while((status=hdr_read(&header)) >0);
**        else
*/
            exit(SS$_NORMAL);
    }
    if(status==0)                       /* End of tar file  */
    {
        printf("tar: EOF hit on tarfile.\n");
        exit(SS$_NORMAL);
    }
    if(status<0)                        /* An error  */
    {
        printf("tar: error reading tarfile.\n");
        exit(SS$_NORMAL);
    }
}

/* This function simply copies the file to the output, no conversion */

int copyfile(outfile,nbytes)
char outfile[]; /* name of output version */
int nbytes;

{
int inbytes, fil, s;

/*  Open the output file */

    if (binmode)
        fil = creat(outfile,0,"rfm=fix","mrs=512","alq=5");
    else
        fil = creat(outfile,0,"rfm=stmlf","rat=cr");

    if(fil == ERROR1)
    {
        printf("tar: error creating %s \n",outfile);
        tarskip(nbytes);
        return(-2);
    }
    s = 0;
    if(linktype !=0)
    {
        sprintf(buffer,"*** This file is a link to %s\n",linkname);
        s = write(fil,buffer,strlen(temp));
    }
    else
    {
        while(nbytes > 0 && s >= 0)
        {
            if((inbytes=fread(buffer,1,DSIZE,tarfp)) > 0)
            {
                s = write(fil,buffer,(nbytes > DSIZE)? DSIZE:nbytes);
                nbytes -= inbytes;
            }
            else
            {
                printf("tar: EOF hit on input file.\n");
                close(fil);
                return(-1);
            }
        }
    }

/* Close the file */

    close(fil);

    if (s < 0)
    {
        printf("tar: error writing file %s\n",outfile);
	if (nbytes) tarskip(nbytes);
	return(-1);
    }

    if(verbose)
    {
        printf("%s %8d %s\n",creation+4,bytecount,outfile);
        if(linktype!=0)
            printf("                         --> %s\n",linkname);
    }
    return(0);
}

/* scan_title -- decode a Un*x file name into the directory and name */

/* Return a value to indicate if this is a directory name, or another file
* We return the extracted directory string in "dire", and the
* filename (if it exists) in "fname". The full title is in "line"
* at input.
*/

int scan_title(line,dire,fname)
char line[],dire[],fname[];
{
char *end1;
int len,len2,i,ind;
/* The format will be UNIX at input, so we have to scan for the
* UNIX directory separator '/'
* If the name ends with '/' then it is actually a directory name.
* If the directory consists only of '.', then don't add a subdirectory
* The output directory will be a complete file spec, based on the default
* directory.
*/

    strcpy(dire,curdir);                /* Start with the current dir */
    if(strncmp(line,"./",2)==0)
        strcpy(line,line+2);            /* ignore "./" */
    strcpy(temp,line);                  /* Start in local buffer */
    ind=vms_cleanup(temp);              /* Remove illegal vms characters */
    if((end1=strrchr(temp,'/'))==0)     /* No directory at all  ? */
        strcpy(fname,temp);             /* Only a file name */
    else
    {                                   /* End of directory name is '/' */
        *end1 = 0;                      /* Terminate directory name */
        strcpy(fname,end1+1);           /* File name without directory */
        for (i=1;temp[i];i++)           /* Change '/' to '.' in directory */
            if(temp[i]=='/')		/* and '.' to '_' */
                temp[i]='.';
	    else if (temp[i] == '.')
	        temp[i] = '_';
        if (*temp == '/')               /* absolute path ? */
        {
            *temp = '[';		/* yes, build absolute VMS path */
            strcpy(dire,temp);
        }
        else
        {
            dire[strlen(dire)-1] = (*temp=='.')?0:'.' ;
                 /* "." to indicate a subdirectory (unless already there )*/
            strcat(dire,temp);      /* Add on the new directory  */
        }
        strcat(dire,"]") ;              /* And close with ']' */
    }
    if(strlen(fname)==0)        /* Could this cause problems ? */
    {
        return(ISDIRE);
    }
    else
        for(i=0,end1=fname;*end1;end1++) /* Replace multiple . */
            if(*end1 == '.')
                if(i++)*end1 = '_'; /* After the first */
    return(ISFILE);
}

/* make_new -- create a new directory */

int make_new(want)
char want[];
{
int status, created;
char *dotp;

    created = 1;
    status = mkdir(want, 0);   /* mkdir in VAX C creates all missing levels */
    if (status != 0)
    {
        if (errno == EEXIST)
            return (0);
        if (errno != EINVAL)
            return (-1);          /* unknown error, simply return */
        else                      /* maybe too many levels of directories */
        {                         /* change "[...FOO.BAR]" to "[...FOO$BAR]" */
        for (dotp = &want[strlen(want) - 1];dotp > want && status != 0;)
            if (*--dotp == '.')
            {
                *dotp = '$';
                status = mkdir(want, 0);
                if (status != 0 && errno == EEXIST)
                {
                    status = created = 0;
                    break;
                }
            }
        }
    }
    if (status != 0)
        return (-1);
    if(verbose && created)
        printf("                              %s\n",want);
    return(0);
}

 /* Function to open and get data from the blocked input file */
FILE *opentar()
{
FILE *fp;
    fp = fopen(tarfile, "rb");
    return(fp);
}

/* Get the next file header from the input file buffer. We will always
* move to the next 512 byte boundary.
*/
int hdr_read(buffer)
char *buffer;
{
int stat;
    stat = fread(buffer,1,DSIZE,tarfp);    /* read the header */
    return(stat);                       /* catch them next read ? */
}

/* This is supposed to skip over data to get to the desired position */
/* Position is the number of bytes to skip. We should never have to use
* this during data transfers; just during listings. */
int tarskip(bytes)
int bytes;
{
int i=0;
    while(bytes > 0)
        {
        if((i=fread(buffer,1,DSIZE,tarfp)) == 0)
            {
            printf("tar: EOF hit while skipping.\n");
            return(-1);
            }
        bytes -= i;
        }
    return(0);
}

/* Decode the fields of the header */

int decode_header()
{
int idate, *bintim, chksum, value;
char ll, *ptr;
bintim = &idate;
    linktype=0; strcpy(linkname,"");
    strcpy(pathname,header.title);
    sscanf(header.time,"%o",bintim);
    strcpy(creation,ctime(bintim));     /* Work on this! */
    creation[24]=0;
    sscanf(header.count,"%o",&bytecount);
    sscanf(header.protection,"%o",&mode);
    sscanf(header.uid,"%o",&uic1);
    sscanf(header.gid,"%o",&uic2);
    sscanf(header.chksum,"%o",&chksum);

/* Verify checksum */

    for(value = 0, ptr = &header; ptr < &header.chksum; ptr++)
             value += *ptr;                /* make the checksum */
    for(ptr = &header.linkcount; ptr <= &header.dummy[255]; ptr++)
             value += *ptr;
    value += (8 * ' ');	               /* checksum considered as all spaces */

    if (chksum != value)
    {                                       /* abort if incorrect */
        printf("tar: directory checksum error for %s\n",pathname);
        exit(1);
    }

/* We may have the link written as binary or as character:  */

    linktype = isdigit(header.linkcount)?
            (header.linkcount - '0'):header.linkcount;
    if(linktype != 0)
        sscanf(header.linkname,"%s",linkname);
    return(0);
}

/* vms_cleanup -- removes illegal characters from directory and file names
 * Replaces hyphens and commas with underscores. Returns number of translations
 * that were made.
 */

vms_cleanup(string)
char string[];
{
    int i,flag=0;
    char c;

    for(i=0; c=string[i]; ++i)
    {
        if (strchr(badchars, c) != NULL)
        {                    /* Replace illegal characters by underscores */
            string[i]= '_';
            flag++;          /* Record if any changes were made */
        }
        else
            string[i] = toupper(c); /* Map to uppercase */
    }
    return(flag);
}

/* vms2tar -- handles create function */

vms2tar(argc,argv)
int argc;
char **argv;
{
    int argi, process, file_type, absolute;
    char string[NAMSIZE], *cp, *dp, *wp;

    if (argc < 3 || (foption && argc < 4)) usage();

    bufferpointer = 0;
    gid = getgid();
    uid = getuid();
    mode = 0644;

/* Open the output file */

    outfd = creat(tarfile,0600,"rfm=fix","mrs=512");
    if (outfd < 0)
    {
        printf("tar: error opening output tarfile %s\n", tarfile);
        exit(SS$_NORMAL);
    }

    for (argi = foption ? 3 : 2; argi < argc; ++argi)
    {
        strcpy(string, argv[argi]);
        absolute = 0;
        cp = strchr(string, ':');   /* device name in argument ? */
        if (cp != NULL)            /* yes, force absolute mode */
            absolute = 1;
        else
        {
            cp = strchr(string, '[');  /* test argument for absolute... */
            if (cp != NULL)            /* ...or relative filespec */
            {
                ++cp;
                if (*cp != '.' && *cp != '-')
                    absolute = 1;
            }
        }
        if(initsearch(string) <= 0)
            printf("tar: no files matching %s\n",string);
        else
          while(search(newfile,NAMSIZE)!=0)
          {
            strcpy(linkname, newfile);
            cleanup_dire(newfile);
            file_type = scan_name(newfile,new_directory,outfile,absolute);
            strcpy(pathname,new_directory);
            strcat(pathname,outfile);
            if (get_attributes(newfile) != 0) exit(SS$_NORMAL);
            process = 1;
            if(waitp)
            {
                *temp = '\0';
                while (*temp != 'y' && *temp != 'n')
                {
                    printf("%s   (y/n) ? ", linkname);
                    scanf("%s", temp);
                    *temp = tolower(*temp);
                }
                process = (*temp == 'y');
            }
            if (process)
            {
                if(file_type == ISDIRE)
                {
                    bytecount =  0;
                    mode = 0755;
                    fill_header(pathname);
                    write_header(outfd);
                }
                if(file_type == ISFILE)
                {
                    mode = 0644;
                    if(addfile(newfile, pathname) < 0)
                    {
                        printf("tar: error copying %s\n",linkname);
                        exit(SS$_NORMAL);
                    }
                }
                if (bytecount == 0 && file_type == ISFILE)
		{
                    printf("*** skipped  %s\n", linkname);
		    printf("*** empty file or unsupported format\n");
		}
                if (verbose && (bytecount || file_type == ISDIRE))
                {
                     strcpy(creation,ctime(&vmstime)); /* work on this */
                     creation[24]=0;
                     printf("%s %8d %s\n",creation + 4,bytecount,pathname);
                }
            }
          }
    }
    write_trailer(outfd);
    close(outfd);
}

/* addfile - copy the vms file to the output file */

int addfile(vmsname,unixname)
char vmsname[],unixname[];
{
int ind;
    if(bytecount == 0)          /* we don't output null files  */
        return(0);
    if((ind=out_file(vmsname,bytecount,outfd)) < 0)
        return(ind);
    bufferpointer = bufferpointer%BLKSIZE;
    return(1);
}

/* out_file - write out the file.
* move nbytes of data from "fdin" to "fdout";
* Always pad the output to a full DSIZE
* If it a VMS text file, it may be various formats, so we will
* read the file twice in case of a text file
* so that we get the correct byte count.
* We set the bytecount=0 if this is funny file.
*/

int out_file(filename,nbytes,fdout)
char filename[];
int fdout, nbytes;
{
int i, n, pos;
char *bufp, *linep;
FILE *filein;

    if(vmsrfm == FAB$C_FIX || vmsrfm == FAB$C_STM ||
       vmsrfm == FAB$C_STMLF)
    {
    	if((filein = fopen(filename,"rb")) == NULL)
    	{
    		printf("tar: error opening input file %s\n",filename);
        	return(-1);
        }
        fill_header(pathname);          /* We have all of the information */
        write_header(outfd);            /* So write to the output */
        while(nbytes > 0)
        {
            n = fread(buffer, 1, nbytes>DSIZE? DSIZE:nbytes, filein);
            if (n == 0)
            {
                fclose(filein);
                printf("tar: error reading input file %s\n",filename);
                return(-1);
            }
            nbytes -= n;
            flushout(fdout);
        }
        fclose(filein);
        return(0);
    }
    else
        if(vmsrat != 0)                 /* must be a text file  */
        {                               /* compute "Unix" file size */
            if((filein = fopen(filename,"r")) == NULL)
            {
                printf("tar: error opening input file %s\n",filename);
                return(-1);
            }
            nbytes = 0;
            while(fgets(dbuffer,DSIZE,filein) !=  NULL)
                nbytes += strlen(dbuffer);
	    if (!feof(filein))
	    {
                fclose(filein);
                printf("tar: error reading input file %s\n",filename);
                printf("     record too large or incorrect RMS attributes\n");
                return(-1);
            }
            rewind(filein);                 /* Back to the beginning  */
            bytecount = nbytes;             /* Use the real count  */
            fill_header(pathname);          /* Compute the header */
            write_header(outfd);            /* Write it  */
            bufp = buffer;

	    if (nbytes != 0)		    /* Check for empty file */
	    {
                while(fgets(dbuffer,DSIZE,filein) !=  NULL)
                {                           /* Now copy to the output */
                    linep = dbuffer;
                    while (*linep != '\0')  /* read source file line by line */
                    {
                        if (bufp >= &buffer[DSIZE])
                        {
                            bufp = buffer;
                            flushout(fdout); /* if buffer full, flush it */
                        }               /* copy in fixed size output buffer */
                        *bufp++ = *linep++;
		    }
                }
                flushout(fdout);
	    }
            fclose(filein);
        }
        else              /* Other formats e.g. .OBJ are not done  */
        	bytecount = 0;
        return (0);
}

/* flushout - write a fixed size block in output tarfile */

flushout(fdout)
int fdout;
{
    if (write(fdout,buffer,DSIZE) != DSIZE)
    {
        printf("tar: error writing tarfile.\n");
        exit(SS$_NORMAL);
    }
    bufferpointer += DSIZE;
}

/* write_header - copy the header to the output file  */

int write_header(fd)
int fd;
{
int n;
    if((n=write(fd,&header,DSIZE))!=DSIZE)
    {
        printf("tar: error writing header in tarfile.\n");
        exit(SS$_NORMAL);
    }
    bufferpointer += DSIZE;
    return(n);
}

/*  get_attributes - get the file attributes via stat()  */

int get_attributes(fname)
char fname[];
{
    if(stat(fname,&sblock))
    {
        printf("tar: can't get file status on %s\n",fname);
        vmstime = 0;                    /* Prevent garbage printoout */
        bytecount = 0;                  /* of inaccessible files  */
        return(-1);
    }

/* now get the file attributes, we don't use them all */
    bytecount = sblock.st_size;
    vmsrat = sblock.st_fab_rat;
    vmsmrs = sblock.st_fab_mrs;
    vmsrfm = sblock.st_fab_rfm;
    vmstime = sblock.st_mtime;
    return (0);
}

/* write_trailer - write the two blank blocks on the output file */
/* pad the output to a full blocksize if needed. */

write_trailer(fdout)
int fdout;
{
int i;
    for (i=0; i < NAMSIZE; ++i)
        header.title[i] = '\0';
    write_header(fdout);
    write_header(fdout);
    bufferpointer = bufferpointer%BLKSIZE;
    while (bufferpointer < BLKSIZE)
        write_header(fdout);
    return(1);
}

/* scan_name - decode a file name */

/* Decode a file name into the directory, and the name, and convert
* to a valid UNIX pathname. Return  a value to indicate if this is
* a directory name, or another file.
* We return the extracted directory string in "dire", and the
* filename (if it exists) in "fname". The full title is in "line"
* at input.
*/

int scan_name(line,dire,fname,absolute)
char line[],dire[],fname[];
int absolute;
{
char *cp1,*cp2;
int len,len2,i;
char *strindex();

/* The format will be VMS at input, so we have to scan for the
* VMS device separator ':', and also the VMS directory separators
* '[' and ']'.
* If the name ends with '.dir;1' then it is actually a directory name.
* The outputs dire and  fname will be a complete file spec, based on the default
* directory.
* It may be a rooted directory, in which case there will be a "][" string,
* remove it.
* Strip out colons from the right, in case there is a node name (should not be!)
* If the filename ends in a trailing '.', suppress it , unless the "d" option
* is set.
*/

    strcpy(temp,strrchr(line,':')+1);           /* Start with the whole name */

/* If relative path, get rid of default directory part of the name  */

    if (absolute == 0)
    {
        strcpy(dire,"./");
        for(cp1 = temp ,cp2 = curdir; *cp2 && (*cp1 == *cp2);
            cp1++, cp2++);
        if(*cp2 == 0)
            *cp1 = 0;              /* Perfect match, no directory spec  */
        else
        {
            switch(*cp1)
            {
            case '.':
            case '[':               /* Legal beginnings or ends  */
                break;
            default:            /* We are above the default, use full name */
                cp1 = strchr(temp,'[');    /* Fixed this from 1.5  */
                *dire = '\0';
                break;
            }
            cp1++;                 /* Get past the starting . or [ */
        }
        strcat(dire, cp1);
    }
    else
        strcpy(dire, temp + 1);
    cp1 = strrchr(dire, ']');       /* change trailing directory mark */
    if (cp1 != NULL)
    {
        *cp1++ = '/';
        *cp1 = '\0';                /* and terminate string (no file name) */
    }
    strcpy(temp,strrchr(line,']')+1); /* Now get the file name */
    if((cp1=strindex(temp,".DIR;1"))!=0)
    {
        *cp1++ = '/';                          /* Terminate directory name */
        *cp1 = '\0';
        strcat(dire,temp);
        *fname = '\0';
    }
    else
    {
        strcpy(fname,temp);
        *strchr(fname,';') = '\0';;           /* no version numbers */
        lowercase(fname);                     /* map to lowercase */
    }

    /* Now rewrite the directory name  */

   lowercase(dire);

    for (cp1 = dire + 1; *cp1; ++cp1)         /* Change '.' to '/'  */
        if(*cp1 == '.')
            *cp1 = '/';
    if((len=strlen(fname))==0)
        return(ISDIRE);
    else
        if(fname[--len] == '.')
           if (dot == 0)
                fname[len] = 0; /* No trailing dots  */
        return(ISFILE);
}
/* initsearch - parse input file name */
/* To start looking for file names to satisfy the requested input,
* use the sys$parse routine to create a wild-card name block. When
* it returns, we can then use the resultant FAB and NAM blocks on
* successive calls to sys$search() until there are no more files
* that match.
*/

int initsearch(string)
char string[];
{
int status;
char *strindex();

    if(strindex(string,"::")!=NULL)
    {
        printf("tar: DECnet file access is not supported.\n");
        return(-1);
    }
    fblock = cc$rms_fab;
    nblock = cc$rms_nam;
    fblock.fab$l_dna = default_name;
    fblock.fab$b_dns = strlen(default_name);
    fblock.fab$l_fna = string;
    fblock.fab$b_fns = strlen(string);
    fblock.fab$l_nam = &nblock;
    nblock.nam$l_esa = searchname;
    nblock.nam$b_ess = sizeof(searchname);
#ifdef debug
    printf("searching on: %s\n",string);
#endif
    status = sys$parse(&fblock);
    if(status != RMS$_NORMAL)
    {
        if(status == RMS$_DNF)
            printf("tar: directory not found %s\n",searchname);
        else
            printf("tar: error in SYS$PARSE.\n");
        return (-1);
    }
    searchname[nblock.nam$b_esl] = 0;   /* Terminate the string  */

    /* Now reset for searching, pointing to the parsed name block */

    fblock = cc$rms_fab;
    fblock.fab$l_nam = &nblock;
    return(nblock.nam$b_esl); /* return the length of the string  */
}

/* search - get the next file name that matches the namblock */
/* that was set up by the sys$search() function. */

int search(buff,maxlen)
char buff[];
int maxlen;
{
int status;

    nblock.nam$l_rsa = buff;
    nblock.nam$b_rss = maxlen;
    nblock.nam$b_rsl = 0;       /* For next time around  */
    while ((status = sys$search(&fblock)) != RMS$_NMF)
    {
        buff[nblock.nam$b_rsl] = 0;
        if(status == RMS$_NORMAL)
            return(nblock.nam$b_rsl);
        else
        {
            if (status == RMS$_PRV)
                printf("tar: no privilege to access %s\n",buff);
            else if (status == RMS$_FNF)
                printf("tar: file not found %s\n",buff);
            else
            {
                printf("tar: error in SYS$SEARCH for %s\n", buff);
                return (0);
            }
        }
    }
    return (0);
}
/* fill_header - fill the fields of the header */
/* enter with the file name, if the file name is empty, */
/* then this is a trailer, and we should fill it with zeroes. */

int fill_header(name)
char name[];
{
int i,chksum;
int zero = 0, hdrsize = DSIZE;
char *ptr,tem[15];

/* Fill the header with zeroes */

    LIB$MOVC5(&zero, &zero, &zero, &hdrsize, &header);
    if(strlen(name)!=0)         /* only fill if there is a file */
    {
        sprintf(header.title,"%s",name);        /* write file name */
        sprintf(header.protection,"%6o ",mode); /* all written with */
        sprintf(header.uid,"%6o ",uid);         /* a trailing space */
        sprintf(header.gid,"%6o ",gid);
        sprintf(tem,"%11o ",bytecount);         /* except the count */
        strncpy(header.count,tem,12);           /* and the time, which */
        sprintf(tem,"%11o ",vmstime);           /* except the count */
        strncpy(header.time,tem,12);            /* have no null */
        strncpy(header.chksum,"        ",8);    /* all blanks for sum*/

/* I know that the next two are already zero, but do them */

        header.linkcount = 0;                   /* always zero */
        sprintf(header.linkname,"%s","");       /* always blank */
        for(chksum=0, ptr = &header;ptr < &header.linkcount;ptr++)
                 chksum += *ptr;                /* make the checksum */
        sprintf(header.chksum,"%6o",chksum);
    }
    return(0);
}

/* strindex - search for string2 in string1, return address pointer */

char *strindex(string1,string2)
char *string1,*string2;
{
char *c1, *c2, *cp;
    for(c1 = string1; *c1 !=0; c1++)
    {
        cp = c1;        /* save the start address */
        for(c2=string2; *c2 !=0 && *c1 == *c2; c1++,c2++);
           if(*c2 == 0)
                return(cp);
    }
    return(NULL);
}

/* lowercase - function to change a string to lower case */

int lowercase(string)
char string[];
{
int i;
        for(i=0;string[i]=tolower(string[i]);i++);
        return (--i);           /* return string length */
}

/* cleanup_dire - routine to get rid of rooted directory problems */
/* and any others that turn up */

int cleanup_dire(string)
char string[];
{
char *ptr;
        if((ptr=strindex(string,"][")) != NULL)
        {       /* Just collapse around the string */
          strcpy(ptr,ptr+2);
          return(1);
        }
        if((ptr=strindex(string,"[000000.")) != NULL)
        {         /* remove "000000." */
            strcpy(ptr + 1, ptr + 8);
            return(1);
        }
        return(0);
}

/* Syntax error exit */

usage()
{
    printf("usage: tar x|c|t[vbd][f tarfile] [file [file...]]\n");
    exit(1);
}
