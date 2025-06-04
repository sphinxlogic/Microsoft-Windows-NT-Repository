# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"






















































































# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]VERSION.H;4"





# 87 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"




















































# 1 "SYS$COMMON:[DECW$INCLUDE]XOS.H;1"








































# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"




































#pragma __nostandard













































































    






	typedef unsigned int __wchar_t;


    typedef unsigned int __id_t;
    typedef unsigned int __size_t;
    typedef unsigned int __u_int;
    typedef unsigned int __uid_t;
    typedef unsigned int __useconds_t;
    typedef unsigned long int __time_t;
    typedef unsigned short __gid_t;
    typedef unsigned short __ino_t;
    typedef unsigned short __mode_t;
    typedef int __nlink_t;
    typedef int __off_t;
    typedef int __pid_t;
    typedef int __ssize_t;
    typedef char *__caddr_t;
    typedef char *__dev_t;
    typedef char *__va_list;
    typedef unsigned int   __in_addr_t;
    typedef unsigned short __in_port_t;
    typedef unsigned char  __sa_family_t;
    typedef unsigned char  __u_char;
    typedef unsigned short __u_short;
    typedef unsigned long  __u_long;
    typedef __wchar_t * __wchar_ptr32;
    typedef const __wchar_t * __const_wchar_ptr32;
    typedef int __wint_t;
    typedef int __wctrans_t;
    typedef int __wctype_t;

    struct _iobuf;
    typedef struct _iobuf *__FILE;
    typedef __FILE * __FILE_ptr32;

    typedef char * __char_ptr32;
    typedef const char * __const_char_ptr32;

    typedef void * __void_ptr32;
    typedef const void * __const_void_ptr32;

    typedef unsigned short * __unsigned_short_ptr32;
    typedef const unsigned short * __const_unsigned_short_ptr32;

    typedef char ** __char_ptr_ptr32;
    typedef char * const *  __char_ptr_const_ptr32;


    typedef __int64 * __int64_ptr32;
    typedef const __int64 * __const_int64_ptr32;

















    typedef char * __char_ptr64;
    typedef const char * __const_char_ptr64;

    typedef char ** __char_ptr_ptr64;
    typedef char * const *  __char_ptr_const_ptr64;

    typedef void * __void_ptr64;
    typedef const void * __const_void_ptr64;

    typedef __size_t * __size_t_ptr64;
    typedef const __size_t * __const_size_t_ptr64;

    typedef __wchar_t * __wchar_ptr64;
    typedef const __wchar_t * __const_wchar_ptr64;







#pragma __standard


# 27 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"
#pragma __nostandard























    typedef __dev_t dev_t;    







    typedef __gid_t gid_t;    







    typedef __ino_t ino_t;    







    typedef __mode_t mode_t;  







    typedef __nlink_t nlink_t;







    typedef __off_t off_t;    







    typedef __pid_t pid_t; 







    typedef __size_t size_t;







    typedef __ssize_t ssize_t;







    typedef __uid_t uid_t;





	 







	typedef __time_t time_t;







	 







	typedef long int  clock_t;    







	typedef long int  key_t;      







	 







	typedef __id_t id_t;







	typedef __useconds_t useconds_t;















        typedef __caddr_t caddr_t;




        typedef unsigned char   u_char;
        typedef unsigned short  u_short;
        typedef unsigned long   u_long;




        typedef __u_int u_int;




	typedef volatile char     v_char;
	typedef volatile short    v_short;
	typedef volatile long     v_long;
	typedef volatile u_char   vu_char;
	typedef volatile u_short  vu_short;
	typedef volatile u_long   vu_long;

	typedef u_char   uchar;   
	typedef u_short  ushort;
	typedef __u_int    uint;
	typedef u_long   ulong;















#pragma __standard



# 41 "SYS$COMMON:[DECW$INCLUDE]XOS.H;1"


























# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard













































void *memchr  (const void *__s, int __c, __size_t __n);
void *memcpy  (void *__s1, __const_void_ptr64 __s2, __size_t __n);
void *memmove (void *__s1, __const_void_ptr64 __s2, __size_t __n);
void *memset  (void *__s, int __c, __size_t __n);
char *strcat  (char *__s1, __const_char_ptr64 __s2);
char *strchr  (const char *__s, int __c);
char *strcpy  (char *__s1, __const_char_ptr64 __s2);
char *strncat (char *__s1, __const_char_ptr64 __s2, __size_t __n);
char *strncpy (char *__s1, __const_char_ptr64 __s2, __size_t __n);
char *strpbrk (const char *__s1, __const_char_ptr64 __s2);
char *strrchr (const char *__s, int __c);
char *strstr  (const char *__s1, __const_char_ptr64 __s2);
char *strtok  (char *__s1, __const_char_ptr64 __s2);





# 122 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"








int    memcmp  (const void *__s1, const void *__s2, __size_t __n);
int    strcmp  (const char *__s1, const char *__s2);
int    strcoll (const char *__s1, const char *__s2);
int    strncmp (const char *__s1, const char *__s2, __size_t __n);
__size_t strcspn (const char *__s1, const char *__s2);
__size_t strlen  (const char *__s);
__size_t strspn  (const char *__s1, const char *__s2);
__size_t strxfrm (char *__s1, const char *__s2, __size_t __n);


__size_t strnlen (const char *__s, __size_t __n);










    __char_ptr32 strerror(int __errnum, ...);




	 







    void *memccpy(void *, __const_void_ptr64, int, __size_t);






	 







    char *strdup(__const_char_ptr64);







	 







    char *strsep(char **, __const_char_ptr64);

























    int  bcmp  (const void * __s1, const void * __s2, __size_t __n);
    void (bcopy) (const void * __s1, void * __s2, __size_t __n);
    void (bzero) (void * __s1, __size_t __n);
    int  ffs   (int);
    int  strcasecmp(const char *, const char *);
    int  strncasecmp(const char *, const char *, __size_t);





    char *index(const char *, int);
    char *rindex(const char *, int);

    char *basename(char *);
    char *dirname(char *);




































    void * __MEMCPY  (void  *__s1, const void *__s2, __size_t __n);
    void * __MEMMOVE (void *__s1, const void *__s2, __size_t __n);
    void * __MEMSET  (void *__s, int __c, __size_t __n);

    __size_t __STRLEN  (const char *__s);
    char * __STRCPY  (char *__s1, const char *__s2);


























# pragma __linkage __str_link1 =( __notused( __r1, __r2, __r3, __r4, __r5, __r6, __r7, __r8, __r9, __r10, __r11, __r12, __r13, __r14, __r15, __r17, __r18, __r19, __r20, __r21, __r22, __r23, __r24, __r25, __r29, __f0, __f1, __f2, __f3, __f4, __f5, __f6, __f7, __f8, __f9, __f10, __f11, __f12, __f13, __f14, __f15, __f16, __f17, __f18, __f19, __f20, __f21, __f22, __f23, __f24, __f25, __f26, __f27, __f28, __f29, __f30) , __notneeded( __ai) ) 
# pragma __linkage __str_link2 =( __notused( __r1, __r2, __r3, __r4, __r5, __r6, __r7, __r8, __r9, __r10, __r11, __r12, __r13, __r14, __r15, __r22, __r23, __r24, __r25, __r29, __f0, __f1, __f2, __f3, __f4, __f5, __f6, __f7, __f8, __f9, __f10, __f11, __f12, __f13, __f14, __f15, __f16, __f17, __f18, __f19, __f20, __f21, __f22, __f23, __f24, __f25, __f26, __f27, __f28, __f29, __f30) , __notneeded( __ai) ) 
# pragma __linkage __str_link3 =( __notused( __r2, __r3, __r4, __r5, __r6, __r7, __r8, __r9, __r10, __r11, __r12, __r13, __r14, __r15, __r18, __r19, __r20, __r21, __r22, __r23, __r24, __r25, __r29, __f0, __f1, __f2, __f3, __f4, __f5, __f6, __f7, __f8, __f9, __f10, __f11, __f12, __f13, __f14, __f15, __f16, __f17, __f18, __f19, __f20, __f21, __f22, __f23, __f24, __f25, __f26, __f27, __f28, __f29, __f30) , __notneeded( __ai) ) 

# pragma __use_linkage __str_link2( strcmp) 
# pragma __use_linkage __str_link3( strnlen) 



















#pragma __standard


# 67 "SYS$COMMON:[DECW$INCLUDE]XOS.H;1"


























# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"

























#pragma __nostandard




	 













	 






#pragma __standard


# 93 "SYS$COMMON:[DECW$INCLUDE]XOS.H;1"






# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"































#pragma __nostandard


















#pragma __member_alignment __save
#pragma __member_alignment





#pragma __message __save
#pragma __message __disable( __MISALGNDSTRCT) 
#pragma __message __disable( __MISALGNDMEM) 






typedef struct tm * __struct_tm_ptr32;


























struct tm {
   int tm_sec;      
   int tm_min;      
   int tm_hour;     
   int tm_mday;     
   int tm_mon;      
   int tm_year;     
   int tm_wday;     
   int tm_yday;     
   int tm_isdst;    



   long tm_gmtoff;  
   char *tm_zone;   


};






    





    



 


# pragma __extern_model __save
# pragma __extern_model __strict_refdef
            extern char *tzname[2];
# pragma __extern_model __restore









 



# pragma __extern_model __save
# pragma __extern_model __strict_refdef
            extern int daylight; 
            extern long int timezone;
# pragma __extern_model __restore







 


    typedef struct tm tm_t;

    

 


        struct timeb {
            __time_t  time;
            unsigned short  millitm;
            short  timezone;
            short  dstflag;
        };




       typedef struct timeb timeb_t;





          struct tms {
             clock_t tms_utime;   
             clock_t tms_stime;   
             clock_t tms_cutime;  
             clock_t tms_cstime;  
       };





       typedef struct tbuffer {
           int proc_user_time;
           int proc_system_time;
           int child_user_time;
           int child_system_time;
       } tbuffer_t;



















typedef struct tm * __struct_tm_ptr64;




 
    clock_t  clock    (void);
    double   difftime (__time_t __time1, __time_t __time0);
    __char_ptr32      asctime (const struct tm *__timeptr);



    __char_ptr32 asctime_r(const struct tm *tm, __char_ptr32 __buf);












# pragma __extern_prefix __save
# pragma __extern_prefix "__utc_"










    __time_t   mktime   (struct tm *__timeptr);
    __time_t   time     (__time_t *__timer);
    __char_ptr32 ctime (const __time_t *__timer);
    __size_t strftime (char *__s, __size_t __maxsize, const char *__format, 
                     const struct tm *__timeptr);








# pragma __extern_prefix __restore













# pragma __extern_prefix __save
# pragma __extern_prefix "__utc_"





    __char_ptr32 ctime_r (const __time_t *__timer, __char_ptr32 __buf);



# pragma __extern_prefix __restore
















# pragma __extern_prefix __save
# pragma __extern_prefix "__utctz_"
















    __struct_tm_ptr32 localtime (const __time_t *__timer); 
    __struct_tm_ptr32 gmtime  (const __time_t *__timer); 








# pragma __extern_prefix __restore













# pragma __extern_prefix __save
# pragma __extern_prefix "__utctz_"














    __struct_tm_ptr32 gmtime_r (const __time_t *__timer, __struct_tm_ptr32 __result); 
    __struct_tm_ptr32 localtime_r (const __time_t *__timer, __struct_tm_ptr32 __result); 



# pragma __extern_prefix __restore







 


        void tzset (void);















       char *strptime (const char *__buf, __const_char_ptr64 __fmt, __struct_tm_ptr64 __tm);










 










# pragma __extern_prefix __save
# pragma __extern_prefix "__utc_"







    int ftime (struct timeb *__timeptr);       








# pragma __extern_prefix __restore





       clock_t times(struct tms *buffer);


































        struct timeval {
            __time_t  tv_sec;      
            long    tv_usec;     
        };                                                                                           


    struct itimerval {
        struct timeval it_interval;  
        struct timeval it_value;     
    };         

    





    













        typedef long fd_mask;







        typedef struct fd_set {                            
            fd_mask fds_bits[(((32)+(( (sizeof(fd_mask) * 8))-1))/( (sizeof(fd_mask) * 8)))];
        } fd_set;






	    void *__MEMSET(void *__s, int __c, __size_t __n);


























      

    int getitimer (int __which, struct itimerval *__value);
    int setitimer (int __which, const struct itimerval *__value, struct itimerval *__ovalue);

        int gettimeofday (struct timeval *__tp, void *__tzp);


    int select (int __nfds, fd_set *__readfds, fd_set *__writefds, fd_set *__exceptfds, 
                struct timeval *__timeout);















#pragma __message __restore
#pragma __member_alignment __restore
#pragma __standard


# 99 "SYS$COMMON:[DECW$INCLUDE]XOS.H;1"
# 145 "SYS$COMMON:[DECW$INCLUDE]XOS.H;1"



# 53 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"


























































































# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard









 


# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"



























#pragma __nostandard
















	 


    

	 












	 


# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard













#pragma __member_alignment __save
#pragma __member_alignment




	 












	 
typedef void	(*__sighnd_t) (int);
typedef void	(*__bsd_sighnd_t)(int, ... );
typedef unsigned int (*__vms_handler) ( void *__sigarr, void *__mecharr );




	typedef struct {unsigned int _set[2];} __sigset_t;





















typedef int	sig_atomic_t;




	 












	    typedef __sigset_t sigset_t;


# pragma __extern_model __save
# pragma __extern_model __common_block noshr
	static const __sigset_t	_SIG_EMPTY_SET	= {0x00000000, 0x00000000},
				_SIG_FULL_SET	= {0xFFFFFFFF, 0xFFFFFFFF};
# pragma __extern_model __restore

	struct sigaction
	{
	    __sighnd_t	sa_handler;
	    __sigset_t	sa_mask;
	    int		sa_flags;
	};







	 











	 


    struct sigstack {
	char	*ss_sp;		
	int	ss_onstack;	
    };












	 

    struct	sigvec
    {
	void 	(*sv_handler)();	
	int	sv_mask;		
	int	sv_onstack;		
    };

    struct	sigcontext
    {
	int	sc_onstack;		
	int	sc_mask;		
	int	sc_sp;			
	int	sc_pc;			
	int	sc_ps;			
    };





	 










	 

























	 








	 



































































__sighnd_t signal( int __sig, __sighnd_t __func );

    


	 

    int raise (int __sig, ...);	 







	 











	int kill (__pid_t __pid, int __sig);








	int sigaddset (__sigset_t * __set, int __sig);
	int sigdelset (__sigset_t * __set, int __sig);
	int sigismember (const __sigset_t * __set, int __sig);

	int sigaction (int __sig, const struct sigaction * __act,
				  struct sigaction * __oact);
	int sigprocmask (int __how, const __sigset_t * __set, 
		    __sigset_t * __oset);
	int sigpending (__sigset_t * __set);
	int sigsuspend (const __sigset_t * __set);












	    int sigemptyset (__sigset_t * __set);
	    int sigfillset  (__sigset_t * __set);












	 


    int	sigpause (int __mask);
    int	sigstack (struct sigstack *__ss, struct sigstack *__oss);









    __bsd_sighnd_t ssignal (int __sig, __bsd_sighnd_t __action);
    int	sigvec (int __sigveca, struct sigvec *__sv, struct sigvec *__osv);
    int	sigblock (int __mask);
    int	sigsetmask (int __mask);
    int	gsignal (int __sig, ...);

    void VAXC$ESTABLISH	(__vms_handler __f);
    void VAXC$CRTL_INIT	(void);


	void DECC$CRTL_INIT (void);



        void c$$establish(__void_ptr32, unsigned short);






    


	 

	 unsigned int alarm  (unsigned int __seconds);
         unsigned int sleep (unsigned __seconds);





    

	 
    int	pause (void);

    


	 

        int sigmask	(int __sig);
















#pragma __member_alignment __restore
#pragma __standard


# 66 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"

# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard








 




#pragma __member_alignment __save
#pragma __member_alignment

























struct rusage {
    struct timeval ru_utime;   
    struct timeval ru_stime;   
};

#pragma __member_alignment __restore




#pragma __standard


# 67 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"
















#pragma __member_alignment __save
#pragma __member_alignment




	 






	 












	 






    

	 



    

	 
    typedef enum {
	P_ALL, P_PID, P_PGID } idtype_t;













	 


    union wait
       {
   	   int     w_status;

	

	 
	struct
	{
	    unsigned        _w_termsig:7;    
	    unsigned        _w_coredump:1;   
	    unsigned        _w_retcode:8;    
	    unsigned short  _w_PAD16;
	} _w_t;
	

	 
	struct {
	    unsigned        _w_stopval:8;    
	    unsigned        _w_stopsig:8;    
	    unsigned short  _w_PAD16;
	} _w_s;
    };







	























__pid_t wait(int *__stat_loc);



























__pid_t waitpid( __pid_t __pid, int *__stat_loc, int __options );




	 







    __pid_t wait3(int *__stat_loc, int options, struct rusage *__resource_usage);






	 

    __pid_t wait4(__pid_t __pid, union wait *__exit_status, int __options, 
                struct rusage *__resource_usage);
















#pragma __member_alignment __restore



















#pragma __standard



# 40 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"












#pragma __member_alignment __save
#pragma __member_alignment


















    typedef __wchar_t wchar_t;


typedef struct {
    int quot, rem;
} div_t;

typedef struct {
    long int quot, rem;
} ldiv_t;




 































# pragma __extern_model __save
# pragma __extern_model __strict_refdef
    extern int decc$$gl___mb_cur_max;
# pragma __extern_model __restore













    int atexit(void(*__func)(void));







       int      putenv  (const char *__string);








    char *setstate(char *);
    char *initstate(unsigned int, char *, int);























    void     abort    (void);
    int      abs      (int __j);
    double   atof     (const char *__nptr);
    int      atoi     (const char *__nptr);
    long int atol     (const char *__nptr);
    div_t    div      (int __numer, int __denom);
    void     free     (void *__ptr);
    long int labs     (long int __j);
    ldiv_t   ldiv     (long int __numer, long int __denom);
    int      mblen    (const char *__s, __size_t __n);
    __size_t mbstowcs (__wchar_t *__pwcs, const char *__s, __size_t __n);
    int      mbtowc   (__wchar_t *__pwc, const char *__s, __size_t __n);
    int      rand     (void);
    void     srand    (unsigned int __seed);
    __size_t wcstombs (char *__s, const __wchar_t *__pwcs, __size_t __n);
    int      wctomb   (char *__s, __wchar_t __wchar);












    void     exit     (int __status);
    int      system   (const char *__string);












       double   drand48 (void);
       double   erand48 (unsigned short int __xsubi[3]);
       long int jrand48 (unsigned short int __xsubi[3]);
       void     lcong48 (unsigned short int __param[7]);
       long int lrand48 (void);
       long int mrand48 (void);
       long int nrand48 (unsigned short int __xsubi[3]);
       void     srand48 (long int __seedval);









       int      mkstemp (char *path); 
       long int random  (void);
       void     srandom (unsigned int);









    int      nice  (int __increment);     
  __mode_t   umask (__mode_t __old_mask); 
    void     cfree (void *__ptr);


       typedef struct { __int64 quot, rem; } __qdiv_t, __lldiv_t;
       __int64   atoll (const char *__nptr);
       __int64   atoq  (const char *__nptr);
       __int64   llabs (__int64 __j);
       __lldiv_t lldiv (__int64 __numer, __int64 __denom);
       __int64   qabs  (__int64 __j);
       __lldiv_t qdiv  (__int64 __numer, __int64 __denom);



       int  setenv   (const char *__name, const char *__value, int __overwrite);
       void unsetenv (const char *__name);












    void    _exit  (int __status);        





















    __char_ptr32 getenv (const char *__name);







       __unsigned_short_ptr32 seed48(unsigned short __seed16v[3]);








    __char_ptr32 ecvt (double __value, int __ndigit, int *__decpt, int *__sign);
    __char_ptr32 fcvt (double __value, int __ndigit, int *__decpt, int *__sign);







    __void_ptr32 brk  (unsigned long int __addr);	
    __void_ptr32 sbrk (long int __incr);		





















    void * calloc  (__size_t __nmemb, __size_t __size);
    void * malloc  (__size_t __size);
    void * realloc (__void_ptr64 __ptr, __size_t __size);
    void * bsearch (const void *, const void *, __size_t, __size_t, int (*__compar)(const void *, const void *));
    void   qsort   (void *, __size_t, __size_t, int (*__compar)(const void *, const void *));
    double strtod  (const char *__nptr, char **__endptr);
    long int strtol (const char *__nptr, char **__endptr, int __base);
    unsigned long int strtoul (const char *__nptr, char **__endptr, int __base);






    char * gcvt   (double __value, int __ndigit, char *__buffer);
    char * mktemp (char *__template);







    __int64 strtoll(const char *__nptr, char **__endptr, int __base);
    __int64 strtoq (const char *__nptr, char **__endptr, int __base);
    unsigned __int64 strtoull(const char *__nptr, char **__endptr, int __base);
    unsigned __int64 strtouq (const char *__nptr, char **__endptr, int __base);
















# 469 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"












    int __ABS (int __j);
    long int __LABS (long int __j);
















#pragma __member_alignment __restore
#pragma __standard



# 143 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"











# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard












	 






































 















































    

 












    

 

















    




    



# pragma __extern_model __save
# pragma __extern_model __strict_refdef
       extern int optind;
       extern int opterr;
       extern int optopt;
       extern char *optarg;
# pragma __extern_model __restore







    











    














    

                                       







    











    




    

 










 


    

 

    


















    












 













 


























 


       int getopt (int __argc, char * const __argv[], const char *__optstring);












                             



 


int  access    (const char *__file_spec, int __mode);
int  close     (int __file_desc);
int  dup2      (int __file_desc1, int __file_desc2);
int  dup       (int __file_desc);
int  execv     (const char *__path, __char_ptr_const_ptr32 __argv);
int  execve    (const char *__path, __char_ptr_const_ptr32 __argv, __char_ptr_const_ptr32 __envp);
int  execvp    (const char *__file, __char_ptr_const_ptr32 __argv);
int  isatty    (int __file_desc);
__off_t lseek  (int __file_desc, __off_t __offset, int __direction);
int  pause     (void);




    int pipe   (int __array_fdscptr[2], ...);


int           rmdir     (const char *__path);

__gid_t getegid (void);
__gid_t getgid  (void);
__pid_t getpid  (void);
__pid_t getppid (void);



# pragma __extern_prefix __save
# pragma __extern_prefix "__posix_"






void _exit     (int status);



# pragma __extern_prefix "__unix_"







__uid_t geteuid (void);
__uid_t getuid  (void);



# pragma __extern_prefix __restore






 




         int gethostname (char *__name, __size_t __namelen);








    __char_ptr32 getlogin(void);





 

    __void_ptr32 sbrk (long int __incr);













char *ctermid (char *__str);


    char *cuserid (char *__str);





    char *getcwd(char *__buffer, unsigned int __size, ...);






















 

    unsigned int alarm (unsigned int __seconds);
    int chown  (const char *__file_spec, __uid_t __owner, __gid_t __group);
    __ssize_t read   (int __file_desc, void *__buffer, __size_t __nbytes);
    __ssize_t write  (int __file_desc, const void *__buffer, __size_t __nbytes);
    int execl  (const char *__path, const char *arg0, ...);
    int execle (const char *__path, const char *arg0, ...);
    int execlp (const char *__path, const char *arg0, ...);
    int setgid  (__gid_t);
    int setuid  (__uid_t);
    unsigned int  sleep     (unsigned __seconds);














    long int fpathconf (int __fd, int __name);
    long int pathconf  (const char * __path, int __name);
    long int sysconf   (int __name);
    int unlink    (const char *__path);


    


 


       __size_t confstr   (int __name, char * __buf, __size_t __len);






 

    int fsync     (int __file_desc);
    int nice      (int __increment);

       void swab (const void * __src, void * __dst, __ssize_t __nbytes);






 


       int ftruncate (int __filedes, __off_t __length );
       int getdtablesize (void);
       int getpagesize (void);
       int truncate  (const char *__path, __off_t __length );
       __useconds_t ualarm (__useconds_t __useconds, __useconds_t __interval);
       int usleep (__useconds_t __useconds);


       __int64_ptr32 decc$$get_vfork_jmpbuf(void);
       int lib$get_current_invo_context(__int64_ptr32 __invo_context);
       int decc$$alloc_vfork_blocks(void);
















 



    int chdir (const char *__dir_spec, ...);














#pragma __standard


# 154 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"




# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"
# 287 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


# 158 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"



# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard














































































































int *CMA$TIS_ERRNO_GET_ADDR     (void);     
int *CMA$TIS_VMSERRNO_GET_ADDR  (void);     
void CMA$TIS_ERRNO_SET_VALUE    (int __value);
void CMA$TIS_VMSERRNO_SET_VALUE (int __value);



























































#pragma __standard


# 161 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"















# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]PROTO.H;1"


































# 176 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]PTYX.H;1"




































































# 1 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"





















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 54 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"



# 1 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"






























































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 63 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"










# 1 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"
































































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 65 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"


















# 1 "SYS$COMMON:[DECW$INCLUDE]X.H;1"











































typedef unsigned long XID;

typedef XID Window;
typedef XID Drawable;
typedef XID Font;
typedef XID Pixmap;
typedef XID Cursor;
typedef XID Colormap;
typedef XID GContext;
typedef XID KeySym;

typedef unsigned long Mask;

typedef unsigned long Atom;

typedef unsigned long VisualID;

typedef unsigned long Time;

typedef unsigned char KeyCode;

































































































































































		       

































































































































































































































































































































































































































































# 83 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"







# 1 "SYS$COMMON:[DECW$INCLUDE]XFUNCPROTO.H;1"

































































































# 90 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]XOSDEFS.H;1"





























































































  






















# 91 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"









# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"




























typedef int ptrdiff_t;



















# 100 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"










typedef char *XPointer;











































































typedef struct _XExtData {
	int number;		
	struct _XExtData *next;	
	int (*free_private)();	
	XPointer private_data;	
} XExtData;




typedef struct {		
	int extension;		
	int major_opcode;	
	int first_event;	
	int first_error;	
} XExtCodes;




typedef struct _XExten {	
	struct _XExten *next;	
	XExtCodes codes;	
	int (*create_GC)();	
	int (*copy_GC)();	
	int (*flush_GC)();	
	int (*free_GC)();	
	int (*create_Font)();	
	int (*free_Font)();	
	int (*close_display)();	
	int (*error)();		
        char *(*error_string)();  
	char *name;		
	void (*error_values)(); 
} _XExtension;





typedef struct {
    int depth;
    int bits_per_pixel;
    int scanline_pad;
} XPixmapFormatValues;





typedef struct {
	int function;		
	unsigned long plane_mask;
	unsigned long foreground;
	unsigned long background;
	int line_width;		
	int line_style;	 	
	int cap_style;	  	

	int join_style;	 	
	int fill_style;	 	

	int fill_rule;	  	
	int arc_mode;		
	Pixmap tile;		
	Pixmap stipple;		
	int ts_x_origin;	
	int ts_y_origin;
        Font font;	        
	int subwindow_mode;     
	int graphics_exposures;
	int clip_x_origin;	
	int clip_y_origin;
	Pixmap clip_mask;	
	int dash_offset;	
	char dashes;
} XGCValues;






typedef struct _XGC










*GC;




typedef struct {
	XExtData *ext_data;	
	VisualID visualid;	



	int class;		

	unsigned long red_mask, green_mask, blue_mask;	
	int bits_per_rgb;	
	int map_entries;	
} Visual;



	
typedef struct {
	int depth;		
	int nvisuals;		
	Visual *visuals;	
} Depth;




struct _XDisplay;
struct _XPrivate;
struct _XrmHashBucketRec;







struct _XDisplay;		

typedef struct {
	XExtData *ext_data;	
	struct _XDisplay *display;
	Window root;		
	int width, height;	
	int mwidth, mheight;	
	int ndepths;		
	Depth *depths;		
	int root_depth;		
	Visual *root_visual;	
	GC default_gc;		
	Colormap cmap;		
	unsigned long white_pixel;
	unsigned long black_pixel;	
	int max_maps, min_maps;	
	int backing_store;	
	int save_unders;	
	long root_input_mask;	
} Screen;




typedef struct {
	XExtData *ext_data;	
	int depth;		
	int bits_per_pixel;	
	int scanline_pad;	
} ScreenFormat;




typedef struct {
    Pixmap background_pixmap;	
    unsigned long background_pixel;	
    Pixmap border_pixmap;	
    unsigned long border_pixel;	
    int bit_gravity;		
    int win_gravity;		
    int backing_store;		
    unsigned long backing_planes;
    unsigned long backing_pixel;
    int save_under;		
    long event_mask;		
    long do_not_propagate_mask;	
    int override_redirect;	
    Colormap colormap;		
    Cursor cursor;		
} XSetWindowAttributes;

typedef struct {
    int x, y;			
    int width, height;		
    int border_width;		
    int depth;          	
    Visual *visual;		
    Window root;        	



    int class;			

    int bit_gravity;		
    int win_gravity;		
    int backing_store;		
    unsigned long backing_planes;
    unsigned long backing_pixel;
    int save_under;		
    Colormap colormap;		
    int map_installed;		
    int map_state;		
    long all_event_masks;	
    long your_event_mask;	
    long do_not_propagate_mask; 
    int override_redirect;	
    Screen *screen;		
} XWindowAttributes;






typedef struct {
	int family;		
	int length;		
	char *address;		
} XHostAddress;




typedef struct _XImage {
    int width, height;		
    int xoffset;		
    int format;			
    char *data;			
    int byte_order;		
    int bitmap_unit;		
    int bitmap_bit_order;	
    int bitmap_pad;		
    int depth;			
    int bytes_per_line;		
    int bits_per_pixel;		
    unsigned long red_mask;	
    unsigned long green_mask;
    unsigned long blue_mask;
    XPointer obdata;		
    struct funcs {		
	struct _XImage *(*create_image)();







	int (*destroy_image)();
	unsigned long (*get_pixel)();
	int (*put_pixel)();
	struct _XImage *(*sub_image)();
	int (*add_pixel)();

	} f;
} XImage;




typedef struct {
    int x, y;
    int width, height;
    int border_width;
    Window sibling;
    int stack_mode;
} XWindowChanges;




typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  
	char pad;
} XColor;






typedef struct {
    short x1, y1, x2, y2;
} XSegment;

typedef struct {
    short x, y;
} XPoint;
    
typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;
    
typedef struct {
    short x, y;
    unsigned short width, height;
    short angle1, angle2;
} XArc;




typedef struct {
        int key_click_percent;
        int bell_percent;
        int bell_pitch;
        int bell_duration;
        int led;
        int led_mode;
        int key;
        int auto_repeat_mode;   
} XKeyboardControl;



typedef struct {
        int key_click_percent;
	int bell_percent;
	unsigned int bell_pitch, bell_duration;
	unsigned long led_mask;
	int global_auto_repeat;
	char auto_repeats[32];
} XKeyboardState;



typedef struct {
        Time time;
	short x, y;
} XTimeCoord;



typedef struct {
 	int max_keypermod;	
 	KeyCode *modifiermap;	
} XModifierKeymap;


typedef struct _AsyncItem {
    struct _AsyncItem *next;
    Window window;
    struct EventsData {
	int (*proc)();



	unsigned long *data;

    } *events;
} _XAsyncItem;


typedef struct _CBdata {
    struct _CBdata *next;
    int (*proc)();
    unsigned long *data;
} _XCBdata;








typedef struct _XDisplay Display;


struct _XPrivate;		
struct _XrmHashBucketRec;

typedef struct 



