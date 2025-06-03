/* error handling common to all routines. */
/* 
 * Copyright (c) 1994 John E. Davis
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <stdio.h>
#include <string.h>
#ifndef NO_STDLIB_H
#include <stdlib.h>
#endif

#include "slang.h"

void (*SLang_Error_Routine)(char *);
void (*SLang_Exit_Error_Hook)(char *);
volatile int SLang_Error = 0;

void SLang_doerror(char *error)
{
   char err[80]; char *str = NULL;

   if (!SLang_Error) SLang_Error = UNKNOWN_ERROR;
   *err = 0;
   switch(SLang_Error)
     {
	case (UNDEFINED_NAME): str = "Undefined_Name"; break;
	case (SYNTAX_ERROR): str = "Syntax_Error"; break;
	case (STACK_OVERFLOW): str = "Stack_Overflow"; break;
	case (STACK_UNDERFLOW): str = "Stack_Underflow"; break;
	case (DUPLICATE_DEFINITION): str = "Duplicate_Definition"; break;
	case (TYPE_MISMATCH): str = "Type_Mismatch"; break;
	case(READONLY_ERROR): str = "Variable is read only."; break;
	case (SL_MALLOC_ERROR) : str = "S-Lang: Malloc Error."; break;
	case (SL_INVALID_PARM) : str = "S-Lang: Invalid Parameter."; break;
      case USER_BREAK: strcpy(err, "User Break!"); break;
	case (INTRINSIC_ERROR): str = "Intrinsic Error"; break;
      case DIVIDE_ERROR: str = "Divide by zero."; break;
	/* application code should handle this */
	default: if (error != NULL) str = error; else str = "Unknown Error.";
     }

   if (*err == 0) sprintf(err, "S-Lang Error: %s", str);
   
   if (SLang_Error_Routine == NULL)
     {
	if (error != NULL) 
	  {
	     fputs(error, stderr);
	     putc('\n', stderr);
	  }
	
	if (err != error) 
	  {
	     fputs(err, stderr);
	     putc('\n', stderr);
	  }
     }
   else
     {	if (error != NULL) (*SLang_Error_Routine)(error);
	if (err != error) (*SLang_Error_Routine)(err);
     }
}

void SLang_exit_error (char *s)
{
   if (SLang_Exit_Error_Hook != NULL)
     {
	(*SLang_Exit_Error_Hook) (s);
     }
   fprintf (stderr, s);
   exit (-1);
}
