/*
 * File:      wb_list.cc
 * Purpose:     List implementation
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	August 1994
 * RCS_ID:      $Id: wb_list.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
 * Copyright:   (c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)wb_list.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation "wx_list.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx_defs.h"
#include "wx_list.h"
#include "wx_utils.h"
#endif

// Sun CC compatibility (interference with xview/pkg.h, apparently...)
#if defined(SUN_CC) && defined(wx_xview)
#undef va_start
#undef va_end
#undef va_arg
#undef va_list
#endif

#include <stdarg.h>
#include <stdlib.h>

wxNode::wxNode (wxList * the_list, wxNode * last_one, wxNode * next_one,
	wxObject * object)
{
  WXSET_TYPE(wxNode, wxTYPE_NODE)
  
  data = object;
  previous = last_one;
  next = next_one;
  list = the_list;
  key.string = NULL;
  WXGC_IGNORE(list);

  if (previous)
    previous->next = this;

  if (next)
    next->previous = this;
}

// Keyed constructor
wxNode::wxNode (wxList * the_list, wxNode * last_one, wxNode * next_one,
	wxObject * object, long the_key)
{
  WXSET_TYPE(wxNode, wxTYPE_NODE)
  
  data = object;
  previous = last_one;
  next = next_one;
  list = the_list;
  key.integer = the_key;
  WXGC_IGNORE(list);

  if (previous)
    previous->next = this;

  if (next)
    next->previous = this;
}

wxNode::wxNode (wxList * the_list, wxNode * last_one, wxNode * next_one,
	wxObject * object, const char *the_key)
{
  WXSET_TYPE(wxNode, wxTYPE_NODE)
  
  data = object;
  previous = last_one;
  next = next_one;
  list = the_list;
  key.string = copystring (the_key);
  WXGC_IGNORE(list);

  if (previous)
    previous->next = this;

  if (next)
    next->previous = this;
}


wxNode::~wxNode (void)
{
  if (list)
    list->n--;

  if (list && list->destroy_data)
    delete data;

  if (list && list->key_type == wxKEY_STRING && key.string)
    delete[]key.string;

  // Make next node point back to the previous node from here
  if (next)
    next->previous = previous;
  else if (list)
    // If there's a new end of list (deleting the last one)
    // make sure the list knows about it.
    list->last_node = previous;

  // Make the previous node point to the next node from here
  if (previous)
    previous->next = next;

  // Or if no previous node (start of list), make sure list points at
  // the next node which becomes the first!.
  else if (list)
    list->first_node = next;
}

wxList::wxList (void)
{
  WXSET_TYPE(wxList, wxTYPE_LIST)
  
  first_node = NULL;
  last_node = NULL;
  n = 0;
  destroy_data = 0;
  key_type = wxKEY_NONE;
}

wxList::wxList (int N, wxObject * Objects[])
{
  WXSET_TYPE(wxList, wxTYPE_LIST)
  
  wxNode *last = NULL;

  int i;
  for (i = 0; i < N; i++)
    {
      wxNode *next = new wxNode (this, last, NULL, Objects[i]);
      last = next;
      if (i == 0)
	first_node = next;
    }
  last_node = last;
  n = N;
  key_type = wxKEY_NONE;
}

wxList::wxList (unsigned int the_key_type)
{
  WXSET_TYPE(wxList, wxTYPE_LIST)  
  n = 0;
  destroy_data = 0;
  first_node = NULL;
  last_node = NULL;
  key_type = the_key_type;
}

// Variable argument list, terminated by a zero
wxList::wxList (wxObject * first_one...)
{
// #ifndef __sgi
  WXSET_TYPE(wxList, wxTYPE_LIST)
  
  va_list ap;

  va_start (ap, first_one);

  wxNode *last = new wxNode (this, NULL, NULL, first_one);
  first_node = last;
  n = 1;

  for (;;)
    {
      wxObject *object = va_arg (ap, wxObject *);
//    if (object == NULL) // Doesn't work in Windows -- segment is non-zero for NULL!
#ifdef wx_msw
      if ((int) object == 0)
#else
      if ((long) object == 0)
#endif
	break;
      else
	{
	  wxNode *node = new wxNode (this, last, NULL, object);
	  last = node;
	  n++;
	}
    }
  last_node = last;
  va_end (ap);

  destroy_data = 0;
  key_type = wxKEY_NONE;
/*
#else
  fprintf (stderr, "Error: cannot use variable-argument functions on SGI!\n");
#endif
*/
}

