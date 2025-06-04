/* verb_disallows.c

	This module displays the disallows clause..
*/

#include "verb.h"

extern int check_block();
extern CommandBlock *verb_find_entity();

#define TABLE_SIZE(a) (sizeof((a)) / sizeof((a[0])))

int verb_print_disallows(VectorBlock *table, CommandBlock *verb,
                     CommandBlock *cmd, ExpressionBlock *exp, int *c)
{
  static char *op_type[] = {"illegal", "path", "not", "any2",
                            "and", "or", "xor", "neg"};
  int i, sts;

  sts = check_block(exp, BLOCK_K_EXPRESSION);
  if (!(sts & 1)) return sts;

  switch (exp->exp_b_subtype)
  {
    case EXP_K_PATH:
      if (*c > 60)
      {
        printf("\n");
        *c = printf("      ");
      }
      for (i = 0; i < exp->exp_w_tro_count; ++i)
      {
        EntityBlock *op_ent = TRO(table, exp->exp_l_operand_list[i]);
        ascic *name = BRO(op_ent, op_ent->ent_w_label);
 
        if (op_ent->ent_b_subtype == ENT_K_KEYWORD)
          *c += printf("%s%.*s", ((i == 0)?"":"."), name->length, name->text);
        else
        {
          CommandBlock *tmp = verb_find_entity(table, verb, cmd, op_ent);
          if (tmp != cmd)
          {
            ascic *path = BRO(tmp, tmp->cmd_w_name);
            if (tmp && tmp->cmd_b_subtype == CMD_K_VERB)
              path = (ascic *) ((char *)path + 1);
            *c += printf("<%.*s>", path->length, path->text);
          }
          *c += printf("%s%.*s", ((i == 0)?"":"."), name->length, name->text);
        }
      }
      break;

    case EXP_K_ANY2:
      if (*c > 72)
      {
        printf("\n");
        *c = printf("      ");
      }
      *c += printf("any2 (");
      for (i = 0; i < exp->exp_w_tro_count; ++i)
      {
        ExpressionBlock *op_exp = TRO(table, exp->exp_l_operand_list[i]);

        if (i > 0) *c += printf(", ");
        sts = verb_print_disallows(table, verb, cmd, op_exp, c);
        if (!(sts & 1)) return sts;
      }
      *c += printf(")");
      break;

    case EXP_K_NOT:
    case EXP_K_NEG:
      {
        ExpressionBlock *op_exp = TRO(table, exp->exp_l_operand_list[0]);

        if (exp->exp_w_tro_count != 1)
          printf("Assumption about tro_count == 1 is wrong.\n");
        if (*c > 72)
        {
          printf("\n");
          *c = printf("      ");
        }
        *c += printf("%s ", op_type[exp->exp_b_subtype]);
        sts = verb_print_disallows(table, verb, cmd, op_exp, c);
        if (!(sts & 1)) return sts;
      }
      break;

    case EXP_K_AND:
    case EXP_K_OR:
    case EXP_K_XOR:
      {
        ExpressionBlock *op_exp = TRO(table, exp->exp_l_operand_list[0]);

        if (*c > 72)
        {
          printf("\n");
          *c = printf("      ");
        }
        *c += printf("(");

        sts = verb_print_disallows(table, verb, cmd, op_exp, c);
        if (!(sts & 1)) return sts;

        for (i = 1; i < exp->exp_w_tro_count; ++i)
        {
          op_exp = TRO(table, exp->exp_l_operand_list[i]);

          *c += printf(" %s ", op_type[exp->exp_b_subtype]);

          sts = verb_print_disallows(table, verb, cmd, op_exp, c);
          if (!(sts & 1)) return sts;
        }

        *c += printf(")");
      }
      break;
  }

  return 1;
}
