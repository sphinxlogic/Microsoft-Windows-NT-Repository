/********************************************************************************************************************************/
/* Created  1-NOV-1993 13:00:43 by VAX SDL T3.2-8      Source: 11-OCT-1993 16:15:30 UAFPVTDEF.SDL;5 */
/********************************************************************************************************************************/
/* Define the basic format of an itemlist entry.                            */
 
/*** MODULE ITMLST ***/
#define itm$c_length 12                 /* Length of structure              */
#define itm$k_length 12
struct ITMDEF {
    unsigned short int itm$w_bufsiz;    /* Size of the item buffer          */
    unsigned short int itm$w_itmcod;    /* Item entry code                  */
    unsigned long int itm$l_bufadr;     /* Address of item buffer           */
    unsigned long int itm$l_retlen;     /* Address of where to return information */
    } ;
/* Define the format of a user list entry.                                  */
 
/*** MODULE USRLST ***/
#define uaf$c_usrlst_length 128         /* Structure length                 */
#define uaf$k_usrlst_length 128
struct USRLSTDEF {
    unsigned long int uaf$l_usrlst_flink; /* Forward link                   */
    unsigned long int uaf$l_usrlst_blink; /* Backward link                  */
    unsigned long int uaf$l_usrlst_own_id; /* Account owner UIC (binary)    */
    char uaf$t_usrlst_name [33];        /* Username storage (assumes UAF$S_USERNAME = 32) */
    char uaf$t_usrlst_uic [15];         /* UIC storage (group number, member number & three delimiters) */
    char uaf$t_usrlst_uic_id [68];      /* UIC storage (assumes KGB$S_NAME = 32: two identifiers & three delimiters) */
    } ;
/* Define the format of an item table entry (used by xxxUAI).               */
 
/*** MODULE ITMTAB ***/

#ifdef __DECC
#pragma member_alignment __save
#pragma nomember_alignment
#endif

#define uaf$c_itm_ent_subset_copy 0     /* Copy value supplied              */
#define uaf$c_itm_ent_subset_mask 1     /* Bit mask                         */
#define uaf$c_itm_ent_subset_value 2    /* Simple value                     */
#define uaf$c_itm_ent_subset_uic 3      /* Check UIC                        */
#define uaf$c_itm_ent_subset_cpu 4      /* Check CPU time                   */
#define uaf$m_itm_flg_rdonly 1
#define uaf$m_itm_flg_spfill 2
#define uaf$m_itm_flg_wrtonly 4
#define uaf$m_itm_flg_var 8
#define uaf$m_itm_flg_nop 16
#define uaf$c_itm_ent_length 16         /* Structure length                 */
#define uaf$k_itm_ent_length 16
struct ITMTABDEF {
    unsigned long int uaf$l_itm_ent_audit_0; /* item auditing flags, part 1 */
    unsigned long int uaf$l_itm_ent_audit_4; /* item auditing flags, part 2 */
    unsigned short int uaf$w_itm_ent_base; /* table entry UAF record offset */
    unsigned short int uaf$w_itm_ent_size; /* table entry size              */
    unsigned short int uaf$w_itm_ent_spare; /* spare word                   */
    unsigned char uaf$b_itm_ent_subtyp; /* subseting type code              */
    union  {
        unsigned char uaf$b_itm_ent_flags; /* needed flags                  */
        struct  {
            unsigned uaf$v_itm_flg_rdonly : 1; /* field is read only        */
            unsigned uaf$v_itm_flg_spfill : 1; /* space (not zero) fill field */
            unsigned uaf$v_itm_flg_wrtonly : 1; /* field is write only      */
            unsigned uaf$v_itm_flg_var : 1; /* accept variable buffer       */
            unsigned uaf$v_itm_flg_nop : 1; /* ignore item code             */
            unsigned uaf$v_fill_2 : 3;
            } uaf$r_fill_1;
        } uaf$r_fill_0;
    } ;
#ifdef __DECC
#pragma member_alignment __restore
#endif
