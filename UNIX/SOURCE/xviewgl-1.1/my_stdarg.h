#ifndef MY_STDARG_INCLUDED
#define MY_STDARG_INCLUDED
#include "patchlevel.h"
#define MAXARGS 255

typedef char *my_va_list[MAXARGS];

#define set_arg(ap,n,v) ((ap)[n]=(v))
#define my_va_arg_c(ap) (*(ap++))
#define my_va_arg_i(ap) (atol(*(ap++)))
#endif
