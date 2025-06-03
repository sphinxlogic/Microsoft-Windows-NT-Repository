/* These are so we don't end up using the MultiNet versions.  */
#define socket	VMSsocket
#define bind	VMSbind
#define connect	VMSconnect
#define listen	VMSlisten
#define accept	VMSaccept
#define select	VMSselect
#define recv	VMSrecv
#define recvfrom	VMSrecvfrom
#define send	VMSsend
#define sendto	VMSsendto
#define getsockname	VMSgetsockname
#define shutdown	VMSshutdown
#define getsockopt	VMSgetsockopt
#define setsockopt	VMSsetsockopt
