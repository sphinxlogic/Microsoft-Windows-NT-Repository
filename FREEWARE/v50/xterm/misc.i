# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"



























































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

































































# 60 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"



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


# 63 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"


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


# 65 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

# 1 "SYS$COMMON:[SYSLIB]DECC$RTLDEF.TLB;2"



























#pragma __nostandard













#pragma __member_alignment __save
#pragma __member_alignment





































struct passwd {
    char    *pw_name;
    __uid_t pw_uid;
    __gid_t pw_gid;
    char    *pw_dir;
    char    *pw_shell;
};






typedef struct passwd * __struct_passwd_ptr32;
















__struct_passwd_ptr32 getpwuid (__uid_t);




    __struct_passwd_ptr32 getpwnam (const char *, ...);













#pragma __member_alignment __restore
#pragma __standard



# 66 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"


# 1 "SYS$COMMON:[DECW$INCLUDE]XATOM.H;1"












































































































# 68 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

# 1 "SYS$COMMON:[DECW$INCLUDE]CURSORFONT.H;1"
















































































































# 69 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"


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




# 71 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]ERROR.H;1"





























 

extern int XmuPrintDefaultErrorMessage(





);

extern int XmuSimpleErrorHandler(




);

 



# 72 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]SYSUTIL.H;1"































 

extern int XmuGetHostname(




);

 



# 73 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

# 1 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]WINUTIL.H;1"

























# 1 "SYS$COMMON:[DECW$INCLUDE]XUTIL.H;1"



























































# 928 "SYS$COMMON:[DECW$INCLUDE]XUTIL.H;1"


# 26 "SEQAXP$DKA0:[SHARED.PROGRAMS.X11KIT.][XMU]WINUTIL.H;1"






 

extern Window XmuClientWindow(




);

extern int XmuUpdateMapHints(





);

extern Screen *XmuScreenOfWindow(




);

 



# 74 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"


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





















# 76 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

# 1 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]ERROR.H;1"
















































































































# 77 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

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



















































































































































































































































# 78 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

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




# 79 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"

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



# 80 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"
















extern char **environ;		













static Boolean ChangeColorsRequest (XtermWidget pTerm, int start, char *names, int final);
static void DoSpecialEnterNotify (XEnterWindowEvent *ev);
static void DoSpecialLeaveNotify (XEnterWindowEvent *ev);
static void selectwindow (TScreen *screen, int flag);
static void unselectwindow (TScreen *screen, int flag);

void
do_xevents(void)
{
	register TScreen *screen = &term->screen;

	if (XtAppPending(app_con)
	|| 

        screen->display->qlen > 0



        )
		xevents();
}

void
xevents(void)
{
	XEvent event;
	XtInputMask input_mask;
	register TScreen *screen = &term->screen;

	if(screen->scroll_amt)
		FlushScroll(screen);
	





	while ((input_mask = XtAppPending(app_con)) & 2)
		XtAppProcessEvent(app_con, 2);
	


	if ((input_mask & 1) != 1)
	    return;
	do {
		if (waitingForTrackInfo)
			return;
		XtAppNextEvent (app_con, &event);
		





		if((event.type == 7 && (event.xcrossing.window == ((((term)->core.parent))->core.window) || (tekWidget && event.xcrossing.window == ((((tekWidget)->core.parent))->core.window)))))
		  DoSpecialEnterNotify (&event.xcrossing);
		else
		if((event.type == 8 && (event.xcrossing.window == ((((term)->core.parent))->core.window) || (tekWidget && event.xcrossing.window == ((((tekWidget)->core.parent))->core.window)))))
		  DoSpecialLeaveNotify (&event.xcrossing);
		else if ((screen->send_mouse_pos == ANY_EVENT_MOUSE



								)
		 && event.xany.type == 6
		 && event.xcrossing.window == ((term)->core.window)) {
		    SendMousePosition((Widget)term, &event);
		    continue;
		}

		if (!event.xany.send_event ||
		    screen->allowSendEvents ||
		    ((event.xany.type != 2) &&
		     (event.xany.type != 3) &&
		     (event.xany.type != 4) &&
		     (event.xany.type != 5)))
		    XtDispatchEvent(&event);
	} while ((input_mask = XtAppPending(app_con)) & 1);
}


Cursor make_colored_cursor (
	unsigned cursorindex,			
	unsigned long fg,			
	unsigned long bg)			
{
	register TScreen *screen = &term->screen;
	Cursor c;
	register Display *dpy = screen->display;

	c = XCreateFontCursor (dpy, cursorindex);
	if (c == (Cursor) 0) return (c);

	recolor_cursor (c, fg, bg);
	return (c);
}


void HandleKeyPressed(
	Widget w  ,
	XEvent *event,
	String *params  ,
	Cardinal *nparams  )
{
    register TScreen *screen = &term->screen;




	Input (&term->keyboard, screen, &event->xkey, 0);
}


void HandleEightBitKeyPressed(
	Widget w  ,
	XEvent *event,
	String *params  ,
	Cardinal *nparams  )
{
    register TScreen *screen = &term->screen;




	Input (&term->keyboard, screen, &event->xkey, 1);
}


