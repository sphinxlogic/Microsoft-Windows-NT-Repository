/*
 * File:     wx_hash.cc
 * Purpose:  Hash table implementation
 *
 *                       wxWindows 1.50
 * Copyright (c) 1993 Artificial Intelligence Applications Institute,
 *                   The University of Edinburgh
 *
 *                     Author: Julian Smart
 *                       Date: 19-4-93
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

#include "wx_hash.h"

#include <iostream.h>
#include <string.h>
#include <stdarg.h>

wxHashTable::wxHashTable(unsigned int the_key_type, int size)
{
  __type = wxTYPE_HASH_TABLE;
  n = size;
  current_position = -1;
  current_node = NULL;

  key_type = wxKEY_NONE;
  hash_table = new wxList *[size];
  for (int i = 0; i < size; i++)
  {
    hash_table[i] = NULL;
  }
}

wxHashTable::~wxHashTable(void)
{
  for (int i = 0; i < n; i++)
    if (hash_table[i])
      delete hash_table[i];
  delete hash_table;
}

void wxHashTable::Put(long key, long value, wxObject *object)
{
  if (key < 0) key = -key;
  
  long position = key % n;
  if (!hash_table[position])
    hash_table[position] = new wxList(wxKEY_INTEGER);

  hash_table[position]->Append(value, object);
}

void wxHashTable::Put(long key, char *value, wxObject *object)
{
  if (key < 0) key = -key;
  
  long position = key % n;
  if (!hash_table[position])
    hash_table[position] = new wxList(wxKEY_INTEGER);

  hash_table[position]->Append(value, object);
}

void wxHashTable::Put(long key, wxObject *object)
{
  if (key < 0) key = -key;

  long position = key % n;
  if (!hash_table[position])
    hash_table[position] = new wxList(wxKEY_INTEGER);

  hash_table[position]->Append(key, object);
}

void wxHashTable::Put(char *key, wxObject *object)
{
  long int_key = 0;
  int len = strlen(key);
  for (int i = 0; i < len; i++)
    int_key += (long)key[i];

  long position = int_key % n;
  if (!hash_table[position])
    hash_table[position] = new wxList(wxKEY_INTEGER);

  hash_table[position]->Append(key, object);
}

wxObject *wxHashTable::Get(long key, long value)
{
  if (key < 0) key = -key;

  long position = key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(value);
    if (node)
      return node->Data();
    else return NULL;
  }
}

wxObject *wxHashTable::Get(long key, char *value)
{
  if (key < 0) key = -key;

  long position = key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(value);
    if (node)
      return node->Data();
    else return NULL;
  }
}

wxObject *wxHashTable::Get(long key)
{
  if (key < 0) key = -key;

  long position = key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(key);
    if (node)
      return node->Data();
    else return NULL;
  }
}

wxObject *wxHashTable::Get(char *key)
{
  long int_key = 0;
  int len = strlen(key);
  for (int i = 0; i < len; i++)
    int_key += (long)key[i];

  long position = int_key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(key);
    if (node)
      return node->Data();
    else return NULL;
  }
}

wxObject *wxHashTable::Delete(long key)
{
  if (key < 0) key = -key;

  long position = key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(key);
    if (node)
    {
      wxObject *data = node->Data();
      delete node;
      return data;
    }
    else return NULL;
  }
}

wxObject *wxHashTable::Delete(char *key)
{
  long int_key = 0;
  int len = strlen(key);
  for (int i = 0; i < len; i++)
    int_key += (long)key[i];

  long position = int_key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(key);
    if (node)
    {
      wxObject *data = node->Data();
      delete node;
      return data;
    }
    else return NULL;
  }
}

wxObject *wxHashTable::Delete(long key, int value)
{
  if (key < 0) key = -key;
  
  long position = key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(value);
    if (node)
    {
      wxObject *data = node->Data();
      delete node;
      return data;
    }
    else return NULL;
  }
}

wxObject *wxHashTable::Delete(long key, char *value)
{
  if (key < 0) key = -key;
  
  long position = key % n;
  if (!hash_table[position])
    return NULL;
  else
  {
    wxNode *node = hash_table[position]->Find(value);
    if (node)
    {
      wxObject *data = node->Data();
      delete node;
      return data;
    }
    else return NULL;
  }
}

long wxHashTable::MakeKey(char *string)
{
  long int_key = 0;
  int len = strlen(string);
  for (int i = 0; i < len; i++)
    int_key += (long)string[i];

  if (int_key < 0) int_key = -int_key;
  
  return int_key;
}

void wxHashTable::BeginFind(void)
{
  current_position = -1;
  current_node = NULL;
}

wxNode *wxHashTable::Next(void)
{
  wxNode *found = NULL;
  Bool end = FALSE;
  while (!end && !found)
  {
    if (!current_node)
    {
      current_position ++;
      if (current_position >= n)
      {
        current_position = -1;
        current_node = NULL;
        end = TRUE;
      }
      else
      {
        if (hash_table[current_position])
        {
          current_node = hash_table[current_position]->First();
          found = current_node;
        }
      }
    }
    else
    {
      current_node = current_node->Next();
      found = current_node;
    }
  }
  return found;
}

void wxHashTable::DeleteContents(Bool flag)
{
  for (int i = 0; i < n; i++)
  {
    if (hash_table[i])
      hash_table[i]->DeleteContents(flag);
  }
}

void wxHashTable::Clear(void)
{
  for (int i = 0; i < n; i++)
  {
    if (hash_table[i])
      hash_table[i]->Clear();
  }
}


