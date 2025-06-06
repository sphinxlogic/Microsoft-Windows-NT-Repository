//========================================================================
//
// Error.h
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifndef ERROR_H
#define ERROR_H

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

// File to send error (and other) messages to.
extern FILE *errFile;

extern void errorInit();

extern void error(int pos, char *msg, ...);

#endif
