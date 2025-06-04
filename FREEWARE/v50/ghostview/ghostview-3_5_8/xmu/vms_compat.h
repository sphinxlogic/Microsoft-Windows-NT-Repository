extern void vms_bcopy(
#if NeedFunctionPrototypes
     char *,
     char *,
     long
#endif
);

extern void vms_bzero(
#if NeedFunctionPrototypes
     char *,
     long
#endif
);

#if __VMS_VER < 70000000
#define bcopy vms_bcopy
#define bzero vms_bzero
#endif
