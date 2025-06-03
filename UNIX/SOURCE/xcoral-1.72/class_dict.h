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

#ifndef CLASSDICT_H
#define CLASSDICT_H


/*------------------------------------------------------------------------------
*/
enum ScopeType {
	UNKNOWN_SCOPE,
	PUBLIC_SCOPE,
	PROTECTED_SCOPE,
	PRIVATE_SCOPE
};
typedef enum ScopeType ScopeType;


/*------------------------------------------------------------------------------
*/
struct ParentRec {
  char*             _name;             /*  Le nom de la classe parent   */
  ScopeType         _scope;            /*  La nature de l'heritage      */
  struct ParentRec* _next;             /*  La classe suivante           */
};

typedef struct ParentRec ParentRec;


struct ClassRec;

struct MethodRec {
  char*             _name;             /*  Le de la methode                    */
  char*             _class_name;       /*  Le nom de la classe                 */
  ScopeType         _scope;            /*  La portee de la methode             */
  int               _virtual_decl;     /*  La methode est virtuelle ou non     */
  FileRec*          _decl_file;        /*  Le fichier de declaration           */
  LineNumber        _decl_line;        /*  La ligne dans le fichier precedent  */
  FileRec*          _impl_file;        /*  Le fichier d'implementation         */
  LineNumber        _impl_line;        /*  La ligne d'implementation           */
  struct MethodRec* _next_marked;      /*  Le pointeur de marquage             */
  struct MethodRec* _next;             /*  La methode suivante                 */
};

typedef struct MethodRec MethodRec;


/*------------------------------------------------------------------------------
*/
struct ClassRec {
  char*            _name;              /*  La nom de la classe                 */
  FileRec*         _decl_file;         /*  Le fichier de declaration           */
  LineNumber       _decl_line;         /*  La ligne de declaration             */
  ParentRec*       _parents_list;      /*  La liste des parents                */
  int              _parents_count;     /*  Le nombre de parents                */
  MethodRec*       _methods_list;      /*  La liste des methodes               */
  struct ClassRec* _next_marked;       /*  Le pointeur de marquage             */
  struct ClassRec* _next;              /*  La classe suivante                  */
};
typedef struct ClassRec ClassRec;


/*------------------------------------------------------------------------------
//                         Le dictionnaire des classes
//------------------------------------------------------------------------------
*/

#define CLASS_DICT_SIZE      503

extern ClassRec*  class_dict[CLASS_DICT_SIZE];

extern int        class_count;


/*------------------------------------------------------------------------------
//       Les procedures utilisees manipuler le dictionnaire des fichiers
//------------------------------------------------------------------------------
*/

extern ClassRec*    create_class  (/* char* file_name */);

extern ClassRec*    find_class    (/* char* file_name */);


/*------------------------------------------------------------------------------
//          La procedure d'initialisation du dictionnaire des classes
//------------------------------------------------------------------------------
*/

extern   void init_class ();

#endif  /*  CLASSDICT_H  */

