/* apitest - test qi API */

#include <stdio.h>
#include <string.h>
#include <descrip.h>
#include <lib$routines.h>

/* result array descriptor */
typedef struct {
    struct dsc$descriptor_a a;
    struct {
        struct dsc$descriptor_d *dsc$a_a0;
        long dsc$l_m;
    } m;
    struct {
        long dsc$l_l;
        long dsc$l_u;
    } b;
} array;

extern qi_command(void **, struct dsc$descriptor_s *, array **);

main()
{
    char *cp, cmd[100];
    void *ctx = NULL;         /* init ctx to null */
    array *result = NULL;     /* init result to null */
    int ind, status;
    struct dsc$descriptor_s cmd_dsc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    for(;;) {
        printf("Enter qi command: ");
        fgets(cmd, sizeof(cmd), stdin);
        /* command may not contain newlines */
        if (cp = strchr(cmd, '\n'))
            *cp = '\0';

        /* build a command descriptor */
        cmd_dsc.dsc$w_length = (short) strlen(cmd);
        cmd_dsc.dsc$a_pointer = cmd;

        /* do the command */
        status = qi_command(&ctx, &cmd_dsc, &result);

        if ((status & 1) == 0)   /* this is a bad thing */
            lib$stop(status);
        if (strlen(cmd) == 0)
            break;               /* we shut down on the null command */

        /* print the results */
        for (ind = 0; ind < result->m.dsc$l_m; ind++)
            printf("%s\n", result->m.dsc$a_a0[ind].dsc$a_pointer);
    }
}
