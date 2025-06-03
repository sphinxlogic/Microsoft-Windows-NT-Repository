/*
 * Author: Jason Baietto, jason@ssd.csd.harris.com
 * xdiary Copyright 1990 Harris Corporation
 *
 * Permission to use, copy, modify, and distribute, this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holder be used in
 * advertising or publicity pertaining to distribution of the software with
 * specific, written prior permission, and that no fee is charged for further
 * distribution of this software, or any modifications thereof.  The copyright
 * holder makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, AND IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM ITS USE,
 * LOSS OF DATA, PROFITS, QPA OR GPA, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _StringTableConverter_h
#define _StringTableConverter_h
/* 
   StringTableConverter --  Convert a comma separated list of quote
                            enclosed strings into a StringTable resource.

   example:  xdiary*dayNames: "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"

   A StringTable is an array of pointers to strings whose last entry is NULL.
   If no strings are specified, the initial entry is NULL.  Double quotes 
   delimit the string, but two consecutive quotes become a single double
   quote in the final string.  For example, """Wow!""" becomes "Wow!".

   int StringTableNumber(StringTable) -- Returns the number of strings in
                                         a specified string table.

   void StringTableDump(StringTable) -- Dumps the contents of a string table.
                                        Useful for debugging purposes only.

   StringTable StringTableCopy(StringTable) -- Allocates a new array, but uses
                                               the same string pointers.
                                               Be careful with this one.

   Author: Jason Baietto
   Date:   9/20/90
*/

typedef char ** StringTable;

extern void StringTableConverter();
extern int StringTableNumber();
extern void StringTableDump();
extern StringTable StringTableCopy();

#ifndef XtRStringTable
#define XtRStringTable "StringTable"
#endif

#endif /* _StringTableConverter_h */
