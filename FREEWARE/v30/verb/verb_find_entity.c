/* verb_find_entity.c

	This module has the pleasure of figuring out what command
block actually references the given entity block. This is used
for displaying the disallows clause.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib$routines.h>
#include <ssdef.h>

#include "verb.h"

CommandBlock *verb_find_entity(VectorBlock *, CommandBlock*,
                               CommandBlock *, EntityBlock*);

CommandBlock *verb_find_entity_in_cmd(VectorBlock *, CommandBlock*,
                                      EntityBlock*);

CommandBlock *verb_find_entity_in_ent(VectorBlock *, CommandBlock*,
                                      EntityBlock*, EntityBlock*);

CommandBlock *verb_find_entity_in_typ(VectorBlock *, CommandBlock*,
                                      EntityBlock*, TypeBlock*);

#define MAX_ENTITIES 8192

static struct info_block
{
  CommandBlock *command;
  EntityBlock *entity;
} lookup_table[MAX_ENTITIES];
/* static CommandBlock *verb_in_use = 0; */
static int max_entity = 0;

CommandBlock *check_entity_table(EntityBlock *ent)
{
  int i;
  for (i = 0; i < max_entity; ++i)
  {
    if (lookup_table[i].entity == ent) return lookup_table[i].command;
  }
  return 0;
}

add_to_entity_table(CommandBlock *cmd, EntityBlock *ent)
{
    if (max_entity < MAX_ENTITIES)
    {
        lookup_table[max_entity].command = cmd;
        lookup_table[max_entity].entity = ent;
        ++max_entity;
    }
    else
    {
        printf("Internal error: MAX_ENTITIES in %s needs to be larger!\n",
            __FILE__);
        lib$signal(SS$_ABORT);
    }
}

CommandBlock *verb_find_entity(VectorBlock *table, CommandBlock *verb,
                    CommandBlock *cmd, EntityBlock *ent)
{
  CommandBlock *tmp = 0;

/*
**  if (verb_in_use != verb)
**  {
**    verb_in_use = verb;
**    max_entity = 0;
**  }
*/

  tmp = check_entity_table(ent);
  if (tmp) return tmp;

  /* first check to see if 'ent' is found in the 'cmd' block */
  tmp = verb_find_entity_in_cmd(table, cmd, ent);
  if (tmp) return tmp;

  /* then start traversing the whole verb (ouch). */
  tmp = verb_find_entity_in_cmd(table, verb, ent);
  if (tmp) return tmp;

  /* Otherwise, start traversing the whole verb tree (big ouch). */
  {
    CommandTableBlock *cmds;
    VerbTableBlock *verbs;
    int i, verb_count;

    verbs = TRO(table, table->vec_l_verbtbl);
    cmds  = TRO(table, table->vec_l_comdptr);

    verb_count = (verbs->verb_w_size - VERB_K_HEADER_LENGTH) /
                 sizeof(verbs->verb_l_name[0]);

    for (i = 0; i < verb_count; ++i)
    {
      CommandBlock *cmd = TRO(table, cmds->comd_l_cmdtro[i]);

      tmp = verb_find_entity_in_cmd(table, cmd, ent);
      if (tmp) return tmp;
    }
  }

  /* otherwise, we couldn't find the thing (error?) */
  return 0;
}

CommandBlock *verb_find_entity_in_cmd(VectorBlock *table,
                    CommandBlock *cmd, EntityBlock *ent)
{
  CommandBlock *tmp;

  if (cmd->cmd_l_parms)
  {
    tmp = verb_find_entity_in_ent(table, cmd, ent,
                                  TRO(table, cmd->cmd_l_parms));
    if (tmp) return tmp;
  }

  if (cmd->cmd_l_quals)
  {
    tmp = verb_find_entity_in_ent(table, cmd, ent,
                                  TRO(table, cmd->cmd_l_quals));
    if (tmp) return tmp;
  }

  return 0;
}

CommandBlock *verb_find_entity_in_ent(VectorBlock *table, CommandBlock *cmd,
                    EntityBlock *target, EntityBlock *ent)
{
  CommandBlock *tmp, *retval = 0;

  if (check_entity_table(ent))
  {
    return 0; /* already checked this fully */
  }
  else
  {
    add_to_entity_table(cmd, ent);
  }

  if (target == ent)
    retval = cmd; /* is it here? */

  if (ent->ent_l_next)
  {
    tmp = verb_find_entity_in_ent(table, cmd, target,
                                  TRO(table, ent->ent_l_next));
    if (tmp && !retval) retval = tmp;
  }

  if (ent->ent_l_syntax)
  {
    tmp = verb_find_entity_in_cmd(table, TRO(table, ent->ent_l_syntax),
                                  target);
    if (tmp && !retval) retval = tmp;
  }

  if (ent->ent_l_user_type)
  {
    tmp = verb_find_entity_in_typ(table, cmd, target,
                                  TRO(table, ent->ent_l_user_type));
    if (tmp && !retval) retval = tmp;
  }

  return retval;
}


CommandBlock *verb_find_entity_in_typ(VectorBlock *table, CommandBlock *cmd,
                    EntityBlock *target, TypeBlock *typ)
{
  CommandBlock *tmp;

  if (target == (EntityBlock *) typ) return cmd; /* is it here? */

  if (typ->type_l_keywords)
  {
    tmp = verb_find_entity_in_ent(table, cmd, target,
                                  TRO(table, typ->type_l_keywords));
    if (tmp) return tmp;
  }

  return 0;
}
