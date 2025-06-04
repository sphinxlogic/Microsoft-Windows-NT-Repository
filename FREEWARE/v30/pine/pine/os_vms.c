/* OS_VMS.C */
/* Boilerplate header.	*/
#include	<unixio.h>
#include	<stat.h>

#include	"headers.h"

/* Can Access file */
/*----------------------------------------------------------------------
       Check if we can access a file in a given way

   Args: file      -- The file to check
         mode      -- The mode ala the access() system call, see ACCESS_EXISTS
                      and friends in pine.h.

 Result: returns 0 if the user can access the file according to the mode,
         -1 if he can't (and errno is set).
 ----*/
int
can_access(file, mode)
    char *file; 
    int   mode;
{
    return(access(file, mode));
}




/* File Size */
/*----------------------------------------------------------------------
      Return the number of bytes in given file

    Args: file -- file name

  Result: the number of bytes in the file is returned or
          -1 on error, in which case errno is valid
 ----*/
long
file_size(file)
    char *file;
{
    struct stat buffer;

    if (stat(file, &buffer) != 0)
        return(-1L);
    return(buffer.st_size);
}




/* Is_writable_dir function.  Return success always */
is_writable_dir()
{
	return 0;
}

/*----------------------------------------------------------------------
      Create the mail subdirectory.

  Args: dir -- Name of the directory to create
 
 Result: Directory is created.  Returns 0 on success, else -1 on error
	 and errno is valid.
  ----*/
create_mail_dir(dir)
    char *dir;
{
    if(mkdir(dir, 0700) < 0)
      return(-1);

    (void)chmod(dir, 0700);
    return(0);
}



/*----------------------------------------------------------------------
      Rename a file

  Args: tmpfname -- Old name of file
        fname    -- New name of file
 
 Result: File is renamed.  Returns 0 on success, else -1 on error
	 and errno is valid.
  ----*/
rename_file(tmpfname, fname)
    char *tmpfname, *fname;
{
    return(rename(tmpfname, fname));
}



/*----------------------------------------------------------------------
      Paste together two pieces of a file name path

  Args: pathbuf      -- Put the result here
        first_part   -- of path name
        second_part  -- of path name
 
 Result: New path is in pathbuf.  No check is made for overflow. 

BUGS:  This is a first stab at dealing with fs naming dependencies, and others 
still exist.
  ----*/
void
build_path(pathbuf, first_part, second_part)
    char *pathbuf, *first_part, *second_part;
{
    if(first_part == NULL)
      strcpy(pathbuf, second_part);
    else {
	if(first_part[strlen(first_part) - 1] == '/') {	/* remove it */
		strcpy(pathbuf, first_part);
		strcpy(&pathbuf[strlen(pathbuf) - 1], second_part);
	} else
		sprintf(pathbuf, "%s%s", first_part, second_part);
    }
}

char *fnexpand(buf, len)
char	*buf;
int	len;
{
    return(len ? buf : (char *)NULL);
}


/*----------------------------------------------------------------------
      Return pointer to last component of pathname.

  Args: filename     -- The pathname.
 
 Result: Returned pointer points to last component in the input argument.
  ----*/
char *
last_cmpnt(filename)
    char *filename;
{
    register char *p;

    for(p = filename + strlen(filename) -1; p > filename; p--){
        if(*p == '/' && *(p+1) != '\0')
          return(p+1);
    }
    return(p);
}

/* On VMS - return the same name.	*/
int
expand_foldername(filename)
    char *filename;
{
	return 1;
}



/*----------------------------------------------------------------------
    Filter file names for strange characters

   Args:  file  -- the file name to check
 
 Result: Returns NULL if file name is OK
         Returns formatted error message if it is not
  ----*/
