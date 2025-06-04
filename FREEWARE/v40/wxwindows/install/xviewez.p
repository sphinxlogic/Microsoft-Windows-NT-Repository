Here is the patch so that you can upgrade to XView 3.2 (source on
most X contrib hosts) and still use your OpenWindows 3.0 deskset apps.
The cmdtool/shelltool size bug is not addressed so I switch back to
XView 3.1 with Sun patches. Both work.
______________________
<A HREF="whois://rs.internic.net/ecz">Edward C. Zimmermann</A>
<A HREF="http://info.bsn.com/BSn.html">Basis Systeme netzwerk/Munich</A>


--1461799882-1688099999-785700608:#1064
Content-Type: APPLICATION/octet-stream; name="xview.bw.patch"
Content-ID: <Pine.3.05.9411241908.B1064@GATEMASTER.CRG.NET>
Content-Description: 

*** ./lib/libxview/base/xv_init.c.orig	Mon Jun 28 22:14:39 1993
--- ./lib/libxview/base/xv_init.c	Sat Aug 14 16:32:52 1993
***************
*** 140,145 ****
--- 140,147 ----
       */
      for (attrs = attrs_start; *attrs; attrs = attr_next(attrs)) {
  	switch ((Xv_attr) attrs[0]) {
+ 	case XV_INIT_ARGS_K1:
+ 	case XV_INIT_ARGS_K2:
  	  case XV_INIT_ARGS:
  	    argc = (int) attrs[1];
  	    argv = (char **) attrs[2];
***************
*** 160,166 ****
  #endif /* OW_I18N */
  	    }
  	    break;
! 
  	  case XV_INIT_ARGC_PTR_ARGV:
  	    argc = *(int *) attrs[1];
  	    argv = (char **) attrs[2];
--- 162,169 ----
  #endif /* OW_I18N */
  	    }
  	    break;
! 	case XV_INIT_ARGC_PTR_ARGV_K1:
! 	case XV_INIT_ARGC_PTR_ARGV_K2:
  	  case XV_INIT_ARGC_PTR_ARGV:
  	    argc = *(int *) attrs[1];
  	    argv = (char **) attrs[2];
***************
*** 240,246 ****
  	    help_proc = (void (*) ()) attrs[1];
  	    ATTR_CONSUME(attrs[0]);
  	    break;
! 
  	  case XV_INIT_ARGS:
  	    argc = (int) attrs[1];
  	    argv = (char **) attrs[2];
--- 243,250 ----
  	    help_proc = (void (*) ()) attrs[1];
  	    ATTR_CONSUME(attrs[0]);
  	    break;
! 	case XV_INIT_ARGS_K1:
! 	case XV_INIT_ARGS_K2:
  	  case XV_INIT_ARGS:
  	    argc = (int) attrs[1];
  	    argv = (char **) attrs[2];
***************
*** 250,256 ****
  
  	    ATTR_CONSUME(attrs[0]);
  	    break;
! 
  	  case XV_INIT_ARGC_PTR_ARGV:
  	    argv = (char **) attrs[2];
  
--- 254,261 ----
  
  	    ATTR_CONSUME(attrs[0]);
  	    break;
! 	case XV_INIT_ARGC_PTR_ARGV_K1:
! 	case XV_INIT_ARGC_PTR_ARGV_K2:
  	  case XV_INIT_ARGC_PTR_ARGV:
  	    argv = (char **) attrs[2];
  
