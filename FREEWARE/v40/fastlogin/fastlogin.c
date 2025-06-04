/*******************************************************************************
 *                                                                             *
 *  Copyright (c) 1996 by Digital Equipment Corporation. All rights reserved.  *
 *                                                                             *
 *  This  software  is  furnished  under a license and may be used and copied  *
 *  only  in accordance with the terms of such license and with the inclusion  *
 *  of the above copyright notice.  This software or any other copies thereof  *
 *  may  not  be  provided  or  otherwise made available to any other person.  *
 *  No title to and ownership of the software is hereby transferred.           *
 *                                                                             *
 *  The  information in this software is subject to change without notice and  *
 *  should not be construed as a commitment by Digital Equipment Corporation.  *
 *                                                                             *
 *  Digital  assumes  no  responsibility  for  the  use or reliability of its  *
 *  software on equipment that is not supplied by Digital.                     *
 *                                                                             *
 *  Support  for  this  software  is  not  covered under any Digital software  *
 *  product  support  contract,  but  may  be provided under the terms of the  *
 *  consulting agreement under which this software was developed.              *
 *                                                                             *
 *******************************************************************************
 *++
 *
 *  Facility: FastLogin Facility for OpenVMS
 *
 *  Abstract: This module contains the code for the FastLogin shareable
 *            image. This image contains LOGINOUT.EXE callouts which
 *            implement the FastLogin features.
 *
 *  Author:   Thierry Lelegard, Digital Equipment France
 *
 *  Creation: 3-Jan-1996
 *
 *  Modification history:
 *
 *      - 15-Apr-1996, T.Lelegard, Replaced text-file database by
 *        standard DECnet proxy database. Added implicit intra-cluster
 *        login using logical name FASTLOGIN_ALLOW_CLUSTER.
 *
 *      - 17-Apr-1996, T.Lelegard, Added incoming LAT support. Make
 *        usage of proxies optional (check logical names). Add opcom
 *        support for trace message.
 */


#if __VMS_VER < 60100000
#error "FastLogin requires OpenVMS V6.1 or higher"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <signal.h>
#include <ssdef.h>
#include <stsdef.h>
#include <opcdef.h>
#include <prcdef.h>
#include <psldef.h>
#include <lnmdef.h>
#include <prvdef.h>
#include <descrip.h>
#include <rms.h>
#include <starlet.h>

#define OUTPUT_DEVICE_LOGNAME        "FASTLOGIN_OUTPUT_DEVICE"
#define OUTPUT_DEVICE_OPCOM          "OPCOM"
#define OUTPUT_DEVICE_DEFSPEC        "SYS$MANAGER:FASTLOGIN.LOG"
#define ALLOW_CLUSTER_LOGNAME        "FASTLOGIN_ALLOW_CLUSTER"
#define ALLOW_DECNET_CLUSTER_LOGNAME "FASTLOGIN_ALLOW_DECNET_CLUSTER"
#define ALLOW_LAT_CLUSTER_LOGNAME    "FASTLOGIN_ALLOW_LAT_CLUSTER"
#define ALLOW_DECNET_PROXIES_LOGNAME "FASTLOGIN_ALLOW_DECNET_PROXIES"
#define ALLOW_LAT_PROXIES_LOGNAME    "FASTLOGIN_ALLOW_LAT_PROXIES"
#define ALIAS_LOGNAME_1              "FASTLOGIN_CLUSTER_ALIAS"
#define ALIAS_LOGNAME_2              "SYS$CLUSTER_NODE"
#define TRACE_LOGNAME                "FASTLOGIN_TRACE"
#define KEY_LOGNAME                  "FASTLOGIN_KEY"
#define KEY_CHARACTER                '#'
#define PROMPT_PREFIX                "\r\n"
#define PROMPT_SUFFIX                ": "
#define PROMPT_USERNAME              "Username"

#define MAX_ARGS            255
#define MAX_OUTPUT_LINE    1024
#define MAX_INPUT_LINE      255


/*
 *  The following variables describe the site-specific configuration.
 *  They are set during the initilization of the session.
 */

static int allow_decnet_cluster = 0;
static int allow_lat_cluster    = 0;
static int allow_decnet_proxies = 0;
static int allow_lat_proxies    = 0;
static int trace_on             = 0;


