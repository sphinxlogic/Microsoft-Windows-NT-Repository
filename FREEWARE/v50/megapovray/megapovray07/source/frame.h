/****************************************************************************
*                   frame.h
*
*  ------------------------------------------------------------------------
*  ATTENTION:
*  This is an unofficial version of frame.h modified by
*    Ryoichi Suzuki, rsuzuki@etl.go.jp for "isosurface".
*  ------------------------------------------------------------------------
*  This header file is included by all C modules in POV-Ray. It defines all
*  globally-accessible types and constants.
*
*  from Persistence of Vision(tm) Ray Tracer
*  Copyright 1996,1999 Persistence of Vision Team
*---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POV-Ray and to port the software to platforms other
*  than those supported by the POV-Ray Team.  There are strict rules under
*  which you are permitted to use this file.  The rules are in the file
*  named POVLEGAL.DOC which should be distributed with this file.
*  If POVLEGAL.DOC is not available or for more info please contact the POV-Ray
*  Team Coordinator by email to team-coord@povray.org or visit us on the web at
*  http://www.povray.org. The latest version of POV-Ray may be found at this site.
*
* This program is based on the popular DKB raytracer version 2.12.
* DKBTrace was originally written by David K. Buck.
* DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
*
* Modified by Andreas Dilger to add PNG file format support 05/09/95
* Modified by Edward Coffey to add coloured attenuation on 09/09/99
* Modified by Burton Radons to abstract DATA_FILE Jan. 2000
*
*****************************************************************************/

/*
 * Modification For OpenVMS By Robert Alan Byer <byer@mail.ourservers.net>
 * Feb. 13, 2001
 */

#ifndef FRAME_H
#define FRAME_H

/* Generic header for all modules */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "config.h"


#define BurnAllGifs
/*
Dear POV Community,

  As of May, 2000, GIF support has been removed from MegaPov.  Potential
patent issues have been discovered that limit the legal use of GIF files.
Although as of this writing no official stance has been taken by Unisys
(holder of the LZW compression patent) regarding GIF decompression, it is
possible that even decompression is covered by the patent and thus in the
future Unisys might possibly seek legal action against software that provides
GIF decompression capability.  In the case of POV-Ray and MegaPov, PNG files
provide better functionality than GIF (support for higher color depths, as
well as slightly smaller file size for pallette images), and the use of PNG
is not limited by patents.

Sincerely,
Nathan Kopp
*/
/* JB Patches */
#define SampleSpacingPatch
#define EmissionMethodPatch
#define NoiseTranslateFixPatch 


/*#define FastDiscPatch*/

#define SetPatch /*Chris Huff 2000.8.30 (added to mp jan 2001*/
/*#define SignPatch*/ /*Chris Huff July 2000 (added to mp jan 2001)*/
									/* not truned on because of the conflict with sign
										in the isosurface. sng and sign ???? no :) */
										
#define SplineWavePatch /*Chris Huff  jan 2001*/
#define TransformableWarpsPatch /*Chris Huff  jan 2001*/


#define UpdatedUVMappingPatch /*Mike Hough's updated uv mapping*/
#define ImagePhasePatch /* Mark Wagner's patch july 2000 */
#define AllTransmitPatch	 /* Mark Wagner's patch july 2000 */
#define NoisePigmentPatch/*Chris Huff June 2000*/
#define VWarpPatch/*Chris Huff June 2000*/
#define DisplaceWarpPatch/*Chris Huff 6/25/2000 added ability to warp with pigment*/


#define AxisRotFixPatch/*Chris Huff June 2000*/
#define GlowPatch /*Chris Huff september  2000*/

#define AtanWavePatch/*Chris Huff June 2000*/
#define df3OuputPatch

#define PostProcessKeepDataPatch	/*continue with post process YS aug 2000 */

#define HuffPostProcessPatches /* this is needed for some global stuff for the patches below */

#ifdef HuffPostProcessPatches
  /* these post process patches all (c) Chris Huff April 2000 */
/*  #define PostProcessInCameraPatch*/
  #define PostProcessCurvesPatch/*Chris Huff November 3*/
  #define PostProcessColorMatrixPatch
  #define PostProcessBlurMatrixPatch
  #define PostProcessFindEdgesPatch
  #define PostProcessInvertPatch
  #define PostProcessClipColorsPatch
  #define PostProcessAddPatch
  #define PostProcessMultiplyPatch
  #define PostProcessExponentPatch
  #define PostProcessStarsPatch
  #define PostProcessPatternBlurPatch
  #define PostProcesNormalPatch
  #define PostProcessStepsPatch
  #define PostProcessSubtractPatch/*Chris Huff June 2000*/
  #define PostProcessDividePatch
  #define PostProcessMinPatch
  #define PostProcessMaxPatch
  #define PostProcessMathPatch
	#define PostProcessRawImagePatch	/* Chris Huff jan 2001 */
	#define PostProcessLimitBrightnessPatch	/* Chris Huff jan 2001 */

#endif

#ifndef ProgressOnOneLine /*YS 28 april 2000 */
  /* this has no use here but to let everyone notice that
    by defining ProgressOnOneLine in config.h all progress
    info for post processing and photons will be output on 
    one line instead of a long list of lines.
    This define replaces the MACOS switch in MegaPov < 0.5. */
#endif

/*YS april 2000 faster arealights
  and enabling cache in lighting.c and media.c to eliminate
  calls to malloc while rendering. */
#define FastArealight
#define UseMediaAndLightCache
#define PostCartoonPatch

#define FastPolyPatch
#define FastSpherePatch
#define FastLathePatch
#define FastPlanePatch

#define DNoiseFixPatch
#define PigmentPatternPatch
#define MultiTextureCsgPatch
#define GetTicksPatch
#define TimeDatePatch
#define BlinnPatch
#define UnofficialBlocking
#define PostProcessPatch
#define FastMacroPatch
#define ComplexFunctionsPatch
#define NormalBugFix
#define Noise3DPatch
#define CubeSqrHatPatch
#define BlobPatternPatch
#define SolidPatternPatch
#define EvalPatternPatch
#define EvalPigmentPatch
#define ProximityPatch

#define InteriorTexturePatch
#define ObjectPatternPatch
#define HfHeightAtPatch
#define MotionBlurPatch
#define FontAnlignmentPatch 

#define InverseTransformsPatch
#define TransformPatch
#define VtransformPatch

#define GetImageSizePatch   
#define GetClockValuesPatch
#define BsplinePatch
#define VturbulencePatch
#define TrianglulairSquarePatch
#define NoImageNoReflectionPatch
#define ColorTrianglePatch
#define CellsPatch
#define CircularOrientAreaLightPatch 
#define PolaricalPatch
#define SplinePatch
#define TracePatch
#define PatternWarpPatch
#define PatternPatch
#define RBezierPatch
#define BoundPatch
#define CracklePatch
#define POVISO
#define IsoBlobPatch
#define IsoPigmentPatch 


/* The following patches are currently disabled.
   They may be re-included at a later date. */

/*#define VanSicklePatternPatch*/
/*#define TorodialPatch*/
/*#define ClothPatternPatch*/

#ifdef UseMediaAndLightCache
  #define DEFAULT_LIGHT_BUF_DEPTH 20
  #define DEFAULT_MEDIA_BUF_DEPTH  5
  /* uncomment AccumulateCacheStatistics to display statistics
      about the recursion depth after each rendering. 
      This should always be disabled in final compiles because it 
      slows things down again.
      Only available if UseMediaAndLightCache is defined*/
  /*#define AccumulateCacheStatistics*/
#endif

#ifdef GetTicksPatch
  #ifndef CLOCKDIVIDER /*ticks per second */
    #define CLOCKDIVIDER CLOCKS_PER_SEC /*provided by standard c */
  #endif
  #ifndef GET_TICKS
    #define GET_TICKS clock();     
  #endif
#endif


