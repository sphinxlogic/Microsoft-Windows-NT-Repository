#module uaf_global "X-2"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This module contains the global storage definitions and
**		the global storage initialization routine.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:  25-Nov-1992
**
**  MODIFICATION HISTORY:
**
**	X-2	LMP		L. Mark Pilant,		28-JAN-1993  13:24
**		Convert to C from BLISS sources.
**
**	X-1	LMP		L. Mark Pilant,		25-NOV-1992
**		Original version.
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include string

#include <decw$include:mrmappl>

#include "uaf_header"

/* Define the macro used to populate the formatting table. */

#define		ITEM_ENTRY(uai_code, widget_code, bytcnt, faoctrstr, flags, padchr)					\
		    {													\
		    uaf_r_item_table[uai_code].item_l_widget = widget_code;						\
		    uaf_r_item_table[uai_code].item_r_flag_overlay.item_b_flags = flags;				\
		    uaf_r_item_table[uai_code].item_b_pad_char = padchr;						\
		    uaf_r_item_table[uai_code].item_w_byte_cnt = bytcnt;						\
		    strcpy (uaf_r_item_table[uai_code].item_r_fao_ctrs, faoctrstr);					\
		    uaf_r_item_table[uai_code].item_l_fao_ctrs_len = strlen (faoctrstr);				\
		    uaf_r_item_table[uai_code].item_a_fao_ctrs_ptr = &uaf_r_item_table[uai_code].item_r_fao_ctrs;	\
		    }

/* The following storage is used for the user authorization information.
** This information is (defined) local to this routine; only the actual
** item list is defined globally.
**
** This information is copied from the static item list (here) to the
** dynamic item list.
**
**  The storage quantities must parallel the definitions in UAFDEF.H
*/