char *
filter_filename(file)
    char *file;
{
#ifdef ALLOW_WEIRD
    static char illegal[] = {'\177', '\0'};
#else
    static char illegal[] = {'"', '#', '$', '%', '&', '\'','(', ')','*',
                          ',', ':', ';', '<', '=', '>', '?', '[', ']',
                          '\\', '^', '|', '\177', '\0'};
#endif
    static char error[100];
    char ill_file[MAXPATH+1], *ill_char, *ptr, e2[10];

    ptr = file;
    while (*ptr == ' ') ptr++;	/* leading spaces GONE! */

    if(*ptr == '*')
      ptr++; /* Allow * at beginning for news groups */

    while(*ptr && *ptr > ' ' && strindex(illegal, *ptr) == 0)
      ptr++;

    if(*ptr != '\0') {
        if(*ptr == ' ') {
            ill_char = "<space>";
        } else if(*ptr == '\n') {
            ill_char = "<newline>";
        } else if(*ptr == '\r') {
            ill_char = "<carriage return>";
        } else if(*ptr == '\t') {
    	    ill_char = "<tab>";
        } else if(*ptr < ' ') {
            sprintf(e2, "control-%c", *ptr + '@');
            ill_char = e2;
        } else if (*ptr == '\177') {
    	    ill_char = "<del>";
        } else {
    	    e2[0] = *ptr;
    	    e2[1] = '\0';
    	    ill_char = e2;
        }
        if(ptr != file) {
            strncpy(ill_file, file, ptr - file);
            ill_file[ptr - file] = '\0';
            sprintf(error,"Character \"%s\" after \"%s\" not allowed",ill_char,
                    ill_file);
        } else {
            sprintf(error,
                    "First character, \"%s\", not allowed",
                    ill_char);
        }
            
        return(error);
    }
    return((char *)NULL);
}


/*----------------------------------------------------------------------
    Read whole file into memory

  Args: filename -- path name of file to read

  Result: Returns pointer to malloced memory with the contents of the file
          or NULL

This won't work very well if the file has NULLs in it and is mostly
intended for fairly small text files.
 ----*/
char *
read_file(filename)
    char *filename;
{
    FILE	*fd;
    struct stat statbuf;
    char       *buf, record[1024];

    fd = fopen(filename, "r");
    if(fd == NULL)
      return((char *)NULL);

    stat(filename, &statbuf);

    buf = fs_get(statbuf.st_size + 512);
    *buf = '\0';

    while(fgets(record, sizeof(record) - 1, fd) != NULL)
	strcat(buf, record);

    fclose(fd);

    return(buf);
}

/*----------------------------------------------------------------------
   Create a temporary file, the name of which we don't care about 
and that goes away when it is closed.  Just like ANSI C tmpfile.
  ----*/
FILE  *
create_tmpfile()
{
    return(tmpfile());
}

/*----------------------------------------------------------------------
      Return a unique file name in a given directory.  This is not quite
      the same as the usual tempnam() function, though it is very similar.
      We want it to use the TMP environment variable only if dir is NULL,
      instead of using TMP regardless if it is set.

  Args: dir      -- The directory to create the name in
        prefix   -- Prefix of the name
 
 Result: Malloc'd string equal to new name is returned.  It must be free'd
	 by the caller.  Returns the string on success and NULL on failure.
  ----*/
char *
temp_nam(dir, prefix)
    char *dir, *prefix;
{
	static char	name[512];

    if(dir == NULL)
	sprintf(name, "SYS$LOGIN:%sXXXXXX", prefix);
    else
	sprintf(name, "%s%sXXXXXX", dir, prefix);
    return(mktemp(name));
}
/*----------------------------------------------------------------------
     Abort with a core dump
 ----*/
void
coredump()
{
    abort();
}


/*----------------------------------------------------------------------
       Call system gethostname

  Args: hostname -- buffer to return host name in 
        size     -- Size of buffer hostname is to be returned in

 Result: returns 0 if the hostname is correctly set,
         -1 if not (and errno is set).
 ----*/
hostname(hostname,size) 
    char *hostname;
    int size;
{
#ifdef MULTINET
    return(gethostname(hostname, size));
#endif	/* Multinet */
#ifdef NETLIB
    return get_local_host_name(hostname, size);	/* defined in C-CLIENT/OSDEP_VMS */
#endif	/* NETLIB */
#if !defined(MULTINET) && !defined(NETLIB)
    strcpy(hostname, getenv("SYS$NODE"));
    return hostname;
#endif	/* !Multinet || !Netlib */
}


