#module INSTALL_PROCESS "V3.07"

/*
   Process the output of an $ INSTALL /LIST /FULL command, and puts the
   information in a format where it can be sorted, reported, etc.

   Bart Z. Lederman	13-Jul-1992
*/

#include STDIO
#include SSDEF
#include DESCRIP
#include RMS
#include "BAS$EDIT.H"			/* use local definitions	*/

globalvalue STR$_NOELEM;

struct FAB in_fab;	/* input file RMS structures			*/
struct RAB in_rab;

struct FAB out_fab;	/* output file RMS structures			*/
struct RAB out_rab;

struct {
    char directory[64];
    char image[32];
    char open;
    char hdr;
    char shar;
    char prv;
    char prot;
    char lnkbl;
    char cmode;
    char nopurg;
    char acnt;
    char entry[6];
    char max_shared[6];
    char global[6];
} out_rec = {
    "                                                                ",
    "                                ",
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    "000000", "000000", "000000"};

void write_record ()
{
    int status;

/* Check to see if there is an output record waiting to be written.	*/

    if (out_rec.image[0] != ' ')	/* if not blank, there is	*/
    {					/* information waiting		*/
	status = SYS$PUT (&out_rab);	/* write previous record	*/

	if (status != RMS$_NORMAL)
	{
	    printf ("\n Output file write error.");
	    LIB$SIGNAL (out_rab.rab$l_sts, out_rab.rab$l_stv);
	};

	out_rec.open	    = ' ';	/* clear out old fields		*/
	out_rec.hdr	    = ' ';	/* except directory		*/
	out_rec.shar	    = ' ';
	out_rec.prv	    = ' ';
	out_rec.prot	    = ' ';
	out_rec.lnkbl	    = ' ';
	out_rec.cmode	    = ' ';
	out_rec.nopurg	    = ' ';
	out_rec.acnt	    = ' ';
	strcpy (out_rec.entry, "000000");
	strcpy (out_rec.max_shared, "000000");
	strcpy (out_rec.global, "000000");

	strcpy (out_rec.image, "                               ");
	out_rec.image[31] = ' ';
    };

    return;
}

