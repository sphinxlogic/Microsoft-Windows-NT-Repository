/*
 *  Copyright (c) 1992, 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#ifndef _JED_KEYMAP_H_
#define _JED_KEYMAP_H_  
/* Jed Keymap routines.
    
   The main problem here is how to associate a sequence of characters to a
   given function in as fast a way as possible.  In addition, I require that
   the procedure be memory efficient as well as allowing the user the
   flexability to redefine the keys and add new keymaps.

   To do this, I define a key map to be an array of keys.  There may be many
   keymaps (global, local, etc...).    A key structure is defined */
   

#ifdef HAS_MOUSE
typedef struct
{
   int x, y;			       /* row column of mouse event */
   int time;			       /* time since last event */
   int button;			       /* button that triggered event */
   int state;			       /* button info BEFORE event */
   int type;			       /* type of event.  1 is press 0 release */
} JMouse_Type;

extern JMouse_Type JMouse;
#endif


extern SLKeyMap_List_Type *Global_Map, *Mini_Map;

extern void init_keymaps(void);
extern int digit_arg(void);
extern void do_jed(void);
extern void jed(void);
extern int do_key(void);
extern int kbd_quit(void);
extern VOID *Last_Key_Function;
extern int *Repeat_Factor;
extern char *find_key(int *);
extern char *what_keymap(void);
extern int which_key(char *);
extern void use_keymap(char *);
extern void set_abort_char(int *);
extern int next_function_list(char *);
extern int open_function_list(char *);

extern char *Read_This_Character;      /* alternate keyboard buffer */
extern void add_to_completion(char *);
extern int is_internal(char *);
extern void dump_bindings (char *);

extern void create_keymap (char *);
extern char Key_Buffer[13];
extern char Jed_Key_Buffer[13];
extern char *Key_Bufferp;
extern int beep (void);
#endif