/*YS*/
/*Added these for the Mac version of MegaPov*/
/*needed for the GUI*/
#ifndef REDRAW_IMAGE_WINDOW
#define REDRAW_IMAGE_WINDOW
#endif
#ifndef ADDINCLUDEMARKER
#define ADDINCLUDEMARKER
#endif

#ifndef MAKEFILESPEC
#define MAKEFILESPEC
#endif

#ifndef CHANGEFILETYPE
#define CHANGEFILETYPE
#endif
/*YS*/
/*
 * These two are used if POV is being called from within another program
 * like a GUI interface.
 */
#ifndef MAIN_RETURN_TYPE
#define MAIN_RETURN_TYPE void
#endif

#ifndef MAIN_RETURN_STATEMENT
#define MAIN_RETURN_STATEMENT
#endif


/*
 * Functions that POV calls once per render to do various initializations,
 * in the order that they are normally called.
 */
#ifndef STARTUP_POVRAY  /* First function called in main() for each render */
#define STARTUP_POVRAY
#endif

#ifndef PRINT_CREDITS   /* Prints POV-Ray version information banner */
#define PRINT_CREDITS Print_Credits();
#endif

#ifndef PRINT_OTHER_CREDITS /* Prints credits for custom POV versions */
#define PRINT_OTHER_CREDITS
#endif

/*
 * These read the INI files.  READ_ENV_VAR reads an INI file specified by
 * (usually) the POVINI environment variable instead of the default file.
 * PROCESS_POVRAY_INI reads the INI file from the default location if
 * READ_ENV_VAR wasn't successful.  ALT_WRITE_INI_FILE writes out a new
 * INI file with the values as specified by the used for this render.
 */
#ifndef READ_ENV_VAR
#define READ_ENV_VAR Warning(0,"Environment variable not implemented on this platform.\n");
#endif

#ifndef PROCESS_POVRAY_INI
#define PROCESS_POVRAY_INI Warning(0,"Reading 'povray.ini' not implemented on this platform.\n");
#endif

#ifndef ALT_WRITE_INI_FILE
#define ALT_WRITE_INI_FILE
#endif

#ifndef FINISH_POVRAY   /* The last call that POV makes to exit */
#define FINISH_POVRAY(n) exit(n);
#endif


/*
 * Functions that POV calls once per frame to do varios (de)initializations,
 * in the order they are normally called.
 */
#ifndef POV_PRE_RENDER    /* Called just prior to the start of rendering */
#define POV_PRE_RENDER
#endif

#ifndef CONFIG_MATH       /* Macro for setting up any special FP options */
#define CONFIG_MATH
#endif

#ifndef POV_PRE_PIXEL     /* Called before each pixel is rendered */
#define POV_PRE_PIXEL(x,y,c)
#endif

#ifndef POV_POST_PIXEL    /* Called after each pixel is rendered */
#define POV_POST_PIXEL(x,y,c)
#endif

#ifndef POV_PRE_SHUTDOWN  /* Called before memory and objects are freed */
#define POV_PRE_SHUTDOWN
#endif

#ifndef POV_POST_SHUTDOWN /* Called after memory and objects are freed */
#define POV_POST_SHUTDOWN
#endif

#ifndef PRINT_STATS
#define PRINT_STATS(a) Print_Stats(a);
#endif

/* Platform-specific User Interface callbacks, allowing the UI to get status information */
#ifndef UICB_PARSE_ERROR  /* Alert UI that a parse error occurred */
#define UICB_PARSE_ERROR
#endif

#ifndef UICB_USER_ABORT  /* alert UI that user aborted the render */
#define UICB_USER_ABORT
#endif

#ifndef UICB_OPEN_INCLUDE_FILE  /* alert UI that an include file was opened during parse */
#define UICB_OPEN_INCLUDE_FILE
#endif

#ifndef UICB_CLOSE_INCLUDE_FILE  /* alert UI that an include file was closed during parse */
#define UICB_CLOSE_INCLUDE_FILE
#endif


/* Various numerical constants that are used in the calculations */
#ifndef EPSILON     /* A small value used to see if a value is nearly zero */
#define EPSILON 1.0e-10
#endif

#ifndef HUGE_VAL    /* A very large value, can be considered infinity */
#define HUGE_VAL 1.0e+17
#endif

/*
 * If the width of a bounding box in one dimension is greater than
 * the critical length, the bounding box should be set to infinite.
 */

#ifndef CRITICAL_LENGTH
#define CRITICAL_LENGTH 1.0e6
#endif

#ifndef BOUND_HUGE  /* Maximum lengths of a bounding box. */
#define BOUND_HUGE 2.0e10
#endif

/*
 * These values determine the minumum and maximum distances
 * that qualify as ray-object intersections.
 */

#define Small_Tolerance 0.001
#define Max_Distance 1.0e7


#ifndef DBL_FORMAT_STRING
#define DBL_FORMAT_STRING "%lf"
#endif

#ifndef DBL
#define DBL double
#endif

#ifndef SNGL
#define SNGL float
#endif

#ifndef COLC
#define COLC float
#endif

#ifndef M_PI
#define M_PI   3.1415926535897932384626
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef TWO_M_PI
#define TWO_M_PI 6.283185307179586476925286766560
#endif

#ifndef M_PI_180
#define M_PI_180 0.01745329251994329576
#endif

#ifndef M_PI_360
#define M_PI_360 0.00872664625997164788
#endif

/* Some implementations of scanf return 0 on failure rather than EOF */
#ifndef SCANF_EOF
#define SCANF_EOF EOF
#endif

/* Get minimum/maximum of two values. */

#ifndef min
#define min(x,y) (((x)>(y))?(y):(x))
#endif

#ifndef max
#define max(x,y) (((x)<(y))?(y):(x))
#endif

/* Get minimum/maximum of three values. */

#define max3(x,y,z) (((x)>(y))?(((x)>(z))?(x):(z)):(((y)>(z))?(y):(z)))
#define min3(x,y,z) (((x)<(y))?(((x)<(z))?(x):(z)):(((y)<(z))?(y):(z)))

#ifndef labs      /* Absolute value of the long integer x. */
#define labs(x) (long) (((x)<0)?-(x):(x))
#endif

#ifndef fabs      /* Absolute value of the double x. */
#define fabs(x) ((x) < 0.0 ? -(x) : (x))
#endif

/** poviso: R.S. July 14 '96 **/
#ifdef POVISO

#ifndef ACOS
#define ACOS acos
#endif

#ifndef SQRT
#define SQRT sqrt
#endif

#ifndef POW
#define POW pow
#endif

#ifndef COS
#define COS cos
#endif

#ifndef SIN
#define SIN sin
#endif

/** poviso: Sat 06-01-1996 0rfelyus **/

#ifndef TAN 
#define TAN tan 
#endif

#ifndef ASIN 
#define ASIN asin 
#endif

#ifndef ATAN 
#define ATAN atan 
#endif

#ifndef SINH 
#define SINH sinh 
#endif

#ifndef COSH 
#define COSH cosh 
#endif

#ifndef TANH 
#define TANH tanh 
#endif

#ifndef ASINH 
#define ASINH asinh 
#endif

#ifndef ACOSH 
#define ACOSH acosh 
#endif

#ifndef ATANH 
#define ATANH atanh 
#endif

#ifndef LOG 
#define LOG log 
#endif

#ifndef EXP 
#define EXP exp 
#endif

#ifndef ABS 
#define ABS fabs 
#endif

#ifndef FFLOOR
#define FFLOOR floor
#endif

#ifndef FCEIL 
#define FCEIL  ceil   
#endif

#endif
/** --- **/

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef CONST       /* How to define a local variable - normally 'const' */
#define CONST
#endif

#ifndef CDECL
#define CDECL
#endif

#ifndef NEW_LINE_STRING
#define NEW_LINE_STRING "\n"
#endif

/* If compiler version is undefined, then make it 'u' for unknown */
#ifndef COMPILER_VER
   /*
    * If we are on an OpenVMS machine, set the compiler version to
    * reflect this.
    */
