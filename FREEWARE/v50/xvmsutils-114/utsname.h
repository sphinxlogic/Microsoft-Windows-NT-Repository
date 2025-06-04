/* $Header: utsname.h,v 1.10.61.3 92/04/19 19:38:26 smp Exp $ */

#ifndef _SYS_UTSNAME_INCLUDED
#define _SYS_UTSNAME_INCLUDED

#define NOSWLEN  5
#define NONALEN  16
#define RELLEN   9
#define VERLEN   2
#define HWNLEN   32
#define SIDLEN   5

struct utsname {
	char	sysname[NOSWLEN];          /* SYI$_NODE_SWTYPE */
	char	nodename[NONALEN];         /* SYI$_NODENAME */
	char	release[RELLEN];           /* SYI$_VERSION */
	char	version[VERLEN];           /* SYI$_? */
	char	machine[HWNLEN];           /* SYI$_HW_NAME */
	char	__idnumber[SIDLEN];        /* SYI$_SID */
   };

#ifdef __cplusplus
  extern "C" {
#endif /* __cplusplus */

#  ifdef _PROTOTYPES
     extern int uname(struct utsname *);
#  else /* not _PROTOTYPES */
     extern int uname();
#  endif /* _not _PROTOTYPES */

#ifdef __cplusplus
  }
#endif /* __cplusplus */

#endif /* _SYS_UTSNAME_INCLUDED */