wxList::~wxList (void)
{
  wxNode *each = first_node;
  while (each)
    {
      wxNode *next = each->Next ();
      delete each;
      each = next;
    }
}

wxNode *wxList::Nth (int i)
{
  int j = 0;
  for (wxNode * current = First (); current; current = current->Next ())
    {
      if (j++ == i)
	return current;
    }
  return NULL;			// No such element

}

wxNode *wxList::Find (long key)
{
  wxNode *current = First();
  while (current)
  {
    if (current->key.integer == key)
      return current;
    current = current->Next();
  }
    
  return NULL;			// Not found!
}

wxNode *wxList::Find (const char *key)
{
  wxNode *current = First();
  while (current)
  {
      if (!current->key.string)
	{
	  wxFatalError ("wxList: string key not present, probably did not Append correctly!");
	  break;
	}
      if (strcmp (current->key.string, key) == 0)
	return current;
      current = current->Next();
  }

  return NULL;			// Not found!

}

wxNode *wxList::Member (wxObject * object)
{
  for (wxNode * current = First (); current; current = current->Next ())
    {
      wxObject *each = current->Data ();
      if (each == object)
	return current;
    }
  return NULL;
}

Bool wxList::DeleteNode (wxNode * node)
{
  if (node)
    {
      delete node;
      return TRUE;
    }
  return FALSE;
}

Bool wxList::DeleteObject (wxObject * object)
{
  // Search list for object
  for (wxNode * current = first_node; current; current = current->Next ())
    {
      if (current->Data () == object)
	{
	  delete current;
	  return TRUE;
	}
    }
  return FALSE;			// Did not find the object

}


// Insert new node at front of list
wxNode *wxList::Insert (wxObject * object)
{
  wxNode *node = new wxNode (this, NULL, First (), object);
  first_node = node;

  if (!(node->Next ()))
    last_node = node;

  n++;
  return node;
}


// Insert new node before given node.
wxNode *wxList::Insert (wxNode * position, wxObject * object)
{
  wxNode *prev = NULL;
  if (position)
    prev = position->Previous ();

  wxNode *node = new wxNode (this, prev, position, object);
  if (!first_node)
    {
      first_node = node;
      last_node = node;
    }
  if (!prev)
    first_node = node;

  n++;
  return node;
}

// Keyed append
wxNode *wxList::Append (long key, wxObject * object)
{
  wxNode *node = new wxNode (this, last_node, NULL, object, key);
  if (!first_node)
    first_node = node;
  last_node = node;
  n++;
  return node;
}

wxNode *wxList::Append (const char *key, wxObject * object)
{
  wxNode *node = new wxNode (this, last_node, NULL, object, key);
  if (!first_node)
    first_node = node;
  last_node = node;
  n++;
  return node;
}

void wxList::Clear (void)
{
  wxNode *current = first_node;
  while (current)
    {
      wxNode *next = current->Next ();
      delete current;
      current = next;
    }
  first_node = NULL;
  last_node = NULL;
  n = 0;
}

//Executes function F with all items present in the list
void wxList::ForEach(wxListIterateFunction F)
{
  wxNode *each = first_node;
  while (each)
    { (*F)( each->Data ());
      each = each->Next();
    }
}
// Returns a pointer to the item which returns TRUE with function F
// or NULL if no such item found
wxObject *wxList::FirstThat(wxListIterateFunction F)
{
  wxNode *each = first_node;
  while (each)
    { if ((*F)( each->Data ())) return each->Data();
      each = each->Next();
    }
  return NULL;
}
// Like FirstThat, but proceeds from the end backward
wxObject *wxList::LastThat(wxListIterateFunction F)
{
  wxNode *each = last_node;
  while (each)
    { if ((*F)( each->Data ())) return each->Data();
      each = each->Previous();
    }
  return NULL;
}

// (stefan.hammes@urz.uni-heidelberg.de)
//
// function for sorting lists. the concept is borrowed from 'qsort'.
// by giving a sort function, arbitrary lists can be sorted.
// method:
// - put wxObject pointers into an array
// - sort the array with qsort
// - put back the sorted wxObject pointers into the list
//
// CAVE: the sort function receives pointers to wxObject pointers (wxObject **),
//       so dereference right!
// EXAMPLE:
//   int listcompare(const void *arg1, const void *arg2)
//   {
//      return(compare(**(wxString **)arg1,
//                     **(wxString **)arg2));
//   }
//
//   void main()
//   { 
//     wxList list;
//
//     list.Append(new wxString("DEF"));
//     list.Append(new wxString("GHI"));
//     list.Append(new wxString("ABC"));
//     list.Sort(listcompare);
//   }