#  if defined(VMS) && defined(DECC)
#    define COMPILER_VER " OpenVMS"
#  else
#    define COMPILER_VER ".u"
#  endif
#endif

#ifndef QSORT
#define QSORT(a,b,c,d) qsort((a),(b),(c),(d))
#endif


/*
 * POV_NAME_MAX is for file systems that have a separation of the filename
 * into name.ext.  The POV_NAME_MAX is the name part.  FILE_NAME_LENGTH
 * is the sum of name + extension.
 */
#ifndef POV_NAME_MAX
#define POV_NAME_MAX 8
#endif

#ifndef FILE_NAME_LENGTH
#define FILE_NAME_LENGTH 150
#endif

#ifndef FILENAME_SEPARATOR
#define FILENAME_SEPARATOR '/'
#endif

#ifndef DRIVE_SEPARATOR
#define DRIVE_SEPARATOR ':'
#endif

/*
 * Splits a given string into the path and file components using the
 * FILENAME_SEPARATOR and DRIVE_SEPARATOR
 */
#ifndef POV_SPLIT_PATH
#define POV_SPLIT_PATH(s,p,f) POV_Split_Path((s),(p),(f))
#endif

/* How to read, write and append to files using fopen() */
/* -- Binary -- */
#ifndef READ_BINFILE_STRING
#define READ_BINFILE_STRING "rb"
#endif

#ifndef WRITE_BINFILE_STRING
#define WRITE_BINFILE_STRING "wb"
#endif

#ifndef APPEND_BINFILE_STRING
#define APPEND_BINFILE_STRING "ab"
#endif
/* -- Text -- */
#ifndef READ_TXTFILE_STRING
#define READ_TXTFILE_STRING "r"
#endif

#ifndef WRITE_TXTFILE_STRING
#define WRITE_TXTFILE_STRING "w"
#endif

#ifndef APPEND_TXTFILE_STRING
#define APPEND_TXTFILE_STRING "a"
#endif

/* The output file format used if the user doesn't specify one */
#ifndef DEFAULT_OUTPUT_FORMAT
#define DEFAULT_OUTPUT_FORMAT   't'
#endif

/* System specific image format like BMP for Windows or PICT for Mac */
#ifndef READ_SYS_IMAGE
#define READ_SYS_IMAGE(i,n) Read_Targa_Image((i),(n))
#endif

#ifndef GET_SYS_FILE_HANDLE
#define GET_SYS_FILE_HANDLE Get_Targa_File_Handle
#endif

#ifndef SYS_DEF_EXT
#define SYS_DEF_EXT ".tga"
#endif

/* Functions to delete and rename a file */
#ifndef DELETE_FILE_ERR
#define DELETE_FILE_ERR -1
#endif

#ifndef DELETE_FILE
#define DELETE_FILE(name) unlink(name)
#endif

#ifndef RENAME_FILE_ERR
#define RENAME_FILE_ERR -1
#endif

#ifndef RENAME_FILE
#define RENAME_FILE(orig,new) rename(orig,new)
#endif

#ifndef MAX_BUFSIZE  /* The maximum size of the output file buffer */
#define MAX_BUFSIZE INT_MAX
#endif


/*
 * The TIME macros are used when displaying the rendering time for the user.
 * These are called in such a manner that STOP_TIME can be called multiple
 * times for a givn START_TIME in order to get intermediate TIME_ELAPSED
 * values.  TIME_ELAPSED is often defined as (tstop - tstart).
 */
#ifndef START_TIME
#define START_TIME time(&tstart);     
#endif

#ifndef STOP_TIME
#define STOP_TIME  time(&tstop);
#endif

#ifndef TIME_ELAPSED
#define TIME_ELAPSED difftime (tstop, tstart);
#endif

#ifndef SPLIT_TIME
#define SPLIT_TIME(d,h,m,s) POV_Std_Split_Time ((d),(h),(m),(s))
#endif


/*
 * The PRECISION_TIMER macros are used in generating histogram images on
 * systems that have very accurate timers (usually in the microsecond range).
 */
#ifndef PRECISION_TIMER_AVAILABLE
#define PRECISION_TIMER_AVAILABLE 0
#endif

#ifndef PRECISION_TIMER_INIT  /* Called once to initialize the timer */
#define PRECISION_TIMER_INIT
#endif

#ifndef PRECISION_TIMER_START
#define PRECISION_TIMER_START ;
#endif

#ifndef PRECISION_TIMER_STOP
#define PRECISION_TIMER_STOP
#endif

#ifndef PRECISION_TIMER_COUNT  /* The difference between START and STOP times */
#define PRECISION_TIMER_COUNT 0
#endif


/*
 * The COOPERATE macros are used on co-operative multi-tasking systems to
 * return control to the GUI or OS.  COOPERATE is the old form, and one
 * or both of COOPERATE_0 and COOPERATE_1 should be defined instead.
 */
#ifdef COOPERATE
#define COOPERATE_0     COOPERATE
#define COOPERATE_1     COOPERATE
#endif

#ifndef COOPERATE_0    /* Called less frequently */
#define COOPERATE_0
#endif

#ifndef COOPERATE_1    /* Called more frequently */
#define COOPERATE_1
#endif


/* How to get input from the user */
#ifndef TEST_ABORT
#define TEST_ABORT
#endif

#ifndef WAIT_FOR_KEYPRESS
#define WAIT_FOR_KEYPRESS
#else
#define WAIT_FOR_KEYPRESS_EXISTS
#endif

#ifndef GET_KEY /* Gets a keystroke from the user without waiting */
#define GET_KEY
#else
#define GET_KEY_EXISTS
#endif

/* Defines for light groups */

#ifndef MAXLIGHTGROUPS 
#define MAXLIGHTGROUPS 32
#endif

#ifndef NONE_GROUP
#define NONE_GROUP 0
#endif

#ifndef ALL_GROUP 
#define ALL_GROUP 1
#endif

/*
 * Functions that write text for the user to see.  These functions will
 * usually be customized for GUI environments so that POV outputs its
 * messages to a status bar or popup window.
 */
#ifndef POV_BANNER
#define POV_BANNER(s) POV_Std_Banner(s)
#endif

#ifndef POV_WARNING
#define POV_WARNING(s) POV_Std_Warning(s)
#endif

#ifndef POV_RENDER_INFO
#define POV_RENDER_INFO(s) POV_Std_Render_Info(s)
#endif

#ifndef POV_STATUS_INFO
#define POV_STATUS_INFO(s) POV_Std_Status_Info(s)
#endif

#ifndef POV_DEBUG_INFO
#define POV_DEBUG_INFO(s) POV_Std_Debug_Info(s)
#endif

#ifndef POV_FATAL
#define POV_FATAL(s) POV_Std_Fatal(s)
#endif

#ifndef POV_STATISTICS
#define POV_STATISTICS(s) POV_Std_Statistics(s)
#endif


/*
 * Functions that handle the graphical display preview.  These functions
 * will be customeized for all versions of POV that want to do any sort
 * of rendering preview.  The default functions will create a 80x25 text
 * "rendering" using crude ASCII graphics.
 */
#ifndef POV_DISPLAY_INIT     /* Initializes display for each frame rendered */
#define POV_DISPLAY_INIT(w,h) POV_Std_Display_Init((w),(h));
#endif

#ifndef POV_DISPLAY_FINISHED  /* Waits for user input after rendering done */
#define POV_DISPLAY_FINISHED POV_Std_Display_Finished();
#endif

#ifndef POV_DISPLAY_CLOSE     /* Closes the display window after each frame */
#define POV_DISPLAY_CLOSE POV_Std_Display_Close();
#endif

#ifndef POV_DISPLAY_PLOT      /* Plots a single pixel */
#define POV_DISPLAY_PLOT(x,y,r,g,b,a) POV_Std_Display_Plot((x),(y),(r),(g),(b),(a));
#endif

