/*
  Image define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define DegreesToRadians(x) ((x)*3.14159265358979323846/180.0)
#define Intensity(color)  (unsigned int)  \
  ((unsigned int) ((color).red*77+(color).green*150+(color).blue*29) >> 8)
#define MaxColormapSize  65535
#define MaxImageSize  (4096*4096)
#define MaxRGB  255
#define MaxRunlength  255

/*
  Image Id's
*/
#define UndefinedId  0
#define ImageMagickId  1
/*
  Image classes:
*/
#define UndefinedClass  0
#define DirectClass  1
#define PseudoClass  2
/*
  Image colorspaces:
*/
#define UndefinedColorspace  0
#define RGBColorspace  1
#define GRAYColorspace 2
#define XYZColorspace  3
#define YIQColorspace  4
#define YUVColorspace  5
/*
  Image compression algorithms:
*/
#define UndefinedCompression  0
#define NoCompression  1
#define RunlengthEncodedCompression  2
#define QEncodedCompression  3
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
  Typedef declarations for the Display program.
*/
typedef struct _ColorPacket
{
  unsigned char
    red,
    green,
    blue,
    flags;

  unsigned short
    index;
} ColorPacket;

typedef struct _RunlengthPacket
{
  unsigned char
    red,
    green,
    blue,
    length;

  unsigned short
    index;
} RunlengthPacket;

typedef struct _Image
{
  FILE
    *file;

  char
    filename[2048],
    magick[12],
    *comments,
    *label;

  unsigned int
    id,
    class,
    colorspace,
    alpha,
    compression,
    columns,
    rows,
    colors,
    scene,
    quality;

  char
    *montage,
    *directory;

  ColorPacket
    *colormap;

  char
    *signature;

  RunlengthPacket
    *pixels,
    *packet;

  unsigned long int
    packets;

  unsigned int
    runlength,
    packet_size;

  unsigned char
    *packed_pixels;

  unsigned int
    orphan;

  struct _Image
    *last,
    *next;
} Image;

/*
  Image utilities routines.
*/
extern Image
  *AllocateImage _Declare((char *)),
  *BorderImage _Declare((Image *,unsigned int,unsigned int,ColorPacket)),
  *ClipImage _Declare((Image *,int,int,unsigned int,unsigned int)),
  *CopyImage _Declare((Image *,unsigned int,unsigned int,unsigned int)),
  *EnhanceImage _Declare((Image *)),
  *NoisyImage _Declare((Image *)),
  *ReadImage _Declare((char *)),
  *ReflectImage _Declare((Image *)),
  *RollImage _Declare((Image *,int,int)),
  *RotateImage _Declare((Image *,double,unsigned int)),
  *ScaleImage _Declare((Image *,unsigned int,unsigned int)),
  *ShearImage _Declare((Image *,double,double,unsigned int)),
  *StereoImage _Declare((Image *,Image *));

extern int
  ReadDataBlock _Declare((char *,FILE *));

extern unsigned int
  NumberColors _Declare((Image *,FILE *)),
  PackImage _Declare((Image *)),
  PrintImage _Declare((Image *,char *)),
  ReadData _Declare((char *,int,int,FILE *)),
  UncompressImage _Declare((Image *)),
  UnpackImage _Declare((Image *)),
  WriteImage _Declare((Image *));

extern void
  CloseImage _Declare((Image *)),
  ColormapSignature _Declare((Image *)),
  CompositeImage _Declare((Image *,unsigned int,Image *,int,int)),
  CompressColormap _Declare((Image *)),
  CompressImage _Declare((Image *)),
  DestroyImage _Declare((Image *)),
  DestroyImages _Declare((Image *)),
  GammaImage _Declare((Image *,double)),
  InverseImage _Declare((Image *)),
  NormalizeImage _Declare((Image *)),
  OpenImage _Declare((Image *,char *)),
  QuantizationError _Declare((Image *,unsigned int *,double *,double *)),
  QuantizeImage _Declare((Image *,unsigned int,unsigned int,unsigned int,
    unsigned int,unsigned int)),
  QuantizeImages _Declare((Image **,unsigned int,Image *,unsigned int,
    unsigned int,unsigned int,unsigned int,unsigned int)),
  RGBTransformImage _Declare((Image *,unsigned int)),
  SortColormapByIntensity _Declare((Image *)),
  SyncImage _Declare((Image *)),
  TransformImage _Declare((Image **,char *,char *,char *)),
  TransformRGBImage _Declare((Image *,unsigned int));
