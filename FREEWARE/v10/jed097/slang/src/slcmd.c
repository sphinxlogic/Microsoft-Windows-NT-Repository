/* cmd line facility for slang */
#include <stdio.h>
#include "slang.h"
#include "_slang.h"

SLcmd_Cmd_Type *SLcmd_find_command (char *s, SLcmd_Cmd_Type *cmd)
{
   char *cmdstr;
   register char chs = *s++, ch;
   while (1)
     {
	cmdstr = cmd->cmd;
	ch = *cmdstr++;
	if (ch == 0) break;
	if ((ch == chs) && !strcmp (s, cmdstr)) return cmd;
	cmd++;
     }
   return NULL;
}

int SLcmd_execute_string (char *str, SLcmd_Cmd_Table_Type *table)
{
   char *s, *b = NULL, *arg_type, *last_str;
   SLcmd_Cmd_Type *cmd;
   char buf[256];
   int token_present;
   int i, ret = -1, len;
   int argc;
   
   if ((0 == SLang_extract_token (&str, buf, 0))
       || (*buf == '%'))
     return 0;
   if (SLang_Error) return -1;
   
   if (((len = strlen (buf)) >= 32)
       || (NULL == (cmd = SLcmd_find_command (buf, table->table))))
     {
	SLang_doerror ("Invalid command.");
	return -1;
     }

   
   if (NULL == (s = (char *) SLMALLOC(1 + len))) 
     {
	SLang_Error = SL_MALLOC_ERROR;
	return -1;
     }
   strcpy (s, buf);
   argc = 0;
   table->string_args[argc++] = s;
   
   arg_type = cmd->arg_type;
   
   while (*arg_type)
     {
	last_str = str;
	token_present = SLang_extract_token (&str, buf, 0);
	if (SLang_Error) goto error;
	if (token_present)
	  {
	     b = buf;
	     len = strlen (b);
	     if ((*b == '"') && (len > 1))
	       {
		  b++;
		  len -= 2;
		  b[len] = 0;
	       }
	  }
	
	switch (*arg_type++)
	  {
	     /* variable argument number */
	   case 'v':
	     if (token_present == 0) break;
	   case 'V':
	     if (token_present == 0)
	       {
		  SLang_doerror ("Expecting argument.");
		  goto error;
	       }
	     
	     while (*last_str == ' ') last_str++;
	     len = strlen (last_str);
	     str = last_str + len;
	     
	     s = SLmake_nstring (last_str, len);
	     if (s == NULL) goto error;
	     table->string_args[argc++] = s;
	     break;
	     
	   case 's':
	     if (token_present == 0) break;
	   case 'S':
	     if (token_present == 0)
	       {
		  SLang_doerror ("Expecting string argument.");
		  goto error;
	       }
	     
	     s = SLmake_nstring (b, len);

	     if (s == NULL) goto error;
	     table->string_args[argc++] = s;
	     break;
	     
	     /* integer argument */
	   case 'i':
	     if (token_present == 0) break;
	   case 'I':
	     if ((token_present == 0) || (INT_TYPE != slang_guess_type (buf)))
	       {
		  SLang_doerror ("Expecting integer argument.");
		  SLang_Error = TYPE_MISMATCH;
		  goto error;
	       }

	     table->int_args[argc++] = SLatoi((unsigned char *) buf);
	     break;

	     /* floating point arg */
#ifdef FLOAT_TYPE
	   case 'f':
	     if (token_present == 0) break;
	   case 'F':
	     if ((token_present == 0) || (STRING_TYPE == slang_guess_type (buf)))
	       {
		  SLang_doerror ("Expecting floating point argument.");
		  SLang_Error = TYPE_MISMATCH;
		  goto error;
	       }
	     table->float_args[argc++] = atof(buf);
	     break;
#endif
	  }
     }
   
   /*                 call function */
   ret = (*cmd->cmdfun)(argc, table);
   
   error:
   for (i = 0; i < argc; i++)
     {
	if (NULL != table->string_args[i])
	  {
	     SLFREE (table->string_args[i]);
	     table->string_args[i] = NULL;
	  }
     }
   return ret;
   
}

   


   