static char			uaf_t_usr_account[MAX_ACCOUNT];		/* UAF$T_ACCOUNT */
static unsigned short int	uaf_w_usr_astlm;			/* UAF$W_ASTLM */
static unsigned char		uaf_b_batch_access_p[ACCESS_SIZE];	/* UAF$B_BATCH_ACCESS_P */
static unsigned char		uaf_b_batch_access_s[ACCESS_SIZE];	/* UAF$B_BATCH_ACCESS_S */
static unsigned short int	uaf_w_usr_biolm;			/* UAF$W_BIOLM */
static unsigned long int	uaf_l_usr_bytlm;			/* UAF$L_BYTLM */
static char			uaf_t_usr_clitables[MAX_CLITABLE];	/* UAF$T_CLITABLES */
static unsigned long int	uaf_l_usr_cputim;			/* UAF$L_CPUTIM */
static unsigned char		uaf_q_usr_def_priv[QUADWORD];		/* UAF$Q_DEF_PRIV */
static char			uaf_t_usr_defcli[MAX_DEFCLI];		/* UAF$T_DEFCLI */
static char			uaf_t_usr_defdev[MAX_DEFDEV];		/* UAF$T_DEFDEV */
static char			uaf_t_usr_defdir[MAX_DEFDIR];		/* UAF$T_DEFDIR */
static unsigned long int	uaf_l_usr_dfwscnt;			/* UAF$L_DFWSCNT */
static unsigned char		uaf_b_dialup_access_p[ACCESS_SIZE];	/* UAF$B_DIALUP_ACCESS_P */
static unsigned char		uaf_b_dialup_access_s[ACCESS_SIZE];	/* UAF$B_DIALUP_ACCESS_S */
static unsigned short int	uaf_w_usr_diolm;			/* UAF$W_DIOLM */
static unsigned short int	uaf_w_usr_enqlm;			/* UAF$W_ENQLM */
static unsigned char		uaf_q_usr_expiration[QUADWORD];		/* UAF$Q_EXPIRATION */
static unsigned short int	uaf_w_usr_fillm;			/* UAF$W_FILLM */
static unsigned long int	uaf_l_usr_flags;			/* UAF$L_FLAGS */
static unsigned long int	uaf_l_usr_jtquota;			/* UAF$L_JTQUOTA */
static unsigned char		uaf_q_usr_lastlogin_i[QUADWORD];	/* UAF$Q_LASTLOGIN_I */
static unsigned char		uaf_q_usr_lastlogin_n[QUADWORD];	/* UAF$Q_LASTLOGIN_N */
static char			uaf_t_usr_lgicmd[MAX_LGICMD];		/* UAF$T_LGICMD */
static unsigned char		uaf_b_local_access_p[ACCESS_SIZE];	/* UAF$B_LOCAL_ACCESS_P */
static unsigned char		uaf_b_local_access_s[ACCESS_SIZE];	/* UAF$B_LOCAL_ACCESS_S */
static unsigned short int	uaf_w_usr_logfails;			/* UAF$W_LOGFAILS */
static unsigned short int	uaf_w_usr_maxacctjobs;			/* UAF$W_MAXACCTJOBS */
static unsigned short int	uaf_w_usr_maxdetach;			/* UAF$W_MAXDETACH */
static unsigned short int	uaf_w_usr_maxjobs;			/* UAF$W_MAXJOBS */
static unsigned char		uaf_b_network_access_p[ACCESS_SIZE];	/* UAF$B_NETWORK_ACCESS_P */
static unsigned char		uaf_b_network_access_s[ACCESS_SIZE];	/* UAF$B_NETWORK_ACCESS_S */
static char			uaf_t_usr_owner[MAX_OWNER];		/* UAF$T_OWNER */
static unsigned long int	uaf_l_usr_pbytlm;			/* UAF$L_PBYTLM */
static unsigned long int	uaf_l_usr_pgflquota;			/* UAF$L_PGFLQUOTA */
static unsigned short int	uaf_w_usr_prccnt;			/* UAF$W_PRCCNT */
static unsigned char		uaf_b_usr_pri;				/* UAF$B_PRI */
static unsigned char		uaf_b_usr_primedays;			/* UAF$B_PRIMEDAYS */
static unsigned char		uaf_q_usr_priv[QUADWORD];		/* UAF$Q_PRIV */
static unsigned char		uaf_q_usr_pwd_date[QUADWORD];		/* UAF$Q_PWD_DATE */
static unsigned char		uaf_b_usr_pwd_length;			/* UAF$B_PWD_LENGTH */
static unsigned char		uaf_q_usr_pwd_lifetime[QUADWORD];	/* UAF$Q_PWD_LIFETIME */
static unsigned char		uaf_q_usr_pwd2_date[QUADWORD];		/* UAF$Q_PWD2_DATE */
static unsigned char		uaf_b_usr_quepri;			/* UAF$B_QUEPRI */
static unsigned char		uaf_b_remote_access_p[ACCESS_SIZE];	/* UAF$B_REMOTE_ACCESS_P */
static unsigned char		uaf_b_remote_access_s[ACCESS_SIZE];	/* UAF$B_REMOTE_ACCESS_S */
static unsigned short int	uaf_w_usr_shrfillm;			/* UAF$W_SHRFILLM */
static unsigned short int	uaf_w_usr_tqcnt;			/* UAF$W_TQCNT */
static unsigned long int	uaf_l_usr_uic;				/* UAF$L_UIC */
static char			uaf_t_usr_username[MAX_USERNAME];	/* UAF$T_USERNAME */
static unsigned long int	uaf_l_usr_wsextent;			/* UAF$L_WSEXTENT */
static unsigned long int	uaf_l_usr_wsquota;			/* UAF$L_WSQUOTA */

/*
**
**  DEFINE THE GLOBAL STORAGE.
**
*/

/* Storage needed by several modules. */

globaldef char			*default_sysuaf_file_name = "SYSUAF";	/* Authorization file default file name */
globaldef char			*default_sysuaf_file_type = ".DAT";	/* Authorization file default file type */