/*
 *  OpenVMS types
 */

#pragma member_alignment save
#pragma nomember_alignment

struct item {
    short  length;
    short  code;
    void*  buffer;
    short* retlen;
};

struct iosb {
    short status;
    short count;
    long  info;
};

struct opcom {
    int type   :  8;
    int target : 24;
    int rqstid : 32;
    char line [MAX_OUTPUT_LINE];
};

#pragma member_alignment restore


/*
 *  Vector of callout routines.
 */

#pragma member_alignment save
#pragma nomember_alignment

struct lgi$callout_vector {
    long lgi$l_icr_entry_count;
    int (*lgi$icr_init) ();
    int (*lgi$icr_iact_start) ();
    int (*lgi$icr_decwinit) ();
    int (*lgi$icr_identify) ();
    int (*lgi$icr_authenticate) ();
    int (*lgi$icr_chkrestrict) ();
    int (*lgi$icr_finish) ();
    int (*lgi$icr_logout) ();
    int (*lgi$icr_jobstep) ();
    int (*lgi$icr_chklicense) ();
};

#pragma member_alignment restore


/*
 *  Vector of callback routines and data.
 */

#pragma member_alignment save
#pragma nomember_alignment

struct lgi$arg_vector {
    int                      (*lgi$icb_get_input) ();
    int                      (*lgi$icb_decw_ident) ();
    int                      (*lgi$icb_decw_auth) ();
    void                     (*lgi$icb_get_syspwd) ();
    int                      (*lgi$icb_userprompt) ();
    int                      (*lgi$icb_userparse) ();
    int                      (*lgi$icb_autologin) ();
    int                      (*lgi$icb_password) ();
    int                      (*lgi$icb_check_pass) ();
    int                      (*lgi$icb_validate) ();
    void                     (*lgi$icb_acctexpired) ();
    void                     (*lgi$icb_pwdexpired) ();
    int                      (*lgi$icb_disuser) ();
    void                     (*lgi$icb_modalhours) ();
    short*                   lgi$a_icr_creprc_flags;
    char*                    lgi$a_icr_job_type;
    char*                    lgi$a_icr_subprocess;
    char*                    lgi$a_icr_terminal_dev;
    struct dsc$descriptor_s* lgi$a_icr_tt_phydevnam;
    struct dsc$descriptor_s* lgi$a_icr_tt_accpornam;
    struct dsc$descriptor_s* lgi$a_icr_cliname;
    struct dsc$descriptor_s* lgi$a_icr_clitables;
    struct dsc$descriptor_s* lgi$a_icr_ncb;
    int*                     lgi$a_icr_loglink;
    struct dsc$descriptor_s* lgi$a_icr_rem_node_nam;
    struct dsc$descriptor_s* lgi$a_icr_rem_id;
    unsigned char*           lgi$a_icr_uaf_record;
    struct RAB*              lgi$a_icr_input_rab;
    char*                    lgi$a_icr_autologin;
    struct dsc$descriptor_s* lgi$a_icr_username;
    struct dsc$descriptor_s* lgi$a_icr_pwd1;
    struct dsc$descriptor_s* lgi$a_icr_pwd2;
    int*                     lgi$a_icr_pwdcount;
    short*                   lgi$a_icr_netflags;
};

#pragma member_alignment restore


/*
 *  LGI$ status codes (not defined in any header file).
 */

#pragma extern_model save
#pragma extern_model globalvalue

extern int LGI$_SKIPRELATED;
extern int LGI$_DISUSER;
extern int LGI$_INVPWD;
extern int LGI$_NOSUCHUSER;
extern int LGI$_NOTVALID;
extern int LGI$_INVINPUT;
extern int LGI$_CMDINPUT;
extern int LGI$_FILEACC;

#pragma extern_model restore


/*
 *  Routines from this module.
 */

static int callout_iactinit ();
static int callout_identify ();
static int callout_authenticate ();
static int find_trusted (struct lgi$arg_vector*, char*, size_t);
static int octal (const char*, int);
static int is_in_cluster (const struct dsc$descriptor_s*);
static int find_proxy (const struct dsc$descriptor_s*,
    const struct dsc$descriptor_s*, char* name, size_t);
