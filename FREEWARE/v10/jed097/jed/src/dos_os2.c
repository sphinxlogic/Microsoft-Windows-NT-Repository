/* ///////////////////////////////////////////////////////////////////
//  File:	dos_os2.c
//
//  Descript:	routines that are common to dos & os/2 and `fairly'
//		compiler independent
//		included by i386.c, ibmpc.c, os2.c
//
//  Public:	sys_flush();
//		sys_delete_file();
//		sys_rename();
//		sys_suspend();
//		sys_System();
//
//  Private:	discover_shell();
//		parse_command_line();
//
//  Public:	NumLock_Is_Gold, PC_Alt_Char
//
//  Private:	---
//
//  Revisions:
//   - --- --	---	---
/////////////////////////////////////////////////////////////////// */
#ifndef __DOS_OS2_CODE_INCLUDED
#define __DOS_OS2_CODE_INCLUDED

#ifdef UNIXSLASH
const char SlashChar[2] = "/";
#else
const char SlashChar[2] = "\\";
#endif
extern char *get_cwd (void);

int NumLock_Is_Gold = 0;	       /* map numlock key to F1 */
int PC_Alt_Char = 27;

void sys_flush (int dummy)
{
   (void) dummy;
}

/* Delete the file NAME.  returns 0 on failure, 1 on sucess
 * Under OS/2 and DOS, unlink()[UNIX] and remove()[ANSI] are equivalent.
 */

int sys_delete_file(char *name)
{
    return(1 + remove(name));
}

/* Rename the file or directory OLDNAME to NEWNAME.  Moving a file
 * to a different directory on the same drive is possible.
 * returns 0 on success and -1 on error
 */

int sys_rename(char *oldname, char *newname)
{
    return(rename(oldname,newname));
}

/* find shell (once only) */
static char *shell = NULL, *shell_switch = NULL;
static void discover_shell(void)
{
   if ( shell != NULL )  return;
#ifdef SH_SHELL			       /* maybe some day ... */
   shell_switch = "-c";
   if ( (shell = getenv("SHELL")) != NULL )  return;
#endif
   shell_switch = "/c";
   if ( (shell = getenv("COMSPEC")) != NULL ) return;
#ifdef __os2__   
   shell = "cmd.exe";		       /* absolute last resort! */
#else
   shell = "command.com";	       /* absolute last resort! */   
#endif
}



/* ///////////////////////////////////////////////////////////////////
//  Function:   static int parse_command_line( int *argc, char ***argv,
//					       char **fname,
//					       char *command_line );
//  Descript:	parse command_line
//
//  Returns:	the handles to redirect
//
//  Caveat:	requires spaces to separate each argument
//		ie., args>filename	will NOT work
///////////////////////////////////////////////////////////////////// */
#define DIRECT_SPAWN 0x1000	       /* some arbitrary number */
static int
parse_command_line( int *argc, char ***argv, char **fname, char *command_line )
{
   int count, handles = 0;
   char *pt;

   discover_shell();		       /* find which shell to use */
   while ( (*command_line != '\0') && (*command_line == ' '))
     command_line++;		       /* start on 1st non-space */

   if ( *command_line == '!' ) {
      handles = DIRECT_SPAWN;
      command_line++;		       /* advance past '!' */
      while ((*command_line != '\0') && (*command_line == ' '))
      	command_line++;		       /* start on next non-space */
   }

   pt = command_line;
   count = 0;
   while ( *pt != '\0' ) {
      count++;			       /* this is an argument */
      while ((*pt != '\0') && (*pt != ' '))  
	{
	   if ( *pt == '|' )	       /* cannot spawn directly */
	     handles = 0;		       /* need shell for pipes */
	   pt++;			       /* advance until a space */
      }
      while ( *pt == ' '  )
      	pt++;			       /* advance until a non-space */
   }

   *argv = (char **) SLMALLOC( (count+3) * sizeof(char *) );
   if ( *argv == NULL )
     return 0;			       /* malloc error */

   *argc = 0;   
   if ( !(handles & DIRECT_SPAWN) ) {
      (*argv)[ *argc ] = shell;
      (*argc)++;
      if ( count > 0 )  {
	 (*argv)[ *argc ] = shell_switch;
	 (*argc)++;
      }
      count += (*argc);
   }

   pt = command_line;
   while ((*argc < count) && (*pt != '\0')) {
      (*argv)[ *argc ] = pt;
      (*argc)++;
      while ( *pt != '\0' && *pt != ' ' )
      	pt++;			/* advance until a space */
      if ( *pt != '\0' )
      	*(pt++) = '\0';		/* parse argument here */
      while ( *pt == ' ')
      	pt++;		        /* advance until a non-space */
   }
   (*argv) [ *argc ] = (char *) NULL;	/* NULL terminated list */

/*  now examine the arguments for '>' redirect */

   for ( count = 0; count < *argc; count++ ) {
      for ( pt = (*argv)[count]; *pt && *pt != '>'; pt++ )
	 /* find '>' char */;
      if ( *pt == '>' ) {
	 if ( pt == (*argv)[count] ) {
	    handles |= 0x01;
	 } else {
	    switch ( *(--pt) ) {
	     case '1':
	       handles |= 0x01;
	       break;
	     case '2':
	       handles |= 0x02;
	       break;
	     case '&':
	       handles |= 0x03;
	       break;
	    }
	 }
	 (*argv)[count] = NULL;		/* remove from the list */
	 count++;			/* file name follows '>' */
	 if ( (*argv)[count] != NULL )
	   *fname = (*argv)[count];
      }
   }
   if ((*fname == NULL) || (**fname == '\0' ))
     handles = 0x00;		/* don't redirect if there is no name */

   return handles;
}


