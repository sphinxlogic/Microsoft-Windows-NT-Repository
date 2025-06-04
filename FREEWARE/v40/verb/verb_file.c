/* verb.c

	This is the main entry point for the VERB utility.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <strdef.h>
#include <secdef.h>
#include <rms.h>
#include <ssdef.h>
#include <starlet.h>

#include "verb.h"

int load_file(string *s, char **address, int count)
{
  static $DESCRIPTOR(dnm, ".EXE");
  struct FAB sec_fab = cc$rms_fab;
  long inadr[2] = {512, 512};
  long retadr[2];
  long status, found;
  VectorBlock search;

  sec_fab.fab$b_fns = PLEN(s);
  sec_fab.fab$l_fna = PPTR(s);
  sec_fab.fab$b_dns = SLEN(dnm);
  sec_fab.fab$l_dna = SPTR(dnm);
  sec_fab.fab$b_fac = FAB$M_GET;
  sec_fab.fab$l_fop = FAB$M_UFO;

  status = sys$open(&sec_fab);
  if (!(status & 1)) return status;

  status = sys$crmpsc(inadr, retadr, 0, SEC$M_EXPREG, 0, 0, 0,
                      sec_fab.fab$l_stv, 0, 0, 0, 0);
  if (!(status & 1)) return status;

  search.vec_w_size = VEC_K_LENGTH;
  search.vec_b_type = BLOCK_K_VECTOR;
  search.vec_b_subtype = VEC_K_DCL;
  search.vec_w_flags = 0;
  search.vec_b_strlvl = VEC_K_STRLVL; /* actually, this is part of 'flags' */
  search.vec_w_tro_count = 2;

  for (*address = (char *)retadr[0]; *address < (char *)retadr[1]; ++(*address))
  {
    if (**address == VEC_K_LENGTH)
    {
      if (memcmp(*address, &search, VEC_K_HEADER_LENGTH) == 0)
      {
        if (--count == 0) return 1;
      }
    }
  }

  return SS$_ABORT;
}
