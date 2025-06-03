#include "patchlevel.h"
#include "libutils.h"
#define CMDLEN_MAX 20
#define MAXDATAOBJ 80
extern MY_FILE *program_fp;
extern char program_fname[];
extern int line_no;

struct prog_loc {
    my_fpos_t filepos;
    int line;
};

struct label_ent {
    char *label;
    struct prog_loc pos;
    struct label_ent *next;
};

struct prog_info_rec {
    MY_FILE *save_fp;
    char save_fname[FILENAME_MAX];
    struct label_ent *save_labels;
    struct prog_loc save_loc;
    struct prog_loc lastloc;
    int prog_index;
};

BOOLEAN get_prog_loc(struct prog_loc *loc);
BOOLEAN set_prog_loc(struct prog_loc *loc);
BOOLEAN exec_line(char *cmd, char *err_msg);
BOOLEAN find_data(struct prog_loc *loc);
BOOLEAN find_label_location(char *label, struct prog_loc *pos, char *err_msg);
BOOLEAN save_prog_info(struct prog_info_rec *pi, char *err_msg);
BOOLEAN restore_prog_info(struct prog_info_rec *pi, char *err_msg);