void HandleStringEvent(
	Widget w  ,
	XEvent *event  ,
	String *params,
	Cardinal *nparams)
{
    register TScreen *screen = &term->screen;






    if (*nparams != 1) return;

    if ((*params)[0] == '0' && (*params)[1] == 'x' && (*params)[2] != '\0') {
	char c, *p, hexval[2];
	hexval[0] = hexval[1] = 0;
	for (p = *params+2; (c = *p); p++) {
	    hexval[0] *= 16;
	    if (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(c)]&0x1:(isupper)(c))) c = tolower(c);
	    if (c >= '0' && c <= '9')
		hexval[0] += c - '0';
	    else if (c >= 'a' && c <= 'f')
		hexval[0] += c - 'a' + 10;
	    else break;
	}
	if (c == '\0')
	    StringInput (screen, hexval, 1);
    }
    else {
	StringInput (screen, *params, __STRLEN(*params));
    }
}














void HandleInterpret(
	Widget w  ,
	XEvent *event  ,
	String *params,
	Cardinal *param_count)
{
    if (*param_count == 1) {
	char *value = params[0];
	int need = __STRLEN(value);
	int used = usedPtyData(&VTbuffer);
	int have = (VTbuffer.cnt >= 0) ? VTbuffer.cnt : 0;
	int n;

	if (have - used + need < 4096) {

	     ;

	    if (have != 0
	     && used < have) {
		__MEMMOVE(VTbuffer.ptr + (need - used), VTbuffer.ptr, VTbuffer.cnt * sizeof(*VTbuffer.ptr))

;
	    } else {
		initPtyData(&VTbuffer);
		used = 0;
	    }

	    VTbuffer.cnt += (need - used);
	    VTbuffer.ptr -= used;
	    for (n = 0; n < need; n++)
		VTbuffer.ptr[n] = ((value[n]) & 0xff);
	}
    }
}

static void DoSpecialEnterNotify (register XEnterWindowEvent *ev)
{
    register TScreen *screen = &term->screen;




      if (((ev->detail) != 2) &&
	  ev->focus &&
	  !(screen->select & 02))
	selectwindow(screen, 01);
}


void HandleEnterWindow(
	Widget w  ,
	XtPointer eventdata  ,
	XEvent *event  ,
	Boolean *cont  )
{
    
}


static void DoSpecialLeaveNotify (register XEnterWindowEvent *ev)
{
    register TScreen *screen = &term->screen;




      if (((ev->detail) != 2) &&
	  ev->focus &&
	  !(screen->select & 02))
	unselectwindow(screen, 01);
}



void HandleLeaveWindow(
	Widget w  ,
	XtPointer eventdata  ,
	XEvent *event  ,
	Boolean *cont  )
{
    
}



void HandleFocusChange(
	Widget w  ,
	XtPointer eventdata  ,
	XEvent *ev,
	Boolean *cont  )
{
	register XFocusChangeEvent *event = (XFocusChangeEvent  *)ev;
	register TScreen *screen = &term->screen;

	if(event->type == 9)
		selectwindow(screen,
			     (event->detail == 5) ? 01 :
								02);
	else {
		unselectwindow(screen,
			       (event->detail == 5) ? 01 :
								  02);
		if (screen->grabbedKbd && (event->mode == 2)) {
		    Bell(0, 100);
		    ReverseVideo(term);
		    screen->grabbedKbd = 0;
		    UpdateMenuItem(mainMenuEntries[mainMenu_securekbd].widget,term->screen.grabbedKbd);
		}
	}
}


static void
selectwindow(register TScreen *screen, register int flag)
{

	if(screen->TekEmu) {
		if(!Ttoggled)
			TCursorToggle(1);
		screen->select |= flag;
		if(!Ttoggled)
			TCursorToggle(1);
		return;
	} else

	{
		if (screen->xic)
		    XSetICFocus(screen->xic);

		if(screen->cursor_state &&
		   (screen->cursor_col != screen->cur_col ||
		    screen->cursor_row != screen->cur_row))
		    HideCursor();
		screen->select |= flag;
		if(screen->cursor_state)
			ShowCursor();
		return;
	}
}

static void
unselectwindow(register TScreen *screen, register int flag)
{
    if (screen->always_highlight) return;


    if(screen->TekEmu) {
	if(!Ttoggled) TCursorToggle(1);
	screen->select &= ~flag;
	if(!Ttoggled) TCursorToggle(1);
    } else

    {
	if (screen->xic)
	    XUnsetICFocus(screen->xic);
	screen->select &= ~flag;
	if(screen->cursor_state &&
	   (screen->cursor_col != screen->cur_col ||
	    screen->cursor_row != screen->cur_row))
	      HideCursor();
	if(screen->cursor_state)
	  ShowCursor();
    }
}

static long lastBellTime;	

void
Bell(int which  , int percent)
{
    register TScreen *screen = &term->screen;
    struct timeval curtime;
    long now_msecs;

     

    

    if(screen->bellSuppressTime) {
	if(screen->bellInProgress) {
	    do_xevents();
	    if(screen->bellInProgress) { 
		return;
	    }
	}
	gettimeofday(&curtime,(struct timezone *)0);
	now_msecs = 1000*curtime.tv_sec + curtime.tv_usec/1000;
	if(lastBellTime != 0  &&  now_msecs - lastBellTime >= 0  &&
	   now_msecs - lastBellTime < screen->bellSuppressTime) {
	    return;
	}
	lastBellTime = now_msecs;
    }

    if (screen->visualbell)
	VisualBell();
    else



	XBell(screen->display, percent);


    if(screen->bellSuppressTime) {
	



	Widget w = (((screen)->TekEmu) ? (Widget)tekWidget : (Widget)term);
	XChangeProperty((((w)->core.screen)->display), ((w)->core.window),
			((Atom) 62), ((Atom) 62), 8, 2, ((void *) 0), 0);
	screen->bellInProgress = 1;
    }
}

