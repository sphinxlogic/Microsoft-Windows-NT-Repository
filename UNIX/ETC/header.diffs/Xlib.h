--- /usr/X/include/X11/Xlib.h~	Sun Nov  1 17:20:36 1992
+++ /usr/X/include/X11/Xlib.h	Tue May  4 15:42:41 1993
@@ -52,8 +52,12 @@
 #include <stddef.h>
 #else
 /* replace this with #include or typedef appropriate for your system */
+#ifndef _WCHAR_T
+/* This was previously unprotected (Greg Lehey, LEMIS, 4 May 1993) */
 typedef unsigned long wchar_t;
+#define _WCHAR_T
 #endif
+#endif
 
 typedef char *XPointer;
 