/*----------------------------------------------------------------------
       Get the current host and domain names

    Args: hostname   -- buffer to return the hostname in
          hsize      -- size of buffer above
          domainname -- buffer to return domain name in
          dsize      -- size of buffer above

  Result: The system host and domain names are returned. If the full host
          name is akbar.cac.washington.edu then the domainname is
          cac.washington.edu.

On Internet connected hosts this look up uses /etc/hosts and DNS to
figure all this out. On other less well connected machines some other
file may be read. If there is no notion of a domain name the domain
name may be left blank. On a PC where there really isn't a host name
this should return blank strings. The .pinerc will take care of
configuring the domain names. That is, this should only return the
native system's idea of what the names are if the system has such
a concept.
 ----*/
void
getdomainnames(hostname, hsize, domainname, dsize)
    char *hostname, *domainname;
    int   hsize, dsize;
{
    char           *dn, hname[MAX_ADDRESS+1];
    struct hostent *he;

    *hostname = *hname = '\0';
#ifdef MULTINET
    gethostname(hname, MAX_ADDRESS);

    he = gethostbyname(hname);

    if(he == NULL && strlen(hname) == 0) {
        strcpy(hostname, "");
    } else if(he == NULL) {
        strncpy(hostname, hname, hsize - 1);
    } else {
        strncpy(hostname, he->h_name, hsize-1);
    }
    hostname[hsize-1] = '\0';
#endif	/* Multinet */
#ifdef NETLIB
    get_local_host_name(hostname, MAX_ADDRESS);
#endif /* NETLIB */

    if((dn = strindex(hostname, '.')) != NULL) {
        strncpy(domainname, dn+1, dsize-1);
    } else {
        strncpy(domainname, hostname, dsize-1);
    }
    domainname[dsize-1] = '\0';
}



/*----------------------------------------------------------------------
       Return canonical form of host name ala c-client (UNIX version).

   Args: host      -- The host name

 Result: Canonical form, or input argument (worst case)
 ----*/
char *
canonical_name(host)
    char *host;
{
    struct hostent *hent;
    char hostname[MAILTMPLEN];
    char tmp[MAILTMPLEN];
    extern char *lcase();
                                /* domain literal is easy */
    if (host[0] == '[' && host[(strlen (host))-1] == ']')
      return host;

    strcpy (hostname,host);       /* UNIX requires lowercase */
                                /* lookup name, return canonical form */
#ifdef MULTINET
    return (hent = gethostbyname (lcase (strcpy (tmp,host)))) ?
      hent->h_name : host;
#else	/* MULTINET */
    return (host);
#endif	/* Multinet */
}


/*----------------------------------------------------------------------
     This routine returns 1 if job control is available.  Note, thiis
     could be some type of fake job control.  It doesn't have to be
     real BSD-style job control.
  ----*/
have_job_control()
{
    return 0;
}


/*----------------------------------------------------------------------
    If we don't have job control, this routine is never called.
  ----*/
stop_process()
{
    kill(0, SIGKILL); 
}



/*----------------------------------------------------------------------
       Return string describing the error

   Args: errnumber -- The system error number (errno)

 Result:  long string describing the error is returned
  ----*/
char *
error_description(errnumber)
    int errnumber;
{
    static char buffer[128];

	sprintf(buffer, "%d", errnumber);	/*~~~~*/
/*~~~    strcpy(buffer, strerror(errnumber));	*/

    return ( (char *) buffer);
}


/*----------------------------------------------------------------------
  This collection of routines looks up the login name and password on the
system. For things like PC's it's OK for these to return NULL since there
is no system login. Other code will figure out who the user actually is.
  ----*/

/*----------------------------------------------------------------------
  Since VMS does not have the GCOS notion we simply copy the string.

 Result: returns pointer to buffer with name
  ----*/
static char *
gcos_name(gcos_field, logname)
    char *logname, *gcos_field;
{
    static char fullname[MAX_FULLNAME+1];

	strcpy(fullname, gcos_field);
    return(fullname);
}


