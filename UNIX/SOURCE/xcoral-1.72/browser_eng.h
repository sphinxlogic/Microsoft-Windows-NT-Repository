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

#ifndef BROWSERENGINE_H
#define BROWSERENGINE_H


/*------------------------------------------------------------------------------
//       Les procedures pour consulter le dictionnaire des classes
//
//    toutes les informations sont retournees via un pointeur sur une 
//     structure de donnees alloue'e dynamiquement. Ce pointeur (et 
//     uniquement lui) doit e^tre de'salloue' par un appel explicite
//     a la fonction free().
//------------------------------------------------------------------------------
*/

extern StringTable* get_files_list    ();


extern StringTable* get_classes_list  ();

extern StringTable* get_parents_list  (/* char* class_name */);

extern StringTable* get_sons_list     (/* char* class_name */);

extern StringTable* get_methods_list  (/* char* class_name */);

extern Position*    get_class_decl    (/* char* class_name */);

extern Position*    get_method_decl   (/* char* class_name,
				                          char* method_name */);

extern Position*    get_method_impl   (/* char* class_name,
				                          char* method_name */);


extern StringTable* get_procs_list    ();

extern Position*    get_proc_impl     (/* char* proc_name */);


/*------------------------------------------------------------------------------
//                     Les procedures de gestion du "Browser"
//------------------------------------------------------------------------------
*/

extern void init_browser ();

extern void parse_file   (/* char* file_name */);

extern void delete_file  (/* char* file_name */);


#endif  /*  BROWSERENGINE_H  */

