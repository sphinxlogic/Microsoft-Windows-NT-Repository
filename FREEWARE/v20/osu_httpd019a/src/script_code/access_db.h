#ifndef IFILE_T
#define IFILE void	/* make structure opaque */
#endif

IFILE* ifopen(char *fn, char *mode);
int ifclose(IFILE *ifile);
int iferror(IFILE *ifile);
int ifread_rec ( void *ptr, size_t size_of_item, size_t *size_read,
  IFILE *ifile, int index, void *key, int key_size);
int ifwrite_rec(void *ptr, size_t size_of_item, IFILE *ifile);
int ifupdate_rec(void *ptr, size_t size_of_item, IFILE *ifile);
int ifdlcreate(char *fdl, char *fname, char *defname);
int icheck_access ( char *fname, int uic );