#ifndef POV_DISPLAY_PLOT_RECT  /* Plots a filled rectangle */
#define POV_DISPLAY_PLOT_RECT(x1,y1,x2,y2,r,g,b,a) POV_Std_Display_Plot_Rect((x1),(y1),(x2),(y2),(r),(g),(b),(a));
#endif

#ifndef POV_DISPLAY_PLOT_BOX   /* Plots a hollow box */
#define POV_DISPLAY_PLOT_BOX(x1,y1,x2,y2,r,g,b,a) POV_Std_Display_Plot_Box((x1),(y1),(x2),(y2),(r),(g),(b),(a));
#endif

#ifndef POV_GET_FULL_PATH      /* returns full pathspec */
#define POV_GET_FULL_PATH(f,p,b) if (b) strcpy(b,p);
#endif

#ifndef POV_WRITE_LINE         /* write the current line to something */
#define POV_WRITE_LINE(line,y)
#endif

#ifndef POV_ASSIGN_PIXEL       /* assign the colour of a pixel */
#define POV_ASSIGN_PIXEL(x,y,colour)
#endif

/* The next two are palette modes, for normal and grayscale display */
#ifndef NORMAL
#define NORMAL '0'
#endif

#ifndef GREY
#define GREY   'G'
#endif

/*
 * The DEFAULT_DISPLAY_GAMMA is used when there isn't one specified by the
 * user in the POVRAY.INI.  For those systems that are very savvy, this
 * could be a function which returns the current display gamma.  The
 * DEFAULT_ASSUMED_GAMMA should be left alone.
 */
#ifndef DEFAULT_DISPLAY_GAMMA
#define DEFAULT_DISPLAY_GAMMA 2.2
#endif

#ifndef DEFAULT_ASSUMED_GAMMA
#define DEFAULT_ASSUMED_GAMMA 1.0
#endif


/*****************************************************************************
 *
 * MEMIO.C Memory macros
 *
 *****************************************************************************/

#ifndef __FILE__
#define __FILE__ ""
#endif

#ifndef __LINE__
#define __LINE__ (-1)
#endif

/*
 * These functions define macros which do checking for memory allocation,
 * and can also do other things.  Check mem.c before you change them, since
 * they aren't simply replacements for malloc, calloc, realloc, and free.
 */
#ifndef POV_MALLOC
#define POV_MALLOC(size,msg)        pov_malloc ((size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_CALLOC
#define POV_CALLOC(nitems,size,msg) pov_calloc ((nitems), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_REALLOC
#define POV_REALLOC(ptr,size,msg)   pov_realloc ((ptr), (size), __FILE__, __LINE__, (msg))
#endif

#ifndef POV_FREE
#define POV_FREE(ptr)               pov_free ((void *)(ptr), __FILE__, __LINE__)
#endif

#ifndef POV_MEM_INIT
#define POV_MEM_INIT()              mem_init()
#endif

#ifndef POV_MEM_RELEASE_ALL
#define POV_MEM_RELEASE_ALL(log)    mem_release_all(log)
#endif

#ifndef POV_STRDUP
#define POV_STRDUP(str)             pov_strdup(str)
#endif

/* For those systems that don't have memmove, this can also be pov_memmove */
#ifndef POV_MEMMOVE
#define POV_MEMMOVE(dst,src,len)    memmove((dst),(src),(len))
#endif


/*
 * Functions which invoke external programs to do work for POV, generally
 * at the request of the user.
 */
#ifndef POV_SHELLOUT
#define POV_SHELLOUT(string) pov_shellout(string)
#endif

#ifndef POV_MAX_CMD_LENGTH
#define POV_MAX_CMD_LENGTH 250
#endif

#ifndef POV_SYSTEM
#define POV_SYSTEM(string) system(string)
#endif


/*****************************************************************************
 *
 * Typedefs that need to be known here.
 *
 *****************************************************************************/

typedef struct Object_Struct OBJECT;
typedef struct Ray_Struct RAY;
typedef struct istack_struct ISTACK;
typedef struct istk_entry INTERSECTION;



/*****************************************************************************
 *
 * Scalar, color and vector stuff.
 *
 *****************************************************************************/

typedef DBL UV_VECT [2];
typedef DBL VECTOR [3];
typedef DBL VECTOR_4D [4];
typedef DBL MATRIX [4][4];
typedef DBL EXPRESS [5];
typedef COLC COLOUR [5];
typedef COLC RGB [3];
typedef short WORD;

/* Stuff for bounding boxes. */

#define BBOX_VAL SNGL

typedef BBOX_VAL BBOX_VECT[3];

#define Assign_BBox_Vect(d,s) \
{ \
  (d)[X] = (s)[X]; \
  (d)[Y] = (s)[Y]; \
  (d)[Z] = (s)[Z]; \
}

#define Make_BBox(BBox, llx, lly, llz, lex, ley, lez) \
{ \
  (BBox).Lower_Left[X] = (BBOX_VAL)(llx); \
  (BBox).Lower_Left[Y] = (BBOX_VAL)(lly); \
  (BBox).Lower_Left[Z] = (BBOX_VAL)(llz); \
  (BBox).Lengths[X] = (BBOX_VAL)(lex); \
  (BBox).Lengths[Y] = (BBOX_VAL)(ley); \
  (BBox).Lengths[Z] = (BBOX_VAL)(lez); \
}

#define Make_BBox_from_min_max(BBox, mins, maxs) \
{ \
  (BBox).Lower_Left[X] = (BBOX_VAL)(mins[X]); \
  (BBox).Lower_Left[Y] = (BBOX_VAL)(mins[Y]); \
  (BBox).Lower_Left[Z] = (BBOX_VAL)(mins[Z]); \
  (BBox).Lengths[X] = (BBOX_VAL)(maxs[X]-mins[X]); \
  (BBox).Lengths[Y] = (BBOX_VAL)(maxs[Y]-mins[Y]); \
  (BBox).Lengths[Z] = (BBOX_VAL)(maxs[Z]-mins[Z]); \
}

#define Make_min_max_from_BBox(mins, maxs, BBox) \
{ \
  (mins)[X] = (BBox).Lower_Left[X]; \
  (mins)[Y] = (BBox).Lower_Left[Y]; \
  (mins)[Z] = (BBox).Lower_Left[Z]; \
  (maxs)[X] = (mins)[X] + (BBox).Lengths[X]; \
  (maxs)[Y] = (mins)[Y] + (BBox).Lengths[Y]; \
  (maxs)[Z] = (mins)[Z] + (BBox).Lengths[Z]; \
}

/* Stuff for SNGL vectors. */

typedef SNGL SNGL_VECT[3];

#define Assign_SNGL_Vect(d,s) \
{ \
  (d)[X] = (SNGL)((s)[X]); \
  (d)[Y] = (SNGL)((s)[Y]); \
  (d)[Z] = (SNGL)((s)[Z]); \
}


/* Vector array elements. */
#define U 0
#define V 1

#define X 0
#define Y 1
#define Z 2
#define T 3


/* Colour array elements. */

#define RED    0
#define GREEN  1
#define BLUE   2
#define FILTER 3
#define TRANSM 4

/* Macros to manipulate scalars, vectors, and colors. */

#define Destroy_Float(x)    if ((x)!=NULL) POV_FREE(x)

#define Assign_Vector(d,s)  memcpy((d),(s),sizeof(VECTOR))
#define Destroy_Vector(x)   if ((x)!=NULL) POV_FREE(x)

#define Assign_UV_Vect(d,s) memcpy((d),(s),sizeof(UV_VECT))
#define Destroy_UV_Vect(x)  if ((x)!=NULL) POV_FREE(x)

#define Assign_Vector_4D(d,s) memcpy((d),(s),sizeof(VECTOR_4D))
#define Destroy_Vector_4D(x)  if ((x)!=NULL) POV_FREE(x)

