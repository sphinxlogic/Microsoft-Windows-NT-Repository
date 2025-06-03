/* $Id: patchlevel.h,v 3.5 1992/06/02 08:15:01 ajf Exp ajf $
 *
 * $Log: patchlevel.h,v $
 * Revision 3.5  1992/06/02  08:15:01  ajf
 * Patch 5:
 *    1. allow printing to a pipe (apple network printer??)
 *    2. fix problem with first line loss if language file used
 *
 * Revision 3.4  1992/02/24  06:40:55  ajf
 * Patch 4:
 *    1. force paper tray selection (makefile + user option) added
 *    2. ctl/d is compile time option
 *    3. fix problems with def file generation
 *    4. allow psf to read printer device for responses
 *    5. paper usage logged to accounting file
 *    6. new options -a -A -y -Y
 *
 * Revision 3.3  1992/02/07  15:07:04  ajf
 * patch 3:
 *    1. add -e -E -M options for wrap and margins
 *    2. -i -m are physical page offsets
 *    3. fixup scaling with pointsize settings
 *
 * Revision 3.2  1992/01/19  05:48:36  ajf
 * 1. used ctype.h (from bart@cs.uoregon.edu)
 *
 * Revision 3.1  1991/11/27  06:01:03  ajf
 * patch 1:
 *    1. add psflpd filter support with links. options in psfprint.def.
 *    2. revise documentation
 *    3. correct problem with psfbs and underscore order.
 *    4. remove support for modified lpr/lpd
 *
*/

#define PATCHLEVEL 5