/* Define the default file specification to use for the authorization
** file.  The rules for determining this are a little strange:
**
** If the NONSHARED_UAI_CONTEXT symbol is defined, it means this utility
** is to run on a VMS version 5.x system.  In this situation, the $GETUAI and
** $SETUAI system services to not know how to deal with the authorization
** file being opened outside the services.  For this reason, the default
** specification should be SYS$SYSTEM:SYSUAF.DAT, to allow the (internal)
** $CREUAI and $DELUAI services to use the same file.
**
** If the NONSHARED_UAI_CONTEXT symbol is not defined, it means this utility
** is to run on a VMS version 6.x system.  In this situation, the $GETUAI
** and $SETUAI system service are able to deal with the authorization
** file being opened outside the services.  (Because of changes made to
** the context is handled and exec mode accessing of files.) */

#ifdef NONSHARED_UAI_CONTEXT
globaldef char			*default_sysuaf_file_spec = "SYS$SYSTEM:SYSUAF.DAT";	/* Authorization filed default spec */
#else
globaldef char			*default_sysuaf_file_spec = "SYS$DISK:SYSUAF.DAT";	/* Authorization filed default spec */
#endif /* NONSHARED_UAI_CONTEXT */

/* The following storage is defined first, to allow storage following to
** be correctly sized.
**
** NOTE:  These lists are maintained in memory to allow for the insertion
**	  of new entries in alphabetical order.  Because it is not possible
**	  to get all the entries currently in a list box, there is no way
**	  to determine where a new entry is to be inserted, without outside
**	  (the list box) help. */

globaldef struct USRLSTDEF	uaf_r_group_list = 0;				/* Group user name list head */
globaldef struct USRLSTDEF	uaf_r_master_list = 0;				/* Complete user list (from the file) */
globaldef struct USRLSTDEF	uaf_r_user_list = 0;				/* User name list head */

/* Remaining global storage. */

globaldef unsigned int		uaf_l_current_group_max;			/* Count of entries in group list */
globaldef unsigned int		uaf_l_current_user_index;			/* Index of current user */
globaldef unsigned int		uaf_l_current_user_max;				/* Count of entries in user list */
globaldef unsigned int		uaf_l_current_user_uic;				/* Current user UIC */
globaldef int			uaf_l_main_operation;				/* Operation (i.e., create, modify, etc.) */
globaldef Widget		uaf_r_access_pri_window_widget;			/* Primary access restriction window widget info */
globaldef Widget		uaf_r_access_sec_window_widget;			/* Secondary access restriction window widget info */
globaldef XtAppContext		uaf_r_app_context;				/* Application context */
globaldef struct ITMDEF		(*uaf_r_current_group_info)[] = 0;		/* Current group UAF info */
globaldef struct ITMDEF		(*uaf_r_current_user_info)[] = 0;		/* Current user UAF info */
globaldef Display		*uaf_r_entry_display;				/* Display id for authorization entry window */
globaldef Widget		uaf_r_entry_popup_menu_widget;			/* Entry window popup menu */
globaldef Widget		uaf_r_entry_top_level_widget;			/* Entry window widget shell info */
globaldef Widget		uaf_r_entry_window_widget;			/* Entry window widget info */
globaldef Widget		uaf_r_error_dialog_widget;			/* Error dialog box widget */
globaldef Display		*uaf_r_file_display;				/* Display id for file selection window */
globaldef Widget		uaf_r_file_top_level_widget;			/* File selction widget shell info */
globaldef Widget		uaf_r_file_window_widget;			/* File selection widget info */
globaldef struct UAFLAG		uaf_r_flags;					/* Miscellaneous utility flags */
globaldef Widget		uaf_r_id_popup_menu_widget;			/* Identifier window popup menu */
globaldef Widget		uaf_r_identifier_window_widget;			/* Identifier window widget info */
globaldef struct FMTABL		uaf_r_item_table[UAI$_MAX_ITEM_CODE];		/* Entry window info format table */
globaldef Display		*uaf_r_main_display;				/* Display id for main window */
globaldef Widget		uaf_r_main_shell_widget;			/* Main window widget shell info */
globaldef Widget		uaf_r_main_top_level_widget;			/* Main window widget shell info */
globaldef Widget		uaf_r_main_window_widget;			/* Main window widget info */
globaldef MrmHierarchy		uaf_r_mrm_hierarchy;				/* Hierarchy info for .UID file(s) */
globaldef Widget		uaf_r_system_id_window_widget;			/* System identifier window widget info */
globaldef struct ITMDEF		(*uaf_r_template_info)[];			/* Modification template information */
globaldef Cursor		uaf_r_wait_cursor_entry;			/* Entry window wait cursor id */
globaldef Cursor		uaf_r_wait_cursor_main;				/* Main window wait cursor id */
globaldef Widget		uaf_r_widget_id_array[uaf$c_max_widget_code];	/* Array of Widget IDs in use */
globaldef char			uaf_t_current_groupname[sizeof uaf_r_group_list.uaf$t_usrlst_name];	/* Current group name */
globaldef char			uaf_t_current_username[sizeof uaf_r_user_list.uaf$t_usrlst_name];	/* Current username */
globaldef char			*uaf_t_default_filter = "*";			/* Default username filter */
globaldef char			*uaf_t_default_username = "DEFAULT";		/* Default account username */
globaldef char			*uaf_t_no_date_time = "(none)";			/* String to use when no date/time present */
globaldef char			*uaf_t_pre_expired = "(pre-expired)";		/* String to use when no pwd chg dat/time present */
globaldef char			uaf_t_username_filter[sizeof uaf_r_user_list.uaf$t_usrlst_name];	/* Current username filter string */
globaldef unsigned short int	uaf_w_uai_context[2];				/* $xxxUAI context cell */

