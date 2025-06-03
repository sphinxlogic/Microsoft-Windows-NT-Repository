/* $XFree86: mit/server/ddx/x386/accel/s3/s3pcach.h,v 2.1 1993/08/17 16:16:39 dawes Exp $ */

#undef DEBUG_PCACHE

  typedef struct _CacheInfo {
     int   size;
     int   id;
     int   x;
     int   y;
     int   w;
     int   h;
     int   nx;
     int   ny;
     int   pix_w;
     int   pix_h;
     unsigned int lru;
  }
CacheInfo, *CacheInfoPtr;
