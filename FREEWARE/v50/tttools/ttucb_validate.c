
/*
 ****************************************************************************
 *									    *
 *  COPYRIGHT (c) 1988                                 			    *
 *  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
 *  ALL RIGHTS RESERVED.						    *
 * 									    *
 *  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
 *  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
 *  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
 *  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
 *  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
 *  TRANSFERRED.							    *
 * 									    *
 *  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
 *  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
 *  CORPORATION.							    *
 * 									    *
 *  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
 *  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
 * 									    *
 *									    *
 ****************************************************************************
 
 ++
FACILITY:
 
 	Unknown
 
ABSTRACT:
 
 	This program will build and load a piece of code into nonpaged pool that
verifies that the terminal pointers in TT UCB's are still valid.  The program 
will crash the system with a TTDRVR1 bugcheck if an invalid set of pointers are
found.  The program comes with a list of two acceptable terminal MID drivers, 
and allows the user to add more.  The program requires that all MID drivers be
loaded before it is run.  Failure to do so will result in an error and the user
will be given to opportunity to exit the program.  An additional capability has
been added that allows the verification to be disabled.  The only valid reason to 
do this would be to add another MID driver to the list.  To use this program a 
user must have CMKRNL priviledge, and PSWAPM priv.
 
	This code was written to detect if a terminal had the port and class 
vectors replaced.  It was originally written to detect if the WATCH program 
was being used on a workstation to break into a cluster.  If it detects that 
the vectors have been replaced it crashes the system.  Before running it on 
anything but a standalone system I would strongly suggest changing this to 
something less severe.  It will allow the TFF code, and the TDSMP code to 
work and will allow you to add other similar device drivers to the list of 
approved mid drivers.


AUTHOR:
 
 	F. Kenney		12-December-1988
 
Revision history:
 
 	X-XX	XXXnnn		Xxxxxxx X. Xxxxxx	DD-MMMMMMM-YYYY
 		Explain change
 
--
*/

/* Constant definitions */

#define		DRIVER_TABLE_LEN	12
#define		DRIVER_NAME_LEN 	12


/* Included files */

#include	ctype
#include	ssdef
#include	stdio


/* Global variables */
char		driver_names[DRIVER_TABLE_LEN][DRIVER_NAME_LEN+1];


short int	driver_name_lens[DRIVER_TABLE_LEN];

int		driver_entries;

struct
{
int		start;
int		end;
}	driver_table[DRIVER_TABLE_LEN];


/* Main program */
validate_tt_vec()
{
char		resp;
char		trash;

int		*disable_tt_watching();
int		*enable_tt_watching();
int		get_driver_names();
int		lock_code(); 
int		status;

struct
{
short int	num_args;
short int	unused; 
int		num_entries;
int		*driver_table;
}	arglist = {2, 0, 0, &driver_table[0]};


status = lock_code();
if ((status & SS$_NORMAL) != SS$_NORMAL) LIB$SIGNAL(status);


/* Fill in known driver names and figure out what to do */
do
{
   driver_name_lens[0] = (short int)sprintf(&driver_names[0][0],"%s", "FBDRIVER");
   driver_name_lens[1] = (short int)sprintf(&driver_names[1][0],"%s", "TDDRIVER");

   driver_entries = 2;
   printf("\n\n\nEnter E to enable, D to diable TT vector checking, or CTRL-Z to exit: ");
   status = scanf("%c", &resp);

   if (status != EOF)
   {
      do
      {
	 trash = (char)getchar();
      } while (trash != '\n');

      resp = _toupper(resp);
      if (resp == 'E')
      {
	 status = get_driver_names();
         if (status & 1) 
         {
	    arglist.num_entries = driver_entries;
	    status = SYS$CMKRNL(&enable_tt_watching, &arglist);
	    if (status == SS$_DEVACTIVE)
	    {
	      printf("\nYou must disable the current watching code before enabling new watching code.\n");
	    }
            else if ((status & SS$_NORMAL) != SS$_NORMAL) LIB$SIGNAL(status);
         }
      }
      else if (resp == 'D') 
      {
	 status = SYS$CMKRNL(&disable_tt_watching, 0);
	 if (status == SS$_NOTINSTALL)
	 {
	   printf("\nTT Watching code is not enabled...\n");
	 }
         else if ((status & SS$_NORMAL) != SS$_NORMAL) LIB$SIGNAL(status);
      }
      else
      {
	 printf("\nIllegal command try again!\n");
      }  
   }
} while (!feof(stdin));

printf("\nUser requested EXIT!.");
return(1);

}

/* Get an additional MID driver names and build fill in "driver_table" */
int get_driver_names()
{
char		buffer[DRIVER_NAME_LEN];
char		resp;
char		trash;

int		actual_driver_entries = 0;
int		*get_driver_addresses();
int		i;
int		status;

struct
{
short int	arg_count;
short int	unused;
int		entries;
int		*names;
int		*lens;
int		*table;
}	arglist = {4, 0, 0, &driver_names[0][0], &driver_name_lens[0], &driver_table[0]};


/* Print out provided MID driver names */
printf("\n\nThe following MID drivers are provided:\n");
for (i=0; i<driver_entries; i++)
{
   printf("\t%s\n", &driver_names[i][0]);
}

/* Now find out if they want more driver names */
printf("\nDo you want to add more mid drivers (y/n): ");
status = scanf("%c", &resp);
if (status != EOF)
{
   do
   {
      trash = (char)getchar();
   } while (trash != '\n');

   resp = _toupper(resp);
   if (resp == 'Y')
   {
      /* Now get additional driver names */
      do
      {
	 printf("Enter device name maximum of 12 characers (<cr> to end list): ");
         for (i=0; i<DRIVER_NAME_LEN; i++)
         {
            buffer[i] = (char)getchar();
            if ((buffer[i] == '\n') || (feof(stdin)))
            {
               buffer[i] = (char)0;
               break;
            }
         }

         if ((!feof(stdin)) && (buffer[0] != 0))
         {
	    if (buffer[0] == '\n') break;
            for (i=0; i<strlen(buffer); i++) buffer[i] = _toupper(buffer[i]);
	    driver_name_lens[driver_entries] = (short int)sprintf(&driver_names[driver_entries][0], "%s", buffer);
	    driver_entries += 1;
         }
         else
         {
            break;
         }
      } while (driver_entries < DRIVER_TABLE_LEN);
   }

   /* Get the driver addresse and fill in table of drivers */

   arglist.entries = driver_entries;
   status = SYS$CMKRNL (&get_driver_addresses, &arglist);
   if ((status & SS$_NORMAL) != SS$_NORMAL) LIB$SIGNAL(status);


   /* Now actually display drivers and idicate if we found an entry for them */
   printf("\n\nThe list belows tells the status of the drivers you requested:");
   for (i=0; i<driver_entries; i++)
   {
      if (driver_name_lens[i] != 0)
      {
	actual_driver_entries += 1;
        printf("\n\t%s is loaded", &driver_names[i][0]);
      }
      else
      {
        printf("\n\t%s is not loaded", &driver_names[i][0]);
      }
   }

   /* See if they want to use this set of drivers */
   printf("\n\nDo you want to use these driver (y/n): ");
   status = scanf("%c", &resp);
   if (status != EOF)
   {
      do
      {
         trash = (char)getchar();
      } while (trash != '\n');

      resp = _toupper(resp);
      if (resp == 'Y')
      {
	driver_entries = actual_driver_entries;
	return(1);
      }
   }
}	    

driver_entries = 0;
return(0);

}
    
