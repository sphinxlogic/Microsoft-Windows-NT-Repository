/* TCP/IP routines using NETLIB 2.0 */

#include <netlib_dir/netlibdef.h>
#include <descrip.h>
#include <string.h>

extern void init_dsc(struct dsc$descriptor_s *, char *, int);

/* Close a socket */
void closenet(unsigned int s)
{
    netlib_close((void **) &s);
}


void inet_netnames(unsigned int sock, char *host_name, char *ipnum)
{
    struct SINDEF sa;
    struct dsc$descriptor_s host_dsc;
    unsigned int length, status;
    unsigned int retlen;

    length = sizeof(sa);
    status =  netlib_getpeername((void **) &sock, (struct SINDEF *)&sa,
                                  &length, &retlen);
    if ((status & 1) == 0) {
/*      qilog(-2, "getpeername() failure: %s", vms_errno_string()); */
        strcpy(ipnum,"Unknown");
        strcpy(host_name,"Unknown");
        return;
    }
    init_dsc(&host_dsc, host_name, 255);
    length = sizeof(sa.sin_x_addr);
    status = netlib_address_to_name((void **) &sock, 0, &sa.sin_x_addr,
                                    &length, &host_dsc, &retlen);
    if ((status & 1) == 0) {
        status = netlib_addrtostr(&sa.sin_x_addr, &host_dsc, &retlen);
    }
    host_name[retlen] = '\0';
}


int readline(unsigned int sock, char *buf, int size)
{
    struct dsc$descriptor_s buf_dsc;
    int status;
    unsigned short retlen;

    strncpy(buf, "", size); 
    init_dsc(&buf_dsc, buf, size-1);

    status = netlib_read((void **) &sock, &buf_dsc, &retlen);

    if ((status & 1) == 0)
        return -1;

    return (int) retlen;
}


int writestring(unsigned int sock, char *buf)
{
    struct dsc$descriptor_s buf_dsc;
    int status;
    unsigned short retlen;

    if (buf == NULL)
        return 0;

    init_dsc(&buf_dsc, buf, strlen(buf));

    status = netlib_write((void **) &sock, &buf_dsc);

    if ((status & 1) == 0)
        return -1;

    return 0;
}


