/* chkequiv.c: Check files for equivalence.
 * Greg Lehey, 6 September 1993
 *
 * A file is considered to be equivalent if the inode is
 * different (otherwise it's equal) and the EOF is the same.
 * This is a primitive helper program for the packaging tools
 */
#include <sys/types.h>
#include <sys/stat.h>

main (int argc, char *argv [])
{
  struct stat file1_stat;
  struct stat file2_stat;

  if (argc != 3)
    {
    printf ("Usage:\n\n\t%s first-file second-file", argv [0]);
    exit (2);
    }
  if (stat (argv [1], &file1_stat))
    {
    perror ("Can't stat first file");
    exit (2);
    }
  if (stat (argv [2], &file2_stat))
    {
    perror ("Can't stat second file");
    exit (2);
    }
  if ((file1_stat.st_dev == file2_stat.st_dev)		    /* both on the same file system */
      && (file1_stat.st_size == file2_stat.st_size) )	    /* and the same size */
    {
    if (file1_stat.st_ino != file2_stat.st_ino)		    /* not already the same inode */
      exit (0);						    /* all fine and dandy, we can replace with a link */
    else						    /* they're the same already, why bother? */
      exit (-1);
      }
  else							    /* something not quite OK here, */
    exit (1);
  }

