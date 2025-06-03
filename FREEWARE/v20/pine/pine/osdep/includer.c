#if !defined(lint) && !defined(DOS)
static char rcsid[] = "$Id: includer.c,v 5.3 1993/07/20 21:24:07 hubert Exp $";
#endif
#include <stdio.h>
#include <string.h>

/*
 * Inflexible cat with include files.
 * The include lines must look exactly like  include(filename)  with no
 * spaces before the include, or between the parens and the surrounding
 * characters.
 *
 * This probably ought to just be a script that uses "cat".
 */
main() {
    readfile(stdin, stdout);
    exit(0);
}

readfile(fpin, fpout)
FILE *fpin, *fpout;
{
    char line[BUFSIZ+1];
    char tmp[BUFSIZ+1];
    FILE *fp;
    char *p, *fname, *fend;

    while ((p = fgets(line, BUFSIZ, fpin)) != NULL) {

        if (!strncmp("include(", p, strlen("include("))) {

            /* do include */
            fname = strchr(p, '(');
            if (fname == NULL) {
                fprintf(stderr, "Can't find include file %s\n", p);
                exit(1);
            }
            fname++;
            fend = strrchr(fname, ')');
            if (fend == NULL) {
                fprintf(stderr, "Can't find include file %s\n", p);
                exit(1);
            }
            *fend = '\0';
            if ((fp = fopen(fname, "r")) == NULL) {
                fprintf(stderr, "Can't open include file %s\n", fname);
                exit(1);
            }
            readfile(fp, fpout);
            fclose(fp);

        /* skip if comment line (begins with ;) */
        }else if (*p != ';') {
            fputs(p, fpout);
        }
    }
}