static int terminal_type (struct lgi$arg_vector*, const char*);
static int translate (const char*, const char*, unsigned char, char*, size_t);
static int true_logical (const char*, const char*, unsigned char);
static void output (int, const char*, ...);
static void message (int, int, int, ...);
static int message_output (const struct dsc$descriptor_s*, void*);


/*
 *  Site-specific callout vector.
 *  Initially, the only connected routine is the character-cell login
 *  initialization routine. Further connections will be made here.
 */

#pragma extern_model save
#pragma extern_model strict_refdef

struct lgi$callout_vector LGI$LOGINOUT_CALLOUTS = {
    /* lgi$l_icr_entry_count */  10,
    /* lgi$icr_init */           NULL,
    /* lgi$icr_iact_start */     callout_iactinit,
    /* lgi$icr_decwinit */       NULL,
    /* lgi$icr_identify */       NULL,
    /* lgi$icr_authenticate */   NULL,
    /* lgi$icr_chkrestrict */    NULL,
    /* lgi$icr_finish */         NULL,
    /* lgi$icr_logout */         NULL,
    /* lgi$icr_jobstep */        NULL,
    /* lgi$icr_chklicense */     NULL,
};

#pragma extern_model restore


/*
 *  Initialization routine for interactive character cell login.
 */

static int callout_iactinit (struct lgi$arg_vector* arg, void** context)
{
    trace_on = true_logical (TRACE_LOGNAME, "LNM$SYSTEM_TABLE", PSL$C_USER);

    allow_decnet_cluster = true_logical (ALLOW_CLUSTER_LOGNAME,
        "LNM$SYSTEM_TABLE", PSL$C_EXEC) ||
        true_logical (ALLOW_DECNET_CLUSTER_LOGNAME,
        "LNM$SYSTEM_TABLE", PSL$C_EXEC);
    allow_lat_cluster = true_logical (ALLOW_CLUSTER_LOGNAME,
        "LNM$SYSTEM_TABLE", PSL$C_EXEC) ||
        true_logical (ALLOW_LAT_CLUSTER_LOGNAME,
        "LNM$SYSTEM_TABLE", PSL$C_EXEC);
    allow_decnet_proxies = true_logical (ALLOW_DECNET_PROXIES_LOGNAME,
        "LNM$SYSTEM_TABLE", PSL$C_EXEC);
    allow_lat_proxies = true_logical (ALLOW_LAT_PROXIES_LOGNAME,
        "LNM$SYSTEM_TABLE", PSL$C_EXEC);

    output (trace_on, "FastLogin facility is active!/"
        "allow_decnet_cluster = !SL!/allow_lat_cluster    = !SL!/"
        "allow_decnet_proxies = !SL!/allow_lat_proxies    = !SL",
        allow_decnet_cluster, allow_lat_cluster,
        allow_decnet_proxies, allow_lat_proxies);

    /* Ready to service FastLogin. Connect our own identification routine. */

    LGI$LOGINOUT_CALLOUTS.lgi$icr_identify = callout_identify;
    return SS$_NORMAL;
}


/*
 *  Identification callout routine. This routine is connected only
 *  for interactive sessions on character-cell input (not DECwindows
 *  login, not batch, not network, etc...).
 */

