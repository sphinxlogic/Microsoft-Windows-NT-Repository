
#ifdef VMS
#   if (XtSpecificationRelease<5) && !defined(CADDR_T) && !defined(__CADDR_T)
#      if (XlibSpecificationRelease<5)
          typedef char * caddr_t;
#      else
#         if NeedFunctionPrototypes
             typedef void* caddr_t;
#         else
             typedef char* caddr_t;
#         endif
#      endif
#      define __CADDR_T
#      define CADDR_T
#   endif
#endif
