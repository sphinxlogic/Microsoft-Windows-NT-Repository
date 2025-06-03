*** /usr/include/math.h~	Mon Nov  2 18:50:17 1992
--- /usr/include/math.h	Fri Jan 21 11:37:29 1994
***************
*** 90,96 ****
  #define HUGE_VAL (+__huge_val)
  #endif	/* HUGE_VAL */
  
! #if (__STDC__ == 0 && !defined(_POSIX_SOURCE)) || defined(_XOPEN_SOURCE)
  
  extern double erf(double); 
  extern double erfc(double); 
--- 90,96 ----
  #define HUGE_VAL (+__huge_val)
  #endif	/* HUGE_VAL */
  
! #if (__STDC__ && !defined(_POSIX_SOURCE)) || defined(_XOPEN_SOURCE)
  
  extern double erf(double); 
  extern double erfc(double); 
*** 174,180 ****
  
  #endif	/* __STDC__ */	
  
! #if (__STDC__ - 0 == 0 && !defined(_POSIX_SOURCE)) || defined(_XOPEN_SOURCE)
  
  extern int signgam;
  
--- 174,180 ----
  
  #endif	/* __STDC__ */	
  
! #if (__STDC__ && !defined(_POSIX_SOURCE)) || defined(_XOPEN_SOURCE)
  
  extern int signgam;
  
