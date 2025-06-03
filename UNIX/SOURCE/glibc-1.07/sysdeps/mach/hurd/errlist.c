/* This file is generated from errno.texi by errlist.awk.  */

#ifndef HAVE_GNU_LD
#define _sys_nerr sys_nerr
#define _sys_errlist sys_errlist
#endif

const char *_sys_errlist[] =
  {
    "Success",
    "Operation not permitted",          /* 1 = EPERM */
    "No such file or directory",        /* 2 = ENOENT */
    "No such process",                  /* 3 = ESRCH */
    "Interrupted system call",          /* 4 = EINTR */
    "Input/output error",               /* 5 = EIO */
    "Device not configured",            /* 6 = ENXIO */
    "Argument list too long",           /* 7 = E2BIG */
    "Exec format error",                /* 8 = ENOEXEC */
    "Bad file descriptor",              /* 9 = EBADF */
    "No child processes",               /* 10 = ECHILD */
    "Resource deadlock avoided",        /* 11 = EDEADLK */
    "Cannot allocate memory",           /* 12 = ENOMEM */
    "Permission denied",                /* 13 = EACCES */
    "Bad address",                      /* 14 = EFAULT */
    "Block device required",            /* 15 = ENOTBLK */
    "Device busy",                      /* 16 = EBUSY */
    "File exists",                      /* 17 = EEXIST */
    "Invalid cross-device link",        /* 18 = EXDEV */
    "Operation not supported by device",/* 19 = ENODEV */
    "Not a directory",                  /* 20 = ENOTDIR */
    "Is a directory",                   /* 21 = EISDIR */
    "Invalid argument",                 /* 22 = EINVAL */
    "Too many open files",              /* 23 = EMFILE */
    "Too many open files in system",    /* 24 = ENFILE */
    "Inappropriate ioctl for device",   /* 25 = ENOTTY */
    "Text file busy",                   /* 26 = ETXTBSY */
    "File too large",                   /* 27 = EFBIG */
    "No space left on device",          /* 28 = ENOSPC */
    "Illegal seek",                     /* 29 = ESPIPE */
    "Read-only file system",            /* 30 = EROFS */
    "Too many links",                   /* 31 = EMLINK */
    "Broken pipe",                      /* 32 = EPIPE */
    "Numerical argument out of domain", /* 33 = EDOM */
    "Numerical result out of range",    /* 34 = ERANGE */
    "Resource temporarily unavailable", /* 35 = EAGAIN */
    "Operation would block",            /* 36 = EWOULDBLOCK */
    "Operation now in progress",        /* 37 = EINPROGRESS */
    "Operation already in progress",    /* 38 = EALREADY */
    "Socket operation on non-socket",   /* 39 = ENOTSOCK */
    "Destination address required",     /* 40 = EDESTADDRREQ */
    "Message too long",                 /* 41 = EMSGSIZE */
    "Protocol wrong type for socket",   /* 42 = EPROTOTYPE */
    "Protocol not available",           /* 43 = ENOPROTOOPT */
    "Protocol not supported",           /* 44 = EPROTONOSUPPORT */
    "Socket type not supported",        /* 45 = ESOCKTNOSUPPORT */
    "Operation not supported",          /* 46 = EOPNOTSUPP */
    "Protocol family not supported",    /* 47 = EPFNOSUPPORT */
    "Address family not supported by protocol family",/* 48 = EAFNOSUPPORT */
    "Address already in use",           /* 49 = EADDRINUSE */
    "Can't assign requested address",   /* 50 = EADDRNOTAVAIL */
    "Network is down",                  /* 51 = ENETDOWN */
    "Network is unreachable",           /* 52 = ENETUNREACH */
    "Network dropped connection on reset",/* 53 = ENETRESET */
    "Software caused connection abort", /* 54 = ECONNABORTED */
    "Connection reset by peer",         /* 55 = ECONNRESET */
    "No buffer space available",        /* 56 = ENOBUFS */
    "Socket is already connected",      /* 57 = EISCONN */
    "Socket is not connected",          /* 58 = ENOTCONN */
    "Can't send after socket shutdown", /* 59 = ESHUTDOWN */
    "Connection timed out",             /* 60 = ETIMEDOUT */
    "Connection refused",               /* 61 = ECONNREFUSED */
    "Too many levels of symbolic links",/* 62 = ELOOP */
    "File name too long",               /* 63 = ENAMETOOLONG */
    "Host is down",                     /* 64 = EHOSTDOWN */
    "No route to host",                 /* 65 = EHOSTUNREACH */
    "Directory not empty",              /* 66 = ENOTEMPTY */
    "Too many users",                   /* 67 = EUSERS */
    "Disc quota exceeded",              /* 68 = EDQUOT */
    "Stale NFS file handle",            /* 69 = ESTALE */
    "Too many levels of remote in path",/* 70 = EREMOTE */
    "No locks available",               /* 71 = ENOLCK */
    "Function not implemented",         /* 72 = ENOSYS */
    "Inappropriate operation for background process",/* 73 = EBACKGROUND */
    "?",                                /* 74 = ED */
    "You really blew it this time",     /* 75 = EGREGIOUS */
    "Computer bought the farm",         /* 76 = EIEIO */
    "Gratuitous error",                 /* 77 = EGRATUITOUS */
  };

const int _sys_nerr = 77;
