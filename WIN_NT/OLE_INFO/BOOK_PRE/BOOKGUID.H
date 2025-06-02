/*
 * BOOKGUID.H
 *
 * CLSIDs and IIDs defined for the Windows Object book.  This file
 * centralizes them all so we can keep track of which ones are used.
 * The file also includes a number of useful macros that all samples
 * might be interested in as well as including DEBUG.H and WIN1632.H.
 *
 * Copyright (c)1993 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _BOOKGUID_H_
#define _BOOKGUID_H_


#include <win1632.h>
#include <debug.h>

#ifdef INITGUIDS
#include <initguid.h>
#endif


//Types that OLE2.H et. al. leave out
typedef LPVOID FAR * LPLPVOID;

#ifdef _OLE2_H_   //May not include ole2.h at all times.
typedef POINTL FAR * LPPOINTL;

//Useful macros.
#define SETFormatEtc(fe, cf, asp, td, med, li)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=asp;\
    (fe).ptd=td;\
    (fe).tymed=med;\
    (fe).lindex=li;\
    };

#define SETDefFormatEtc(fe, cf, med)   \
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=DVASPECT_CONTENT;\
    (fe).ptd=NULL;\
    (fe).tymed=med;\
    (fe).lindex=-1;\
    };


#define SETRECTL(rcl, l, t, r, b) \
    {\
    (rcl).left=l;\
    (rcl).top=t;\
    (rcl).right=r;\
    (rcl).bottom=b;\
    }

#define SETSIZEL(szl, h, v) \
    {\
    (szl).cx=h;\
    (szl).cy=v;\
    }


#define RECTLFROMRECT(rcl, rc)\
    {\
    (rcl).left=(long)(rc).left;\
    (rcl).top=(long)(rc).top;\
    (rcl).right=(long)(rc).right;\
    (rcl).bottom=(long)(rc).bottom;\
    }


#define RECTFROMRECTL(rc, rcl)\
    {\
    (rc).left=(int)(rcl).left;\
    (rc).top=(int)(rcl).top;\
    (rc).right=(int)(rcl).right;\
    (rc).bottom=(int)(rcl).bottom;\
    }


#define POINTLFROMPOINT(ptl, pt) \
    { \
    (ptl).x=(long)(pt).x; \
    (ptl).y=(long)(pt).y; \
    }


#define POINTFROMPOINTL(pt, ptl) \
    { \
    (pt).x=(int)(ptl).x; \
    (pt).y=(int)(ptl).y; \
    }

//Here's one that should be in windows.h
#define SETPOINT(pt, h, v) \
    {\
    (pt).x=h;\
    (pt).y=v;\
    }

#define SETPOINTL(ptl, h, v) \
    {\
    (ptl).x=h;\
    (ptl).y=v;\
    }

#endif  //_OLE2_H_



/*
 * GUIDs
 *
 * For this book the author has been allocated the GUIDs in the range:
 *     00021100-0000-0000-C000-000000000046
 *     000211FF-0000-0000-C000-000000000046
 *
 * Gaps in the sequences are GUIDS used by the author for purposes other
 * than this book, e.g. magazine articles.
 */


//GUIDs for Chapter 4

//Koala
DEFINE_OLEGUID(CLSID_Koala,              0x00021102, 0, 0);


//Polyine
DEFINE_OLEGUID(CLSID_Polyline4,          0x00021103, 0, 0);
DEFINE_OLEGUID(IID_IPolyline4,           0x00021104, 0, 0);
DEFINE_OLEGUID(IID_IPolylineAdviseSink4, 0x00021105, 0, 0);


//GUIDs for Chapter 5

//Patron Files for Chapter 5 and beyond:  becomes Patron's App ID
DEFINE_OLEGUID(CLSID_PatronPages,        0x00021106, 0, 0);

//Schmoo Files for Chapter 5 and beyond:  becomes Schmoo's App ID
DEFINE_OLEGUID(CLSID_Schmoo2Figure,      0x00021107, 0, 0);

//Polyine
DEFINE_OLEGUID(CLSID_Polyline5,          0x00021108, 0, 0);
DEFINE_OLEGUID(IID_IPolyline5,           0x00021109, 0, 0);
DEFINE_OLEGUID(IID_IPolylineAdviseSink5, 0x0002110A, 0, 0);


//GUIDs for Chapter 6

//DataObject variations for the data set.  Same server supplies them all.
DEFINE_OLEGUID(CLSID_DataObjectSmall,    0x0002110B, 0, 0);
DEFINE_OLEGUID(CLSID_DataObjectMedium,   0x0002110C, 0, 0);
DEFINE_OLEGUID(CLSID_DataObjectLarge,    0x0002110D, 0, 0);

//Polyine
DEFINE_OLEGUID(CLSID_Polyline6,          0x0002110E, 0, 0);
DEFINE_OLEGUID(IID_IPolyline6,           0x0002110F, 0, 0);
DEFINE_OLEGUID(IID_IPolylineAdviseSink6, 0x00021110, 0, 0);

/*
 * The same numbers are defined in OLEGUID.H as CLSID_StaticMetafile
 * and CLSID_StaticDib.  Those will define the symbols but the actual
 * values are not in OLE2.LIB as all IIDs and other CLSIDs are.  To
 * use the GUIDs, we have to define our own labels for them but still
 * using the same GUIDs.
 */

DEFINE_OLEGUID(CLSID_FreeMetafile,       0x00000315, 0, 0);
DEFINE_OLEGUID(CLSID_FreeDib,            0x00000316, 0, 0);


//GUIDS for Chapter 7

//Data Transfer Component Object
DEFINE_OLEGUID(CLSID_DataTransferObject, 0x00021111, 0, 0);


//Schmoo in chapter 10 uses CLSID_Schmoo2Figure above.


//GUIDs for Chapter 11

//HSchmoo is registered under CLSID_Schmoo2Figure

//Polyine
DEFINE_OLEGUID(CLSID_Polyline11,         0x00021112, 0, 0);


//GUIDs for Chapter 12

//LinksAssistant
DEFINE_OLEGUID(CLSID_LinksAssistant,     0x00021113, 0, 0);

//OLE 2.0 SDK forgot this one.
DEFINE_OLEGUID(IID_IOleUILinkContainer,  0x00021114, 0, 0);


#endif //_BOOKGUID_H_
