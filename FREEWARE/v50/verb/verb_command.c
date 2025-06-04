/* verb_command.c

	This module prints a "command block", then evaluates any entity
blocks pointed to by this command block.
*/

#include <ssdef.h>
#include <lib$routines.h>

#include "verb.h"

extern int check_block();
extern int verb_evaluate_entity();
extern int verb_print_entity();
extern int verb_print_disallows();

#define MIN(a,b) (((a) < (b))?(a):(b))
#define ABS(a) (((a) < 0)?(-(a)):(a))

#define MAX_ENTITIES 8192
static CommandBlock *cmd_displayed[MAX_ENTITIES];
static int next_saved_cmd = 0;
/* static int verb_in_use = 0; */

int verb_print_command(VectorBlock *table, CommandBlock *verb, CommandBlock *cmd)
{
    int overall, start, length, first, sts, col, i;
    char *command;

    sts = check_block(cmd, BLOCK_K_COMMAND);
    if (!(sts & 1)) return sts;

    for (i = 0; i < next_saved_cmd; ++i)
        if (cmd_displayed[i] == cmd) return 1; /* we already displayed this! */

    if (next_saved_cmd < MAX_ENTITIES)
    {
        cmd_displayed[next_saved_cmd++] = cmd; /* save for future reference */
    }
    else
    {
        printf("Internal error: MAX_ENTITIES in %s needs to be larger!\n",
            __FILE__);
        lib$signal(SS$_ABORT);
    }

  command = BRO(cmd, cmd->cmd_w_name);
  switch (cmd->cmd_b_subtype)
  {
    case CMD_K_VERB:
      /* the logic here changed -
          Todd Flolo fixed the handling of synonyms in '89, which appears
          to have made it into V5.4 of VMS, however there's no easy way
          for us to tell that the fix has been used on a command verb,
          so we have to try and guess. Since synonyms have to be 31 characters,
          it should be easy to find the beginning and end of an ascic string..
      */
      overall = command[0];
      first   = command[1]; /* length of verb */
      start  = 1;
      while (start < overall)
      {
        length = command[start];
        ++start;
        if (length)
        {
            int truncated = 0;
            /* if pre-fix, then "length" may be truncated to size of verb */
            if (length > first)
            {
                /* verify that there are no count bytes in between (in case it
                   was shortened on us), and that the end isn't past the end of
                   the overall string */
                int i;
                for (i = start; i < start+length; ++i)
                {
                    if ((command[i] < ' ') || i > overall)
                    {
                        length = i - start;
                        truncated = 1;
                    }
                }
            }
            printf("%s %.*s%s\n",
               ((start == 2)?"define verb":"   synonym"),
               length, &command[start],
               truncated?" ! synonym truncated by CDU":"");
        }
        start += length;
      }
      break;
    case CMD_K_SYNTAX:
      printf("define syntax %.*s\n", command[0], &command[1]);
      break;
  }

  /* display the command handler.. */
  command = BRO(cmd, cmd->cmd_w_image);

  col = 0;
  switch (cmd->cmd_b_handler)
  {
    case CMD_K_NONE:
      break;
    case CMD_K_CLI:
      col += printf("   cliroutine %.*s", command[0], &command[1]);
      break;
    case CMD_K_USER:
      col += printf("   routine %.*s", command[4], &command[5]);
      break;
    case CMD_K_IMAGE:
      col += printf("   image %.*s", command[0], &command[1]);
      break;
    case CMD_K_SAME:
      break;
  }

  if (cmd->cmd_w_flags & 0x1f)
  { /* some of the cliflags must be set */
    int first = 0;

    col += printf(", cliflags(");
    if (cmd->cmd_v_abbrev)
      col += printf("%s%s",((first++)?", ":""),"abbreviate");
    if (cmd->cmd_v_nostat)
      col += printf("%s%s",((first++)?", ":""),"nostatus");
    if (cmd->cmd_v_foreign)
      col += printf("%s%s",((first++)?", ":""),"foreign");
    if (cmd->cmd_v_immed)
      col += printf("%s%s",((first++)?", ":""),"immediate");
    if (cmd->cmd_v_mcrparse)
      col += printf("%s%s",((first++)?", ":""),"mcrparse");
    col += printf(")");
  }

  if (col > 0)
  {
    col = 0;
    printf("\n");
  }

  if (cmd->cmd_w_prefix)
  {
    command = BRO(cmd, cmd->cmd_w_prefix);
    printf("   prefix %.*s\n", command[0], &command[1]);
  }

  if (cmd->cmd_v_parms) /* print parameters */
  {
    if (cmd->cmd_l_parms)
    {
      sts = verb_print_entity(table, verb, TRO(table, cmd->cmd_l_parms));
      if (!(sts & 1)) return sts;
    }
    else printf("   noparameters\n");
  }

  if (cmd->cmd_v_quals) /* print qualifiers */
  {
    if (cmd->cmd_l_quals)
    {
      sts = verb_print_entity(table, verb, TRO(table, cmd->cmd_l_quals));
      if (!(sts & 1)) return sts;
    }
    else printf("   noqualifiers\n");
  }

  if (cmd->cmd_v_disallows) /* print disallows */
  { 
    if (cmd->cmd_l_disallow)
    {
      ExpressionBlock *exp;
      int i;

      exp = TRO(table, cmd->cmd_l_disallow);

      for (i = 0; i < exp->exp_w_tro_count; ++i)
      {
        int c = 0;
        c += printf("   disallow ");
        sts = verb_print_disallows(table, verb, cmd,
                                   TRO(table, exp->exp_l_operand_list[i]), &c);
        printf("\n");
        if (!(sts & 1)) return sts;
      }
    }
    else printf("   nodisallows\n");
  }

  /* print outputs list */
  if (cmd->cmd_w_outputs)
  {
    EntityBlock *ent;
    char *outputs, *name;
    int i, j, output_count, first = 0, ent_index;

    outputs = BRO(cmd, cmd->cmd_w_outputs);
    output_count = *outputs;
    ++outputs;

    printf("   outputs(");
    for (i = 0; i < output_count; ++i)
    {
      ent_index = *outputs;
      ++outputs;

      if (ent_index < 0)
        ent = TRO(table, cmd->cmd_l_parms);
      else
        ent = TRO(table, cmd->cmd_l_quals);

      for (j = 0; j < (ABS(ent_index) - 1); ++j)
      {
        ent = TRO(table, ent->ent_l_next);
      }
      name = BRO(ent, ent->ent_w_label);
      printf("%s%.*s", (first++?", ":""), name[0], &name[1]);
    }
    printf(")\n");
  }
  printf("\n"); /* let's have a blank line! */

  /* okay, evaluate all the things we just printed */
  if (cmd->cmd_v_parms && cmd->cmd_l_parms) /* evaluate parameters first */
  {
    sts = verb_evaluate_entity(table, verb, TRO(table, cmd->cmd_l_parms));
    if (!(sts & 1)) return sts;
  }

  if (cmd->cmd_v_quals && cmd->cmd_l_quals) /* evaluate qualifiers next */
  {
    sts = verb_evaluate_entity(table, verb, TRO(table, cmd->cmd_l_quals));
    if (!(sts & 1)) return sts;
  }

  return 1;
}
