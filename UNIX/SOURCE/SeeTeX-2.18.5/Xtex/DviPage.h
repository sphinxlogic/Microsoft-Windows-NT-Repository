#ifndef _DviPage_h
#define _DviPage_h

/****************************************************************
*
* DviPage widget
*
****************************************************************/

/* Resources:

Name		     Class		RepType		Default Value
----		     -----		-------		-------------
background	     Background		Pixel		XtDefaultBackground
border		     BorderColor	Pixel		XtDefaultForeground
borderWidth	     BorderWidth	Dimension	1
destroyCallback     Callback		Pointer		NULL
height		     Height		Dimension	0
mappedWhenManaged   MappedWhenManaged	Boolean		True
sensitive	     Sensitive		Boolean		True
width		     Width		Dimension	0
x		     Position		Position	0
y		     Position		Position	0

*/

#define XtNpage		"page"
#define XtNpai		"pai"
#define XtNuserMag	"userMag"
#define XtNdpiHoriz	"dpiHoriz"
#define XtNdpiVert	"dpiVert"
#define XtNdviFonts	"dviFonts"
#define XtNxFonts	"xFonts"
#define XtNtopOffset	"topOffset"
#define XtNleftOffset	"leftOffset"
#define XtNpaperWidth	"paperWidth"
#define XtNpaperHeight	"paperHeight"
#define XtNscaleUnits	"scaleUnits"
#define XtNdpsPreamble	"dpsPreamble"
#define XtNtrueDpi	"trueDpi"
#define XtNredisplay	"redisplay"
#define XtNboxColor	"boxColor"

#ifdef HAVE_GHOSTSCRIPT
#define XtNgsPreamble	"gsPreamble"
#define XtNinterpreter	"ghostscript"
#define XtNghostOutput	"ghostOutput"
#define XtNintSrchPath	"ghostSearchPath"
#endif

#ifndef XtCBackingStore
# define XtCBackingStore	"BackingStore"
#endif

#define XtCPage		"Page"
#define XtCPai		"Pai"
#define XtCUserMag	"UserMag"
#define XtCDpiHoriz	"DpiHoriz"
#define XtCDpiVert	"DpiVert"
#define XtCDviFonts	"DviFonts"
#define XtCXFonts	"XFonts"
#define XtCTopOffset	"TopOffset"
#define XtCLeftOffset	"LeftOffset"
#define XtCPaperWidth	"PaperWidth"
#define XtCPaperHeight	"PaperHeight"
#define XtCScaleUnits	"ScaleUnits"
#define XtCDPSpreamble	"DPSpreamble"
#define XtCTrueDpi	"TrueDpi"
#define XtCRedisplay	"Redisplay"
#define XtCBoxColor	"BoxColor"

#ifdef HAVE_GHOSTSCRIPT
#define XtCGSpreamble	"GSpreamble"
#define XtCInterpreter	"Interpreter"
#define XtCIntSrchPath	"GhostSearchPath"
#endif

/* for DECwidnows */

#ifndef XtCReverseVideo
#  define XtCReverseVideo               "ReverseVideo"
#endif

/* declare specific DviPageWidget class and instance datatypes */

typedef struct _DviPageClassRec*	DviPageWidgetClass;
typedef struct _DviPageRec*		DviPageWidget;

/* declare the class constant */

extern WidgetClass dviPageWidgetClass;

#endif  _DviPage_h
