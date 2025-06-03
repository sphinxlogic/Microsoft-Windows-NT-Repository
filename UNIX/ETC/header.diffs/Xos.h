--- /usr/X/include/X11/Xlib.h~	Sun Nov  1 17:20:36 1992
+++ /usr/X/include/X11/Xlib.h	Mon Sep 13 14:51:47 1993
@@ -52,8 +52,12 @@
 #include <stddef.h>
 #else
 /* replace this with #include or typedef appropriate for your system */
-typedef unsigned long wchar_t;
+/* Replaced by Greg Lehey, LEMIS, 13 September 1993 */
+#ifndef _WCHAR_T
+#define _WCHAR_T
+typedef long wchar_t;
 #endif
+#endif
 
 typedef char *XPointer;
 