{
	XExtData *ext_data;	
	struct _XPrivate *private1;
	int fd;			
	int private2;
	int proto_major_version;
	int proto_minor_version;
	char *vendor;		
        XID private3;
	XID private4;
	XID private5;
	int private6;
	XID (*resource_alloc)();
	int byte_order;		
	int bitmap_unit;	
	int bitmap_pad;		
	int bitmap_bit_order;	
	int nformats;		
	ScreenFormat *pixmap_format;	
	int private8;
	int release;		
	struct _XPrivate *private9, *private10;
	int qlen;		
	unsigned long last_request_read; 
	unsigned long request;	
	XPointer private11;
	XPointer private12;
	XPointer private13;
	XPointer private14;
	unsigned max_request_size; 
	struct _XrmHashBucketRec *db;
	int (*private15)();
	char *display_name;	
	int default_screen;	
	int nscreens;		
	Screen *screens;	
	unsigned long motion_buffer;	
	unsigned long private16;
	int min_keycode;	
	int max_keycode;	
	KeySym *private17;
	XModifierKeymap *private18;
	int private19;
	char *xdefaults;
	XPointer private21;
	unsigned long private22;
	int private23;
	_XExtension *ext_procs;	
	int (*private25[128])();  
	int (*private26[128])();

	char *private27;




	struct _XPrivate *private30, *private31;
	KeySym private32;
	struct _XPrivate *private33;
	Font private34;
	struct _XPrivate *private35;
	struct {
	    long private36;
	    int (*private37)();
	    int private38;
	} private39;

	int efn;	

        struct _CBdata *safe;           
                                        
                                        
        unsigned long event_ctr;       

        unsigned int mode_switch;  

	
}



*_XPrivDisplay;








typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	unsigned int state;	
	unsigned int keycode;	
	int same_screen;	
} XKeyEvent;
typedef XKeyEvent XKeyPressedEvent;
typedef XKeyEvent XKeyReleasedEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	unsigned int state;	
	unsigned int button;	
	int same_screen;	
} XButtonEvent;
typedef XButtonEvent XButtonPressedEvent;
typedef XButtonEvent XButtonReleasedEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	unsigned int state;	
	char is_hint;		
	int same_screen;	
} XMotionEvent;
typedef XMotionEvent XPointerMovedEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;	        
	Window root;	        
	Window subwindow;	
	Time time;		
	int x, y;		
	int x_root, y_root;	
	int mode;		
	int detail;
	



	int same_screen;	
	int focus;		
	unsigned int state;	
} XCrossingEvent;
typedef XCrossingEvent XEnterWindowEvent;
typedef XCrossingEvent XLeaveWindowEvent;

typedef struct {
	int type;		
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;		
	int mode;		
	int detail;
	




} XFocusChangeEvent;
typedef XFocusChangeEvent XFocusInEvent;
typedef XFocusChangeEvent XFocusOutEvent;


typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	char key_vector[32];
} XKeymapEvent;	

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	int x, y;
	int width, height;
	int count;		
} XExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Drawable drawable;
	int x, y;
	int width, height;
	int count;		
	int major_code;		
	int minor_code;		
} XGraphicsExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Drawable drawable;
	int major_code;		
	int minor_code;		
} XNoExposeEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	int state;		
} XVisibilityEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;		
	Window window;		
	int x, y;		
	int width, height;	
	int border_width;	
	int override_redirect;	
} XCreateWindowEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
} XDestroyWindowEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int from_configure;
} XUnmapEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int override_redirect;	
} XMapEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;
	Window window;
} XMapRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	Window parent;
	int x, y;
	int override_redirect;
} XReparentEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	int override_redirect;
} XConfigureEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int x, y;
} XGravityEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	int width, height;
} XResizeRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;
	Window window;
	int x, y;
	int width, height;
	int border_width;
	Window above;
	int detail;		
	unsigned long value_mask;
} XConfigureRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window event;
	Window window;
	int place;		
} XCirculateEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window parent;
	Window window;
	int place;		
} XCirculateRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Atom atom;
	Time time;
	int state;		
} XPropertyEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Atom selection;
	Time time;
} XSelectionClearEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window owner;
	Window requestor;
	Atom selection;
	Atom target;
	Atom property;
	Time time;
} XSelectionRequestEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window requestor;
	Atom selection;
	Atom target;
	Atom property;		
	Time time;
} XSelectionEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Colormap colormap;	



	int new;

	int state;		
} XColormapEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;
	Atom message_type;
	int format;
	union {
		char b[20];
		short s[10];
		long l[5];
		} data;
} XClientMessageEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;	
	Window window;		
	int request;		

	int first_keycode;	
	int count;		
} XMappingEvent;

typedef struct {
	int type;
	Display *display;	
	XID resourceid;		
	unsigned long serial;		
	unsigned char error_code;	
	unsigned char request_code;	
	unsigned char minor_code;	
} XErrorEvent;

typedef struct {
	int type;
	unsigned long serial;	
	int send_event;	
	Display *display;
	Window window;	
} XAnyEvent;





typedef union _XEvent {
        int type;		
	XAnyEvent xany;
	XKeyEvent xkey;
	XButtonEvent xbutton;
	XMotionEvent xmotion;
	XCrossingEvent xcrossing;
	XFocusChangeEvent xfocus;
	XExposeEvent xexpose;
	XGraphicsExposeEvent xgraphicsexpose;
	XNoExposeEvent xnoexpose;
	XVisibilityEvent xvisibility;
	XCreateWindowEvent xcreatewindow;
	XDestroyWindowEvent xdestroywindow;
	XUnmapEvent xunmap;
	XMapEvent xmap;
	XMapRequestEvent xmaprequest;
	XReparentEvent xreparent;
	XConfigureEvent xconfigure;
	XGravityEvent xgravity;
	XResizeRequestEvent xresizerequest;
	XConfigureRequestEvent xconfigurerequest;
	XCirculateEvent xcirculate;
	XCirculateRequestEvent xcirculaterequest;
	XPropertyEvent xproperty;
	XSelectionClearEvent xselectionclear;
	XSelectionRequestEvent xselectionrequest;
	XSelectionEvent xselection;
	XColormapEvent xcolormap;
	XClientMessageEvent xclient;
	XMappingEvent xmapping;
	XErrorEvent xerror;
	XKeymapEvent xkeymap;
	long pad[24];
} XEvent;







typedef struct {
    short	lbearing;	
    short	rbearing;	
    short	width;		
    short	ascent;		
    short	descent;	
    unsigned short attributes;	
} XCharStruct;





typedef struct {
    Atom name;
    unsigned long card32;
} XFontProp;

typedef struct {
    XExtData	*ext_data;	
    Font        fid;            
    unsigned	direction;	
    unsigned	min_char_or_byte2;
    unsigned	max_char_or_byte2;
    unsigned	min_byte1;	
    unsigned	max_byte1;	
    int	all_chars_exist;
    unsigned	default_char;	
    int         n_properties;   
    XFontProp	*properties;	
    XCharStruct	min_bounds;	
    XCharStruct	max_bounds;	
    XCharStruct	*per_char;	
    int		ascent;		
    int		descent;	
} XFontStruct;




typedef struct {
    char *chars;		
    int nchars;			
    int delta;			
    Font font;			
} XTextItem;

typedef struct {		
    unsigned char byte1;
    unsigned char byte2;
} XChar2b;

typedef struct {
    XChar2b *chars;		
    int nchars;			
    int delta;			
    Font font;			
} XTextItem16;


typedef union { Display *display;
		GC gc;
		Visual *visual;
		Screen *screen;
		ScreenFormat *pixmap_format;
		XFontStruct *font; } XEDataObject;

typedef struct {
    XRectangle      max_ink_extent;
    XRectangle      max_logical_extent;
} XFontSetExtents;

typedef struct _XFontSet *XFontSet;

typedef struct {
    char           *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XmbTextItem;

typedef struct {
    wchar_t        *chars;
    int             nchars;
    int             delta;
    XFontSet        font_set;
} XwcTextItem;

typedef void (*XIMProc)();

typedef struct _XIM *XIM;
typedef struct _XIC *XIC;

typedef unsigned long XIMStyle;

typedef struct {
    unsigned short count_styles;
    XIMStyle *supported_styles;
} XIMStyles;


















































typedef XPointer XVaNestedList;


typedef struct {
    XPointer client_data;
    XIMProc callback;
} XIMCallback;

typedef unsigned long XIMFeedback;








typedef struct _XIMText {
    unsigned short length;
    XIMFeedback *feedback;
    int encoding_is_wchar; 
    union {
	char *multi_byte;
	wchar_t *wide_char;
    } string; 
} XIMText;

typedef struct _XIMPreeditDrawCallbackStruct {
    int caret;		
    int chg_first;	
    int chg_length;	
    XIMText *text;
} XIMPreeditDrawCallbackStruct;

typedef enum {
    XIMForwardChar, XIMBackwardChar,
    XIMForwardWord, XIMBackwardWord,
    XIMCaretUp, XIMCaretDown,
    XIMNextLine, XIMPreviousLine,
    XIMLineStart, XIMLineEnd, 
    XIMAbsolutePosition,
    XIMDontChange
} XIMCaretDirection;

typedef enum {
    XIMIsInvisible,	 
    XIMIsPrimary,	
    XIMIsSecondary	
} XIMCaretStyle;

typedef struct _XIMPreeditCaretCallbackStruct {
    int position;		 
    XIMCaretDirection direction; 
    XIMCaretStyle style;	 
} XIMPreeditCaretCallbackStruct;

typedef enum {
    XIMTextType,
    XIMBitmapType
} XIMStatusDataType;
	
typedef struct _XIMStatusDrawCallbackStruct {
    XIMStatusDataType type;
    union {
	XIMText *text;
	Pixmap  bitmap;
    } data;
} XIMStatusDrawCallbackStruct;

 

extern XFontStruct *XLoadQueryFont(




);

extern XFontStruct *XQueryFont(




);


extern XTimeCoord *XGetMotionEvents(







);

extern XModifierKeymap *XDeleteModifiermapEntry(









);

extern XModifierKeymap	*XGetModifierMapping(



);

extern XModifierKeymap	*XInsertModifiermapEntry(









);

extern XModifierKeymap *XNewModifiermap(



);

extern XImage *XCreateImage(












);
extern XImage *XGetImage(










);
extern XImage *XGetSubImage(













);




extern Display *XOpenDisplay(



);

extern void XrmInitialize(



);

extern char *XFetchBytes(




);
extern char *XFetchBuffer(





);
extern char *XGetAtomName(




);
extern char *XGetDefault(





);
extern char *XDisplayName(



);
extern char *XKeysymToString(



);

extern int (*XSynchronize(




))();
extern int (*XSetAfterFunction(








))();
extern Atom XInternAtom(





);
extern Colormap XCopyColormapAndFree(




);
extern Colormap XCreateColormap(






);
extern Cursor XCreatePixmapCursor(









);
extern Cursor XCreateGlyphCursor(









);
extern Cursor XCreateFontCursor(




);
extern Font XLoadFont(




);
extern GC XCreateGC(






);
extern GContext XGContextFromGC(



);
extern void XFlushGC(




);
extern Pixmap XCreatePixmap(







);
extern Pixmap XCreateBitmapFromData(







);
extern Pixmap XCreatePixmapFromBitmapData(










);
extern Window XCreateSimpleWindow(











);
extern Window XGetSelectionOwner(




);
extern Window XGetIconWindow();

extern Window XCreateWindow(














); 
extern Colormap *XListInstalledColormaps(





);
extern char **XListFonts(






);
extern char **XListFontsWithInfo(







);
extern char **XGetFontPath(




);
extern char **XListExtensions(




);
extern Atom *XListProperties(





);
extern XHostAddress *XListHosts(





);
extern KeySym XKeycodeToKeysym(









);
extern KeySym XLookupKeysym(




);
extern KeySym *XGetKeyboardMapping(










);
extern KeySym XStringToKeysym(



);
extern long XMaxRequestSize(



);
extern char *XResourceManagerString(



);
extern char *XScreenResourceString(



);
extern unsigned long XDisplayMotionBufferSize(



);
extern VisualID XVisualIDFromVisual(



);








extern XExtCodes *XInitExtension(




);

extern XExtCodes *XAddExtension(



);
extern XExtData *XFindOnExtensionList(




);
extern XExtData **XEHeadOfExtensionList(



);


extern Window XRootWindow(




);
extern Window XDefaultRootWindow(



);
extern Window XRootWindowOfScreen(



);
extern Visual *XDefaultVisual(




);
extern Visual *XDefaultVisualOfScreen(



);
extern GC XDefaultGC(




);
extern GC XDefaultGCOfScreen(



);
extern unsigned long XBlackPixel(




);
extern unsigned long XWhitePixel(




);
extern unsigned long XAllPlanes(



);
extern unsigned long XBlackPixelOfScreen(



);
extern unsigned long XWhitePixelOfScreen(



);
extern unsigned long XNextRequest(



);
extern unsigned long XLastKnownRequestProcessed(



);
extern char *XServerVendor(



);
extern char *XDisplayString(



);
extern Colormap XDefaultColormap(




);
extern Colormap XDefaultColormapOfScreen(



);
extern Display *XDisplayOfScreen(



);
extern Screen *XScreenOfDisplay(




);
extern Screen *XDefaultScreenOfDisplay(



);
extern long XEventMaskOfScreen(



);

extern int XScreenNumberOfScreen(



);

typedef int (*XErrorHandler) (	    




);

extern XErrorHandler XSetErrorHandler (



);


typedef int (*XIOErrorHandler) (    



);

extern XIOErrorHandler XSetIOErrorHandler (



);


extern XPixmapFormatValues *XListPixmapFormats(




);
extern int *XListDepths(





);



extern int XReconfigureWMWindow(







);

extern int XGetWMProtocols(






);
extern int XSetWMProtocols(






);
extern int XIconifyWindow(





);
extern int XWithdrawWindow(





);
extern int XGetCommand(






);
extern int XGetWMColormapWindows(






);
extern int XSetWMColormapWindows(






);
extern void XFreeStringList(



);
extern void XSetTransientForHint(





);



extern XActivateScreenSaver(



);

extern XAddHost(




);

extern XAddHosts(





);

extern XAddToExtensionList(




);

extern XAddToSaveSet(




);

extern int XAllocColor(





);

extern int XAllocColorCells(









);

extern int XAllocColorPlanes(













);

extern int XAllocNamedColor(







);

extern XAllowEvents(





);

extern XAutoRepeatOff(



);

extern XAutoRepeatOn(



);

extern XBell(




);

extern int XBitmapBitOrder(



);

extern int XBitmapPad(



);

extern int XBitmapUnit(



);

extern int XCellsOfScreen(



);

extern XChangeActivePointerGrab(






);

extern XChangeGC(






);

extern XChangeKeyboardControl(





);

extern XChangeKeyboardMapping(







);

extern XChangePointerControl(








);

extern XChangeProperty(










);

extern XChangeSaveSet(





);

extern XChangeWindowAttributes(






);

extern int XCheckIfEvent(












);

extern int XCheckMaskEvent(





);

extern int XCheckTypedEvent(





);

extern int XCheckTypedWindowEvent(






);

extern int XCheckWindowEvent(






);

extern XCirculateSubwindows(





);

extern XCirculateSubwindowsDown(




);

extern XCirculateSubwindowsUp(




);

extern XClearArea(









);

extern XClearWindow(




);

extern XCloseDisplay(



);

extern XConfigureWindow(






);

extern int XConnectionNumber(



);

extern XConvertSelection(








);

extern XCopyArea(












);

extern XCopyGC(






);

extern XCopyPlane(













);

extern int XDefaultDepth(




);

extern int XDefaultDepthOfScreen(



);

extern int XDefaultScreen(



);

extern XDefineCursor(





);

extern XDeleteProperty(





);

extern XDestroyWindow(




);

extern XDestroySubwindows(




);

extern int XDoesBackingStore(



);

extern int XDoesSaveUnders(



);

extern XDisableAccessControl(



);


extern int XDisplayCells(




);

extern int XDisplayHeight(




);

extern int XDisplayHeightMM(




);

extern XDisplayKeycodes(





);

extern int XDisplayPlanes(




);

extern int XDisplayWidth(




);

extern int XDisplayWidthMM(




);

extern XDrawArc(











);

extern XDrawArcs(







);

extern XDrawImageString(









);

extern XDrawImageString16(









);

extern XDrawLine(









);

extern XDrawLines(








);

extern XDrawPoint(







);

extern XDrawPoints(








);

extern XDrawRectangle(









);

extern XDrawRectangles(







);

extern XDrawSegments(







);

extern XDrawString(









);

extern XDrawString16(









);

extern XDrawText(









);

extern XDrawText16(









);

extern XEnableAccessControl(



);

extern int XEventsQueued(




);

extern int XFetchName(





);

extern XFillArc(











);

extern XFillArcs(







);

extern XFillPolygon(









);

extern XFillRectangle(









);

extern XFillRectangles(







);

extern XFlush(



);

extern XForceScreenSaver(




);

extern XFree(



);

extern XFreeColormap(




);

extern XFreeColors(







);

extern XFreeCursor(




);

extern XFreeExtensionList(



);

extern XFreeFont(




);

extern XFreeFontInfo(





);

extern XFreeFontNames(



);

extern XFreeFontPath(



);

extern XFreeGC(




);

extern XFreeModifiermap(



);

extern XFreePixmap(




);

extern int XGeometry(















);

extern XGetErrorDatabaseText(








);

extern XGetErrorText(






);

extern int XGetFontProperty(





);

extern int XGetGCValues(






);

extern int XGetGeometry(











);

extern int XGetIconName(





);

extern XGetInputFocus(





);

extern XGetKeyboardControl(




);

extern XGetPointerControl(






);

extern int XGetPointerMapping(





);

extern XGetScreenSaver(







);

extern int XGetTransientForHint(





);

extern int XGetWindowProperty(














);

extern int XGetWindowAttributes(





);

extern XGrabButton(












);

extern XGrabKey(









);

extern int XGrabKeyboard(








);

extern int XGrabPointer(











);

extern XGrabServer(



);

extern int XHeightMMOfScreen(



);

extern int XHeightOfScreen(



);

extern XIfEvent(












);

extern int XImageByteOrder(



);

extern XInstallColormap(




);

extern KeyCode XKeysymToKeycode(




);

extern XKillClient(




);

extern unsigned long XLastKnownRequestProcessed(



);

extern int XLookupColor(







);

extern XLowerWindow(




);

extern XMapRaised(




);

extern XMapSubwindows(




);

extern XMapWindow(




);

extern XMaskEvent(





);

extern int XMaxCmapsOfScreen(



);

extern int XMinCmapsOfScreen(



);

extern XMoveResizeWindow(








);

extern XMoveWindow(






);

extern XNextEvent(




);

extern XNoOp(



);

extern int XParseColor(






);

extern int XParseGeometry(







);

extern XPeekEvent(




);

extern XPeekIfEvent(












);

extern int XPending(



);

extern int XPlanesOfScreen(




);

extern int XProtocolRevision(



);

extern int XProtocolVersion(



);


extern XPutBackEvent(




);

extern XPutImage(












);

extern int XQLength(



);

extern int XQueryBestCursor(








);

extern int XQueryBestSize(









);

extern int XQueryBestStipple(








);

extern int XQueryBestTile(








);

extern XQueryColor(





);

extern XQueryColors(






);

extern int XQueryExtension(







);

extern XQueryKeymap(




);

extern int XQueryPointer(











);

extern XQueryTextExtents(










);

extern XQueryTextExtents16(










);

extern int XQueryTree(








);

extern XRaiseWindow(




);

extern int XReadBitmapFile(










);

extern XRebindKeysym(








);

extern XRecolorCursor(






);

extern XRefreshKeyboardMapping(



);

extern XRemoveFromSaveSet(




);

extern XRemoveHost(




);

extern XRemoveHosts(





);

extern XReparentWindow(







);

extern XResetScreenSaver(



);

extern XResizeWindow(






);

extern XRestackWindows(





);

extern XRotateBuffers(




);

extern XRotateWindowProperties(







);

extern int XScreenCount(



);

extern XSelectInput(





);

extern int XSendEvent(







);

extern XSetAccessControl(




);

extern XSetArcMode(





);

extern XSetBackground(





);

extern XSetClipMask(





);

extern XSetClipOrigin(






);

extern XSetClipRectangles(









);

extern XSetCloseDownMode(




);

extern XSetCommand(






);

extern XSetDashes(







);

extern XSetFillRule(





);

extern XSetFillStyle(





);

extern XSetFont(





);

extern XSetFontPath(





);

extern XSetForeground(





);

extern XSetFunction(





);

extern XSetGraphicsExposures(





);

extern XSetIconName(





);

extern XSetInputFocus(






);

extern XSetLineAttributes(








);

extern int XSetModifierMapping(




);

extern XSetPlaneMask(





);

extern int XSetPointerMapping(





);

extern XSetScreenSaver(







);

extern XSetSelectionOwner(






);

extern XSetState(








);

extern XSetStipple(





);

extern XSetSubwindowMode(





);

extern XSetTSOrigin(






);

extern XSetTile(





);

extern XSetWindowBackground(





);

extern XSetWindowBackgroundPixmap(





);

extern XSetWindowBorder(





);

extern XSetWindowBorderPixmap(





);

extern XSetWindowBorderWidth(





);

extern XSetWindowColormap(





);

extern XStoreBuffer(






);

extern XStoreBytes(





);

extern XStoreColor(





);

extern XStoreColors(






);

extern XStoreName(





);

extern XStoreNamedColor(







);

extern XSync(




);

extern XTextExtents(









);

extern XTextExtents16(









);

extern int XTextWidth(





);

extern int XTextWidth16(





);

extern int XTranslateCoordinates(










);

extern XUndefineCursor(




);

extern XUngrabButton(






);

extern XUngrabKey(






);

extern XUngrabKeyboard(




);

extern XUngrabPointer(




);

extern XUngrabServer(



);

extern XUninstallColormap(




);

extern XUnloadFont(




);

extern XUnmapSubwindows(




);

extern XUnmapWindow(




);

extern int XVendorRelease(



);

extern XWarpPointer(











);

extern int XWidthMMOfScreen(



);

extern int XWidthOfScreen(



);

extern XWindowEvent(






);

extern int XWriteBitmapFile(









);








int XWMGeometry();
int XGetWMSizeHints(), XGetWMNormalHints();
int XGetRGBColormap();
int XGetTextProperty(), XGetWMName(), XGetWMIconName();
int XGetWMClientMachine();
void XSetWMProperties(), XSetWMSizeHints(), XSetWMNormalHints();
void XSetRGBColormap();
void XSetTextProperty(), XSetWMName(), XSetWMIconName();
void XSetWMClientMachine();
int XStringListToTextProperty(), XTextPropertyToStringList();
int XGetSizeHints();



extern int XSupportsLocale(



);

extern char *XSetLocaleModifiers(



);

extern XFontSet XCreateFontSet(







);

extern void XFreeFontSet(




);

extern int XFontsOfFontSet(





);

extern char *XBaseFontNameListOfFontSet(



);

extern char *XLocaleOfFontSet(



);

extern int XContextDependentDrawing(



);

extern XFontSetExtents *XExtentsOfFontSet(



);

extern int XmbTextEscapement(





);

extern int XwcTextEscapement(





);

extern int XmbTextExtents(







);

extern int XwcTextExtents(







);

extern int XmbTextPerCharExtents(











);

extern int XwcTextPerCharExtents(











);

extern void XmbDrawText(









);

extern void XwcDrawText(









);

extern void XmbDrawString(










);

extern void XwcDrawString(










);

extern void XmbDrawImageString(










);

extern void XwcDrawImageString(










);

extern XIM XOpenIM(






);

extern int XCloseIM(



);

extern char *XGetIMValues(



);

extern Display *XDisplayOfIM(



);

extern char *XLocaleOfIM(



);

extern XIC XCreateIC(



);

extern void XDestroyIC(



);

extern void XSetICFocus(



);

extern void XUnsetICFocus(



);

extern wchar_t *XwcResetIC(



);

extern char *XmbResetIC(



);

extern char *XSetICValues(



);

extern char *XGetICValues(



);

extern XIM XIMOfIC(



);

extern int XFilterEvent(




);

extern int XmbLookupString(








);

extern int XwcLookupString(








);

extern XVaNestedList XVaCreateNestedList(



);

 


# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 4456 "SYS$COMMON:[DECW$INCLUDE]XLIB.H;1"



# 73 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]XUTIL.H;1"






























































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 63 "SYS$COMMON:[DECW$INCLUDE]XUTIL.H;1"




















typedef struct {
    	long flags;	
	int x, y;		
	int width, height;	
	int min_width, min_height;
	int max_width, max_height;
    	int width_inc, height_inc;
	struct {
		int x;	
		int y;	
	} min_aspect, max_aspect;
	int base_width, base_height;		
	int win_gravity;			
} XSizeHints;


























typedef struct {
	long flags;	
	int input;	

	int initial_state;	
	Pixmap icon_pixmap;	
	Window icon_window; 	
	int icon_x, icon_y; 	
	Pixmap icon_mask;	
	XID window_group;	
	
} XWMHints;
























			






typedef struct {
    unsigned char *value;		
    Atom encoding;			
    int format;				
    unsigned long nitems;		
} XTextProperty;

typedef struct {
    long flags;				
    int state;
    Window icon_window;
} XWMState;
    






typedef enum {
    XStringStyle,		
    XCompoundTextStyle,		
    XTextStyle,			
    XStdICCTextStyle		
} XICCEncodingStyle;

typedef struct {
	int min_width, min_height;
	int max_width, max_height;
	int width_inc, height_inc;
} XIconSize;

typedef struct {
	char *res_name;
	char *res_class;
} XClassHint;



















typedef struct _XComposeStatus {
    XPointer compose_ptr;	
    int chars_matched;		
} XComposeStatus;


























typedef struct _XRegion *Region; 


 



 






typedef struct {
  Visual *visual;
  VisualID visualid;
  int screen;
  int depth;



  int class;

  unsigned long red_mask;
  unsigned long green_mask;
  unsigned long blue_mask;
  int colormap_size;
  int bits_per_rgb;
} XVisualInfo;

















typedef struct {
	Colormap colormap;
	unsigned long red_max;
	unsigned long red_mult;
	unsigned long green_max;
	unsigned long green_mult;
	unsigned long blue_max;
	unsigned long blue_mult;
	unsigned long base_pixel;
	VisualID visualid;		
	XID killid;			
} XStandardColormap;

























typedef int XContext;




 
extern XContext  XUniqueContext();



extern XClassHint *XAllocClassHint (



);

extern XIconSize *XAllocIconSize (



);

extern XSizeHints *XAllocSizeHints (



);

extern XStandardColormap *XAllocStandardColormap (



);

extern XWMHints *XAllocWMHints (



);

extern XClipBox(




);

extern Region XCreateRegion(



);

extern char *XDefaultString(



);

extern int XDeleteContext(





);

extern XDestroyRegion(



);

extern XEmptyRegion(



);

extern XEqualRegion(




);

extern int XFindContext(






);

extern int XGetClassHint(





);

extern int XGetIconSizes(






);

extern int XGetNormalHints(





);

extern int XGetRGBColormaps(







);

extern int XGetSizeHints(






);

extern int XGetStandardColormap(






);

extern int XGetTextProperty(






);

extern XVisualInfo *XGetVisualInfo(






);

extern int XGetWMClientMachine(





);

extern XWMHints *XGetWMHints(




);

extern int XGetWMIconName(





);

extern int XGetWMName(





);

extern int XGetWMNormalHints(






);

extern int XGetWMSizeHints(







);

extern int XGetZoomHints(





);

extern XIntersectRegion(





);

extern int XLookupString(







);

extern int XMatchVisualInfo(







);

extern XOffsetRegion(





);

extern int XPointInRegion(





);

extern Region XPolygonRegion(





);

extern int XRectInRegion(







);

extern int XSaveContext(






);

extern XSetClassHint(





);

extern XSetIconSizes(






);

extern XSetNormalHints(





);

extern void XSetRGBColormaps(







);

extern XSetSizeHints(






);

extern XSetStandardProperties(










);

extern void XSetTextProperty(






);

extern void XSetWMClientMachine(





);

extern XSetWMHints(





);

extern void XSetWMIconName(





);

extern void XSetWMName(





);

extern void XSetWMNormalHints(





);

extern void XSetWMProperties(











);

extern void XmbSetWMProperties(











);

extern void XSetWMSizeHints(






);

extern XSetRegion(





);

extern void XSetStandardColormap(






);

extern XSetZoomHints(





);

extern XShrinkRegion(





);

extern int XStringListToTextProperty(





);

extern XSubtractRegion(





);

extern int XmbTextListToTextProperty(







);

extern int XwcTextListToTextProperty(







);

extern void XwcFreeStringList(



);

extern int XTextPropertyToStringList(





);

extern int XmbTextPropertyToTextList(






);

extern int XwcTextPropertyToTextList(






);

extern XUnionRectWithRegion(





);

extern XUnionRegion(





);

extern int XWMGeometry(













);

extern XXorRegion(





);

 





XSizeHints *XAllocSizeHints ();
XStandardColormap *XAllocStandardColormap ();
XWMHints *XAllocWMHints ();
XClassHint *XAllocClassHint ();
XIconSize *XAllocIconSize ();
XWMState *XAllocWMState ();


# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 928 "SYS$COMMON:[DECW$INCLUDE]XUTIL.H;1"


# 74 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]XRESOURCE.H;1"































































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 64 "SYS$COMMON:[DECW$INCLUDE]XRESOURCE.H;1"












 


















extern char *Xpermalloc(



);







typedef int     XrmQuark, *XrmQuarkList;


typedef char *XrmString;



extern XrmQuark XrmStringToQuark(



);

extern XrmQuark XrmPermStringToQuark(



);


extern XrmString XrmQuarkToString(



);

extern XrmQuark XrmUniqueQuark(



);










typedef enum {XrmBindTightly, XrmBindLoosely} XrmBinding, *XrmBindingList;

extern void XrmStringToQuarkList(




);

extern void XrmStringToBindingQuarkList(





);







typedef XrmQuark     XrmName;
typedef XrmQuarkList XrmNameList;




typedef XrmQuark     XrmClass;
typedef XrmQuarkList XrmClassList;











typedef XrmQuark     XrmRepresentation;



typedef struct {
    unsigned int    size;
    XPointer	    addr;
} XrmValue, *XrmValuePtr;








typedef struct _XrmHashBucketRec *XrmHashBucket;
typedef XrmHashBucket *XrmHashTable;
typedef XrmHashTable XrmSearchList[];
typedef struct _XrmHashBucketRec *XrmDatabase;


extern void XrmDestroyDatabase(



);

extern void XrmQPutResource(







);

extern void XrmPutResource(






);

extern void XrmQPutStringResource(






);

extern void XrmPutStringResource(





);

extern void XrmPutLineResource(




);

extern  XrmQGetResource(







);

extern int XrmGetResource(







);

extern int XrmQGetSearchList(







);

extern int XrmQGetSearchResource(







);







extern void XrmSetDatabase(




);

extern XrmDatabase XrmGetDatabase(



);

extern XrmDatabase XrmGetFileDatabase(



);

extern int XrmCombineFileDatabase(





);

extern XrmDatabase XrmGetStringDatabase(



);

extern void XrmPutFileDatabase(




);

extern void XrmMergeDatabases(




);

extern void XrmCombineDatabase(





);




extern int XrmEnumerateDatabase(

















);

extern char *XrmLocaleOfDatabase(



);








typedef enum {
    XrmoptionNoArg,	
    XrmoptionIsArg,     
    XrmoptionStickyArg, 
    XrmoptionSepArg,    
    XrmoptionResArg,	
    XrmoptionSkipArg,   
    XrmoptionSkipLine,  
    XrmoptionSkipNArgs	

} XrmOptionKind;

typedef struct {
    char	    *option;	    
    char	    *specifier;     
    XrmOptionKind   argKind;	    
    XPointer	    value;	    
} XrmOptionDescRec, *XrmOptionDescList;


extern void XrmParseCommand(








);

 


# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 432 "SYS$COMMON:[DECW$INCLUDE]XRESOURCE.H;1"




# 75 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

























typedef char *String;



















































typedef struct _WidgetRec *Widget;
typedef Widget *WidgetList;
typedef struct _WidgetClassRec *WidgetClass;
typedef struct _CompositeRec *CompositeWidget;
typedef struct _XtActionsRec *XtActionList;
typedef struct _XtEventRec *XtEventTable;

    




typedef struct _XtAppStruct *XtAppContext;
typedef unsigned long	XtValueMask;
typedef unsigned long	XtIntervalId;
typedef unsigned long	XtInputId;
typedef unsigned long	XtWorkProcId;
typedef unsigned int	XtGeometryMask;
typedef unsigned long	XtGCMask;   
typedef unsigned long	Pixel;	    
typedef int		XtCacheType;































typedef char		Boolean;
typedef long		XtArgVal;
typedef unsigned char	XtEnum;


typedef unsigned int	Cardinal;
typedef unsigned short	Dimension;  
typedef short		Position;   




typedef char*		XtPointer;




typedef XtPointer	Opaque;


# 1 "SYS$COMMON:[DECW$INCLUDE]CORE.H;1"



















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 52 "SYS$COMMON:[DECW$INCLUDE]CORE.H;1"


typedef struct _WidgetClassRec *CoreWidgetClass;
typedef struct _WidgetRec *CoreWidget;
extern WidgetClass coreWidgetClass;


extern WidgetClass widgetClass;





# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 65 "SYS$COMMON:[DECW$INCLUDE]CORE.H;1"




# 224 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]COMPOSITE.H;1"
















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 49 "SYS$COMMON:[DECW$INCLUDE]COMPOSITE.H;1"


typedef struct _CompositeClassRec *CompositeWidgetClass;

typedef Cardinal (*XtOrderProc)(



);

 

extern void XtManageChildren(




);

extern void XtManageChild(



);

extern void XtUnmanageChildren(




);

extern void XtUnmanageChild(



);

 


extern WidgetClass compositeWidgetClass;




# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 95 "SYS$COMMON:[DECW$INCLUDE]COMPOSITE.H;1"




# 225 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]CONSTRAINT.H;1"
















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 49 "SYS$COMMON:[DECW$INCLUDE]CONSTRAINT.H;1"


typedef struct _ConstraintClassRec *ConstraintWidgetClass;


extern WidgetClass constraintWidgetClass;




# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 59 "SYS$COMMON:[DECW$INCLUDE]CONSTRAINT.H;1"




# 226 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]OBJECT.H;1"
















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 49 "SYS$COMMON:[DECW$INCLUDE]OBJECT.H;1"


typedef struct _ObjectRec *Object;
typedef struct _ObjectClassRec *ObjectClass;


extern WidgetClass objectClass;



# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 59 "SYS$COMMON:[DECW$INCLUDE]OBJECT.H;1"




# 227 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]RECTOBJ.H;1"
















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 49 "SYS$COMMON:[DECW$INCLUDE]RECTOBJ.H;1"


typedef struct _RectObjRec *RectObj;
typedef struct _RectObjClassRec *RectObjClass;


extern WidgetClass rectObjClass;



# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 59 "SYS$COMMON:[DECW$INCLUDE]RECTOBJ.H;1"




# 228 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"

typedef struct _TranslationData *XtTranslations;
typedef struct _TranslationData *XtAccelerators;
typedef unsigned int Modifiers;

typedef void (*XtActionProc)(






);

typedef XtActionProc* XtBoundActions;

typedef struct _XtActionsRec{
    String	 string;
    XtActionProc proc;
} XtActionsRec;

typedef enum {


    XtAddress,		
    XtBaseOffset,	
    XtImmediate,	
    XtResourceString,	
    XtResourceQuark,	
    XtWidgetBaseOffset,	
    XtProcedureArg	
} XtAddressMode;

typedef struct {
    XtAddressMode   address_mode;
    XtPointer	    address_id;
    Cardinal	    size;
} XtConvertArgRec, *XtConvertArgList;

typedef void (*XtConvertArgProc)(





);

typedef struct {
    XtGeometryMask request_mode;
    Position x, y;
    Dimension width, height, border_width;
    Widget sibling;
    int stack_mode;   
} XtWidgetGeometry;







typedef void (*XtConverter)( 






);

typedef Boolean (*XtTypeConverter)(








);

typedef void (*XtDestructor)(







);

typedef Opaque XtCacheRef;

typedef Opaque XtActionHookId;

typedef void (*XtActionHookProc)(








);

typedef void (*XtKeyProc)(







);

typedef void (*XtCaseProc)(






);

typedef void (*XtEventHandler)(






);
typedef unsigned long EventMask;

typedef enum {XtListHead, XtListTail } XtListPosition;

typedef unsigned long	XtInputMask;





typedef void (*XtTimerCallbackProc)(




);

typedef void (*XtInputCallbackProc)(





);

typedef struct {
    String	name;
    XtArgVal	value;
} Arg, *ArgList;

typedef XtPointer	XtVarArgsList;

typedef void (*XtCallbackProc)(





);

typedef struct _XtCallbackRec {
    XtCallbackProc  callback;
    XtPointer	    closure;
} XtCallbackRec, *XtCallbackList;

typedef enum {
	XtCallbackNoList,
	XtCallbackHasNone,
	XtCallbackHasSome
} XtCallbackStatus;

typedef enum  {
    XtGeometryYes,	  
    XtGeometryNo,	  
    XtGeometryAlmost,	  
    XtGeometryDone	  
} XtGeometryResult;

typedef enum {XtGrabNone, XtGrabNonexclusive, XtGrabExclusive} XtGrabKind;

typedef struct {
    Widget  shell_widget;
    Widget  enable_widget;
} XtPopdownIDRec, *XtPopdownID;

typedef struct _XtResource {
    String	resource_name;	
    String	resource_class;	
    String	resource_type;	
    Cardinal	resource_size;	
    Cardinal	resource_offset;
    String	default_type;	
    XtPointer	default_addr;	
} XtResource, *XtResourceList;

typedef void (*XtResourceDefaultProc)(





);

typedef String (*XtLanguageProc)(





);

typedef void (*XtErrorMsgHandler)(








);

typedef void (*XtErrorHandler)(



);

typedef void (*XtCreatePopupChildProc)(



);

typedef Boolean (*XtWorkProc)(



);

typedef struct {
    char match;
    String substitution;
} SubstitutionRec, *Substitution;

typedef Boolean (*XtFilePredicate)(



);

typedef XtPointer XtRequestId;

typedef Boolean (*XtConvertSelectionProc)(









);

typedef void (*XtLoseSelectionProc)(




);

typedef void (*XtSelectionDoneProc)(





);

typedef void (*XtSelectionCallbackProc)(









);

typedef void (*XtLoseSelectionIncrProc)(





);

typedef void (*XtSelectionDoneIncrProc)(







);

typedef Boolean (*XtConvertSelectionIncrProc)(












);

typedef void (*XtCancelConvertSelectionProc)(







);







 

extern Boolean XtConvertAndStore(







);

extern Boolean XtCallConverter(









);

extern Boolean XtDispatchEvent(



);

extern Boolean XtCallAcceptFocus(




);

extern Boolean XtPeekEvent( 



);

extern Boolean XtAppPeekEvent(




);

extern Boolean XtIsSubclass(




);

extern Boolean XtIsObject(



);

extern Boolean _XtCheckSubclassFlag( 




);

extern Boolean _XtIsSubclassOf( 






);

extern Boolean XtIsManaged(



);

extern Boolean XtIsRealized(



);

extern Boolean XtIsSensitive(



);

extern Boolean XtOwnSelection(








);

extern Boolean XtOwnSelectionIncremental(










);

extern XtGeometryResult XtMakeResizeRequest(







);

extern void XtTranslateCoords(







);

extern KeySym* XtGetKeysymTable(





);

extern void XtKeysymToKeycodeList(






);

extern void XtStringConversionWarning( 




);





extern void XtDisplayStringConvWarning(





);





extern XtConvertArgRec colorConvertArgs[];
extern XtConvertArgRec screenConvertArg[];


extern void XtAppAddConverter( 








);

extern void XtAddConverter( 







);

extern void XtSetTypeConverter(









);

extern void XtAppSetTypeConverter(










);

extern void XtConvert( 







);

extern void XtDirectConvert( 







);







extern XtTranslations XtParseTranslationTable(



);

extern XtAccelerators XtParseAcceleratorTable(



);

extern void XtOverrideTranslations(




);

extern void XtAugmentTranslations(




);

extern void XtInstallAccelerators(




);

extern void XtInstallAllAccelerators(




);

extern void XtUninstallTranslations(



);

extern void XtAppAddActions(





);

extern void XtAddActions( 




);

extern XtActionHookId XtAppAddActionHook(





);

extern void XtRemoveActionHook(



);

extern void XtGetActionList(





);

extern void XtCallActionProc(







);

extern void XtRegisterGrabAction(







);

extern void XtSetMultiClickTime(




);

extern int XtGetMultiClickTime(



);

extern KeySym XtGetActionKeysym(




);







extern void XtTranslateKeycode(







);

extern void XtTranslateKey(







);

extern void XtSetKeyTranslator(




);

extern void XtRegisterCaseConverter(






);

extern void XtConvertCase(






);












extern void XtAddEventHandler(







);

extern void XtRemoveEventHandler(







);

extern void XtAddRawEventHandler(







);

extern void XtRemoveRawEventHandler(







);

extern void XtInsertEventHandler(








);

extern void XtInsertRawEventHandler(








);

extern EventMask XtBuildEventMask(



);

extern void XtAddGrab(





);

extern void XtRemoveGrab(



);

extern void XtProcessEvent( 



);

extern void XtAppProcessEvent(




);

extern void XtMainLoop( 



);

extern void XtAppMainLoop(



);

extern void XtAddExposureToRegion(




);

extern void XtSetKeyboardFocus(




);

extern Time XtLastTimestampProcessed(



);







extern XtIntervalId XtAddTimeOut( 





);

extern XtIntervalId XtAppAddTimeOut(






);

extern void XtRemoveTimeOut(



);

extern XtInputId XtAddInput( 






);

extern XtInputId XtAppAddInput(







);

extern void XtRemoveInput(



);

extern void XtNextEvent( 



);

extern void XtAppNextEvent(




);






extern XtInputMask XtPending( 



);

extern XtInputMask XtAppPending(



);



























extern void XtRealizeWidget(



);

void XtUnrealizeWidget(



);

extern void XtDestroyWidget(



);

extern void XtSetSensitive(




);

extern void XtSetMappedWhenManaged(




);

extern Widget XtNameToWidget(




);

extern Widget XtWindowToWidget(




);











extern ArgList XtMergeArgLists(






);










extern XtVarArgsList XtVaCreateArgsList(



);











extern Display *XtDisplay(



);

extern Display *XtDisplayOfObject(



);

extern Screen *XtScreen(



);

extern Screen *XtScreenOfObject(



);

extern Window XtWindow(



);

extern Window XtWindowOfObject(



);

extern String XtName(



);

extern WidgetClass XtSuperclass(



);

extern WidgetClass XtClass(



);

extern Widget XtParent(



);







extern void XtAddCallback(






);

extern void XtRemoveCallback(






);

extern void XtAddCallbacks(





);

extern void XtRemoveCallbacks(





);

extern void XtRemoveAllCallbacks(




);


extern void XtCallCallbacks(





);

extern void XtCallCallbackList(





);

extern XtCallbackStatus XtHasCallbacks(




);








extern XtGeometryResult XtMakeGeometryRequest(





);

extern XtGeometryResult XtQueryGeometry(





);

extern Widget XtCreatePopupShell(







);

extern Widget XtVaCreatePopupShell(






);

extern void XtPopup(




);

extern void XtPopupSpringLoaded(



);

extern void XtCallbackNone(





);

extern void XtCallbackNonexclusive(





);

extern void XtCallbackExclusive(





);

extern void XtPopdown(



);

extern void XtCallbackPopdown(





);

extern void XtMenuPopupAction(






);

extern Widget XtCreateWidget(







);

extern Widget XtCreateManagedWidget(







);

extern Widget XtVaCreateWidget(






);

extern Widget XtVaCreateManagedWidget(






);

extern Widget XtCreateApplicationShell( 






);

extern Widget XtAppCreateShell(








);

extern Widget XtVaAppCreateShell(







);







extern void XtToolkitInitialize(



);

extern XtLanguageProc XtSetLanguageProc(





);

extern void XtDisplayInitialize(










);

extern Widget XtAppInitialize(











);

extern Widget XtVaAppInitialize(










);

extern Widget XtInitialize( 








);

extern Display *XtOpenDisplay(










);

extern XtAppContext XtCreateApplicationContext(



);

extern void XtAppSetFallbackResources(




);

extern void XtDestroyApplicationContext(



);

extern void XtInitializeWidgetClass(



);

extern XtAppContext XtWidgetToApplicationContext(



);

extern XtAppContext XtDisplayToApplicationContext(



);

extern XrmDatabase XtDatabase(



);

extern XrmDatabase XtScreenDatabase(



);

extern void XtCloseDisplay(



);

extern void XtGetApplicationResources(








);

extern void XtVaGetApplicationResources(







);

extern void XtGetSubresources(










);

extern void XtVaGetSubresources(









);

extern void XtSetValues(





);

extern void XtVaSetValues(




);

extern void XtGetValues(





);

extern void XtVaGetValues(




);

extern void XtSetSubvalues(







);

extern void XtVaSetSubvalues(






);

extern void XtGetSubvalues(







);

extern void XtVaGetSubvalues(






);

extern void XtGetResourceList(





);

extern void XtGetConstraintResourceList(





);











































extern XtErrorMsgHandler XtAppSetErrorMsgHandler(




);

extern void XtSetErrorMsgHandler( 



);

extern XtErrorMsgHandler XtAppSetWarningMsgHandler(




);

extern void XtSetWarningMsgHandler( 



);

extern void XtAppErrorMsg(









);

extern void XtErrorMsg( 








);

extern void XtAppWarningMsg(









);

extern void XtWarningMsg( 








);

extern XtErrorHandler XtAppSetErrorHandler(




);

extern void XtSetErrorHandler( 



);

extern XtErrorHandler XtAppSetWarningHandler(




);

extern void XtSetWarningHandler( 



);

extern void XtAppError(




);

extern void XtError( 



);

extern void XtAppWarning(




);

extern void XtWarning( 



);

extern XrmDatabase *XtAppGetErrorDatabase(



);

extern XrmDatabase *XtGetErrorDatabase( 



);

extern void XtAppGetErrorDatabaseText(










);

extern void XtGetErrorDatabaseText( 








);







extern char *XtMalloc(



);

extern char *XtCalloc(




);

extern char *XtRealloc(




);


extern void XtFree(



);


# 2196 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"












extern XtWorkProcId XtAddWorkProc( 




);

extern XtWorkProcId XtAppAddWorkProc(





);

extern void  XtRemoveWorkProc(



);







extern GC XtGetGC(





);

extern GC XtAllocateGC(








);





extern void XtDestroyGC( 



);

extern void XtReleaseGC(




);



extern void XtAppReleaseCacheRefs(




);

extern void XtCallbackReleaseCacheRef(





);

extern void XtCallbackReleaseCacheRefList(





);

extern void XtSetWMColormapWindows(





);

extern String XtFindFile(






);

extern String XtResolvePathname(










);








    
extern void XtDisownSelection(





);

extern void XtGetSelectionValue(








);

extern void XtGetSelectionValues(









);

extern void XtAppSetSelectionTimeout(




);

extern void XtSetSelectionTimeout( 



);

extern unsigned long XtAppGetSelectionTimeout(



);

extern unsigned long XtGetSelectionTimeout( 



);

extern XSelectionRequestEvent *XtGetSelectionRequest(





);

extern void XtGetSelectionValueIncremental(








);

extern void XtGetSelectionValuesIncremental(









);

extern void XtGrabKey(








);

extern void XtUngrabKey(





);

extern int XtGrabKeyboard(







);

extern void XtUngrabKeyboard(




);

extern void XtGrabButton(











);

extern void XtUngrabButton(





);

extern int XtGrabPointer(










);

extern void XtUngrabPointer(




);

extern void XtGetApplicationNameAndClass(





);









extern Boolean XtCvtStringToAcceleratorTable(








);

extern Boolean XtCvtStringToAtom(








);

extern Boolean XtCvtStringToBoolean(








);

extern Boolean XtCvtStringToBool(








);

extern Boolean XtCvtStringToCursor(








);

extern Boolean XtCvtStringToDimension(








);

extern Boolean XtCvtStringToDisplay(








);

extern Boolean XtCvtStringToFile(








);

extern Boolean XtCvtStringToFloat(








);

extern Boolean XtCvtStringToFont(








);

extern Boolean XtCvtStringToFontSet(








);

extern Boolean XtCvtStringToFontStruct(








);

extern Boolean XtCvtStringToInt(








);

extern Boolean XtCvtStringToInitialState(








);

extern Boolean XtCvtStringToPixel(








);



extern Boolean XtCvtStringToShort(








);

extern Boolean XtCvtStringToTranslationTable(








);

extern Boolean XtCvtStringToUnsignedChar(








);

extern Boolean XtCvtStringToVisual(








);



extern Boolean XtCvtIntToBoolean(








);

extern Boolean XtCvtIntToBool(








);

extern Boolean XtCvtIntToColor(








);



extern Boolean XtCvtIntToFloat(








);

extern Boolean XtCvtIntToFont(








);

extern Boolean XtCvtIntToPixel(








);

extern Boolean XtCvtIntToPixmap(








);



extern Boolean XtCvtIntToShort(








);

extern Boolean XtCvtIntToUnsignedChar(








);



extern Boolean XtCvtColorToPixel(








);






 



# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 2853 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"




# 57 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"


typedef struct _vms_time {
     unsigned long low;
     unsigned long high;
}vms_time;


typedef struct {
    XrmQuark	xrm_name;	  
    XrmQuark	xrm_class;	  
    XrmQuark	xrm_type;	  
    Cardinal	xrm_size;	  

    int		xrm_offset;	  



    XrmQuark	xrm_default_type; 
    XtPointer	xrm_default_addr; 
} XrmResource, *XrmResourceList;

typedef unsigned long XtVersionType;








typedef void (*XtProc)(



);

typedef void (*XtWidgetClassProc)(



);

typedef void (*XtWidgetProc)(



);

typedef Boolean (*XtAcceptFocusProc)(




);

typedef void (*XtArgsProc)(





);

typedef void (*XtInitProc)(






);

typedef Boolean (*XtSetValuesFunc)(







);

typedef Boolean (*XtArgsFunc)(





);

typedef void (*XtAlmostProc)(






);

typedef void (*XtExposeProc)(





);













typedef void (*XtRealizeProc)(





);

typedef XtGeometryResult (*XtGeometryHandler)(





);

typedef void (*XtStringProc)(




);



struct _XtStateRec;

typedef struct _XtTMRec {
    XtTranslations  translations;	
    XtBoundActions  proc_table;		
    struct _XtStateRec *current_state;  
    unsigned long   lastEventTime;
} XtTMRec, *XtTM;


# 1 "SYS$COMMON:[DECW$INCLUDE]COREP.H;1"



















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 52 "SYS$COMMON:[DECW$INCLUDE]COREP.H;1"




extern int _XtInheritTranslations;
















typedef struct _CorePart {
    Widget	    self;		
    WidgetClass	    widget_class;	
    Widget	    parent;		
    XrmName         xrm_name;		
    Boolean         being_destroyed;	
    XtCallbackList  destroy_callbacks;	
    XtPointer       constraints;        
    Position        x, y;		
    Dimension       width, height;	
    Dimension       border_width;	
    Boolean         managed;            
    Boolean	    sensitive;		
    Boolean         ancestor_sensitive;	
    XtEventTable    event_table;	
    XtTMRec	    tm;                 
    XtTranslations  accelerators;       
    Pixel	    border_pixel;	
    Pixmap          border_pixmap;	
    WidgetList      popup_list;         
    Cardinal        num_popups;         
    String          name;		
    Screen	    *screen;		
    Colormap        colormap;           
    Window	    window;		
    Cardinal        depth;		
    Pixel	    background_pixel;	
    Pixmap          background_pixmap;	
    Boolean         visible;		
    Boolean	    mapped_when_managed;
} CorePart;

typedef struct _WidgetRec {
    CorePart    core;
 } WidgetRec, CoreRec;


















typedef struct _CoreClassPart {
    WidgetClass     superclass;		
    String          class_name;		
    Cardinal        widget_size;	
    XtProc	    class_initialize;   
    XtWidgetClassProc class_part_initialize; 
    XtEnum          class_inited;       
    XtInitProc      initialize;		
    XtArgsProc      initialize_hook;    
    XtRealizeProc   realize;		
    XtActionList    actions;		
    Cardinal	    num_actions;	
    XtResourceList  resources;		
    Cardinal        num_resources;      
    XrmClass        xrm_class;		
    Boolean         compress_motion;    
    XtEnum          compress_exposure;  
    Boolean         compress_enterleave;
    Boolean         visible_interest;   
    XtWidgetProc    destroy;		
    XtWidgetProc    resize;		
    XtExposeProc    expose;		
    XtSetValuesFunc set_values;		
    XtArgsFunc      set_values_hook;    
    XtAlmostProc    set_values_almost;  
    XtArgsProc      get_values_hook;    
    XtAcceptFocusProc accept_focus;     
    XtVersionType   version;	        
    XtPointer       callback_private;   
    String          tm_table;           
    XtGeometryHandler query_geometry;	
    XtStringProc    display_accelerator;
    XtPointer	    extension;		
 } CoreClassPart;

typedef struct _WidgetClassRec {
    CoreClassPart core_class;
} WidgetClassRec, CoreClassRec;

extern WidgetClassRec widgetClassRec;




# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 170 "SYS$COMMON:[DECW$INCLUDE]COREP.H;1"




# 212 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]COMPOSITEP.H;1"
















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 49 "SYS$COMMON:[DECW$INCLUDE]COMPOSITEP.H;1"










typedef struct _CompositePart {
    WidgetList  children;	     
    Cardinal    num_children;	     
    Cardinal    num_slots;           
    XtOrderProc insert_position;     
} CompositePart,*CompositePtr;

typedef struct _CompositeRec {
    CorePart      core;
    CompositePart composite;
} CompositeRec;







typedef struct _CompositeClassPart {
    XtGeometryHandler geometry_manager;	  
    XtWidgetProc      change_managed;	  
    XtWidgetProc      insert_child;	  
    XtWidgetProc      delete_child;	  
    XtPointer	      extension;	  
} CompositeClassPart,*CompositePartPtr;

typedef struct {
    XtPointer next_extension;	
    XrmQuark record_type;	
    long version;		
    Cardinal record_size;	
    Boolean accepts_objects;
} CompositeClassExtensionRec, *CompositeClassExtension;


typedef struct _CompositeClassRec {
     CoreClassPart      core_class;
     CompositeClassPart composite_class;
} CompositeClassRec;

extern CompositeClassRec compositeClassRec;









# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 109 "SYS$COMMON:[DECW$INCLUDE]COMPOSITEP.H;1"




# 213 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]CONSTRAINP.H;1"
















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 49 "SYS$COMMON:[DECW$INCLUDE]CONSTRAINP.H;1"




typedef struct _ConstraintPart {
    XtPointer   mumble;		
} ConstraintPart;

typedef struct _ConstraintRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart  constraint;
} ConstraintRec, *ConstraintWidget;

