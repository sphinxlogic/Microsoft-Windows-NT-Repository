#ifndef _VMS_R4_COMPAT_H_
#define _VMS_R4_COMPAT_H_

#if (XlibSpecificationRelease<5) /* suggested by M.Zinser ###jp###*/


extern XrmQuark XrmPermStringToQuark(
#if NeedFunctionPrototypes
    char *
#endif
);


extern XrmDatabase XrmGetDatabase(
#if NeedFunctionPrototypes
    Display *
#endif
);


extern XrmDatabase XtScreenDatabase(
#if NeedFunctionPrototypes
    Screen *
#endif
);


extern void XrmSetDatabase(
#if NeedFunctionPrototypes
    Display *,
    XrmDatabase
#endif
);


#endif /* XlibSpecificationRelease */

#endif /* _VMS_R4_COMPAT_H_ */