void
VisualBell(void)
{
    register TScreen *screen = &term->screen;
    register Pixel xorPixel = screen->foreground ^ term->core.background_pixel;
    XGCValues gcval;
    GC visualGC;

    gcval.function = 0x6;
    gcval.foreground = xorPixel;
    visualGC = XtGetGC((Widget)term, (1L<<0)+(1L<<2), &gcval);

    if(screen->TekEmu) {
	XFillRectangle(
		       screen->display,
		       ((screen)->fullTwin.window),
		       visualGC,
		       0, 0,
		       (unsigned) ((screen)->fullTwin.fullwidth),
		       (unsigned) ((screen)->fullTwin.fullheight));
	XFlush(screen->display);
	XFillRectangle(
		       screen->display,
		       ((screen)->fullTwin.window),
		       visualGC,
		       0, 0,
		       (unsigned) ((screen)->fullTwin.fullwidth),
		       (unsigned) ((screen)->fullTwin.fullheight));
    } else

    {
	XFillRectangle(
		       screen->display,
		       ((screen)->fullVwin.window),
		       visualGC,
		       0, 0,
		       (unsigned) ((screen)->fullVwin.fullwidth),
		       (unsigned) ((screen)->fullVwin.fullheight));
	XFlush(screen->display);
	XFillRectangle(
		       screen->display,
		       ((screen)->fullVwin.window),
		       visualGC,
		       0, 0,
		       (unsigned) ((screen)->fullVwin.fullwidth),
		       (unsigned) ((screen)->fullVwin.fullheight));
    }
}


void HandleBellPropertyChange(
	Widget w  ,
	XtPointer data  ,
	XEvent *ev,
	Boolean *more  )
{
    register TScreen *screen = &term->screen;

    if (ev->xproperty.atom == ((Atom) 62)) {
	screen->bellInProgress = 0;
    }
}




void HandleDeIconify (
    Widget gw,
    XEvent *event  ,
    String *params  ,
    Cardinal *nparams  )
{
    if ((((gw)->core.widget_class) == xtermWidgetClass)) {
	register TScreen *screen = &((XtermWidget)gw)->screen;
	XMapWindow(screen->display, ((((term)->core.parent))->core.window));
    }
}


void HandleIconify (
    Widget gw,
    XEvent *event  ,
    String *params  ,
    Cardinal *nparams  )
{
    if ((((gw)->core.widget_class) == xtermWidgetClass)) {
	register TScreen *screen = &((XtermWidget)gw)->screen;
	XIconifyWindow(screen->display,
		       ((((term)->core.parent))->core.window),
		       (((_XPrivDisplay)screen->display)->default_screen));
    }
}

int QueryMaximize(TScreen *screen, unsigned *width, unsigned *height)
{
    XSizeHints hints;
    long supp = 0;
    Window root_win;
    int root_x = -1;		
    int root_y = -1;
    unsigned root_border;
    unsigned root_depth;

    if (XGetGeometry(screen->display,
		    XDefaultRootWindow(screen->display),
		    &root_win,
		    &root_x,
		    &root_y,
		    width,
		    height,
		    &root_border,
		    &root_depth)) {
	 





	*width -= (screen->border * 2),
	*height -= (screen->border * 2);

	hints.flags = (1L << 5);
	if (XGetWMNormalHints(screen->display,
			    ((((term)->core.parent))->core.window),
			    &hints,
			    &supp)
	 && (hints.flags & (1L << 5)) != 0) {

	     



	    if ((unsigned) hints.max_width < *width)
		*width = hints.max_width;
	    if ((unsigned) hints.max_height < *height)
		*height = hints.max_height;
	}
	return 1;
    }
    return 0;
}

void RequestMaximize (XtermWidget termw, int maximize)
{
    register TScreen	*screen	= &termw->screen;
    XWindowAttributes win_attrs;
    Position root_x, root_y;
    unsigned root_width, root_height;

    if (maximize) {

	if (QueryMaximize(screen, &root_width, &root_height)) {

	    if (XGetWindowAttributes(screen->display,
				     ((((term)->core.parent))->core.window),
				     &win_attrs)) {
		XtTranslateCoords(toplevel, 0, 0, &root_x, &root_y);

		if (screen->restore_data != 1
		 || screen->restore_width != root_width
		 || screen->restore_height != root_height) {
		    screen->restore_data = 1;
		    screen->restore_x = root_x - win_attrs.x;
		    screen->restore_y = root_y - win_attrs.y;
		    screen->restore_width = win_attrs.width;
		    screen->restore_height = win_attrs.height;
		     




		}

		XMoveResizeWindow(screen->display,
				  ((((term)->core.parent))->core.window),
				  0, 
				  0, 
				  root_width,
				  root_height);
	    }
	}
    } else {
	if (screen->restore_data) {
	     




	    screen->restore_data = 0;
	    XMoveResizeWindow(screen->display,
			      ((((term)->core.parent))->core.window),
			      screen->restore_x,
			      screen->restore_y,
			      screen->restore_width,
			      screen->restore_height);
	}
    }
}


