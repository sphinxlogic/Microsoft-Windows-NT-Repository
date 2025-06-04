/********************************************************************************************************************************/
/* Created  3-NOV-1993 10:24:45 by VAX SDL T3.2-8      Source:  3-NOV-1993 10:24:20 UAFDEF.SDL;3 */
/********************************************************************************************************************************/
 
/*** MODULE $UAFDEF ***/
/*++                                                                        */
/*                                                                          */
/* Subset user authorization file format.  This file contains the authorization */
/* file format definitions needed by the DECwindows Authorize utility.  This is */
/* a subset of the full authorization file format.                          */
/*                                                                          */
/*--                                                                        */
#define UAF$C_USER_ID 1                 /* main user ID record              */
#define UAF$C_VERSION1 1                /* this version                     */
#define UAF$C_AD_II 0                   /* AUTODIN-II 32 bit crc code       */
#define UAF$C_PURDY 1                   /* Purdy polynomial over salted input */
#define UAF$C_PURDY_V 2                 /* Purdy polynomial + variable length username */
#define UAF$C_PURDY_S 3                 /* PURDY_V folded into password length */
#define UAF$K_CURRENT_ALGORITHM 3       /* current DEC algorithm number     */
#define UAF$C_CURRENT_ALGORITHM 3       /* current DEC algorithm number     */
#define UAF$K_FIXED 644                 /* length of fixed portion          */
#define UAF$C_FIXED 644                 /* length of fixed portion          */
#define UAF$K_LENGTH 1412
#define UAF$C_LENGTH 1412
struct UAFDEF {
    unsigned char UAF$B_RTYPE;          /* UAF record type                  */
    unsigned char UAF$B_VERSION;        /* UAF format version               */
    unsigned char UAF$b_xxx_FILL_1 [2]; /* Not used by DECwindows Authorize utility */
    union  {
        char UAF$T_USERNAME [32];       /* username                         */
        struct  {
            char UAF$T_FILL_0 [31];
            } UAF$r_fill_1;
        } UAF$r_fill_0;
    union  {
        unsigned long int UAF$L_UIC;    /* user ID code                     */
        struct  {
            unsigned short int UAF$W_MEM; /* member subfield                */
            unsigned short int UAF$W_GRP; /* group subfield                 */
            } UAF$r_fill_3;
        } UAF$r_fill_2;
    unsigned long int UAF$L_SUB_ID;     /* user sub-identifier              */
    unsigned int UAF$Q_PARENT_ID [2];   /* identifier of owner of this account */
    unsigned char UAF$b_xxx_FILL_3 [288]; /* Not used by DECwindows Authorize utility */
    union  {
        unsigned int UAF$Q_PWD [2];     /* hashed password                  */
        struct  {
            unsigned char UAF$b_xxx_FILL_4 [4]; /* Not used by DECwindows Authorize utility */
            } UAF$r_fill_5;
        } UAF$r_fill_4;
    unsigned int UAF$Q_PWD2 [2];        /* second password                  */
    unsigned char UAF$b_xxx_FILL_5 [2]; /* Not used by DECwindows Authorize utility */
    unsigned short int UAF$W_SALT;      /* random password salt             */
    unsigned char UAF$B_ENCRYPT;        /* primary password hash algorithm  */
    unsigned char UAF$B_ENCRYPT2;       /* secondary password hash algorithm */
    unsigned char UAF$b_xxx_FILL_6 [18]; /* Not used by DECwindows Authorize utility */
    unsigned int UAF$Q_PWD_DATE [2];    /* date of password change          */
    unsigned int UAF$Q_PWD2_DATE [2];   /* date of 2nd password change      */
    unsigned char UAF$b_xxx_FILL_8 [248]; /* Not used by DECwindows Authorize utility */
    unsigned char UAF$b_xxx_FILL_9 [768]; /* Not used by DECwindows Authorize utility */
    } ;
