/************************************************************************
 *
 *  (C) Copyright 1992-1993 The Trustees of Indiana University
 *
 *   Permission to use, copy, modify, and distribute this program for
 *   non-commercial use and without fee is hereby granted, provided that
 *   this copyright and permission notice appear on all copies and
 *   supporting documentation, the name of Indiana University not be used
 *   in advertising or publicity pertaining to distribution of the program
 *   without specific prior permission, and notice be given in supporting
 *   documentation that copying and distribution is by permission of
 *   Indiana University.
 *
 *   Indiana University makes no representations about the suitability of
 *   this software for any purpose. It is provided "as is" without express
 *   or implied warranty.
 *
 **************************************************************************
 *
 * Module:
 *     iufingerd.h - header containing general stuff.
 *
 * Description:
 *     This header file contains general stuff for the entire project.
 *
 ************************************************************************/


/************************************************************************
 * #include's
 ************************************************************************/
#include <sys/types.h>
#ifdef TCPWARE
#include "sys$library:types.h"
#endif
#include <stdio.h>
#include <string.h>
#include <maildef.h>
#include <ssdef.h>
#include <jpidef.h>
#include <uaidef.h>
#include <rmsdef.h>
#include <descrip.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <file.h>
#include <libdtdef.h>

/** berkeley network stuff **/
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/netdb.h>

#ifdef WINS
#include <vms/iodef.h>
#else /* WINS */
#include <iodef.h>
#endif /* WINS */

#ifdef UCX
#include "ucx_compatibility.h"
#endif /* UCX */

#if defined(MULTINET) || defined(TCPWARE)
#include <vms/inetiodef.h>
#endif /* MULTINET || TCPWARE */

/************************************************************************
 * #define's
 ************************************************************************/
#define FINGER_PORT  79

#define RFC931_PORT 113
#define RFC931_MAYBE  0
#define RFC931_YES    1
#define RFC931_NO     2

#define ATTN_CONNECT     1
#define ATTN_DISCONNECT  2
#define ATTN_TIMEOUT     3
#define ATTN_DATA        4

#define FUIP_C           0
#define FUIP_Q1          1
#define FUIP_Q1W         2
#define FUIP_Q2          3
#define FUIP_TIMEOUT     4
#define FUIP_CLOSED      5

#define SIZE_IMAGE       255
#define SIZE_TERMINAL    12
#define SIZE_REMOTE      255
#define SIZE_HOMEDIR     80
#define SIZE_PROCNAME    16
#define SIZE_UAFOWNER    255
#define SIZE_USERNAME    12

#define MAX_PLAN_CHARS    8192
#define MAX_PLAN_NAMES       8
#define MAX_PLAN_FILENAME   64

#define MAX_ONSINCE 8

#define TIME_NUMERIC	0
#define TIME_TEXT	1

#ifndef TIME_ZONE_LOGICAL	/* Allow compile-time definition to override */
#ifdef WINS			/* defaults for Wollongong Pathway...        */
#define TIME_ZONE_LOGICAL "WIN$TIME_ZONE"
#endif WINS
#ifdef MULTINET			/* ...and TGV Multinet                       */
#define TIME_ZONE_LOGICAL "MULTINET_TIMEZONE"
#endif MULTINET
#endif TIME_ZONE_LOGICAL

#ifndef TIME_ZONE_LOGICAL	/* If still undefined, try the DECdts logical*/
#define TIME_ZONE_LOGICAL "SYS$TIMEZONE_NAME"
#endif TIME_ZONE_LOGICAL

#ifdef WINS
#define nwrite netwrite
#define nread  netread
#define nclose netclose
#endif /* WINS */

#ifdef UCX
#define nwrite write
#define nread  read
#define nclose close
#define bzero(obj, size) memset(obj, '\0', size)
#endif /* UCX */

#ifdef MULTINET
#define nwrite socket_write
#define nread  socket_read
#define nclose socket_close
#define ioctl  socket_ioctl
#endif MULTINET

#ifdef TCPWARE
#define accept     inet_accept
#define bind       inet_bind
#define socket     inet_socket
#define listen     inet_listen
#define connect    inet_connect
#define select     inet_select
#define ioctl      inet_socket_ioctl
#define setsockopt inet_setsockopt
#define nclose     inet_socket_close
#define nread      inet_socket_read
#define nwrite     inet_socket_write
#define SOMAXCONN  5
#endif TCPWARE

#ifdef ALPHA
#define close decc$close
#define open  decc$open
#define read  decc$read
#endif ALPHA

/************************************************************************
 * type definitions
 ************************************************************************/

typedef struct item_list
{
  unsigned short buflen;
  unsigned short icode;
  unsigned long  bufaddr;
  unsigned long  rla;
} itemlist;

typedef unsigned short unsigned_word;

typedef unsigned int quadword[2];

struct IOSB
{
  unsigned short status;
  unsigned short terminator_offset;
  unsigned short terminator;
  unsigned short terminator_size;
};

typedef struct
{
  unsigned short keyCount; /** should be 1 **/
  unsigned short type;     /** should be DSC$K_DTYPE_T **/
  unsigned short order;    /** should be 0 **/
  unsigned short offset;
  unsigned short length;
} sortBlock;

struct user_data
{
  char username[80];
  char realName[80];
  char homeDir[SIZE_HOMEDIR + 1];
  char forward[256];
  char lastLogin[80];
  char onSince[MAX_ONSINCE][80];
  int  validUser;
  int  hasNewMail;
  int  wasCached;
};

struct client_data
{
  char readBuffer[256];
  char request[256];
  char username[80];
  int  readComplete;
  int  socket;
  unsigned channel;
  struct IOSB iosb;
  struct sockaddr_in sin;
};