void HandleMaximize (
    Widget gw,
    XEvent *event  ,
    String *params  ,
    Cardinal *nparams  )
{
    if ((((gw)->core.widget_class) == xtermWidgetClass)) {
	RequestMaximize((XtermWidget)gw, 1);
    }
}


void HandleRestoreSize (
    Widget gw,
    XEvent *event  ,
    String *params  ,
    Cardinal *nparams  )
{
    if ((((gw)->core.widget_class) == xtermWidgetClass)) {
	RequestMaximize((XtermWidget)gw, 0);
    }
}


void
Redraw(void)
{
	register TScreen *screen = &term->screen;
	XExposeEvent event;

	event.type = 12;
	event.display = screen->display;
	event.x = 0;
	event.y = 0;
	event.count = 0;

	if(((screen)->fullVwin.window)) {
		event.window = ((screen)->fullVwin.window);
		event.width = term->core.width;
		event.height = term->core.height;
		(*term->core.widget_class->core_class.expose)((Widget)term, (XEvent *)&event, ((void *) 0));
		if(((screen)->fullVwin.scrollbar))
			(*screen->scrollWidget->core.widget_class->core_class.expose)(screen->scrollWidget, (XEvent *)&event, ((void *) 0));
		}


	if(((screen)->fullTwin.window) && screen->Tshow) {
		event.window = ((screen)->fullTwin.window);
		event.width = tekWidget->core.width;
		event.height = tekWidget->core.height;
		TekExpose ((Widget)tekWidget, (XEvent *)&event, ((void *) 0));
	}

}

# 813 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"


# 990 "PRGDISK:[SHARED.PROGRAMS.XTERM-122]MISC.C;2"





static void ReportAnsiColorRequest(XtermWidget pTerm, int colornum, int final)
{
	XColor color;
	Colormap cmap =	pTerm->core.colormap;
	char buffer[80];

	 
	color.pixel = pTerm->screen.Acolors[colornum];
	XQueryColor(term->screen.display, cmap, &color);
	sprintf(buffer, "4;%d;rgb:%04x/%04x/%04x",
                colornum,
		color.red,
		color.green,
		color.blue);
	unparseputc1(0x9D,   pTerm->screen.respond);
	unparseputs(buffer, pTerm->screen.respond);
	unparseputc1(final, pTerm->screen.respond);
}

static Boolean
AllocateAnsiColor(
	XtermWidget	 pTerm,
	int		 color,
	char		*name)
{
XColor			 def;
register TScreen	*screen =	&pTerm->screen;
Colormap		 cmap =		pTerm->core.colormap;

    if (XParseColor(screen->display, cmap, name, &def)
     && XAllocColor(screen->display, cmap, &def)) {
	screen->Acolors[color] = def.pixel;
	 
	return(1);
    }
     
    return(0);
}

static Boolean
ChangeAnsiColorRequest(
	XtermWidget	pTerm,
	register char	*buf,
	int		final)
{
    char *name;
    int color;
    int r = 0;

     

    while (buf && *buf) {
	name = strchr(buf, ';');
	if (name == ((void *) 0))
	    break;
	*name = '\0';
	name++;
	color = atoi(buf);
	if (color < 0 || color >= 16)
	    break;
	buf = strchr(name, ';');
	if (buf) {
	    *buf = '\0';
	    buf++;
	}
	if (!strcmp(name, "?"))
	    ReportAnsiColorRequest(pTerm, color, final);
	else if (!AllocateAnsiColor(pTerm, color, name))
	    break;
	


	r = 1;
    }
    if (r)
	ChangeAnsiColors(pTerm);
    return(r);
}




void
do_osc(Char *oscbuf, int len  , int final)
{
	register TScreen *screen = &(term->screen);
	register int mode;
	register Char *cp;
	int state = 0;
	char *buf = 0;

	






	mode = 0;
	for (cp = oscbuf; *cp != '\0'; cp++) {
		switch (state) {
		case 0:
			if (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(*cp)]&0x4:(isdigit)(*cp))) {
				mode = 10 * mode + (*cp - '0');
				break;
			}
			
		case 1:
			if (*cp != ';')
				return;
			state = 2;
			break;
		case 2:
			buf = (char *)cp;
			state = 3;
			
		default:
			if (!((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(*cp & 0x7f)]&0x80:(isprint)(*cp & 0x7f)))
				return;
		}
	}

	switch(mode) {
	case 0:	
		Changename(buf);
		Changetitle(buf);
		break;

	case 1:	
		Changename(buf);
		break;

	case 2:	
		Changetitle(buf);
		break;

	case 3: 
		ChangeXprop(buf);
		break;

	case 4:
		ChangeAnsiColorRequest(term, buf, final);
		break;

	case 10:	case 11:	case 12:
	case 13:	case 14:	case 15:
	case 16:	case 17:
		if (term->misc.dynamicColors)
		       ChangeColorsRequest(term, mode-10, buf, final);
		break;























	case 50:
		if (buf != 0 && !strcmp(buf, "?")) {
		    int num = screen->menu_font_number;

		    unparseputc1(0x9D,   screen->respond);
		    unparseputs("50",   screen->respond);

		    if ((buf = screen->menu_font_names[num]) != 0) {
			unparseputc(';', screen->respond);
			unparseputs(buf, screen->respond);
		    }
		    unparseputc1(final,    screen->respond);
		} else {
		    




		    if (buf != 0 && *buf == '#') {
			int num = screen->menu_font_number;
			int rel = 0;

			if (*++buf == '+') {
			    rel = 1;
			    buf++;
			} else if (*buf == '-') {
			    rel = -1;
			    buf++;
			}

			if (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(*buf)]&0x4:(isdigit)(*buf))) {
			    int val = atoi(buf);
			    if (rel > 0)
				num += val;
			    else if (rel < 0)
				num -= val;
			    else
				num = val;
			} else if (rel) {
			    num += rel;
			} else {
			    num = 0;
			}

			if (num < 0
			 || num > fontMenu_font6
			 || (buf = screen->menu_font_names[num]) == 0) {
			    Bell(1,0);
			    break;
			}
		    }
		    SetVTFont (fontMenu_fontescape, 1, buf, ((void *) 0));
		}
		break;

	



	}
}


