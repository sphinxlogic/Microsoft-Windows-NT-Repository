/* init_cli.c

    status = init_cli(table_addr, verb_name)

	This routine allows you to define a command as either a foreign
command (via PROG :== $<wherever>PROG), or as a real command (via the
SET COMMAND command). "table_addr" is the address of the command tables
(you must do a SET COMMAND/OBJECT and link the resulting object file
into your code, then use "globalvalue" to reference the table.
"verb_name" is the name of the verb (a null terminated, uppercase
string).

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <descrip.h>
#include <lib$routines.h>

extern int cli$get_value();
extern int cli$dcl_parse();

int cli_get_value();

/* "table" is a pointer to the internal CLD
 * "name" is a text string containing the name of the verb
 */
int init_cli(char *table, char *name)
{
    static struct dsc$descriptor cmd = {0,DSC$K_DTYPE_T,DSC$K_CLASS_D,0};
    static $DESCRIPTOR(verb,"$VERB");
    static $DESCRIPTOR(line,"$LINE");
    int i,sts;

    sts = cli_get_value(&verb,&cmd);
    if ((sts & 1) && cmd.dsc$w_length)
    {
        if (strncmp(cmd.dsc$a_pointer,name,cmd.dsc$w_length) == 0)
        {
            return(1);    /* the command must have been properly defined! */
        }
    }

    sts = cli_get_value(&line,&cmd);
    if (!(sts & 1))
    {
        printf("Unexpected error in INIT_CLI, error %d in retrieving $LINE\n");
        return(sts);
    }

    /* this code assumes that the verb is shorter than than the foreign
    symbol (which includes device:[dir], so, should be reasonable) */
    for (i = 0; (i < cmd.dsc$w_length) &&
                (cmd.dsc$a_pointer[i] != ' ') &&
                (cmd.dsc$a_pointer[i] != '/'); ++i)
    {
        cmd.dsc$a_pointer[i] = (i < strlen(name))?name[i]:' ';
    }

    if (i < strlen(name))
    {
        printf(
"Unexpected error in INIT_CLI, verb name wasn't smaller than $line prefix\n"
        );
    }

    return(cli$dcl_parse(&cmd,table)); /* initialize the parse tables */
}


int cli_get_value(struct dsc$descriptor *s1, struct dsc$descriptor *s2)
{
  lib$establish(lib$sig_to_ret);
  return cli$get_value(s1, s2);
}
