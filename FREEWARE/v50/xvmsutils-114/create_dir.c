/*
 * Plain & Simple Create directory routine for use with
 * VMS programs.  I did this because I didn't like the
 * way the VAX C RTL mkdir worked.
 */

#include <descrip.h>
#include <string.h>
#include <lib$routines.h>
#include <ssdef.h>

int create_dir(name)
char *name;
{
   int istatus;
   struct dsc$descriptor_s dirname;

   dirname.dsc$w_length	= strlen(name);
   dirname.dsc$b_dtype	= DSC$K_DTYPE_T;
   dirname.dsc$b_class	= DSC$K_CLASS_S;
   dirname.dsc$a_pointer= name;

   istatus = lib$create_dir(&dirname, 0, 0, 0, 0, 0);

   if (istatus == SS$_CREATED)
	return (0);
   else
	return (-1);
}

