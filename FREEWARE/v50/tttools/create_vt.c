/*
*
* ****************************************************************************
**									    *
**  COPYRIGHT (c) 1989 BY    						    *
**  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
**  ALL RIGHTS RESERVED.						    *
** 									    *
**  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
**  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
**  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
**  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
**  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
**  TRANSFERRED.							    *
** 									    *
**  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
**  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
**  CORPORATION.							    *
** 									    *
**  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
**  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
** 									    *
**									    *
*****************************************************************************
*
* ++
* FACILITY:
*
*	None
*
* ABSTRACT:
*
*	Simple test program to drive the create_vt code.
*
*    	This will create a virtual terminal and link it to another 
*       terminal.  Using this, you could end up with virtual terminals 
*       connected to your DECterms.  I do not even remeber why I wrote 
*       it, but if you can find a use for it, enjoy...
* Notes:
*
*	1) This program needs CHMK, PSWAPM, SYSPRV privileges to run.
*
* AUTHOR: Forrest A. Kenney 	24-November-1989
*
* Revision history:
*
*	X-nn	XXXnnn 		Xxxxxxx X. Xxxxxx		DD-MMM-YYYY
*		Description of change
*
* --
*/

/* Include system definitions */

#include	descrip
#include	prcdef
#include	ssdef


#define		MAXINPUT	1024


/* Main routine */
main()
{

$DESCRIPTOR(phydev,"");
$DESCRIPTOR(logdev,"           ");
$DESCRIPTOR(image,"SYS$SYSTEM:LOGINOUT.EXE");
$DESCRIPTOR(prcnam,"LOGIN");

char		in_buff[MAXINPUT];
short	int	channel;
int		status;
int		unit;


int		create_vt();


/* Setup descriptor */
phydev.dsc$a_pointer = in_buff;
phydev.dsc$w_length = MAXINPUT;


/* Get device to link VT to */
printf("Enter device to link VT to: ");
scanf("%s", phydev.dsc$a_pointer);

phydev.dsc$w_length = (short int)strlen(in_buff);

/* Assign channel*/
status = SYS$ASSIGN(&phydev, &channel, 0, 0);

if (status & SS$_NORMAL)
{
   status = create_vt(channel, &unit);
   if (status & SS$_NORMAL)
   {
      logdev.dsc$w_length = (short int)sprintf(logdev.dsc$a_pointer, "_VTA%d:", unit);
      status = SYS$CREPRC(0, &image, &logdev, &logdev, &logdev, 0, 0, &prcnam,
			  0, 0, 0, PRC$M_INTER|PRC$M_DETACH);
   }
}

return(status);

}
