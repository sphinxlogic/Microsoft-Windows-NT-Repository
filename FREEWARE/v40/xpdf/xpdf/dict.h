//========================================================================
//
// Dict.h
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifndef DICT_H
#define DICT_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"

//------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------

struct DictEntry {
  char *key;
  Object val;
};

class Dict {
public:

  // Constructor.
  Dict();

  // Destructor.
  ~Dict();

  // Reference counting.
  int incRef() { return ++ref; }
  int decRef() { return --ref; }

  // Get number of entries.
  int getLength() { return length; }

  // Add an entry.  NB: does not copy key.
  void add(char *key, Object *val);

  // Check if dictionary is of specified type.
  GBool is(char *type);

  // Look up an entry and return the value.  Returns a null object
  // if <key> is not in the dictionary.
  Object *lookup(char *key, Object *obj);
  Object *lookupNF(char *key, Object *obj);

  // Iterative accessors.
  char *getKey(int i);
  Object *getVal(int i, Object *obj);
  Object *getValNF(int i, Object *obj);

private:

  DictEntry *entries;		// array of entries
  int size;			// size of <entries> array
  int length;			// number of entries in dictionary
  int ref;			// reference count

  DictEntry *find(char *key);
};

#endif
