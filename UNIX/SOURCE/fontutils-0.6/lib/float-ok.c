/* float-ok.c: test if a string is a valid floating-point number.

Copyright (C) 1992 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "config.h"

#include <ctype.h>


/* Actually, we don't worry about a trailing exponent.  */

boolean
float_ok (string str)
{
  boolean found_digit = false;
  
  if (str == NULL)
    return false;
    
  /* Allow leading `-' or `+' sign (but digits must follow).  */
  if (*str == '-' || *str == '+')
    {
      str++;
    }
  
  /* Skip decimal digits.  */
  while (isdigit (*str))
    {
      str++;
      found_digit = true;
    }
  
  /* If a `.' follows, can have more digits.  */
  if (*str == '.')
    {
      str++;
      while (isdigit (*str))
        {
          str++;
          found_digit = true;
	}
    }
  
  return *str == 0 && found_digit;
}

