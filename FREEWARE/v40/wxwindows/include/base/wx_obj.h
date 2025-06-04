/*
 * File:	wx_obj.h
 * Purpose:	Top level object for wxWindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   25 May 97   11:16 am
 */

/* sccsid[] = "@(#)wx_obj.h	1.2 5/9/94" */

#ifndef wxb_objh
#define wxb_objh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"

#ifdef IN_CPROTO
typedef       void    *wxObject ;
#else

class wxObject;

#if USE_DYNAMIC_CLASSES

#ifdef GNUWIN32
#ifdef GetClassName
#undef GetClassName
#endif
#endif

class wxClassInfo;

/*
 * Dynamic object system declarations
 */

typedef wxObject * (*wxObjectConstructorFn) (void);

class WXDLLEXPORT wxClassInfo
{
 public:
   char *className;
   char *baseClassName1;
   char *baseClassName2;
   int objectSize;
   wxObjectConstructorFn objectConstructor;

   // Pointers to base wxClassInfos: set in InitializeClasses
   // called from wx_main.cc
   wxClassInfo *baseInfo1;
   wxClassInfo *baseInfo2;

   static wxClassInfo *first;
   wxClassInfo *next;

   wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn fn);

   wxObject *CreateObject(void);
   inline char *GetClassName(void) { return className; }
   inline char *GetBaseClassName1(void) { return baseClassName1; }
   inline char *GetBaseClassName2(void) { return baseClassName2; }
   inline int GetSize(void) { return objectSize; }
   Bool IsKindOf(wxClassInfo *info);

   static wxClassInfo *FindClass(char *c);
   // Initializes parent pointers for fast searching.
   static void InitializeClasses(void);
};

WXDLLEXPORT wxObject *wxCreateDynamicObject(char *name);

#define DECLARE_DYNAMIC_CLASS(name) \
 public:\
  static wxClassInfo class##name;\
  wxClassInfo *GetClassInfo() \
   { return &name::class##name; }

#define DECLARE_ABSTRACT_CLASS(name) DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_CLASS(name) DECLARE_DYNAMIC_CLASS(name)

//////
////// for concrete classes
//////

// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS(name, basename) \
WXDLLEXPORT wxObject *wxConstructorFor##name(void) \
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name);

// Multiple inheritance with two base classes
#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2) \
WXDLLEXPORT wxObject *wxConstructorFor##name(void) \
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename1, #basename2, sizeof(name), wxConstructorFor##name);

//////
////// for abstract classes
//////

// Single inheritance with one base class
#define IMPLEMENT_ABSTRACT_CLASS(name, basename) \
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), NULL);

// Multiple inheritance with two base classes
#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2) \
 wxClassInfo name::class##name(#name, #basename1, #basename2, sizeof(name), NULL);

#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#define CLASSINFO(name) (&name::class##name)

#else

// No dynamic class system: so stub out the macros
#define DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_ABSTRACT_CLASS(name)
#define DECLARE_CLASS(name)
#define IMPLEMENT_DYNAMIC_CLASS(name, basename)
#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2)
#define IMPLEMENT_ABSTRACT_CLASS(name, basename)
#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)
#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#endif

// Unfortunately Borland seems to need this include.

#if USE_IOSTREAMH

#if defined(__BORLANDC__) || defined(__MWERKS)

#include <iostream.h>

#else

class istream;
class ostream;

#endif

#else

// Template version
#include <iostream>

#endif


#if WXGARBAGE_COLLECTION_ON
#include "gc_cpp.h"
#define WXGC_IGNORE(ptr) GC_general_register_disappearing_link((void **)&(ptr), NULL)
#define WXGC_ATOMIC (AtomicGC)
class WXDLLEXPORT wxObject : public gc_cleanup
#else
#define WXGC_IGNORE(ptr)
#define WXGC_ATOMIC
class WXDLLEXPORT wxObject
#endif
{
 protected:
  int refCount;
 public:

#if USE_OLD_TYPE_SYSTEM
  WXTYPE __type;
#endif

#if USE_DYNAMIC_CLASSES
  // This is equivalent to using the macro DECLARE_ABSTRACT_CLASS
  static wxClassInfo classwxObject;
#endif

  wxObject(void);
  virtual ~wxObject(void);

#if USE_DYNAMIC_CLASSES
  virtual wxClassInfo *GetClassInfo(void) { return &classwxObject; }
  
  virtual istream& LoadObject(istream&);
  virtual ostream& SaveObject(ostream&);

  Bool IsKindOf(wxClassInfo *info);
#endif
#if DEBUG && USE_MEMORY_TRACING
  void * operator new (size_t size, char * fileName = NULL, int lineNum = 0);
  void operator delete (void * buf);
#endif

#if DEBUG || USE_DEBUG_CONTEXT
  virtual void Dump(ostream& str);
#endif

  // Reference counting
  inline int GetRefCount(void) { return refCount; }
  inline void SetRefCount(int ref) { refCount = ref; }
  inline int Ref(void) { refCount ++; return refCount; }
  inline int UnRef(void) { refCount --; return refCount; }
};


#endif // IN_CPROTO
#endif // wx_objh