typedef struct _ConstraintClassPart {
    XtResourceList resources;	      
    Cardinal   num_resources;         
    Cardinal   constraint_size;       
    XtInitProc initialize;            
    XtWidgetProc destroy;             
    XtSetValuesFunc set_values;       
    XtPointer	    extension;		
} ConstraintClassPart;

typedef struct {
    XtPointer next_extension;	
    XrmQuark record_type;	
    long version;		
    Cardinal record_size;	
    XtArgsProc get_values_hook;
} ConstraintClassExtensionRec, *ConstraintClassExtension;

typedef struct _ConstraintClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
} ConstraintClassRec;

extern ConstraintClassRec constraintClassRec;





# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 93 "SYS$COMMON:[DECW$INCLUDE]CONSTRAINP.H;1"




# 214 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]OBJECTP.H;1"

















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 50 "SYS$COMMON:[DECW$INCLUDE]OBJECTP.H;1"










typedef struct _ObjectPart {
    Widget          self;               
    WidgetClass     widget_class;       
    Widget          parent;             
    XrmName         xrm_name;           
    Boolean         being_destroyed;    
    XtCallbackList  destroy_callbacks;  
    XtPointer       constraints;        
} ObjectPart;

typedef struct _ObjectRec {
    ObjectPart  object;
} ObjectRec;










typedef struct _ObjectClassPart {

    WidgetClass     superclass;         
    String          class_name;         
    Cardinal        widget_size;        
    XtProc          class_initialize;   
    XtWidgetClassProc class_part_initialize; 
    XtEnum          class_inited;       
    XtInitProc      initialize;         
    XtArgsProc      initialize_hook;    
    XtProc          obj1;		

    
    XtPointer       obj2;               



    Cardinal        obj3;               
    XtResourceList  resources;          
    Cardinal        num_resources;      
    XrmClass        xrm_class;          
    Boolean         obj4;               

    
    XtEnum          obj5;               



    Boolean         obj6;               
    Boolean         obj7;               
    XtWidgetProc    destroy;            
    XtProc          obj8;               
    XtProc          obj9;               
    XtSetValuesFunc set_values;         
    XtArgsFunc      set_values_hook;    
    XtProc          obj10;              
    XtArgsProc      get_values_hook;    
    XtProc          obj11;              
    XtVersionType   version;            
    XtPointer       callback_private;   
    String          obj12;              
    XtProc          obj13;              
    XtProc          obj14;              
    XtPointer       extension;          
}ObjectClassPart;

typedef struct _ObjectClassRec {
    ObjectClassPart object_class;
} ObjectClassRec;


extern ObjectClassRec objectClassRec;



# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 138 "SYS$COMMON:[DECW$INCLUDE]OBJECTP.H;1"



# 215 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]RECTOBJP.H;1"

















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 50 "SYS$COMMON:[DECW$INCLUDE]RECTOBJP.H;1"











typedef struct _RectObjPart {
    Position        x, y;               
    Dimension       width, height;      
    Dimension       border_width;       
    Boolean         managed;            
    Boolean         sensitive;          
    Boolean         ancestor_sensitive; 
}RectObjPart;

typedef struct _RectObjRec {
    ObjectPart object;
    RectObjPart rectangle;
} RectObjRec;












typedef struct _RectObjClassPart {

    WidgetClass     superclass;         
    String          class_name;         
    Cardinal        widget_size;        
    XtProc          class_initialize;   
    XtWidgetClassProc class_part_initialize; 
    XtEnum          class_inited;       
    XtInitProc      initialize;         
    XtArgsProc      initialize_hook;    
    XtProc          rect1;		
    XtPointer       rect2;              
    Cardinal        rect3;              
    XtResourceList  resources;          
    Cardinal        num_resources;      
    XrmClass        xrm_class;          
    Boolean         rect4;              

    
    XtEnum          rect5;              



    Boolean         rect6;              
    Boolean         rect7;              
    XtWidgetProc    destroy;            
    XtWidgetProc    resize;             
    XtExposeProc    expose;             
    XtSetValuesFunc set_values;         
    XtArgsFunc      set_values_hook;    
    XtAlmostProc    set_values_almost;  
    XtArgsProc      get_values_hook;    
    XtProc          rect9;              
    XtVersionType   version;            
    XtPointer       callback_private;   
    String          rect10;             
    XtGeometryHandler query_geometry;   
    XtProc          rect11;             
    XtPointer       extension;          
} RectObjClassPart;

typedef struct _RectObjClassRec {
    RectObjClassPart rect_class;
} RectObjClassRec;

extern RectObjClassRec rectObjClassRec;



# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 135 "SYS$COMMON:[DECW$INCLUDE]RECTOBJP.H;1"



# 216 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"























































 

extern Widget _XtWindowedAncestor( 



);

extern void _XtInherit(



);

extern void XtCreateWindow(







);

extern void XtResizeWidget(






);

extern void XtMoveWidget(





);

extern void XtConfigureWidget(








);

extern void XtResizeWindow(



);

 



# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 333 "SYS$COMMON:[DECW$INCLUDE]INTRINSICP.H;1"




# 69 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]PTYX.H;1"

# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]MISC.H;1"










































# 70 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]PTYX.H;1"

# 1 "SYS$COMMON:[DECW$INCLUDE]XFUNCS.H;3"
















































































# 71 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]PTYX.H;1"


# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]CONVERTERS.H;1"

























# 1 "SYS$COMMON:[DECW$INCLUDE]XFUNCPROTO.H;1"






































# 96 "SYS$COMMON:[DECW$INCLUDE]XFUNCPROTO.H;1"


# 26 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]CONVERTERS.H;1"




 








extern void XmuCvtFunctionToCallback(






);












extern void XmuCvtStringToBackingStore(






);





extern void XmuCvtStringToCursor(






);
extern Boolean XmuCvtStringToColorCursor(








);









typedef int XtGravity;















extern void XmuCvtStringToGravity (






);





typedef enum {
    XtJustifyLeft,       
    XtJustifyCenter,     
    XtJustifyRight       
} XtJustify;






extern void XmuCvtStringToJustify(






);






extern void XmuCvtStringToLong(






);





typedef enum {XtorientHorizontal, XtorientVertical} XtOrientation;
extern void XmuCvtStringToOrientation(






);





extern void XmuCvtStringToBitmap(






);





extern void XmuCvtStringToPixmap(






);      


















extern Boolean XmuCvtStringToShapeStyle(








);

extern Boolean XmuReshapeWidget(






);




extern void XmuCvtStringToWidget(






);




extern Boolean XmuNewCvtStringToWidget(








);

 



# 73 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]PTYX.H;1"


















































































































typedef enum {NORMAL, LEFTEXTENSION, RIGHTEXTENSION} EventMode;






typedef unsigned char Char;		
typedef Char **ScrnBuf;























































typedef struct {
	unsigned char	a_type;
	unsigned char	a_pintro;
	unsigned char	a_final;
	unsigned char	a_inters;
	char	a_nparam;		
	short	a_param[30];	
} ANSI;










typedef struct {
	int	x;
	int	y;
	int	fontsize;
	int	linetype;
} Tmodes;

typedef struct {
	int Twidth;
	int Theight;
} T_fontsize;

typedef struct {
	short *bits;
	int x;
	int y;
	int width;
	int height;
} BitmapBits;

































typedef struct {
	unsigned	which;	
	Pixel		colors[8];
	char		*names[8];
} ScrnColors;
















































































































































































































































	





	



	




















	







	





























































typedef unsigned char IChar;	






typedef struct {
	int	cnt;		
	IChar *	ptr;		
	Char	buf[4096];	






	} PtyData;













typedef enum {
	OFF_FLAGS = 0		
	, OFF_CHARS = 1
	, OFF_ATTRS = 2





	, OFF_COLOR



	, OFF_CSETS




} BufOffsets;

	









	









typedef struct {
	unsigned	chrset;
	unsigned	flags;
	XFontStruct *	fs;
	GC		gc;
	char *		fn;
} XTermFonts;

	
typedef enum {
	DP_DECCKM,
	DP_DECANM,
	DP_DECCOLM,	
	DP_DECSCLM,
	DP_DECSCNM,
	DP_DECOM,
	DP_DECAWM,
	DP_DECARM,
	DP_X_X10MSE,
	DP_DECPFF,
	DP_DECPEX,
	DP_DECTCEM,
	DP_DECTEK,
	DP_X_DECCOLM,
	DP_X_MORE,
	DP_X_MARGIN,
	DP_X_REVWRAP,
	DP_X_LOGGING,
	DP_X_ALTSCRN,
	DP_DECBKM,
	DP_X_MOUSE,
	DP_LAST
	} SaveModes;




	
typedef enum {
	mainMenu,
	vtMenu,
	fontMenu,
	tekMenu
} MenuIndex;



typedef struct {
	Boolean		saved;
	int		row;
	int		col;
	unsigned	flags;		
	char		curgl;
	char		curgr;
	char		gsets[4];

	int		cur_foreground; 
	int		cur_background; 
	int		sgr_foreground; 

} SavedCursor;

struct _vtwin {
	Window		window;		
	int		width;		
	int		height;		
	int		fullwidth;	
	int		fullheight;	
	int		f_width;	
	int		f_height;	
	int		scrollbar;	
					
	GC		normalGC;	
	GC		reverseGC;	
	GC		normalboldGC;	
	GC		reverseboldGC;	




};

struct _tekwin {
	Window		window;		
	int		width;		
	int		height;		
	int		fullwidth;	
	int		fullheight;	
	double		tekscale;	
};

