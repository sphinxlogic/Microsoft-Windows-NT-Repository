/*
 * Define structures for tracking usage and prototypes for counter routines.
 */
struct client_counter {
    int ndx;				/* Thread index tag */
    int port;
    unsigned char *address;
    char *method;			/* Request method */
    char *ident;
};
struct host_counter {
    struct host_counter *next;
    char *class_name;
    int count;
    char mask[4096];
};
struct counter_block {
    int active_size;			/* storage allocated for active list */
    int active_sp;			/* Number active */
    int *active_hist;			/* histogram of concurrent clients */
    struct client_counter *active_stack;

    struct host_counter *host;
    char zero_time[40];			/* Time counters zeroed, formatted */
} *http_counters;

int http_enable_active_counters();
int http_open_active_counter ( int ndx, int port, unsigned char *address );
int http_set_active_counter ( int ndx, char *method, char *request );
int http_close_active_counter ( int ndx );
int http_define_host_counter ( char *class_name, char *host_mask );
int http_increment_host_counters ( unsigned char *address );
int http_lock_counters();
int http_unlock_counters();
int http_zero_counters(int have_lock);
