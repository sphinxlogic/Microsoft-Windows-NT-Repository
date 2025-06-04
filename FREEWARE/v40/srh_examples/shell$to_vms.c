/*
/*  SHELL$TO_VMS.C
/*
/*  The following program tests the shell$to_vms function.
/*  This function converts certain unix-style file names
/*  into a matching VMS-style file name.  Unfortunately,
/*  not all valid UNIX-style names are converted.
/*
/*  Author:
/*	Stephen Hoffman
/*	XDELTA::HOFFMAN
/*	08-Aug-1991
/*	Digital Equipment Corporation
*/
char *c$$ga_save_name_addr;
int c$$gl_save_name_len;
main()
    {
    unsigned long int retstat;
    void c$$copy_buffer(char *, int );

    c$$ga_save_name_addr = 0;
    c$$gl_save_name_len = 0;

    /*
    /*	The following will work...
    */
    retstat = SHELL$TO_VMS( "/sys$system", &c$$copy_buffer, 0, 0 );
    retstat = SHELL$TO_VMS( "/dua0/crtl", &c$$copy_buffer, 0, 0 );
    retstat = SHELL$TO_VMS( "/dua0/crtl", &c$$copy_buffer, 0, 1 );

    /*
    /*	The following will not work...
    */
    retstat = SHELL$TO_VMS( "/", &c$$copy_buffer, 0, 0 );
    retstat = SHELL$TO_VMS( "/", &c$$copy_buffer, 0, 1 );
    retstat = SHELL$TO_VMS( "/crtl", &c$$copy_buffer, 0, 0 );
    retstat = SHELL$TO_VMS( "/crtl", &c$$copy_buffer, 0, 1 );

    return 1;

    }
void c$$copy_buffer(char *file_spec, int dummy_flag)
    {
    int length;
    char *buff_address;

    if ( c$$gl_save_name_len )
	{
	free( c$$ga_save_name_addr );
	c$$gl_save_name_len = 0;
	}
    length = strlen(file_spec) + 1;
    c$$ga_save_name_addr = buff_address = malloc(length);
    c$$gl_save_name_len = length;
    strcpy(buff_address, file_spec);
    buff_address[length - 1] = '\0';
    }
    