static int callout_identify (struct lgi$arg_vector* arg, void** context)
{
    static const char command_prefix [] = "LOGIN ";
    static const $DESCRIPTOR (default_prompt,
        PROMPT_PREFIX PROMPT_USERNAME PROMPT_SUFFIX);

    int status;
    int trusted_user;
    char prompt [MAX_INPUT_LINE];
    char input [MAX_INPUT_LINE];
    char username [MAX_INPUT_LINE];
    char login [MAX_INPUT_LINE + sizeof (command_prefix)];
    struct dsc$descriptor_s login_d;

    output (trace_on, "Identifying user");

    /* Ensure that the system authentication routine is active by default */

    LGI$LOGINOUT_CALLOUTS.lgi$icr_authenticate = NULL;

    /* Always accept subprocesses */

    if (*arg->lgi$a_icr_subprocess)
        return SS$_NORMAL;

    /* If not interactive job, do normal processing */

    if ((*arg->lgi$a_icr_creprc_flags & PRC$M_INTER) == 0 ||
        (*arg->lgi$a_icr_creprc_flags & PRC$M_NOPASSWORD) != 0)
        return SS$_NORMAL;

    if ($VMS_STATUS_SUCCESS (arg->lgi$icb_autologin ()))
        return LGI$_SKIPRELATED;

    /* Look for a trusted user */

    trusted_user = find_trusted (arg, username, sizeof (username));

    /* If the originator is not a trusted user, perform normal username */
    /* prompting. */

    if (!trusted_user) {
        status = arg->lgi$icb_userprompt (&default_prompt);
        return $VMS_STATUS_SUCCESS (status) ? LGI$_SKIPRELATED : status;
    }

    /* We found a remote trusted user. Propose default login. */

    strcpy (prompt, PROMPT_PREFIX PROMPT_USERNAME " [");
    strcat (prompt, username);
    strcat (prompt, "]" PROMPT_SUFFIX);

    arg->lgi$a_icr_input_rab->rab$l_ubf = input;
    arg->lgi$a_icr_input_rab->rab$w_usz = sizeof (input) - 1;
    arg->lgi$a_icr_input_rab->rab$l_pbf = prompt;
    arg->lgi$a_icr_input_rab->rab$b_psz = strlen (prompt);
    arg->lgi$a_icr_input_rab->rab$l_rop |= RAB$M_PMT | RAB$M_CVT;
    arg->lgi$a_icr_input_rab->rab$l_rop &= ~RAB$M_RNE;

    arg->lgi$icb_get_input (arg->lgi$a_icr_input_rab, &1);

    input [arg->lgi$a_icr_input_rab->rab$w_rsz] = '\0';

    /* If the answer is an empty string or starts with the key character */
    /* ('#') the default login is accepted. Otherwise, the selected username */
    /* is used. In the later case, this is no longer a trusted login. */

    strcpy (login, command_prefix);

    if (arg->lgi$a_icr_input_rab->rab$w_rsz == 0 || input [0] == KEY_CHARACTER)
        strcat (login, username);
    else {
        trusted_user = 0;
        strcat (login, input);
    }

    login_d.dsc$a_pointer = login;
    login_d.dsc$w_length  = strlen (login);
    login_d.dsc$b_class   = DSC$K_CLASS_S;
    login_d.dsc$b_dtype   = DSC$K_DTYPE_T;

    if (!$VMS_STATUS_SUCCESS (status = arg->lgi$icb_userparse (&login_d)))
        return status;

    /* If a key is given, define the key logical name in supervisor mode */

    if (input [0] == KEY_CHARACTER) {

        struct item items [2];

        static const $DESCRIPTOR (name_d, KEY_LOGNAME);
        static const $DESCRIPTOR (table_d, "LNM$PROCESS_TABLE");

        items[0].code = LNM$_STRING;
        items[0].buffer = input + 1;
        items[0].length = strlen (input) - 1;
        items[0].retlen = NULL;

        items[1].code = items[1].length = 0;

        status = sys$crelnm (0, &table_d, &name_d, &PSL$C_SUPER, items);

        if (!$VMS_STATUS_SUCCESS (status)) {
            output (1, "Cannot define logical name " KEY_LOGNAME);
            message (1, 1, status);
        }
    }

    /* If trusted user, override the system authentication routine */

    if (trusted_user)
        LGI$LOGINOUT_CALLOUTS.lgi$icr_authenticate = callout_authenticate;

    return LGI$_SKIPRELATED;
}


/*
 *  Authentication callout routine.
 *  This routine is connected only if a trusted user was found.
 */

static int callout_authenticate (struct lgi$arg_vector* arg, void** context)
{
    output (trace_on, "User \"!AS\" is trusted", arg->lgi$a_icr_username);
    return LGI$_SKIPRELATED;
}


/*
 *  This routine looks for a trusted local user name.
 *  Fill the user supplied buffer with a nul-terminated string.
 *  Return 1 if a user is found, 0 otherwise.
 */

