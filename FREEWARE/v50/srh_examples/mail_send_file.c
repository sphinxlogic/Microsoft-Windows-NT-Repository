/*
/*  mail_send_file.c 
/*
/*  Demonstrates the use of the callable MAIL API.
/*  Sends the specified text file as a mail message.
*/ 
  
#include <descrip.h> 
#include <lib$routines.h>
#include <maildef.h> 
#include <mail$routines.h>
#include <rms.h> 
#include <ssdef.h> 
#include <starlet.h>
#include <stdio.h> 
#include <string.h>
#include <stsdef.h>
   
typedef struct itmlst 
  { 
  unsigned short int buffer_length; 
  unsigned short int item_code; 
  void *buffer_address; 
  unsigned short int *return_length_address; 
  } ITMLST; 

  
  
int getline(char *line, int max) 
  { 
  if (fgets(line, max, stdin) == NULL) 
    return 0; 
  else 
    return strlen(line); 
  } 

int main (int argc, char *argv[]) 
  { 
  char to_user[NAM$C_MAXRSS], 
    subject_line[NAM$C_MAXRSS], 
    file[NAM$C_MAXRSS], 
    resultspec[NAM$C_MAXRSS]; 
  unsigned short int resultspeclen = 0; 
  int send_context = 0; 
  ITMLST nulllist[] = { {0,0,0,0} }; 
  
  int status;
  
  ITMLST
     address_itmlst[] =
       { 
       {sizeof(to_user), MAIL$_SEND_USERNAME, to_user, NULL}, 
       {0,0,0,0}
       }, 
     bodypart_itmlst[] = 
       { 
       {sizeof(file), MAIL$_SEND_FILENAME, file, NULL }, 
       {0,0,0,0}
       }, 
     out_bodypart_itmlst[] = 
       { 
       {sizeof(resultspec), MAIL$_SEND_RESULTSPEC, resultspec, &resultspeclen }, 
       {0,0,0,0}
       }, 
     attribute_itmlst[] = { 
       {sizeof(to_user), MAIL$_SEND_TO_LINE, to_user, NULL }, 
       {sizeof(subject_line), MAIL$_SEND_SUBJECT, subject_line, NULL}, 
       {0,0,0,0}}; 
  
  
  status = mail$send_begin(&send_context, &nulllist, &nulllist); 
  if ( !$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
  
  /* Get the destination and add it to the message */ 
  printf("To: "); 
  to_user[getline(to_user, NAM$C_MAXRSS) - 1] = '\0'; 
  
  address_itmlst[0].buffer_length = strlen(to_user); 
  address_itmlst[0].buffer_address = to_user; 
  
  status = mail$send_add_address(&send_context, address_itmlst, &nulllist); 
  if ( !$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
   
  /* Get the subject line and add it to the message header */ 
  printf("Subject: "); 
  subject_line[getline(subject_line, NAM$C_MAXRSS) - 1] = '\0'; 
  
  /*  Displayed TO: line */ 
  attribute_itmlst[0].buffer_length = strlen(to_user); 
  attribute_itmlst[0].buffer_address = to_user; 
  
  /* Subject: line */ 
  attribute_itmlst[1].buffer_length = strlen(subject_line); 
  attribute_itmlst[1].buffer_address = subject_line; 
  
  status = mail$send_add_attribute(&send_context, attribute_itmlst, &nulllist); 
  if ( !$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
  
  /* Get the file to send and add it to the bodypart of the message */ 
  printf("File: "); 
  file[getline(file, NAM$C_MAXRSS) - 1] = '\0'; 
  
  bodypart_itmlst[0].buffer_length = strlen(file); 
  bodypart_itmlst[0].buffer_address = file; 
  
  status = mail$send_add_bodypart(&send_context, bodypart_itmlst, out_bodypart_itmlst); 
  if ( !$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
  
  resultspec[resultspeclen] = '\0'; 
  printf("Full file spec actually sent: [%s]\n", resultspec); 
  
  /* Send the message */ 
  status = mail$send_message(&send_context, nulllist, nulllist); 
  if ( !$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
  
  /* Done processing witht the SEND context */ 
  status = mail$send_end(&send_context, nulllist, nulllist); 
  if ( !$VMS_STATUS_SUCCESS( status ))
    lib$signal( status );
  
  return (status); 
  } 