typedef struct {

	Display		*display;	
	int		respond;	






	long		pid;		
	int		uid;		
	int		gid;		
	GC		cursorGC;	
	GC		fillCursorGC;	
	GC		reversecursorGC;
	GC		cursoroutlineGC;
	Pixel		foreground;	
	Pixel		cursorcolor;	
	Pixel		mousecolor;	
	Pixel		mousecolorback;	

	Pixel		Acolors[(16+3)]; 
	Boolean		boldColors;	
	Boolean		colorMode;	
	Boolean		colorULMode;	
	Boolean		colorBDMode;	
	Boolean		colorBLMode;	
	Boolean		colorAttrMode;	


	Pixel		highlightcolor;	


	Boolean		font_doublesize;
	int		cache_doublesize;
	Char		cur_chrset;	
	int		fonts_used;	
	XTermFonts	double_fonts[8];







	int		border;		
	Cursor		arrow;		
	unsigned long	event_mask;
	unsigned short	send_mouse_pos;	
					











	int		mouse_button;	
	int		mouse_row;	
	int		mouse_col;	
	int		select;		
	Boolean		visualbell;	
	Boolean		allowSendEvents;
	Boolean		awaitInput;	
	Boolean		grabbedKbd;	






	int		inhibit;	


	Boolean		Vshow;		
	struct _vtwin	fullVwin;





	Cursor	pointer_cursor;		

	String	answer_back;		
	String	printer_command;	
	Boolean printer_autoclose;	
	Boolean printer_extent;		
	Boolean printer_formfeed;	
	int	printer_controlmode;	

	int	print_attributes;	


	Boolean		fnt_prop;	
	Boolean		fnt_boxes;	
	Dimension	fnt_wide;
	Dimension	fnt_high;
	XFontStruct	*fnt_norm;	
	XFontStruct	*fnt_bold;	



	int		enbolden;	
	XPoint		*box;		

	int		cursor_state;	

	Boolean		cursor_blink;	
	int		cursor_on;	
	int		cursor_off;	
	XtIntervalId	cursor_timer;	

	int		cursor_set;	
	int		cursor_col;	
	int		cursor_row;	
	int		cur_col;	
	int		cur_row;	
	int		max_col;	
	int		max_row;	
	int		top_marg;	
	int		bot_marg;	
	Widget		scrollWidget;	
	int		topline;	
	int		savedlines;     
	int		savelines;	
	int		scrolllines;	
	Boolean		scrollttyoutput; 
	Boolean		scrollkey;	

	ScrnBuf		visbuf;		
	ScrnBuf		allbuf;		

	Char		*sbuf_address;	
	ScrnBuf		altbuf;		
	Char		*abuf_address;	
	Char		**save_ptr;	
	size_t		save_len;	
	Boolean		alternate;	
	unsigned short	do_wrap;	

	int		incopy;		


	int		copy_src_x;	
	int		copy_src_y;
	unsigned int	copy_width;
	unsigned int	copy_height;
	int		copy_dest_x;
	int		copy_dest_y;
	Boolean		c132;		
	Boolean		curses;		
	Boolean		hp_ll_bc;	
	Boolean		marginbell;	
	int		nmarginbell;	
	int		bellarmed;	
	Boolean 	multiscroll;	
	int		scrolls;	

	SavedCursor	sc[2];		
	int		save_modes[24];	

	
	String		keyboard_dialect; 
	char		gsets[4];	
	char		curgl;		
	char		curgr;		
	char		curss;		
	String		term_id;	
	int		terminal_id;	
	int		ansi_level;	
	int		scroll_amt;	
	int		refresh_amt;	
	int		protected_mode;	
	Boolean		old_fkeys;	
	Boolean		jumpscroll;	
	Boolean         always_highlight; 
	Boolean		underline;	
	Boolean		bold_mode;	


	Boolean		restore_data;
	unsigned	restore_x;
	unsigned	restore_y;
	unsigned	restore_width;
	unsigned	restore_height;

	









	GC		TnormalGC;	
	GC		TcursorGC;	
	Pixel		Tforeground;	
	Pixel		Tbackground;	
	Pixel		Tcursorcolor;	

	int		Tcolor;		
	Boolean		Tshow;		
	Boolean		waitrefresh;	
	struct _tekwin	fullTwin;





	int		xorplane;	
	GC		linepat[4]; 
	Boolean		TekEmu;		
	int		cur_X;		
	int		cur_Y;		
	Tmodes		cur;		
	Tmodes		page;		
	int		margin;		
	int		pen;		
	char		*TekGIN;	
	int		gin_terminator; 


	int		multiClickTime;	 
	int		bellSuppressTime; 
	Boolean		bellInProgress; 
	char		*charClass;	
	Boolean		cutNewline;	
	Boolean		cutToBeginningOfLine;  
	Boolean		highlight_selection; 
	Boolean		trim_selection; 
	char		*selection_data; 
	int		selection_size; 
	int		selection_length; 
	Time		selection_time;	
	int		startHRow, startHCol, 
			endHRow, endHCol,
			startHCoord, endHCoord;
	Atom*		selection_atoms; 
	Cardinal	sel_atoms_size;	
	Cardinal	selection_count; 
	Boolean		input_eight_bits;
	Boolean		output_eight_bits; 
	Boolean		control_eight_bits; 
	Boolean		backarrow_key;		
	Boolean		meta_sends_esc;		
	Pixmap		menu_item_bitmap;	
	String		menu_font_names[9];
	unsigned long	menu_font_sizes[9];
	int		menu_font_number;
	XIC		xic;
} TScreen;

typedef struct _TekPart {
	XFontStruct *	Tfont[4];
	int		tobaseline[4]; 
	char *		initial_font;		
	char *		gin_terminator_str;	




} TekPart;









typedef struct
{
	unsigned	flags;
} TKeyboard;

typedef struct _Misc {
    char *geo_metry;
    char *T_geometry;
    char *f_n;
    char *f_b;



    Boolean login_shell;
    Boolean re_verse;
    Boolean re_verse0;	
    XtGravity resizeGravity;
    Boolean reverseWrap;
    Boolean autoWrap;
    Boolean logInhibit;
    Boolean signalInhibit;

    Boolean tekInhibit;
    Boolean tekSmall;	

    Boolean scrollbar;



    Boolean titeInhibit;
    Boolean appcursorDefault;
    Boolean appkeypadDefault;





    Boolean dynamicColors;
    Boolean shared_ic;









    Boolean shift_keys;		


    Boolean real_NumLock;	
    unsigned long num_lock;	
    unsigned long alt_left;	
    unsigned long alt_right;	
    Boolean meta_trans;		
    unsigned long meta_left;	
    unsigned long meta_right;	

} Misc;

typedef struct {int foo;} XtermClassPart, TekClassPart;

typedef struct _XtermClassRec {
    CoreClassPart  core_class;
    XtermClassPart xterm_class;
} XtermClassRec;

extern WidgetClass xtermWidgetClass;




typedef struct _TekClassRec {
    CoreClassPart core_class;
    TekClassPart tek_class;
} TekClassRec;














typedef unsigned Tabs [10];

typedef struct _XtermWidgetRec {
    CorePart	core;
    TKeyboard	keyboard;	
    TScreen	screen;		
    unsigned	flags;		
    int         cur_foreground;	
    int         cur_background;	
    Pixel       dft_foreground;	
    Pixel       dft_background;	

    int         sgr_foreground;	


    int         num_ptrs;	

    unsigned	initflags;	
    Tabs	tabs;		
    Misc	misc;		
} XtermWidgetRec, *XtermWidget;


typedef struct _TekWidgetRec {
    CorePart core;
    TekPart tek;
} TekWidgetRec, *TekWidget;


























			






































































































typedef struct Tek_Link
{
	struct Tek_Link	*next;	

	short fontsize;		
	short count;		
	char *ptr;		
	char data [1024];
} TekLink;


















# 177 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"
























# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard

















# pragma __member_alignment __save
# pragma __member_alignment



        typedef __int64 jmp_buf [68]; 

# pragma __member_alignment __restore














        int LIB$GET_CURRENT_INVO_CONTEXT (__int64 * __invo_context);
        void c$$establish(void *, unsigned short);






void longjmp (jmp_buf __env, int __val);




            typedef __int64 __sigjmp_buf[70];




	    typedef __sigjmp_buf sigjmp_buf;


	void siglongjmp (__sigjmp_buf __env, int __val);
	int sigsetjmp   (__sigjmp_buf __env, int __savemask);


            void c$$establish(void *, unsigned short);
















#pragma __standard


# 201 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XTERM.H;1"



































































































































































































extern int TekInit (void);
extern int TekPtyData (void);
extern void ChangeTekColors (TScreen *screen, ScrnColors *pNew);
extern void TCursorToggle (int toggle);
extern void TekCopy (void);
extern void TekEnqMouse (int c);
extern void TekExpose (Widget w, XEvent *event, Region region);
extern void TekGINoff (void);
extern void TekReverseVideo (TScreen *screen);
extern void TekRun (void);
extern void TekSetFontSize (int newitem);
extern void TekSimulatePageButton (int reset);
extern void dorefresh (void);


extern Boolean SendMousePosition (Widget w, XEvent* event);
extern int SetCharacterClassRange (int low, int high, int value);
extern void DiredButton               (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void DisownSelection (XtermWidget termw);
extern void HandleGINInput            (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleInsertSelection     (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleKeyboardSelectEnd   (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleKeyboardSelectStart (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleKeyboardStartExtend (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSecure              (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSelectEnd           (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSelectExtend        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSelectSet           (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSelectStart         (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleStartExtend         (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void ResizeSelection (TScreen *screen, int rows, int cols);
extern void ScrollSelection (TScreen* screen, int amount);
extern void TrackMouse (int func, int startrow, int startcol, int firstrow, int lastrow);
extern void TrackText (int frow, int fcol, int trow, int tcol);
extern void ViButton                  (Widget w, XEvent *event, String *params, Cardinal *num_params);









extern int VTInit (void);
extern int v_write (int f, Char *d, int len);
extern void FindFontSelection (char *atom_name, int justprobe);
extern void HideCursor (void);
extern void ShowCursor (void);
extern void SwitchBufPtrs (TScreen *screen);
extern void ToggleAlternate (TScreen *screen);
extern void VTReset (int full, int saved);
extern void VTRun (void);
extern void dotext (TScreen *screen, int charset, IChar *buf, Cardinal len);
extern void resetCharsets (TScreen *screen);
extern void set_cursor_gcs (TScreen *screen);
extern void unparseputc (int c, int fd);
extern void unparseputc1 (int c, int fd);
extern void unparseputs (char *s, int fd);
extern void unparseseq (ANSI *ap, int fd);


extern void ToggleCursorBlink(TScreen *screen);



extern void SGR_Background (int color);
extern void SGR_Foreground (int color);



extern unsigned xtermCharSetIn (unsigned code, int charset);
extern int xtermCharSetOut (IChar *buf, IChar *ptr, char charset);


extern void CarriageReturn (TScreen *screen);
extern void CursorBack (TScreen *screen, int  n);
extern void CursorDown (TScreen *screen, int  n);
extern void CursorForward (TScreen *screen, int  n);
extern void CursorNextLine (TScreen *screen, int count);
extern void CursorPrevLine (TScreen *screen, int count);
extern void CursorRestore (XtermWidget tw);
extern void CursorSave (XtermWidget tw);
extern void CursorSet (TScreen *screen, int row, int col, unsigned flags);
extern void CursorUp (TScreen *screen, int  n);
extern void Index (TScreen *screen, int amount);
extern void RevIndex (TScreen *screen, int amount);


extern void xterm_DECDHL (int top);
extern void xterm_DECSWL (void);
extern void xterm_DECDWL (void);

extern int xterm_Double_index(unsigned chrset, unsigned flags);
extern GC xterm_DoubleGC(unsigned chrset, unsigned flags, GC old_gc);



extern void Input (TKeyboard *keyboard, TScreen *screen, XKeyEvent *event, int eightbit);
extern void StringInput (TScreen *screen, char *string, size_t nbytes);


extern void VTInitModifiers(void);








extern int main (int argc, char **argv);




extern int GetBytesAvailable (int fd);
extern int kill_process_group (int pid, int sig);
extern int nonblocking_wait (void);
extern void first_map_occurred (void);


extern void Exit (int n);



extern void do_hangup          (Widget gw, XtPointer closure, XtPointer data);
extern void show_8bit_control  (int value);


extern Cursor make_colored_cursor (unsigned cursorindex, unsigned long fg, unsigned long bg);
extern char *SysErrorMsg (int n);
extern char *strindex (char *s1, char *s2);
extern char *udk_lookup (int keycode, int *len);
extern int XStrCmp (char *s1, char *s2);
extern int xerror (Display *d, XErrorEvent *ev);
extern int xioerror (Display *dpy);
extern void Bell (int which, int percent);
extern void ChangeXprop (char *name);
extern void Changename (char *name);
extern void Changetitle (char *name);
extern void Cleanup (int code);
extern void Error (int i);
extern void HandleBellPropertyChange (Widget w, XtPointer closure, XEvent *event, Boolean *cont);
extern void HandleEightBitKeyPressed (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleEnterWindow        (Widget w, XtPointer closure, XEvent *event, Boolean *cont);
extern void HandleFocusChange        (Widget w, XtPointer closure, XEvent *event, Boolean *cont);
extern void HandleInterpret          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleKeyPressed         (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleLeaveWindow        (Widget w, XtPointer closure, XEvent *event, Boolean *cont);
extern void HandleStringEvent        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void Panic (char *s, int a);
extern void Redraw (void);
extern void ReverseOldColors (void);
extern void Setenv (char *var, char *value);
extern void SysError (int i);
extern void VisualBell (void);
extern void creat_as (int uid, int gid, char *pathname, int mode);
extern void do_dcs (Char *buf, size_t len);
extern void do_osc (Char *buf, int len, int final);
extern void do_xevents (void);
extern void end_tek_mode (void);
extern void end_vt_mode (void);
extern void hide_tek_window (void);
extern void hide_vt_window (void);
extern void reset_decudk (void);
extern void set_tek_visibility (Boolean on);
extern void set_vt_visibility (Boolean on);
extern void switch_modes (int tovt);
extern void xevents (void);
extern void xt_error (String message);


extern int QueryMaximize (TScreen *screen, unsigned *width, unsigned *height);
extern void HandleDeIconify          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleIconify            (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleMaximize           (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleRestoreSize        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void RequestMaximize (XtermWidget termw, int maximize);











extern int xtermPrinterControl (int chr);
extern void xtermAutoPrint (int chr);
extern void xtermMediaControl (int param, int private_seq);
extern void xtermPrintScreen (Boolean use_DECPEX);


extern int getPtyData (TScreen *screen, fd_set *select_mask, PtyData *data);
extern unsigned usedPtyData(PtyData *data);
extern void initPtyData (PtyData *data);












extern int non_blank_line (ScrnBuf sb, int row, int col, int len);
extern ScrnBuf Allocate (int nrow, int ncol, Char **addr);
extern int ScreenResize (TScreen *screen, int width, int height, unsigned *flags);
extern size_t ScrnPointers (TScreen *screen, size_t len);
extern void ClearBufRows (TScreen *screen, int first, int last);
extern void ScreenWrite (TScreen *screen, Char *str, unsigned flags, unsigned cur_fg_bg, int length);
extern void ScrnDeleteChar (TScreen *screen, int n, int size);
extern void ScrnDeleteLine (TScreen *screen, ScrnBuf sb, int n, int last, int size, int where);
extern void ScrnInsertChar (TScreen *screen, int n, int size);
extern void ScrnInsertLine (TScreen *screen, ScrnBuf sb, int last, int where, int n, int size);
extern void ScrnRefresh (TScreen *screen, int toprow, int leftcol, int nrows, int ncols, int force);













extern void DoResizeScreen (XtermWidget xw);
extern void HandleScrollBack (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleScrollForward (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void ResizeScrollBar (TScreen *screen);
extern void ScrollBarDrawThumb (Widget scrollWidget);
extern void ScrollBarOff (TScreen *screen);
extern void ScrollBarOn (XtermWidget xw, int init, int doalloc);
extern void ScrollBarReverseVideo (Widget scrollWidget);
extern void ToggleScrollBar (XtermWidget w);
extern void WindowScroll (TScreen *screen, int top);


extern Boolean TabToNextStop (void);
extern Boolean TabToPrevStop (void);
extern int TabNext (Tabs tabs, int col);
extern int TabPrev (Tabs tabs, int col);
extern void TabClear (Tabs tabs, int col);
extern void TabReset (Tabs tabs);
extern void TabSet (Tabs tabs, int col);
extern void TabZonk (Tabs	tabs);


extern GC updatedXtermGC (TScreen *screen, int flags, int fg_bg, int hilite);
extern int AddToRefresh (TScreen *screen);
extern int HandleExposure (TScreen *screen, XEvent *event);
extern int char2lower(int ch);
extern int drawXtermText (TScreen *screen, unsigned flags, GC gc, int x, int y, int chrset, Char *text, Cardinal len);
extern void ChangeAnsiColors (XtermWidget tw);
extern void ChangeColors (XtermWidget tw, ScrnColors *pNew);
extern void ClearRight (TScreen *screen, int n);
extern void ClearScreen (TScreen *screen);
extern void DeleteChar (TScreen *screen, int n);
extern void DeleteLine (TScreen *screen, int n);
extern void FlushScroll (TScreen *screen);
extern void GetColors (XtermWidget tw, ScrnColors *pColors);
extern void InsertChar (TScreen *screen, int n);
extern void InsertLine (TScreen *screen, int n);
extern void RevScroll (TScreen *screen, int amount);
extern void ReverseVideo (XtermWidget termw);
extern void Scroll (TScreen *screen, int amount);
extern void do_erase_display (TScreen *screen, int param, int mode);
extern void do_erase_line (TScreen *screen, int param, int mode);
extern void recolor_cursor (Cursor cursor, unsigned long fg, unsigned long bg);
extern void resetXtermGC (TScreen *screen, int flags, int hilite);
extern void scrolling_copy_area (TScreen *screen, int firstline, int nlines, int amount);



extern Pixel getXtermBackground (int flags, int color);
extern Pixel getXtermForeground (int flags, int color);
extern int extract_fg (unsigned color, unsigned flags);
extern unsigned makeColorPair (int fg, int bg);
extern void ClearCurBackground (TScreen *screen, int top, int left, unsigned height, unsigned width);

































































# 88 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"



# 1 "SYS$COMMON:[DECW$INCLUDE]XATOM.H;1"












































































































# 91 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"


# 1 "SYS$COMMON:[DECW$INCLUDE]CURSORFONT.H;1"
















































































































# 93 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "SYS$COMMON:[DECW$INCLUDE]STRINGDEFS.H;1"



























# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 28 "SYS$COMMON:[DECW$INCLUDE]STRINGDEFS.H;1"

# 243 "SYS$COMMON:[DECW$INCLUDE]STRINGDEFS.H;1"









extern   char XtStrings[];




































































































































































































































































































































































































































































































































































































































































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 897 "SYS$COMMON:[DECW$INCLUDE]STRINGDEFS.H;1"



# 94 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]ATOMS.H;1"

























# 1 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"


























































# 2854 "SYS$COMMON:[DECW$INCLUDE]INTRINSIC.H;1"



# 26 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]ATOMS.H;1"






typedef struct _AtomRec *AtomPtr;

extern AtomPtr
    _XA_ATOM_PAIR,
    _XA_CHARACTER_POSITION,
    _XA_CLASS,
    _XA_CLIENT_WINDOW,
    _XA_CLIPBOARD,
    _XA_COMPOUND_TEXT,
    _XA_DECNET_ADDRESS,
    _XA_DELETE,
    _XA_FILENAME,
    _XA_HOSTNAME,
    _XA_IP_ADDRESS,
    _XA_LENGTH,
    _XA_LIST_LENGTH,
    _XA_NAME,
    _XA_NET_ADDRESS,
    _XA_NULL,
    _XA_OWNER_OS,
    _XA_SPAN,
    _XA_TARGETS,
    _XA_TEXT,
    _XA_TIMESTAMP,
    _XA_USER;
























 

extern char *XmuGetAtomName(




);

extern Atom XmuInternAtom(




);

extern void XmuInternStrings(






);

extern AtomPtr XmuMakeAtom(



);

extern char *XmuNameOfAtom(



);

 



# 95 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]CHARSET.H;1"





























 

extern void XmuCopyISOLatin1Lowered(




);

extern void XmuCopyISOLatin1Uppered(




);

extern int XmuCompareISOLatin1(




);

 



# 96 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"







# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard





















#pragma __member_alignment __save
#pragma __member_alignment










struct _iobuf 
{
        int            _cnt;
        char          *_ptr;
        char          *_base;
        unsigned char  _flag;



        unsigned char  _padfile;  
        unsigned char  _pad1;
        unsigned char  _pad2;
        int            _file;





};



    typedef __FILE FILE;


typedef struct {unsigned : 32; unsigned : 32; } fpos_t;
























#pragma __extern_model __save
#pragma __extern_model __strict_refdef
 extern __FILE * (decc$ga_stderr);
 extern __FILE * (decc$ga_stdin);
 extern __FILE * (decc$ga_stdout);
#pragma __extern_model __restore









# pragma __extern_model __save
# pragma __extern_model __strict_refdef
       extern int optind;
       extern int opterr;
       extern int optopt;
       extern char *optarg;
# pragma __extern_model __restore
































































































































































    



















    






# 357 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


    

























void  setbuf  (__FILE *__stream, char *__buf);
int   setvbuf (__FILE *__stream, char *__buf, int __mode, __size_t __size);










	 


       int getopt (int __argc, char * const __argv[], const char *__optstring);

















__size_t fread  (void *__ptr, __size_t __size, __size_t __nmemb, __FILE *__stream);
__size_t fwrite (const void *__ptr, __size_t __size, __size_t __nmemb, __FILE *__stream);

int    fclose   (__FILE *__stream);
int    fflush   (__FILE *__stream);
int    fgetpos  (__FILE *__stream, fpos_t *__pos);
int    fprintf  (__FILE *__stream, const char *__format, ...);
int    fputs    (const char *__s, __FILE *__stream);
int    fscanf   (__FILE *__stream, const char *__format, ...);
int    fseek    (__FILE *__stream, long int __offset, int __whence);
int    fsetpos  (__FILE *__stream, const fpos_t *__pos);
long   ftell    (__FILE *__stream);
void   perror   (const char *__s);
int    printf   (const char *__format, ...);
int    puts     (const char *__s);
int    remove   (const char *__filename);
int    rename   (const char *__old, const char *__new);
int    scanf    (const char *__format, ...);
int    sprintf  (char *__s, const char *__format, ...);
int    sscanf   (const char *__s, const char *__format, ...);
int    ungetc   (int __c, __FILE *__stream);
int    vfprintf (__FILE *__stream, const char *__format, __va_list __arg); 
int    vprintf  (const char *__format, __va_list __arg);
int    vsprintf (char *__s, const char *__format, __va_list __arg);

__FILE_ptr32 tmpfile(void);





void (clearerr) (__FILE *__stream);
int  (feof)     (__FILE *__stream);
int  (ferror)   (__FILE *__stream);
int  (fgetc)    (__FILE *__stream);
int  (fputc)    (int __c, __FILE *__stream);
int  (getc)     (__FILE *__stream);
int  (getchar)  (void);
int  (putc)     (int __c, __FILE *__stream);
int  (putchar)  (int __c);
















       __FILE_ptr32 fopen (const char *__filename, const char *__mode, ...);













    __FILE_ptr32 freopen (const char *__filename, const char *__mode, __FILE *__stream, ...);
    int  rewind (__FILE *__stream);













    char *ctermid (char *__str);



    char *cuserid (char *__str);


    char *gets    (char *__s);
    char *fgets   (char *__s, int __n, __FILE *__stream);
    char *tmpnam  (char *__s);


























    int (fileno) (__FILE *__stream);
    __FILE_ptr32 fdopen(int __file_desc, char *__mode);





	 

    __FILE_ptr32 popen(const char *__command, const char *__mode);
    int pclose(__FILE *__stream);





	 

    int getw (__FILE *__stream);
    int putw (int __integer, __FILE *__stream);





	 

    __char_ptr32 tempnam(const char *dir, const char *pfx);







	 





    char *fgetname(__FILE *__stream, char *__buf, ...);








    int decc$read (int __file_desc, void *__buffer, int __nbytes);





    int fwait (__FILE *__stream);
    int lwait (int __file_desc);

    

	 
    int fsync (int __file_desc);







# 645 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"














#pragma __member_alignment __restore
#pragma __standard


# 103 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"


























#pragma __nostandard


















int isalnum  (int);
int isalpha  (int);
int iscntrl  (int);
int isdigit  (int);
int isgraph  (int);
int islower  (int);
int isprint  (int);
int ispunct  (int);
int isspace  (int);
int isupper  (int);
int isxdigit (int);





int tolower (int);
int toupper (int);













# pragma __extern_model __save
# pragma __extern_model __strict_refdef

    






    extern const unsigned int  * (decc$$ga___ctypet) ;

    extern int (decc$$gl___ctypea) ;
    extern int (decc$$gl___isclocale) ;
# pragma __extern_model __restore














































 




        int isascii (int);
        int toascii (int);













# 215 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"













#pragma __standard


# 104 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"










# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]VTPARSE.H;1"










































typedef char PARSE_T;

extern   PARSE_T ansi_table[];
extern   PARSE_T csi_ex_table[];
extern   PARSE_T csi_quo_table[];



extern   PARSE_T csi_table[];
extern   PARSE_T dec2_table[];
extern   PARSE_T dec3_table[];
extern   PARSE_T dec_table[];
extern   PARSE_T eigtable[];
extern   PARSE_T esc_sp_table[];
extern   PARSE_T esc_table[];
extern   PARSE_T iestable[];
extern   PARSE_T igntable[];
extern   PARSE_T scrtable[];
extern   PARSE_T scstable[];
extern   PARSE_T sos_table[];


extern   PARSE_T vt52_table[];
extern   PARSE_T vt52_esc_table[];









































































































































# 114 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]DATA.H;1"
































extern Widget toplevel;

extern XtAppContext app_con;


extern Char *Tpushb;
extern Char *Tpushback;
extern PtyData *Tbuffer;
extern TekLink *TekRefresh;
extern TekWidget tekWidget;
extern Widget tekshellwidget;
extern int TEKgcFontMask;
extern int T_lastx;
extern int T_lasty;
extern int Ttoggled;
extern jmp_buf Tekend;





extern char *ptydev;
extern char *ttydev;
extern char *xterm_name;
extern Boolean sunFunctionKeys;
extern int hold_screen;






extern int zIconBeep; 
extern Boolean zIconBeep_flagged; 



extern Boolean sameName; 



extern Boolean sunKeyboard;


extern PtyData VTbuffer;
extern int am_slave;
extern int max_plus1;
extern jmp_buf VTend;





extern fd_set Select_mask;
extern fd_set X_mask;
extern fd_set pty_mask;

extern int waitingForTrackInfo;

extern EventMode eventMode;

extern XtermWidget term;





















# 115 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]ERROR.H;1"
















































































































# 116 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MENU.H;1"
































































typedef struct _MenuEntry {
    char *name;
    void (*function) (Widget gw, XtPointer closure, XtPointer data);
    Widget widget;
} MenuEntry;

extern MenuEntry mainMenuEntries[], vtMenuEntries[];
extern MenuEntry fontMenuEntries[];

extern MenuEntry tekMenuEntries[];


extern void Handle8BitControl      (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAllow132         (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAllowSends       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAltScreen        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAppCursor        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAppKeypad        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAutoLineFeed     (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleAutoWrap         (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleBackarrow        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleClearSavedLines  (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleCreateMenu       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleCursesEmul       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleCursorBlink      (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleFontDoublesize   (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleFontLoading      (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleHardReset        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleHpFunctionKeys   (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleJumpscroll       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleLogging          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleMetaEsc          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleNumLock          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleMarginBell       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandlePopupMenu        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandlePrint            (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleQuit             (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleRedraw           (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleReverseVideo     (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleReverseWrap      (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleScrollKey        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleScrollTtyOutput  (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleScrollbar        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSendSignal       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSetTekText       (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSetTerminalType  (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSetVisualBell    (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSoftReset        (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSunFunctionKeys  (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleSunKeyboard      (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleTekCopy          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleTekPage          (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleTekReset         (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleTiteInhibit      (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleVisibility       (Widget w, XEvent *event, String *params, Cardinal *num_params);

extern void DoSecureKeyboard (Time tp);
extern void SetupMenus (Widget shell, Widget *forms, Widget *menus);









typedef enum {
    mainMenu_securekbd,
    mainMenu_allowsends,



    mainMenu_print,
    mainMenu_redraw,
    mainMenu_line1,
    mainMenu_8bit_ctrl,
    mainMenu_backarrow,

    mainMenu_num_lock,
    mainMenu_meta_esc,

    mainMenu_sun_fkeys,

    mainMenu_sun_kbd,




    mainMenu_line2,
    mainMenu_suspend,
    mainMenu_continue,
    mainMenu_interrupt,
    mainMenu_hangup,
    mainMenu_terminate,
    mainMenu_kill,
    mainMenu_line3,
    mainMenu_quit,
    mainMenu_LAST
} mainMenuIndices;





typedef enum {
    vtMenu_scrollbar,
    vtMenu_jumpscroll,
    vtMenu_reversevideo,
    vtMenu_autowrap,
    vtMenu_reversewrap,
    vtMenu_autolinefeed,
    vtMenu_appcursor,
    vtMenu_appkeypad,
    vtMenu_scrollkey,
    vtMenu_scrollttyoutput,
    vtMenu_allow132,
    vtMenu_cursesemul,
    vtMenu_visualbell,
    vtMenu_marginbell,

    vtMenu_cursorblink,

    vtMenu_titeInhibit,



    vtMenu_line1,
    vtMenu_softreset,
    vtMenu_hardreset,
    vtMenu_clearsavedlines,
    vtMenu_line2,

    vtMenu_tekshow,
    vtMenu_tekmode,
    vtMenu_vthide,

    vtMenu_altscreen,
    vtMenu_LAST
} vtMenuIndices;




typedef enum {
    fontMenu_fontdefault,
    fontMenu_font1,
    fontMenu_font2,
    fontMenu_font3,
    fontMenu_font4,
    fontMenu_font5,
    fontMenu_font6,

    fontMenu_fontescape,
    fontMenu_fontsel,


    fontMenu_line1,

    fontMenu_font_doublesize,





    fontMenu_LAST
} fontMenuIndices;






typedef enum {
    tekMenu_tektextlarge,
    tekMenu_tektext2,
    tekMenu_tektext3,
    tekMenu_tektextsmall,
    tekMenu_line1,
    tekMenu_tekpage,
    tekMenu_tekreset,
    tekMenu_tekcopy,
    tekMenu_line2,
    tekMenu_vtshow,
    tekMenu_vtmode,
    tekMenu_tekhide,
    tekMenu_LAST
} tekMenuIndices;








extern void UpdateMenuItem(Widget mi, XtArgVal val);


extern void SetItemSensitivity(Widget mi, XtArgVal val);



















































































































































































































































# 117 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MAIN.H;1"
































# 118 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]FONTUTILS.H;1"










































extern int xtermLoadFont (TScreen *screen, char *nfontname, char *bfontname, int doresize, int fontnum);
extern void HandleSetFont (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void SetVTFont (int i, int doresize, char *name1, char *name2);
extern void xtermComputeFontInfo (TScreen *screen, struct _vtwin *win, XFontStruct *font, int sbwidth);
extern void xtermSaveFontInfo (TScreen *screen, XFontStruct *font);
extern void xtermSetCursorBox (TScreen *screen);
extern void xtermUpdateFontInfo (TScreen *screen, int doresize);



extern char *xtermSpecialFont(unsigned atts, unsigned chrset);



extern int xtermMissingChar(unsigned ch, XFontStruct *font);
extern void xtermDrawBoxChar(TScreen *screen, int ch, unsigned flags, GC gc, int x, int y);



extern void HandleSmallerFont (Widget w, XEvent *event, String *params, Cardinal *num_params);
extern void HandleLargerFont (Widget w, XEvent *event, String *params, Cardinal *num_params);




# 119 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]XCHARMOUSE.H;1"

























































enum {
	MOUSE_OFF
	, X10_MOUSE
	, VT200_MOUSE
	, VT200_HIGHLIGHT_MOUSE
	, BTN_EVENT_MOUSE
	, ANY_EVENT_MOUSE
	, DEC_LOCATOR
};



# 120 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"







# 1 "SYS$COMMON:[DECW$INCLUDE]SHELL.H;1"

















































# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVSET.H;1"































#pragma member_alignment __save



#pragma member_alignment

#pragma extern_model __save
#pragma extern_model __relaxed_refdef


# 50 "SYS$COMMON:[DECW$INCLUDE]SHELL.H;1"














# 142 "SYS$COMMON:[DECW$INCLUDE]SHELL.H;1"









extern   char XtShellStrings[];













































































































































































































































typedef struct _ShellClassRec *ShellWidgetClass;
typedef struct _OverrideShellClassRec *OverrideShellWidgetClass;
typedef struct _WMShellClassRec *WMShellWidgetClass;
typedef struct _TransientShellClassRec *TransientShellWidgetClass;
typedef struct _TopLevelShellClassRec *TopLevelShellWidgetClass;
typedef struct _ApplicationShellClassRec *ApplicationShellWidgetClass;


extern WidgetClass shellWidgetClass;
extern WidgetClass overrideShellWidgetClass;
extern WidgetClass wmShellWidgetClass;
extern WidgetClass transientShellWidgetClass;
extern WidgetClass topLevelShellWidgetClass;
extern WidgetClass applicationShellWidgetClass;




# 1 "SYS$COMMON:[DECW$INCLUDE]APIENVRST.H;1"




























#pragma member_alignment __restore
#pragma extern_model __restore


# 407 "SYS$COMMON:[DECW$INCLUDE]SHELL.H;1"




# 127 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"


extern char *ProgramName;

static int in_put (void);
static int set_character_class (char *s);
static void FromAlternate (TScreen *screen);
static void RequestResize (XtermWidget termw, int rows, int cols, int text);
static void SwitchBufs (TScreen *screen);
static void ToAlternate (TScreen *screen);
static void VTallocbuf (void);
static void WriteText (TScreen *screen, Char *str, Cardinal len);
static void ansi_modes (XtermWidget termw, void (*func)(unsigned *p, unsigned mask));
static void bitclr (unsigned *p, unsigned mask);
static void bitcpy (unsigned *p, unsigned q, unsigned mask);
static void bitset (unsigned *p, unsigned mask);
static void dpmodes (XtermWidget termw, void (*func)(unsigned *p, unsigned mask));
static void restoremodes (XtermWidget termw);
static void savemodes (XtermWidget termw);
static void unparseputn (unsigned int n, int fd);
static void window_ops (XtermWidget termw);


static void BlinkCursor ( XtPointer closure, XtIntervalId* id);
static void StartBlinking (TScreen *screen);
static void StopBlinking (TScreen *screen);










static int nparam;
static ANSI reply;
static int param[30];






static jmp_buf vtjmpbuf;


static void HandleBell          (Widget w, XEvent *event, String *params, Cardinal *num_params);
static void HandleIgnore        (Widget w, XEvent *event, String *params, Cardinal *num_params);
static void HandleKeymapChange  (Widget w, XEvent *event, String *params, Cardinal *num_params);
static void HandleVisualBell    (Widget w, XEvent *event, String *params, Cardinal *num_params);

static void HandleStructNotify  (Widget w, XtPointer closure, XEvent *event, Boolean *cont);










static  Boolean	defaultCOLORMODE   = 1;











static  Boolean	defaultFALSE	   = 0;
static  Boolean	defaultTRUE	   = 1;
static  int	defaultZERO        = 0;
static  int	defaultIntBorder   = 2;
static  int	defaultSaveLines   = 64;
static	int	defaultScrollLines = 1;
static  int	defaultNMarginBell = 10;
static  int	defaultMultiClickTime = 250;
static  int	defaultBellSuppressTime = 200;
static	char *	_Font_Selected_ = "yes";  


static  int	defaultBlinkOnTime = 600;
static  int	defaultBlinkOffTime = 300;



static  int	defaultONE = 1;



static	int	default_NUM_CHRSET = 8;


static char defaultTranslations[] =
"          Shift <KeyPress> Prior:scroll-back(1,halfpage) \n           Shift <KeyPress> Next:scroll-forw(1,halfpage) \n         Shift <KeyPress> Select:select-cursor-start() select-cursor-end(PRIMARY, CUT_BUFFER0) \n         Shift <KeyPress> Insert:insert-selection(PRIMARY, CUT_BUFFER0) \n                ~Meta <KeyPress>:insert-seven-bit() \n                 Meta <KeyPress>:insert-eight-bit() \n                !Ctrl <Btn1Down>:popup-menu(mainMenu) \n           !Lock Ctrl <Btn1Down>:popup-menu(mainMenu) \n !Lock Ctrl @Num_Lock <Btn1Down>:popup-menu(mainMenu) \n     ! @Num_Lock Ctrl <Btn1Down>:popup-menu(mainMenu) \n                ~Meta <Btn1Down>:select-start() \n              ~Meta <Btn1Motion>:select-extend() \n                !Ctrl <Btn2Down>:popup-menu(vtMenu) \n           !Lock Ctrl <Btn2Down>:popup-menu(vtMenu) \n !Lock Ctrl @Num_Lock <Btn2Down>:popup-menu(vtMenu) \n     ! @Num_Lock Ctrl <Btn2Down>:popup-menu(vtMenu) \n          ~Ctrl ~Meta <Btn2Down>:ignore() \n                 Meta <Btn2Down>:clear-saved-lines() \n            ~Ctrl ~Meta <Btn2Up>:insert-selection(PRIMARY, CUT_BUFFER0) \n                !Ctrl <Btn3Down>:popup-menu(fontMenu) \n           !Lock Ctrl <Btn3Down>:popup-menu(fontMenu) \n !Lock Ctrl @Num_Lock <Btn3Down>:popup-menu(fontMenu) \n     ! @Num_Lock Ctrl <Btn3Down>:popup-menu(fontMenu) \n          ~Ctrl ~Meta <Btn3Down>:start-extend() \n              ~Meta <Btn3Motion>:select-extend()      \n                 Ctrl <Btn4Down>:scroll-back(1,halfpage) \n            Lock Ctrl <Btn4Down>:scroll-back(1,halfpage) \n  Lock @Num_Lock Ctrl <Btn4Down>:scroll-back(1,halfpage) \n       @Num_Lock Ctrl <Btn4Down>:scroll-back(1,halfpage) \n                      <Btn4Down>:scroll-back(5,line)     \n                 Ctrl <Btn5Down>:scroll-forw(1,halfpage) \n            Lock Ctrl <Btn5Down>:scroll-forw(1,halfpage) \n  Lock @Num_Lock Ctrl <Btn5Down>:scroll-forw(1,halfpage) \n       @Num_Lock Ctrl <Btn5Down>:scroll-forw(1,halfpage) \n                      <Btn5Down>:scroll-forw(5,line)     \n                         <BtnUp>:select-end(PRIMARY, CUT_BUFFER0) \n                       <BtnDown>:bell(0) "





































; 

static XtActionsRec actionsList[] = {
    { "allow-send-events",	HandleAllowSends },
    { "bell",			HandleBell },
    { "clear-saved-lines",	HandleClearSavedLines },
    { "create-menu",		HandleCreateMenu },
    { "dired-button",		DiredButton },
    { "hard-reset",		HandleHardReset },
    { "ignore", 		HandleIgnore },
    { "insert", 		HandleKeyPressed },  
    { "insert-eight-bit",	HandleEightBitKeyPressed },
    { "insert-selection",	HandleInsertSelection },
    { "insert-seven-bit",	HandleKeyPressed },
    { "interpret",		HandleInterpret },
    { "keymap", 		HandleKeymapChange },
    { "popup-menu",		HandlePopupMenu },
    { "print", 			HandlePrint },
    { "quit",			HandleQuit },
    { "redraw", 		HandleRedraw },
    { "scroll-back",		HandleScrollBack },
    { "scroll-forw",		HandleScrollForward },
    { "secure", 		HandleSecure },
    { "select-cursor-end",	HandleKeyboardSelectEnd },
    { "select-cursor-start",	HandleKeyboardSelectStart },
    { "select-end",		HandleSelectEnd },
    { "select-extend",		HandleSelectExtend },
    { "select-set",		HandleSelectSet },
    { "select-start",		HandleSelectStart },
    { "send-signal",		HandleSendSignal },
    { "set-8-bit-control",	Handle8BitControl },
    { "set-allow132",		HandleAllow132 },
    { "set-altscreen",		HandleAltScreen },
    { "set-appcursor",		HandleAppCursor },
    { "set-appkeypad",		HandleAppKeypad },
    { "set-autolinefeed",	HandleAutoLineFeed },
    { "set-autowrap",		HandleAutoWrap },
    { "set-backarrow",		HandleBackarrow },
    { "set-cursesemul", 	HandleCursesEmul },
    { "set-jumpscroll", 	HandleJumpscroll },
    { "set-marginbell", 	HandleMarginBell },
    { "set-reverse-video",	HandleReverseVideo },
    { "set-reversewrap",	HandleReverseWrap },
    { "set-scroll-on-key",	HandleScrollKey },
    { "set-scroll-on-tty-output", HandleScrollTtyOutput },
    { "set-scrollbar",		HandleScrollbar },
    { "set-sun-function-keys",	HandleSunFunctionKeys },
    { "set-sun-keyboard",	HandleSunKeyboard },
    { "set-titeInhibit",	HandleTiteInhibit },
    { "set-visual-bell",	HandleSetVisualBell },
    { "set-vt-font",		HandleSetFont },
    { "soft-reset",		HandleSoftReset },
    { "start-cursor-extend",	HandleKeyboardStartExtend },
    { "start-extend",		HandleStartExtend },
    { "string", 		HandleStringEvent },
    { "vi-button",		ViButton },
    { "visual-bell",		HandleVisualBell },




    { "set-cursorblink",	HandleCursorBlink },


    { "set-font-doublesize",	HandleFontDoublesize },








    { "deiconify",		HandleDeIconify },
    { "iconify",		HandleIconify },
    { "maximize",		HandleMaximize },
    { "restore",		HandleRestoreSize },


    { "set-num-lock",		HandleNumLock },


    { "larger-vt-font",		HandleLargerFont },
    { "smaller-vt-font",	HandleSmallerFont },


    { "set-terminal-type",	HandleSetTerminalType },
    { "set-visibility", 	HandleVisibility },
    { "set-tek-text",		HandleSetTekText },
    { "tek-page",		HandleTekPage },
    { "tek-reset",		HandleTekReset },
    { "tek-copy",		HandleTekCopy },

};

static XtResource resources[] = {
{((char*)&XtStrings[199]), ((char*)&XtStrings[1017]), ((char*)&XtStrings[1797]), sizeof(char *),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.f_n)), ((char*)&XtStrings[1797]),
	"fixed"},
{"boldFont", "BoldFont", ((char*)&XtStrings[1797]), sizeof(char *),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.f_b)), ((char*)&XtStrings[1797]),
	((void *) 0)},
{"c132", "C132", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.c132)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"charClass", "CharClass", ((char*)&XtStrings[1797]), sizeof(char *),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.charClass)),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"curses", "Curses", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.curses)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"hpLowerleftBugCompat", "HpLowerleftBugCompat", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.hp_ll_bc)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},














{"cutNewline", "CutNewline", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cutNewline)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"cutToBeginningOfLine", "CutToBeginningOfLine", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cutToBeginningOfLine)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"highlightSelection","HighlightSelection",((char*)&XtStrings[1561]),
	sizeof(Boolean),((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.highlight_selection)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"trimSelection","TrimSelection",((char*)&XtStrings[1561]),
	sizeof(Boolean),((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.trim_selection)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{((char*)&XtStrings[52]), ((char*)&XtStrings[902]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->core.background_pixel)),
	((char*)&XtStrings[1797]), "XtDefaultBackground"},
{((char*)&XtStrings[214]), ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.foreground)),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},
{"cursorColor", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cursorcolor)),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},

{"cursorBlink", "CursorBlink", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cursor_blink)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"cursorOnTime", "CursorOnTime", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cursor_on)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultBlinkOnTime},
{"cursorOffTime", "CursorOffTime", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cursor_off)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultBlinkOffTime},

{"keyboardDialect", "KeyboardDialect", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.keyboard_dialect)),
	((char*)&XtStrings[1797]), (XtPointer) "B"},
{"eightBitInput", "EightBitInput", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.input_eight_bits)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"eightBitOutput", "EightBitOutput", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.output_eight_bits)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"eightBitControl", "EightBitControl", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.control_eight_bits)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"metaSendsEscape", "MetaSendsEscape", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.meta_sends_esc)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"geometry","Geometry", ((char*)&XtStrings[1797]), sizeof(char *),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.geo_metry)),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"alwaysHighlight","AlwaysHighlight",((char*)&XtStrings[1561]),
	sizeof(Boolean),((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.always_highlight)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"appcursorDefault","AppcursorDefault",((char*)&XtStrings[1561]),
	sizeof(Boolean),((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.appcursorDefault)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"appkeypadDefault","AppkeypadDefault",((char*)&XtStrings[1561]),
	sizeof(Boolean),((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.appkeypadDefault)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"backarrowKey", "BackarrowKey", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.backarrow_key)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"bellSuppressTime", "BellSuppressTime", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.bellSuppressTime)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultBellSuppressTime},
{"tekGeometry","Geometry", ((char*)&XtStrings[1797]), sizeof(char *),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.T_geometry)),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"internalBorder",((char*)&XtStrings[940]),((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.border)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultIntBorder},
{"jumpScroll", "JumpScroll", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.jumpscroll)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"oldXtermFKeys", "OldXtermFKeys", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.old_fkeys)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},











{"loginShell", "LoginShell", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.login_shell)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"marginBell", "MarginBell", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.marginbell)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"pointerColor", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.mousecolor)),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},
{"pointerColorBackground", ((char*)&XtStrings[902]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.mousecolorback)),
	((char*)&XtStrings[1797]), "XtDefaultBackground"},
{"pointerShape",((char*)&XtStrings[976]), ((char*)&XtStrings[1611]), sizeof(Cursor),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.pointer_cursor)),
	((char*)&XtStrings[1797]), (XtPointer) "xterm"},
{"answerbackString","AnswerbackString", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.answer_back)),
	((char*)&XtStrings[1797]), (XtPointer) ""},

{"printAttributes","PrintAttributes", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.print_attributes)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultONE},

{"printerAutoClose","PrinterAutoClose", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.printer_autoclose)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"printerControlMode", "PrinterControlMode", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.printer_controlmode)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultZERO},
{"printerCommand","PrinterCommand", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.printer_command)),
	((char*)&XtStrings[1797]), (XtPointer) "lpr"},
{"printerExtent","PrinterExtent", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.printer_extent)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"printerFormFeed","PrinterFormFeed", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.printer_formfeed)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"multiClickTime","MultiClickTime", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.multiClickTime)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultMultiClickTime},
{"multiScroll","MultiScroll", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.multiscroll)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"nMarginBell","Column", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.nmarginbell)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultNMarginBell},
{((char*)&XtStrings[571]),((char*)&XtStrings[1260]),((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.re_verse)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"resizeGravity", "ResizeGravity", "Gravity", sizeof(XtGravity),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.resizeGravity)),
	((char*)&XtStrings[1695]), (XtPointer) 7},
{"reverseWrap","ReverseWrap", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.reverseWrap)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"autoWrap","AutoWrap", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.autoWrap)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"saveLines", "SaveLines", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.savelines)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultSaveLines},
{"scrollBar", "ScrollBar", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.scrollbar)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},





