/*
*****************************************************************************
** xbmbrowser version 2.0  (c) Copyright Ashley Roll, 1992.
** FILE: user-menu.h
**
** xbmbrowser is Public Domain. However it, and all the code still belong to me.
** I do, however grant permission for you to freely copy and distribute it on 
** the condition that this and all other copyright notices remain unchanged in 
** all distributions.
**
** This software comes with NO warranty whatsoever. I therefore take no
** responsibility for any damages, losses or problems that the program may 
** cause.
*****************************************************************************
*/

#ifdef USER_MENU_C_FILE
#define extr
#else
#define extr extern
#endif

/*
 * global variables
 */
extr char *directory; /* pointer to a string containing the current directory */
extr char *bname; /* pointer to a string containing the current bitmap name */


/*
 * Expand the tilder in the string passed to it.
 * void expand_tilder(text)
 * char *text;
 */
extr void expand_tilder();

/*
 * Create a new menu - and put a line under the lable
 * Widget MakeMenu(parent,name,label)
 * Widget parent;
 * char *name,*label;
 */
extr Widget MakeMenu();

/* 
 * add an smeBSBObjectClass object to a menu (widget)
 * callback is the procedure that it calls or NULL
 * Widget AddMenuItem(menu,label,callback,cbdata,sensitive)
 * Widget menu;
 * char *label;
 * XtCallbackProc callback;
 * XtPointer cbdata;
 * int sensitive;
 */
extr Widget AddMenuItem();

/* 
 * create the menu
 * Widget Create_user_menu(parent,name,file)
 * Widget parent;
 * char *name,*file;
 *
 */ 
extr Widget Create_user_menu();