static struct {
	char *str;
	int   len;
	} user_keys[35];

static int
hexvalue(int c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return -1;
}

void
reset_decudk(void)
{
	int n;
	for (n = 0; n < 35; n++) {
		if (user_keys[n].str != 0) {
			free(user_keys[n].str);
			user_keys[n].str = 0;
			user_keys[n].len = 0;
		}
	}
}

void
do_dcs(Char *dcsbuf, size_t dcslen)
{
	register TScreen *screen = &term->screen;
	char *cp = (char *)dcsbuf;

	if (*cp == '$') { 
		char reply[32768];
		int okay = 1;

		cp++;
		if (*cp++ == 'q') {
			if (!strcmp(cp, "\"q")) {		
				sprintf(reply, "%d%s",
					(screen->protected_mode == 1)
					 && (term->flags & 0x40) ? 1 : 0,
					cp);
			} else if (!strcmp(cp, "\"p")) {	
				sprintf(reply, "%d%s",
					(screen->ansi_level ?
					 screen->ansi_level : 1) + 60,
					cp);
			} else if (!strcmp(cp, "r")) {		
				sprintf(reply, "%d;%dr",
					screen->top_marg + 1,
					screen->bot_marg + 1);
			} else if (!strcmp(cp, "m")) {		
				__STRCPY(reply, "0");
				if (term->flags & 0x04)
					strcat(reply, ";1");
				if (term->flags & 0x02)
					strcat(reply, ";4");
				if (term->flags & 0x08)
					strcat(reply, ";5");
				if (term->flags & 0x01)
					strcat(reply, ";7");
				if (term->flags & 0x40000)
					strcat(reply, ";8");
				 

























				if(screen->colorMode) { if (term->flags & 0x20) sprintf(reply+__STRLEN(reply), ";%d%d", term->cur_foreground >= 8 ? 9 : 3, term->cur_foreground >= 8 ? term->cur_foreground - 8 : term->cur_foreground); if (term->flags & 0x10) sprintf(reply+__STRLEN(reply), ";%d%d", term->cur_background >= 8 ? 10 : 4, term->cur_background >= 8 ? term->cur_background - 8 : term->cur_background); }















				strcat(reply, "m");
			} else
				okay = 0;

			unparseputc1(0x90, screen->respond);
			unparseputc(okay ? '1' : '0', screen->respond);
			unparseputc('$', screen->respond);
			unparseputc('r', screen->respond);
			if (okay)
				cp = reply;
			unparseputs(cp, screen->respond);
			unparseputc1(0x9C, screen->respond);
		} else {
			unparseputc(0x18, screen->respond);
		}
	} else { 
		int clear_all = 1;
		int lock_keys = 1;

		if (dcslen != __STRLEN(cp))
			
			return;

		if (*cp == '0') {
			cp++;
		} else if (*cp == '1') {
			cp++;
			clear_all = 0;
		}

		if (*cp == ';')
			cp++;
		else if (*cp != '|')
			return;

		if (*cp == '0') {
			cp++;
		} else if (*cp == '1') {
			cp++;
			lock_keys = 0;
		}

		if (*cp++ != '|')
			return;

		if (clear_all)
			reset_decudk();

		while (*cp) {
			char *str = (char *)malloc(__STRLEN(cp) + 2);
			int key = 0;
			int len = 0;

			while (((decc$$gl___ctypea)?(decc$$ga___ctypet)[(int)(*cp)]&0x4:(isdigit)(*cp)))
				key = (key * 10) + (*cp++ - '0');
			if (*cp == '/') {
				cp++;
				while (*cp != ';' && *cp != '\0') {
					int hi = hexvalue(*cp++);
					int lo = hexvalue(*cp++);
					if (hi >= 0 && lo >= 0)
						str[len++] = (hi << 4) | lo;
					else
						return;
				}
			}
			if (len > 0 && key < 35) {
				if (user_keys[key].str != 0)
					free(user_keys[key].str);
				user_keys[key].str = str;
				user_keys[key].len = len;
			} else {
				free(str);
			}
			if (*cp == ';')
				cp++;
		}
	}
}

