/*
 * File:     wx_list.h
 * Purpose:  wxList implementation much used in wxWindows
 *
 *                       wxWindows 1.50
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

#ifndef wxb_listh
#define wxb_listh

#include "wx_obj.h"

#ifdef IN_CPROTO
typedef       void    *wxList ;
typedef       void    *wxNode;
typedef       void    *wxStringList;
#else

class wxList;

#define wxKEY_NONE    0
#define wxKEY_INTEGER 1
#define wxKEY_STRING  2
class wxNode: public wxObject
{
  wxObject *data;
  wxNode *next;
  wxNode *previous;

 public:
  wxList *list;

  // Optional key stuff
  union
  {
    long integer;
    char *string;
  } key;

  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one, wxObject *object);
  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
         wxObject *object, long the_key);
  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
         wxObject *object, char *the_key);
  ~wxNode(void);

  inline wxNode *Next(void) { return next; }
  inline wxNode *Previous(void) { return previous; }
  inline wxObject *Data(void) { return data; }
  inline void SetData(wxObject *the_data) { data = the_data; }
};

class wxList: public wxObject
{
 public:
  int n;
  int destroy_data;
  wxNode *first_node;
  wxNode *last_node;
  unsigned int key_type;

  wxList(void);
  wxList(unsigned int the_key_type);
  wxList(int N, wxObject *Objects[]);
  wxList(wxObject *object, ...);
  ~wxList(void);

  inline int Number(void) { return n; }

  // Append to end of list
  inline wxNode *Append(wxObject *object)
  {
    wxNode *node = new wxNode(this, last_node, NULL, object);
    if (!first_node)
      first_node = node;
    last_node = node;
    n ++;
    return node;
  }

  // Insert at front of list
  wxNode *Insert(wxObject *object);

  // Insert before given node
  wxNode *Insert(wxNode *position, wxObject *object);

  // Keyed append
  wxNode *Append(long key, wxObject *object);
  wxNode *Append(char *key, wxObject *object);

  Bool DeleteNode(wxNode *node);
  Bool DeleteObject(wxObject *object);  // Finds object pointer and
                                        // deletes node (and object if
                                        // DeleteContents is on)
  void Clear(void);                     // Delete all nodes

  inline wxNode *First(void) { return first_node; }
  inline wxNode *Last(void) { return last_node; }
  wxNode *Nth(int i);                  // nth node counting from 0

  // Keyed search
  wxNode *Find(long key);
  wxNode *Find(char *key);

  wxNode *Member(wxObject *object);

  inline void DeleteContents(int destroy) { destroy_data = destroy; }
                                             // Instruct it to destroy user data
                                             // when deleting nodes
};

// String list class. N.B. this always copies strings
// with Add and deletes them itself.
class wxStringList: public wxList
{
 public:
  wxStringList(void);
  wxStringList(char *first ...);
  ~wxStringList(void);

  virtual wxNode *Add(char *s);
  virtual void Delete(char *s);
  virtual char **ListToArray(Bool new_copies = FALSE);
  virtual void Sort(void);
  virtual Bool Member(char *s);
};

#endif // IN_CPROTO
#endif // wxb_listh
