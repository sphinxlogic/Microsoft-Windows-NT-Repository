#ifndef MTOOLS_FILE_H
#define MTOOLS_FILE_H

#include "stream.h"

Stream_t *open_file(Stream_t *Dir, struct directory *dir);
Stream_t *open_root(Stream_t *Dir);
int FileIsLocked(Stream_t *Stream);
void LockFile(Stream_t *Stream);


#endif