char *
udk_lookup(int keycode, int *len)
{
	if (keycode >= 0 && keycode < 35) {
		*len = user_keys[keycode].len;
		return user_keys[keycode].str;
	}
	return 0;
}

static void
ChangeGroup(String attribute, XtArgVal value)
{
	Arg args[1];
	char *name = (value != 0) ? (char *)value : "";


	

	if( sameName ) {
	    char *buf;
	    ((void)( ( args[0]).name = ( attribute), ( args[0]).value = (XtArgVal)( &buf) ));
	    XtGetValues( toplevel, args, 1 );
	    if (strcmp(name, buf) == 0)
		return;
	}


	((void)( ( args[0]).name = ( attribute), ( args[0]).value = (XtArgVal)( name) ));
	XtSetValues( toplevel, args, 1 );
}

void
Changename(register char *name)
{
    if (name == 0)
	name = "";

    if ( zIconBeep && zIconBeep_flagged ) {
	char *newname = (char *)malloc(__STRLEN(name)+ 4 + 1);
	if (!newname) {
	    fprintf((decc$ga_stderr), "malloc failed in Changename\n");
	    return;
	}
	__STRCPY(newname, "*** ");
	strcat(newname, name);
	ChangeGroup( ((char*)&XtShellStrings[0]), (XtArgVal)newname );
	free(newname);
    } else

	ChangeGroup( ((char*)&XtShellStrings[0]), (XtArgVal)name );
}

void
Changetitle(register char *name)
{
    ChangeGroup( ((char*)&XtShellStrings[356]), (XtArgVal)name );
}



void
ChangeXprop(register char *buf)
{
    Display *dpy = (((toplevel)->core.screen)->display);
    Window w = ((toplevel)->core.window);
    XTextProperty text_prop;
    Atom aprop;
    Char *pchEndPropName = (Char *)strchr(buf,'=');

    if (pchEndPropName)
	*pchEndPropName = '\0';
    aprop = XInternAtom(dpy, buf, 0);
    if (pchEndPropName == ((void *) 0)) {
    
	XDeleteProperty(dpy, w, aprop);
    } else {
	text_prop.value = pchEndPropName+1;
	text_prop.encoding = ((Atom) 31);
	text_prop.format = 8;
	text_prop.nitems = __STRLEN((char *)(text_prop.value));
	XSetTextProperty(dpy,w,&text_prop,aprop);
    }
}



ScrnColors	*pOldColors= ((void *) 0);

static Boolean
GetOldColors(XtermWidget pTerm)
{
int	i;
    if (pOldColors == ((void *) 0)) {
	pOldColors = (ScrnColors *)XtMalloc(sizeof(ScrnColors));
	if (pOldColors == ((void *) 0)) {
	    fprintf((decc$ga_stderr),"allocation failure in GetOldColors\n");
	    return(0);
	}
	pOldColors->which = 0;
	for (i = 0; i < 8; i++) {
	    pOldColors->colors[i] = 0;
	    pOldColors->names[i] = ((void *) 0);
	}
	GetColors(pTerm,pOldColors);
    }
    return(1);
}

static void ReportColorRequest(XtermWidget pTerm, int ndx, int final)
{
	XColor color;
	Colormap cmap =	pTerm->core.colormap;
	char buffer[80];

	GetOldColors(pTerm);
	color.pixel = pOldColors->colors[ndx];
	 
	XQueryColor(term->screen.display, cmap, &color);
	sprintf(buffer, "%d;rgb:%04x/%04x/%04x", ndx + 10,
		color.red,
		color.green,
		color.blue);
	unparseputc1(0x9D,   pTerm->screen.respond);
	unparseputs(buffer, pTerm->screen.respond);
	unparseputc1(final, pTerm->screen.respond);
}

static Boolean
UpdateOldColors(
	XtermWidget	pTerm  ,
	ScrnColors	*pNew)
{
int	i;

    






    for (i = 0; i < 8; i++) {
	if (((pNew)->which&(1<<( i)))) {
	    if (pOldColors->names[i] != ((void *) 0)) {
		XtFree(pOldColors->names[i]);
		pOldColors->names[i] = ((void *) 0);
	    }
	    if (pNew->names[i]) {
		pOldColors->names[i] = pNew->names[i];
	    }
	    pOldColors->colors[i] = pNew->colors[i];
	}
    }
    return(1);
}

void
ReverseOldColors(void)
{
register ScrnColors	*pOld= pOldColors;
Pixel	 tmpPix;
char	*tmpName;

    if (pOld) {
	
	if (pOld->colors[2] == pOld->colors[0]) {
	    pOld->colors[2] =	 pOld->colors[1];
	    if (pOld->names[2]) {
		XtFree(pOldColors->names[2]);
		pOld->names[2] = ((void *) 0);
	    }
	    if (pOld->names[1]) {
		tmpName = XtMalloc(__STRLEN(pOld->names[1])+1);
		if (tmpName) {
		    __STRCPY(tmpName, pOld->names[1]);
		    pOld->names[2]= tmpName;
		}
	    }
	}

	 tmpPix = pOld->colors[0]; pOld->colors[0] = pOld->colors[1]; pOld->colors[1] = tmpPix;;
	 tmpName = pOld->names[0]; pOld->names[0] = pOld->names[1]; pOld->names[1] = tmpName;;

	 tmpPix = pOld->colors[3]; pOld->colors[3] = pOld->colors[4]; pOld->colors[4] = tmpPix;;
	 tmpName = pOld->names[3]; pOld->names[3] = pOld->names[4]; pOld->names[4] = tmpName;;

	 tmpPix = pOld->colors[5]; pOld->colors[5] = pOld->colors[6]; pOld->colors[6] = tmpPix;;
	 tmpName = pOld->names[5]; pOld->names[5] = pOld->names[6]; pOld->names[6] = tmpName;;
    }
    return;
}

