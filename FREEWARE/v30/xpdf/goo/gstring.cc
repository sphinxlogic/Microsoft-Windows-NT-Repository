//========================================================================
//
// GString.cc
//
// Simple variable-length string type.
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <GString.h>

static inline int size(int len) {
  int delta;

  delta = len < 256 ? 7 : 255;
  return ((len + 1) + delta) & ~delta;
}

inline void GString::resize(int length1) {
  char *s1;

  if (!s) {
    s = new char[size(length1)];
  } else if (size(length1) != size(length)) {
    s1 = new char[size(length1)];
    memcpy(s1, s, length + 1);
    delete[] s;
    s = s1;
  }
}

GString::GString() {
  s = NULL;
  resize(length = 0);
  s[0] = '\0';
}

GString::GString(char *s1) {
  int n = strlen(s1);

  s = NULL;
  resize(length = n);
  memcpy(s, s1, n + 1);
}

GString::GString(char *s1, int length1) {
  s = NULL;
  resize(length = length1);
  memcpy(s, s1, length * sizeof(char));
  s[length] = '\0';
}

GString::GString(GString *str) {
  s = NULL;
  resize(length = str->getLength());
  memcpy(s, str->getCString(), length + 1);
}

GString::GString(GString *str1, GString *str2) {
  int n1 = str1->getLength();
  int n2 = str2->getLength();

  s = NULL;
  resize(length = n1 + n2);
  memcpy(s, str1->getCString(), n1);
  memcpy(s + n1, str2->getCString(), n2 + 1);
}

GString::~GString() {
  delete[] s;
}

GString *GString::clear() {
  resize(0);
  s[length = 0] = '\0';
  return this;
}

GString *GString::append(char c) {
  resize(length + 1);
  s[length++] = c;
  s[length] = '\0';
  return this;
}

GString *GString::append(GString *str) {
  int n = str->getLength();

  resize(length + n);
  memcpy(s + length, str->getCString(), n + 1);
  length += n;
  return this;
}

GString *GString::append(char *str) {
  int n = strlen(str);

  resize(length + n);
  memcpy(s + length, str, n + 1);
  length += n;
  return this;
}

GString *GString::insert(int i, char c) {
  int j;

  resize(length + 1);
  for (j = length + 1; j > i; --j)
    s[j] = s[j-1];
  s[i] = c;
  ++length;
  return this;
}

GString *GString::insert(int i, GString *str) {
  int n = str->getLength();
  int j;

  resize(length + n);
  for (j = length; j >= i; --j)
    s[j+n] = s[j];
  memcpy(s+i, str->getCString(), n);
  length += n;
  return this;
}

GString *GString::insert(int i, char *str) {
  int n = strlen(str);
  int j;

  resize(length + n);
  for (j = length; j >= i; --j)
    s[j+n] = s[j];
  memcpy(s+i, str, n);
  length += n;
  return this;
}

GString *GString::del(int i, int n) {
  int j;

  if (n > 0) {
    for (j = i; j <= length - n; ++j)
      s[j] = s[j + n];
    resize(length -= n);
  }
  return this;
}
