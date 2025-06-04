#define module_name  MAILCOUNT
#define module_ident "V1.3"

/*   MAILCOUNT.C

Description: This program determines the location of a user's mail directory,
  tabulates  all MAIL$nnnnnnnnnnnnnnnn.MAI files found and correlates this
  information with the files expected in MAIL.MAI.  A summary report is output
  at the end indicating if any extra files exist, or if any are missing.  It
  does a lot of work, so it can take a while to execute...

Environment:  Works in the mail environment for the current username only.

Privileges required: none

Written:  H. Brydon, December 1991

Disclaimer: Be kind to me - I'm a new C programmer.  I know it looks like
  Fortran.

Modifications:

	V1.3	Hunter Goatley <goathunter@goatley.com>		28-SEP-2000
		Initialize longword variables so we don't get garbage in
		the high word when callable MAIL$ routines return words.
		Some minor edits to make newer versions of DEC C happy.
		Still needs /STANDARD=VAXC; I didn't have time to fix all of
		those issues....

	V1.2	Hunter Goatley <goathunter@ALPHA.WKU.EDU>	June 6, 1995
		Merge in changes from Jerry Leichter, June 1992, to support
		multiple files.

	V1.1	Hunter Goatley <goathunter@ALPHA.WKU.EDU>	June 3, 1995
		Cleaned up code to compile using DEC C with /STANDARD=VAXC.

*/
#ifdef __DECC
#pragma module module_name module_ident
#else
#module module_name module_ident
#endif

#include <descrip.h>
#include <ctype.h>
#include <maildef.h>
#include <fscndef.h>
#include <stdio.h>
#include <string.h>

long status;
#define ckstatus(x) if(((status = x) & 7) != 1) LIB$SIGNAL(status)
#define min(a,b) ((a) < (b) ? (a) : (b))
#define TRUE  (1)
#define FALSE (0)
#define maxmsgs 500000 /***/

typedef struct _ItemList
{
  short buffer_length;
  short item_code;
  void *buffer_address;
  long *return_length_address;
} ItemList;

typedef struct _minfo     /* Structure to hold message info */
{ char name[26];          /* Name of external message */
  unsigned ptrfound : 1;  /* Ptr to message identified */
  unsigned msgfound : 1;  /* Actual message file has been located */
} Minfo;

Minfo msgs[maxmsgs];
int msgctr = -1;

char mfds[255];
int  mfdslen;


/*
 add_msgid scans the Minfo data and adds a given message name if
 not already there, or updates the information if previously entered
*/
unsigned add_msgid(char *msgid, unsigned pf, unsigned mf)
{
  int i;
  char *p;
  if(msgid[0] == 0)
  {
    fprintf(stderr,
"\nMAILCOUNT-W: Attempt to enter null string\n");
  }
  if (++msgctr >= maxmsgs)
  {
    fprintf(stderr,
"\nMAILCOUNT-F: Number of messages exceeds maxmsgs(%d) - recompile required\n",
            maxmsgs);
    exit(2);
  }
  if((int) strlen(msgid) > sizeof msgs[msgctr].name)
  {
    fprintf(stderr,
"\nMAILCOUNT-W: Size of message name exceeds data field size\n");
    fprintf(stderr, "Message file name: >%s<", msgid);
  }

  /* Strip the ".MAI" extension and anything after it... */
  for(p=msgid; p<=msgid + sizeof msgs[msgctr].name; p++)
  if(*p == ".") *p = 0;

  strcpy(msgs[msgctr].name, msgid);
  for(i=0; i<= msgctr; i++)
  if(!strcmp(msgs[i].name, msgid))
  {
    msgs[i].ptrfound |= pf;
    msgs[i].msgfound |= mf;
    if (i == msgctr)
    {
      return 1;
    }
    else
    {
      msgctr--;
      return 0;
    }
  }
}


int folder_routine(int *pfctx, struct dsc$descriptor *name)
{
  static int folders = 0;
  int mcount;
  int mcountl;
  short rectype;
  char subject[510], extid[255];
  int subjlen = 0, extid_len = 0;
  int mcontext =0;
  ItemList
    null_list[] = {{0,0,0,0}},
    mslist[] = {
                {0, MAIL$_MESSAGE_FOLDER, 0, 0},
                {0, MAIL$_NOSIGNAL, 0, 0},
                {0,0,0,0}},
    mblist[] = {{4,MAIL$_MESSAGE_FILE_CTX,pfctx,0},
                {0,0,0,0}},
    milist[] = {{0,MAIL$_MESSAGE_NEXT,0,0},
                {0,MAIL$_NOSIGNAL,0,0},
                {0,0,0,0}},
    molist[] = {{sizeof subject, MAIL$_MESSAGE_SUBJECT, subject, &subjlen},
                {sizeof extid, MAIL$_MESSAGE_EXTID, extid, &extid_len},
                {0,0,0,0}};

  if (name->dsc$w_length)
  {
    ckstatus(mail$message_begin(&mcontext, mblist, null_list));

    mslist[0].buffer_length = name->dsc$w_length;
    mslist[0].buffer_address = name->dsc$a_pointer;
    if (mail$message_select(&mcontext, mslist, null_list) & 1)
    {
      printf("  Scanning folder %.*s...",
             name->dsc$w_length, name->dsc$a_pointer);
      fflush(stdout);
      ++folders;
      mcount = 0;
      mcountl = 0;

      while (mail$message_info(&mcontext, milist, molist) & 1)
      {
	mcount++;
        subject[subjlen] = 0;
        extid[extid_len] = 0;
        if(extid_len)
        {
/*        printf("\n   Message %.*s found, subj=%.*s",  /**/
/*                extid_len, extid, min(subjlen, 50), subject);  /**/
	  mcountl++;
          add_msgid(extid, TRUE, FALSE);
        }
      }
      printf("%d message%s, %d with external files\n",
		mcount,(mcount == 1)?"":"s",mcountl);
      ckstatus(mail$message_end(&mcontext, null_list, null_list));
    }
  }
  else
  { /* no more folders, print out the total! */
    printf("\n Total of %d folder%s processed\n",
           folders,(folders == 1)?"":"s");
  }
  return 1;
}


