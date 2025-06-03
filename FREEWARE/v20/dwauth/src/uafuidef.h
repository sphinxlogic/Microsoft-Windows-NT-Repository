/********************************************************************************************************************************/
/* Created  1-NOV-1993 13:00:46 by VAX SDL T3.2-8      Source: 16-JUN-1993 11:29:24 UAFUIDEF.SDL;52 */
/********************************************************************************************************************************/
 
/*** MODULE UAFUI ***/
/* Define the constants associated with the file menu items.                */
#define uaf$c_ctl_menu_base 1
#define uaf$c_ctl_open 1                /* Open a new file                  */
#define uaf$c_ctl_refresh 2             /* Refresh current (open) file      */
#define uaf$c_ctl_quit 3                /* Exit from the utility            */
#define uaf$c_ctl_max_code 4            /* Last entry + 1                   */
/* Define the constants associated with the command menu items.             */
#define uaf$c_cmd_menu_base 10
#define uaf$c_cmd_list 10               /* List entries to a file           */
#define uaf$c_cmd_print 11              /* Print entries                    */
#define uaf$c_cmd_modify_template 12    /* Modify new account template      */
#define uaf$c_cmd_max_code 13           /* Last entry + 1                   */
/* Define the constants associated with the help menu items.                */
#define uaf$c_hlp_menu_base 20
#define uaf$c_hlp_overview 20           /* Provide an overview of the utility */
#define uaf$c_hlp_about 21              /* Provide feature level help       */
#define uaf$c_hlp_max_code 22           /* Last entry + 1                   */
/* Define the constants associated with the file selection items.           */
#define uaf$c_file_window_base 25
#define uaf$c_file_open_file 25         /* Open an authorization file       */
#define uaf$c_file_select 26            /* Select new list of authorization files */
#define uaf$c_file_cancel 27            /* Cancel file selection            */
#define uaf$c_file_max_code 28          /* Last entry + 1                   */
/* Define the constants associated with the main window widgets.            */
#define uaf$c_main_window_base 30
#define uaf$c_main_create 30            /* Pushbutton - Create a new authorization entry */
#define uaf$c_main_modify 31            /* Pushbutton - Modify an existing authorization entry */
#define uaf$c_main_delete 32            /* Pushbutton - Delete an existing authorization entry */
#define uaf$c_main_reset 33             /* Pushbutton - Reset to initial state */
#define uaf$c_main_select 34            /* Pushbutton - Select entries using filter value */
#define uaf$c_main_spare_1 35           /* Spare                            */
#define uaf$c_main_spare_2 36           /* Spare                            */
#define uaf$c_main_spare_3 37           /* Spare                            */
#define uaf$c_main_spare_4 38           /* Spare                            */
#define uaf$c_main_spare_5 39           /* Spare                            */
#define uaf$c_main_filter 40            /* Simpletext - Entry filter value  */
#define uaf$c_main_username 41          /* Simpletext - Current username value */
#define uaf$c_main_spare_6 42           /* Spare                            */
#define uaf$c_main_spare_7 43           /* Spare                            */
#define uaf$c_main_spare_8 44           /* Spare                            */
#define uaf$c_main_spare_9 45           /* Spare                            */
#define uaf$c_main_spare_10 46          /* Spare                            */
#define uaf$c_main_user_list 47         /* List - Available authorization entries using filter */
#define uaf$c_main_group_list 48        /* List - Available group authorization entries */
#define uaf$c_main_identifiers 49       /* Pushbutton - Alter account identifiers */
#define uaf$c_main_max_code 50          /* Last entry + 1                   */
/* Define the constants associated with the authorization entry window widgets. */
#define uaf$c_entry_window_base 60
#define uaf$c_entry_ok_previous 60      /* Pushbutton - Select previous entry and change current */
#define uaf$c_entry_ok 61               /* Pushbutton - Accept authorization entry changes */
#define uaf$c_entry_ok_next 62          /* Pushbutton - Select next entry and change current */
#define uaf$c_entry_cancel 63           /* Pushbutton - Abort authorization changes */
#define uaf$c_entry_rename 64           /* Pushbutton - Take new username and rename original entry */
#define uaf$c_entry_copy 65             /* Pushbutton - Take new username and copy original entry */
#define uaf$c_entry_select_previous 66  /* Pushbutton - Select previous entry and don't change current */
#define uaf$c_entry_select 67           /* Pushbutton - Select next entry and don't change current */
#define uaf$c_entry_select_next 68      /* Pushbutton - Select next entry and don't change current */
#define uaf$c_entry_apply_template 69   /* Pushbutton - Apply template changes to current */
#define uaf$c_entry_spare_1 70          /* Spare                            */
#define uaf$c_entry_spare_2 71          /* Spare                            */
#define uaf$c_entry_spare_3 72          /* Spare                            */
#define uaf$c_entry_spare_4 73          /* Spare                            */
#define uaf$c_entry_spare_5 74          /* Spare                            */
#define uaf$c_entry_username 75         /* Simpletext - username            */
#define uaf$c_entry_owner 76            /* Simpletext - account owner       */
#define uaf$c_entry_account 77          /* Simpletext - account name        */
#define uaf$c_entry_uic 78              /* Simpletext - UIC                 */
#define uaf$c_entry_cli 79              /* Simpletext - CLI name            */
#define uaf$c_entry_clitable 80         /* Simpletext - CLITABLE name       */
#define uaf$c_entry_default 81          /* Simpletext - default login device and directory */
#define uaf$c_entry_lgicmd 82           /* Simpletext - login command file  */
#define uaf$c_entry_maxjobs 83          /* Simpletext - maximum number of jobs */
#define uaf$c_entry_maxacctjobs 84      /* Simpletext - maximum jobs for account */
#define uaf$c_entry_maxdetach 85        /* Simpletext - maximum detached jobs */
#define uaf$c_entry_prclm 86            /* Simpletext - process/subprocess limit */
#define uaf$c_entry_prio 87             /* Simpletext - process priority    */
#define uaf$c_entry_queprio 88          /* Simpletext - queue priority      */
#define uaf$c_entry_cpu 89              /* Simpletext - CPU time limit      */
#define uaf$c_entry_bytlm 90            /* Simpletext - byte count limit    */
#define uaf$c_entry_fillm 91            /* Simpletext - open file limit     */
#define uaf$c_entry_shrfillm 92         /* Simpletext - shared file limit   */
#define uaf$c_entry_biolm 93            /* Simpletext - buffered I/O limit  */
#define uaf$c_entry_diolm 94            /* Simpletext - direct I/O limit    */
#define uaf$c_entry_wsdef 95            /* Simpletext - working set default */
#define uaf$c_entry_wsquo 96            /* Simpletext - working set quota   */
#define uaf$c_entry_wsextent 97         /* Simpletext - working set extent  */
#define uaf$c_entry_pgflquo 98          /* Simpletext - page file quota     */
#define uaf$c_entry_astlm 99            /* Simpletext - AST limit           */
#define uaf$c_entry_tqelm 100           /* Simpletext - TQE limit           */
#define uaf$c_entry_enqlm 101           /* Simpletext - ENQ limit           */
#define uaf$c_entry_jtquota 102         /* Simpletext - job table quota     */
#define uaf$c_entry_expiration 103      /* Simpletext - account expiration  */
#define uaf$c_entry_lastlogin_i 104     /* Simpletext - last interactive login */
#define uaf$c_entry_lastlogin_n 105     /* Simpletext - last non-interactive login */
#define uaf$c_entry_pwd_date 106        /* Simpletext - primary password change date */
#define uaf$c_entry_pwd_length 107      /* Simpletext - password minimum length */
#define uaf$c_entry_pwd_lifetime 108    /* Simpletext - password lifetime   */
#define uaf$c_entry_pwd2_date 109       /* Simpletext - secondary password change date */
#define uaf$c_entry_pbytlm 110          /* Simpletext - physical byte limit */
#define uaf$c_entry_logfails 111        /* Simpletext - login failure count */
#define uaf$c_entry_login_flag_list 112 /* List - login flags               */
#define uaf$c_entry_auth_priv_list 113  /* List - authorized privileges     */
#define uaf$c_entry_def_priv_list 114   /* List - default privileges        */
#define uaf$c_entry_primeday_list 115   /* List - primary days              */
#define uaf$c_entry_password 116        /* Simpletext - primary password    */
#define uaf$c_entry_password2 117       /* Simpletext - Secondary password  */
#define uaf$c_entry_access_primary 118  /* Pushbutton - Account primary access restrictions */
#define uaf$c_entry_access_secondary 119 /* Pushbutton - Account secondary access restrictions */
#define uaf$c_entry_spare_6 120         /* Spare                            */
#define uaf$c_entry_spare_7 121         /* Spare                            */
#define uaf$c_entry_spare_8 122         /* Spare                            */
#define uaf$c_entry_spare_9 123         /* Spare                            */
#define uaf$c_entry_spare_10 124        /* Spare                            */
#define uaf$c_entry_max_code 125        /* Last entry + 1                   */
/* Define the constants associated with the authorization entry access      */
/* restriction window widgets.                                              */
#define uaf$c_entry_access_base 550
#define uaf$c_access_pri_ok 550         /* Pushbutton - Accept primary access restriction changes */
#define uaf$c_access_pri_cancel 551     /* Pushbutton - Abort primary access restriction changes */
#define uaf$c_access_spare_1 552        /* Spare                            */
#define uaf$c_access_spare_2 553        /* Spare                            */
#define uaf$c_access_spare_3 554        /* Spare                            */
#define uaf$c_access_pri_local_list 555 /* List - primary local access restrictions */
#define uaf$c_access_pri_batch_list 556 /* List - primary batch access restrictions */
#define uaf$c_access_pri_dialup_list 557 /* List - primary dialup access restrictions */
#define uaf$c_access_pri_remote_list 558 /* List - primary remote access restrictions */
#define uaf$c_access_pri_network_list 559 /* List - primary network access restrictions */
#define uaf$c_access_spare_4 560        /* Spare                            */
#define uaf$c_access_spare_5 561        /* Spare                            */
#define uaf$c_access_sec_ok 562         /* Pushbutton - Accept secondary access restriction changes */
#define uaf$c_access_sec_cancel 563     /* Pushbutton - Abort secondary access restriction changes */
#define uaf$c_access_spare_6 564        /* Spare                            */
#define uaf$c_access_spare_7 565        /* Spare                            */
#define uaf$c_access_spare_8 566        /* Spare                            */
#define uaf$c_access_sec_local_list 567 /* List - secondary local access restrictions */
#define uaf$c_access_sec_batch_list 568 /* List - secondary batch access restrictions */
#define uaf$c_access_sec_dialup_list 569 /* List - secondary dialup access restrictions */
#define uaf$c_access_sec_remote_list 570 /* List - secondary remote access restrictions */
#define uaf$c_access_sec_network_list 571 /* List - secondary network access restrictions */
#define uaf$c_access_spare_9 572        /* Spare                            */
#define uaf$c_access_spare_10 573       /* Spare                            */
#define uaf$c_access_max_code 574       /* Last entry + 1                   */
/* Define the constants associated with the authorization entry identifier  */
/* window widgets.                                                          */
#define uaf$c_identifier_base 650
#define uaf$c_identifier_ok 650         /* Pushbutton - Accept identifier changes */
#define uaf$c_identifier_cancel 651     /* Pushbutton - Abort identifier changes */
#define uaf$c_identifier_grant 652      /* Pushbutton - Grant identifier    */
#define uaf$c_identifier_modify 653     /* Pushbutton - Modify identifier attributes */
#define uaf$c_identifier_remove 654     /* Pushbutton - Remove identifier   */
#define uaf$c_identifier_system 655     /* Pushbutton - System identifiers  */
#define uaf$c_identifier_spare_2 656    /* Spare                            */
#define uaf$c_identifier_spare_3 657    /* Spare                            */
#define uaf$c_identifier_spare_4 658    /* Spare                            */
#define uaf$c_identifier_spare_5 659    /* Spare                            */
#define uaf$c_identifier_name 660       /* Simpletext - Identifier name     */
#define uaf$c_identifier_list 661       /* List - entry identifiers held    */
#define uaf$c_identifier_attrib_list 662 /* List - identifier attributes    */
#define uaf$c_identifier_spare_6 663    /* Spare                            */
#define uaf$c_identifier_spare_7 664    /* Spare                            */
#define uaf$c_identifier_spare_8 665    /* Spare                            */
#define uaf$c_identifier_spare_9 666    /* Spare                            */
#define uaf$c_identifier_spare_10 667   /* Spare                            */
#define uaf$c_system_identifier_cancel 668 /* Pushbutton - remove system identifier list */
#define uaf$c_system_identifier_spare_1 669 /* Spare                        */
#define uaf$c_system_identifier_spare_2 670 /* Spare                        */
#define uaf$c_system_identifier_spare_3 671 /* Spare                        */
#define uaf$c_system_identifier_list 672 /* List - all identifiers defined  */
#define uaf$c_system_identifier_spare_4 673 /* Spare                        */
#define uaf$c_system_identifier_spare_5 674 /* Spare                        */
#define uaf$c_identifier_max_code 675   /* Last entry + 1                   */
#define uaf$c_max_widget_code 675
