/*
 * in case strdup is not provided by the system here is one
 * which does the trick
 */
char *
strdup(s1)
    char *s1;
{
    char *s2;
    int l = strlen(s1) + 1;

    if (s2 = (char *) malloc(l))
        strncpy(s2, s1, l);
    return s2;
}
