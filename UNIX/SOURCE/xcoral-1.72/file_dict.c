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

#include "result_types.h"
#include "file_dict.h"
#include "browser_util.h"
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------------
//                         Le dictionnaire des fichiers
//------------------------------------------------------------------------------
*/

FileRec* file_dict[FILE_DICT_SIZE];

int      file_count = 0;


/*------------------------------------------------------------------------------
*/
FileRec* create_file (file_name)
  char* file_name;
{
	FileRec**  head;
  FileRec*   current_file;
	int        x_size;

	get_head_Rec(file_name, file_dict, FILE_DICT_SIZE, head);
  search_Rec(file_name, FileRec, head, current_file);
  if (current_file == Null) {
    x_size = sizeof(FileRec) + FILE_PLENGTH + strlen(file_name)  + 1;
    current_file = (FileRec*) xmalloc(x_size);
    if (current_file != Null) {
      create_Rec(file_name, FileRec, head, current_file, FILE_PREFIX, FILE_PLENGTH);
			file_count++;
		}
	}
  return(current_file);
}


/*------------------------------------------------------------------------------
*/
FileRec* find_file(file_name)
  char* file_name;
{
	FileRec**  head;
  FileRec*   current_file;

	get_head_Rec(file_name, file_dict, FILE_DICT_SIZE, head);
  search_Rec(file_name, FileRec, head, current_file);
  return(current_file);
}


/*------------------------------------------------------------------------------
*/
void remove_file(file_name)
  char* file_name;
{
  FileRec**  head;
  FileRec*   current_file;

	get_head_Rec(file_name, file_dict, FILE_DICT_SIZE, head);
  extract_Rec(file_name, FileRec, head, current_file);
	if (current_file != Null) {
	  free(current_file);
		--file_count;
	}
}


/*------------------------------------------------------------------------------
*/
void init_file() {
  int index;

  for (index = 0; index < FILE_DICT_SIZE; index++)
    file_dict[index] = Null;
}