#define Assign_Colour(d,s)  memcpy((d),(s),sizeof(COLOUR))
/* NK fix assign_colour */
#define Assign_Colour_Express(d,s)  {(d)[RED] = (s)[RED]; (d)[GREEN] = (s)[GREEN]; (d)[BLUE] = (s)[BLUE]; (d)[FILTER] = (s)[FILTER]; (d)[TRANSM] = (s)[TRANSM];}
/* NK ---- */
#define Make_Colour(c,r,g,b) {(c)[RED]=(r);(c)[GREEN]=(g);(c)[BLUE]=(b);(c)[FILTER]=0.0;(c)[TRANSM]=0.0;}
#define Make_ColourA(c,r,g,b,a,t) {(c)[RED]=(r);(c)[GREEN]=(g);(c)[BLUE]=(b);(c)[FILTER]=(a);(c)[TRANSM]=t;}
#define Make_Vector(v,a,b,c) { (v)[X]=(a);(v)[Y]=(b);(v)[Z]=(c); }
#define Destroy_Colour(x) if ((x)!=NULL) POV_FREE(x)
#define Make_RGB(c,r,g,b) {(c)[RED]=(r);(c)[GREEN]=(g);(c)[BLUE]=(b);}



/*****************************************************************************
 *
 * Hi-resolution counter.
 *
 *****************************************************************************/

/* Define counter resolution. */

#define LOW_RESOLUTION  1
#define HIGH_RESOLUTION 2

#ifndef COUNTER_RESOLUTION
#define COUNTER_RESOLUTION HIGH_RESOLUTION
#endif

#if COUNTER_RESOLUTION == HIGH_RESOLUTION

/* 64bit counter. */

typedef struct Counter_Struct COUNTER;

struct Counter_Struct
{
  unsigned long high, low;
};

#define DBL_Counter(x)     ( (x).low + Sqr(65536.0)*(DBL)(x).high )
#define Long_To_Counter(i,x) { (x).low = i; (x).high = 0; }
#define Init_Counter(x)     { (x).high = (x).low = 0L; }
#define Test_Zero_Counter(x) (((x).low == 0L) && ((x).high == 0L))
#define Increase_Counter(x) { if ((++(x).low) == 0L) { (x).high++; } }
#define Add_Counter(x, a, b) \
{ \
  (x).low = (a).low + (b).low; \
  \
  if (((x).low < (a).low) || ((x).low < (b).low)) \
  { \
    /* add with carry */ \
    (x).high = (a).high + (b).high + 1; \
  } \
  else \
  { \
    /* add without carry */ \
    (x).high = (a).high + (b).high; \
  } \
}

#else

/* 32bit counter. */

typedef unsigned long COUNTER;

#define DBL_Counter(x)     ( (DBL)(x) )
#define Long_To_Counter(i,x) { (x) = i; }
#define Init_Counter(x)      { (x) = 0L; }
#define Increase_Counter(x)  { (x)++; }
#define Test_Zero_Counter(x) ((x) == 0L)
#define Add_Counter(x, a, b) { (x) = (a) + (b); }

#endif



/*****************************************************************************
 *
 * Bounding box stuff (see also BOUND.H).
 *
 *****************************************************************************/

typedef struct Bounding_Box_Struct BBOX;

struct Bounding_Box_Struct
{
  BBOX_VECT Lower_Left, Lengths;
};



/*****************************************************************************
 *
 * Transformation stuff.
 *
 *****************************************************************************/

typedef struct Transform_Struct TRANSFORM;

struct Transform_Struct
{
  MATRIX matrix;
  MATRIX inverse;
};

#define Destroy_Transform(x) if ((x)!=NULL) POV_FREE(x)



/*****************************************************************************
 *
 * Spline stuff.
 *
 *****************************************************************************/
#ifdef SplinePatch
typedef struct Spline_Entry2 SPLINE_ENTRY;
typedef struct Spline_Struct2 SPLINE2;

struct Spline_Entry2
{
    DBL par; 
    DBL vec[5];
};

struct Spline_Struct2
{
    short Number_Of_Entries;
    short Type;
    SPLINE_ENTRY * SplineEntries;
};
#endif


/*****************************************************************************
 *
 * Color map stuff.
 *
 *****************************************************************************/

#define MAX_BLEND_MAP_ENTRIES 256

typedef struct Blend_Map_Entry BLEND_MAP_ENTRY;
typedef struct Blend_Map_Struct BLEND_MAP;
typedef struct Pattern_Struct TPATTERN;
typedef struct Texture_Struct TEXTURE;
typedef struct Pigment_Struct PIGMENT;
typedef struct Tnormal_Struct TNORMAL;
typedef struct Finish_Struct FINISH;
typedef struct Turb_Struct TURB;
typedef struct Warps_Struct WARP;
struct Blend_Map_Entry
{
  SNGL value;
  unsigned char Same;
  union
  {
   COLOUR Colour;
   PIGMENT *Pigment;
   TNORMAL *Tnormal;
   TEXTURE *Texture;
   UV_VECT Point_Slope;
  } Vals;
};

struct Blend_Map_Struct
{
  short Number_Of_Entries, Transparency_Flag, Type;
  long  Users;
  BLEND_MAP_ENTRY *Blend_Map_Entries;
};

#define Make_Blend_Map_Entry(entry,v,s,r,g,b,a,t) \
{ \
  (entry).value = (v); \
  (entry).Same = (s); \
  Make_ColourA((entry).Vals.Colour, r, g, b, a, t); \
}



/*****************************************************************************
 *
 * Media stuff.
 *
 *****************************************************************************/

typedef struct Media_Struct IMEDIA;

struct Media_Struct
{
  int Type;
  int Intervals;
  int Min_Samples;
  int Max_Samples;
  int Sample_Method; /* MH */
  DBL Jitter; /* MH */
  #ifdef SampleSpacingPatch
    DBL Sample_Spacing, Sample_Jitter; /* JB */
  #endif
  int is_constant;
  DBL Eccentricity,sc_ext;
  int use_absorption;
  int use_emission;
  int use_extinction;
  int use_scattering;
  COLOUR Absorption;
  COLOUR Emission;
  COLOUR Extinction;
  COLOUR Scattering;
  unsigned char Light_Group;
  int Invert_Light_Group;
  #ifdef  EmissionMethodPatch
    int Emission_Method;		/* JB */
    DBL Emission_Extinction;	/* JB */
  #endif
  DBL Ratio;
  DBL Confidence;
  DBL Variance;
  DBL *Sample_Threshold;

  DBL AA_Threshold;
  int AA_Level;
  int ignore_photons;

  PIGMENT *Density;


  IMEDIA *Next_Media;
};



/*****************************************************************************
 *
 * Interior stuff.
 *
 *****************************************************************************/

typedef struct Interior_Struct INTERIOR;

struct Interior_Struct
{
  long References;
  int  hollow;
  SNGL IOR, /*DSW*/Dispersion/*---*/;
  /*DSW*/
  int Disp_NElems;
  /*---*/
  SNGL Caustics, Old_Refract;
  SNGL Fade_Distance, Fade_Power;
  COLOUR Fade_Colour;
  IMEDIA *IMedia;
};


/*****************************************************************************
 *
 * IFF file stuff.
 *
 *****************************************************************************/

#ifndef IFF_SWITCH_CAST
#define IFF_SWITCH_CAST (int)
#endif

typedef struct Image_Colour_Struct IMAGE_COLOUR;

typedef struct Image_Line_Struct IMAGE_LINE;

struct Image_Colour_Struct
{
  unsigned short Red, Green, Blue, Filter, Transmit;
};

struct Image_Line_Struct
{
  unsigned char *red, *green, *blue, *transm;
};



/*****************************************************************************
 *
 * Image stuff.
 *
 *****************************************************************************/

/* Legal image attributes. */

#define NO_FILE         0x0000
#define GIF_FILE        0x0001
#define POT_FILE        0x0002
#define SYS_FILE        0x0004
#define IFF_FILE        0x0008
#define TGA_FILE        0x0010
#define GRAD_FILE       0x0020
#define PGM_FILE        0x0040
#define PPM_FILE        0x0080
#define PNG_FILE        0x0100