globaldef struct ITMDEF		uaf_r_user_auth_info[] =			/* $xxxUAI item list */
				{
				 {sizeof uaf_t_usr_account,		UAI$_ACCOUNT,		uaf_t_usr_account,	0},
				 {sizeof uaf_w_usr_astlm,		UAI$_ASTLM,		&uaf_w_usr_astlm,	0},
				 {sizeof uaf_b_batch_access_p,		UAI$_BATCH_ACCESS_P,	&uaf_b_batch_access_p,	0},
				 {sizeof uaf_b_batch_access_s,		UAI$_BATCH_ACCESS_S,	&uaf_b_batch_access_s,	0},
				 {sizeof uaf_w_usr_biolm,		UAI$_BIOLM,		&uaf_w_usr_biolm,	0},
				 {sizeof uaf_l_usr_bytlm,		UAI$_BYTLM,		&uaf_l_usr_bytlm,	0},
				 {sizeof uaf_t_usr_clitables,		UAI$_CLITABLES,		uaf_t_usr_clitables,	0},
				 {sizeof uaf_l_usr_cputim,		UAI$_CPUTIM,		&uaf_l_usr_cputim,	0},
				 {sizeof uaf_q_usr_def_priv,		UAI$_DEF_PRIV,		uaf_q_usr_def_priv,	0},
				 {sizeof uaf_t_usr_defcli,		UAI$_DEFCLI,		uaf_t_usr_defcli,	0},
				 {sizeof uaf_t_usr_defdev,		UAI$_DEFDEV,		uaf_t_usr_defdev,	0},
				 {sizeof uaf_t_usr_defdir,		UAI$_DEFDIR,		uaf_t_usr_defdir,	0},
				 {sizeof uaf_l_usr_dfwscnt,		UAI$_DFWSCNT,		&uaf_l_usr_dfwscnt,	0},
				 {sizeof uaf_b_dialup_access_p,		UAI$_DIALUP_ACCESS_P,	&uaf_b_dialup_access_p,	0},
				 {sizeof uaf_b_dialup_access_s,		UAI$_DIALUP_ACCESS_S,	&uaf_b_dialup_access_s,	0},
				 {sizeof uaf_w_usr_diolm,		UAI$_DIOLM,		&uaf_w_usr_diolm,	0},
				 {sizeof uaf_w_usr_enqlm,		UAI$_ENQLM,		&uaf_w_usr_enqlm,	0},
				 {sizeof uaf_q_usr_expiration,		UAI$_EXPIRATION,	uaf_q_usr_expiration,	0},
				 {sizeof uaf_w_usr_fillm,		UAI$_FILLM,		&uaf_w_usr_fillm,	0},
				 {sizeof uaf_l_usr_flags,		UAI$_FLAGS,		&uaf_l_usr_flags,	0},
				 {sizeof uaf_l_usr_jtquota,		UAI$_JTQUOTA,		&uaf_l_usr_jtquota,	0},
				 {sizeof uaf_q_usr_lastlogin_i,		UAI$_LASTLOGIN_I,	uaf_q_usr_lastlogin_i,	0},
				 {sizeof uaf_q_usr_lastlogin_n,		UAI$_LASTLOGIN_N,	uaf_q_usr_lastlogin_n,	0},
				 {sizeof uaf_t_usr_lgicmd,		UAI$_LGICMD,		uaf_t_usr_lgicmd,	0},
				 {sizeof uaf_b_local_access_p,		UAI$_LOCAL_ACCESS_P,	&uaf_b_local_access_p,	0},
				 {sizeof uaf_b_local_access_s,		UAI$_LOCAL_ACCESS_S,	&uaf_b_local_access_s,	0},
				 {sizeof uaf_w_usr_logfails,		UAI$_LOGFAILS,		&uaf_w_usr_logfails,	0},
				 {sizeof uaf_w_usr_maxacctjobs,		UAI$_MAXACCTJOBS,	&uaf_w_usr_maxacctjobs,	0},
				 {sizeof uaf_w_usr_maxdetach,		UAI$_MAXDETACH,		&uaf_w_usr_maxdetach,	0},
				 {sizeof uaf_w_usr_maxjobs,		UAI$_MAXJOBS,		&uaf_w_usr_maxjobs,	0},
				 {sizeof uaf_b_network_access_p,	UAI$_NETWORK_ACCESS_P,	&uaf_b_network_access_p,0},
				 {sizeof uaf_b_network_access_s,	UAI$_NETWORK_ACCESS_S,	&uaf_b_network_access_s,0},
				 {sizeof uaf_t_usr_owner,		UAI$_OWNER,		uaf_t_usr_owner,	0},
				 {sizeof uaf_l_usr_pbytlm,		UAI$_PBYTLM,		&uaf_l_usr_pbytlm,	0},
				 {sizeof uaf_l_usr_pgflquota,		UAI$_PGFLQUOTA,		&uaf_l_usr_pgflquota,	0},
				 {sizeof uaf_w_usr_prccnt,		UAI$_PRCCNT,		&uaf_w_usr_prccnt,	0},
				 {sizeof uaf_b_usr_pri,			UAI$_PRI,		&uaf_b_usr_pri,		0},
				 {sizeof uaf_b_usr_primedays,		UAI$_PRIMEDAYS,		&uaf_b_usr_primedays,	0},
				 {sizeof uaf_q_usr_priv,		UAI$_PRIV,		uaf_q_usr_priv,		0},
				 {sizeof uaf_q_usr_pwd_date,		UAI$_PWD_DATE,		uaf_q_usr_pwd_date,	0},
				 {sizeof uaf_b_usr_pwd_length,		UAI$_PWD_LENGTH,	&uaf_b_usr_pwd_length,	0},
				 {sizeof uaf_q_usr_pwd_lifetime,	UAI$_PWD_LIFETIME,	uaf_q_usr_pwd_lifetime,	0},
				 {sizeof uaf_q_usr_pwd2_date,		UAI$_PWD2_DATE,		uaf_q_usr_pwd2_date,	0},
				 {sizeof uaf_b_usr_quepri,		UAI$_QUEPRI,		&uaf_b_usr_quepri,	0},
				 {sizeof uaf_b_remote_access_p,		UAI$_REMOTE_ACCESS_P,	&uaf_b_remote_access_p,	0},
				 {sizeof uaf_b_remote_access_s,		UAI$_REMOTE_ACCESS_S,	&uaf_b_remote_access_s,	0},
				 {sizeof uaf_w_usr_shrfillm,		UAI$_SHRFILLM,		&uaf_w_usr_shrfillm,	0},
				 {sizeof uaf_w_usr_tqcnt,		UAI$_TQCNT,		&uaf_w_usr_tqcnt,	0},
				 {sizeof uaf_l_usr_uic,			UAI$_UIC,		&uaf_l_usr_uic,		0},
				 {sizeof uaf_t_usr_username,		UAI$_USERNAME,		uaf_t_usr_username,	0},
				 {sizeof uaf_l_usr_wsextent,		UAI$_WSEXTENT,		&uaf_l_usr_wsextent,	0},
				 {sizeof uaf_l_usr_wsquota,		UAI$_WSQUOTA,		&uaf_l_usr_wsquota,	0},

/* Terminate the item list. */

				 {0,				0,			0,			0}
				};

