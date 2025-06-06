#ifndef MTOOLS_PLAINIO_H
#define MTOOLS_PLAINIO_H

#include "stream.h"
#include "msdos.h"

/* plain io */
#define NO_PRIV 1
#define NO_OFFSET 2

Stream_t *SimpleFileOpen(struct device *dev, struct device *orig_dev,
			 const char *name, int mode, char *errmsg, int mode2);
int check_parameters(struct device *ref, struct device *testee);

int get_fd(Stream_t *Stream);
#endif
