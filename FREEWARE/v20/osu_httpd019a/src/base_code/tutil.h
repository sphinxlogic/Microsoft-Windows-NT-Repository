/*
 * Define data structures needed as input arguments to utility routines.
 */
struct tu_streambuf {	/* Special structure for use with read_line() */
    void *ctx;		/* tcp_io context */
    int (*getrtn)();	/* User read function, called with arguments:
			 *   (int) get(ctx, char *buf, int maxlen, int *len );
			 */
    int state;		/* Flags cr-pending, initialize to zero */
    int bufsize;	/* max Network read size, 1-1000 */
    int used;		/* number of bytes in buffer consumed */
    int filled;		/* Number of bytes supplied by last QIO read */
    char data[1000];
};
typedef struct tu_streambuf *tu_stream;

struct tu_textbuf {	/* Structure for incrementally building text lines */
    int l;		/* Number of chars added so far */
    char *s;		/* Pointer to data string */
    int size;		/* Allocated size of s. */
};
typedef struct tu_textbuf *tu_text;
/*
 * Define prototypes for utility files.
 */
int tu_load_file ( char *name, char *defname, int limit, char *txt, int *size );
int tu_strlen(char *str );
char *tu_strcpy ( char *str1, char *str2 );
char *tu_strncpy ( char *str1, char *str2, int limit );
char *tu_strnzcpy ( char *str1, char *str2, int limit );
char *tu_unescape ( char *str1, char *str2, int limit );
int tu_strncmp ( char *str1, char *str2, int maxl );
int tu_strmatchwild ( char *cand, char *pattern );
char *tu_strint ( int value, char *buffer );
char *tu_strupcase ( char *dst, char *src );
char *tu_strlowcase ( char *dst, char *src );
int tu_add_text ( struct tu_textbuf *buf, char *src, int maxlen );

int tu_init_stream ( void *ctx, int getrtn(), tu_stream stream );
int tu_read_line ( tu_stream stream, char *line, int maxlen, int *length );
int tu_read_raw ( tu_stream stream, char *line, int maxlen, int *length );
char *tu_strstr ( char *s1, char *s2 );