static int find_trusted (struct lgi$arg_vector* arg, char* name, size_t size)
{
    int status;
    static const $DESCRIPTOR (empty, "");

    output (trace_on, "Job type is !UB!/Terminal flag is !UB!/"
        "Terminal device is \"!AS\"!/Terminal port is \"!AS\"",
        *arg->lgi$a_icr_job_type, *arg->lgi$a_icr_terminal_dev,
        *arg->lgi$a_icr_terminal_dev ? arg->lgi$a_icr_tt_phydevnam : &empty,
        *arg->lgi$a_icr_terminal_dev ? arg->lgi$a_icr_tt_accpornam : &empty);

    /* If the connection comes from a DECnet SET HOST command */

    if (terminal_type (arg, "RT")) {

        output (trace_on, "Node is \"!AS\", user is \"!AS\"",
            arg->lgi$a_icr_rem_node_nam, arg->lgi$a_icr_rem_id);

        /* If DECnet proxy login enabled, check for a default user name */
        /* for this remote node and username. */

        if (allow_decnet_proxies) {
            status = find_proxy (arg->lgi$a_icr_rem_node_nam,
                arg->lgi$a_icr_rem_id, name, size);
            if ($VMS_STATUS_SUCCESS (status))
                return 1;
        }

        /* If intra-cluster login is enabled and the connection comes from */
        /* the same VMScluster. use same user */

        if (allow_decnet_cluster &&
            is_in_cluster (arg->lgi$a_icr_rem_node_nam)) {

            size_t length = arg->lgi$a_icr_rem_id->dsc$w_length;

            if (length >= size)
                return 0;
            else {
                strncpy (name, arg->lgi$a_icr_rem_id->dsc$a_pointer, length);
                name [length] = '\0';
                return 1;
            }
        }
    }

    /* If the connection comes from a SET HOST/LAT command */

    else if (terminal_type (arg, "LT")) {

        char* slash;
        int remain;
        int member;
        int group;
        struct dsc$descriptor_s node_d;

        /* A LAT connection from an OpenVMS node creates a port name */
        /* "node/UIC_ggggggmmmmmm" where 'g' and 'm' are the UIC group */
        /* and member using 6 octal digits. */

        /* First, check that the port name has the expected syntax */

        slash = arg->lgi$a_icr_tt_accpornam->dsc$a_pointer;
        remain = arg->lgi$a_icr_tt_accpornam->dsc$w_length;
        while (remain > 0 && *slash != '/') {
            remain--;
            slash++;
        }

        /* When the expected syntax is not found, return */

        if (remain != 17 || strncmp (slash, "/UIC_", 5) != 0)
            return 0;

        group = octal (slash + 5, 6);
        member = octal (slash + 11, 6);

        if (group < 0 || member < 0)
            return 0;

        /* At this point, we got the remote node name and UIC */

        node_d.dsc$a_pointer = arg->lgi$a_icr_tt_accpornam->dsc$a_pointer;
        node_d.dsc$w_length  = slash - node_d.dsc$a_pointer;
        node_d.dsc$b_class   = DSC$K_CLASS_S;
        node_d.dsc$b_dtype   = DSC$K_DTYPE_T;

        output (trace_on, "Node is \"!AS\", UIC group is !OW, member is !OW",
            &node_d, group, member);

        /* If LAT proxy login enabled, check for a default user name. */
        /* We search the DECnet proxy with a "node::[ggg,mmm]" where */
        /* "node" is the LAT node name. */

        if (allow_lat_proxies) {

            /* Build an ascii representation of the UIC */

            char uic_string [20];
            struct dsc$descriptor_s uic_d;
            static const $DESCRIPTOR (uic_format_d, "!%U");

            uic_d.dsc$a_pointer = uic_string;
            uic_d.dsc$w_length  = sizeof (uic_string);
            uic_d.dsc$b_class   = DSC$K_CLASS_S;
            uic_d.dsc$b_dtype   = DSC$K_DTYPE_T;

            status = sys$fao (&uic_format_d, &uic_d.dsc$w_length, &uic_d,
                (group << 16) | member);

            if (!$VMS_STATUS_SUCCESS (status)) {
                output (trace_on, "Error building UIC");
                message (trace_on, 0, status);
                return 0;
            }

            /* Search the proxy database */

            status = find_proxy (&node_d, &uic_d, name, size);

            if ($VMS_STATUS_SUCCESS (status))
                return 1;
        }

        /* If intra-cluster login is enabled and the connection comes from */
        /* the same VMScluster. use same user (from UIC) */

        if (allow_lat_cluster && is_in_cluster (&node_d)) {

            struct dsc$descriptor_s name_d;
            unsigned short length;

            name_d.dsc$a_pointer = name;
            name_d.dsc$w_length  = size - 1;
            name_d.dsc$b_class   = DSC$K_CLASS_S;
            name_d.dsc$b_dtype   = DSC$K_DTYPE_T;

            output (trace_on, "Convert UIC !%U to username",
                (group << 16) | member);

            status = sys$idtoasc ((group << 16) | member, &length, &name_d,
                0, 0, 0);

            message (trace_on, 0, status);

            if ($VMS_STATUS_SUCCESS (status)) {
                name [length] = '\0';
                return 1;
            }
        }
    }

    /* No local trusted user found */

    return 0;
}


