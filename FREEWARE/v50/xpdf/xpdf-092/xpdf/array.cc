//========================================================================
//
// Array.cc
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stddef.h>
#include "gmem.h"
#include "Object.h"
#include "Array.h"

//------------------------------------------------------------------------
// Array
//------------------------------------------------------------------------

Array::Array() {
  elems = NULL;
  size = length = 0;
  ref = 1;
}

Array::~Array() {
  int i;

  for (i = 0; i < length; ++i)
    elems[i].free();
  gfree(elems);
}

void Array::add(Object *elem) {
  if (length + 1 > size) {
    size += 8;
    elems = (Object *)grealloc(elems, size * sizeof(Object));
  }
  elems[length] = *elem;
  ++length;
}

Object *Array::get(int i, Object *obj) {
  return elems[i].fetch(obj);
}

Object *Array::getNF(int i, Object *obj) {
  return elems[i].copy(obj);
}
