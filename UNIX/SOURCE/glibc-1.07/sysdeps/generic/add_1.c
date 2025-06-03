#define __mpn_add_1 __noname
#include "gmp.h"
#undef __mpn_add_1

#include "gmp-impl.h"
#include "longlong.h"

mp_limb
__mpn_add_1 (res_ptr, s1_ptr, s1_size, s2_limb)
     register mp_ptr res_ptr;
     register mp_srcptr s1_ptr;
     register mp_size_t s1_size;
     register mp_limb s2_limb;
{
  register mp_limb x;

  x = *s1_ptr++;
  s2_limb = x + s2_limb;
  *res_ptr++ = s2_limb;
  if (s2_limb < x)
    {
      while (--s1_size != 0)
	{
	  x = *s1_ptr++ + 1;
	  *res_ptr++ = x;
	  if (x != 0)
	    goto fin;
	}

      return 1;
    }

 fin:
  if (res_ptr != s1_ptr)
    memcpy (res_ptr, s1_ptr, (s1_size - 1) * sizeof (mp_limb));
  return 0;
}