/* ///////////////////////////////////////////////////////////////////
//  Function:   int sys_System(char *command_line);
//
//  Descript:	shell wrapper that understands some common redirection syntax
//		command args  > filename		; stdout
//		command args 1> filename		; stdout
//		command args 2> filename		; stderr
//		command args &> filename		; stdout+stderr
//		command args  > filename 2>&1		; stdout+stderr
//
// additionally, if command is prefixed by a '!', then the command is
// spawned directly without invoking the shell
//
//  Returns:	returns error codes as per spawn*()
//
//  Caveat:	requires spaces to separate each argument
//		ie., command args>filename	will NOT work
///////////////////////////////////////////////////////////////////// */

int
sys_System(char *command_line1)
{
   int ret, handles, argc = 0;
   char *fname = NULL, **argv = NULL, *command_line;
  
   if (NULL == (command_line = (char *) SLMALLOC (strlen (command_line1) + 1)))
     {
	msg_error ("Malloc error.");
	return -1;
     }
   
   strcpy (command_line, command_line1);

   handles = parse_command_line( &argc, &argv, &fname, command_line );
   if ( (handles & 0x01) && (freopen(fname, "at",  stdout) == NULL) )
     msg_error( "Couldn\'t redirect <stdout>" );
   if ( (handles & 0x02) && (freopen(fname, "at",  stderr) == NULL) )
     msg_error( "Couldn\'t redirect <stderr>" );

   ret = spawnvp(P_WAIT, argv[0], (void *) argv);
   if ( -1 == ret ) {
      switch(errno) {
       case ENOENT:
	 if ( handles & DIRECT_SPAWN ) msg_error("Command not found.");
	 else msg_error("Shell not found.");
	 break;
       case ENOMEM:
	 msg_error("Insufficient Memory.");
	 break;
       default:
	 msg_error("Unknown Error.");
      }
   }

   if ( (handles & 0x01) && (freopen("con", "at",  stdout) == NULL) )
     exit_error( "Couldn\'t reattach <stdout> to console", 1 );
   if ( (handles & 0x02) && (freopen("con", "at",  stderr) == NULL) )
     exit_error( "Couldn\'t reattach <stderr> to console", 1 );

   if ( argc ) SLFREE( argv );
   
   SLFREE (command_line);
   return ret;
}


void sys_suspend(void)
{
   sys_System( "" );
}

#endif	/* whole file */
