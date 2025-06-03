*** /usr/include/sys/stat.h~	Tue Nov 17 20:38:57 1992
--- /usr/include/sys/stat.h	Fri Jan 21 11:42:22 1994
***************
*** 328,336 ****
  
  #if !defined(_STYPES) && !defined(_KERNEL)
  static int
! stat(_path, _buf)
! const char *_path;
! struct stat *_buf;
  {
  int ret;
  	ret = _xstat(_STAT_VER, _path, _buf);
--- 328,334 ----
  
  #if !defined(_STYPES) && !defined(_KERNEL)
  static int
! stat(const char *_path, struct stat *_buf)
  {
  int ret;
  	ret = _xstat(_STAT_VER, _path, _buf);
***************
*** 339,347 ****
  
  #if !defined(_POSIX_SOURCE) 
  static int
! lstat(path, buf)
! const char *path;
! struct stat *buf;
  {
  int ret;
  	ret = _lxstat(_STAT_VER, path, buf);
--- 337,344 ----
  
  #if !defined(_POSIX_SOURCE) 
  static int
! lstat(const char *path,
!       struct stat *buf)
  {
  int ret;
  	ret = _lxstat(_STAT_VER, path, buf);
***************
*** 350,358 ****
  #endif /* !defined(_POSIX_SOURCE) */
  
  static int
! fstat(_fd, _buf)
! int _fd;
! struct stat *_buf;
  {
  int ret;
  	ret = _fxstat(_STAT_VER, _fd, _buf);
--- 347,354 ----
  #endif /* !defined(_POSIX_SOURCE) */
  
  static int
! fstat(int _fd,
!       struct stat *_buf)
  {
  int ret;
  	ret = _fxstat(_STAT_VER, _fd, _buf);
***************
*** 361,370 ****
  
  #if !defined(_POSIX_SOURCE) 
  static int
! mknod(path, mode, dev)
! const char *path;
! mode_t mode;
! dev_t dev;
  {
  int ret;
  	ret = _xmknod(_MKNOD_VER, path, mode, dev);
--- 357,365 ----
  
  #if !defined(_POSIX_SOURCE) 
  static int
! mknod(const char *path,
!       mode_t mode,
!       dev_t dev)
  {
  int ret;
  	ret = _xmknod(_MKNOD_VER, path, mode, dev);
