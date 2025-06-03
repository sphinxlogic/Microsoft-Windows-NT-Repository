/********************************************************************
 * lindner
 * 3.6
 * 1994/03/30 21:37:12
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/STRstring.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: STRstring.h
 * A dynamic string in C that really wants to be C++
 *********************************************************************
 * Revision History:
 * STRstring.h,v
 * Revision 3.6  1994/03/30  21:37:12  lindner
 * Fix for URL code
 *
 * Revision 3.5  1994/03/04  23:26:55  lindner
 * explicitly cast to char
 *
 * Revision 3.4  1994/02/20  16:23:27  lindner
 * Optimize STRinit so that memory isn't freed then reallocated
 *
 * Revision 3.3  1993/10/27  18:53:44  lindner
 * Add STRsize()
 *
 * Revision 3.2  1993/06/22  05:48:03  lindner
 * Mods for VMS
 *
 * Revision 3.1.1.1  1993/02/11  18:03:04  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#ifndef STRstring_H
#define STRstring_H


struct string_struct {
     int  len;
     char *data;
};

typedef struct string_struct String;


/** VMS doesn't like multiple case blech! **/
#ifdef VMS
# define STRcpy GSTRcpy
# define STRcmp GSTRcmp
# define STRcat GSTRcat
#endif


String *STRnew();
String *STRnewSet(/* char* */);
String *STRcpy(/* String, String */);
void    STRinit(/* String*  */);
void    STRset(/* String*, char* */);
void    STRdestroy(/* String* */);
#define STRget(s)  ((char*)((s)->len<0) ? NULL : (s)->data)
#define STRlen(s)  (((s)->len > 0) ? (s)->len : 0)
#define STRsize(s) ((s)->len)
String* STRcat();
int     STRcmp();



#endif  

