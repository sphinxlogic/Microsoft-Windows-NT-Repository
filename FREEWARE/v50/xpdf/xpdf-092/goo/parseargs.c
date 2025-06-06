/*
 * parseargs.h
 *
 * Command line argument parser.
 *
 * Copyright 1996 Derek B. Noonburg
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parseargs.h"

static ArgDesc *findArg(ArgDesc *args, char *arg);
static GBool grabArg(ArgDesc *arg, int i, int *argc, char *argv[]);

GBool parseArgs(ArgDesc *args, int *argc, char *argv[]) {
  ArgDesc *arg;
  int i, j;
  GBool ok;

  ok = gTrue;
  i = 1;
  while (i < *argc) {
    if (!strcmp(argv[i], "--")) {
      --*argc;
      for (j = i; j < *argc; ++j)
	argv[j] = argv[j+1];
      break;
    } else if ((arg = findArg(args, argv[i]))) {
      if (!grabArg(arg, i, argc, argv))
	ok = gFalse;
    } else {
      ++i;
    }
  }
  return ok;
}

void printUsage(char *program, char *otherArgs, ArgDesc *args) {
  ArgDesc *arg;
  char *typ;
  int w, w1;

  w = 0;
  for (arg = args; arg->arg; ++arg) {
    if ((w1 = strlen(arg->arg)) > w)
      w = w1;
  }

  fprintf(stderr, "Usage: %s [options]", program);
  if (otherArgs)
    fprintf(stderr, " %s", otherArgs);
  fprintf(stderr, "\n");

  for (arg = args; arg->arg; ++arg) {
    fprintf(stderr, "  %s", arg->arg);
    w1 = 9 + w - strlen(arg->arg);
    switch (arg->kind) {
    case argInt:
    case argIntDummy:
      typ = " <int>";
      break;
    case argFP:
    case argFPDummy:
      typ = " <fp>";
      break;
    case argString:
    case argStringDummy:
      typ = " <string>";
      break;
    case argFlag:
    case argFlagDummy:
    default:
      typ = "";
      break;
    }
    fprintf(stderr, "%-*s", w1, typ);
    if (arg->usage)
      fprintf(stderr, ": %s", arg->usage);
    fprintf(stderr, "\n");
  }
}

static ArgDesc *findArg(ArgDesc *args, char *arg) {
  ArgDesc *p;

  for (p = args; p->arg; ++p) {
    if (p->kind < argFlagDummy && !strcmp(p->arg, arg))
      return p;
  }
  return NULL;
}

static GBool grabArg(ArgDesc *arg, int i, int *argc, char *argv[]) {
  int n;
  int j;
  GBool ok;

  ok = gTrue;
  n = 0;
  switch (arg->kind) {
  case argFlag:
    *(GBool *)arg->val = gTrue;
    n = 1;
    break;
  case argInt:
    if (i + 1 < *argc && isInt(argv[i+1])) {
      *(int *)arg->val = atoi(argv[i+1]);
      n = 2;
    } else {
      ok = gFalse;
      n = 1;
    }
    break;
  case argFP:
    if (i + 1 < *argc && isFP(argv[i+1])) {
      *(double *)arg->val = atof(argv[i+1]);
      n = 2;
    } else {
      ok = gFalse;
      n = 1;
    }
    break;
  case argString:
    if (i + 1 < *argc) {
      strncpy((char *)arg->val, argv[i+1], arg->size - 1);
      ((char *)arg->val)[arg->size - 1] = '\0';
      n = 2;
    } else {
      ok = gFalse;
      n = 1;
    }
    break;
  default:
    fprintf(stderr, "Internal error in arg table\n");
    n = 1;
    break;
  }
  if (n > 0) {
    *argc -= n;
    for (j = i; j < *argc; ++j)
      argv[j] = argv[j+n];
  }
  return ok;
}

GBool isInt(char *s) {
  if (*s == '-' || *s == '+')
    ++s;
  while (isdigit(*s))
    ++s;
  if (*s)
    return gFalse;
  return gTrue;
}

GBool isFP(char *s) {
  int n;

  if (*s == '-' || *s == '+')
    ++s;
  n = 0;
  while (isdigit(*s)) {
    ++s;
    ++n;
  }
  if (*s == '.')
    ++s;
  while (isdigit(*s)) {
    ++s;
    ++n;
  }
  if (n > 0 && (*s == 'e' || *s == 'E')) {
    ++s;
    if (*s == '-' || *s == '+')
      ++s;
    n = 0;
    if (!isdigit(*s))
      return gFalse;
    do {
      ++s;
    } while (isdigit(*s));
  }
  if (*s)
    return gFalse;
  return gTrue;
}