void wxList::Sort(wxSortCompareFunction compfunc)
{
  // allocate an array for the wxObject pointers of the list
  const size_t num = Number();
  wxObject **objArray = new wxObject *[num];
  wxObject **objPtr = objArray;
  
  // go through the list and put the pointers into the array
  wxNode *node = First();
  while(node!=NULL){
    *objPtr++ = node->Data();
    node = node->Next();
  }
  // sort the array
  qsort((void *)objArray,num,sizeof(wxObject *),compfunc);
  // put the sorted pointers back into the list  
  objPtr = objArray;
  node = First();
  while(node!=NULL){
    node->SetData(*objPtr++);
    node = node->Next();
  }
  // free the array
  delete[] objArray;
}

/*
 * String list
 *
 */

wxStringList::wxStringList (void):
wxList ()
{
  WXSET_TYPE(wxStringList, wxTYPE_STRING_LIST)
}

// Variable argument list, terminated by a zero
// Makes new storage for the strings
wxStringList::wxStringList (char *first...)
{
// #ifndef __sgi
  WXSET_TYPE(wxStringList, wxTYPE_STRING_LIST)
  
  n = 0;
  destroy_data = 0;
  key_type = wxKEY_NONE;
  first_node = NULL;
  last_node = NULL;

  if (!first)
    return;

  va_list ap;

  va_start (ap, first);

  wxNode *last = new wxNode (this, NULL, NULL, (wxObject *) copystring (first));
  first_node = last;
  n = 1;

  for (;;)
    {
      char *s = va_arg (ap, char *);
//    if (s == NULL)
#ifdef wx_msw
      if ((int) s == 0)
#else
      if ((long) s == 0)
#endif
	break;
      else
	{
	  wxNode *node = new wxNode (this, last, NULL, (wxObject *) copystring (s));
	  last = node;
	  n++;
	}
    }
  last_node = last;
  va_end (ap);
/*
#else
  fprintf (stderr, "Error: cannot use variable-argument functions on SGI!\n");
#endif
*/
}

wxStringList::~wxStringList (void)
{
  wxNode *each = first_node;
  while (each)
    {
      char *s = (char *) each->Data ();
      delete[]s;
      wxNode *next = each->Next ();
      delete each;
      each = next;
    }
}

wxNode *wxStringList::Add (const char *s)
{
  return Append ((wxObject *) (copystring (s)));
}

void wxStringList::Delete (const char *s)
{
  for (wxNode * node = First (); node; node = node->Next ())
    {
      char *string = (char *) node->Data ();
      if (string == s || strcmp (string, s) == 0)
	{
	  delete[]string;
	  delete node;
	  break;		// Done!

	}
    }				// for

}

// Only makes new strings if arg is TRUE
char **wxStringList::ListToArray (Bool new_copies)
{
  char **string_array = new char *[Number ()];
  wxNode *node = First ();
  int i;
  for (i = 0; i < n; i++)
    {
      char *s = (char *) node->Data ();
      if (new_copies)
	string_array[i] = copystring (s);
      else
	string_array[i] = s;
      node = node->Next ();
    }
  return string_array;
}

static int 
wx_comparestrings (const void *arg1, const void *arg2)
{
  char **s1 = (char **) arg1;
  char **s2 = (char **) arg2;

  return strcmp (*s1, *s2);
}

// Sort a list of strings - deallocates old nodes, allocates new
void wxStringList::Sort (void)
{
  size_t N = n;
  char **array = new char *[N];

  size_t i = 0;
  for (wxNode * node = First (); node; node = node->Next ())
    array[i++] = (char *) node->Data ();

  qsort (array, N, sizeof (char *), wx_comparestrings);
  Clear ();

  for (i = 0; i < N; i++)
    Append ((wxObject *) (array[i]));

  delete[]array;
}

// Checks whether s is a member of the list
Bool wxStringList::Member (const char *s)
{
  for (wxNode * node = First (); node; node = node->Next ())
    {
      const char *s1 = (const char *) node->Data ();
      if (s == s1 || strcmp (s, s1) == 0)
	return TRUE;
    }
  return FALSE;
}
