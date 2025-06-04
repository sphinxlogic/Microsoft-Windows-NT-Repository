/*
/*  mail_show_folders.c
/*
/*  Demonstrates the use of the callable MAIL API.
/*  Displays a list of folders and message counts.
*/
  
#include <ctype.h> 
#include <descrip.h> 
#include <lib$routines.h> 
#include <maildef.h> 
#include <mail$routines.h> 
#include <ssdef.h> 
#include <starlet.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stsdef.h> 
   
typedef struct itmlst 
  { 
  unsigned short int buffer_length; 
  unsigned short int item_code; 
  void *buffer_address; 
  unsigned short int *return_length_address; 
  } ITMLST; 
  
struct node 
  { 
  struct node *next;            /* Next folder name node */ 
  char *folder_name;            /* Zero terminated folder name */ 
  }; 

int folder_routine(struct node *list, struct dsc$descriptor *name) 
  { 
  if (name->dsc$w_length) 
    { 
    while (list->next) 
    list = list->next; 
  
    list->next = malloc(sizeof(struct node)); 
    list = list->next; 
    list->next = 0; 
    list->folder_name = malloc(name->dsc$w_length + 1); 
    strncpy(list->folder_name,name->dsc$a_pointer,name->dsc$w_length); 
    list->folder_name[name->dsc$w_length] = '\0'; 
       
    } 
  return(SS$_NORMAL); 
  } 
  
int main (int argc, char *argv[]) 
  { 
  struct node list = {0,0}; 
  int status;
  
  int message_context = 0, 
    file_context = 0, 
    messages_selected = 0, 
    total_folders = 0, 
    total_messages = 0; 

  ITMLST 
    NullIL[] = {{0,0,0,0}}, 
    message_in_itmlst[] = 
       { 
       {sizeof(file_context),MAIL$_MESSAGE_FILE_CTX,&file_context,0}, 
       {0,0,0,0}
       }, 
     mailfile_info_itmlst[] = 
       { 
       {4,MAIL$_MAILFILE_FOLDER_ROUTINE,folder_routine,0}, 
       {4,MAIL$_MAILFILE_USER_DATA,&list,0}, 
       {0,0,0,0}
       }, 
     message_select_in_itmlst[] = 
       { 
       {0,MAIL$_MESSAGE_FOLDER,0,0}, 
       {0,0,0,0}
       }, 
     message_select_out_itmlst[] = 
       { 
       {sizeof(messages_selected),MAIL$_MESSAGE_SELECTED,&messages_selected,0}, 
       {0,0,0,0}
       }; 
  struct node *tmp = &list; 
   
  status = mail$mailfile_begin(&file_context, NullIL, NullIL);
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );
  status = mail$mailfile_open(&file_context, NullIL, NullIL);
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );
  status = mail$mailfile_info_file(&file_context, mailfile_info_itmlst, NullIL);
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );
  status = mail$message_begin(&message_context, message_in_itmlst, NullIL);
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );

  while(tmp->next) 
    { 
    tmp = tmp->next; 
    message_select_in_itmlst[0].buffer_address = tmp->folder_name; 
    message_select_in_itmlst[0].buffer_length = strlen(tmp->folder_name); 
    status = mail$message_select(&message_context, message_select_in_itmlst, 
         message_select_out_itmlst);
    if (!$VMS_STATUS_SUCCESS( status ))
       lib$signal( status );
    printf("Folder %s has %d messages\n", 
    tmp->folder_name, messages_selected); 
    total_messages += messages_selected; 
    total_folders++; 
    } 

  printf("Total of %d messages in %d folders\n",total_messages, total_folders); 

  status = mail$message_end(&message_context, NullIL, NullIL); 
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );
  status = mail$mailfile_close(&file_context, NullIL, NullIL); 
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );
  status = mail$mailfile_end(&file_context, NullIL, NullIL); 
  if (!$VMS_STATUS_SUCCESS( status ))
     lib$signal( status );

  return status;
  }