static Boolean
AllocateColor(
	XtermWidget	 pTerm,
	ScrnColors	*pNew,
	int		 ndx,
	char		*name)
{
XColor			 def;
register TScreen	*screen =	&pTerm->screen;
Colormap		 cmap =		pTerm->core.colormap;
char			*newName;

    if (XParseColor(screen->display, cmap, name, &def)
     && XAllocColor(screen->display, cmap, &def)
     && (newName = XtMalloc(__STRLEN(name)+1)) != 0) {
	(((pNew)->colors[ ndx]=( def.pixel)),((pNew)->which|=(1<<( ndx))));
	__STRCPY(newName, name);
	(((pNew)->names[ ndx]=( newName)),((pNew)->which|=(1<<( ndx))));
	 
	return(1);
    }
     
    return(0);
}

static Boolean
ChangeColorsRequest(
	XtermWidget	pTerm,
	int		start,
	register char	*names,
	int		final)
{
char		*thisName;
ScrnColors	newColors;
int		i, ndx;

     

    if ((pOldColors == ((void *) 0))
     && (!GetOldColors(pTerm))) {
	return(0);
    }
    newColors.which = 0;
    for (i = 0; i < 8; i++) {
	newColors.names[i] = ((void *) 0);
    }
    for (i = start; i < 8; i++) {
	if (term->misc.re_verse)	ndx = (((i)==0?1:	((i)==1?0:	((i)==3?4:	((i)==4?3:	((i)==5?6:	((i)==1?5:(i))))))));
	else				ndx = i;
	if ((names == ((void *) 0)) || (names[0] == '\0')) {
	    newColors.names[ndx] = ((void *) 0);
	}
	else {
	    if (names[0] == ';')
		 thisName=	((void *) 0);
	    else thisName=	names;
	    names = strchr(names, ';');
	    if (names != ((void *) 0)) {
		*names=	'\0';
		names++;
	    }
	    if (thisName != 0 && !strcmp(thisName, "?"))
		ReportColorRequest(pTerm, ndx, final);
	    else if (!pOldColors->names[ndx]
	     || (thisName
	      && strcmp(thisName, pOldColors->names[ndx]))) {
		AllocateColor(pTerm, &newColors, ndx, thisName);
	    }
	}
    }

    if (newColors.which == 0)
	return(1);

    ChangeColors(pTerm, &newColors);
    UpdateOldColors(pTerm, &newColors);
    return(1);
}






void
Panic(char *s  , int a  )
{








}

char *SysErrorMsg (int n)
{
    register char *s = strerror(n);
    return s ? s : "unknown error";
}

void
SysError (int i)
{
	int oerrno;

	oerrno = (*CMA$TIS_ERRNO_GET_ADDR());
	
	fprintf ((decc$ga_stderr), "%s: Error %d, errno %d: ", xterm_name, i, oerrno);
	fprintf ((decc$ga_stderr), "%s\n", SysErrorMsg (oerrno));
	Cleanup(i);
}

void
Error (int i)
{
	fprintf ((decc$ga_stderr), "%s: Error %d\n", xterm_name, i);
	Cleanup(i);
}




void
Cleanup (int code)
{
	static Boolean cleaning;
	register TScreen *screen;

	if (cleaning) {
		hold_screen = 0;
		return;
	}
	cleaning = 1;

	 

	screen = &term->screen;

	if (hold_screen) {
		hold_screen = 2;
		while (hold_screen) {
			xevents();
		}
	}

	if (screen->pid > 1) {
	    (void) kill_process_group (screen->pid, 1);
	}






	Exit (code);
}








void
Setenv (register char *var, register char *value)
{
	register int envindex = 0;
	register size_t len = __STRLEN(var);

	 

	while (environ [envindex] != ((void *) 0)) {
	    if (strncmp (environ [envindex], var, len) == 0) {
		
		environ[envindex] = (char *)malloc ((unsigned)len + __STRLEN(value) + 1);
		__STRCPY(environ [envindex], var);
		strcat (environ [envindex], value);
		return;
	    }
	    envindex ++;
	}

	 

	environ [envindex] = (char *) malloc ((unsigned)len + __STRLEN(value) + 1);
	(void) __STRCPY(environ [envindex], var);
	strcat (environ [envindex], value);
	environ [++envindex] = ((void *) 0);
}





char *strindex ( register char *s1, register char *s2)
{
	register char	*s3;
	size_t s2len = __STRLEN(s2);

	while ((s3=strchr(s1, *s2)) != ((void *) 0)) {
		if (strncmp(s3, s2, s2len) == 0)
			return (s3);
		s1 = ++s3;
	}
	return (((void *) 0));
}


