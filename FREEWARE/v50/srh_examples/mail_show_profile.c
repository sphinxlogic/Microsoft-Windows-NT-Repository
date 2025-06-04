/*
/*  mail_show_profile.c
/*
/*  Demonstrates the use of the callable MAIL API.
/*  Displays the profile of the specified user.
*/

  
#include <ctype.h> 
#include <jpidef.h> 
#include <lib$routines.h> 
#include <maildef.h> 
#include <mail$routines.h> 
#include <rms.h> 
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

int main (int argc, char *argv[]) 
  { 
  unsigned short int userlen = 0;
  
  /* return length of strings */ 
  
  unsigned short int editor_len = 0, 
    form_len = 0, 
    forwarding_len = 0, 
    full_directory_len = 0, 
    personal_name_len = 0, 
    queue_len = 0;
  
  /* Flags */ 
  
  int auto_purge = 0, 
    cc_prompt = 0, 
    copy_forward = 0, 
    copy_reply = 0, 
    copy_send = 0; 

  char delimiter[3];

  int user_context = 0; 
  
  char user[32+1], 
    editor[NAM$C_MAXRSS], 
    form[NAM$C_MAXRSS], 
    forwarding[NAM$C_MAXRSS], 
    full_directory[NAM$C_MAXRSS], 
    personal_name[NAM$C_MAXRSS], 
    queue[NAM$C_MAXRSS]; 
  
  unsigned short int new_messages = 0; 
  
  ITMLST jpi_list[]  = 
      { 
      {sizeof(user) - 1, JPI$_USERNAME, user, &userlen}, 
      {0,0,0,0}
      }, 
    user_itmlst[] = 
      { 
      {0, MAIL$_USER_USERNAME, 0, 0}, 
      {0,0,0,0}
      }, 
    out_itmlst[] = 
      { 
      /* Full directory spec */ 
      {sizeof(full_directory),MAIL$_USER_FULL_DIRECTORY,full_directory,&full_directory_len}, 
      /* New message count */ 
      {sizeof(new_messages), MAIL$_USER_NEW_MESSAGES, &new_messages, 0},     
      /* Forwarding field */ 
      {sizeof(forwarding), MAIL$_USER_FORWARDING, forwarding, &forwarding_len}, 
      /* Personal name field */ 
      {sizeof(personal_name), MAIL$_USER_PERSONAL_NAME, personal_name, &personal_name_len}, 
      /* Editor field */ 
      {sizeof(editor), MAIL$_USER_EDITOR, editor, &editor_len},       
      /* CC prompting flag */ 
      {sizeof(cc_prompt), MAIL$_USER_CC_PROMPT, &cc_prompt, 0},       
      /* Copy send flag */ 
      {sizeof(copy_send), MAIL$_USER_COPY_SEND, &copy_send, 0},      
      /* Copy reply flag */ 
      {sizeof(copy_reply), MAIL$_USER_COPY_REPLY, &copy_reply, 0},      
      /* Copy forward flag */ 
      {sizeof(copy_forward), MAIL$_USER_COPY_FORWARD, &copy_forward, 0},     
      /* Auto purge flag */ 
      {sizeof(auto_purge), MAIL$_USER_AUTO_PURGE, &auto_purge, 0},     
      /* Queue field */ 
      {sizeof(queue), MAIL$_USER_QUEUE, queue, &queue_len},      
      /* Form field */ 
      {sizeof(form), MAIL$_USER_FORM, form, &form_len},       
      {0,0,0,0}
      }; 
  int status;
  unsigned short int iosb[4];
  ITMLST NullIL[] = { {0,0,0,0} }; 

  /* Get a mail user context */ 
  status = MAIL$USER_BEGIN(&user_context, &NullIL, &NullIL); 
  if (!$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
   
  if (argc > 1) 
    { 
    strcpy(user,argv[1]); 
    }    
  else 
    { 
    status = sys$getjpiw(0,0,0,jpi_list,iosb,0,0); 
    if (!$VMS_STATUS_SUCCESS( status ))
      lib$signal( status );
    if (!$VMS_STATUS_SUCCESS( iosb[0] ))
      lib$signal( status );
    user[userlen] = '\0'; 
    }; 
   
  while(isspace(user[--userlen])) 
    user[userlen] = '\0'; 
  
  user_itmlst[0].buffer_length = strlen(user); 
  user_itmlst[0].buffer_address = user; 
  
  status = MAIL$USER_GET_INFO(&user_context, user_itmlst, out_itmlst); 
  if (!$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
  
  /* Release the mail USER context */ 
  status = MAIL$USER_END(&user_context, &NullIL, &NullIL); 
  if (!$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
   
  /* display the information just gathered */ 
  
  full_directory[full_directory_len] = '\0'; 
  printf("Displaying MAIL settings for user %s\n", user ); 
  printf("Mail file directory is %s.\n", full_directory); 
  switch ( new_messages )
    {
    case 0:
      printf("There are no new messages.\n"); 
      break;
    case 1:
      printf("There is one new message.\n"); 
      break;
    default:
      printf("There are %d new messages.\n", new_messages); 
      break;
    }
  
  forwarding[forwarding_len] = '\0'; 
  if (strlen(forwarding) == 0) 
    printf("No forwarding address set.\n"); 
  else 
    printf("Forwarding is set to %s.\n", forwarding); 
  
  personal_name[personal_name_len] = '\0'; 
  printf("The personal name is \"%s\"\n", personal_name); 
  
  editor[editor_len] = '\0'; 
  if (strlen(editor) == 0) 
    printf("No editor was specified.\n"); 
  else 
    printf("The editor is %s\n", editor); 
  
  printf("CC prompting is %s.\n", (cc_prompt == TRUE) ? "disabled" : "enabled"); 
  
  strcpy( delimiter, " ");
  printf("Automatic copy"); 
  if (copy_send == TRUE) 
    {
    printf("%son SEND", delimiter); 
    strcpy( delimiter, ", ");
    }
  if (copy_reply == TRUE) 
    { 
    printf("%son REPLY", delimiter); 
    strcpy( delimiter, ", ");
    } 
  if (copy_forward == TRUE)
    { 
    printf("%son FORWARD", delimiter); 
    strcpy( delimiter, ", ");
    } 
  if (!copy_reply && !copy_send && !copy_forward )
    printf(" is disabled"); 
  printf("\n"); 
  
  printf("Automatic deleted message purge is %s.\n", 
    (auto_purge == TRUE) ? "disabled" : "enabled"); 
  
  queue[queue_len] = '\0'; 
  if (strlen(queue) == 0) 
    printf("No specified default print queue.\n"); 
  else 
    printf("The default print queue is %s.\n", queue); 
  form[form_len] = '\0'; 
  if (strlen(form) == 0) 
    printf("No specified default print form.\n"); 
  else 
    printf("The default print form is %s.\n", form); 

  return SS$_NORMAL;
  } 
