/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#ifndef RESULTTYPES_H
#define RESULTTYPES_H


/*------------------------------------------------------------------------------
//                     Le pointeur null
//------------------------------------------------------------------------------
*/
#define Null 0


/*------------------------------------------------------------------------------
//                     Le type des differents resultats 
//------------------------------------------------------------------------------
*/
typedef int LineNumber;


typedef char* StringTable[];


struct Position {
  char*      file_name;
  LineNumber line_number;
};

typedef struct Position Position;


/*------------------------------------------------------------------------------
//                     Le type des differents resultats 
//------------------------------------------------------------------------------
*/

#define FILE_PREFIX        ""

#define FILE_PLENGTH       0


#define PROC_PREFIX        "[   ]  "

#define PROC_PLENGTH       7


#define PARENT_PREFIX      "[ ]  "

#define PARENT_PLENGTH     5


#define METHOD_PREFIX      "[    ]  "

#define METHOD_PLENGTH     8


#define CLASS_PREFIX       "[ ]  "

#define CLASS_PLENGTH      5


/*------------------------------------------------------------------------------
//   Les differentes macros applicable aux resultats retournes par le browser 
//------------------------------------------------------------------------------
*/
#define GET_FILE_INFO(pt)       (pt - FILE_PLENGTH)
#define REMOVE_FILE_INFO(pt) (pt+FILE_PLENGTH)

#define GET_PROCEDURE_INFO(pt)  (pt - PROC_PLENGTH)
#define REMOVE_PROCEDURE_INFO(pt) (pt+PROC_PLENGTH)

#define GET_PARENT_INFO(pt)     (pt - PARENT_PLENGTH)
#define REMOVE_PARENT_INFO(pt) (pt+PARENT_PLENGTH)

#define GET_SON_INFO(pt)        (pt - CLASS_PLENGTH)
#define REMOVE_SON_INFO(pt) (pt+SON_PLENGTH)

#define GET_METHOD_INFO(pt)     (pt - METHOD_PLENGTH)
#define REMOVE_METHOD_INFO(pt) (pt+METHOD_PLENGTH)

#define GET_CLASS_INFO(pt)      (pt - CLASS_PLENGTH)
#define REMOVE_CLASS_INFO(pt) (pt+CLASS_PLENGTH)

#define GET_METHOD_CLASS(pt)    (get_method_class(pt))

extern char* get_method_class();


#endif  /*  RESULTTYPES_H  */
