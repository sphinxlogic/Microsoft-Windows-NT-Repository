/* READDIR_SUPPORTED indicates that the system will support readdir().
 * READDIR_TYPE_DIRECT indicates that the type returned by readdir is a
 *    pointer to struct direct.  If it is undefined, the program will use
 *    a pointer to struct dirent.
 * INCLUDE_DIRENT indicates that <dirent.h> should be included when using
 *    readdir() and opendir().
 * DIRENT_IN_SYS indicates that <sys/dirent.h> should be included instead
 *    of <dirent.h> when INCLUDE_DIRENT is set.
 * INCLUDE_SYSDIR indicates that <sys/dir.h> should be included when using
 *    readdir() and opendir().
 * HAS_ASYNC_INPUT indicates that the platform supports
 *    XSelectAsyncInput.  If you get an undefined symbol error on
 *    XSelectAsyncInput at link time, remove this definition.
 * STDLIB indicates that stdlib.h exists on your system.  If it does not,
 *    remove this definition.
 * UNISTD indicates that unistd.h exists on your system.
 * WAIT_H indicates the <sys/wait.h> exists on your system.
 * FGETPOS indicates that your system supports fpos_t, fgetpos, and
 *    fsetpos.  If your compiler complains that fpos_t is undefined, remove
 *    this definiton.
 * VOID_POINTERS indicates that your compiler supports pointers to void.
 *    If you get complaints about a void * declaration, remove this
 *    definition.
 * HAS_VOLATILE indicates that your compiler supports the volatile storage
 *    class.  Remove this definition if lines containing the word "volatile"
 *    cause your compiler to die.
 * HAS_STRERROR indicates that your compiler system has the strerror()
 *    function.  Remove this definition if you get strerror as an undefined
 *    symbol.
 * HAS_ERRNO inicates that errno is declared in errno.h.
 * FAST_SPARKLE indicates that the machine is relatively slow, and a faster
 *    but less pretty sparkle mechanism should be used.
 */

#define FAST_SPARKLE
#define INCLUDE_SYSDIR
#define READDIR_TYPE_DIRECT
#define READDIR_SUPPORTED
#define STDLIB
#define UNISTD
#define WAIT_H
#define HAS_VOLATILE
#define VOID_POINTERS
#define HAS_ERRNO
#define FGETPOS
#define HAS_STRERROR
