/*
 * File:     wx_list.cc
 * Purpose:  List implementation
 *
 *                        wxWindows 1.50
 * Copyright (c) 1993 Artificial Intelligence Applications Institute,
 *                   The University of Edinburgh
 *
 *                     Author: Julian Smart
 *                       Date: 7-9-93
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice, author statement and this permission
 * notice appear in all copies of this software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE ARTIFICIAL INTELLIGENCE APPLICATIONS INSTITUTE OR THE
 * UNIVERSITY OF EDINBURGH BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Uncomment for Borland/UNIX combination
#ifdef wx_msw
#include "wx.h"
#pragma hdrstop
#else

#include "common.h"
#include "wx_list.h"
#include "wx_utils.h"
#endif
*/

#include "wx.h"                         // MSC 7/UNIX
#include <stdarg.h>
#include <stdlib.h>

wxNode::wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
               wxObject *object)
{
  __type = wxTYPE_NODE;
  data = object;
  previous = last_one;
  next = next_one;
  list = the_list;
  key.string = NULL;

  if (previous)
    previous->next = this;

  if (next)
    next->previous = this;
}

// Keyed constructor
wxNode::wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
               wxObject *object, long the_key)
{
  __type = wxTYPE_NODE;
  data = object;
  previous = last_one;
  next = next_one;
  list = the_list;
  key.integer = the_key;

  if (previous)
    previous->next = this;

  if (next)
    next->previous = this;
}

wxNode::wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
               wxObject *object, char *the_key)
{
  __type = wxTYPE_NODE;
  data = object;
  previous = last_one;
  next = next_one;
  list = the_list;
  key.string = copystring(the_key);

  if (previous)
    previous->next = this;

  if (next)
    next->previous = this;
}


