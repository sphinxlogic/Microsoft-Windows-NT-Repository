// -*- C++ -*-
/* Copyright (C) 1989, 1990, 1991, 1992 Free Software Foundation, Inc.
     Written by James Clark (jjc@jclark.com)

This file is part of groff.

groff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

groff is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with groff; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

class macro;

class charinfo {
  static int next_index;
  charinfo *translation;
  int index;
  int number;
  macro *mac;
  unsigned char special_translation;
  unsigned char hyphenation_code;
  unsigned char flags;
  unsigned char ascii_code;
  char not_found;
  char transparent_translate;	// non-zero means translation applies to
				// to transparent throughput
public:
  enum { 
    ENDS_SENTENCE = 1,
    BREAK_BEFORE = 2,
    BREAK_AFTER = 4,
    OVERLAPS_HORIZONTALLY = 8,
    OVERLAPS_VERTICALLY = 16,
    TRANSPARENT = 32,
    NUMBERED = 64
    };
  enum {
    TRANSLATE_NONE,
    TRANSLATE_SPACE,
    TRANSLATE_DUMMY,
    TRANSLATE_STRETCHABLE_SPACE,
    TRANSLATE_HYPHEN_INDICATOR
  };
  symbol nm;
  charinfo(symbol s);
  int get_index();
  int ends_sentence();
  int overlaps_vertically();
  int overlaps_horizontally();
  int can_break_before();
  int can_break_after();
  int transparent();
  unsigned char get_hyphenation_code();
  unsigned char get_ascii_code();
  void set_hyphenation_code(unsigned char);
  void set_ascii_code(unsigned char);
  charinfo *get_translation(int = 0);
  void set_translation(charinfo *, int);
  void set_flags(unsigned char);
  void set_special_translation(int, int);
  int get_special_translation(int = 0);
  macro *set_macro(macro *);
  macro *get_macro();
  int first_time_not_found();
  void set_number(int);
  int get_number();
  int numbered();
};

charinfo *get_charinfo(symbol);
extern charinfo *charset_table[];
charinfo *get_charinfo_by_number(int);

inline int charinfo::overlaps_horizontally()
{
  return flags & OVERLAPS_HORIZONTALLY;
}

inline int charinfo::overlaps_vertically()
{
  return flags & OVERLAPS_VERTICALLY;
}

inline int charinfo::can_break_before()
{
  return flags & BREAK_BEFORE;
}

inline int charinfo::can_break_after()
{
  return flags & BREAK_AFTER;
}

inline int charinfo::ends_sentence()
{
  return flags & ENDS_SENTENCE;
}

inline int charinfo::transparent()
{
  return flags & TRANSPARENT;
}

inline int charinfo::numbered()
{
  return flags & NUMBERED;
}

inline charinfo *charinfo::get_translation(int transparent_throughput)
{
  return (transparent_throughput && !transparent_translate
	  ? 0
	  : translation);
}

inline unsigned char charinfo::get_hyphenation_code()
{
  return hyphenation_code;
}

inline unsigned char charinfo::get_ascii_code()
{
  return ascii_code;
}

inline void charinfo::set_flags(unsigned char c)
{
  flags = c;
}

inline int charinfo::get_index()
{
  return index;
}

inline int charinfo::get_special_translation(int transparent_throughput)
{
  return (transparent_throughput && !transparent_translate
	  ? int(TRANSLATE_NONE)
	  : special_translation);
}

inline macro *charinfo::get_macro()
{
  return mac;
}

inline int charinfo::first_time_not_found()
{
  if (not_found)
    return 0;
  else {
    not_found = 1;
    return 1;
  }
}