/*
  get_names_from_mail_mai collects the names of all valid external
  MAIL$nnnnnnnnnnnnnnnn.MAI files from the user's mail.mai file.
*/
unsigned get_names_from_mail_mai(char mdir[],char mnam[])
{
  int sts;
  int mcontext=0;
  int fcontext=0;
  ItemList
    null_list[] = {{0,0,0,0}},
    mblist[] = {{4,MAIL$_MESSAGE_FILE_CTX,&fcontext,0}, {0,0,0,0}},
    mflist[] = {{4,MAIL$_MAILFILE_FOLDER_ROUTINE,folder_routine,0},
                {4,MAIL$_MAILFILE_USER_DATA,&fcontext,0},
                {0,0,0,0}},
    mnlist[] = {{strlen(mdir),MAIL$_MAILFILE_DEFAULT_NAME,mdir,0}, 
	      {strlen(mnam),MAIL$_MAILFILE_NAME,mnam,0},
              {0,0,0,0}};

  if (mail$mailfile_begin(&fcontext, null_list, null_list) & 1)
  {
    if (mail$mailfile_open(&fcontext, mnlist, null_list) & 1)
    {
      ckstatus(mail$mailfile_info_file(&fcontext, mflist, null_list));
      mail$mailfile_close(&fcontext, null_list, null_list);
    }
    mail$mailfile_end(&fcontext, null_list, null_list);
  }
  printf(
 "\n Total of %d external message%s found following processing of file\n   %s%s\n",
         msgctr+1,(msgctr==0)?"":"s",mdir,mnam);

  return(1);

}


/*
  get_names_from_dir collects the names of all existing external
  MAIL$nnnnnnnnnnnnnnnn.MAI files from the user's mail [sub]directory.
*/
unsigned get_names_from_dir(char mdir[])
{
  int i;
  int ffcontext;
  char *filext;
  char rfs[200];
  struct dsc$descriptor rfs_desc =
    {sizeof rfs, DSC$K_DTYPE_T, DSC$K_CLASS_S, rfs};
  struct dsc$descriptor dfs_desc =
    {strlen(mdir), DSC$K_DTYPE_T, DSC$K_CLASS_S, mdir};
  $DESCRIPTOR(filespec_desc, "MAIL$*.MAI");

  ffcontext = 0;
  while(lib$find_file(&filespec_desc, &rfs_desc, &ffcontext, &dfs_desc, 0,
                      0, 0) & 1)
  {
/*
  rfs now has device, directory, name, extension and version info in it.  We
  need to locate the name.ext info only and pass to add_msgid.
*/
    filext = &rfs[sizeof rfs];
    for(i = 0; i <= sizeof rfs; i++)
    {	if(rfs[i] == ';')
	    {
	      rfs[i] = 0;
	      filext = &rfs[i];
	      break;
	    }
    }
#if 1			/* add_msgid comparisons won't work otherwise! */
    while(filext > &rfs)
	if( *--filext == ']')
	{
	      filext++;
	      break;
	}
#else
    filext = rfs;
#endif
    add_msgid(filext, FALSE, TRUE);
  }
  printf(
"\n Total of %d external message%s found following processing of directory\n   %s\n",
         msgctr+1,(msgctr==0)?"":"s",mdir);
  return(1);
}


/*
  make_report outputs the list of all MAIL$nnnnnnnnnnnnnnnn.MAI files that
  either exist but have no pointer referencing them or do not exist but a
  dangling pointer assumes they are present.
*/
unsigned make_report()
{
  int i;

  for(i=0; i<= msgctr; i++)
  {
    if ((!msgs[i].ptrfound) && msgs[i].msgfound)
    {
    	printf("\nNo mail file pointer exists for external file %s",
               msgs[i].name);
    }
    else if ((msgs[i].ptrfound) && (!msgs[i].msgfound))
    {
    	printf("\nPointer exists but no external file %s found",
               msgs[i].name);
    }
  }
  return(1);
}


main (int argc, char *argv[])
{
  int ucontext;
  ItemList
    null_list[] = {{0,0,0,0}},
    info_list[] = {
      {sizeof mfds, MAIL$_USER_FULL_DIRECTORY, &mfds, &mfdslen},
      {0,0,0,0}};
  char *default_argv[] = {"dummy", "#", NULL};
  char *mdir;
  char *mnam;
  int i;
  char *p;

  argc = getredirection(argc,argv);

  ucontext = 0;
  ckstatus(mail$user_begin(&ucontext, null_list, info_list));
  mfds[mfdslen] = 0;

  if (argc == 1)
  {	argc = 2;
	argv = default_argv;
  }

  for (i = 1; i < argc; i++)
  {	if (*argv[i] == '#')
	{	mdir = mfds;
		mnam = "MAIL.MAI";
	}
	else
	{	mdir = fparse(argv[i],"MAIL.MAI",NULL,"dev,dir");
		mnam = fparse(argv[i],"MAIL.MAI",NULL,"nam,typ");
	}

	printf("\nScanning folders in mail file %s%s\n", mdir, mnam);
	get_names_from_mail_mai(mdir,mnam);
	printf("\nScanning directory %s%s\n", mdir, mnam);
	get_names_from_dir(mdir);
  }
  make_report();
}