{"scrollTtyOutput","ScrollCond", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.scrollttyoutput)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"scrollKey", "ScrollCond", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.scrollkey)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"scrollLines", "ScrollLines", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.scrolllines)),
	((char*)&XtStrings[1718]), (XtPointer) &defaultScrollLines},
{"signalInhibit","SignalInhibit",((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.signalInhibit)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},

{"numLock", "NumLock", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.real_NumLock)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},


{"shiftKeys", "ShiftKeys", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.shift_keys)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},


{"tekInhibit", "TekInhibit", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.tekInhibit)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"tekSmall", "TekSmall", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.tekSmall)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"tekStartup", "TekStartup", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.TekEmu)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},

{"titeInhibit", "TiteInhibit", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.titeInhibit)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"visualBell", "VisualBell", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.visualbell)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"allowSendEvents", "AllowSendEvents", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.allowSendEvents)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"awaitInput", "AwaitInput", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.awaitInput)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"font1", "Font1", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.menu_font_names[fontMenu_font1])),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"font2", "Font2", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.menu_font_names[fontMenu_font2])),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"font3", "Font3", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.menu_font_names[fontMenu_font3])),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"font4", "Font4", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.menu_font_names[fontMenu_font4])),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"font5", "Font5", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.menu_font_names[fontMenu_font5])),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},
{"font6", "Font6", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.menu_font_names[fontMenu_font6])),
	((char*)&XtStrings[1797]), (XtPointer) ((void *) 0)},

{"fontDoublesize", "FontDoublesize", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.font_doublesize)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"cacheDoublesize", "CacheDoublesize", ((char*)&XtStrings[1718]), sizeof(int),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.cache_doublesize)),
	((char*)&XtStrings[1718]), (XtPointer) &default_NUM_CHRSET},













{"color0", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[0])),
	((char*)&XtStrings[1797]), "black"},
{"color1", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[1])),
	((char*)&XtStrings[1797]), "red3"},
{"color2", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[2])),
	((char*)&XtStrings[1797]), "green3"},
{"color3", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[3])),
	((char*)&XtStrings[1797]), "yellow3"},
{"color4", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[4])),
	((char*)&XtStrings[1797]), "blue3"},
{"color5", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[5])),
	((char*)&XtStrings[1797]), "magenta3"},
{"color6", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[6])),
	((char*)&XtStrings[1797]), "cyan3"},
{"color7", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[7])),
	((char*)&XtStrings[1797]), "gray90"},
{"color8", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[8])),
	((char*)&XtStrings[1797]), "gray30"},
{"color9", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[9])),
	((char*)&XtStrings[1797]), "red"},
{"color10", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[10])),
	((char*)&XtStrings[1797]), "green"},
{"color11", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[11])),
	((char*)&XtStrings[1797]), "yellow"},
{"color12", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[12])),
	((char*)&XtStrings[1797]), "blue"},
{"color13", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[13])),
	((char*)&XtStrings[1797]), "magenta"},
{"color14", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[14])),
	((char*)&XtStrings[1797]), "cyan"},
{"color15", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[15])),
	((char*)&XtStrings[1797]), "white"},





{"colorBD", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[(16)])),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},
{"colorBL", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[(16+2)])),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},
{"colorUL", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.Acolors[(16+1)])),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},
{"colorMode", "ColorMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.colorMode)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultCOLORMODE},
{"colorULMode", "ColorMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.colorULMode)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"colorBDMode", "ColorMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.colorBDMode)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"colorBLMode", "ColorMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.colorBLMode)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"colorAttrMode", "ColorMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.colorAttrMode)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultFALSE},
{"boldColors", "ColorMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.boldColors)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},

{"dynamicColors", "DynamicColors", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->misc.dynamicColors)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},

{"highlightColor", ((char*)&XtStrings[1022]), ((char*)&XtStrings[1754]), sizeof(Pixel),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.highlightcolor)),
	((char*)&XtStrings[1797]), "XtDefaultForeground"},

{"boldMode", "BoldMode", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.bold_mode)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"underLine", "UnderLine", ((char*)&XtStrings[1561]), sizeof(Boolean),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.underline)),
	((char*)&XtStrings[1561]), (XtPointer) &defaultTRUE},
{"decTerminalID", "DecTerminalID", ((char*)&XtStrings[1797]), sizeof(String),
	((__size_t)(&((XtermWidgetRec*) ((void *) 0))->screen.term_id)),
	((char*)&XtStrings[1797]), (XtPointer) "vt100"},






























};

static Boolean VTSetValues (Widget cur, Widget request, Widget new_arg, ArgList args, Cardinal *num_args);
static void VTClassInit (void);
static void VTDestroy (Widget w);
static void VTExpose (Widget w, XEvent *event, Region region);
static void VTInitialize (Widget wrequest, Widget new_arg, ArgList args, Cardinal *num_args);
static void VTRealize (Widget w, XtValueMask *valuemask, XSetWindowAttributes *values);
static void VTResize (Widget w);





static WidgetClassRec xtermClassRec = {
  {

    	(WidgetClass) &widgetClassRec,
    	"VT100",
    	sizeof(XtermWidgetRec),
        VTClassInit,
     ((void *) 0),
    	0,
    	VTInitialize,
        ((void *) 0),
    	VTRealize,
    	actionsList,
    	((Cardinal) (sizeof(actionsList) / sizeof(actionsList[0]))),
    	resources,
    	((Cardinal) (sizeof(resources) / sizeof(resources[0]))),
    	((XrmQuark) 0),
    	1,
    	0,
       1,
    	0,
    	VTDestroy,
    	VTResize,
    	VTExpose,
    	VTSetValues,
        ((void *) 0),
        ((void *) 0),
        ((void *) 0),
    	((void *) 0),
        (11 * 1000 + 5),
        ((void *) 0),
        defaultTranslations,
        ((XtGeometryHandler) _XtInherit),
        ((XtStringProc) _XtInherit),
        ((void *) 0)
  }
};

WidgetClass xtermWidgetClass = (WidgetClass)&xtermClassRec;








void SGR_Foreground(int color)
{
	register TScreen *screen = &term->screen;
	Pixel	fg;

	
	if (color >= 0) {
		term->flags |= 0x20;
	} else {
		term->flags &= ~0x20;
	}
	fg = (((term->flags) & 0x20) && (( color) >= 0) ? term->screen.Acolors[ color] : term->screen.foreground);
	term->cur_foreground = color;

	XSetForeground(screen->display, ((screen)->fullVwin.normalGC), fg);
	XSetBackground(screen->display, ((screen)->fullVwin.reverseGC), fg);

	if (((screen)->fullVwin.normalGC) != ((screen)->fullVwin.normalboldGC)) {
		XSetForeground(screen->display, ((screen)->fullVwin.normalboldGC), fg);
		XSetBackground(screen->display, ((screen)->fullVwin.reverseboldGC), fg);
	}
}

void SGR_Background(int color)
{
	register TScreen *screen = &term->screen;
	Pixel	bg;

	





	if (screen->scroll_amt && (color != term->cur_background))
		FlushScroll(screen);

	if (color >= 0) {
		term->flags |= 0x10;
	} else {
		term->flags &= ~0x10;
	}
	bg = (((term->flags) & 0x10) && (( color) >= 0) ? term->screen.Acolors[ color] : term->core.background_pixel);
	term->cur_background = color;

	XSetBackground(screen->display, ((screen)->fullVwin.normalGC), bg);
	XSetForeground(screen->display, ((screen)->fullVwin.reverseGC), bg);

	if (((screen)->fullVwin.normalGC) != ((screen)->fullVwin.normalboldGC)) {
		XSetBackground(screen->display, ((screen)->fullVwin.normalboldGC), bg);
		XSetForeground(screen->display, ((screen)->fullVwin.reverseboldGC), bg);
	}
}




static void
setExtendedFG(void)
{
	int fg = term->sgr_foreground;


	if (term->screen.colorAttrMode
	 || (fg < 0)) {
		if (term->screen.colorULMode && (term->flags & 0x02))
			fg = (16+1);
		if (term->screen.colorBDMode && (term->flags & 0x04))
			fg = (16);
		if (term->screen.colorBLMode && (term->flags & 0x08))
			fg = (16+2);
	}


	





	if (term->screen.boldColors
	 && (fg >= 0)
	 && (fg < 8)
	 && (term->flags & 0x04))
		fg |= 8;


	SGR_Foreground(fg);
}

static void
reset_SGR_Foreground(void)
{
	term->sgr_foreground = -1;
	setExtendedFG();
}

static void
reset_SGR_Colors(void)
{
	reset_SGR_Foreground();
	SGR_Background(-1);
}


void resetCharsets(TScreen *screen)
{
	screen->gsets[0] = 'B';			
	screen->gsets[1] = '0';			
	screen->gsets[2] = 'B';			
	screen->gsets[3] = 'B';
	screen->curgl = 0;			
	screen->curgr = 2;			
	screen->curss = 0;			
}

	






















