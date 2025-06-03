/*
 * Define structures for mailbox messages used in communication between
 * http server and authenticator process.
 */
struct msg1 {			/* Authentication request messages */
    int code;				/* function requested by server */
    int local_port, remote_port;	/* TCP port numbers */
    unsigned char remote_addr[4];	/* IP address of client */
    int setup_len;			/* setup file length */
    int ident_len;			/* File ident len */
    char method[8];			/* Method, truncated 7 chars */
    char data[1000];			/* additional data, delimited
					 * by linefeeds */
};
#define REQ_HDR_SIZE (sizeof(char)*12 + sizeof(int)*5)

struct msg2 {			/* Authentication response */
    int status;				/* Result status */
    int sts_len;			/* Length of HTTP response */
    int log_len;			/* Length of log file line */
    char data[500];
};
#define RSP_HDR_SIZE (sizeof(int)*3)
/*
 * Define mailbox protection to use, disallow world and group access.
 */
#define MBX_PROT 0x0ff00
