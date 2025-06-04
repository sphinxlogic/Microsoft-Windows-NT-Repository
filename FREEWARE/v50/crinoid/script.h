/*  external script definitions */
#ifndef _SCRIPT_H
#define _SCRIPT_H 1

#include "CRINOID_types.h"
pScript new_Script(char *wild, int type, char *user);
pScript findwild_Script(char *in, char *server);

#endif