static void VTparse(void)
{
	
	static IChar *print_area;
	static size_t print_size, print_used;

	
	static Char *string_area;
	static size_t string_size, string_used;


	static int vt52_cup = 0;


	  PARSE_T *groundtable = ansi_table;
	TScreen *screen = &term->screen;
	  PARSE_T *parsestate;
	unsigned int c;
	Char *cp;
	int row, col, top, bot, scstype, count;
	int private_function;	
	int string_mode;	
	int lastchar;		
	int nextstate;
	int laststate;

	
	(void)(c$$establish(0, 32 ),LIB$GET_CURRENT_INVO_CONTEXT(vtjmpbuf));

	groundtable = screen->ansi_level ? ansi_table : vt52_table;



	parsestate = groundtable;
	scstype = 0;
	private_function = 0;
	string_mode = 0;
	lastchar = -1;		
	nextstate = -1;		

	for( ; ; ) {
	    int thischar = -1;
	    c = (((&VTbuffer)->cnt > 0) ? ((&VTbuffer)->cnt)--, (*((&VTbuffer)->ptr)++) : in_put());

	    
	    if (screen->printer_controlmode == 2) {
		if ((c = xtermPrinterControl(c)) == 0)
		    continue;
	    }

	    





	    if (vt52_cup) {
		param[nparam++] = (c & 0x7f) - 32;
		if (nparam < 2)
			continue;
		vt52_cup = 0;
		if((row = param[0]) < 0)
			row = 0;
		if((col = param[1]) < 0)
			col = 0;
		CursorSet(screen, row, col, term->flags);
		parsestate = vt52_table;
		param[0] = 0;
		param[1] = 0;
		continue;
	    }


	    




	    laststate = nextstate;















	      nextstate = parsestate[(c)];

	    



	    if (nextstate == 63) {
		IChar *new_string = print_area; unsigned new_length = print_size; if (new_length == 0) { new_length = 256; new_string = (IChar *)malloc(new_length * sizeof(IChar)); } else if ( print_used+1 >= new_length) { new_length = print_size * 2; new_string = (IChar *)malloc(new_length * sizeof(IChar)); if (new_string != 0 && print_area != 0 && print_used != 0) __MEMCPY(new_string, print_area, print_used * sizeof(IChar)); };
		if (new_string == 0) {
		    fprintf((decc$ga_stderr),
			    "Cannot allocate %d bytes for printable text\n",
			    new_length);
		    continue;
		}


		




		if (screen->ansi_level < 1)
		    c &= 0x7f;

		print_area = new_string;
		print_size = new_length;
		print_area[print_used++] = lastchar = thischar = c;
		if (((&VTbuffer)->cnt > 0)) {
		    continue;
		}
	    }

	    if (nextstate == 63
	     || (laststate == 63 && print_used)) {
		unsigned single = 0;

		if (screen->curss) {
		    dotext(screen,
			    screen->gsets[(int)(screen->curss)],
			    print_area, 1);
		    screen->curss = 0;
		    single++;
		}
		if (print_used > single) {
		    dotext(screen,
			    screen->gsets[(int)(screen->curgl)],
			    print_area + single,
			    print_used - single);
		}
		print_used = 0;
	    }

	    



	    if (parsestate == sos_table) {
		Char *new_string = string_area; unsigned new_length = string_size; if (new_length == 0) { new_length = 256; new_string = (Char *)malloc(new_length * sizeof(Char)); } else if ( string_used+1 >= new_length) { new_length = string_size * 2; new_string = (Char *)malloc(new_length * sizeof(Char)); if (new_string != 0 && string_area != 0 && string_used != 0) __MEMCPY(new_string, string_area, string_used * sizeof(Char)); };
		if (new_string == 0) {
		    fprintf((decc$ga_stderr),
			    "Cannot allocate %d bytes for string mode %d\n",
			    new_length, string_mode);
		    continue;
		}












		string_area = new_string;
		string_size = new_length;
		string_area[string_used++] = c;
	    } else if (parsestate != esc_table) {
		
	    	string_mode = 0;
		string_used = 0;
	    }

	     

	    switch (nextstate) {
		 case 63:
			
			break;

		 case 0:
			
			parsestate = groundtable;
			break;

		 case 1:
			
			parsestate = igntable;
			break;

		 case 2:
			
			parsestate = iestable;
			break;

		 case 3:
			
			break;

		 case 84:
			for (count = 0; screen->answer_back[count] != 0; count++)
				unparseputc(screen->answer_back[count], screen->respond);
			break;

		 case 4:
			if (string_mode == 0x9D) {
				if (string_used)
					string_area[--string_used] = '\0';
				do_osc(string_area, string_used, c);
				parsestate = groundtable;
			} else {
				
				Bell(9,0);
			}
			break;

		 case 5:
			
			CursorBack(screen, 1);
			break;

		 case 6:
			
			CarriageReturn(screen);
			parsestate = groundtable;
			break;

		 case 7:
			
			if(screen->ansi_level == 0) { parsestate = vt52_esc_table; break;}


			parsestate = esc_table;
			break;


		 case 106:
			vt52_cup = 1;
			nparam = 0;
			break;


		 case 8:
			


			xtermAutoPrint(c);
			Index(screen, 1);
			if (term->flags & 0x2000)
				CarriageReturn(screen);
			do_xevents();
			parsestate = groundtable;
			break;

		 case 78:
			
			if((count = param[0]) == -1)
				count = 1;
			while ((count-- > 0)
			  &&   (TabToPrevStop()));
			parsestate = groundtable;
			break;

		 case 75:
			
			if((count = param[0]) == -1)
				count = 1;
			while ((count-- > 0)
			  &&   (TabToNextStop()));
			parsestate = groundtable;
			break;

		 case 9:
			
			TabToNextStop();
			break;

		 case 10:
			screen->curgl = 0;
			parsestate = groundtable;
			break;

		 case 11:
			screen->curgl = 1;
			parsestate = groundtable;
			break;

		 case 110:
			xterm_DECDHL(c == '3');
			parsestate = groundtable;
			break;

		 case 111:
			xterm_DECSWL();
			parsestate = groundtable;
			break;

		 case 112:
			xterm_DECDWL();
			parsestate = groundtable;
			break;

		 case 12:
			
			parsestate = scrtable;
			break;

		 case 13:
			
			scstype = 0;
			parsestate = scstable;
			break;

		 case 14:
			
			scstype = 1;
			parsestate = scstable;
			break;

		 case 15:
			
			scstype = 2;
			parsestate = scstable;
			break;

		 case 16:
			
			scstype = 3;
			parsestate = scstable;
			break;

		 case 17:
			
			parsestate = eigtable;
			break;

		 case 18:
			
			if((row = param[nparam - 1]) == -1)
				row = 0;
			param[nparam - 1] = 10 * row + (c - '0');
			break;

		 case 19:
			
			if (nparam < 30)
			    param[nparam++] = -1;
			break;

		 case 20:
			
			parsestate = dec_table;
			break;

		 case 102:
			
			parsestate = dec2_table;
			break;

		 case 104:
			
			parsestate = dec3_table;
			break;

		 case 21:
			
			if((row = param[0]) < 1)
				row = 1;
			InsertChar(screen, row);
			parsestate = groundtable;
			break;

		 case 22:
			
			if((row = param[0]) < 1)
				row = 1;
			CursorUp(screen, row);
			parsestate = groundtable;
			break;

		 case 23:
			
			if((row = param[0]) < 1)
				row = 1;
			CursorDown(screen, row);
			parsestate = groundtable;
			break;

		 case 24:
			
			if((col = param[0]) < 1)
				col = 1;
			CursorForward(screen, col);
			parsestate = groundtable;
			break;

		 case 25:
			
			if((col = param[0]) < 1)
				col = 1;
			CursorBack(screen, col);
			parsestate = groundtable;
			break;

		 case 26:
			
			 


			if((row = param[0]) < 1)
				row = 1;
			if(nparam < 2 || (col = param[1]) < 1)
				col = 1;
			CursorSet(screen, row-1, col-1, term->flags);
			parsestate = groundtable;
			break;

		 case 72:
			if((row = param[0]) < 1)
				row = 1;
			CursorSet(screen, row-1, screen->cur_col, term->flags);
			parsestate = groundtable;
			break;

		 case 71:
			
			if((col = param[0]) < 1)
				col = 1;
			CursorSet(screen, screen->cur_row, col-1, term->flags);
			parsestate = groundtable;
			break;

		 case 70:
			


			if (screen->hp_ll_bc)
			    CursorSet(screen, screen->max_row, 0, term->flags);
			parsestate = groundtable;
			break;

		 case 27:
			
			do_erase_display(screen, param[0], 0);
			parsestate = groundtable;
			break;

		 case 28:
			
			do_erase_line(screen, param[0], 0);
			parsestate = groundtable;
			break;

		 case 74:
			
			ClearRight(screen, param[0] < 1 ? 1 : param[0]);
			parsestate = groundtable;
			break;

		 case 29:
			
			if((row = param[0]) < 1)
				row = 1;
			InsertLine(screen, row);
			parsestate = groundtable;
			break;

		 case 30:
			
			if((row = param[0]) < 1)
				row = 1;
			DeleteLine(screen, row);
			parsestate = groundtable;
			break;

		 case 31:
			
			if((row = param[0]) < 1)
				row = 1;
			DeleteChar(screen, row);
			parsestate = groundtable;
			break;

		 case 33:
			if (screen->send_mouse_pos == VT200_HIGHLIGHT_MOUSE
			 || nparam > 1) {
				


				TrackMouse(param[0],
					param[2]-1, param[1]-1,
					param[3]-1, param[4]-2);
			} else {
				
				if((count = param[0]) < 1)
					count = 1;
				RevScroll(screen, count);
				do_xevents();
			}
			parsestate = groundtable;
			break;

		 case 67:
			if(screen->ansi_level == 0) { unparseputc(0x1B, screen->respond); unparseputc('/', screen->respond); unparseputc('Z', screen->respond); parsestate = groundtable; break; }






			param[0] = -1;		
			
		 case 32:
			
			if (param[0] <= 1) {	
			    count = 0;
			    reply.a_type   = 0x9B;
			    reply.a_pintro = '?';

			    






			    if (screen->terminal_id < 200) {
				switch (screen->terminal_id) {
				case 102:
				    reply.a_param[count++] = 6; 
				    break;
				case 101:
				    reply.a_param[count++] = 1; 
				    reply.a_param[count++] = 0; 
				    break;
				default: 
				    reply.a_param[count++] = 1; 
				    reply.a_param[count++] = 2; 
				    break;
				}
			    } else {
				reply.a_param[count++] = 60 + screen->terminal_id/100;
				reply.a_param[count++] = 1; 
				reply.a_param[count++] = 2; 
				reply.a_param[count++] = 6; 

				if (sunKeyboard)

				reply.a_param[count++] = 8; 
				reply.a_param[count++] = 9; 
				reply.a_param[count++] = 15; 
				if(screen->colorMode) { reply.a_param[count++] = 22; }



















			    }
			    reply.a_nparam = count;
			    reply.a_inters = 0;
			    reply.a_final  = 'c';
			    unparseseq(&reply, screen->respond);
			}
			parsestate = groundtable;
			break;

		 case 103:
			
			if (param[0] <= 0) {	
				count = 0;
				reply.a_type   = 0x9B;
				reply.a_pintro = '>';

				if (screen->terminal_id >= 200)
					reply.a_param[count++] = 1; 
				else
					reply.a_param[count++] = 0; 
				reply.a_param[count++] = 0; 
				reply.a_param[count++] = 0; 
				reply.a_nparam = count;
				reply.a_inters = 0;
				reply.a_final  = 'c';
				unparseseq(&reply, screen->respond);
			}
			parsestate = groundtable;
			break;

		 case 105:
			
			if ((screen->terminal_id >= 400)
			 && (param[0] <= 0)) {	
				unparseputc1(0x90, screen->respond);
				unparseputc('!',  screen->respond);
				unparseputc('|',  screen->respond);
				unparseputc('0',  screen->respond);
				unparseputc1(0x9C,  screen->respond);
			}
			parsestate = groundtable;
			break;

		 case 34:
			
			if ((row = param[0]) <= 0) 
				TabClear(term->tabs, screen->cur_col);
			else if (row == 3)
				TabZonk(term->tabs);
			parsestate = groundtable;
			break;

		 case 35:
			
			ansi_modes(term, bitset);
			parsestate = groundtable;
			break;

		 case 36:
			
			ansi_modes(term, bitclr);
			parsestate = groundtable;
			break;

		 case 37:
			
			for (row=0; row<nparam; ++row) {
				 


				switch (param[row]) {
				 case -1:
				 case 0:
					term->flags &=
						~(0x01|0x04|0x08|0x02|0x40000);
					if(screen->colorMode) {reset_SGR_Colors();}
					break;
				 case 1:	
					term->flags |= 0x04;
					if(screen->colorMode) {setExtendedFG();}
					break;
				 case 5:	
					term->flags |= 0x08;
					if(screen->colorMode) {setExtendedFG();}
					break;
				 case 4:	
					term->flags |= 0x02;
					if(screen->colorMode) {setExtendedFG();}
					break;
				 case 7:
					term->flags |= 0x01;
					break;
				 case 8:
					term->flags |= 0x40000;
					break;
				 case 22: 
					term->flags &= ~0x04;
					if(screen->colorMode) {setExtendedFG();}
					break;
				 case 24:
					term->flags &= ~0x02;
					if(screen->colorMode) {setExtendedFG();}
					break;
				 case 25: 
					term->flags &= ~0x08;
					if(screen->colorMode) {setExtendedFG();}
					break;
				 case 27:
					term->flags &= ~0x01;
					break;
				 case 28:
					term->flags &= ~0x40000;
					break;
				 case 30:
				 case 31:
				 case 32:
				 case 33:
				 case 34:
				 case 35:
				 case 36:
				 case 37:
					if(screen->colorMode) { term->sgr_foreground = (param[row] - 30); setExtendedFG(); }



					break;
				 case 38:
					



					if(screen->colorMode) { row++; if (row < nparam) { switch(param[row]) { case 5: row++; if (row < nparam && param[row] < 16) { term->sgr_foreground = param[row]; setExtendedFG(); } break; default: row += 7; break; } } }

















					break;
				 case 39:
					if(screen->colorMode) { reset_SGR_Foreground(); }


					break;
				 case 40:
				 case 41:
				 case 42:
				 case 43:
				 case 44:
				 case 45:
				 case 46:
				 case 47:
					if(screen->colorMode) { SGR_Background(param[row] - 40); }


					break;
				 case 48:
					if(screen->colorMode) { row++; if (row < nparam) { switch(param[row]) { case 5: row++; if (row < nparam && param[row] < 16) { SGR_Background(param[row]); } break; default: row += 7; break; } } }
















					break;
				 case 49:
					if(screen->colorMode) { SGR_Background(-1); }


					break;
				 case 90:
				 case 91:
				 case 92:
				 case 93:
				 case 94:
				 case 95:
				 case 96:
				 case 97:
					if(screen->colorMode) { term->sgr_foreground = (param[row] - 90 + 8); setExtendedFG(); }



					break;
				 case 100:







				 case 101:
				 case 102:
				 case 103:
				 case 104:
				 case 105:
				 case 106:
				 case 107:
					if(screen->colorMode) { SGR_Background(param[row] - 100 + 8); }


					break;
				}
			}
			parsestate = groundtable;
			break;

			
		 case 97:
			private_function = 1;

			
		 case 38:
			count = 0;
			reply.a_type   = 0x9B;
			reply.a_pintro = private_function ? '?' : 0;
			reply.a_inters = 0;
			reply.a_final  = 'n';

			switch (param[0]) {
			case 5:
				
				reply.a_param[count++] = 0; 
				break;
			case 6:
				
				
				reply.a_param[count++] = screen->cur_row + 1;
				reply.a_param[count++] = screen->cur_col + 1;
				reply.a_final  = 'R';
				break;
			case 15:
				
				reply.a_param[count++] = 13; 
				break;
			case 25:
				
				reply.a_param[count++] = 20; 
				break;
			case 26:
				
				reply.a_param[count++] = 27;
				reply.a_param[count++] = 1; 
				if (screen->terminal_id >= 400) {
					reply.a_param[count++] = 0; 
					reply.a_param[count++] = 0; 
				}
				break;
			case 53:
				



				reply.a_param[count++] = 53; 

				break;
			}

			if ((reply.a_nparam = count) != 0)
				unparseseq(&reply, screen->respond);

			parsestate = groundtable;
			private_function = 0;
			break;

		 case 101:
			xtermMediaControl(param[0], 0);
			parsestate = groundtable;
			break;

		 case 113:
			xtermMediaControl(param[0], 1);
			parsestate = groundtable;
			break;

		 case 68:
		 case 69:
			if(screen->scroll_amt)
			    FlushScroll(screen);
			if (parsestate[c] == 68)
			    screen->top_marg = screen->cur_row;
			else
			    screen->top_marg = 0;
			parsestate = groundtable;
			break;

		 case 39:
			
			if((top = param[0]) < 1)
				top = 1;
			if(nparam < 2 || (bot = param[1]) == -1
			   || bot > screen->max_row + 1
			   || bot == 0)
				bot = screen->max_row+1;
			if (bot > top) {
				if(screen->scroll_amt)
					FlushScroll(screen);
				screen->top_marg = top-1;
				screen->bot_marg = bot-1;
				CursorSet(screen, 0, 0, term->flags);
			}
			parsestate = groundtable;
			break;

		 case 40:
			
			if (screen->terminal_id < 200) { 
			    if ((row = param[0]) == -1)
				row = 0;
			    if (row == 0 || row == 1) {
				reply.a_type = 0x9B;
				reply.a_pintro = 0;
				reply.a_nparam = 7;
				reply.a_param[0] = row + 2;
				reply.a_param[1] = 1;	
				reply.a_param[2] = 1;	
				reply.a_param[3] = 112;	
				reply.a_param[4] = 112;	
				reply.a_param[5] = 1;	
				reply.a_param[6] = 0;	
				reply.a_inters = 0;
				reply.a_final  = 'x';
				unparseseq(&reply, screen->respond);
			    }
			}
			parsestate = groundtable;
			break;

		 case 41:
			
			dpmodes(term, bitset);
			parsestate = groundtable;

			if(screen->TekEmu)
				return;

			break;

		 case 42:
			
			dpmodes(term, bitclr);

			if (screen->ansi_level == 0)
				groundtable = vt52_table;
			else if (screen->terminal_id >= 100)
				groundtable = ansi_table;

			parsestate = groundtable;
			break;

		 case 43:
			
			if(screen->cursor_state)
				HideCursor();
			for(row = screen->max_row ; row >= 0 ; row--) {
				__MEMSET((screen->visbuf[term->num_ptrs * (row) + OFF_ATTRS]), 0, col = screen->max_col + 1)
;
				for(cp = (screen->visbuf[term->num_ptrs * (row) + OFF_CHARS]) ; col > 0 ; col--)
					*cp++ = (Char)'E';
				 



			}
			ScrnRefresh(screen, 0, 0, screen->max_row + 1,
			 screen->max_col + 1, 0);
			parsestate = groundtable;
			break;

		 case 44:
			 
			screen->gsets[scstype] = c;
			parsestate = groundtable;
			break;

		 case 45:
			
			CursorSave(term);
			parsestate = groundtable;
			break;

		 case 46:
			
			CursorRestore(term);
			if(screen->colorMode) {setExtendedFG();}
			parsestate = groundtable;
			break;

		 case 47:
			
			term->keyboard.flags |= 0x02;
			UpdateMenuItem(vtMenuEntries[vtMenu_appkeypad].widget,(term->keyboard.flags & 0x02));
			parsestate = groundtable;
			break;

		 case 48:
			
			term->keyboard.flags &= ~0x02;
			UpdateMenuItem(vtMenuEntries[vtMenu_appkeypad].widget,(term->keyboard.flags & 0x02));
			parsestate = groundtable;
			break;

		 case 96:
			parsestate = csi_quo_table;
		 	break;

			



		 case 98:
			if (screen->terminal_id >= 100) {
				screen->ansi_level = 1;
				show_8bit_control(0);

				groundtable =
				parsestate = ansi_table;

			}
			break;
		 case 99:
			if (screen->terminal_id >= 200)
				screen->ansi_level = 2;
			break;
		 case 100:
			if (screen->terminal_id >= 300)
				screen->ansi_level = 3;
			break;

		 case 85:
			if (param[0] >= 61 && param[0] <= 63) {
				screen->ansi_level = param[0] - 60;
				if (param[0] > 61) {
					if (param[1] == 1)
						show_8bit_control(0);
					else if (param[1] == 0 || param[1] == 2)
						show_8bit_control(1);
				}
			}
			parsestate = groundtable;
		 	break;

		 case 86:
			screen->protected_mode = 1;
			if (param[0] <= 0 || param[0] == 2)
				term->flags &= ~0x40;
			else if (param[0] == 1)
				term->flags |= 0x40;
			parsestate = groundtable;
		 	break;

		 case 87:
			
			do_erase_display(screen, param[0], 1);
			parsestate = groundtable;
		 	break;

		 case 88:
			
			do_erase_line(screen, param[0], 1);
			parsestate = groundtable;
		 	break;

		 case 92:
			if (!string_used)
				break;
			string_area[--string_used] = '\0';
			switch (string_mode) {
			case 0x9F:
				
				break;
			case 0x90:
				do_dcs(string_area, string_used);
				break;
			case 0x9D:
				do_osc(string_area, string_used, 0x9C);
				break;
			case 0x9E:
				
				break;
			case 0x98:
				
				break;
			}
			parsestate = groundtable;
		 	break;

		 case 91:
			
			string_mode = 0x98;
			parsestate = sos_table;
			break;

		 case 90:
			
			string_mode = 0x9E;
			parsestate = sos_table;
			break;

		 case 89:
			
			string_mode = 0x90;
			parsestate = sos_table;
		 	break;

		 case 93:
			
			string_mode = 0x9F;
			parsestate = sos_table;
		 	break;

		 case 95:
			screen->protected_mode = 2;
			term->flags |= 0x40;
			parsestate = groundtable;
		 	break;

		 case 94:
			term->flags &= ~0x40;
			parsestate = groundtable;
		 	break;

		 case 79:
			
			if((count = param[0]) < 1)
				count = 1;
			Scroll(screen, count);
			parsestate = groundtable;
			break;

		 case 49:
			
			Index(screen, 1);
			do_xevents();
			parsestate = groundtable;
			break;

		 case 76:
			
			CursorPrevLine(screen, param[0]);
			parsestate = groundtable;
			break;

		 case 77:
			
			CursorNextLine(screen, param[0]);
			parsestate = groundtable;
			break;

		 case 50:
			
			Index(screen, 1);
			CarriageReturn(screen);
			do_xevents();
			parsestate = groundtable;
			break;

		 case 51:
			
			TabSet(term->tabs, screen->cur_col);
			parsestate = groundtable;
			break;

		 case 52:
			
			RevIndex(screen, 1);
			parsestate = groundtable;
			break;

		 case 53:
			
			screen->curss = 2;
			parsestate = groundtable;
			break;

		 case 54:
			
			screen->curss = 3;
			parsestate = groundtable;
			break;

		 case 55:
			
			nparam = 1;
			param[0] = -1;
			parsestate = csi_table;
			break;

		 case 83:
			
			parsestate = esc_sp_table;
			break;

		 case 108:
			
			parsestate = csi_ex_table;
			break;

# 2240 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"


		 case 81:
			show_8bit_control(0);
			parsestate = groundtable;
		 	break;

		 case 82:

			if (screen->ansi_level <= 1)
				break;

			show_8bit_control(1);
			parsestate = groundtable;
		 	break;

		 case 56:
			
			parsestate = sos_table;
			string_mode = 0x9D;
			break;

		 case 57:
			
			VTReset(1, 1);
			parsestate = groundtable;
			break;

		 case 109:
			
			VTReset(0, 0);
			parsestate = groundtable;
			break;

		 case 107:
			
			if (lastchar >= 0 &&
				groundtable[(lastchar)] == 63) {
			    IChar repeated[2];
			    count = (param[0] < 1) ? 1 : param[0];
			    repeated[0] = lastchar;
			    while (count-- > 0) {
				dotext(screen,
					screen->gsets[(int)(screen->curgl)],
					repeated, 1);
			    }
			}
			parsestate = groundtable;
			break;

		 case 58:
			
			screen->curgl = 2;
			parsestate = groundtable;
			break;

		 case 59:
			
			screen->curgl = 3;
			parsestate = groundtable;
			break;

		 case 60:
			
			screen->curgr = 3;
			parsestate = groundtable;
			break;

		 case 61:
			
			screen->curgr = 2;
			parsestate = groundtable;
			break;

		 case 62:
			
			screen->curgr = 1;
			parsestate = groundtable;
			break;

		 case 64:
			savemodes(term);
			parsestate = groundtable;
			break;

		 case 65:
			restoremodes(term);
			parsestate = groundtable;
			break;

		 case 73:
			window_ops(term);
			parsestate = groundtable;
			break;











	    }
	    if (parsestate == groundtable)
		    lastchar = thischar;
	}
}

static Char *v_buffer;		
static Char *v_bufstr = ((void *) 0);	
static Char *v_bufptr;		
static Char *v_bufend;		




int
v_write(int f, Char *data, int len)
{
	int riten;
	int c = len;

	if (v_bufstr == ((void *) 0)  &&  len > 0) {
	        v_buffer = (Char *) XtMalloc(len);
		v_bufstr = v_buffer;
		v_bufptr = v_buffer;
		v_bufend = v_buffer + len;
	}











	if (!(( &pty_mask)->fds_bits[(f)/(sizeof(fd_mask) * 8)] & (1 << ((f) % (sizeof(fd_mask) * 8)))))
		return(write(f, (char *)data, len));





	








	if (len > 0) {
	    if (v_bufend < v_bufptr + len) { 
		if (v_bufstr != v_buffer) {
		    
		    





		    __MEMMOVE( v_buffer, v_bufstr, v_bufptr - v_bufstr);
		    v_bufptr -= v_bufstr - v_buffer;
		    v_bufstr = v_buffer;
		}
		if (v_bufend < v_bufptr + len) {
		    
		    
		    int size = v_bufptr - v_buffer; 
		    v_buffer = (Char *)realloc(v_buffer, size + len);
		    if (v_buffer) {





			v_bufstr = v_buffer;
			v_bufptr = v_buffer + size;
			v_bufend = v_bufptr + len;
		    } else {
			
			fprintf((decc$ga_stderr), "%s: cannot allocate buffer space\n",
				xterm_name);
			v_buffer = v_bufstr; 
			c = 0;
		    }
		}
	    }
	    if (v_bufend >= v_bufptr + len) {
		
		__MEMMOVE( v_bufptr, data, len);
		v_bufptr += len;
	    }
	}

	
















	if (v_bufptr > v_bufstr) {

	    riten = write(f, v_bufstr, v_bufptr - v_bufstr <= 128 ?
			  	       v_bufptr - v_bufstr : 128);
	    if (riten < 0)





	    {



		riten = 0;
	    }







	    v_bufstr += riten;
	    if (v_bufstr >= v_bufptr) 
		v_bufstr = v_bufptr = v_buffer;
	}

	


	if (v_bufend - v_bufptr > 1024) { 
	    
	    int start = v_bufstr - v_buffer;
	    int size = v_bufptr - v_buffer;
	    int allocsize = size ? size : 1;

	    v_buffer = (Char *)realloc(v_buffer, allocsize);
	    if (v_buffer) {
		v_bufstr = v_buffer + start;
		v_bufptr = v_buffer + size;
		v_bufend = v_buffer + allocsize;




	    } else {
		
		v_buffer = v_bufstr - start; 
	    }
	}
	return(c);
}

static fd_set select_mask;
static fd_set write_mask;
static int pty_read_bytes;


in_put()
{
  int status;
  Dimension replyWidth, replyHeight;
  XtGeometryResult stat;

	register TScreen *screen = &term->screen;
	register char *cp;
	register int i;

	select_mask = pty_mask;	
	for ( ; ;)
	{

	  
	  if(tt_changed)
	    {	    
	      tt_changed = 0;
	      
	      stat = XtMakeResizeRequest (
					    (Widget) term, 
					    (Dimension) ((screen)->fullVwin.f_width)
					    * (tt_width)
					    + 2*screen->border
					    + screen->scrollbar,
					    (Dimension) ((screen)->fullVwin.f_height)
					    * (tt_length)
					    + 2 * screen->border,
					    &replyWidth, &replyHeight);

	      if (stat == XtGeometryYes || stat == XtGeometryDone)
		{
		  term->core.width = replyWidth;
		  term->core.height = replyHeight;
		  
		  ScreenResize (&term->screen,replyWidth,replyHeight,
				&term->flags);
		}
	    }

		if((select_mask & pty_mask) && (eventMode == NORMAL)) {
			if(screen->logging)
				 ;
			if (read_queue.flink != 0) {
				bcnt = tt_read(bptr = buffer);
       				if(bcnt == 0)
	       		       		Panic("input: read returned zero\n", 0);
       	       			else 
				{
	       				
	       				for(i = bcnt, cp = bptr ; i > 0 ; i--)
						*cp++ &= CHAR;
		       	     		if(screen->scrollWidget && screen->scrollttyoutput &&
		 				screen->topline < 0)
		 				
		  	      		 WindowScroll(screen, 0);
		       			break;
				}
			}
			else
				sys$hiber();

		}
		if(screen->scroll_amt)
			FlushScroll(screen);
		if(screen->cursor_set && (screen->cursor_col != screen->cur_col
		 || screen->cursor_row != screen->cur_row)) {
			if(screen->cursor_state)
				HideCursor();
			ShowCursor();
		} else if(screen->cursor_set != screen->cursor_state) {
			if(screen->cursor_set)
				ShowCursor();
			else
				HideCursor();
		}
		
		if ((((_XPrivDisplay)screen->display)->qlen)){
			select_mask = X_mask;
		}
		else {
			write_mask = ptymask();
			XFlush(screen->display);
			select_mask = Select_mask;
			if (eventMode != NORMAL)
				select_mask = X_mask;
		}
		if (write_mask & ptymask()) {
			v_write(screen->respond, 0, 0);	
		}

		if(select_mask & X_mask) {
			if (bcnt <= 0) {
				bcnt = 0;
				bptr = buffer;
			}
			xevents();
			if (bcnt > 0)
				break;
		}
	}
	bcnt--;
	return(*bptr++);
}
# 2751 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"