/*
 *  This routine decodes N octal digits from the string.
 *  Return -1 in case of error.
 */

static int octal (const char* s, int max)
{
    int result = 0;

    while (max-- > 0)
        if (*s < '0' || *s > '7')
            return -1;
        else
            result = (result << 3) + (*s++ - '0');

    return result;
}


/*
 *  This routine translates a logical name.
 *  Return a VMS status code and a nul-terminated string.
 */

static int translate (
    const char*   name,
    const char*   table,
    unsigned char access_mode,
    char*         value,
    size_t        value_size)
{
    int status;
    struct dsc$descriptor_s name_d;
    struct dsc$descriptor_s table_d;
    struct item items [2];
    short length;

    name_d.dsc$a_pointer = (char*) name;
    name_d.dsc$w_length  = strlen (name);
    name_d.dsc$b_class   = DSC$K_CLASS_S;
    name_d.dsc$b_dtype   = DSC$K_DTYPE_T;

    table_d.dsc$a_pointer = (char*) table;
    table_d.dsc$w_length  = strlen (table);
    table_d.dsc$b_class   = DSC$K_CLASS_S;
    table_d.dsc$b_dtype   = DSC$K_DTYPE_T;

    items[0].code   = LNM$_STRING;
    items[0].buffer = value;
    items[0].length = value_size - 1;
    items[0].retlen = &length;

    items[1].code = items[1].length = 0;

    status = sys$trnlnm (0, &table_d, &name_d, &access_mode, items);

    if (!$VMS_STATUS_SUCCESS (status))
        length = 0;

    value [length] = '\0';

    return status;
}


/*
 *  This routine translates a logical name. Return 1 if the logical name
 *  is defined and its value is true. Return 0 otherwise.
 */

static int true_logical (
    const char* name,
    const char* table,
    unsigned char access_mode)
{
    int status;
    char value [80];

    status = translate (name, table, access_mode, value, sizeof (value));

    return $VMS_STATUS_SUCCESS (status) && strcmp (value, "TRUE") == 0;
}


/*
 *  This routine checks if the specified node name is inside the cluster
 *  or matches the cluster alias. Return a boolean.
 */

static int is_in_cluster (const struct dsc$descriptor_s* node)
{
    int status;
    char* start;
    char* end;
    char* p;
    struct iosb iosb;
    char alias [MAX_INPUT_LINE];

    static const struct item empty_list [] = {{0, 0, NULL, NULL}};

    /* Check if specified node is currently inside the cluster */

    output (trace_on, "Checking if \"!AS\" is inside the VMScluster", node);

    status = sys$getsyiw (0, 0, node, empty_list, &iosb, 0, 0);

    if ($VMS_STATUS_SUCCESS (status))
        status = iosb.status;

    message (trace_on, 0, status);

    /* If inside the cluster, return OK */

    if ($VMS_STATUS_SUCCESS (status))
        return 1;

    /* Get cluster alias, try custom logical name */

    output (trace_on, "Fetching cluster alias, trying logical name \"!AZ\"",
        ALIAS_LOGNAME_1);
    status = translate (ALIAS_LOGNAME_1, "LNM$SYSTEM_TABLE", PSL$C_EXEC,
        alias, sizeof (alias));
    message (trace_on, 0, status);

    /* If not found, try standard logical name */

    if (!$VMS_STATUS_SUCCESS (status)) {
        output (trace_on, "Fetching cluster alias, trying logical name \"!AZ\"",
            ALIAS_LOGNAME_2);
        status = translate (ALIAS_LOGNAME_2, "LNM$SYSTEM_TABLE", PSL$C_EXEC,
            alias, sizeof (alias));
        message (trace_on, 0, status);
    }

    /* If still not found, return error */

    if (!$VMS_STATUS_SUCCESS (status))
        return 0;

    /* In cluster alias, remove leading underscores, if any */

    for (start = alias; *start == '_'; start++);

    /* Remove trailing "::", if any */

    end = start + strlen (start);
    while (end > start && end [-1] == ':')
        end--;
    *end = '\0';

    /* Keep only local part if full name supplied. Normally the */
    /* full name of the cluster alias is in a separate logical name */
    /* called SYS$CLUSTER_NODE_FULLNAME, but this may change. */

    for (p = end - 1; p >= start && *p != '.' && *p != ':'; p--);
    start = p + 1;

    output (trace_on, "Cluster alias is \"!AZ\"", start);

    return node->dsc$w_length == (end - start) &&
        memcmp (node->dsc$a_pointer, start, end - start) == 0;
}