*** ./lib/libxview/base/generic.h.orig	Mon Jun 28 22:14:38 1993
--- ./lib/libxview/base/generic.h	Sat Aug 14 16:34:51 1993
***************
*** 247,254 ****
--- 247,258 ----
  } Xv_generic_struct;
  
  typedef enum {
+     XV_INIT_ARGS_K1             = XV_ATTR(ATTR_INT_PAIR,         	3),
      XV_INIT_ARGS             = XV_ATTR(ATTR_INT_PAIR,         	4),
+     XV_INIT_ARGS_K2             = XV_ATTR(ATTR_INT_PAIR,         	5),
+     XV_INIT_ARGC_PTR_ARGV_K1    = XV_ATTR(ATTR_INT_PAIR,         	6),  /* -S- */
      XV_INIT_ARGC_PTR_ARGV    = XV_ATTR(ATTR_INT_PAIR,         	7),  /* -S- */
+     XV_INIT_ARGC_PTR_ARGV_K2    = XV_ATTR(ATTR_INT_PAIR,         	8),  /* -S- */
      XV_USAGE_PROC       = XV_ATTR(ATTR_FUNCTION_PTR,     	9),  /* -S- */
      XV_ERROR_PROC       = XV_ATTR(ATTR_FUNCTION_PTR,    	12),
      XV_X_ERROR_PROC	= XV_ATTR(ATTR_FUNCTION_PTR,    	15)
*** ./lib/libxview/ttysw/term_ntfy.c.orig	Mon Jun 28 22:17:22 1993
--- ./lib/libxview/ttysw/term_ntfy.c	Sat Aug 14 16:38:39 1993
***************
*** 187,192 ****
--- 187,199 ----
  	if (textsw == TTY_VIEW_PUBLIC(ttysw_view)) {
  	    extern Notify_value ttysw_event();
  	    nv = ttysw_event(TTY_VIEW_PUBLIC(ttysw_view), event, arg, type);
+ #if	1
+ 	    /* jk: termsw cursor hack */
+ 	    if (event_id(event) == KBD_USE) {
+ 	        notify_next_event_func((Notify_client) (textsw),
+ 				       (Notify_event) event, arg, type);
+ 	    }
+ #endif
  	} else {
  	    nv = notify_next_event_func((Notify_client) (textsw),
  					(Notify_event) event, arg, type);
*** ./lib/libxview/textsw/txt_menu.c.orig	Tue Jun 29 16:44:57 1993
--- ./lib/libxview/textsw/txt_menu.c	Sat Aug 14 16:37:27 1993
***************
*** 731,737 ****
      textsw_freeze_caret(folio);
  
      xv_set(folio->menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
! 	   textsw_view, 0);
  
      menu_show(folio->menu, textsw_view, ie, 0);
  
--- 731,741 ----
      textsw_freeze_caret(folio);
  
      xv_set(folio->menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
! 	   textsw_view,
! #if	1
! 	   MENU_DONE_PROC, textsw_done_menu,
! #endif
! 	   0);
  
      menu_show(folio->menu, textsw_view, ie, 0);
  
*** ./lib/libolgx/ol_button.c.orig	Mon Jun 28 22:18:26 1993
--- ./lib/libolgx/ol_button.c	Tue Aug 31 07:34:16 1993
***************
*** 772,778 ****
--- 772,794 ----
  	width = XTextWidth(TextFont_Struct(info),string,len);
      }	
  #endif /* OW_I18N */
+ #if	1
+     /*
+      * jk 27.7.93:
+      * gc_rec[OLGX_TEXTGC_REV] not initialized on original code
+      */
+     if (!info->gc_rec[OLGX_TEXTGC])
+ 	olgx_initialise_gcrec(info, OLGX_TEXTGC);
+     if (!info->three_d && !info->gc_rec[OLGX_TEXTGC_REV])
+ 	olgx_initialise_gcrec(info, OLGX_TEXTGC_REV);
+ 
      if ((state & OLGX_INVOKED) && !(info->three_d)) {
+ 	gc = info->gc_rec[OLGX_TEXTGC_REV]->gc;
+     } else {
+ 	gc = info->gc_rec[OLGX_TEXTGC]->gc;
+     }
+ #else
+     if ((state & OLGX_INVOKED) && !(info->three_d)) {
  	if (!info->gc_rec[OLGX_TEXTGC_REV])
  	    olgx_initialise_gcrec(info, OLGX_TEXTGC_REV);
  	gc = info->gc_rec[OLGX_TEXTGC_REV]->gc;
***************
*** 781,786 ****
--- 797,803 ----
  	    olgx_initialise_gcrec(info, OLGX_TEXTGC);
  	gc = info->gc_rec[OLGX_TEXTGC]->gc;
      }
+ #endif
      if (state & OLGX_INACTIVE) {
  	XSetFillStyle(info->dpy,gc,FillStippled);
      }

--1461799882-1688099999-785700608:#1064--