typedef struct user_cache_entry
{
  char username[SIZE_USERNAME + 1];
  char realName[SIZE_UAFOWNER + 1];
  char homeDir[SIZE_HOMEDIR + 1];
};

typedef struct user_hash_entry
{
  time_t timeStamp;
  struct user_cache_entry cacheEntry;
  struct user_hash_entry *nextEntry;
} USER_HASH_ENTRY;

typedef struct
{
  USER_HASH_ENTRY **table;
  int tableSize;
} USER_HASH_TABLE;

typedef struct host_cache_entry
{
  u_long IPaddress;
  char hostName[SIZE_REMOTE+1];
  int rfc931Status;
};

typedef struct host_hash_entry
{
  time_t timeStamp;
  struct host_cache_entry cacheEntry;
  struct host_hash_entry *nextEntry;
} HOST_HASH_ENTRY;

typedef struct
{
  int tableSize;
  HOST_HASH_ENTRY **table;
} HOST_HASH_TABLE;

struct jpiInfo
{
  char title[256];
  char fao[256];
  struct
  {
    int  image, loginTime, pid, procName,
         remote, terminal, uafowner, username;
  } fields;
};

struct jpiData
{
  char username[SIZE_USERNAME+1];
  char terminal[SIZE_TERMINAL+1];
  char image[SIZE_IMAGE+1];
  char remote[SIZE_REMOTE+1];
  char uafowner[SIZE_UAFOWNER+1];
  char procName[SIZE_PROCNAME+1];
  char *realName;
  int  loginTime[2];
  char numericTimeString[32];
  char vmsTimeString[32];
  int  type;
  int  mode;
  unsigned int pid;
  unsigned int masterPid;
  char pidString[12];
};


/************************************************************************
 * macros
 ************************************************************************/
#define VmsError(status) (!(status & 1))

/************************************************************************
 * Global Externs
 ************************************************************************/
extern int debugFlag;
extern int headerFlag;
extern int systatFlag;
extern int homeDirFlag;
extern int loginFlag;
extern int mailFlag;
extern int planFlag;
extern int projectFlag;
extern int resolveFlag;
extern int rfc931Flag;
extern int sortFlag;

extern int attnState;

extern int portNumber;

extern int sortPosition;
extern int sortSize;

extern int rfc931Timeout;

extern int userCacheSize;
extern int userCacheTtl;

extern int hostCacheSize;
extern int hostCacheTtl;

extern int purgeInterval;

extern int timeFormat;

extern struct IOSB connectIosb;
extern struct client_data clientData;
extern struct jpiInfo jpiConfig;

extern char planNames[MAX_PLAN_NAMES+1][MAX_PLAN_FILENAME+1];
extern char projectNames[MAX_PLAN_NAMES+1][MAX_PLAN_FILENAME+1];

/************************************************************************
 * function prototypes
 ************************************************************************/
int        CacheHost(struct host_cache_entry *hostEntry);
int        CacheUser(struct user_cache_entry *userEntry);
void       CancelReadTimeoutQio(void);
void       CheckMail(struct user_data *userData);
int        CheckUser(char *username, char *aUsername);
HOST_HASH_TABLE *CreateHostHashTable(unsigned int tableSize);
USER_HASH_TABLE *CreateUserHashTable(unsigned int tableSize);
void       DownCase(char *string);
void       Fao(struct jpiData *job, char *outString, int outSize);
void       FixRemote(char *remote);
int        FType(char *theQuery);
void       FListQuery(int sd);
void       FSingleQuery(int sd, struct user_data *userData);
int        GetJpi(int *pid, struct jpiData *job);
int        HashUser(char *string, USER_HASH_TABLE *hash);
int        HashIPaddr(u_long IPaddress, HOST_HASH_TABLE *hash);
int        HostCached(struct host_cache_entry *hostEntry);
HOST_HASH_ENTRY *HostHashInsert(struct host_cache_entry *hostEntry,
                                HOST_HASH_TABLE *hash);
HOST_HASH_ENTRY *HostHashFind(struct host_cache_entry *hostEntry,
                              HOST_HASH_TABLE *hash);
int        InitCaches(int userTableSize, int hostTableSize);
int        IssueNewConnectQio(int sd);
int        IssueReadDataQio(void);
int        IssueReadTimeoutQio(void);
void       ListHeader(int sd);
int        LoggedIn(struct user_data *userData);
int        NetworkAcceptConnection(int sd, struct client_data *clientData);
void       NetworkCloseConnection(struct client_data *clientData);
void       NetworkConnectAst();
int        NetworkOpen(unsigned short portNum);
void       NumericTime(int *sysTime, char *timeString);
void       ProcessArgs();
int        ProcessRequest();
void       PurgeCaches(int userTimeToLive, int hostTimeToLive);
void       ReadAst(void);
void       ReadTimeoutAst(void);
int        ResolveRemote(u_long IPaddress, char *remoteName);
int        UserCached(struct user_cache_entry *userEntry);
USER_HASH_ENTRY *UserHashInsert(struct user_cache_entry *userEntry,
                                USER_HASH_TABLE *hash);
USER_HASH_ENTRY *UserHashFind(struct user_cache_entry *userEntry,
                              USER_HASH_TABLE *hash);
char       *RemoteUsername(u_long remoteAddress, int remotePort, int localPort);
void	   SystatListHeader(int sd, int interactiveJobs, int batchJobs);
void       SystemLog();
void       Tokenize(char *inBuffer, char *outBuffer);
void       TrimImage(char *image);
void       UpCase(char *string);
void       UaiRetrieve(struct user_data *userData);
char       *VmsMessage(unsigned int code);
void       VmsTime(int *time, char *buffer);
void       WayCoolTime(int *sysTime, char *timeString);