extern void AUTHORIZE$INIT_GLOBAL ()
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is called to initialize all the global storage.
**
**  FORMAL PARAMETERS:
**
**	None
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      None
**
**--
*/

/* Initialize the global strings. */

memset (uaf_t_current_groupname, 0, sizeof uaf_t_current_groupname);
memset (uaf_t_current_username, 0, sizeof uaf_t_current_username);
memset (uaf_t_username_filter, 0, sizeof uaf_t_username_filter);

/* Initialize the widget ID array. */

memset (uaf_r_widget_id_array, 0, sizeof uaf_r_widget_id_array);

/* Initialize the dynamic item lists. /

uaf_r_current_group_info = 0;
uaf_r_current_user_info = 0;

/* Set up an empty template item list. */

uaf_r_template_info = calloc (1, 4);				/* Just the longword terminator */

/* Set up the $xxUAI context cell. */

uaf_w_uai_context[0] = -1;
uaf_w_uai_context[1] = -1;

/* Initialize the master, group, and user list heads. */

uaf_r_group_list.uaf$l_usrlst_flink = &uaf_r_group_list.uaf$l_usrlst_flink;
uaf_r_group_list.uaf$l_usrlst_blink = &uaf_r_group_list.uaf$l_usrlst_flink;

uaf_r_master_list.uaf$l_usrlst_flink = &uaf_r_master_list.uaf$l_usrlst_flink;
uaf_r_master_list.uaf$l_usrlst_blink = &uaf_r_master_list.uaf$l_usrlst_flink;

