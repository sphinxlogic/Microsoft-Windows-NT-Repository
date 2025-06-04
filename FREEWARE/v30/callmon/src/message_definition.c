/*  File:     MESSAGE_DEFINITION.C
 *  Author:   Thierry Lelegard
 *  Date:     24-JUL-1996
 *
 *  Abstract: This program generates a C or Ada definition file
 *            for all constants that are defined in an object
 *            file, like message files.
 *
 *  Arguments: -language object-file-name module-name
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <egsdef.h>
#include <egsydef.h>
#include <esdfdef.h>
#include <eobjrecdef.h>


/*
 *  Languages properties.
 */

typedef void (*fmt_start_t) (FILE* fp, char* module);
typedef void (*fmt_symbol_t) (FILE* fp, long value, char* symbol, int length);
typedef void (*fmt_end_t) (FILE* fp, char* module);

typedef struct {
    char*        language;
    char*        file_name_format;
    char*        comment_start;
    char*        comment_end;
    fmt_start_t  fmt_start;
    fmt_symbol_t fmt_symbol;
    fmt_end_t    fmt_end;
} language_t;

static void c_fmt_start (FILE* fp, char* module);
static void c_fmt_symbol (FILE* fp, long value, char* symbol, int length);
static void c_fmt_end (FILE* fp, char* module);

static void ada_fmt_start (FILE* fp, char* module);
static void ada_fmt_symbol (FILE* fp, long value, char* symbol, int length);
static void ada_fmt_end (FILE* fp, char* module);

static language_t languages [] = {
    {"-c", "%s.H", "/* ", " */", c_fmt_start, c_fmt_symbol, c_fmt_end},
    {"-ada", "%s_.ADA", "-- ", "", ada_fmt_start, ada_fmt_symbol, ada_fmt_end},
    {NULL}};


/*
 *  C language routines
 */

static void c_fmt_start (FILE* fp, char* module)
{
    fprintf (fp, "#ifndef %s_H_LOADED\n#define %s_H_LOADED 1\n",
        module, module);
}

static void c_fmt_symbol (FILE* fp, long value, char* symbol, int length)
{
    fprintf (fp, "#define %.*s 0x%08X\n", length, symbol, value);
}

static void c_fmt_end (FILE* fp, char* module)
{
    fprintf (fp, "#endif /* %s_H_LOADED */\n", module);
}


/*
 *  Ada language routines
 */

static void ada_fmt_start (FILE* fp, char* module)
{
    fprintf (fp, "package %s is\n", module);
}

static void ada_fmt_symbol (FILE* fp, long value, char* symbol, int length)
{
    fprintf (fp, "    ");
    for (; length > 0; length--, symbol++) {
        if (*symbol == '$') {
            if (length > 1 && symbol [1] != '_')
                putc ('_', fp);
        }
        else if (*symbol == '_') {
            if (length > 1 && symbol [1] != '_')
                putc ('_', fp);
        }
        else
            putc (*symbol, fp);
    }
    fprintf (fp, " : constant := 16#%08X#;\n", value);
}

static void ada_fmt_end (FILE* fp, char* module)
{
    fprintf (fp, "end %s;\n", module);
}


/*
 *  Main program
 */

int main (int argc, char** argv)
{
    char* language_name;
    char* object_file;
    char* module_name;
    language_t* lang;
    int obj;
    FILE* out;
    char out_file_name [255];
    char object_record [EOBJ$C_MAXRECSIZ];
    int record_size;
    struct egsdef* egsd;
    struct esdfdef* esdf;
    char* end_record;
    char* p;

    /* Check command line options */

    if (argc != 4) {
        fprintf (stderr, "Arguments: -language object-file module-name\n");
        exit (EXIT_FAILURE);
    }

    language_name = argv [1];
    object_file = argv [2];
    module_name = argv [3];

    /* Look for the requested language */

    for (lang = languages; lang->language != NULL; lang++)
        if (strcmp (lang->language, language_name) == 0)
            break;

    if (lang->language == NULL) {
        fprintf (stderr, "Unknown language %s\n", language_name);
        exit (EXIT_FAILURE);
    }

    /* Convert the module name to uppercase */

    for (p = module_name; *p != '\0'; p++)
        if (islower (*p))
            *p = toupper (*p);

    /* Open the object file in record mode */

    if ((obj = open (object_file, O_RDONLY, 0, "ctx=rec", "dna=.obj")) < 0) {
        perror (object_file);
        exit (EXIT_FAILURE);
    }

    /* Open the output definition file */

    sprintf (out_file_name, lang->file_name_format, module_name);

    if ((out = fopen (out_file_name, "w")) == NULL) {
        perror (out_file_name);
        exit (EXIT_FAILURE);
    }

    /* Generate introduction sequence */

    fprintf (out, "%s%s - Automatically generated file, do not modify%s\n\n",
        lang->comment_start, out_file_name, lang->comment_end);

    lang->fmt_start (out, module_name);
    fprintf (out, "\n");

    /* Read all object records */

    while ((record_size = read (obj, object_record, EOBJ$C_MAXRECSIZ)) > 0) {

        egsd = (struct egsdef*) object_record;
        end_record = object_record + egsd->egsd$w_recsiz;

        /* Verify actual record size */

        if (record_size < egsd->egsd$w_recsiz) {
            fprintf (stderr, "Invalid record in %s: %d bytes, expected %d\n",
                object_file, record_size, egsd->egsd$w_recsiz);
            continue;
        }

        /* Keep Global Symbol Directory records only */

        if (egsd->egsd$w_rectyp != EOBJ$C_EGSD)
            continue;

        /* Get address of first subrecord */

        esdf = (struct esdfdef*) &egsd->egsd$w_gsdtyp;

        /* Loop on all subrecords of this record */

        while ((char*)esdf->esdf$t_name < end_record &&
               (char*)esdf + esdf->esdf$w_size <= end_record) {

            /* Keep only symbol definitions of constants */

            if (esdf->esdf$w_gsdtyp == EGSD$C_SYM &&
                (esdf->esdf$w_flags & EGSY$M_DEF) != 0 &&
                (esdf->esdf$w_flags & EGSY$M_REL) == 0) {

                /* Generate symbol definition */

                lang->fmt_symbol (out, esdf->esdf$l_value,
                    esdf->esdf$t_name, esdf->esdf$b_namlng);
            }

            /* Point to next subrecord. Subrecords are always rounded
             * up to quadword boundary. */

            if (esdf->esdf$w_size % 8 == 0)
                esdf = (struct esdfdef*) ((char*)esdf + esdf->esdf$w_size);
            else
                esdf = (struct esdfdef*) ((char*)esdf + esdf->esdf$w_size +
                    8 - esdf->esdf$w_size % 8);
        }
    }

    if (record_size < 0)
        perror (object_file);

    /* Generate termination sequence */

    fprintf (out, "\n");
    lang->fmt_end (out, module_name);

    fclose (out);
    close (obj);
}
