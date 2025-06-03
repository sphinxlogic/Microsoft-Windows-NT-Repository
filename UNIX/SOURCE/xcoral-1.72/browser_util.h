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

#ifndef BROWSERUTIL_H
#define BROWSERUTIL_H


/*------------------------------------------------------------------------------
//       La macro utilisee pour l'acces au table de "hash code"
//------------------------------------------------------------------------------
*/
#define get_head_Rec(name, dict, dict_size, head)  {                   \
  char*     current_char;                                              \
  unsigned  g, h = 0;                                                  \
                                                                       \
  for (current_char = name; *current_char != '\0'; current_char++) {   \
    h = (h << 4) + (*current_char);                                    \
    if (g = h & 0xf0000000) {                                          \
      h = h ^ (g >> 24);                                               \
      h = h ^ g;                                                       \
    }                                                                  \
  }                                                                    \
  head = & (dict[h % dict_size]);                                      \
}


/*------------------------------------------------------------------------------
//       Les macros utilisees pour manipuler les list de "TypeRec"
//------------------------------------------------------------------------------
*/
#define create_Rec(name, TypeRec, head, current, PREFIX, PLENGTH)  {   \
   char* cpy_name;                                                     \
                                                                       \
   cpy_name       = ((char*) current) + sizeof(TypeRec) ;              \
   strcpy(cpy_name, PREFIX);                                           \
   cpy_name      += PLENGTH;                                           \
   current->_name = cpy_name;                                          \
   strcpy(cpy_name, name);                                             \
   current->_next = *head;                                             \
   *head          = current;                                           \
}


#define search_Rec(name, TypeRec, head, current)  {                    \
  current = *head;                                                     \
  while (current != Null) {                                            \
    if (strcmp(current->_name, name) == 0)                             \
      break;                                                           \
    else                                                               \
      current = current->_next;                                        \
  }                                                                    \
}


#define extract_Rec(name, TypeRec, head, current)  {                   \
  TypeRec* previous;                                                   \
                                                                       \
  previous = Null;                                                     \
  current  = *head;                                                    \
  while (current != Null) {                                            \
    if (strcmp(current->_name, name) == 0) {                           \
      if (previous == Null)                                            \
	    *head = current->_next;                                        \
      else                                                             \
	    previous->_next = current->_next;                              \
      break;                                                           \
    }                                                                  \
    previous = current;                                                \
    current  = current->_next;                                         \
  }                                                                    \
}


/*------------------------------------------------------------------------------
//                    Quelques types d'usage divers
//------------------------------------------------------------------------------
*/
#define False   ((0))
#define True    ((!False))


/*------------------------------------------------------------------------------
//                    Quelques procedures d'usage divers
//------------------------------------------------------------------------------
*/
extern char* xmalloc     (/* int size  */);

extern int sort_util     (/* char** i,
                             char** j  */);

#endif    /*  BROWSERUTIL_H  */

