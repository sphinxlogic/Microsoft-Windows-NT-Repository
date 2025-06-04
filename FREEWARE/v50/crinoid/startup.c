/*  start CRINOID */

#include <stdio.h>
#include <string.h>
#include <starlet.h>
#include <ssdef.h>
#include <iodef.h>
#include <lib$routines.h>
#include "util.h"

int
main(int argc, char *argv[])
{
    int iss;
    char name[200];
    pSTRING dev;
    word chan;
    char buf[100];
    IOSB iosb;

    if (argc < 3) {
        printf("usage: startup USERNAME SERVICENAME\n");
        return(1);
    }

    sprintf(name,"0\"%s\"::\"0=%s\"NL:",argv[1],argv[2]);
    dev = new_STRING(name);

    iss = sys$assign(dev,&chan,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    /* main params */
    iss = sys$qiow(0,chan,IO$_WRITEVBLK,0,0,0,buf,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = sys$qiow(0,chan,IO$_WRITEVBLK,0,0,0,buf,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = sys$qiow(0,chan,IO$_WRITEVBLK,0,0,0,buf,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    iss = sys$qiow(0,chan,IO$_WRITEVBLK,0,0,0,buf,0,0,0,0,0);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);

    while (1) {
        iss = sys$qiow(0,chan,IO$_READVBLK,&iosb,0,0,buf,sizeof(buf)-1,0,0,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        iss = iosb.status;
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
        buf[iosb.count] = '\0';
        if (strncmp(buf,"</DNETCGI>",10) == 0) break;
        iss = sys$qiow(0,chan,IO$_WRITEVBLK,0,0,0,buf,0,0,0,0,0);
        if (VMS_ERR(iss)) VMS_SIGNAL(iss);
    }

    iss = sys$dassgn(chan);
    if (VMS_ERR(iss)) VMS_SIGNAL(iss);
}
