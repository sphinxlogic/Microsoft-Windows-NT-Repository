/* libX11 : libX11.c
 * exported data
 * Template created by mkshtmpl.sh 1.4
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived partially from work of Thomas Roell
 *
 * $XFree86: mit/lib/X/libX11.c,v 1.4 1993/05/22 06:42:58 dawes Exp $
 */

#ifdef SVR3SHLIB

#define VoidInit(var) void *var = 0

VoidInit(_libX11__ctype);
VoidInit(_libX11__filbuf);
VoidInit(_libX11__flsbuf);
VoidInit(_libX11__iob);
VoidInit(_libX11_access);
VoidInit(_libX11_alarm);
VoidInit(_libX11_atoi);
VoidInit(_libX11_calloc);
VoidInit(_libX11_close);
VoidInit(_libX11_connect);
VoidInit(_libX11_errno);
VoidInit(_libX11_exit);
VoidInit(_libX11_fclose);
VoidInit(_libX11_fcntl);
VoidInit(_libX11_fgets);
VoidInit(_libX11_fopen);
VoidInit(_libX11_fprintf);
VoidInit(_libX11_fputs);
VoidInit(_libX11_fread);
VoidInit(_libX11_free);
VoidInit(_libX11_fwrite);
VoidInit(_libX11_getenv);
VoidInit(_libX11_gethostbyname);
VoidInit(_libX11_uname);
VoidInit(_libX11_getmsg);
VoidInit(_libX11_getpid);
VoidInit(_libX11_getpwnam);
VoidInit(_libX11_getpwuid);
VoidInit(_libX11_getuid);
VoidInit(_libX11_grantpt);
VoidInit(_libX11_htons);
VoidInit(_libX11_inet_addr);
VoidInit(_libX11_ioctl);
VoidInit(_libX11_malloc);
VoidInit(_libX11_memset);
VoidInit(_libX11_ntohl);
VoidInit(_libX11_ntohs);
VoidInit(_libX11_open);
VoidInit(_libX11_ptsname);
VoidInit(_libX11_putmsg);
VoidInit(_libX11_qsort);
VoidInit(_libX11_read);
VoidInit(_libX11_realloc);
VoidInit(_libX11_rewind);
VoidInit(_libX11_select);
VoidInit(_libX11_setsockopt);
VoidInit(_libX11_signal);
VoidInit(_libX11_sleep);
VoidInit(_libX11_socket);
VoidInit(_libX11_sprintf);
VoidInit(_libX11_sscanf);
VoidInit(_libX11_stat);
VoidInit(_libX11_strcat);
VoidInit(_libX11_strchr);
VoidInit(_libX11_strcmp);
VoidInit(_libX11_strcpy);
VoidInit(_libX11_strncmp);
VoidInit(_libX11_strncpy);
VoidInit(_libX11_strrchr);
VoidInit(_libX11_strtok);
VoidInit(_libX11_sys_errlist);
VoidInit(_libX11_sys_nerr);
VoidInit(_libX11_tolower);
VoidInit(_libX11_unlockpt);
VoidInit(_libX11_write);
VoidInit(_libX11_writev);
VoidInit(_libX11_XauDisposeAuth);
VoidInit(_libX11_XauFileName);
VoidInit(_libX11_XauGetBestAuthByAddr);
VoidInit(_libX11_XauReadAuth);
VoidInit(_libX11__Xevent_to_mask);
VoidInit(_libX11__XdmcpAuthSetup);
VoidInit(_libX11__XdmcpAuthDoIt);
VoidInit(_libX11_XdmcpWrap);
VoidInit(_libX11_getsockname);
VoidInit(_libX11_time);


#ifndef __GNUC__
/*
 * If we are working with floating point aritmetic, stock AT&T cc generates
 * an unresolved reference to __fltused. But we want to make a shared lib from
 * this here and don't want to reference /lib/libc_s.a, just define this sym as
 * (shared lib) static.
 * The trick is that while building the shared lib all references to this
 * symbol are resolved internally. But the symbol will be outside only visible
 * as a static one, so preventing a name conflict with other shared libs.
 */
long __fltused = 0;
#endif

/* A dummy function for free branchtab-slots.
 * This would be a great place for an error-check mechanism for shared libs.
 * (Print error message if not existent function is called.)
 */
extern void _libX11_dummy(){};

#ifndef HASXDMAUTH
extern void _no_des_dummy1(){};
extern void _no_des_dummy2(){};
extern void _no_des_dummy3(){};
#endif


#endif


