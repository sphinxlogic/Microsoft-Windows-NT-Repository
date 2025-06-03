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

#ifndef BROWSERPARSE_H
#define BROWSERPARSE_H


/*------------------------------------------------------------------------------
*/
enum BrowserError {
  NO_BERROR,
  BERROR
};

typedef enum BrowserError BrowserError;


/*------------------------------------------------------------------------------
*/
extern FileRec*  parsed_file;

extern ClassRec* class_cache;


/*------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
*/

extern BrowserError add_file        (/* char*      file_name */);


/*------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
*/

extern BrowserError add_proc        (/* char*      proc_name,
                                        ProcType   proc_type,
                                        LineNumber impl_line */);


/*------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
*/

extern BrowserError add_class       (/* char*      class_name,
                                        LineNumber decl_line */);

extern BrowserError add_parent      (/* char*      class_name,
                                        char*      parent_name,
                                        ScopeType  scope */);

extern BrowserError add_method_decl (/* char*      class_name, 
                                        char*      method_name,
                                        ScopeType  scope,
                                        int        virtual_decl,
                                        LineNumber decl_line */);

extern BrowserError add_method_impl (/* char*      class_name,
                                        char*      method_name,
                                        LineNumber impl_line */);


#endif    /*  BROWSERPARSE_H  */




