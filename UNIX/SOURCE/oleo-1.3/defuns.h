
/* This file repeatedly includes tahe contents of defun.h which contains
 * doc strings and FUNC_ARGS structures for all of the interactive
 * built-ins.
 * 
 * It uses ugly preprocessor magic to build various tables.
 */


/* First, build arrays containing FUNC_ARG strings. */

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
0, \
}; \
\
static char * FAname [] = \
{

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)

#define DEFUN_3(Sname, FDname, Cname)

#define FUNC_ARG_STRINGS	1

static char * FAfnord[] = 
{
#include "defun.h"
0,
};



/* Docstrings */

#undef DEFUN
#undef DEFUN_3
#undef DEFUN_5
#undef FUNC_ARG_STRINGS
#undef DOC_STRINGS
#undef FUNC_INIT_CODE

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
0, \
}; \
\
static char * FDname [] = \
{

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)
#define DEFUN_3(Sname, FDname, Cname) DEFUN(Sname, FDname, 0, Cname)

#define DOC_STRINGS	1

static char * FDfnord[] = 
{
  "Well, you know.",
#include "defun.h"
0,
};




/* init_code */

#undef DEFUN
#undef DEFUN_3
#undef DEFUN_5
#undef FUNC_ARG_STRINGS
#undef DOC_STRINGS
#undef FUNC_INIT_CODE

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
0, \
}; \
\
static char * DFname [] = \
{

#define DEFUN(Sname, FDname, FAname, Cname)
#define DEFUN_3(Sname, FDname, Cname) 

#define FUNC_INIT_CODE	1

static char * DFfnord[] = 
{
#include "defun.h"
0,
};





/* Building the function table. */

#undef DEFUN
#undef DEFUN_3
#undef DEFUN_5
#undef FUNC_ARG_STRINGS
#undef DOC_STRINGS
#undef FUNC_INIT_CODE

#define DEFUN_5(Sname, FDname, FAname, DFname, Cname) \
	{ Sname, FDname, FAname, DFname, Cname },

#define DEFUN(Sname, FDname, FAname, Cname) \
	DEFUN_5(Sname, FDname, FAname, 0, Cname)
#define DEFUN_3(Sname, FDname, Cname) \
	DEFUN_5(Sname, FDname, 0, 0, Cname)

struct cmd_func cmd_funcs[] =
{
  { "fnord", FDfnord, FAfnord, DFfnord, fnord },
#include "defun.h"
  { 0, 0, 0, 0, 0 }
};