#define IMAGE_FTYPE     0x0400
#define HF_FTYPE        0x0800
#define HIST_FTYPE      0x1000
#define GRAY_FTYPE      0x2000
#define NORMAL_FTYPE    0x4000
#define MATERIAL_FTYPE  0x8000

/* Image types. */

#define IMAGE_FILE    IMAGE_FTYPE+GIF_FILE+SYS_FILE+IFF_FILE+GRAD_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE
#define NORMAL_FILE   NORMAL_FTYPE+GIF_FILE+SYS_FILE+IFF_FILE+GRAD_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE
#define MATERIAL_FILE MATERIAL_FTYPE+GIF_FILE+SYS_FILE+IFF_FILE+GRAD_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE
#define HF_FILE       HF_FTYPE+GIF_FILE+POT_FILE+TGA_FILE+PGM_FILE+PPM_FILE+PNG_FILE

typedef struct Image_Struct IMAGE;

struct Image_Struct
{
  int References; /* Keeps track of number of pointers to this structure */
  int Map_Type;
  int File_Type;
  int Image_Type; /* What this image is being used for */
  int Interpolation_Type;
  short Once_Flag;
  short Use_Colour_Flag;
  VECTOR Gradient;
  SNGL width, height;
  int iwidth, iheight;
  short Colour_Map_Size;
#ifdef AllTransmitPatch 
  DBL AllFilter, AllTransmit; 
#endif
  IMAGE_COLOUR *Colour_Map;
  union
  {
    IMAGE_LINE *rgb_lines;
    unsigned char **map_lines;
  } data;
  /** poviso: Fri 09-27-1996 0. **/
#ifdef POVISO
   void *Object;
#endif
  /** --- **/

};

#define PIGMENT_TYPE  0
#define NORMAL_TYPE   1
#define PATTERN_TYPE  2
#define TEXTURE_TYPE  4
#define COLOUR_TYPE   5
#define SLOPE_TYPE    6
#define DENSITY_TYPE  7

#define DEFAULT_FRACTAL_EXTERIOR_TYPE 1
#define DEFAULT_FRACTAL_INTERIOR_TYPE 0
#define DEFAULT_FRACTAL_EXTERIOR_FACTOR 1
#define DEFAULT_FRACTAL_INTERIOR_FACTOR 1


/*****************************************************************************
 *
 * Pigment, Tnormal, Finish, Texture & Warps stuff.
 *
 *****************************************************************************/

typedef struct Density_file_Struct DENSITY_FILE;
typedef struct Density_file_Data_Struct DENSITY_FILE_DATA;

struct Density_file_Struct
{
  int Interpolation;
  DENSITY_FILE_DATA *Data;
};

struct Density_file_Data_Struct
{
  int References;
  char *Name;
  int Sx, Sy, Sz;
  unsigned char ***Density;
};

#ifdef BlobPatternPatch
/*START Chris Huff blob pattern*/

typedef struct Blob_Pattern_Data_Struct BLOB_PATTERN_DATA;
struct Blob_Pattern_Data_Struct
{
  VECTOR center;/*Also point A if component is cylinderical or box*/
  VECTOR pointB;/*point B if component is cylinderical or box*/
  DBL radius;
  DBL strength;
  DBL falloff;
  PIGMENT * pigment;
  TRANSFORM * transform;
  void *blob;
/*  TPATTERN * pattern;*/
  int inverse;
  int type;/*0=sphere, 1=cylinder, 2=box, 3=pattern, 4=blob. Change to enum?*/
  int function;/*The density function. 0 for standard blob. Change to enum?*/
  BLOB_PATTERN_DATA * next;
};

#define BlobPatternDef struct{    \
    BLOB_PATTERN_DATA * blob_dat; \
    DBL blob_threshold;           \
    DBL max_density;              \
  }Blob;

/*END Chris Huff blob pattern*/
#else
  #define BlobPatternDef
#endif

#ifdef CracklePatch 
  #define CrackleDef   struct {DBL Size, UseCoords; VECTOR Metric; } Facets; \
    struct {VECTOR Form; VECTOR Metric; DBL Offset; DBL Dim; short IsSolid; VECTOR *cv; long lastseed; VECTOR lastcenter;}  Crackle; 
#else
  #define CrackleDef
#endif

#ifdef POVISO
  #define IsoDef void *Function;
#else
  #define IsoDef
#endif

#ifdef ProximityPatch
  #define ProximityDef struct{ \
    int samples;               \
    int sample_bailout;        \
    VECTOR sampleWeight;       \
    unsigned int sides:2;         /* 0,1,2 */     \
    unsigned int sampleMthd:2;    /* 0,1,2 */     \
    unsigned int proxCalcMthd:2;  /* 0,1,2,3 */   \
    DBL max_density;           \
    DBL proxDist;              \
    OBJECT *proxObject;        \
    DBL falloff;               \
  } Proximity;
#else
  #define ProximityDef
#endif

#ifdef SolidPatternPatch
  #define SolidPatternDef  SNGL SolidVal;
#else
  #define SolidPatternDef 
#endif

#if defined(ObjectPatternPatch)
  #define ObjectPatternDef OBJECT *Object;
#else
  #define ObjectPatternDef 
#endif

#ifdef PigmentPatternPatch
#define PigmentPatternDef PIGMENT* Pigment;
#else
  #define PigmentPatternDef
#endif

#ifdef SplineWavePatch
    #define SplineWaveDef SPLINE2 * spline_wave;
#else
    #define SplineWaveDef
#endif

#define TPATTERN_FIELDS       \
 unsigned short Type, Wave_Type, Flags; \
  int References;             \
  SNGL Frequency, Phase;      \
  SNGL Exponent;              \
  WARP *Warps;                \
  SplineWaveDef\
  TPATTERN *Next;             \
  BLEND_MAP *Blend_Map;       \
  union {                     \
   struct {int NoiseType; COLOUR Min; COLOUR Max;} NoisePigment;             \
   DENSITY_FILE *Density_File; \
   IMAGE *Image;              \
   VECTOR Gradient;           \
   /* -hdf- and MA Apr/May 98 */                   \
   struct {                                     \
      VECTOR Slope_Vector, Altit_Vector;      \
      short Slope_Base, Altit_Base;           \
      SNGL Slope_Len, Altit_Len;              \
      UV_VECT Slope_Mod, Altit_Mod;           \
   } Slope;                                     \
   SNGL Agate_Turb_Scale;     \
   short Num_of_Waves;        \
   short Iterations;          \
   short Arms;                \
   struct {SNGL Mortar; VECTOR Size;} Brick; \
   struct {SNGL Control0, Control1; } Quilted;   \
   struct {UV_VECT Coord; short Iterations, interior_type, exterior_type; \
           DBL efactor, ifactor;} Fractal; \
   /* NEW PATCH OPTIONS */           \
   SolidPatternDef \
   IsoDef \
   CrackleDef \
   ObjectPatternDef \
   ProximityDef \
   BlobPatternDef \
   PigmentPatternDef \
  } Vals;

struct Pattern_Struct
{
  TPATTERN_FIELDS
};

struct Pigment_Struct
{
  TPATTERN_FIELDS
  COLOUR Colour; 
};

struct Tnormal_Struct
{
  TPATTERN_FIELDS
  SNGL Amount;
  SNGL Delta; /* NK delta */
};

#define TEXTURE_FIELDS \
  TPATTERN_FIELDS      \
  TEXTURE *Next_Material;

struct Texture_Struct
{
  TEXTURE_FIELDS
  PIGMENT *Pigment;
  TNORMAL *Tnormal;
  FINISH *Finish;
  TEXTURE *Materials;
  int Num_Of_Mats;

};