wxNode::~wxNode(void)
{
  if (list)
    list->n --;

  if (list && list->destroy_data)
    delete data;

  if (list && list->key_type == wxKEY_STRING && key.string)
    delete[] key.string;

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

wxList::wxList(void)
{
  __type = wxTYPE_LIST;
  first_node = NULL;
  last_node = NULL;
  n = 0;
  destroy_data = 0;
  key_type = wxKEY_NONE;
}

wxList::wxList(int N, wxObject *Objects[])
{
  __type = wxTYPE_LIST;
  wxNode *last = NULL;

  for (int i = 0; i < N; i ++)
  {
    wxNode *next = new wxNode(this, last, NULL, Objects[i]);
    last = next;
    if (i == 0) first_node = next;
  }
  last_node = last;
  n = N;
  key_type = wxKEY_NONE;
}

wxList::wxList(unsigned int the_key_type)
{
  __type = wxTYPE_LIST;
  n = 0;
  destroy_data = 0;
  first_node = NULL;
  last_node = NULL;
  key_type = the_key_type;
}

// Variable argument list, terminated by a zero
wxList::wxList(wxObject *first_one ...)
{
// #ifndef __sgi
  __type = wxTYPE_LIST;
  va_list ap;

  va_start(ap, first_one);

  wxNode *last = new wxNode(this, NULL, NULL, first_one);
  first_node = last;
  n = 1;

  for (;;)
  {
    wxObject *object = va_arg(ap, wxObject *);
//    if (object == NULL) // Doesn't work in Windows -- segment is non-zero for NULL!
    if ((int)object == 0)
      break;
    else
      { wxNode *node = new wxNode(this, last, NULL, object);
        last = node;
        n++;
      }
  }
  last_node = last;
  va_end(ap);

  destroy_data = 0;
  key_type = wxKEY_NONE;
/*
#else
  fprintf(stderr, "Error: cannot use variable-argument functions on SGI!\n");
#endif
*/
}

wxList::~wxList(void)
{
  wxNode *each = first_node;
  while (each)
  {
    wxNode *next = each->Next();
    delete each;
    each = next;
  }
}

wxNode *wxList::Nth(int i)
{
  int j = 0;
  wxNode *current = First();
  while (current && j < i)
  {
    current = current->Next();
    j ++;
  }
  if (j == i)
    return current;
  else
    return NULL;
}

wxNode *wxList::Find(long key)
{
  wxNode *current = First();
  wxNode *found = NULL;
  while (current && !found)
  {
    if (current->key.integer == key)
      found = current;
    else current = current->Next();
  }
  return found;
}

wxNode *wxList::Find(char *key)
{
  wxNode *current = First();
  wxNode *found = NULL;
  while (current && !found)
  {
    if (!current->key.string)
    {
      wxFatalError("wxList: string key not present, probably did not Append correctly!");
      return NULL;
    }
    if (strcmp(current->key.string, key) == 0)
      found = current;
    else current = current->Next();
  }
  return found;
}

wxNode *wxList::Member(wxObject *object)
{
  wxNode *current = First();
  wxNode *found = NULL;
  while (current && !found)
  {
    wxObject *each = current->Data();
    if (each == object)
      found = current;
    else current = current->Next();
  }
  return found;
}

Bool wxList::DeleteNode(wxNode *node)
{
  if (node)
  {
    delete node;
    return TRUE;
  }
  else
    return FALSE;
}

Bool wxList::DeleteObject(wxObject *object)
{
  wxNode *current = first_node;
  while (current)
  {
    if (current->Data() == object)
      break;
    else current = current->Next();
  }
  if (current)
  {
    delete current;
    return TRUE;
  }  else return FALSE;
}


// Insert new node at front of list
wxNode *wxList::Insert(wxObject *object)
{
  wxNode *node = new wxNode(this, NULL, First(), object);
  first_node = node;

  if (!(node->Next()))
    last_node = node;

  n ++;
  return node;
}


// Insert new node before given node.
wxNode *wxList::Insert(wxNode *position, wxObject *object)
{
  wxNode *prev = NULL;
  if (position)
    prev = position->Previous();

  wxNode *node = new wxNode(this, prev, position, object);
  if (!first_node)
  {
    first_node = node;
    last_node = node;
  }
  if (!prev)
    first_node = node;

  n ++;
  return node;
}

// Keyed append
wxNode *wxList::Append(long key, wxObject *object)
{
  wxNode *node = new wxNode(this, last_node, NULL, object, key);
  if (!first_node)
    first_node = node;
  last_node = node;
  n ++;
  return node;
}

wxNode *wxList::Append(char *key, wxObject *object)
{
  wxNode *node = new wxNode(this, last_node, NULL, object, key);
  if (!first_node)
    first_node = node;
  last_node = node;
  n ++;
  return node;
}

void wxList::Clear(void)
{
  wxNode *current = first_node;
  while (current)
  {
    wxNode *next = current->Next();
    delete current;
    current = next;
  }
  first_node = NULL;
  last_node = NULL;
  n = 0;
}

/*
 * String list
 *
 */

wxStringList::wxStringList(void):wxList()
{
  __type = wxTYPE_STRING_LIST;
}

// Variable argument list, terminated by a zero
// Makes new storage for the strings
wxStringList::wxStringList(char *first ...)
{
// #ifndef __sgi
  __type = wxTYPE_STRING_LIST;
  n = 0;
  destroy_data = 0;
  key_type = wxKEY_NONE;
  first_node = NULL;
  last_node = NULL;

  if (!first)
    return;

  va_list ap;

  va_start(ap, first);

  wxNode *last = new wxNode(this, NULL, NULL, (wxObject *)copystring(first));
  first_node = last;
  n = 1;

  for (;;)
  {
    char *s = va_arg(ap, char *);
//    if (s == NULL)
    if ((int)s == 0)
      break;
    else
      { wxNode *node = new wxNode(this, last, NULL, (wxObject *)copystring(s));
        last = node;
        n++;
      }
  }
  last_node = last;
  va_end(ap);
/*
#else
  fprintf(stderr, "Error: cannot use variable-argument functions on SGI!\n");
#endif
*/
}

wxStringList::~wxStringList(void)
{
  wxNode *each = first_node;
  while (each)
  {
    char *s = (char *)each->Data();
    delete[] s;
    wxNode *next = each->Next();
    delete each;
    each = next;
  }
}

wxNode *wxStringList::Add(char *s)
{
  return Append((wxObject *)(copystring(s)));
}

void wxStringList::Delete(char *s)
{
  wxNode *node = First();
  Bool found = FALSE;
  while (node && !found)
  {
    char *string = (char *)node->Data();
    if (strcmp(string, s) == 0)
    {
      found = TRUE;
      delete[] string;
      delete node;
    }
    else node = node->Next();
  }
}

// Only makes new strings if arg is TRUE
char **wxStringList::ListToArray(Bool new_copies)
{
  char **string_array = new char *[Number()];
  wxNode *node = First();
  for (int i = 0; i < n; i++)
  {
    char *s = (char *)node->Data();
    if (new_copies)
      string_array[i] = copystring(s);
    else
      string_array[i] = s;
    node = node->Next();
  }
  return string_array;
}

int wx_comparestrings(const void *arg1, const void *arg2)
{
  char **s1 = (char **)arg1;
  char **s2 = (char **)arg2;

  return strcmp(*s1, *s2);
}

// Sort a list of strings - deallocates old nodes, allocates new
void wxStringList::Sort(void)
{
  int N = n;
  char **array = new char *[N];
  int i = 0;
  wxNode *node = First();
  while (node)
  {
    array[i] = (char *)node->Data();
    i ++;
    node = node->Next();
  }
  qsort(array, N, sizeof(char *), wx_comparestrings);
  Clear();

  for (i = 0; i < N; i++)
    Append((wxObject *)(array[i]));

  delete[] array;
}

// Checks whether s is a member of the list
Bool wxStringList::Member(char *s)
{
  wxNode *node = First();
  while (node)
  {
    char *s1 = (char *)node->Data();
    if (strcmp(s, s1) == 0)
      return TRUE;
    else node = node->Next();
  }
  return FALSE;
}
