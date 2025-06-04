/*
  Include declarations
*/
#ifdef hpux
#define _HPUX_SOURCE 1
#endif
#include <stdio.h>
#if defined(__STDC__) || defined(sgi) || defined(_AIX)
#include <stdlib.h>
#include <unistd.h>
#else
#ifdef vms
#include <stdlib.h>
#else
#include <malloc.h>
#include <memory.h>
#endif
#endif
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#undef index
#undef assert

/*
  Define declarations for the Display program.
*/
#if __STDC__ || defined(sgi) || defined(_AIX)
#define _Declare(formal_parameters) formal_parameters
#else
#define const 
#define _Declare(formal_parameters) ()
#endif
#define ColorShift(color)  (((unsigned int) (color)) >> 8)
#define DownShift(x) ((int) ((x)+(1L << 15)) >> 16)
#define False  0
#define Max(x,y)  (((x) > (y)) ? (x) : (y))
#define Min(x,y)  (((x) < (y)) ? (x) : (y))
#define PI  3.14159265358979323846
#define True  1
#define UpShift(x) ((x) << 16)
#define UpShifted(x) ((int) ((x)*(1L << 16)+0.5))
#ifdef vms
#define pclose(file)  fclose(file)
#define popen(command,mode)  fopen(command,mode)
#define unlink(file)  remove(file)
#endif

/*
  Typedef declarations.
*/
typedef void
  (*ErrorHandler) _Declare((char *,char *));

/*
  Variable declarations.
*/
static char 
  Version[]="@(#)ImageMagick 3.4 94/11/01 cristy@dupont.com";

/*
  Define declarations.
*/

/*
  Image colorspaces:
*/
#define UndefinedColorspace  0
#define RGBColorspace  1
#define GRAYColorspace 2
#define OHTAColorspace  3
#define XYZColorspace  4
#define YCbCrColorspace  5
#define YCCColorspace  6
#define YIQColorspace  7
#define YPbPrColorspace  8
#define YUVColorspace  9
/*
  Image compression algorithms:
*/
#define UndefinedCompression  0
#define NoCompression  1
#define RunlengthEncodedCompression  2
#define QEncodedCompression  3
/*
  Image interlace:
*/
#define UndefinedInterlace  0
#define NoneInterlace  1
#define LineInterlace  2
#define PlaneInterlace  3
/*
  Image compositing operations:
*/
#define UndefinedCompositeOp  0
#define OverCompositeOp  1
#define InCompositeOp  2
#define OutCompositeOp  3
#define AtopCompositeOp  4
#define XorCompositeOp  5
#define PlusCompositeOp  6
#define MinusCompositeOp  7
#define AddCompositeOp  8
#define SubtractCompositeOp  9
#define DifferenceCompositeOp  10
#define ReplaceCompositeOp  11
/*
  Image color matching algorithms:
*/
#define PointMethodOp  0
#define ReplaceMethodOp  1
#define FloodfillMethodOp  2
/*
  Page geometries:
*/
#define PSDensityGeometry  "72x72"
#define PSPageGeometry  "612x792+18+94"
#define TextPageGeometry  "612x792+36+36"
