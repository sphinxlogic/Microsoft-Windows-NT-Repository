/* Copyright(c) 1986 Association of Universities for Research in Astronomy Inc.
 */

#include <descrip.h>
#include "spp.h"
#include "vms.h"

/*  ZFDELE -- Delete a file.
 */
zfdele (osfn)
char  *osfn;
{
	DESCRIPTOR  file;

	_strdesc (osfn, &file);
	if (lib$delete_file (&file) & 1)
	    return (XOK);
	else
	    return (XERR);
}
