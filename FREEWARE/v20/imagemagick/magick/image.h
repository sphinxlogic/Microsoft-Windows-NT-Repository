/*
  Image define declarations.
*/
#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))
#define DegreesToRadians(x) ((x)*PI/180.0)
#define Intensity(color)  (unsigned int)  \
  ((unsigned int) ((color).red*77+(color).green*150+(color).blue*29) >> 8)
#define MaxColormapSize  65535
#define MaxImageSize  (4096*4096)
#define MaxRGB  255
#define MaxRunlength  255
#define MaxTextLength  2048
#define Opaque  255
#ifndef vms
#define PostscriptCommand  \
  "gs -sDEVICE=%s -q -dNOPAUSE %s -sOutputFile=%s -- %s < /dev/null > /dev/null"
#else
#define PostscriptCommand  \
  "gs \"-sDEVICE=%s\" -q \"-dNOPAUSE\" \"%s\" \"-sOutputFile=%s\" -- \"%s\""
#endif
#define RadiansToDegrees(x) ((x)*180/PI)
#define Transparent  0

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

typedef struct _ImageInfo
{
  char
    *filename,
    magick[MaxTextLength];

  unsigned int
    assert,
    subimage;

  char
    *server_name,
    *font,
    *size,
    *density,
    *page;

  unsigned int
    dither,
    interlace,
    monochrome,
    quality,
    verbose;

  char
    *undercolor;
} ImageInfo;

typedef struct _RectangleInfo
{
  unsigned int
    width,
    height;

  int
    x,
    y;
} RectangleInfo;

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

  int
    status,
    temporary;

  char
    filename[MaxTextLength];

  long int
    filesize;

  int
    pipe;

  char
    magick[MaxTextLength],
    *comments,
    *label;

  unsigned int
    id,
    class,
    matte,
    compression,
    columns,
    rows,
    scene;

  char
    *montage,
    *directory;

  ColorPacket
    *colormap;

  unsigned int
    colorspace,
    colors;

  unsigned int
    mean_error_per_pixel;

  double
    normalized_mean_error,
    normalized_maximum_error;

  unsigned long
    total_colors;

  char
    *signature;

  RunlengthPacket
    *pixels,
    *packet;

  unsigned long
    packets;

  unsigned int
    runlength,
    packet_size;

  unsigned char
    *packed_pixels;

  unsigned int
    orphan;

  long int
    magick_time;

  char
    magick_filename[MaxTextLength];

  unsigned int
    magick_columns,
    magick_rows;

  struct _Image
    *previous,
    *next;
} Image;

/*
  Image utilities routines.
*/
extern void
  CommentImage _Declare((Image *,char *)),
  Error _Declare((char *,char *)),
  LabelImage _Declare((Image *,char *)),
  Warning _Declare((char *,char *));

extern Image
  *AllocateImage _Declare((ImageInfo *)),
  *BorderImage _Declare((Image *,RectangleInfo *,ColorPacket *)),
  *BlurImage _Declare((Image *)),
  *CopyImage _Declare((Image *,unsigned int,unsigned int,unsigned int)),
  *CropImage _Declare((Image *,RectangleInfo *)),
  *CutImage _Declare((Image *,RectangleInfo *)),
  *DespeckleImage _Declare((Image *)),
  *EmbossImage _Declare((Image *)),
  *EdgeImage _Declare((Image *)),
  *EnhanceImage _Declare((Image *)),
  *FlipImage _Declare((Image *)),
  *FlopImage _Declare((Image *)),
  *FrameImage _Declare((Image *,RectangleInfo *,unsigned int,ColorPacket *,
    ColorPacket *,ColorPacket *)),
  *NoisyImage _Declare((Image *)),
  *OilPaintImage _Declare((Image *)),
  *ReadImage _Declare((ImageInfo *)),
  *ReduceImage _Declare((Image *)),
  *RollImage _Declare((Image *,int,int)),
  *RotateImage _Declare((Image *,double,ColorPacket *,unsigned int)),
  *SampleImage _Declare((Image *,unsigned int,unsigned int)),
  *ScaleImage _Declare((Image *,unsigned int,unsigned int)),
  *SharpenImage _Declare((Image *)),
  *ShearImage _Declare((Image *,double,double,ColorPacket *,unsigned int)),
  *StereoImage _Declare((Image *,Image *)),
  *ZoomImage _Declare((Image *));

extern int
  ReadDataBlock _Declare((char *,FILE *));

extern unsigned int
  IsGrayImage _Declare((Image *)),
  IsPseudoClass _Declare((Image *)),
  ReadData _Declare((char *,int,int,FILE *)),
  UncompressImage _Declare((Image *)),
  WriteImage _Declare((ImageInfo *,Image *));

extern void
  CloseImage _Declare((Image *)),
  ColormapSignature _Declare((Image *)),
  CompositeImage _Declare((Image *,unsigned int,Image *,int,int)),
  CompressColormap _Declare((Image *)),
  CompressImage _Declare((Image *)),
  ContrastImage _Declare((Image *,unsigned int)),
  DescribeImage _Declare((Image *)),
  DestroyImage _Declare((Image *)),
  DestroyImages _Declare((Image *)),
  EqualizeImage _Declare((Image *)),
  GammaImage _Declare((Image *,char *)),
  GetImageInfo _Declare((ImageInfo *)),
  MapImage _Declare((Image *,Image *,unsigned int)),
  ModulateImage _Declare((Image *,char *)),
  MogrifyImage _Declare((ImageInfo *,int,char **,Image **)),
  NegateImage _Declare((Image *)),
  NormalizeImage _Declare((Image *)),
  NumberColors _Declare((Image *,FILE *)),
  OpenImage _Declare((Image *,char *)),
  ParseImageGeometry _Declare((char *,unsigned int *,unsigned int *)),
  QuantizationError _Declare((Image *)),
  QuantizeImage _Declare((Image *,unsigned int,unsigned int,unsigned int,
    unsigned int)),
  QuantizeImages _Declare((Image **,unsigned int,unsigned int,unsigned int,
    unsigned int,unsigned int)),
  RGBTransformImage _Declare((Image *,unsigned int)),
  SetErrorHandler _Declare((ErrorHandler)),
  SetImageMagick _Declare((ImageInfo *)),
  SetWarningHandler _Declare((ErrorHandler)),
  SortColormapByIntensity _Declare((Image *)),
  SyncImage _Declare((Image *)),
  TransformImage _Declare((Image **,char *,char *)),
  TransformRGBImage _Declare((Image *,unsigned int));
