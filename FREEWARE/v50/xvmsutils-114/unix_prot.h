/*----------------------------------------------------------------------------*/
/* This file contains the values for file protections as used by Unix OS's.   */
/* The values have been taken from the file lylocal.h of the lynx             */
/* distribution.                                                              */
/* 950628 Martin P.J. Zinser (M.Zinser@gsi.de or eurmpz@eur.sas.com )         */
/*----------------------------------------------------------------------------*/

#define S_IRWXU         0000700 /* rwx, owner */
#define         S_IRUSR 0000400 /* read permission, owner */
#define         S_IWUSR 0000200 /* write permission, owner */
#define         S_IXUSR 0000100 /* execute/search permission, owner */
#define S_IRWXG         0000070 /* rwx, group */
#define         S_IRGRP 0000040 /* read permission, group */
#define         S_IWGRP 0000020 /* write permission, grougroup */
#define         S_IXGRP 0000010 /* execute/search permission, group */
#define S_IRWXO         0000007 /* rwx, other */
#define         S_IROTH 0000004 /* read permission, other */
#define         S_IWOTH 0000002 /* write permission, other */
#define         S_IXOTH 0000001 /* execute/search permission, other */
