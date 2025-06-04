/* verb_type.c

	This module prints type blocks, then prints any entity
blocks referenced by this type block.
*/

#include "verb.h"

extern int cli$dcl_parse();

extern int check_block();
extern int verb_entity_displayed();
extern int verb_print_entity();
extern int verb_evaluate_entity();

int verb_print_type(VectorBlock *table, CommandBlock *verb, TypeBlock *typ)
{
  EntityBlock *ent = TRO(table, typ->type_l_keywords);
  int sts;

  sts = check_block(typ, BLOCK_K_TYPE);
  if (!(sts & 1)) return sts;

  if (verb_entity_displayed(verb, typ)) return 1; /* already displayed */

  if (typ->type_w_name)
  {
    ascic *name = BRO(typ, typ->type_w_name);
    printf("define type %.*s\n", name->length, name->text);
  }
  else return SS$_ABORT;

  if (typ->type_w_prefix)
  {
    ascic *name = BRO(typ, typ->type_w_prefix);
    printf("   prefix %.*s\n", name->length, name->text);
  }

  if (typ->type_l_keywords)
  {
    sts = verb_print_entity(table, verb, ent);
    if (!(sts & 1)) return sts;
    printf("\n");

    sts = verb_evaluate_entity(table, verb, ent);
    if (!(sts & 1)) return sts;
  }
  else
  {
    printf("\n");
  }

  return 1;
}