char *
get_system_login()
{
	return(cuserid(NULL));
}

/*
 | Use $GETUAI to get the full name.
 */
#include <uaidef.h>
char *
get_system_fullname()
{
    int	sys$getuai(), status;

    struct	{ short length;
		  short	type;
		  char *address;
	} usernameD;
    char	Username[64], owner[34];
    static char	OwnerName[34];	/* Return here the name */

    struct {
        short   length,         /* Buffer length */
                code;           /* Item/action code */
        char    *buffer;        /* Input buffer address */
        int     *Olength;       /* Where to place length of result */
        } itmlst[] = {
         {32,		UAI$_OWNER, 	owner, NULL },
         {0, 0, NULL, NULL } };

/* Get the username and upcase it */
	strcpy(Username, cuserid(NULL));
    ucase(Username);

    usernameD.address = Username;
    usernameD.length = strlen(Username);
    usernameD.type = 0;
    status = sys$getuai(0, 0, &usernameD, itmlst, 0, 0, 0);

    if((status & 0x1) == 0)
        return "";                 /* unexpected error of some kind */

/* Copy the owner's name which is a counted string */
   strcpy(OwnerName, &owner[1], (owner[0] & 0xff));
   OwnerName[owner[0] & 0xff] = '\0';
   return OwnerName;
}


char *
get_system_homedir()
{
	return(getenv("HOME"));
}

char *
get_system_passwd()
{
	return NULL;
}


/*----------------------------------------------------------------------
      Look up a userid on the local system and return rfc822 address

 Args: name  -- possible login name on local system

 Result: returns NULL or pointer to static string rfc822 address.
  ----*/
char *
local_name_lookup(name)
    char *name;
{
	return NULL;
}


/*-----------------------------------------------------------------*/
change_passwd()
{
	return 0;
}

/*----------------------------------------------------------------------
       Can we display this type/subtype?

   Args: type      -- the MIME type to check
         subtype   -- the MIME subtype
         params    -- parameters

 Result: returns 1 if the type is displayable, 0 otherwise.
 ----*/
mime_can_display(type, subtype, params)
    int type;
    char *subtype;
    PARAMETER *params;
{
  switch(type) {
    case TYPETEXT:
        /* we always try to display text now */
	return 1;
#ifdef OLDWAY
        if(ps_global->show_all_characters)
          return(1);
        while(params != NULL && strucmp(params->attribute,"charset") != 0)
          params = params->next;

	return(match_charset((params) ? params->value : "US-ASCII",
			     ps_global->VAR_CHAR_SET) != 2);
#endif

    case TYPEAUDIO:
     return(0);

    case TYPEIMAGE:
      if(getenv("DECW$DISPLAY") == NULL)
        return(0);
      if(strucmp(subtype, "gif") == 0 ||
         strucmp(subtype, "pgm") == 0 ||
         strucmp(subtype, "pbm") == 0 ||
	 strucmp(subtype, "tiff") == 0 ||
	 strucmp(subtype, "jpeg") == 0)
          return(1);
      else
        return(0);


    case TYPEAPPLICATION:
      return(0);

    case TYPEMULTIPART:
      return(1);

    case TYPEMESSAGE:
      return(1);

    default:
      return(0);
  }
}


/*----------------------------------------------------------------------
       We don't know how to play audio.
 ----*/
void
gf_play_sound(f, c, flg)
    FILTER_S *f;
    int       c, flg;
{
}


/*----------------------------------------------------------------------
   This is just a call to the ANSI C fgetpos function.
  ----*/
fget_pos(stream, ptr)
FILE *stream;
fpos_t *ptr;
{
    return(fgetpos(stream, ptr));
}


/*----------------------------------------------------------------------
   This is just a call to the ANSI C fsetpos function.
  ----*/
fset_pos(stream, ptr)
FILE *stream;
fpos_t *ptr;
{
    return(fsetpos(stream, ptr));
}


/*----------------------------------------------------------------------
       Dummy srandom function.  Srandom isn't important.
 ----*/
void
srandom(i)
    int i;
{
}


/* Return a random number */
random()
{
	return rand();
}