uaf_r_user_list.uaf$l_usrlst_flink = &uaf_r_user_list.uaf$l_usrlst_flink;
uaf_r_user_list.uaf$l_usrlst_blink = &uaf_r_user_list.uaf$l_usrlst_flink;

/* Initialize the item formatting table.  This is done at run time because I
** do not know how to get C to set up the table at compile time (without having
** to fill each entry in order). */

ITEM_ENTRY (UAI$_ACCOUNT,	uaf$c_entry_account,		MAX_ACCOUNT,	"",	ITEM_M_FLAG_ASC +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_ASTLM,		uaf$c_entry_astlm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_BIOLM,		uaf$c_entry_biolm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_BYTLM,		uaf$c_entry_bytlm,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_CLITABLES,	uaf$c_entry_clitable,		MAX_CLITABLE,	"",	ITEM_M_FLAG_CSTR +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_CPUTIM,	uaf$c_entry_cpu,		LONGWORD,	"",	ITEM_M_FLAG_SPC,	'\0');
ITEM_ENTRY (UAI$_DEFCLI,	uaf$c_entry_cli,		MAX_DEFCLI,	"",	ITEM_M_FLAG_CSTR +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_DEFDEV,	0,				MAX_DEFDEV,	"",	ITEM_M_FLAG_CSTR +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_DEFDIR,	0,				MAX_DEFDIR,	"",	ITEM_M_FLAG_CSTR +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_DFWSCNT,	uaf$c_entry_wsdef,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_DIOLM,		uaf$c_entry_diolm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_ENQLM,		uaf$c_entry_enqlm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_EXPIRATION,	uaf$c_entry_expiration,		QUADWORD,	"",	ITEM_M_FLAG_DAT,	'\0');
ITEM_ENTRY (UAI$_FILLM,		uaf$c_entry_fillm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_JTQUOTA,	uaf$c_entry_jtquota,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_LASTLOGIN_I,	uaf$c_entry_lastlogin_i,	QUADWORD,	"",	ITEM_M_FLAG_DAT,	'\0');
ITEM_ENTRY (UAI$_LASTLOGIN_N,	uaf$c_entry_lastlogin_n,	QUADWORD,	"",	ITEM_M_FLAG_DAT,	'\0');
ITEM_ENTRY (UAI$_LGICMD,	uaf$c_entry_lgicmd,		MAX_LGICMD,	"",	ITEM_M_FLAG_CSTR +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_LOGFAILS,	uaf$c_entry_logfails,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_MAXACCTJOBS,	uaf$c_entry_maxacctjobs,	WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_MAXDETACH,	uaf$c_entry_maxdetach,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_MAXJOBS,	uaf$c_entry_maxjobs,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_OWNER,		uaf$c_entry_owner,		MAX_OWNER,	"",	ITEM_M_FLAG_CSTR +
											ITEM_M_FLAG_MAX,	' ');
