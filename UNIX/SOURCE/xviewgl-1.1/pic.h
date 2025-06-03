#include "patchlevel.h"
#include "libutils.h"
int pic_read(byte *buf, int n, MY_FILE *fp);
void *pic_alloc(int sz);
void pic_free(byte *b);
int pic_blread(byte **b, MY_FILE *fp, byte *esc);
BOOLEAN pic_load(char *fname, int reg, BOOLEAN load_cmap, int trancolor, char *err_msg);
