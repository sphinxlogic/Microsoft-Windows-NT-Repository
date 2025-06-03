/*****************************************************************************
 *
 * FACILITY:
 *   XPool	DECwindows Pool game
 *	
 * ABSTRACT:
 *   This module contains routines to deal with DECNET communication.
 *
 * AUTHOR:
 *   Doug Stefanelli
 *
 * CREATION DATE: 20-December-1989
 *
 * Edit History 
 *
 *  DS	20-Dec-1989	Original version
 *  DS	13-Mar-1990	Add Ultrix support.
 *
 */

/*
 * Routines in this module
 */
int get_remote_user();		/* returns remote node and username */
void net_open_remote();		/* open remote DECnet link */
void net_open_local();		/* open local DECnet link */
void net_read();		/* start a DECnet read */
int net_read_complete();	/* test for DECnet read completion */
int net_write();		/* perform a DECnet write */
void net_close();		/* close a DECnet link */
void get_local_user();		/* get local node and username */

#ifdef VMS
/*
 * Include files
 */
#include <iodef.h>
#include <ssdef.h>
#include <descrip.h>

/*
 * Local variables
 */
static struct iosb {
    short status;
    short byte_count;
    long filler;
};

static int channel;
static unsigned char net_ast_complete;
static struct iosb read_iosb;
static int read_status;

net_open(device_name)
char *device_name;
{
    $DESCRIPTOR(device_desc, device_name);
    device_desc.dsc$w_length = strlen(device_name);

    sys$assign(&device_desc, &channel, 0, 0);
}

void net_open_local()
{
    net_open("SYS$NET");
}

void net_open_remote(node)
char *node;
{
    int i;
    char string[80];

    strcpy(string, node);
    for (i=0; i < strlen(string); i++)
	if (string[i] == ' ' || string[i] == ':') break;
    string[i] = 0;
    strcat(string, "::\"0=XPOOL\"");

    net_open(string);
}

void net_close()
{
    sys$dassgn(channel);
}

static void net_ast(astprm)
{
    net_ast_complete = 1;
}

int net_read_complete(bytes_read)
int *bytes_read;
{
    if (read_status != SS$_NORMAL || net_ast_complete) {
	if (read_status != SS$_NORMAL || read_iosb.status != SS$_NORMAL)
	    *bytes_read = -1;
	else
	    *bytes_read = read_iosb.byte_count;
	return(1);
    } else
	return(0);
}

void net_read(data, size)
char *data;
int size;
{
    net_ast_complete = 0;
    read_status = sys$qio(0, channel, IO$_READVBLK, &read_iosb, &net_ast, 0,
		data, size, 0, 0, 0, 0);
    
}

int net_write(data, size)
char *data;
int size;
{
    struct iosb write_iosb;
    int status;

    status = sys$qiow(0, channel, IO$_WRITEVBLK, &write_iosb, 0, 0, data, size,
		0, 0, 0, 0);
    if (status != SS$_NORMAL || write_iosb.status != SS$_NORMAL)
	return(-1);
    else
	return(write_iosb.byte_count);
}

int get_remote_user(nodename, username)
char *nodename;
char *username;
{
    int status;
    int string_len;
    char *ptr1, *ptr2, *ptr3;
    char *task_string;

    task_string = getenv("SYS$NET");
    if (task_string == 0)
	return(0);

    ptr1 = strchr(task_string, ':');
    ptr2 = strchr(task_string, '=');
    ptr3 = strchr(task_string, '/');
    if (ptr1 == 0 || ptr2 == 0 || ptr3 == 0 || ptr1 > ptr2 || ptr2 > ptr3) {
	strcpy(nodename, "SOMEWHERE");
	strcpy(username, "SOMEBODY");
	return(1);
    }

    string_len = (int)ptr1 - (int)task_string;
    strncpy(nodename, task_string, string_len);
    *(nodename + string_len) = '\0';

    ptr2++;
    string_len = (int)ptr3 - (int)ptr2;
    strncpy(username, ptr2, string_len);
    *(username + string_len) = '\0';

    return(1);
}
#else
int get_remote_user()	{return(0);};
void net_open_remote()	{};
void net_open_local()	{};
void net_read()		{};
int net_read_complete()	{};
int net_write()		{};
void net_close()	{};
#endif

void get_local_user(nodename, username)
char *nodename;
char *username;
{
    char *node_ptr;
    char *user_ptr;

#ifdef VMS
    int string_len;
    char *ptr;

    node_ptr = getenv("SYS$NODE");
    if (node_ptr != 0) {
	ptr = strchr(node_ptr, ':');
	if (ptr != 0) {
	    string_len = (int)ptr - (int)node_ptr;
	    strncpy(nodename, node_ptr, string_len);
	    *(nodename + string_len) = '\0';
	} else
	    strcpy(nodename, node_ptr);
    } else
	strcpy(nodename, "SOMEWHERE");
#else
    node_ptr = (char *)getnodename();
    strcpy(nodename, node_ptr != 0 ? node_ptr : "SOMEWHERE");
#endif
    user_ptr = (char *)getenv("USER");
    strcpy(username, user_ptr != 0 ? user_ptr : "SOMEBODY");
}
