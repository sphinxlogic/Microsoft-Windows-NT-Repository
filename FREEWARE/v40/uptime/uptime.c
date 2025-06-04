

/*
 *  UPTIME.C - Show VMS system uptime in delta time format and on the next
 *      line show the actual calendar date and time of the last bootup
 *
 *  13-NOV-1989 Lyle W. West
 *  01-Sep-1995 Revised (lww)
 *  14-Oct-1998 Decc 5.6 (axp)
 */

    
#include <stdio.h>  
#include <syidef.h> 
#include <ssdef.h>  
#include <descrip.h>
#define NULL 0

main()
{
    char    uptime[32];
    char    delta[32];
    char    boottime[32];
    char    *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    short   timlen, bootbin_len;
    short   numbuf[7];

    long    cvtflg;
    long    status;

    long    deltabin[2];
    long    bootbin[2];
    long    curbin[2];
    short   boot_dat[7];

    $DESCRIPTOR(dsc_delta, delta);
    struct { short len, code; char *buf, *retlen; } itmlis[2];

    cvtflg = 0;

    itmlis[0].len   = sizeof(bootbin);
    itmlis[0].code   = SYI$_BOOTTIME;
    itmlis[0].buf   = &bootbin;
    itmlis[0].retlen = &bootbin_len;       
    itmlis[1].len   = 0;           
    itmlis[1].code   = 0;          

    status = SYS$GETTIM(&curbin);
    status = SYS$GETSYIW(0, 0, 0, &itmlis, 0, 0, 0);
    if (status != SS$_NORMAL) exit(status);
    status = lib$sub_times(&curbin, &bootbin, &deltabin);
    status = SYS$ASCTIM(&timlen, &dsc_delta, &deltabin, &cvtflg);
    delta[timlen] = '\0';
    printf("\n\tUptime:  %s\n", delta);

    status = SYS$NUMTIM(&numbuf, &bootbin);
    printf("\tReboot:   %s %02d %02d:%02d:%02d\n\n", month[numbuf[1]-1],
        numbuf[2], numbuf[3], numbuf[4], numbuf[5]);
}
