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

#ifndef FILEDICT_H
#define FILEDICT_H

/*------------------------------------------------------------------------------
*/
struct FileRec {
  char*           _name;
  struct FileRec* _next;
};
typedef struct FileRec FileRec;



/*------------------------------------------------------------------------------
//                         Le dictionnaire des fichiers
//------------------------------------------------------------------------------
*/

#define FILE_DICT_SIZE    101

extern  FileRec* file_dict[];

extern  int      file_count;


/*------------------------------------------------------------------------------
//       Les procedures utilisees manipuler le dictionnaire des fichiers
//------------------------------------------------------------------------------
*/

extern FileRec*     create_file   (/* char* file_name */);

extern FileRec*     find_file     (/* char* file_name */);

extern void         remove_file   (/* char* file_name */);


/*------------------------------------------------------------------------------
//       La procedure d'initialisation du dictionnaire des fichiers
//------------------------------------------------------------------------------
*/

extern void         init_file     ();


#endif  /*  FILEDICT_H  */



