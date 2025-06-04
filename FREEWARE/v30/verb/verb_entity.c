/* verb_entity.c

	This module evaluates and prints entity blocks.

	Modified 29-JUL-1994 23:33 by Hunter Goatley <goathunter@WKU.EDU>
		Included $old_file fix for V6.1 from John Osudar.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ssdef.h>
#include <lib$routines.h>

#include "verb.h"

int ascic_not_equal();

extern int check_block();
extern int verb_print_type();
extern int verb_print_command();

#define MAX_ENTITIES 8192
static void *displayed_ent[MAX_ENTITIES]; /* points at both EntityBlock & TypeBlock */
static int next_saved_entity = 0;
static int verb_in_use = 0;

int verb_entity_displayed(CommandBlock *verb, EntityBlock *ent)
{
    int i;

    for (i = 0; i < next_saved_entity; ++i)
    {
        if (displayed_ent[i] == ent) return 1;
    }

    if (next_saved_entity < MAX_ENTITIES)
    {
        displayed_ent[next_saved_entity++] = ent;
    }
    else
    {
        printf("Internal error: MAX_ENTITIES in %s needs to be larger!\n",
            __FILE__);
        lib$signal(SS$_ABORT);
    }

    return 0;
}

verb_evaluate_entity(VectorBlock *table, CommandBlock *verb, EntityBlock *ent)
{
  int sts;

  sts = check_block(ent, BLOCK_K_ENTITY);	/* sanity check */
  if (!(sts & 1)) return sts;

  while (ent)
  {
    /* see if there are user defined types to display */
    if (ent->ent_l_user_type)
    {
      sts = verb_print_type(table, verb, TRO(table, ent->ent_l_user_type));
      if (!(sts & 1)) return sts;
    }

    if (ent->ent_l_syntax)
    {
      sts = verb_print_command(table, verb, TRO(table, ent->ent_l_syntax));
      if (!(sts & 1)) return sts;
    }

    ent = TRO(table, ent->ent_l_next);
  }

  return 1;
}

verb_print_entity(VectorBlock *table, CommandBlock *verb, EntityBlock *ent)
{
  int sts;

  sts = check_block(ent, BLOCK_K_ENTITY);
  if (!(sts & 1)) return sts;

  if (verb_entity_displayed(verb, ent)) return 1;

  while (ent)
  {
    if (ent->ent_w_name)
    {
      static char *types[] = {"","parameter","qualifier","keyword"};
      ascic *name = BRO(ent, ent->ent_w_name);

      printf("   %s %.*s", types[ent->ent_b_subtype],
             name->length, name->text);
    }

    if (ent->ent_l_syntax)
    {
      CommandBlock *syntax = TRO(table, ent->ent_l_syntax);
      ascic *name = BRO(syntax, syntax->cmd_w_name);

      printf(", syntax=%.*s", name->length, name->text);
    }

    if (ent->ent_w_label)
    {
      ascic *name = BRO(ent, ent->ent_w_label);

      if (ascic_not_equal(BRO(ent,ent->ent_w_name), name))
        printf(", label=%.*s", name->length, name->text);
    }

    if (ent->ent_w_prompt && (ent->ent_w_prompt != ent->ent_w_label))
    {
      ascic *name = BRO(ent, ent->ent_w_prompt);
 
      if (ascic_not_equal(BRO(ent, ent->ent_w_label), name))
        printf(", prompt=\"%.*s\"", name->length, name->text);
    }

    switch (ent->ent_b_subtype)
    {
      case ENT_K_QUALIFIER:
        if (!ent->ent_v_neg) printf(", nonnegatable");
        break;
      case ENT_K_PARAMETER: /* negatable doesn't apply to parameters */
        break;
      case ENT_K_KEYWORD:
        if (ent->ent_v_neg) printf(", negatable");
        break;
    }

    if (ent->ent_v_deftrue) printf(", default");

    if (ent->ent_v_batdef) printf(", batch");

    printf("\n");

    if (ent->ent_v_val)
    {
      int first = 0;

      printf("      value");

      if (ent->ent_v_valreq) printf("%srequired",(first++?",":" ("));

      if (ent->ent_v_list)
      {
        printf("%slist",(first++?",":" ("));
        if (!ent->ent_v_concat) printf("%snoconcatenate",(first++?",":" ("));
      }
      else
      { /* well, this should never happen, but we'll put it here anyway.. */
        if (ent->ent_v_concat) printf("%sconcatenate",(first++?",":" ("));
      }
      if (ent->ent_v_impcat) printf("%simpcat",(first++?",":" ("));
 
      if (ent->ent_w_defval)
      {
        ascic *overall = BRO(ent, ent->ent_w_defval), *defval;
        int comma = 0;
  
        printf("%sdefault=\"",(first++?",":" ("));

	defval = (ascic *)(overall->text);
        overall = (ascic *) ((char *) overall + overall->length);
        while (defval)
        {
          printf("%s%.*s", (comma++ ? "," : ""), defval->length, defval->text);
          defval = (ascic *) ((char *) defval + defval->length);
          if (defval == overall) defval = NULL;
        }

        printf("\"");
      }

      if (ent->ent_b_valtype)
      {
        static char *types[] = {"",
          "$infile", "$outfile", "$number", "$privilege", "$datetime",
          "$protection", "$process", "$inlog", "$outlog", "$insym",
          "$outsym", "$node", "$device", "$directory", "$uic",
          "$rest_of_line", "$parenthesized_value", "$deltatime",
          "$quoted_string", "$file", "$expression", "$$test1",
          "$$test2", "$$test3", "$acl", "$old_file"};
        printf("%stype=%s",(first++?",":" ("),types[ent->ent_b_valtype]);
      }
      else
      {
        if (ent->ent_l_user_type)
        {
          TypeBlock *typ = TRO(table, ent->ent_l_user_type);
          ascic *name = BRO(typ, typ->type_w_name);

          printf("%stype=%.*s",(first++?",":" ("), name->length, name->text);
        }
      }
      printf("%s\n",first?")":"");
    }
    
    /* print cliflags .. */
    if (ent->ent_v_mcroptdelim || ent->ent_v_mcrignore)
    {
      int first = 0;
 
      printf("      cliflags(");
      if (ent->ent_v_mcroptdelim)
        printf("%s%s",(first++?", ":""), "mcroptdelim");
      if (ent->ent_v_mcrignore)
        printf("%s%s",(first++?", ":""), "mcrignore");
      printf(")\n");
    }

    if (ent->ent_v_parm)
    {
      printf("      placement=");
      if (ent->ent_v_verb) printf("positional\n");
      else printf("local\n");
    }

    ent = TRO(table, ent->ent_l_next);
  }

  return 1;
}

int ascic_not_equal(ascic *s1, ascic *s2)
{
  if (s1->length == s2->length)
    return strncmp(s1->text, s2->text, s1->length);
  return 1;
}
