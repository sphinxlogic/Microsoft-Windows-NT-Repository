/*
 *  Copyright (c) 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
extern int (*X_Read_Hook) (void);	 
extern int (*X_Input_Pending_Hook) (void);
extern void (*X_Get_Term_Size_Hook) (int *, int *);
extern void (*X_Suspend_Hook)(void);
extern int (*X_Argc_Argv_Hook)(int, char **);
extern int (*X_Init_SLang_Hook)(void);
extern int (*X_Init_Term_Hook) (void);
extern void (*X_Reset_Term_Hook) (void);
extern void (*X_Update_Open_Hook)(void);      /* hooks called when starting */
extern void (*X_Update_Close_Hook)(void);     /* and finishing update */
#ifndef VMS
extern void (*X_Set_Color_Hook)(int, char *, char *, char *);
#endif
#ifndef pc_system 
#ifndef VMS
extern void (*X_Set_Color_Esc_Hook)(int, char *);
#endif
#endif
extern void (*X_Define_Keys_Hook) (SLKeyMap_List_Type *);



