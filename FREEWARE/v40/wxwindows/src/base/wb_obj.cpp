/*
 * File:	wb_obj.cc
 * Purpose:	wxObject base class implementation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_obj.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_obj.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation "wx_obj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx_hash.h"

#ifndef WX_PRECOMP
#include "wx_obj.h"
#include "wx_types.h"
#endif

#include "string.h"

#if (DEBUG && USE_MEMORY_TRACING) || USE_DEBUG_CONTEXT
#include "wx_mem.h"
#endif

/*
 * wxWindows root object.
 */

wxObject::wxObject(void)
{
  WXSET_TYPE(wxObject, wxTYPE_ANY)

  refCount = 0;
  // The most specific class, if this has been set correctly
  // in the constructor.
//  leafClassInfo = NULL;
}

wxObject::~wxObject(void)
{
}

#if USE_DYNAMIC_CLASSES

/*
 * Is this object a kind of (a subclass of) 'info'?
 * E.g. is wxWindow a kind of wxObject?
 * Go from this class to superclass, taking into account
 * two possible base classes.
 */
 
Bool wxObject::IsKindOf(wxClassInfo *info)
{
  wxClassInfo *thisInfo = GetClassInfo();
  if (thisInfo)
    return thisInfo->IsKindOf(info);
  else
    return FALSE;
}

istream& wxObject::LoadObject(istream& str)
{
  return str;
}

ostream& wxObject::SaveObject(ostream& str)
{
  return str;
}

#if DEBUG || USE_DEBUG_CONTEXT
void wxObject::Dump(ostream& str)
{
  if (GetClassInfo() && GetClassInfo()->GetClassName())
    str << GetClassInfo()->GetClassName();
  else
    str << "unknown object class";
}
#endif

#if DEBUG && USE_MEMORY_TRACING
void * wxObject::operator new (size_t size, char * fileName, int lineNum)
{
  return wxDebugAlloc(size, fileName, lineNum, TRUE);
}

void wxObject::operator delete (void * buf)
{
  wxDebugFree(buf);
}

#endif

/*
 * Class info: provides run-time class type information.
 */
 
wxClassInfo::wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn constr)
{
  className = cName;
  baseClassName1 = baseName1;
  baseClassName2 = baseName2;

  objectSize = sz;
  objectConstructor = constr;
  
  next = first;
  first = this;

  baseInfo1 = NULL;
  baseInfo2 = NULL;
}

wxObject *wxClassInfo::CreateObject(void)
{
  if (objectConstructor)
    return (wxObject *)(*objectConstructor)();
  else
    return NULL;
}

wxClassInfo *wxClassInfo::FindClass(char *c)
{
  wxClassInfo *p = first;
  while (p)
  {
    if (p && p->GetClassName() && strcmp(p->GetClassName(), c) == 0)
      return p;
    p = p->next;
  }
  return NULL;
}

// Climb upwards through inheritance hierarchy.
// Dual inheritance is catered for.
Bool wxClassInfo::IsKindOf(wxClassInfo *info)
{
  if (info == NULL)
    return FALSE;

  if (this == info)
    return TRUE;

  if (baseInfo1)
    if (baseInfo1->IsKindOf(info))
      return TRUE;

  if (baseInfo2)
    return baseInfo2->IsKindOf(info);

  return FALSE;
}

// Set pointers to base class(es) to speed up IsKindOf
void wxClassInfo::InitializeClasses(void)
{
  wxHashTable table(wxKEY_STRING);

  // Index all class infos by their class name
  wxClassInfo *info = first;
  while (info)
  {
    if (info->className)
      table.Put(info->className, (wxObject *)info);
    info = info->next;
  }

  // Set base pointers for each wxClassInfo
  info = first;
  while (info)
  {
    if (info->GetBaseClassName1())
      info->baseInfo1 = (wxClassInfo *)table.Get(info->GetBaseClassName1());
    if (info->GetBaseClassName2())
      info->baseInfo2 = (wxClassInfo *)table.Get(info->GetBaseClassName2());
    info = info->next;
  }
}

/* Can't clean up, because wxClassInfo objects are declared static in classes.
 * 
void wxClassInfo::CleanUp(void)
{
  while (first)
  {
    wxClassInfo *thisInfo = first;
    first = first->next;
    delete thisInfo;
  }
}
*/

wxObject *wxCreateDynamicObject(char *name)
{
  wxClassInfo *info = wxClassInfo::first;
  while (info)
  {
    if (info->className && strcmp(info->className, name) == 0)
      return info->CreateObject();
    info = info->next;
  }
  return NULL;
}

#endif
  // End USE_DYNAMIC_CLASSES
