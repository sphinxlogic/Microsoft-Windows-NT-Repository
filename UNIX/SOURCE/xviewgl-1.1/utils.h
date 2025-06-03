#include "patchlevel.h"
char *downcase(char *s);
char *preprocess_line(char *s);
char *tokenize(char *s);
char *extend(char *s, char *ext);
#define my_tolower(c) (isupper(c)?tolower(c):(c))