/*
 *  This routine checks if the remote user has a default proxy.
 *  Return a VMS status code.
 */

static int find_proxy (
    const struct dsc$descriptor_s* remote_node,
    const struct dsc$descriptor_s* remote_user,
    char*  name,
    size_t size)
{
    int status;
    unsigned short length;
    struct dsc$descriptor_s name_d;
    union prvdef privs;

    /* The proxy database lookup services were introduced in */
    /* OpenVMS VAX V6.1 and OpenVMS Alpha V6.2. Note that */
    /* FastLogin requires OpenVMS V6.1 anyway. */

#if defined __ALPHA && __VMS_VER < 60200000

    output (trace_on, "Proxy database lookup not implemented");
    return SS$_UNSUPPORTED;

#else

    /* Enable SYSPRV privilege (required by sys$verify_proxy). We don't */
    /* care about resetting it since we don't make it permanent. */

    memset (&privs, 0, sizeof (privs));
    privs.prv$v_sysprv = 1;

    status = sys$setprv (1, &privs, 0, 0);

    if (!$VMS_STATUS_SUCCESS (status) || status == SS$_NOTALLPRIV) {
        output (trace_on, "Cannot set SYSPRV:");
        message (trace_on, 0, status);
    }

    /* Look for a proxy */

    name_d.dsc$a_pointer = name;
    name_d.dsc$w_length  = size - 1;
    name_d.dsc$b_class   = DSC$K_CLASS_S;
    name_d.dsc$b_dtype   = DSC$K_DTYPE_T;

    output (trace_on, "Looking for a proxy for \"!AS::!AS\"",
        remote_node, remote_user);

    status = sys$verify_proxy (remote_node, remote_user, 0, &name_d,
        &length, 0);

    name [$VMS_STATUS_SUCCESS (status) ? length : 0] = '\0';
    message (trace_on, 0, status);

    return status;

#endif
}


/*
 *  Check if the physical terminal name of the session has the specified
 *  device type (ie "RT", "LT", etc...).
 */

static int terminal_type (struct lgi$arg_vector* arg, const char* type)
{
    const char* dev;
    size_t len;

    /* If not a terminal, answer is no */

    if (!*arg->lgi$a_icr_terminal_dev || arg->lgi$a_icr_tt_phydevnam == NULL)
        return 0;

    /* Skip leading underscores */

    dev = arg->lgi$a_icr_tt_phydevnam->dsc$a_pointer;
    len = arg->lgi$a_icr_tt_phydevnam->dsc$w_length;

    while (len > 0 && *dev == '_') {
        len--;
        dev++;
    }

    return len < strlen (type) ? 0 : strncmp (type, dev, strlen (type)) == 0;
}


/*
 *  Output routine. The first parameter is an FAO format.
 *  Other parameters are the FAO arguments.
 */