struct Finish_Struct
{
  SNGL Diffuse, Brilliance;
  SNGL Specular, Roughness;
  SNGL Phong, Phong_Size;
  SNGL Irid, Irid_Film_Thickness, Irid_Turb;
  SNGL Temp_Caustics, Temp_IOR, /*DSW*/Temp_Dispersion,/*---*/ Temp_Refract, Reflect_Exp;
  /*DSW*/
  int Temp_NElems;
  /*---*/
  SNGL Crand, Metallic;
  RGB Ambient, Reflection_Max, Reflection_Min;  /* Changed by MBP 8/27/98 */
  SNGL Reflection_Falloff;  /* Added by MBP 8/27/98 */
  SNGL Reflection_Blur;  /* Added by MBP  6/27/98 */
  int Reflection_Samples;  /* Added by MBP  6/27/98 */
  int Reflection_Type;  /* Added by MBP 9/5/98 */
  SNGL Reflect_Metallic; /* MBP */
  int Conserve_Energy;  /* added by NK Dec 19 1999 */
#ifdef BlinnPatch
  SNGL Blinn, Facets;
#endif
};

#ifdef TransformableWarpsPatch
#define WARP_FIELDS \
    unsigned short Warp_Type; \
    WARP *Next_Warp; \
    TRANSFORM * Warp_Transform;
#else
#define WARP_FIELDS unsigned short Warp_Type; WARP *Next_Warp;
#endif

struct Warps_Struct
{
  WARP_FIELDS
};

struct Turb_Struct
{
  WARP_FIELDS
  VECTOR Turbulence;
  int Octaves;
  SNGL Lambda, Omega;
};

#define Destroy_Finish(x) if ((x)!=NULL) POV_FREE(x)

typedef struct Material_Struct MATERIAL;

struct Material_Struct
{
   TEXTURE *Texture;
#ifdef InteriorTexturePatch
   TEXTURE * Interior_Texture;/*Chris Huff: Interior Texture patch*/
#endif
   INTERIOR *Interior;
};




/*****************************************************************************
 *
 * Object stuff (see also OBJECTS.H and primitive include files).
 *
 *****************************************************************************/

#define All_Intersections(x,y,z) ((*((x)->Methods->All_Intersections_Method)) (x,y,z))
#define Inside(x,y) ((*((y)->Methods->Inside_Method)) (x,y))
#define Normal(x,y,z) ((*((y)->Methods->Normal_Method)) (x,y,z))
/* NK 1998 */
#define UVCoord(x,y,z) ((*((y)->Methods->UVCoord_Method)) (x,y,z))
/* NK ---- */
#define Copy(x) ((*((x)->Methods->Copy_Method)) (x))
#define Translate(x,y,z) ((*((x)->Methods->Translate_Method)) (x,y,z))
#define Scale(x,y,z) ((*((x)->Methods->Scale_Method)) (x,y,z))
#define Rotate(x,y,z) ((*((x)->Methods->Rotate_Method)) (x,y,z))
#define Transform(x,y) ((*((x)->Methods->Transform_Method)) (x,y))
#define Invert(x) ((*((x)->Methods->Invert_Method)) (x))
#define Destroy(x) ((*((x)->Methods->Destroy_Method)) (x))

typedef struct Method_Struct METHODS;

typedef int (*ALL_INTERSECTIONS_METHOD)(OBJECT *, RAY *, ISTACK *);
typedef int (*INSIDE_METHOD)(VECTOR , OBJECT *);
typedef void (*NORMAL_METHOD)(VECTOR, OBJECT *, INTERSECTION *);
/* NK 1998 */
typedef void (*UVCOORD_METHOD)(UV_VECT, OBJECT *, INTERSECTION *);
/* NK ---- */
typedef void *(*COPY_METHOD)(OBJECT *);
typedef void (*TRANSLATE_METHOD)(OBJECT *, VECTOR, TRANSFORM *);
typedef void (*ROTATE_METHOD)(OBJECT *, VECTOR, TRANSFORM *);
typedef void (*SCALE_METHOD)(OBJECT *, VECTOR, TRANSFORM *);
typedef void (*TRANSFORM_METHOD)(OBJECT *, TRANSFORM *);
typedef void (*INVERT_METHOD)(OBJECT *);
typedef void (*DESTROY_METHOD)(OBJECT *);

/* These fields are common to all objects. */

#ifdef MotionBlurPatch
#define TimeStampDef int TimeStamp;
#define TimeStampInit(o)  o->TimeStamp = 0;
#else
#define TimeStampDef
#define TimeStampInit(o)
#endif

#ifdef InteriorTexturePatch
#define InteriorTexDef   TEXTURE *Interior_Texture;/*Chris Huff: Interior Texture patch*/
#define InteriorTexInit(o)  o->Interior_Texture  = NULL;
#else
#define InteriorTexDef
#define InteriorTexInit(o)
#endif

#ifdef GlowPatch
typedef struct Glow_Struct GLOW;
typedef GLOW * GLOW_PTR;

#define GlowPatchDef unsigned int numOfGlows; GLOW_PTR * glowList;/*a dynamically allocated array of glow pointers*/
#define GlowPatchInit(o) o->numOfGlows = 0; o->glowList = NULL;
#else
#define GlowPatchDef
#define GlowPatchInit(o)
#endif

#define OBJECT_FIELDS   \
  METHODS *Methods;     \
  int Type;             \
  OBJECT *Sibling;      \
  TEXTURE *Texture;     \
  INTERIOR *Interior;   \
  OBJECT *Bound;        \
  OBJECT *Clip;         \
  BBOX BBox;            \
  TRANSFORM *UV_Trans;    /* NK 1998 */ \
  unsigned char Ph_Flags; /* NK phmap */\
  SNGL Ph_Density;        /* NK phmap */\
  char *Label;            /* NK persist */\
  unsigned char No_Shadow_Group, Light_Group; \
  unsigned long Flags;    \
  TimeStampDef            \
  InteriorTexDef          \
  GlowPatchDef\
  /* End of OBJECT_FIELDS */


/* These fields are common to all compound objects */

#define COMPOUND_FIELDS \
  OBJECT_FIELDS          \
  OBJECT *Children;

#define INIT_OBJECT_FIELDS(o,t,m) \
  o->Type     = t;                \
  o->Methods  = m;                \
  o->Sibling  = NULL;             \
  o->Texture  = NULL;             \
  o->Bound    = NULL;             \
  o->Clip     = NULL;             \
  o->Interior = NULL;             \
  o->UV_Trans = NULL;             /* NK 1998 */ \
  o->Ph_Flags = 0;                /* NK phmap */\
  o->Ph_Density = 0;              /* NK phmap */\
  o->Label    = NULL;             /* NK persist */\
  o->Flags    = 0;                \
  o->No_Shadow_Group = NONE_GROUP;          \
  o->Light_Group = NONE_GROUP;              \
  Make_BBox(o->BBox, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, -BOUND_HUGE/2.0, \
    BOUND_HUGE, BOUND_HUGE, BOUND_HUGE);    \
  InteriorTexInit(o) \
  TimeStampInit(o)  \
  GlowPatchInit(o)  \
  /* End of INIT_OBJECT_FIELDS */

struct Method_Struct
{
  ALL_INTERSECTIONS_METHOD All_Intersections_Method;
  INSIDE_METHOD Inside_Method;
  NORMAL_METHOD Normal_Method;
  UVCOORD_METHOD UVCoord_Method; /* NK 1998 */
  COPY_METHOD Copy_Method;
  TRANSLATE_METHOD Translate_Method;
  ROTATE_METHOD Rotate_Method;
  SCALE_METHOD Scale_Method;
  TRANSFORM_METHOD Transform_Method;
  INVERT_METHOD Invert_Method;
  DESTROY_METHOD Destroy_Method;
};

/* This is an abstract structure that is never actually used.
   All other objects are descendents of this primative type */

struct Object_Struct
{
  OBJECT_FIELDS
};



/*****************************************************************************
 *
 * Intersection stack stuff.
 *
 *****************************************************************************/