int
xerror(Display *d, register XErrorEvent *ev)
{
    fprintf ((decc$ga_stderr), "%s:  warning, error event received:\n", xterm_name);
    (void) XmuPrintDefaultErrorMessage (d, ev, (decc$ga_stderr));
    Exit (83);
    return 0;	
}


int
xioerror(Display *dpy)
{
    (void) fprintf ((decc$ga_stderr),
		    "%s:  fatal IO error %d (%s) or KillClient on X server \"%s\"\r\n",
		    xterm_name, (*CMA$TIS_ERRNO_GET_ADDR()), SysErrorMsg ((*CMA$TIS_ERRNO_GET_ADDR())),
		    (((_XPrivDisplay)dpy)->display_name));

    Exit(84);
    return 0;	
}

extern char *ProgramName;

void xt_error(String message)
{
    (void) fprintf ((decc$ga_stderr), "%s Xt error: %s\n", ProgramName, message);
    exit(1);
}

int
XStrCmp(char *s1, char *s2)
{
  if (s1 && s2) return(strcmp(s1, s2));
  if (s1 && *s1) return(1);
  if (s2 && *s2) return(-1);
  return(0);
}


static void withdraw_window (Display *dpy, Window w, int scr)
{
     
    (void) XmuUpdateMapHints (dpy, w, ((void *) 0));
    XWithdrawWindow (dpy, w, scr);
    return;
}



void set_vt_visibility (Boolean on)
{
    register TScreen *screen = &term->screen;

     
    if (on) {
	if (!screen->Vshow && term) {
	    VTInit ();
	    XMapWindow((((((term)->core.parent))->core.screen)->display), ((((term)->core.parent))->core.window));




	    screen->Vshow = 1;
	}
    }

    else {
	if (screen->Vshow && term) {
	    withdraw_window ((((term)->core.screen)->display),
			     ((((term)->core.parent))->core.window),
			     XScreenNumberOfScreen(((term)->core.screen)));
	    screen->Vshow = 0;
	}
    }
    SetItemSensitivity(vtMenuEntries[vtMenu_vthide].widget,term->screen.Tshow);
    SetItemSensitivity(tekMenuEntries[tekMenu_tekhide].widget,term->screen.Vshow);
    { UpdateMenuItem(vtMenuEntries[vtMenu_tekmode].widget,term->screen.TekEmu); UpdateMenuItem(tekMenuEntries[tekMenu_vtmode].widget,!term->screen.TekEmu); };
    UpdateMenuItem(vtMenuEntries[vtMenu_tekshow].widget,term->screen.Tshow);
    UpdateMenuItem(tekMenuEntries[tekMenu_vtshow].widget,term->screen.Vshow);

    return;
}


extern Atom wm_delete_window;	

void set_tek_visibility (Boolean on)
{
    register TScreen *screen = &term->screen;

     
    if (on) {
	if (!screen->Tshow && (tekWidget || TekInit())) {
	    Widget tekParent = ((tekWidget)->core.parent);
	    XtRealizeWidget (tekParent);
	    XMapWindow((((tekParent)->core.screen)->display), ((tekParent)->core.window));
	    XtOverrideTranslations(tekParent,
				   XtParseTranslationTable
				   ("<Message>WM_PROTOCOLS: DeleteWindow()"));
	    (void) XSetWMProtocols ((((tekParent)->core.screen)->display),
				    ((tekParent)->core.window),
				    &wm_delete_window, 1);
	    screen->Tshow = 1;
	}
    } else {
	if (screen->Tshow && tekWidget) {
	    withdraw_window ((((tekWidget)->core.screen)->display),
			     ((((tekWidget)->core.parent))->core.window),
			     XScreenNumberOfScreen(((tekWidget)->core.screen)));
	    screen->Tshow = 0;
	}
    }
    SetItemSensitivity(tekMenuEntries[tekMenu_tekhide].widget,term->screen.Vshow);
    SetItemSensitivity(vtMenuEntries[vtMenu_vthide].widget,term->screen.Tshow);
    UpdateMenuItem(tekMenuEntries[tekMenu_vtshow].widget,term->screen.Vshow);
    UpdateMenuItem(vtMenuEntries[vtMenu_tekshow].widget,term->screen.Tshow);
    { UpdateMenuItem(vtMenuEntries[vtMenu_tekmode].widget,term->screen.TekEmu); UpdateMenuItem(tekMenuEntries[tekMenu_vtmode].widget,!term->screen.TekEmu); };
    return;
}

void end_tek_mode (void)
{
    register TScreen *screen = &term->screen;

    if (screen->TekEmu) {






	longjmp(Tekend, 1);
    }
    return;
}

void end_vt_mode (void)
{
    register TScreen *screen = &term->screen;

    if (!screen->TekEmu) {






	screen->TekEmu = 1;
	longjmp(VTend, 1);
    }
    return;
}

void switch_modes (int tovt)		
{
    if (tovt) {
	if (TekRefresh) dorefresh();
	end_tek_mode ();		
    } else {
	end_vt_mode ();			
    }
}

void hide_vt_window (void)
{
    register TScreen *screen = &term->screen;

    set_vt_visibility (0);
    if (!screen->TekEmu) switch_modes (0);	
}

void hide_tek_window (void)
{
    register TScreen *screen = &term->screen;

    set_tek_visibility (0);
    TekRefresh = (TekLink *)0;
    if (screen->TekEmu) switch_modes (1);	
}

