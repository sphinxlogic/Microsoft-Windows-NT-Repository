/*
 * Define structure that holds pointers to essential session information
 * for a thread connection to the HTTP server.
 */
typedef struct { int l; char *s; } string;
#include "tutil.h"
#include "access.h"

struct session_control_block {
    void *cnx;				/* TCP/IP connection context */
    string *request;			/* Request header lines read from
					 * client */
    tu_stream inbound;			/* Cooked input stream */
    tu_text rsphdr;			/* Response header */
    int data_bytes;			/* Content-length of response */
    access_info acc;			/* Protection/authentication info */
    char *log_prefix;			/* tag for log file output */
    int completed;			/* True of response sent */
    int recursion_level;		/* counts script recursions */
};
#define SCRIPT_RECURSION_LIMIT 4

typedef struct session_control_block *session_ctx;
/*
 * Request array values:
 *	[0]	Method.
 *	[1]	URL
 *	[2]	Protocol, either HTTP/1.0 or null.
 *	[2+i]	Additional header lines (1..n).
 *	[3+n]	Null line to indicate end of request.
 */