install_process ()
MAIN_PROGRAM
{
    int status, i, j;

    char in_buf[255], out_buf[255];	/* record buffers		*/
    char temp_str[16], right_str[6];	/* to zero-fill numbers		*/

    struct dsc$descriptor_d in_d =	/* descriptor for processing	*/
	{0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
    struct dsc$descriptor_d out_d =	/* descriptor for processing	*/
	{0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};

    $DESCRIPTOR (infile_d,  "INSTALL_PROCESS.LOG");
    $DESCRIPTOR (outfile_d, "INSTALL_PROCESS.SEQ");
    $DESCRIPTOR (equal_d,   "=");
    $DESCRIPTOR (slash_d,   "/");

/* Program starts here							*/

    in_fab = cc$rms_fab;	/* input file RMS structures		*/
    in_rab = cc$rms_rab;

    out_fab = cc$rms_fab;	/* output file RMS structures		*/
    out_rab = cc$rms_rab;

    in_fab.fab$l_fna = infile_d.dsc$a_pointer;	/* file name		*/
    in_fab.fab$b_fns = infile_d.dsc$w_length;	/* size of file name	*/
    in_fab.fab$b_fac = FAB$M_GET;		/* read only		*/

    in_rab.rab$b_rac = RAB$C_SEQ;	/* sequential access		*/
    in_rab.rab$l_fab = &in_fab;
    in_rab.rab$l_ubf = &in_buf;		/* input record buffer		*/
    in_rab.rab$w_usz = 255;		/* record size			*/
    in_rab.rab$l_rop = RAB$M_RRL |	/* read regardless of lock	*/
		       RAB$M_NLK |	/* don't lock record on read	*/
		       RAB$M_RAH |	/* read ahead			*/
		       RAB$M_WAT;	/* wait if locked (?)		*/

    status = SYS$OPEN (&in_fab);

    if (status != RMS$_NORMAL)
    {
	printf ("\n Input file open error.");
	LIB$SIGNAL (in_fab.fab$l_sts, in_fab.fab$l_stv);
    };

    status = SYS$CONNECT (&in_rab);

    if (status != RMS$_NORMAL)
    {
	printf ("\n Input file connect error.");
	LIB$SIGNAL (in_rab.rab$l_sts, in_rab.rab$l_stv);
    };

    out_fab.fab$l_fna = outfile_d.dsc$a_pointer; /* file name		*/
    out_fab.fab$b_fns = outfile_d.dsc$w_length;	/* size of file name	*/
    out_fab.fab$b_fac = FAB$M_PUT;		/* write only		*/
    out_fab.fab$b_org = FAB$C_SEQ;		/* organization		*/
    out_fab.fab$b_rfm = FAB$C_VAR;		/* record format	*/
    out_fab.fab$b_rat = FAB$M_CR;		/* carriage control	*/
    out_fab.fab$w_mrs = 255;			/* record size		*/
    out_fab.fab$l_fop = FAB$M_TEF;	/* Truncate at End of File	*/

    out_rab.rab$l_fab = &out_fab;
    out_rab.rab$b_rac = RAB$C_SEQ;		/* sequential access	*/
    out_rab.rab$l_rop = RAB$M_WBH;		/* write behind		*/
    out_rab.rab$l_rbf = &out_rec;		/* record buffer	*/
    out_rab.rab$w_rsz = sizeof (out_rec);	/* record size		*/

    status = SYS$CREATE (&out_fab, 0, 0);

    if (status != RMS$_NORMAL)
    {
	printf ("\n Output file open error.");
	LIB$SIGNAL (out_fab.fab$l_sts, out_fab.fab$l_stv);
    };

    status = SYS$CONNECT (&out_rab);

    if (status != RMS$_NORMAL)
    {
	printf ("\n Output file connect error.");
	LIB$SIGNAL (out_rab.rab$l_sts, out_rab.rab$l_stv);
    };

/* Read records from the file until end of file.			*/

READ_LOOP:

    status = SYS$GET (&in_rab);

    if (status == RMS$_EOF) goto END_OF_FILE;

    if (status != RMS$_NORMAL)			/* if there is an error	*/
    {
	printf ("\n Error reading input file.");
	LIB$SIGNAL (in_rab.rab$l_sts, in_rab.rab$l_stv);
    }

    if (in_rab.rab$w_rsz == 0)			/* skip zero length	*/
    {						/* records		*/
	goto READ_LOOP;
    };

/* Put the record into a string descriptor.				*/

    status = STR$COPY_R (&in_d, &in_rab.rab$w_rsz, &in_buf);

    if ((status & 1) != 1)
    {
	LIB$SIGNAL (status);
    };

/* Call an RTL routine which will compress the line in one stroke.	*/

    BAS$EDIT (&out_d, &in_d, (BAS$M_DISFORM | BAS$M_DISLEAD |
				BAS$M_COMPRESS | BAS$M_DISTRAIL) );

/* Make a copy of the processed line into a C string.			*/

    strncpy (out_buf, out_d.dsc$a_pointer, out_d.dsc$w_length);

    out_buf[out_d.dsc$w_length] = '\0';	/* strings must end in null.	*/

    status = strncmp (out_buf, "$", 1);	/* start of DCL command?	*/

    if (status == 0)			/* yes				*/
    {
	goto READ_LOOP;			/* skip this			*/
    };

    status = strncmp (out_buf, "%", 1);	/* start of error message?	*/

    if (status == 0)			/* yes				*/
    {
	goto READ_LOOP;			/* skip this			*/
    };

    status = strncmp (out_buf, "Privileges", 10);   /* list of privs?	*/

    if (status == 0)			/* skip this line		*/
    {
	goto READ_LOOP;
    };

    status = strncmp (out_buf, "DISK$", 5);	/* start of new		*/
						/* directory?		*/
    if (status == 0)				/* yes			*/
    {
	write_record ();

	for (i = 0;  i < in_rab.rab$w_rsz;  i++)	/* copy		*/
	{						/* directory	*/
	    out_rec.directory[i] = in_buf[i];
	};

	for (j = i;  j < sizeof (out_rec.directory);  j ++)
	{
	    out_rec.directory[j] = ' ';		/* blank fill		*/
	};

	goto READ_LOOP;
    };

    status = strstr (out_buf, ";");	/* semi-colon delimits file	*/

    if (status != 0)
    {
	write_record ();

	for (i = 0;  i < sizeof (out_rec.image);  i++)	/* copy image	*/
	{
	    if (out_buf[i] == ';') break;
	    out_rec.image[i] = out_buf[i];
	};

/* *** Image should already be blank filled by write_record ()

	for (j = i;  j < sizeof (out_rec.image);  j ++)
	{
	    out_rec.image[j] = ' ';
	};
*** */

	goto CHECK_FLAGS;
    };

/* Flags can continue on the next line.					*/

    status = strncmp (in_buf, "                    ", 20);

    if (status == 0)
    {
CHECK_FLAGS:

	status = strstr (out_buf, "Open");

	if (status != 0)
	{
	    out_rec.open = 'O';
	};

	status = strstr (out_buf, "Hdr");

	if (status != 0)
	{
	    out_rec.hdr = 'H';
	};

	status = strstr (out_buf, "Shar");

	if (status != 0)
	{
	    out_rec.shar = 'S';
	};

	status = strstr (out_buf, "Prv");

	if (status != 0)
	{
	    out_rec.prv = 'V';
	};

	status = strstr (out_buf, "Prot");

	if (status != 0)
	{
	    out_rec.prot = 'T';
	};

	status = strstr (out_buf, "Lnkbl");

	if (status != 0)
	{
	    out_rec.lnkbl = 'L';
	};

	status = strstr (out_buf, "Cmode");

	if (status != 0)
	{
	    out_rec.cmode = 'C';
	};

	status = strstr (out_buf, "Nopurg");

	if (status != 0)
	{
	    out_rec.nopurg = 'N';
	};

	status = strstr (out_buf, "Acnt");

	if (status != 0)
	{
	    out_rec.acnt = 'A';
	};

        goto READ_LOOP;
    };

    if ( (strncmp (out_buf, "Entry",	5) != 0) &&	/* is this a	*/
	 (strncmp (out_buf, "Current",	7) != 0) &&	/* record we	*/
	 (strncmp (out_buf, "Global",	6) != 0) )	/* want?	*/
    {
	goto READ_LOOP;		/* not a known record type		*/
    };

/* Only numeric fields left. Collapse the line.				*/

    BAS$EDIT (&out_d, &out_d, BAS$M_COLLAPSE);

/* Extract the numeric portion (re-using input descriptor).		*/

    status = STR$ELEMENT (&in_d, &1, &equal_d, &out_d);

    if (status == STR$_NOELEM)		/* this shouldn't happen	*/
    {
	LIB$PUT_OUTPUT (&in_d);
	goto READ_LOOP;
    };

    if ((status & 1) != 1)
    {
	LIB$SIGNAL (status);
    };

    strncpy (temp_str, in_d.dsc$a_pointer, in_d.dsc$w_length);

    temp_str[in_d.dsc$w_length] = '\0';

/* If Shared count, want maximum. skip over the first number.		*/

    status = strncmp (out_buf, "Current", 7);

    if (status == 0)
    {
	status = STR$ELEMENT (&out_d, &1, &slash_d, &in_d);

	if (status == STR$_NOELEM)
	{
	    LIB$PUT_OUTPUT (&in_d);
	    goto READ_LOOP;
	};

	if ((status & 1) != 1)
	{
	    LIB$SIGNAL (status);
	};

	strncpy (temp_str, out_d.dsc$a_pointer, out_d.dsc$w_length);

	temp_str[out_d.dsc$w_length] = '\0';
    };

    i = atol (temp_str);	/* convert from string to number	*/

    sprintf (right_str, "%6d", i);	/* convert back to string,	*/
					/* right justified.		*/
    for (i = 0;  i < 6;  i++)		/* stick in leading zeroes	*/
    {
    	if (right_str[i] == ' ') right_str[i] = '0';
    };

/* put the number in the correct field.					*/

    status = strncmp (out_buf, "Entry", 5);

    if (status == 0)
    {
	strncpy (out_rec.entry, right_str, 6);
	goto READ_LOOP;
    };

    status = strncmp (out_buf, "Global", 6);

    if (status == 0)
    {
	strncpy (out_rec.global, right_str, 6);
	goto READ_LOOP;
    };

    status = strncmp (out_buf, "Current", 7);

    if (status == 0)
    {
	strncpy (out_rec.max_shared, right_str, 6);
	goto READ_LOOP;
    };

    printf ("\n error in loop\n");

    goto READ_LOOP;	/* end of loop reading file until EOF		*/

END_OF_FILE:

/* Disconnect record stream and close input file.			*/

    status = SYS$DISCONNECT (&in_rab);

    if (status != RMS$_NORMAL)
    {
	printf ("\n Error disconnecting input file.");
	LIB$SIGNAL (in_rab.rab$l_sts, in_rab.rab$l_stv);
    };

    status = SYS$CLOSE (&in_fab); 

    if (status != RMS$_NORMAL)
    {
	printf ("\n Error closing input file.");
	LIB$SIGNAL (in_fab.fab$l_sts, in_fab.fab$l_stv);
    };

/* Disconnect and close the output file.				*/

    status = SYS$DISCONNECT (&out_rab);

    if (status != RMS$_NORMAL)
    {
	printf ("\n Error disconnecting output file.");
	LIB$SIGNAL (out_rab.rab$l_sts, out_rab.rab$l_stv);
    };

    status = SYS$CLOSE (&out_fab); 

    if (status != RMS$_NORMAL)
    {
	printf ("\n Error closing output file.");
	LIB$SIGNAL (out_fab.fab$l_sts, out_fab.fab$l_stv);
    };

    exit (status);
}