ITEM_ENTRY (UAI$_PBYTLM,	uaf$c_entry_pbytlm,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_PGFLQUOTA,	uaf$c_entry_pgflquo,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_PRCCNT,	uaf$c_entry_prclm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_PRI,		uaf$c_entry_prio,		BYTE,		"!UB",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_PWD_DATE,	uaf$c_entry_pwd_date,		QUADWORD,	"",	ITEM_M_FLAG_DAT,	'\0');
ITEM_ENTRY (UAI$_PWD_LENGTH,	uaf$c_entry_pwd_length,		BYTE,		"!UB",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_PWD_LIFETIME,	uaf$c_entry_pwd_lifetime,	QUADWORD,	"",	ITEM_M_FLAG_DAT,	'\0');
ITEM_ENTRY (UAI$_PWD2_DATE,	uaf$c_entry_pwd2_date,		QUADWORD,	"",	ITEM_M_FLAG_DAT,	'\0');
ITEM_ENTRY (UAI$_QUEPRI,	uaf$c_entry_queprio,		BYTE,		"!UB",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_SHRFILLM,	uaf$c_entry_shrfillm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_TQCNT,		uaf$c_entry_tqelm,		WORD,		"!UW",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_UIC,		uaf$c_entry_uic,		LONGWORD,	"!%U",	ITEM_M_FLAG_SPC,	'\0');
ITEM_ENTRY (UAI$_USERNAME,	uaf$c_entry_username,		MAX_USERNAME,	"",	ITEM_M_FLAG_ASC,	' ');
ITEM_ENTRY (UAI$_WSEXTENT,	uaf$c_entry_wsextent,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');
ITEM_ENTRY (UAI$_WSQUOTA,	uaf$c_entry_wsquo,		LONGWORD,	"!UL",	ITEM_M_FLAG_VAL,	'\0');

/* Other miscellaneous initialization. */

uaf_l_current_group_max = 0;
uaf_l_current_user_index = 0;
uaf_l_current_user_max = 0;

}