static void output (int really_do_it, const char* fao_format, ...)
{
    static int first_time = 1;
    static int use_opcom = 0;
    static struct FAB outfab;
    static struct RAB outrab;

    va_list ap;
    int arg_count;
    int status;
    struct opcom opcom;
    struct dsc$descriptor_s fao_d;
    struct dsc$descriptor_s line_d;
    long fao_params [MAX_ARGS];
    long* param;
    unsigned short length;

    if (!really_do_it)
        return;

    /* The following sequence is executed only once. */

    if (first_time) {

        char outdev_buffer [MAX_INPUT_LINE];
        char* outdev;

        first_time = 0;

        /* Find the output device. SYS$OUTPUT: is the default */

        status = translate (OUTPUT_DEVICE_LOGNAME, "LNM$SYSTEM_TABLE",
            PSL$C_USER, outdev_buffer, sizeof (outdev_buffer));

        outdev = $VMS_STATUS_SUCCESS (status) ? outdev_buffer : "SYS$OUTPUT:";
        use_opcom = strcmp (outdev, OUTPUT_DEVICE_OPCOM) == 0;

        /* If a regular device is used, open it */

        if (!use_opcom) {

            outfab = cc$rms_fab;
            outfab.fab$b_fac = FAB$M_PUT;
            outfab.fab$b_rfm = FAB$C_VAR;
            outfab.fab$b_rat = FAB$M_CR;
            outfab.fab$l_fna = outdev;
            outfab.fab$b_fns = strlen (outdev);
            outfab.fab$l_dna = OUTPUT_DEVICE_DEFSPEC;
            outfab.fab$b_dns = strlen (outfab.fab$l_dna);

            outrab = cc$rms_rab;
            outrab.rab$l_fab = &outfab;

            if (!$VMS_STATUS_SUCCESS (status = sys$create (&outfab))) {
                message (1, 1, status, outfab.fab$l_stv);
                return;
            }

            if (!$VMS_STATUS_SUCCESS (status = sys$connect (&outrab))) {
                message (1, 1, status, outrab.rab$l_stv);
                sys$close (&outfab);
                return;
            }
        }
    }

    /* Format the output line using FAO */

    va_count (arg_count);
    arg_count -= 2;
    if (arg_count > MAX_ARGS)
        arg_count = MAX_ARGS;

    va_start (ap, fao_format);
    for (param = fao_params; arg_count-- > 0; *param++ = va_arg (ap, long));
    va_end (ap);

    fao_d.dsc$a_pointer = (char*) fao_format;
    fao_d.dsc$w_length  = strlen (fao_format);
    fao_d.dsc$b_class   = DSC$K_CLASS_S;
    fao_d.dsc$b_dtype   = DSC$K_DTYPE_T;

    line_d.dsc$a_pointer = opcom.line;
    line_d.dsc$w_length  = sizeof (opcom.line);
    line_d.dsc$b_class   = DSC$K_CLASS_S;
    line_d.dsc$b_dtype   = DSC$K_DTYPE_T;

    status = sys$faol (&fao_d, &length, &line_d, fao_params);

    if (!$VMS_STATUS_SUCCESS (status)) {
        message (1, 1, status);
        return;
    }

    /* Display the message */

    if (use_opcom) {
        opcom.type = OPC$_RQ_RQST;
        opcom.target = OPC$M_NM_SECURITY;
        opcom.rqstid = 0;

        line_d.dsc$a_pointer = (char*) &opcom;
        line_d.dsc$w_length  = 8 + length;

        status = sys$sndopr (&line_d, 0);
    }
    else {
        outrab.rab$l_rbf = opcom.line;
        outrab.rab$w_rsz = length;

        status = sys$put (&outrab);
    }

    if (!$VMS_STATUS_SUCCESS (status))
        message (1, 1, status, outrab.rab$l_stv);
}


/*
 *  Message routine.
 */

static void message (int really_do_it, int use_term, int message_status, ...)
{
    va_list ap;
    int n;
    int vector [MAX_ARGS + 1];

    if (!really_do_it)
        return;

    va_count (vector [0]);
    vector [0] -= 2;
    if (vector [0] > MAX_ARGS)
        vector [0] = MAX_ARGS;

    vector [1] = message_status;
    va_start (ap, message_status);
    for (n = 2; n <= vector [0]; vector [n++] = va_arg (ap, int));
    va_end (ap);

    sys$putmsg (vector, use_term ? 0 : message_output, 0, 0);
}

static int message_output (const struct dsc$descriptor_s* line, void* param)
{
    output (1, "!AS", line);
    return 0;
}