struct istk_entry
{
   DBL Depth;
   VECTOR IPoint;
   VECTOR INormal;
   VECTOR PNormal; /* perturbed normal vector; -hdf- June 98 */
   OBJECT *Object;
   UV_VECT Iuv; /* NK 1998 */
/*
 *  [DB 8/94]
 *
 * Pass additional values from the intersection function to other functions
 * (normal calculation). 2 ints and 2 DBLS seem enough.
 */
   int i1, i2;
   DBL d1, d2;
#ifdef RBezierPatch
    /*  [0rf 7/97]
  *   No, it does not seem enough.
  *   Yes, it does.
  *   No, it does not.
  *   Yes, it does.
  *   No, it does not.
  *   .
  *   .
  *   .   [like Monty Python's Flying Circus ]
  */  

  DBL u,v;
  DBL d3,d4;
  DBL w1,w2,w3;
#endif
 /* Arbitrary pointer that can be passed. */
   void *Pointer;
};

struct istack_struct
{
   struct istack_struct *next;
   struct istk_entry *istack;
   unsigned int top_entry;
};

#define itop(i) (i->istack[i->top_entry])

/* Macros to push intersection onto stack. */

#define push_entry(d,v,o,i)                  \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_normal_entry(d,v,n,o,i)         \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  Assign_Vector(itop(i).IPoint,v);           \
  Assign_Vector(itop(i).INormal,n);          \
  Assign_Vector(itop(i).PNormal,n);     /* init perturbed normal vector; -hdf- June 98 */       \
  incstack(i);

/* NK 1998 */
#define push_uv_entry(d,v,uv,o,i)                  \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  Assign_Vector(itop(i).IPoint,v);           \
  Assign_UV_Vect(itop(i).Iuv,uv);             \
  incstack(i);

#define push_normal_uv_entry(d,v,n,uv,o,i)         \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  Assign_Vector(itop(i).IPoint,v);           \
  Assign_Vector(itop(i).INormal,n);          \
  Assign_UV_Vect(itop(i).Iuv,uv);             \
  incstack(i);
/* NK ---- */

/* Use these macros to push additional parameters onto the stack. [DB 8/94] */

#define push_entry_pointer(d,v,o,a,i)        \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  itop(i).Pointer = (void *)(a);             \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_entry_i1(d,v,o,a,i)             \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  itop(i).i1 = a;                            \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_entry_d1(d,v,o,a,i)             \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  itop(i).d1 = a;                            \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_entry_i1_i2(d,v,o,a,b,i)        \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  itop(i).i1 = a;                            \
  itop(i).i2 = b;                            \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_entry_i1_d1(d,v,o,a,b,i)        \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  itop(i).i1 = a;                            \
  itop(i).d1 = b;                            \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_entry_i1_i2_d1(d,v,o,a,b,c,i)   \
  itop(i).Depth  = d;                        \
  itop(i).Object = o;                        \
  itop(i).i1 = a;                            \
  itop(i).i2 = b;                            \
  itop(i).d1 = c;                            \
  Assign_Vector(itop(i).IPoint,v);           \
  incstack(i);

#define push_copy(i,e) itop(i)= *e; incstack(i);
#define pop_entry(i) (i->top_entry > 0)?&(i->istack[--i->top_entry]):NULL



/*****************************************************************************
 *
 * Ray stuff (see also RAY.H).
 *
 *****************************************************************************/

#define MAX_CONTAINING_OBJECTS 100

struct Ray_Struct
{
  VECTOR Initial;
  VECTOR Direction;
  int Index;
  INTERIOR *Interiors[MAX_CONTAINING_OBJECTS];
};


/*****************************************************************************
 *
 * Frame tracking information
 *
 *****************************************************************************/

typedef enum
{
  FT_SINGLE_FRAME,
  FT_MULTIPLE_FRAME
} FRAMETYPE;

#define INT_VALUE_UNSET (-1)
#define DBL_VALUE_UNSET (-1.0)

typedef struct
{
  FRAMETYPE FrameType;
  DBL Clock_Value;      /* May change between frames of an animation */
  int FrameNumber;      /* May change between frames of an animation */

  int InitialFrame;
#ifdef df3OuputPatch
  int ActualInitialFrame;
  int ActualFinalFrame;
#endif
  DBL InitialClock;

  int FinalFrame;
  int FrameNumWidth;
  DBL FinalClock;

  int SubsetStartFrame;
  DBL SubsetStartPercent;
  int SubsetEndFrame;
  DBL SubsetEndPercent;
  
  unsigned Field_Render_Flag;
  unsigned Odd_Field_Flag;
} FRAMESEQ;


/*****************************************************************************
 *
 * Miscellaneous stuff.
 *
 *****************************************************************************/

typedef struct Chunk_Header_Struct CHUNK_HEADER;
typedef struct Data_File_Struct DATA_FILE;
typedef struct complex_block complex;
typedef struct file_handle_struct FILE_HANDLE;
typedef int TOKEN;
typedef struct Reserved_Word_Struct RESERVED_WORD;

struct Reserved_Word_Struct
{
  TOKEN Token_Number;
  char *Token_Name;
};

typedef struct Sym_Table_Entry SYM_ENTRY;

struct Sym_Table_Entry 
{
  SYM_ENTRY *next;
  char *Token_Name;
  void *Data;
  TOKEN Token_Number;
};

struct Chunk_Header_Struct
{
  long name;
  long size;
};

struct Data_File_Struct
{
#ifdef FastMacroPatch
  /* Rewritten by Burton Radons, Jan 2000 */
  void *Data; /* For the procedure's work */
  void (*Close) (DATA_FILE *); /* Close the file, as you will */
  int (*Getc) (DATA_FILE *); /* Return a single read character or -1 for EOF */
  void (*Ungetc) (DATA_FILE *, int ch); /* Stuff a character back into the file */
  void (*Putc) (DATA_FILE *, int ch); /* Place a character in stream */
  int (*Tell) (DATA_FILE *);
  int (*Seek) (DATA_FILE *, int pos);
  
  int Line_Number; /* Updating is the procedure concern */
  int R_Flag;
  char *Filename; /* Note, may be NULL! */
#else
  FILE *File;
  int Line_Number,R_Flag;
  char *Filename;
#endif
  int unofficialVersion;
};

struct complex_block
{
  DBL r, c;
};

#define READ_MODE 0
#define WRITE_MODE 1
#define APPEND_MODE 2

struct file_handle_struct
{
  char *filename;
  int mode;
  int width, height;
  int buffer_size;
  char *buffer;
  FILE *file;
  int file_type;  /* What format the output file is */
  int  (*Open_File_p) (struct file_handle_struct *handle,
    char *name, int *width, int *height, int buffer_size, int mode);
  void (*Write_Line_p) (struct file_handle_struct *handle,
    COLOUR *line_data, int line_number);
  int (*Read_Line_p) (struct file_handle_struct *handle,
    COLOUR *line_data, int *line_number);
  void (*Read_Image_p) (IMAGE *Image, char *filename);
  void (*Close_File_p) (struct file_handle_struct *handle);
};

#define Open_File(h,n,wd,ht,sz,m) ((*((h)->Open_File_p)) (h,n,wd,ht,sz,m))
#define Write_Line(h,l,n) ((*((h)->Write_Line_p)) (h, l, n))
#define Read_Line(h,l,n) ((*((h)->Read_Line_p)) (h, l, n))
#define Read_Image(h,i) ((*((h)->Read_Image_p)) (h, i))
#define Close_File(h) ((*((h)->Close_File_p)) (h))

#ifdef FastMacroPatch
#define Data_File_Close(FILE) ((*((FILE)->Close)) (FILE))
#define Data_File_Getc(FILE) ((*((FILE)->Getc)) (FILE))
#define Data_File_Putc(FILE, CH) ((*((FILE)->Putc)) (FILE, CH))
#define Data_File_Ungetc(FILE, CH) ((*((FILE)->Ungetc)) (FILE, CH))
#define Data_File_Seek(FILE, OFF) ((*((FILE)->Seek)) (FILE, OFF))
#define Data_File_Tell(FILE) ((*((FILE)->Tell)) (FILE))
#endif

#endif
