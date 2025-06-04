/*

Program:  announce.c

Purpose:  This program allows for a dynamic SYS$ANNOUNCE message consisting of
          current system loading information.

Author:   (c) 1997 Curtis Rempel (vmsguy@home.com, curtis.rempel@shaw.ca)

*/

#include <descrip.h>    
#include <iodef.h>      
#include <ssdef.h>      
#include <stdio.h>

struct iosb { short status;
              short byte_cnt;
              int   unused;
            };

struct iosb iosb;       /* I/O status block */

short   mbx_channel;    /* channel to SYS$ANNOUNCE mailbox */

int     mbx_msg_len;    /* mailbox message length */
int     status;         /* system service completion status */

char    mbx_msg[250];   /* mailbox message text */

static $DESCRIPTOR (mbx_msg_dsc,mbx_msg);
static $DESCRIPTOR (lav_device,"_LAV0:");
static $DESCRIPTOR (mbx_device,"SYS$ANNOUNCE_MBX");

#ifdef __ALPHA          /* use appropriate architecture data cells */

globalref int SYS$GL_IJOBCNT;
globalref int SYS$GL_BJOBCNT;

#define IJOBCNT SYS$GL_IJOBCNT
#define BJOBCNT SYS$GL_BJOBCNT

#else

globalref short SYS$GW_IJOBCNT;
globalref short SYS$GW_BJOBCNT;

#define IJOBCNT SYS$GW_IJOBCNT
#define BJOBCNT SYS$GW_BJOBCNT

#endif

void ast_handler();
void queue_ast();
void send_mailbox();

main()

{
/*
   Create a permanent mailbox to receive the information for SYS$ANNOUNCE
*/

        status = SYS$CREMBX (1,                  /* prmflg */
                             &mbx_channel,       /* chan */
                             0,                  /* maxmsg */
                             0,                  /* bufquo */
                             0x2202,             /* promsk */
                             0,                  /* acmode */
                             &mbx_device,        /* lognam */
                             0,                  /* flags */
                             0);                 /* lognam */

        if (status != SS$_NORMAL) LIB$STOP (status);

/*
   Now queue a read AST on the channel and wait for somebody to log in.
   The action of logging in will cause LOGINOUT.EXE to translate
   SYS$ANNOUNCE which in turn will cause a read operation on the mailbox.
   This read operation triggers the AST routine to write information into 
   the mailbox which is then passed back to SYS$ANNOUNCE and is displayed 
   on the users terminal.

   The protection mask X%2202 denies write access to world, group, and
   system.
*/
        queue_ast ();

        status = SYS$HIBER();
        if (status != SS$_NORMAL) LIB$STOP (status);
}

/*
   The following procedure executes at AST level to write a message into
   a mailbox associated with the SYS$ANNOUNCE logical name.  
*/

void ast_handler ()

{
        short   lav_channel;    /* load average driver channel */

        float   lav_data[9];    /* data array from LAVDRIVER */

        char    date_time[24];  /* current date/time */
        char    load_msg[26];   /* load average values string */

        $DESCRIPTOR (fao_dsc,"!/!17AS   !AS  Jobs: !UL+!UL!/");
        $DESCRIPTOR (date_time_dsc,date_time);
        $DESCRIPTOR (load_msg_dsc,load_msg);

/*
    Read a set of values from LAV0:
*/

        status = SYS$ASSIGN (&lav_device,       /* devnam */
                             &lav_channel,      /* chan */
                             0,                 /* acmode */
                             0,                 /* mbxnam */
                             0);                /* flags */

        if (status != SS$_NORMAL) LIB$STOP (status);

        status = SYS$QIOW (0,                   /* efn */
                           lav_channel,         /* chan */
                           IO$_READVBLK,        /* func */
                           &iosb,               /* iosb */
                           0,                   /* astadr */
                           0,                   /* astprm */
                           lav_data,            /* p1 */
                           36,                  /* p2 */
                           0,                   /* p3 */
                           0,                   /* p4 */
                           0,                   /* p5 */
                           0);                  /* p6 */

        if (status != SS$_NORMAL) LIB$STOP (status);

        status = SYS$DASSGN (lav_channel);
        if (status != SS$_NORMAL) LIB$STOP (status);

/*
    Build announcement message by formatting the text using $FAO and
    then dumping it in the mailbox via send_mailbox().  These two steps
    can be repeated as many times as necessary before the final $QIO
    call with IO$M_NOW which terminates the operation and sends the 
    contents of the mailbox on its way.
*/

        if (iosb.status) {

                status = LIB$DATE_TIME (&date_time_dsc);
                if (status != SS$_NORMAL) LIB$STOP (status);


                sprintf (load_msg,"Load:   %2.2f %2.2f %2.2f",
                         lav_data[0],lav_data[1],lav_data[2]);

                status = SYS$FAO (&fao_dsc,
                                  &mbx_msg_len,
                                  &mbx_msg_dsc,
                                  &date_time_dsc,
                                  &load_msg_dsc,
                                  IJOBCNT,
                                  BJOBCNT);

                if (status != SS$_NORMAL) LIB$STOP (status);

                send_mailbox();

        }

        else
                LIB$STOP (iosb.status);

/*
    Drop it in the mailbox.
*/

        status = SYS$QIOW (0,                           /* efn */
                           mbx_channel,                 /* chan */
                           IO$_WRITEOF | IO$M_NOW,      /* func */
                           &iosb,                       /* iosb */
                           0,                           /* astadr */
                           0,                           /* astprm */
                           0,                           /* p1 */
                           0,                           /* p2 */
                           0,                           /* p3 */
                           0,                           /* p4 */
                           0,                           /* p5 */        
                           0);                          /* p6 */        

        if (status != SS$_NORMAL) LIB$STOP (status);    
        if (iosb.status != SS$_NORMAL) LIB$STOP (iosb.status);

/*
    Queue another read attention AST on the mailbox for the next login.
*/

        queue_ast ();

}

/*
   The following procedure queues a "read attention" AST on the mailbox
*/

void queue_ast ()

{
        status = SYS$QIOW (0,                           /* efn */
                           mbx_channel,                 /* chan */
                           IO$_SETMODE | IO$M_READATTN, /* func */
                           &iosb,                       /* iosb */
                           0,                           /* astadr */
                           0,                           /* astprm */
                           ast_handler,                 /* p1 */
                           mbx_channel,                 /* p2 */
                           0,                           /* p3 */
                           0,                           /* p4 */
                           0,                           /* p5 */
                           0);                          /* p6 */

        if (status != SS$_NORMAL) LIB$STOP (status);
        if (iosb.status != SS$_NORMAL) LIB$STOP (iosb.status);
}

/*
   The following procedure deposits a message into the mailbox.
*/

void send_mailbox ()

{
        status = SYS$QIOW (0,                   /* efn */
                           mbx_channel,         /* chan */
                           IO$_WRITEVBLK,       /* func */
                           &iosb,               /* iosb */
                           0,                   /* astadr */
                           0,                   /* astprm */
                           mbx_msg,             /* p1 */
                           mbx_msg_len,         /* p2 */
                           0,                   /* p3 */
                           0,                   /* p4 */
                           0,                   /* p5 */
                           0);                  /* p6 */

        if (status != SS$_NORMAL) LIB$STOP (status);
        if (iosb.status != SS$_NORMAL) LIB$STOP (iosb.status);
}
