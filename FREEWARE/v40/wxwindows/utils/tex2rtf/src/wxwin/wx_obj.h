/*
 * File:     wx_obj.h
 * Purpose:  Top level object for wxWindows
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

#ifndef wxb_objh
#define wxb_objh

#include "common.h"

#ifdef IN_CPROTO
typedef       void    *wxObject ;
#else

class wxObject
{
  public:
  WXTYPE __type;
  wxObject(void);
  virtual ~wxObject(void);
};

#endif // IN_CPROTO
#endif // wx_objh
