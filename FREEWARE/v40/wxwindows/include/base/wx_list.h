/*
 * File:	wx_list.h
 * Purpose:	wxList implementation much used in wxWindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_list.h	1.2 5/9/94" */

#ifndef wxb_listh
#define wxb_listh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
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
class WXDLLEXPORT wxNode: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxNode)
 private:
 
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

  wxNode(wxList *the_list = NULL, wxNode *last_one = NULL, wxNode *next_one = NULL, wxObject *object = NULL);
  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
         wxObject *object, long the_key);
  wxNode(wxList *the_list, wxNode *last_one, wxNode *next_one,
         wxObject *object, const char *the_key);
  ~wxNode(void);

  inline wxNode *Next(void) { return next; }
  inline wxNode *Previous(void) { return previous; }
  inline wxObject *Data(void) { return data; }
  inline void SetData(wxObject *the_data) { data = the_data; }
};

// type of compare function for list sort operation (as in 'qsort')
typedef int (*wxSortCompareFunction)(const void *elem1, const void *elem2);
typedef int (*wxListIterateFunction)(wxObject *o);

class WXDLLEXPORT wxList: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxList)

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
  wxNode *Append(const char *key, wxObject *object);

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
  wxNode *Find(const char *key);

  wxNode *Member(wxObject *object);

  inline void DeleteContents(int destroy) { destroy_data = destroy; }
                                             // Instruct it to destroy user data
                                             // when deleting nodes
  // this function allows the sorting of arbitrary lists by giving
  // a function to compare two list elements.
  void Sort(wxSortCompareFunction compfunc);

  wxObject *FirstThat(wxListIterateFunction func);
  void ForEach(wxListIterateFunction func);
  wxObject *LastThat(wxListIterateFunction func);
};

// String list class. N.B. this always copies strings
// with Add and deletes them itself.
class WXDLLEXPORT wxStringList: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxStringList)

 public:
  wxStringList(void);
  wxStringList(char *first ...);
  ~wxStringList(void);

  virtual wxNode *Add(const char *s);
  virtual void Delete(const char *s);
  virtual char **ListToArray(Bool new_copies = FALSE);
  virtual void Sort(void);
  virtual Bool Member(const char *s);
};

#endif // IN_CPROTO
#endif // wxb_listh
