*** /usr/include/string.h~	Fri Jan 28 17:50:59 1994
--- /usr/include/string.h	Fri Jan 28 17:51:07 1994
***************
*** 23,32 ****
--- 23,36 ----
  
  #if defined(__STDC__)
  
+ #ifndef __GNUC__
  extern void *memcpy(void *, const void *, size_t);
+ #endif
  extern void *memccpy(void *, const void *, int, size_t);
  extern void *memmove(void *, const void *, size_t);
+ #ifndef __GNUC__
  extern char *strcpy(char *, const char *);
+ #endif
  extern char *strdup(const char *);
  
  extern char *strncpy(char *, const char *, size_t);
***************
*** 34,41 ****
--- 38,47 ----
  extern char *strcat(char *, const char *);
  extern char *strncat(char *, const char *, size_t);
  
+ #ifndef __GNUC__
  extern int memcmp(const void *, const void *, size_t);
  extern int strcmp(const char *, const char *);
+ #endif
  extern int strcoll(const char *, const char *);
  extern int strncmp(const char *, const char *, size_t);
  extern size_t strxfrm(char *, const char *, size_t);
***************
*** 58,63 ****
--- 64,70 ----
  
  #if #machine(i386) 
  #ifndef __cplusplus
+ #ifndef __GNUC__
  #ifndef strlen
  #define strlen __std_hdr_strlen
  #endif
***************
*** 66,71 ****
--- 73,79 ----
  #endif
  #ifndef strncpy
  #define strncpy __std_hdr_strncpy
+ #endif
  #endif
  #endif
  #endif
