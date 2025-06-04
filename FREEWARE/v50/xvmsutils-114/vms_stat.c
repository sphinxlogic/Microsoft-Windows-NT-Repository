/*
 *	VMS_STAT
 *
 *	Author:		Patrick L. Mahan
 *	Date:		07-Feb-1992
 *	Location:	TGV, Incorporated
 *
 *	Purpose:	Provides for enhanced functionality of the
 *			C RTL stat call to recognize that files
 *			of dev:[directory] are valid directories.
 *			(This is my assumtion at least ;-))
 *
 *	Modification History
 *
 *	Date        | Who	| Version	| Reason
 *	------------+-----------+---------------+---------------------------
 *	07-Feb-1992 | PLM	| 1.0		| First Write
 */

#include <stat.h>
#include <string.h>
#include "unix_types.h"

int vms_stat(file, st)
char *file;
stat_t *st;
{
   char buffer[MAXPATHLEN];
   char *ep;

   /* see if this is a directory path */

   ep = strrchr(file, ']');
   ep++;
   if (*ep != '\0')
   {
	return (stat(file, st));
   }
   else	/* this is a directory specification */
   {
      /* save it off into temp for working */

      strcpy (buffer, file);

      /* find the last '.' */

      ep = strrchr(buffer, '.');
      if (ep != NULL)
	*ep = ']';
      else
      {
	/* assume that we are at the top, subsitute [000000] for this */

	char tmpbuf[MAXPATHLEN];

	ep = strchr(buffer, ':');
	ep++;	*ep = '\0';
	strcpy(tmpbuf, buffer);
	strcat(tmpbuf, "[000000]");
	ep++;
	strcat(tmpbuf, ep);
	strcpy(buffer, tmpbuf);
      }

      /* find the rightmost ']' */

      ep = strrchr(buffer, ']');
      if (ep != NULL) *ep = '\0';

      /* add on the '.dir' */

      strcat(buffer, ".dir");

      /* return the stats */

      return (stat(buffer, st));
   }
}