void
dotext(
	TScreen *screen,
	int	charset,
	IChar	*buf,		
	Cardinal len)		
{
	int this_col;		
	Cardinal n, next_col, offset, last_col;




	if (!xtermCharSetOut(buf, buf+len, charset))
		return;

	 








	for (offset = 0; offset < len; offset += this_col) {
		last_col = ((!(((((screen)->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) == 0) && !(((((screen)->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) > 3)) ? (screen->max_col / 2) : (screen->max_col));
		this_col = last_col - screen->cur_col +1;
		if (this_col <= 1) {
			if (screen->do_wrap && (term->flags & 0x400)) {
			    
			    (screen->visbuf[term->num_ptrs * (screen->cur_row + screen->topline) + OFF_FLAGS]) = (Char *)(((long)(screen->visbuf[term->num_ptrs * (screen->cur_row + screen->topline) + OFF_FLAGS]) | 0x01));
			    xtermAutoPrint('\n');
			    Index(screen, 1);
			    screen->cur_col = 0;
			    screen->do_wrap = 0;
			    this_col = last_col + 1;
			} else
			    this_col = 1;
		}
		if (offset + this_col > len) {
			this_col = len - offset;
		}
		next_col = screen->cur_col + this_col;

# 2837 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"

		WriteText(screen,  buf+offset

, this_col);


		




		screen->do_wrap = (screen->cur_col < (int)next_col);
	}
}











static int mapstate = -1;






static void
WriteText(TScreen *screen, Char *str, Cardinal len)
{
	unsigned flags	= term->flags;
	int	fg_bg = makeColorPair(term->cur_foreground, term->cur_background);
	GC	currentGC;

	 





	if(screen->cur_row - screen->topline <= screen->max_row) {
		if(screen->cursor_state)
			HideCursor();

		if (flags & 0x8000)
			InsertChar(screen, len);
		if (!AddToRefresh(screen)) {
			
			currentGC = updatedXtermGC(screen, flags, fg_bg, 0);

			if(screen->scroll_amt)
				FlushScroll(screen);

			if (flags & 0x40000) {
				__MEMSET(str, ' ', len);
				 



			}

			 



			drawXtermText(screen, flags, currentGC,
				((!(((((screen)->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) == 0) && !(((((screen)->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) > 3)) ? ((2*( screen->cur_col)) * ((screen)->fullVwin.f_width) + (((screen)->fullVwin.scrollbar) + screen->border)) : ((( screen->cur_col)) * ((screen)->fullVwin.f_width) + (((screen)->fullVwin.scrollbar) + screen->border))),
				(((( screen->cur_row) - screen->topline) * ((screen)->fullVwin.f_height)) + screen->border),
				(((!((((((&term->screen))->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) == 0) && !((((((&term->screen))->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) > 3))) ? ((((&term->screen))->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0]) : (term->screen).cur_chrset),
				str, len);

			resetXtermGC(screen, flags, 0);

			









		}
	}
	ScreenWrite(screen, str, flags, fg_bg, len);
	CursorForward(screen, len);

	

	if( zIconBeep && mapstate == 0 && ! zIconBeep_flagged) {
	    static char *icon_name;
	    static Arg args[] = {
		{ ((char*)&XtShellStrings[0]), (XtArgVal) &icon_name }
	    };

	    icon_name = ((void *) 0);
	    XtGetValues(toplevel,args,((Cardinal) (sizeof(args) / sizeof(args[0]))));

	    if( icon_name != ((void *) 0) ) {
		zIconBeep_flagged = 1;
		Changename(icon_name);
	    }
	    if (zIconBeep > 0)
		XBell( (((toplevel)->core.screen)->display), zIconBeep );
	}
	mapstate = -1;

}




static void HandleStructNotify(
	Widget w  ,
	XtPointer closure  ,
	XEvent *event,
	Boolean *cont  )
{
    static char *icon_name;
    static Arg args[] = {
	    { ((char*)&XtShellStrings[0]), (XtArgVal) &icon_name }
    };

    switch( event->type ){
    case 19:
	 

	if( zIconBeep_flagged ) {
	    zIconBeep_flagged = 0;
	    icon_name = ((void *) 0);
	    XtGetValues(toplevel, args, ((Cardinal) (sizeof(args) / sizeof(args[0]))));
	    if( icon_name != ((void *) 0) ) {
		char	*buf = (char *)malloc(__STRLEN(icon_name) + 1);
		if (buf == ((void *) 0)) {
			zIconBeep_flagged = 1;
			return;
		}
		__STRCPY(buf, icon_name + 4);
		Changename(buf);
		free(buf);
	    }
	}

	mapstate = !0;
	break;
    case 18:
	 
	mapstate = 0;
	break;
    case 22:
	 















	break;
    default:
	 
	break;
    }
}





static void
ansi_modes(
	XtermWidget	termw,
	void (*func) (unsigned *p, unsigned mask))
{
	register int	i;

	for (i=0; i<nparam; ++i) {
		switch (param[i]) {
		case 2:			
			(*func)(&termw->keyboard.flags, 0x01);
			break;

		case 4:			
			(*func)(&termw->flags, 0x8000);
			break;

		case 12:		
			(*func)(&termw->keyboard.flags, 0x08);
			break;

		case 20:		
			(*func)(&termw->flags, 0x2000);
			UpdateMenuItem(vtMenuEntries[vtMenu_autolinefeed].widget,(term->flags & 0x2000));
			break;
		}
	}
}







static void
dpmodes(
	XtermWidget	termw,
	void (*func) (unsigned *p, unsigned mask))
{
	register TScreen	*screen	= &termw->screen;
	register int	i, j;

	for (i=0; i<nparam; ++i) {
		 
		switch (param[i]) {
		case 1:			
			(*func)(&termw->keyboard.flags, 0x04);
			UpdateMenuItem(vtMenuEntries[vtMenu_appcursor].widget,(term->keyboard.flags & 0x04));
			break;
		case 2:			
			if (func == bitset) {	
				resetCharsets(screen);
				if(screen->ansi_level == 0) { screen->ansi_level = 1;}

			}

			else if (screen->terminal_id >= 100) {	
				screen->ansi_level = 0;
				param[0] = 0;
				param[1] = 0;
				resetCharsets(screen);
			}

			break;
		case 3:			
			if(screen->c132) {
				ClearScreen(screen);
				CursorSet(screen, 0, 0, termw->flags);
				if((j = func == bitset ? 132 : 80) !=
				 ((termw->flags & 0x20000) ? 132 : 80) ||
				 j != screen->max_col + 1)
					RequestResize(termw, -1, j, 1);
				(*func)(&termw->flags, 0x20000);
			}
			break;
		case 4:			
			if (func == bitset) {
				screen->jumpscroll = 0;
				if (screen->scroll_amt)
					FlushScroll(screen);
			} else
				screen->jumpscroll = 1;
			(*func)(&termw->flags, 0x10000);
			UpdateMenuItem(vtMenuEntries[vtMenu_jumpscroll].widget,term->screen.jumpscroll);
			break;
		case 5:			
			j = termw->flags;
			(*func)(&termw->flags, 0x1000);
			if ((termw->flags ^ j) & 0x1000)
				ReverseVideo(termw);
			
			break;

		case 6:			
			(*func)(&termw->flags, 0x4000);
			CursorSet(screen, 0, 0, termw->flags);
			break;

		case 7:			
			(*func)(&termw->flags, 0x400);
			UpdateMenuItem(vtMenuEntries[vtMenu_autowrap].widget,(term->flags & 0x400));
			break;
		case 8:			
			
			break;
		case 9:     
			if (( screen)->event_mask & (1L<<6)) {	( screen)->event_mask |= (1L<<13);	( screen)->event_mask &= ~(1L<<6);	XSelectInput((((( termw))->core.screen)->display), ((( termw))->core.window), ( screen)->event_mask);	};
			screen->send_mouse_pos = (func == bitset) ? X10_MOUSE : MOUSE_OFF;
			break;
		case 18:		
			screen->printer_formfeed = (func == bitset) ? 1 : 0;
			break;
		case 19:		
			screen->printer_extent = (func == bitset) ? 1 : 0;
			break;
		case 25:		
			screen->cursor_set = (func == bitset) ? 1 : 0;
			break;
		case 30:		
			if (screen->fullVwin.scrollbar != ((func == bitset) ? 1 : 0))
				ToggleScrollBar(termw);
			break;

		case 35:		
			term->misc.shift_keys = (func == bitset) ? 1 : 0;
			break;

		case 38:		

			if(func == bitset && !(screen->inhibit & 0x04)) {






				screen->TekEmu = 1;
			}

			break;
		case 40:		
			screen->c132 = (func == bitset);
			UpdateMenuItem(vtMenuEntries[vtMenu_allow132].widget,term->screen.c132);
			break;
		case 41:		
			screen->curses = (func == bitset);
			UpdateMenuItem(vtMenuEntries[vtMenu_cursesemul].widget,term->screen.curses);
			break;
		case 42:		
			(*func)(&termw->flags, 0x100000);
			break;
		case 44:		
			screen->marginbell = (func == bitset);
			if(!screen->marginbell)
				screen->bellarmed = -1;
			UpdateMenuItem(vtMenuEntries[vtMenu_marginbell].widget,term->screen.marginbell);
			break;
		case 45:		
			(*func)(&termw->flags, 0x800);
			UpdateMenuItem(vtMenuEntries[vtMenu_reversewrap].widget,(term->flags & 0x800));
			break;

















		case 1049:		
			if (!termw->misc.titeInhibit) {
			    if(func == bitset) {
				CursorSave(termw);
				ToAlternate(screen);
				ClearScreen(screen);
			    } else {
				FromAlternate(screen);
				CursorRestore(termw);
			    }
			}
			break;
		case 1047:
		case 47:		
			if (!termw->misc.titeInhibit) {
			    if(func == bitset) {
				ToAlternate(screen);
			    } else {
				if (screen->alternate
				 && (param[i] == 1047))
				    ClearScreen(screen);
				FromAlternate(screen);
			    }
			}
			break;
		case 66:	
			(*func)(&termw->keyboard.flags, 0x02);
			UpdateMenuItem(vtMenuEntries[vtMenu_appkeypad].widget,(term->keyboard.flags & 0x02));
			break;
		case 67:	
			
			(*func)(&termw->keyboard.flags, 0x10);
			UpdateMenuItem(mainMenuEntries[mainMenu_backarrow].widget,term->keyboard.flags & 0x10);
			break;
		case 1000:	
			if (( screen)->event_mask & (1L<<6)) {	( screen)->event_mask |= (1L<<13);	( screen)->event_mask &= ~(1L<<6);	XSelectInput((((( termw))->core.screen)->display), ((( termw))->core.window), ( screen)->event_mask);	};
			screen->send_mouse_pos = (func == bitset) ? VT200_MOUSE : MOUSE_OFF;
			break;
		case 1001: 
			if (( screen)->event_mask & (1L<<6)) {	( screen)->event_mask |= (1L<<13);	( screen)->event_mask &= ~(1L<<6);	XSelectInput((((( termw))->core.screen)->display), ((( termw))->core.window), ( screen)->event_mask);	};
			screen->send_mouse_pos = (func == bitset) ? VT200_HIGHLIGHT_MOUSE : MOUSE_OFF;
			break;
		case 1002:
			if (( screen)->event_mask & (1L<<6)) {	( screen)->event_mask |= (1L<<13);	( screen)->event_mask &= ~(1L<<6);	XSelectInput((((( termw))->core.screen)->display), ((( termw))->core.window), ( screen)->event_mask);	};
			screen->send_mouse_pos = (func == bitset) ? BTN_EVENT_MOUSE : MOUSE_OFF;
			break;
		case 1003:
			screen->send_mouse_pos = (func == bitset) ? ANY_EVENT_MOUSE : MOUSE_OFF;
			if (screen->send_mouse_pos == MOUSE_OFF) {
				screen->event_mask |=   (1L<<13);
				screen->event_mask &= ~(1L<<6);
			} else {
				screen->event_mask &= ~(1L<<13);
				screen->event_mask |= (1L<<6);
			}
			XSelectInput((((termw)->core.screen)->display), ((term)->core.window), screen->event_mask);
			break;
		case 1010:	
			screen->scrollttyoutput = (func == bitset) ? 1 : 0;
			UpdateMenuItem(vtMenuEntries[vtMenu_scrollttyoutput].widget,term->screen.scrollttyoutput);
			break;
		case 1011:	
			screen->scrollkey = (func == bitset) ? 1 : 0;
			UpdateMenuItem(vtMenuEntries[vtMenu_scrollkey].widget,term->screen.scrollkey);
			break;

		case 1035:
			term->misc.real_NumLock = (func == bitset) ? 1 : 0;
			break;
		case 1036:
			screen->meta_sends_esc = (func == bitset) ? 1 : 0;
			break;

		case 1048:
			if (!termw->misc.titeInhibit) {
		        	if(func == bitset)
					CursorSave(termw);
				else
					CursorRestore(termw);
			}
			break;
		case 1051:
			sunFunctionKeys = (func == bitset);
			UpdateMenuItem(mainMenuEntries[mainMenu_sun_fkeys].widget,sunFunctionKeys);
			break;







		case 1061:
			sunKeyboard = (func == bitset);
			UpdateMenuItem(mainMenuEntries[mainMenu_sun_kbd].widget,sunKeyboard);
			break;

		}
	}
}




static void
savemodes(XtermWidget termw)
{
	register TScreen	*screen	= &termw->screen;
	register int i;

	for (i = 0; i < nparam; i++) {
		switch (param[i]) {
		case 1:			
			screen->save_modes[DP_DECCKM] = termw->keyboard.flags & 0x04;
			break;
		case 3:			
			if(screen->c132)
			    screen->save_modes[DP_DECCOLM] = termw->flags & 0x20000;
			break;
		case 4:			
			screen->save_modes[DP_DECSCLM] = termw->flags & 0x10000;
			break;
		case 5:			
			screen->save_modes[DP_DECSCNM] = termw->flags & 0x1000;
			break;
		case 6:			
			screen->save_modes[DP_DECOM] = termw->flags & 0x4000;
			break;

		case 7:			
			screen->save_modes[DP_DECAWM] = termw->flags & 0x400;
			break;
		case 8:			
			
			break;
		case 9:     
			screen->save_modes[DP_X_X10MSE] = screen->send_mouse_pos;
			break;
		case 40:		
			screen->save_modes[DP_X_DECCOLM] = screen->c132;
			break;
		case 41:		
			screen->save_modes[DP_X_MORE] = screen->curses;
			break;
		case 44:		
			screen->save_modes[DP_X_MARGIN] = screen->marginbell;
			break;
		case 45:		
			screen->save_modes[DP_X_REVWRAP] = termw->flags & 0x800;
			break;





		case 1047:		
			
		case 47:		
			screen->save_modes[DP_X_ALTSCRN] = screen->alternate;
			break;
		case 1000:   
		case 1001:
		case 1002:
		case 1003:
			screen->save_modes[DP_X_MOUSE] = screen->send_mouse_pos;
			break;
		case 1048:
			if (!termw->misc.titeInhibit) {
				CursorSave(termw);
			}
			break;
		}
	}
}




static void
restoremodes(XtermWidget termw)
{
	register TScreen	*screen	= &termw->screen;
	register int i, j;

	for (i = 0; i < nparam; i++) {
		switch (param[i]) {
		case 1:			
			bitcpy(&termw->keyboard.flags,
				screen->save_modes[DP_DECCKM], 0x04);
			UpdateMenuItem(vtMenuEntries[vtMenu_appcursor].widget,(term->keyboard.flags & 0x04));
			break;
		case 3:			
			if(screen->c132) {
				ClearScreen(screen);
				CursorSet(screen, 0, 0, termw->flags);
				if((j = (screen->save_modes[DP_DECCOLM] & 0x20000)
				 ? 132 : 80) != ((termw->flags & 0x20000)
				 ? 132 : 80) || j != screen->max_col + 1)
					RequestResize(termw, -1, j, 1);
				bitcpy(&termw->flags,
					screen->save_modes[DP_DECCOLM],
					0x20000);
			}
			break;
		case 4:			
			if (screen->save_modes[DP_DECSCLM] & 0x10000) {
				screen->jumpscroll = 0;
				if (screen->scroll_amt)
					FlushScroll(screen);
			} else
				screen->jumpscroll = 1;
			bitcpy(&termw->flags, screen->save_modes[DP_DECSCLM], 0x10000);
			UpdateMenuItem(vtMenuEntries[vtMenu_jumpscroll].widget,term->screen.jumpscroll);
			break;
		case 5:			
			if((screen->save_modes[DP_DECSCNM] ^ termw->flags) & 0x1000) {
				bitcpy(&termw->flags, screen->save_modes[DP_DECSCNM], 0x1000);
				ReverseVideo(termw);
				
			}
			break;
		case 6:			
			bitcpy(&termw->flags, screen->save_modes[DP_DECOM], 0x4000);
			CursorSet(screen, 0, 0, termw->flags);
			break;

		case 7:			
			bitcpy(&termw->flags, screen->save_modes[DP_DECAWM], 0x400);
			UpdateMenuItem(vtMenuEntries[vtMenu_autowrap].widget,(term->flags & 0x400));
			break;
		case 8:			
			
			break;
		case 9:     
			 screen->send_mouse_pos = screen->save_modes[DP_X_X10MSE];
			break;
		case 40:		
			 screen->c132 = screen->save_modes[DP_X_DECCOLM];
			UpdateMenuItem(vtMenuEntries[vtMenu_allow132].widget,term->screen.c132);
			break;
		case 41:		
			 screen->curses = screen->save_modes[DP_X_MORE];
			UpdateMenuItem(vtMenuEntries[vtMenu_cursesemul].widget,term->screen.curses);
			break;
		case 44:		
			if(( screen->marginbell = screen->save_modes[DP_X_MARGIN]) == 0)
				screen->bellarmed = -1;
			UpdateMenuItem(vtMenuEntries[vtMenu_marginbell].widget,term->screen.marginbell);
			break;
		case 45:		
			bitcpy(&termw->flags, screen->save_modes[DP_X_REVWRAP], 0x800);
			UpdateMenuItem(vtMenuEntries[vtMenu_reversewrap].widget,(term->flags & 0x800));
			break;











		case 1047:		
			
		case 47:		
			if (!termw->misc.titeInhibit) {
			    if(screen->save_modes[DP_X_ALTSCRN])
				ToAlternate(screen);
			    else
				FromAlternate(screen);
			    
			}
			break;
		case 1000:   
		case 1001:
		case 1002:
		case 1003:
			 screen->send_mouse_pos = screen->save_modes[DP_X_MOUSE];
			break;
		case 1048:
			if (!termw->misc.titeInhibit) {
				CursorRestore(termw);
			}
			break;
		}
	}
}





static void
report_win_label(
	TScreen	*screen,
	int code,
	XTextProperty *text,
	int ok)
{
	char **list;
	int length = 0;

	reply.a_type = 0x1B;
	unparseputc(0x1B, screen->respond);
	unparseputc(']', screen->respond);
	unparseputc(code, screen->respond);

	if (ok) {
		if (XTextPropertyToStringList(text, &list, &length)) {
			int n, c;
			for (n = 0; n < length; n++) {
				char *s = list[n];
				while ((c = *s++) != '\0')
					unparseputc(c, screen->respond);
			}
			XFreeStringList(list);
		}
		if (text->value != 0)
			XFree(text->value);
	}

	unparseputc(0x1B, screen->respond);
	unparseputc('\\', screen->respond);
}




static void
window_ops(XtermWidget termw)
{
	register TScreen	*screen	= &termw->screen;
	XWindowChanges values;
	XWindowAttributes win_attrs;
	XTextProperty text;
	unsigned int value_mask;
	Position x, y;
	unsigned root_width, root_height;

	switch (param[0]) {
	case 1:		
		XMapWindow(screen->display,
			((((term)->core.parent))->core.window));
		break;

	case 2:		
		XIconifyWindow(screen->display,
			((((term)->core.parent))->core.window),
			(((_XPrivDisplay)screen->display)->default_screen));
		break;

	case 3:		
		values.x   = param[1];
		values.y   = param[2];
		value_mask = ((1<<0) | (1<<1));
		XReconfigureWMWindow(
			screen->display,
			((((term)->core.parent))->core.window),
			(((_XPrivDisplay)screen->display)->default_screen),
			value_mask,
			&values);
		break;

	case 4:		
		RequestResize(termw, param[1], param[2], 0);
		break;

	case 5:		
		XRaiseWindow(screen->display, ((((term)->core.parent))->core.window));
		break;

	case 6:		
		XLowerWindow(screen->display, ((((term)->core.parent))->core.window));
		break;

	case 7:		
		Redraw();
		break;

	case 8:		
		RequestResize(termw, param[1], param[2], 1);
		break;


	case 9:		
		RequestMaximize(termw, param[1]);
		break;


	case 11:	
		XGetWindowAttributes(screen->display,
			((screen)->fullVwin.window),
			&win_attrs);
		reply.a_type = 0x9B;
		reply.a_pintro = 0;
		reply.a_nparam = 1;
		reply.a_param[0] = (win_attrs.map_state == 2) ? 1 : 2;
		reply.a_inters = 0;
		reply.a_final  = 't';
		unparseseq(&reply, screen->respond);
		break;

	case 13:	
		XtTranslateCoords(toplevel, 0, 0, &x, &y);
		reply.a_type = 0x9B;
		reply.a_pintro = 0;
		reply.a_nparam = 3;
		reply.a_param[0] = 3;
		reply.a_param[1] = x;
		reply.a_param[2] = y;
		reply.a_inters = 0;
		reply.a_final  = 't';
		unparseseq(&reply, screen->respond);
		break;

	case 14:	
		XGetWindowAttributes(screen->display,
			((screen)->fullVwin.window),
			&win_attrs);
		reply.a_type = 0x9B;
		reply.a_pintro = 0;
		reply.a_nparam = 3;
		reply.a_param[0] = 4;
		



		reply.a_param[1] = ((screen)->fullVwin.height);
		reply.a_param[2] = ((screen)->fullVwin.width);
		reply.a_inters = 0;
		reply.a_final  = 't';
		unparseseq(&reply, screen->respond);
		break;

	case 18:	
		reply.a_type = 0x9B;
		reply.a_pintro = 0;
		reply.a_nparam = 3;
		reply.a_param[0] = 8;
		reply.a_param[1] = screen->max_row + 1;
		reply.a_param[2] = screen->max_col + 1;
		reply.a_inters = 0;
		reply.a_final  = 't';
		unparseseq(&reply, screen->respond);
		break;


	case 19:	
		if (!QueryMaximize(screen, &root_height, &root_width)) {
		    root_height = 0;
		    root_width  = 0;
		}
		reply.a_type = 0x9B;
		reply.a_pintro = 0;
		reply.a_nparam = 3;
		reply.a_param[0] = 9;
		reply.a_param[1] = root_height / ((screen)->fullVwin.f_height);
		reply.a_param[2] = root_width / ((screen)->fullVwin.f_width);
		reply.a_inters = 0;
		reply.a_final  = 't';
		unparseseq(&reply, screen->respond);
		break;


	case 20:	
		report_win_label(screen, 'L', &text,
			XGetWMIconName(
				screen->display,
				((((term)->core.parent))->core.window),
				&text));
		break;

	case 21:	
		report_win_label(screen, 'l', &text,
			XGetWMName(
				screen->display,
				((((term)->core.parent))->core.window),
				&text));
		break;

	default: 
		if (param[0] >= 24)
			RequestResize(termw, param[0], -1, 1);
		break;
	}
}




static void bitset(unsigned *p, unsigned mask)
{
	*p |= mask;
}




static void bitclr(unsigned *p, unsigned mask)
{
	*p &= ~mask;
}




static void bitcpy(unsigned *p, unsigned q, unsigned mask)
{
	bitclr(p, mask);
	bitset(p, q & mask);
}

void
unparseputc1(int c, int fd)
{
	if (c >= 0x80 && c <= 0x9F) {
		if (!term->screen.control_eight_bits) {
			unparseputc((0x1B), fd);
			c = (c-0x40);
		}
	}
	unparseputc(c, fd);
}

void
unparseseq(register ANSI *ap, int fd)
{
	register int	c;
	register int	i;
	register int	inters;

	unparseputc1(c = ap->a_type, fd);
	if (c==0x1B || c==0x90 || c==0x9B || c==0x9D || c==0x9E || c==0x9F || c==0x8F) {
		if (ap->a_pintro != 0)
			unparseputc((char) ap->a_pintro, fd);
		for (i=0; i<ap->a_nparam; ++i) {
			if (i != 0)
				unparseputc(';', fd);
			unparseputn((unsigned int) ap->a_param[i], fd);
		}
		inters = ap->a_inters;
		for (i=3; i>=0; --i) {
			c = ((inters >> (8*i)) & 0xff);
			if (c != 0)
				unparseputc(c, fd);
		}
		unparseputc((char) ap->a_final, fd);
	}
}

static void
unparseputn(unsigned int n, int fd)
{
	unsigned int	q;

	q = n/10;
	if (q != 0)
		unparseputn(q, fd);
	unparseputc((char) ('0' + (n%10)), fd);
}

void
unparseputc(int c, int fd)
{
	register TScreen *screen = &term->screen;
	IChar	buf[2];
	register int i = 1;




	if((buf[0] = c) == '\r' && (term->flags & 0x2000)) {
		buf[1] = '\n';
		i++;
	}
	v_write(fd, buf, i);

	
	if ((term->keyboard.flags & 0x08) == 0) {
		dotext(screen, screen->gsets[(int)(screen->curgl)], buf, i);
	}
}

void
unparseputs(char *s, int fd)
{
	while (*s)
		unparseputc(*s++, fd);
}


void
ToggleCursorBlink(register TScreen *screen)
{
	ShowCursor();
	if (screen->cursor_blink) {
		screen->cursor_blink = 0;
		StopBlinking(screen);
	} else {
		screen->cursor_blink = 1;
		StartBlinking(screen);
	}
	UpdateMenuItem(vtMenuEntries[vtMenu_cursorblink].widget,term->screen.cursor_blink);
}


void
ToggleAlternate(register TScreen *screen)
{
	if (screen->alternate)
		FromAlternate(screen);
	else
		ToAlternate(screen);
}

static void
ToAlternate(register TScreen *screen)
{
	if(screen->alternate)
		return;
	 
	if(!screen->altbuf)
		screen->altbuf = Allocate(screen->max_row + 1, screen->max_col
		 + 1, &screen->abuf_address);
	SwitchBufs(screen);
	screen->alternate = 1;
	UpdateMenuItem(vtMenuEntries[vtMenu_altscreen].widget,term->screen.alternate);
}

static void
FromAlternate(register TScreen *screen)
{
	if(!screen->alternate)
		return;
	 
	screen->alternate = 0;
	SwitchBufs(screen);
	UpdateMenuItem(vtMenuEntries[vtMenu_altscreen].widget,term->screen.alternate);
}

static void
SwitchBufs(register TScreen *screen)
{
	register int rows, top;

	if(screen->cursor_state)
		HideCursor();

	rows = screen->max_row + 1;
	SwitchBufPtrs(screen);
	TrackText(0, 0, 0, 0);	

	if((top = -screen->topline) < rows) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		if(top == 0)
			XClearWindow(screen->display, ((screen)->fullVwin.window));
		else
			XClearArea(
			    screen->display,
			    ((screen)->fullVwin.window),
			    (int) (((screen)->fullVwin.scrollbar) + screen->border),
			    (int) top * ((screen)->fullVwin.f_height) + screen->border,
			    (unsigned) ((screen)->fullVwin.width),
			    (unsigned) (rows - top) * ((screen)->fullVwin.f_height),
			    0);
	}
	ScrnRefresh(screen, 0, 0, rows, screen->max_col + 1, 0);
}


void
SwitchBufPtrs(register TScreen *screen)
{
    size_t len = ScrnPointers(screen, screen->max_row + 1);

    __MEMCPY( (char *)screen->save_ptr, (char *)screen->visbuf, len);
    __MEMCPY( (char *)screen->visbuf, (char *)screen->altbuf, len);
    __MEMCPY( (char *)screen->altbuf, (char *)screen->save_ptr, len);
}

void
VTRun(void)
{
	register TScreen *screen = &term->screen;

	register int i;


	if (!screen->Vshow) {
	    set_vt_visibility (1);
	}
	{ UpdateMenuItem(vtMenuEntries[vtMenu_tekmode].widget,term->screen.TekEmu); UpdateMenuItem(tekMenuEntries[tekMenu_vtmode].widget,!term->screen.TekEmu); };
	UpdateMenuItem(tekMenuEntries[tekMenu_vtshow].widget,term->screen.Vshow);
	UpdateMenuItem(vtMenuEntries[vtMenu_tekshow].widget,term->screen.Tshow);
	SetItemSensitivity(vtMenuEntries[vtMenu_vthide].widget,term->screen.Tshow);

	if (screen->allbuf == ((void *) 0)) VTallocbuf ();

	screen->cursor_state = 0;
	screen->cursor_set = 1;
	StartBlinking(screen);

	initPtyData(&VTbuffer);

	while(Tpushb > Tpushback) {
		*(VTbuffer.ptr)++ = *--Tpushb;
		VTbuffer.cnt++;
	}
	VTbuffer.cnt += (i = (Tbuffer ? Tbuffer->cnt : 0));
	for( ; i > 0 ; i--)
		*(VTbuffer.ptr)++ = *(Tbuffer->ptr)++;
	VTbuffer.ptr = (&VTbuffer)->buf;

	if(!(c$$establish(0, 32 ),LIB$GET_CURRENT_INVO_CONTEXT(VTend)))
		VTparse();
	StopBlinking(screen);
	HideCursor();
	screen->cursor_set = 0;
}


static void VTExpose(
	Widget w  ,
	XEvent *event,
	Region region  )
{
	register TScreen *screen = &term->screen;





	if (event->type == 12)
		HandleExposure (screen, event);
}

static void VTGraphicsOrNoExpose (XEvent *event)
{
	register TScreen *screen = &term->screen;
	if (screen->incopy <= 0) {
		screen->incopy = 1;
		if (screen->scrolls > 0)
			screen->scrolls--;
	}
	if (event->type == 13)
	  if (HandleExposure (screen, event))
		screen->cursor_state = 0;
	if ((event->type == 14)
	 || ((XGraphicsExposeEvent *)event)->count == 0) {
		if (screen->incopy <= 0 && screen->scrolls > 0)
			screen->scrolls--;
		if (screen->scrolls)
			screen->incopy = -1;
		else
			screen->incopy = 0;
	}
}


static void VTNonMaskableEvent (
	Widget w  ,
	XtPointer closure  ,
	XEvent *event,
	Boolean *cont  )
{
    switch (event->type) {
       case 13:
       case 14:
	  VTGraphicsOrNoExpose (event);
	  break;
      }
}




static void VTResize(Widget w)
{
    if ((XtWindowOfObject(w) != 0L))
      ScreenResize (&term->screen, term->core.width, term->core.height,
		    &term->flags);
}


static void RequestResize(
	XtermWidget termw,
	int rows,
	int cols,
	int text)
{
	register TScreen	*screen	= &termw->screen;
	Dimension replyWidth, replyHeight;
	Dimension askedWidth, askedHeight;
	XtGeometryResult status;
	XWindowAttributes attrs;

	 

	askedWidth  = cols;
	askedHeight = rows;

	if (askedHeight == 0
	 || askedWidth  == 0) {
		XGetWindowAttributes((((termw)->core.screen)->display),
			((((termw)->core.screen))->root), &attrs);
	}

	if (text) {
		if (rows != 0) {
			if (rows < 0)
				askedHeight = screen->max_row + 1;
			askedHeight *= ((screen)->fullVwin.f_height);
			askedHeight += (2 * screen->border);
		}

		if (cols != 0) {
			if (cols < 0)
				askedWidth = screen->max_col + 1;
			askedWidth  *= ((screen)->fullVwin.f_width);
			askedWidth  += (2 * screen->border) + ((screen)->fullVwin.scrollbar);
		}

	} else {
		if (rows < 0)
			askedHeight = ((screen)->fullVwin.fullheight);
		if (cols < 0)
			askedWidth = ((screen)->fullVwin.fullwidth);
	}

	if (rows == 0)
		askedHeight = attrs.height;
	if (cols == 0)
		askedWidth  = attrs.width;

	status = XtMakeResizeRequest (
	    (Widget) termw,
	     askedWidth,  askedHeight,
	    &replyWidth, &replyHeight);

	if (status == XtGeometryYes ||
	    status == XtGeometryDone) {
	    ScreenResize (&termw->screen,
			  replyWidth,
			  replyHeight,
			  &termw->flags);
	    XSync(screen->display, 0);	
	    if(XtAppPending(app_con))
		xevents();
	}
}

extern Atom wm_delete_window;	

static String xterm_trans =
    "<ClientMessage>WM_PROTOCOLS: DeleteWindow()\n     <MappingNotify>: KeyboardMapping()\n"
;

int VTInit (void)
{
    register TScreen *screen = &term->screen;
    Widget vtparent = ((term)->core.parent);

    XtRealizeWidget (vtparent);
    XtOverrideTranslations(vtparent, XtParseTranslationTable(xterm_trans));
    (void) XSetWMProtocols ((((vtparent)->core.screen)->display), ((vtparent)->core.window),
			    &wm_delete_window, 1);

    if (screen->allbuf == ((void *) 0)) VTallocbuf ();
    return (1);
}

static void VTallocbuf (void)
{
    register TScreen *screen = &term->screen;
    int nrows = screen->max_row + 1;

    
    if (screen->scrollWidget)
      nrows += screen->savelines;
    screen->allbuf = Allocate (nrows, screen->max_col + 1,
     &screen->sbuf_address);
    if (screen->scrollWidget)
      screen->visbuf = &screen->allbuf[term->num_ptrs * screen->savelines];
    else
      screen->visbuf = screen->allbuf;
    return;
}

static void VTClassInit (void)
{
    XtAddConverter(((char*)&XtStrings[1797]), "Gravity", XmuCvtStringToGravity,
		   (XtConvertArgList) ((void *) 0), (Cardinal) 0);
}



static void VTInitialize (
	Widget wrequest,
	Widget new_arg,
	ArgList args  ,
	Cardinal *num_args  )
{
   XtermWidget request = (XtermWidget) wrequest;
   XtermWidget wnew    = (XtermWidget) new_arg;
   Widget my_parent = ((wnew)->core.parent);
   int i;

   Boolean color_ok;

   char *s;

   



   __MEMSET((char *) &wnew->screen, 0, sizeof(wnew->screen));

   




   wnew->core.height = wnew->core.width = 1;

   




   wnew->screen.display = wnew->core.screen->display;

   






   if (request->misc.re_verse) {
	wnew->dft_foreground = ((&((_XPrivDisplay)wnew->screen.display)->screens[(((_XPrivDisplay)wnew->screen.display)->default_screen)])->white_pixel);
	wnew->dft_background = ((&((_XPrivDisplay)wnew->screen.display)->screens[(((_XPrivDisplay)wnew->screen.display)->default_screen)])->black_pixel);
   } else {
	wnew->dft_foreground = ((&((_XPrivDisplay)wnew->screen.display)->screens[(((_XPrivDisplay)wnew->screen.display)->default_screen)])->black_pixel);
	wnew->dft_background = ((&((_XPrivDisplay)wnew->screen.display)->screens[(((_XPrivDisplay)wnew->screen.display)->default_screen)])->white_pixel);
   }
    
    
    
    
    

   wnew->screen.mouse_button = -1;
   wnew->screen.mouse_row = -1;
   wnew->screen.mouse_col = -1;

   wnew->screen.c132 = request->screen.c132;
   wnew->screen.curses = request->screen.curses;
   wnew->screen.hp_ll_bc = request->screen.hp_ll_bc;






   wnew->screen.foreground = request->screen.foreground;
   wnew->screen.cursorcolor = request->screen.cursorcolor;

   wnew->screen.cursor_blink = request->screen.cursor_blink;
   wnew->screen.cursor_on = request->screen.cursor_on;
   wnew->screen.cursor_off = request->screen.cursor_off;

   wnew->screen.border = request->screen.border;
   wnew->screen.jumpscroll = request->screen.jumpscroll;
   wnew->screen.old_fkeys = request->screen.old_fkeys;



   wnew->screen.marginbell = request->screen.marginbell;
   wnew->screen.mousecolor = request->screen.mousecolor;
   wnew->screen.mousecolorback = request->screen.mousecolorback;
   wnew->screen.multiscroll = request->screen.multiscroll;
   wnew->screen.nmarginbell = request->screen.nmarginbell;
   wnew->screen.savelines = request->screen.savelines;
   wnew->screen.scrolllines = request->screen.scrolllines;
   wnew->screen.scrollttyoutput = request->screen.scrollttyoutput;
   wnew->screen.scrollkey = request->screen.scrollkey;

   wnew->screen.term_id = request->screen.term_id;
   for (s = request->screen.term_id; *s; s++) {
	if (!((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(*s)]&0x100:(isalpha)(*s)))
	    break;
   }
   wnew->screen.terminal_id = atoi(s);
   if (wnew->screen.terminal_id < 52)
       wnew->screen.terminal_id = 52;
   if (wnew->screen.terminal_id > 420)
       wnew->screen.terminal_id = 420;
    



   wnew->screen.ansi_level = (wnew->screen.terminal_id / 100);
   wnew->screen.visualbell = request->screen.visualbell;

   wnew->misc.real_NumLock = request->misc.real_NumLock;
   wnew->misc.num_lock = 0;
   wnew->misc.alt_left = 0;
   wnew->misc.alt_right = 0;
   wnew->misc.meta_trans = 0;
   wnew->misc.meta_left = 0;
   wnew->misc.meta_right = 0;


   wnew->misc.shift_keys = request->misc.shift_keys;


   wnew->misc.tekInhibit = request->misc.tekInhibit;
   wnew->misc.tekSmall = request->misc.tekSmall;
   wnew->screen.TekEmu = request->screen.TekEmu;

   wnew->misc.re_verse0 =
   wnew->misc.re_verse = request->misc.re_verse;
   wnew->screen.multiClickTime = request->screen.multiClickTime;
   wnew->screen.bellSuppressTime = request->screen.bellSuppressTime;
   wnew->screen.charClass = request->screen.charClass;
   wnew->screen.cutNewline = request->screen.cutNewline;
   wnew->screen.cutToBeginningOfLine = request->screen.cutToBeginningOfLine;
   wnew->screen.highlight_selection = request->screen.highlight_selection;
   wnew->screen.trim_selection = request->screen.trim_selection;
   wnew->screen.always_highlight = request->screen.always_highlight;
   wnew->screen.pointer_cursor = request->screen.pointer_cursor;

   wnew->screen.answer_back = request->screen.answer_back;

   wnew->screen.printer_command = request->screen.printer_command;
   wnew->screen.printer_autoclose = request->screen.printer_autoclose;
   wnew->screen.printer_extent = request->screen.printer_extent;
   wnew->screen.printer_formfeed = request->screen.printer_formfeed;
   wnew->screen.printer_controlmode = request->screen.printer_controlmode;

   wnew->screen.print_attributes = request->screen.print_attributes;


    
   wnew->screen.keyboard_dialect = request->screen.keyboard_dialect;

   wnew->screen.input_eight_bits = request->screen.input_eight_bits;
   wnew->screen.output_eight_bits = request->screen.output_eight_bits;
   wnew->screen.control_eight_bits = request->screen.control_eight_bits;
   wnew->screen.backarrow_key = request->screen.backarrow_key;
   wnew->screen.meta_sends_esc = request->screen.meta_sends_esc;
   wnew->screen.allowSendEvents = request->screen.allowSendEvents;



   wnew->misc.titeInhibit = request->misc.titeInhibit;
   wnew->misc.dynamicColors = request->misc.dynamicColors;
   for (i = fontMenu_font1; i <= fontMenu_font6; i++) {
       wnew->screen.menu_font_names[i] = request->screen.menu_font_names[i];
   }
   
   wnew->screen.menu_font_names[fontMenu_fontdefault] = ((void *) 0);
   wnew->screen.menu_font_names[fontMenu_fontescape] = ((void *) 0);
   wnew->screen.menu_font_names[fontMenu_fontsel] = ((void *) 0);
   wnew->screen.menu_font_number = fontMenu_fontdefault;


   wnew->screen.font_doublesize = request->screen.font_doublesize;
   wnew->screen.cache_doublesize = request->screen.cache_doublesize;
   if (wnew->screen.cache_doublesize > 8)
       wnew->screen.cache_doublesize = 8;
   if (wnew->screen.cache_doublesize == 0)
       wnew->screen.font_doublesize = 0;
    




   wnew->num_ptrs = (OFF_ATTRS+1); 

   wnew->screen.boldColors    = request->screen.boldColors;
   wnew->screen.colorAttrMode = request->screen.colorAttrMode;
   wnew->screen.colorBDMode   = request->screen.colorBDMode;
   wnew->screen.colorBLMode   = request->screen.colorBLMode;
   wnew->screen.colorMode     = request->screen.colorMode;
   wnew->screen.colorULMode   = request->screen.colorULMode;

   for (i = 0, color_ok = 0; i < (16+3); i++) {
        
       wnew->screen.Acolors[i] = request->screen.Acolors[i];
       if (wnew->screen.Acolors[i] != wnew->dft_foreground
	&& wnew->screen.Acolors[i] != request->screen.foreground
	&& wnew->screen.Acolors[i] != request->core.background_pixel)
	   color_ok = 1;
   }

   




   if (!color_ok) {
	wnew->screen.colorMode = 0;
	 
   }




   wnew->num_ptrs = (OFF_COLOR+1);

   wnew->sgr_foreground = -1;



   wnew->screen.highlightcolor = request->screen.highlightcolor;



   wnew->num_ptrs = (OFF_CSETS+1);













   wnew->screen.bold_mode = request->screen.bold_mode;
   wnew->screen.underline = request->screen.underline;

   wnew->cur_foreground = 0;
   wnew->cur_background = 0;

   wnew->keyboard.flags = 0x08;
   if (wnew->screen.backarrow_key)
	   wnew->keyboard.flags |= 0x10;

   


   XtAddEventHandler(my_parent, (1L<<4), 0,
		HandleEnterWindow, (Opaque)((void *) 0));
   XtAddEventHandler(my_parent, (1L<<5), 0,
		HandleLeaveWindow, (Opaque)((void *) 0));
   XtAddEventHandler(my_parent, (1L<<21), 0,
		HandleFocusChange, (Opaque)((void *) 0));
   XtAddEventHandler((Widget)wnew, 0L, 1,
		VTNonMaskableEvent, (Opaque)((void *) 0));
   XtAddEventHandler((Widget)wnew, (1L<<22), 0,
		     HandleBellPropertyChange, (Opaque)((void *) 0));






   


   if ( zIconBeep )

       XtAddEventHandler(my_parent, (1L<<17), 0,
			 HandleStructNotify, (XtPointer)0);


   wnew->screen.bellInProgress = 0;

   set_character_class (wnew->screen.charClass);

   
   ScrollBarOn (wnew, 1, 0);

   
   if ( wnew->misc.resizeGravity != 1 &&
	wnew->misc.resizeGravity != 7) {
       Cardinal nparams = 1;

       XtAppWarningMsg(app_con, "rangeError", "resizeGravity", "XTermError",
		       "unsupported resizeGravity resource value (%d)",
		       (String *) &(wnew->misc.resizeGravity), &nparams);
       wnew->misc.resizeGravity = 7;
   }








    if (wnew->screen.savelines < 0)
	wnew->screen.savelines = 0;

    wnew->flags = 0;
    if (!wnew->screen.jumpscroll)
	wnew->flags |= 0x10000;
    if (wnew->misc.reverseWrap)
	wnew->flags |= 0x800;
    if (wnew->misc.autoWrap)
	wnew->flags |= 0x400;
    if (wnew->misc.re_verse != wnew->misc.re_verse0)
	wnew->flags |= 0x1000;

    wnew->initflags = wnew->flags;

    if (wnew->misc.appcursorDefault)
	wnew->keyboard.flags |= 0x04;

    if (wnew->misc.appkeypadDefault)
	wnew->keyboard.flags |= 0x02;

   return;
}


static void VTDestroy (Widget w)
{
    XtFree(((XtermWidget)w)->screen.selection_data);
}


static void VTRealize (
	Widget w,
	XtValueMask *valuemask,
	XSetWindowAttributes *values)
{
	unsigned int width, height;
	register TScreen *screen = &term->screen;
	int xpos, ypos, pr;
	XSizeHints		sizehints;
	int scrollbar_width;

	TabReset (term->tabs);

	screen->menu_font_names[fontMenu_fontdefault] = term->misc.f_n;
	screen->fnt_norm = screen->fnt_bold = ((void *) 0);
	if (!xtermLoadFont(screen, term->misc.f_n, term->misc.f_b, 0, 0)) {
	    if (XmuCompareISOLatin1(term->misc.f_n, "fixed") != 0) {
		fprintf ((decc$ga_stderr),
		     "%s:  unable to open font \"%s\", trying \"fixed\"....\n",
		     xterm_name, term->misc.f_n);
		(void) xtermLoadFont (screen, "fixed", ((void *) 0), 0, 0);
		screen->menu_font_names[fontMenu_fontdefault] = "fixed";
	    }
	}

	
	if (!screen->fnt_norm) {
	    fprintf ((decc$ga_stderr), "%s:  unable to locate a suitable font\n",
		     xterm_name);
	    Exit (1);
	}

	
	if (!screen->pointer_cursor)
	  screen->pointer_cursor = make_colored_cursor(152,
						       screen->mousecolor,
						       screen->mousecolorback);
	else
	  recolor_cursor (screen->pointer_cursor,
			  screen->mousecolor, screen->mousecolorback);

	scrollbar_width = (term->misc.scrollbar ?
			   screen->scrollWidget->core.width 
 : 0);

	
	xpos = 1; ypos = 1; width = 80; height = 24;
	pr = XParseGeometry (term->misc.geo_metry, &xpos, &ypos,
			     &width, &height);
	screen->max_col = (width - 1);	
	screen->max_row = (height - 1);	
	xtermUpdateFontInfo (&term->screen, 0);

	width = screen->fullVwin.fullwidth;
	height = screen->fullVwin.fullheight;

	if ((pr & 0x0001) && (0x0010&pr))
	  xpos += ((&((_XPrivDisplay)screen->display)->screens[(((_XPrivDisplay)screen->display)->default_screen)])->width)
			- width - (((term)->core.parent)->core.border_width * 2);
	if ((pr & 0x0002) && (0x0020&pr))
	  ypos += ((&((_XPrivDisplay)screen->display)->screens[(((_XPrivDisplay)screen->display)->default_screen)])->height)
			- height - (((term)->core.parent)->core.border_width * 2);

	
	sizehints.base_width = 2 * screen->border + scrollbar_width;
	sizehints.base_height = 2 * screen->border;
	sizehints.width_inc = ((screen)->fullVwin.f_width);
	sizehints.height_inc = ((screen)->fullVwin.f_height);
	sizehints.min_width = sizehints.base_width + sizehints.width_inc;
	sizehints.min_height = sizehints.base_height + sizehints.height_inc;
	sizehints.flags = ((1L << 8)|(1L << 4)|(1L << 6));
	sizehints.x = xpos;
	sizehints.y = ypos;
	if ((0x0001&pr) || (0x0002&pr)) {
	    sizehints.flags |= (1L << 1)|(1L << 0);
	    sizehints.flags |= (1L << 9);
	    switch (pr & (0x0010 | 0x0020)) {
	      case 0:
		sizehints.win_gravity = 1;
		break;
	      case 0x0010:
		sizehints.win_gravity = 3;
		break;
	      case 0x0020:
		sizehints.win_gravity = 7;
		break;
	      default:
		sizehints.win_gravity = 9;
		break;
	    }
	} else {
	    
	    sizehints.flags |= (1L << 3);
	}
	sizehints.width = width;
	sizehints.height = height;
	if ((0x0004&pr) || (0x0008&pr))
	  sizehints.flags |= (1L << 1);
	else sizehints.flags |= (1L << 3);

	(void) XtMakeResizeRequest((Widget) term,
				   (Dimension)width, (Dimension)height,
				   &term->core.width, &term->core.height);

	



	if (sizehints.flags & (1L << 0))
	    XMoveWindow ((((term)->core.screen)->display), ((((term)->core.parent))->core.window),
			 sizehints.x, sizehints.y);

	XSetWMNormalHints ((((term)->core.screen)->display), ((((term)->core.parent))->core.window),
			   &sizehints);
	XFlush ((((term)->core.screen)->display));	

	

	values->bit_gravity = term->misc.resizeGravity == 1 ?
	    1 : 0;
	term->screen.fullVwin.window = ((term)->core.window) =
	  XCreateWindow((((term)->core.screen)->display), ((((term)->core.parent))->core.window),
		term->core.x, term->core.y,
		term->core.width, term->core.height, term->core.border_width,
		(int) term->core.depth,
		1, 0L,
		*valuemask|(1L<<4), values);
	screen->event_mask = values->event_mask;

# 4611 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"





	term->screen.xic = ((void *) 0);


	VTInitModifiers();


	set_cursor_gcs (screen);

	

	resetCharsets(screen);

	XDefineCursor(screen->display, ((((term)->core.parent))->core.window), screen->pointer_cursor);

	screen->cur_col = screen->cur_row = 0;
	screen->max_col = ((screen)->fullVwin.width)/screen->fullVwin.f_width - 1;
	screen->top_marg = 0;
	screen->bot_marg = screen->max_row = ((screen)->fullVwin.height) /
				screen->fullVwin.f_height - 1;

	__MEMSET(screen->sc, 0, sizeof(screen->sc));

	




	if (!tekWidget)			

	  screen->visbuf = screen->allbuf = ((void *) 0);

	screen->do_wrap = 0;
	screen->scrolls = screen->incopy = 0;
	xtermSetCursorBox (screen);

	screen->savedlines = 0;

	if (term->misc.scrollbar) {
		screen->fullVwin.scrollbar = 0;
		ScrollBarOn (term, 0, 1);
	}
	CursorSave (term);
	return;
}

# 4787 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]CHARPROC.C;2"



static Boolean VTSetValues (
	Widget cur,
	Widget request  ,
	Widget wnew,
	ArgList args  ,
	Cardinal *num_args  )
{
    XtermWidget curvt = (XtermWidget) cur;
    XtermWidget newvt = (XtermWidget) wnew;
    Boolean refresh_needed = 0;
    Boolean fonts_redone = 0;

    if(curvt->core.background_pixel != newvt->core.background_pixel
       || curvt->screen.foreground != newvt->screen.foreground
       || curvt->screen.menu_font_names[curvt->screen.menu_font_number]
	  != newvt->screen.menu_font_names[newvt->screen.menu_font_number]
       || curvt->misc.f_n != newvt->misc.f_n) {
	if(curvt->misc.f_n != newvt->misc.f_n)
	    newvt->screen.menu_font_names[fontMenu_fontdefault] = newvt->misc.f_n;
	if (xtermLoadFont(&newvt->screen,
			newvt->screen.menu_font_names[curvt->screen.menu_font_number],
			newvt->screen.menu_font_names[curvt->screen.menu_font_number],
			1, newvt->screen.menu_font_number)) {
	    
	    refresh_needed = 1;
	    fonts_redone = 1;
	} else
	    if(curvt->misc.f_n != newvt->misc.f_n)
		newvt->screen.menu_font_names[fontMenu_fontdefault] = curvt->misc.f_n;
    }
    if(!fonts_redone
       && curvt->screen.cursorcolor != newvt->screen.cursorcolor) {
	set_cursor_gcs(&newvt->screen);
	refresh_needed = 1;
    }
    if(curvt->misc.re_verse != newvt->misc.re_verse) {
	newvt->flags ^= 0x1000;
	ReverseVideo(newvt);
	newvt->misc.re_verse = !newvt->misc.re_verse; 
	refresh_needed = 1;
    }
    if(curvt->screen.mousecolor != newvt->screen.mousecolor
       || curvt->screen.mousecolorback != newvt->screen.mousecolorback) {
	recolor_cursor (newvt->screen.pointer_cursor,
			newvt->screen.mousecolor,
			newvt->screen.mousecolorback);
	refresh_needed = 1;
    }
    if (curvt->misc.scrollbar != newvt->misc.scrollbar) {
	ToggleScrollBar(newvt);
    }

    return refresh_needed;
}




void
ShowCursor(void)
{
	register TScreen *screen = &term->screen;
	register int x, y, flags;
	Char	clo;
	int	fg_bg = 0;
	GC	currentGC;
	Boolean	in_selection;
	Pixel fg_pix;
	Pixel bg_pix;
	Pixel tmp;

	Pixel hi_pix = screen->highlightcolor;





	if (screen->cursor_state == 2)
		return;

	if (eventMode != NORMAL) return;

	if (screen->cur_row - screen->topline > screen->max_row)
		return;

	screen->cursor_row = screen->cur_row;
	screen->cursor_col = screen->cur_col;








	clo   = (screen->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CHARS])[screen->cursor_col];
	flags = (screen->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_ATTRS])[screen->cursor_col];

	 



	if (clo == 0



	) {
		clo = ' ';
	}

	



	 



	if(screen->colorMode) { fg_bg = (screen->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_COLOR])[screen->cursor_col]; }


	fg_pix = (((flags) & 0x20) && ((extract_fg(fg_bg,flags)) >= 0) ? term->screen.Acolors[extract_fg(fg_bg,flags)] : term->screen.foreground);
	bg_pix = (((flags) & 0x10) && ((((int)((fg_bg) & 0xf))) >= 0) ? term->screen.Acolors[((int)((fg_bg) & 0xf))] : term->core.background_pixel);

	if (screen->cur_row > screen->endHRow ||
	    (screen->cur_row == screen->endHRow &&
	     screen->cur_col >= screen->endHCol) ||
	    screen->cur_row < screen->startHRow ||
	    (screen->cur_row == screen->startHRow &&
	     screen->cur_col < screen->startHCol))
	    in_selection = 0;
	else
	    in_selection = 1;

	



	if(screen->select || screen->always_highlight) {
		if (( (flags & 0x01) && !in_selection) ||
		    (!(flags & 0x01) &&  in_selection)){
		    
		    if (screen->cursorGC) {
			currentGC = screen->cursorGC;
		    } else {
			if (flags & (0x04|0x08)) {
				currentGC = ((screen)->fullVwin.normalboldGC);
			} else {
				currentGC = ((screen)->fullVwin.normalGC);
			}
		    }

		    if (hi_pix != screen->foreground
		     && hi_pix != fg_pix
		     && hi_pix != bg_pix
		     && hi_pix != term->dft_foreground) {
			bg_pix = fg_pix;
			fg_pix = hi_pix;
		    }

		     tmp = fg_pix; fg_pix = bg_pix; bg_pix = tmp;
		} else { 
		    if (screen->reversecursorGC) {
			currentGC = screen->reversecursorGC;
		    } else {
			if (flags & (0x04|0x08)) {
				currentGC = ((screen)->fullVwin.reverseboldGC);
			} else {
				currentGC = ((screen)->fullVwin.reverseGC);
			}
		    }
		}
		if (screen->cursorcolor == term->dft_foreground) {
			XSetBackground(screen->display, currentGC, fg_pix);
		}
		XSetForeground(screen->display, currentGC, bg_pix);
	} else { 
		if (( (flags & 0x01) && !in_selection) ||
		    (!(flags & 0x01) &&  in_selection)) {
		    
			currentGC = ((screen)->fullVwin.reverseGC);
			XSetForeground(screen->display, currentGC, bg_pix);
			XSetBackground(screen->display, currentGC, fg_pix);
		} else { 
			currentGC = ((screen)->fullVwin.normalGC);
			XSetForeground(screen->display, currentGC, fg_pix);
			XSetBackground(screen->display, currentGC, bg_pix);
		}
	}

	 


	drawXtermText(screen, flags, currentGC,
		x = ((!(((((screen)->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) == 0) && !(((((screen)->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) > 3)) ? ((2*( screen->cur_col)) * ((screen)->fullVwin.f_width) + (((screen)->fullVwin.scrollbar) + screen->border)) : ((( screen->cur_col)) * ((screen)->fullVwin.f_width) + (((screen)->fullVwin.scrollbar) + screen->border))),
		y = (((( screen->cur_row) - screen->topline) * ((screen)->fullVwin.f_height)) + screen->border),
		(((!((((((&term->screen))->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) == 0) && !((((((&term->screen))->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0])) > 3))) ? ((((&term->screen))->visbuf[term->num_ptrs * (screen->cur_row) + OFF_CSETS])[0]) : (term->screen).cur_chrset),
		&clo, 1);

	if (!screen->select && !screen->always_highlight) {
		screen->box->x = x;
		screen->box->y = y;
		XDrawLines (screen->display, ((screen)->fullVwin.window),
			    screen->cursoroutlineGC ? screen->cursoroutlineGC
			    			    : currentGC,
			    screen->box, 5, 1);
	}
	screen->cursor_state = 1;
}




void
HideCursor(void)
{
	register TScreen *screen = &term->screen;
	GC	currentGC;
	register int flags;
	register int fg_bg = 0;
	Char	clo;
	Boolean	in_selection;




	if (screen->cursor_state == 0)	
		return;
	if(screen->cursor_row - screen->topline > screen->max_row)
		return;








	clo   = (screen->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CHARS])[screen->cursor_col];
	flags = (screen->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_ATTRS])[screen->cursor_col];

	 



	if(screen->colorMode) { fg_bg = (screen->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_COLOR])[screen->cursor_col]; }



	 



	if (screen->cursor_row > screen->endHRow ||
	    (screen->cursor_row == screen->endHRow &&
	     screen->cursor_col >= screen->endHCol) ||
	    screen->cursor_row < screen->startHRow ||
	    (screen->cursor_row == screen->startHRow &&
	     screen->cursor_col < screen->startHCol))
	    in_selection = 0;
	else
	    in_selection = 1;

	currentGC = updatedXtermGC(screen, flags, fg_bg, in_selection);

	if (clo == 0



	) {
		clo = ' ';
	}

	 

	drawXtermText(screen, flags, currentGC,
		((!(((((screen)->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CSETS])[0])) == 0) && !(((((screen)->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CSETS])[0])) > 3)) ? ((2*( screen->cursor_col)) * ((screen)->fullVwin.f_width) + (((screen)->fullVwin.scrollbar) + screen->border)) : ((( screen->cursor_col)) * ((screen)->fullVwin.f_width) + (((screen)->fullVwin.scrollbar) + screen->border))),
		(((( screen->cursor_row) - screen->topline) * ((screen)->fullVwin.f_height)) + screen->border),
		(((!((((((&term->screen))->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CSETS])[0])) == 0) && !((((((&term->screen))->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CSETS])[0])) > 3))) ? ((((&term->screen))->visbuf[term->num_ptrs * (screen->cursor_row) + OFF_CSETS])[0]) : (term->screen).cur_chrset),
		&clo, 1);

	screen->cursor_state = 0;
	resetXtermGC(screen, flags, in_selection);
}


static void
StartBlinking(TScreen *screen)
{
	if (screen->cursor_blink
	 && screen->cursor_timer == 0) {
		unsigned long interval = (screen->cursor_state == 1 ?
				screen->cursor_on : screen->cursor_off);
		if (interval == 0)		
			interval = 1;	
		screen->cursor_timer = XtAppAddTimeOut(
			app_con,
			interval,
			BlinkCursor,
			screen);
	}
}

static void
StopBlinking(TScreen *screen)
{
	if (screen->cursor_timer)
		XtRemoveTimeOut(screen->cursor_timer);
	screen->cursor_timer = 0;
}






static void
BlinkCursor(XtPointer closure, XtIntervalId* id  )
{
	TScreen *screen = (TScreen *)closure;

	screen->cursor_timer = 0;
	if (screen->cursor_state == 1) {
		if(screen->select || screen->always_highlight) {
			HideCursor();
			if (screen->cursor_state == 0)
				screen->cursor_state = 2;
		}
	} else if (screen->cursor_state == 2) {
		screen->cursor_state = 0;
		ShowCursor();
		if (screen->cursor_state == 0)
			screen->cursor_state = 2;
	}
	StartBlinking(screen);
	xevents();
}












void
VTReset(int full, int saved)
{
	register TScreen *screen = &term->screen;

	if (saved) {
		screen->savedlines = 0;
		ScrollBarDrawThumb(screen->scrollWidget);
	}

	
	screen->cursor_set = 1;

	
	screen->top_marg = 0;
	screen->bot_marg = screen->max_row;

	bitclr(&term->flags, 0x4000);

	if(screen->colorMode) {reset_SGR_Colors();}

	
	resetCharsets(screen);

	
	bitclr(&term->flags, 0x40);
	screen->protected_mode = 0;

	if (full) {	
		TabReset (term->tabs);
		term->keyboard.flags = 0x08;
		if (term->screen.backarrow_key)
			term->keyboard.flags |= 0x10;
		UpdateMenuItem(vtMenuEntries[vtMenu_appcursor].widget,(term->keyboard.flags & 0x04));
		UpdateMenuItem(vtMenuEntries[vtMenu_appkeypad].widget,(term->keyboard.flags & 0x02));
		UpdateMenuItem(mainMenuEntries[mainMenu_backarrow].widget,term->keyboard.flags & 0x10);
		show_8bit_control(0);
		reset_decudk();

		FromAlternate(screen);
		ClearScreen(screen);
		screen->cursor_state = 0;
		if (term->flags & 0x1000)
			ReverseVideo(term);

		term->flags = term->initflags;
		UpdateMenuItem(vtMenuEntries[vtMenu_reversevideo].widget,(term->flags & 0x1000));
		UpdateMenuItem(vtMenuEntries[vtMenu_autowrap].widget,(term->flags & 0x400));
		UpdateMenuItem(vtMenuEntries[vtMenu_reversewrap].widget,(term->flags & 0x800));
		UpdateMenuItem(vtMenuEntries[vtMenu_autolinefeed].widget,(term->flags & 0x2000));

		screen->jumpscroll = !(term->flags & 0x10000);
		UpdateMenuItem(vtMenuEntries[vtMenu_jumpscroll].widget,term->screen.jumpscroll);

		if(screen->c132 && (term->flags & 0x20000)) {
		        Dimension junk;
			XtMakeResizeRequest(
			    (Widget) term,
			    (Dimension) 80*((screen)->fullVwin.f_width)
				+ 2 * screen->border + ((screen)->fullVwin.scrollbar),
			    (Dimension) ((screen)->fullVwin.f_height)
			        * (screen->max_row + 1) + 2 * screen->border,
			    &junk, &junk);
			XSync(screen->display, 0);	
			if(XtAppPending(app_con))
				xevents();
		}
		CursorSet(screen, 0, 0, term->flags);
		CursorSave(term);
	} else {	
		






		term->keyboard.flags &= ~(0x04|0x01);
		bitcpy(&term->flags, term->initflags, 0x400|0x800);
		bitclr(&term->flags, 0x8000|0x01|0x04|0x08|0x02|0x40000);
		if(screen->colorMode) {reset_SGR_Colors();}
		UpdateMenuItem(vtMenuEntries[vtMenu_appcursor].widget,(term->keyboard.flags & 0x04));
		UpdateMenuItem(vtMenuEntries[vtMenu_autowrap].widget,(term->flags & 0x400));
		UpdateMenuItem(vtMenuEntries[vtMenu_reversewrap].widget,(term->flags & 0x800));

		CursorSave(term);
		__MEMSET(screen->sc, 0, sizeof(screen->sc));
	}
	longjmp(vtjmpbuf, 1);	
}










static int
set_character_class (register char *s)
{
    register int i;			
    int len;				
    int acc;				
    int low, high;			
    int base;				
    int numbers;			
    int digits;				
    static char *errfmt = "%s:  %s in range string \"%s\" (position %d)\n";

    if (!s || !s[0]) return -1;

    base = 10;				
    low = high = -1;			

    for (i = 0, len = __STRLEN(s), acc = 0, numbers = digits = 0;
	 i < len; i++) {
	char c = s[i];

	if (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(c)]&0x8:(isspace)(c))) {
	    continue;
	} else if (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(c)]&0x4:(isdigit)(c))) {
	    acc = acc * base + (c - '0');
	    digits++;
	    continue;
	} else if (c == '-') {
	    low = acc;
	    acc = 0;
	    if (digits == 0) {
		fprintf ((decc$ga_stderr), errfmt, ProgramName, "missing number", s, i);
		return (-1);
	    }
	    digits = 0;
	    numbers++;
	    continue;
	} else if (c == ':') {
	    if (numbers == 0)
	      low = acc;
	    else if (numbers == 1)
	      high = acc;
	    else {
		fprintf ((decc$ga_stderr), errfmt, ProgramName, "too many numbers",
			 s, i);
		return (-1);
	    }
	    digits = 0;
	    numbers++;
	    acc = 0;
	    continue;
	} else if (c == ',') {
	    



	    if (high < 0) {
		high = low;
		numbers++;
	    }
	    if (numbers != 2) {
		fprintf ((decc$ga_stderr), errfmt, ProgramName, "bad value number",
			 s, i);
	    } else if (SetCharacterClassRange (low, high, acc) != 0) {
		fprintf ((decc$ga_stderr), errfmt, ProgramName, "bad range", s, i);
	    }

	    low = high = -1;
	    acc = 0;
	    digits = 0;
	    numbers = 0;
	    continue;
	} else {
	    fprintf ((decc$ga_stderr), errfmt, ProgramName, "bad character", s, i);
	    return (-1);
	}				

    }

    if (low < 0 && high < 0) return (0);

    



    if (high < 0) high = low;
    if (numbers < 1 || numbers > 2) {
	fprintf ((decc$ga_stderr), errfmt, ProgramName, "bad value number", s, i);
    } else if (SetCharacterClassRange (low, high, acc) != 0) {
	fprintf ((decc$ga_stderr), errfmt, ProgramName, "bad range", s, i);
    }

    return (0);
}


static void HandleKeymapChange(
	Widget w,
	XEvent *event  ,
	String *params,
	Cardinal *param_count)
{
    static XtTranslations keymap, original;
    static XtResource key_resources[] = {
	{ ((char*)&XtStrings[802]), ((char*)&XtStrings[1484]), ((char*)&XtStrings[1841]),
	      sizeof(XtTranslations), 0, ((char*)&XtStrings[1841]), (XtPointer)((void *) 0)}
    };
    char mapName[1000];
    char mapClass[1000];
    char* pmapName;
    char* pmapClass;
    size_t len;

    if (*param_count != 1) return;

    if (original == ((void *) 0)) original = w->core.tm.translations;

    if (strcmp(params[0], "None") == 0) {
	XtOverrideTranslations(w, original);
	return;
    }

    len = __STRLEN(params[0]) + 7;

    pmapName  = (char *)((len) <= sizeof( mapName)	? (XtPointer)( mapName)	: (XtPointer)malloc((unsigned)(len)));
    pmapClass = (char *)((len) <= sizeof( mapClass)	? (XtPointer)( mapClass)	: (XtPointer)malloc((unsigned)(len)));
    if (pmapName == ((void *) 0)
     || pmapClass == ((void *) 0))
	SysError(57);

    (void) sprintf( pmapName, "%sKeymap", params[0] );
    (void) __STRCPY( pmapClass, pmapName);
    if (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(pmapClass[0])]&0x2:(islower)(pmapClass[0]))) pmapClass[0] = toupper(pmapClass[0]);
    XtGetSubresources( w, (XtPointer)&keymap, pmapName, pmapClass,
		       key_resources, (Cardinal)1, ((void *) 0), (Cardinal)0 );
    if (keymap != ((void *) 0))
	XtOverrideTranslations(w, keymap);

    if ((pmapName) != ((char *)( mapName))) free(pmapName);
    if ((pmapClass) != ((char *)( mapClass))) free(pmapClass);
}



static void HandleBell(
	Widget w,
	XEvent *event  ,
	String *params,		
	Cardinal *param_count)	
{
    int percent = (*param_count) ? atoi(params[0]) : 0;

     





    XBell( (((w)->core.screen)->display), percent );

}



static void HandleVisualBell(
	Widget w  ,
	XEvent *event  ,
	String *params  ,
	Cardinal *param_count  )
{
    VisualBell();
}



static void HandleIgnore(
	Widget w,
	XEvent *event,
	String *params  ,
	Cardinal *param_count  )
{
    
    (void) SendMousePosition(w, event);
}



static void
DoSetSelectedFont(
	Widget w  ,
	XtPointer client_data  ,
	Atom *selection  ,
	Atom *type,
	XtPointer value,
	unsigned long *length  ,
	int *format)
{
    char *val = (char *)value;
    int len;
    if (*type != ((Atom) 31)  ||  *format != 8) {
	Bell(1,0);
	return;
    }
    len = __STRLEN(val);
    if (len > 0) {
	if (val[len-1] == '\n') val[len-1] = '\0';
	



	if (len > 1000  ||  strchr(val, '\n'))
	    return;
	if (!xtermLoadFont (&term->screen, val, ((void *) 0), 1, fontMenu_fontsel))
	    Bell(1,0);
    }
}

void FindFontSelection (char *atom_name, int justprobe)
{
    static AtomPtr *atoms;
    static int atomCount = 0;
    AtomPtr *pAtom;
    int a;
    Atom target;

    if (!atom_name) atom_name = "PRIMARY";

    for (pAtom = atoms, a = atomCount; a; a--, pAtom++) {
	if (strcmp(atom_name, XmuNameOfAtom(*pAtom)) == 0) break;
    }
    if (!a) {
	atoms = (AtomPtr*) XtRealloc ((char *)atoms,
				      sizeof(AtomPtr)*(atomCount+1));
	*(pAtom = &atoms[atomCount++]) = XmuMakeAtom(atom_name);
    }

    target = XmuInternAtom((((term)->core.screen)->display), *pAtom);
    if (justprobe) {
	term->screen.menu_font_names[fontMenu_fontsel] =
	  XGetSelectionOwner((((term)->core.screen)->display), target) ? _Font_Selected_ : 0;
    } else {
	XtGetSelectionValue((Widget)term, target, ((Atom) 31),
			    DoSetSelectedFont, ((void *) 0),
			    XtLastTimestampProcessed((((term)->core.screen)->display)));
    }
    return;
}


void
set_cursor_gcs (TScreen *screen)
{
    XGCValues xgcv;
    XtGCMask mask;
    Pixel cc = screen->cursorcolor;
    Pixel fg = screen->foreground;
    Pixel bg = term->core.background_pixel;
    GC new_cursorGC = ((void *) 0);
    GC new_cursorFillGC = ((void *) 0);
    GC new_reversecursorGC = ((void *) 0);
    GC new_cursoroutlineGC = ((void *) 0);

    






















    







    if(screen->colorMode) {
	if (((screen)->fullVwin.window) != 0 && (cc != bg) && (cc != fg)) {
	    
	    xgcv.font = screen->fnt_norm->fid;
	    mask = ((1L<<2) | (1L<<3) | (1L<<14));
	    xgcv.foreground = fg;
	    xgcv.background = cc;
	    new_cursorGC = XCreateGC (screen->display, ((screen)->fullVwin.window), mask, &xgcv);

	    xgcv.foreground = cc;
	    xgcv.background = fg;
	    new_cursorFillGC = XCreateGC (screen->display, ((screen)->fullVwin.window), mask, &xgcv);

	    if (screen->always_highlight) {
		new_reversecursorGC = (GC) 0;
		new_cursoroutlineGC = (GC) 0;
	    } else {
		xgcv.foreground = bg;
		xgcv.background = cc;
		new_reversecursorGC = XCreateGC (screen->display, ((screen)->fullVwin.window), mask, &xgcv);
		xgcv.foreground = cc;
		xgcv.background = bg;
		new_cursoroutlineGC = XCreateGC (screen->display, ((screen)->fullVwin.window), mask, &xgcv);
	    }
	}
    } else

    if (cc != fg && cc != bg) {
	
	xgcv.font = screen->fnt_norm->fid;
	mask = ((1L<<2) | (1L<<3) | (1L<<14));

	xgcv.foreground = fg;
	xgcv.background = cc;
	new_cursorGC = XtGetGC ((Widget) term, mask, &xgcv);

	xgcv.foreground = cc;
	xgcv.background = fg;
	new_cursorFillGC = XtGetGC ((Widget) term, mask, &xgcv);

	if (screen->always_highlight) {
	    new_reversecursorGC = (GC) 0;
	    new_cursoroutlineGC = (GC) 0;
	} else {
	    xgcv.foreground = bg;
	    xgcv.background = cc;
	    new_reversecursorGC = XtGetGC ((Widget) term, mask, &xgcv);
	    xgcv.foreground = cc;
	    xgcv.background = bg;
	    new_cursoroutlineGC = XtGetGC ((Widget) term, mask, &xgcv);
	}
    }


    if(screen->colorMode)
    {
	if (screen->cursorGC)
	    XFreeGC (screen->display, screen->cursorGC);
	if (screen->fillCursorGC)
	    XFreeGC (screen->display, screen->fillCursorGC);
	if (screen->reversecursorGC)
	    XFreeGC (screen->display, screen->reversecursorGC);
	if (screen->cursoroutlineGC)
	    XFreeGC (screen->display, screen->cursoroutlineGC);
    }
    else

    {
	if (screen->cursorGC)
	    XtReleaseGC ((Widget)term, screen->cursorGC);
	if (screen->fillCursorGC)
	    XtReleaseGC ((Widget)term, screen->fillCursorGC);
	if (screen->reversecursorGC)
	    XtReleaseGC ((Widget)term, screen->reversecursorGC);
	if (screen->cursoroutlineGC)
	    XtReleaseGC ((Widget)term, screen->cursoroutlineGC);
    }

    screen->cursorGC        = new_cursorGC;
    screen->fillCursorGC    = new_cursorFillGC;
    screen->reversecursorGC = new_reversecursorGC;
    screen->cursoroutlineGC = new_cursoroutlineGC;
}
