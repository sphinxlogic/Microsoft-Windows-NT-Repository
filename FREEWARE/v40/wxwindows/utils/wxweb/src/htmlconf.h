///////////////////////////////////////////////////////////////////////////////
//
// Description
//
//  An HTML canvas for wxWindows.
//
// Edit History
//
//	  Started November 25th 1995 by Andrew Davison.
//
// Copyright (c) 1995-6 by Andrew Davison. Permission is granted to use the
// source in unmodified form quite freely, and to modify the source and use
// as part of a WXWINDOWS program only.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HTMLCONF_H
#define HTMLCONF_H

// Set to "1" for networking...

#ifndef USE_WXWWW
#define USE_WXWWW 1
#endif

// Set to "1" to use frames...

#ifndef USE_HTML_FRAMES
#define USE_HTML_FRAMES 1
#endif

// Set to "1" to be able to request and display images...

#ifndef USE_WX_IMAGE
#define USE_WX_IMAGE 1
#endif

// Allow site-builder (WSML) operation...

#ifndef ALLOW_SITE_MODE
#define ALLOW_SITE_MODE 1
#endif

// Set to "1" to use forms. If enabled then will
// derive from wxPanel rather than wxCanvas...

#ifndef USE_HTML_FORMS
#ifdef wx_xview
#define USE_HTML_FORMS 0
#else
#define USE_HTML_FORMS 1
#endif
#endif

// Allow WYSIWYG edit mode. To use you either need
// USE_HTML_FORMS set to "0" or patch wx_win.cpp
// to pass on key-press from a canvas...

#ifndef ALLOW_EDIT_MODE
#define ALLOW_EDIT_MODE 0
#endif

// Using SSL? If so you must get the wonderfull
// SSLeay package! 

#ifndef USE_SSL
#define USE_SSL 0
#endif

//////////////////////////////////////////////////////////////////////
// You probably won't need to change anything past here!!!
//////////////////////////////////////////////////////////////////////

#ifdef wx_x
#define strcmpi(s1, s2) strcasecmp(s1, s2)
#define strncmpi(s1, s2, n) strncasecmp(s1, s2, n)
#ifdef BSD
extern "C" int strcasecmp(const char*s1, const char* s2);
extern "C" int strncasecmp(const char*s1, const char* s2, int n);
#endif
#elif !defined (__BORLANDC__)
#define strcmpi _stricmp
#define strncmpi _strnicmp
#endif

#ifdef wx_msw
#define SEP '\\'
#else
#define SEP '/'
#endif

#ifndef DEFAULT_LEFT_MARGIN 
const float DEFAULT_LEFT_MARGIN = 4.0F;
#endif

#ifndef TAB_SIZE
const TAB_SIZE = 4;
#endif

#ifndef MAX_TABS
const MAX_TABS = 16;
#endif

const SCROLLBAR_WIDTH = 20;        // What is it really
const VSCROLL_INCREMENT = 50;

// Probably won't need to change this...

const HTML_COPY = 100;
const HTML_CUT = 101;
const HTML_PASTE = 102;
const HTML_PROPERTIES = 103;
const HTML_BACK = 104;
const HTML_EDIT = 105;
const HTML_BOLD = 106;
const HTML_ITALIC = 107;
const HTML_UNDERLINE = 108;
const HTML_STRIKE = 109;
const HTML_FIXED = 110;
const HTML_PLUS = 111;
const HTML_MINUS = 112;

#endif


