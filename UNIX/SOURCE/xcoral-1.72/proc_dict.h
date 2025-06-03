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

#ifndef PROCDICT_H
#define PROCDICT_H

/*------------------------------------------------------------------------------
*/
enum ProcType {
    UNKNOWN_PROC,
	C_PROC,
	CPLUS_PROC
};
typedef enum ProcType ProcType;


struct ProcRec {
  char*           _name;
  FileRec*        _impl_file;
  LineNumber      _impl_line;
  ProcType        _proc_type;
  struct ProcRec* _next;
};
typedef struct ProcRec ProcRec;


/*------------------------------------------------------------------------------
//                         Le dictionnaire des procedures
//------------------------------------------------------------------------------
*/

#define PROC_DICT_SIZE     503

extern  ProcRec* proc_dict[];

extern  int proc_count;


/*------------------------------------------------------------------------------
//       Les procedures utilisees manipuler le dictionnaire des procedures
//------------------------------------------------------------------------------
*/

extern ProcRec*     create_proc   (/* char* proc_name */);

extern ProcRec*     find_proc     (/* char* proc_name */);


/*------------------------------------------------------------------------------
//       La procedure d'initialisation du dictionnaire des procedures
//------------------------------------------------------------------------------
*/

extern void         init_proc     ();


#endif  /*  PROCDICT_H  */




