/*	Copyright (C) 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */



/* This is a collection of stubs that are used to call interactive functions.
 * Their responsability is to extract arguments from a command_frame as
 * constructed by the function COMMAND_LOOP.
 */

#include "global.h"
#include "cmd.h"
#include "stub.h"




#ifdef __STDC__
static void
find_args (struct command_arg ** argv_out, int argc, struct command_frame * frame)
#else
static void
find_args (argv_out, argc, frame)
     struct command_arg ** argv_out;
     int argc;
     struct command_frame * frame;
#endif
{
  int found = 0;
  int pos = 0;
  while (found < argc)
    {
      if (frame->argv[pos].style->representation != cmd_none)
	argv_out[found++] = &frame->argv[pos];
      ++pos;
    }
}


/* These macros are invoked in stubs.h and are used to define
 * the stub functions.  Later, these macros will be redifined 
 * an used to build a table of stub functions.
 */

#ifdef __STDC__
#define STUB1(STR,NAME, PRE, VAL, TYPE) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv; \
  find_args (&argv, 1, frame); \
  ((void (*) (TYPE)) frame->cmd->func_func) (PRE argv->val.VAL); \
}
#else
#define STUB1(STR,NAME, PRE, VAL, TYPE) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  int x; \
  struct command_arg * argv; \
  find_args (&argv, 1, frame); \
  frame->cmd->func_func (PRE argv->val.VAL); \
}
#endif

#ifdef __STDC__
#define STUB2(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[2]; \
  find_args (argv, 2, frame); \
  ((void (*) (TYPE1, TYPE2)) frame->cmd->func_func) \
    (PRE1 argv[0]->val.VAL1,  \
     PRE2 argv[1]->val.VAL2); \
}
#else
#define STUB2(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[2]; \
  find_args (argv, 2, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1,  \
     PRE2 argv[1]->val.VAL2); \
}
#endif 


#ifdef __STDC__
#define STUB3(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[3]; \
  find_args (argv, 3, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3); \
}
#else
#define STUB3(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[3]; \
  find_args (argv, 3, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3); \
}
#endif

#ifdef __STDC__
#define STUB4(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[4]; \
  find_args (argv, 4, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3, \
     PRE4 argv[3]->val.VAL4); \
}
#else
#define STUB4(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[4]; \
  find_args (argv, 4, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3, \
     PRE4 argv[3]->val.VAL4); \
}
#endif

#ifdef __STDC__
#define STUB5(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4,PRE5,VAL5,TYPE5) \
static void \
NAME (frame) \
     struct command_frame * frame; \
{ \
  struct command_arg * argv[5]; \
  find_args (argv, 5, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3, \
     PRE4 argv[3]->val.VAL4, \
     PRE5 argv[4]->val.VAL5); \
}
#else
#define STUB5(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4,PRE5,VAL5,TYPE5) \
static void \
NAME (frame) \
          struct command_frame * frame; \
{ \
  struct command_arg * argv[5]; \
  find_args (argv, 5, frame); \
  frame->cmd->func_func \
    (PRE1 argv[0]->val.VAL1, \
     PRE2 argv[1]->val.VAL2, \
     PRE3 argv[2]->val.VAL3, \
     PRE4 argv[3]->val.VAL4, \
     PRE5 argv[4]->val.VAL5); \
}
#endif

/* This contains the list of stub functions. */

#include "stubs.h"

/* There is only one `STUB0' so we needn't bother with a macro. */

#ifdef __STDC__
static void
stub_void (struct command_frame * frame)
#else
static void
stub_void (frame)
     struct command_frame * frame;
#endif
{
  frame->cmd->func_func ();
}

/* For define_usr_fmt */
static void
stub_isssssssss (frame)
     struct command_frame * frame;
{
  struct command_arg * argv[10];
  find_args (argv, 10, frame);
  frame->cmd->func_func (argv[0]->val.integer,
			 argv[1]->val.string,
			 argv[2]->val.string,
			 argv[3]->val.string,
			 argv[4]->val.string,
			 argv[5]->val.string,
			 argv[6]->val.string,
			 argv[7]->val.string,
			 argv[8]->val.string,
			 argv[9]->val.string);
}





/* Single character type-codes denote the types of arguments.  A string
 * of type-codes maps to a stub function (hopefully).
 */
struct cmd_stub
{
  char * type;
  cmd_invoker stub;
};

#undef STUB1
#undef STUB2
#undef STUB3
#undef STUB4
#undef STUB5

#define STUB1(STR,NAME,PRE1,VAL1,TYPE1) { STR, NAME },
#define STUB2(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2)  { STR, NAME },
#define STUB3(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3) \
	{ STR, NAME },
#define STUB4(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4) \
	{ STR, NAME },
#define STUB5(STR,NAME,PRE1,VAL1,TYPE1,PRE2,VAL2,TYPE2,PRE3,VAL3,TYPE3,PRE4,VAL4,TYPE4,PRE5,VAL5,TYPE5) \
	{ STR, NAME },

static struct cmd_stub the_stubs[] =
{
  { "", stub_void },
#include "stubs.h"
  { "isssssssss", stub_isssssssss },
  { 0, 0 }
};



/* This looks at the arguments built for the current command and 
 * finds the right stub.
 */
#ifdef __STDC__
cmd_invoker
find_stub (void)
#else
cmd_invoker
find_stub ()
#endif
{
  char type_buf[100];

  /* Figure out a name for the stub we want. */
  {
    int x, bufpos;
    for (x = 0, bufpos = 0; x < cmd_argc; ++x)
      if (the_cmd_frame->argv[x].style->representation != cmd_none)
	type_buf[bufpos++] = the_cmd_frame->argv[x].style->representation;
    type_buf[bufpos] = '\0';
  }

  /* Look for the stub. */
  {
    int x;
    for (x = 0; the_stubs[x].type; ++x)
      if (!stricmp (the_stubs[x].type, type_buf))
	break;
    return the_stubs[x].stub;
  }
}

