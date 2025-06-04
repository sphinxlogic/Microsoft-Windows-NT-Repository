/* phtool - ph/qi API output to file */
/* usage: phtool "query" output-file */

#include <stdlib.h>
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

main(int argc, char *argv[])
{
    char *cp, cmd[100];
    FILE *fp;
    void *ctx = NULL;         /* init ctx to null */
    array *result = NULL;     /* init result to null */
    int ind, status;
    struct dsc$descriptor_s cmd_dsc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

    if (argc < 3)   /* require both arguments */
        exit(0);


        /* build a command descriptor */
        cmd_dsc.dsc$w_length = (short) strlen(argv[1]);
        cmd_dsc.dsc$a_pointer = argv[1];

        /* do the command */
        status = qi_command(&ctx, &cmd_dsc, &result);

        if ((status & 1) == 0)   /* this is a bad thing */
            lib$stop(status);

        /* print the results */
        if ((fp = fopen(argv[2], "w")) == NULL)
            exit(0);
        for (ind = 0; ind < result->m.dsc$l_m; ind++)
            fprintf(fp, "%s\n", result->m.dsc$a_a0[ind].dsc$a_pointer);
        fclose(fp);
}
