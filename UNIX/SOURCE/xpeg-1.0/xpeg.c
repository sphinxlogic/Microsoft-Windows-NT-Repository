/*************************************************************************\
*   				Xpeg 1.0				  *
*									  *
* Copyright 1990, Kenneth C. Nelson			 	          *
*									  *
* Rights       : I, Ken Nelson own Xpeg.  I am donating the source	  *
*		 to the public domain with the restriction that nobody	  *
*		 sells it, or anything derived from it, for anything more *
*		 than media costs.  Xpeg uses a dynamic object library	  *
*		 that I don't own.  See the subdirectory dynobj for  	  *
*		 restrictions on its use.				  *
*								          *
*                Please feel free to modify Xpeg. See Todo for details.   *
*									  *
\*************************************************************************/

 /*
  * xpeg.c	- contains main and startup code.
  *
  */

#include "xui.h"


char *xpeg_version()
{
  return "Xpeg 1.0";
}


void  Usage()
{
  printf("usage: xpeg [-toolkit_options] pegfile [username]\n\n");
  printf("	 pegfile    - a valid Xpeg file, xpeg will quit if it is not.\n\n");
  printf("	 username   - an optional string that should match your name\n");
  printf("	              on the Xpeg board. If it does not match you will\n");
  printf("	               not be able to edit your location\n\n");
  printf(" These enviroment variables can be used instead of the command line options:\n\n");
  printf("       XPEG_FILE\n");
  printf("       XPEG_USER\n\n");
  printf(" The command line interface will override the environment variables.\n");
  exit(1);
}



main(argc, argv)
  int     argc;
  char    **argv;

  {

    /*
     * Let X do the parsing of it's stuff, and then we do ours!
     *
     */
    
    initXpegInterface(&argc,argv);

    
    if (argc < 2)
    {
      /*
       * No arguments, so check enviroment variables, if they are not set holler!
       *
       */
       
      setPegfileName(getenv("XPEG_FILE"));
      setUserName(getenv("XPEG_USER"));
      if (pegfileName() == NULL)
      {
        Usage();
      }
    }
    else
    {
      if (argc > 2)
      {
        setPegfileName(strdup(argv[argc-2]));      
	setUserName(strdup(argv[argc-1]));
      }
      else
      {
         setPegfileName(strdup(argv[argc-1]));
      }
    }

    /*
     * Build up the storage for the pegboard
     *
     */
     
    buildXpegBoard(pegfileName());

    
    /*
     * Build up the X interface.
     *
     */
     
    buildXpegInterface();
    
    runXpegInterface();

  }

