/********************************************************************************************************************************/
/* Created: 30-MAY-1996 15:09:06 by OpenVMS SDL EV1-39     */
/* Source:  30-MAY-1996 15:09:02 AC_SOURCE:[LOCAL.FLIST]FATDEF.SDL;3 */
/********************************************************************************************************************************/
/*** MODULE FATDEF ***/

#ifdef __DECC
#pragma __member_alignment __save
#pragma __nomember_alignment
#endif

struct FATDEF {
    unsigned char fat$b_rtype;          /* the record type                  */
    unsigned char fat$b_rattrib;
    unsigned short int fat$w_rsize;
    unsigned short int fat$w_hiblkh;
    unsigned short int fat$w_hiblkl;
    unsigned short int fat$w_efblkh;
    unsigned short int fat$w_efblkl;
    unsigned short int fat$w_ffbyte;
    unsigned char fat$b_bktsize;
    unsigned char fat$b_vfcsize;
    unsigned short int fat$w_maxrec;
    unsigned short int fat$w_defext;
    unsigned short int fat$w_gbc;
    unsigned short int fat$w_spare[4];
    unsigned short int fat$w_versions;
    } ;
 
#ifdef __DECC
#pragma __member_alignment __restore
#endif
