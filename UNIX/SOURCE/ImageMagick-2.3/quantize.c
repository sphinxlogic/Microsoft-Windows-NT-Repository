/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           QQQ   U   U   AAA   N   N  TTTTT  IIIII   ZZZZZ  EEEEE            %
%          Q   Q  U   U  A   A  NN  N    T      I        ZZ  E                %
%          Q   Q  U   U  AAAAA  N N N    T      I      ZZZ   EEEEE            %
%          Q  QQ  U   U  A   A  N  NN    T      I     ZZ     E                %
%           QQQQ   UUU   A   A  N   N    T    IIIII   ZZZZZ  EEEEE            %
%                                                                             %
%                                                                             %
%              Reduce the Number of Unique Colors in an Image                 %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
%                                                                             %
%  Copyright 1992 E. I. du Pont de Nemours & Company                          %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. du Pont de Nemours    %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. du Pont de Nemours & Company makes no representations   %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. du Pont de Nemours & Company disclaims all warranties with regard    %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. du Pont de Nemours & Company be       %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Realism in computer graphics typically requires using 24 bits/pixel to
%  generate an image.  Yet many graphic display devices do not contain
%  the amount of memory necessary to match the spatial and color
%  resolution of the human eye.  The QUANTIZE program takes a 24 bit
%  image and reduces the number of colors so it can be displayed on
%  raster device with less bits per pixel.  In most instances, the
%  quantized image closely resembles the original reference image.
%
%  A reduction of colors in an image is also desirable for image
%  transmission and real-time animation.
%
%  Function Quantize takes a standard RGB or monochrome images and quantizes
%  them down to some fixed number of colors.
%
%  For purposes of color allocation, an image is a set of n pixels, where
%  each pixel is a point in RGB space.  RGB space is a 3-dimensional
%  vector space, and each pixel, pi,  is defined by an ordered triple of
%  red, green, and blue coordinates, (ri, gi, bi).
%
%  Each primary color component (red, green, or blue) represents an
%  intensity which varies linearly from 0 to a maximum value, cmax, which
%  corresponds to full saturation of that color.  Color allocation is
%  defined over a domain consisting of the cube in RGB space with
%  opposite vertices at (0,0,0) and (cmax,cmax,cmax).  QUANTIZE requires
%  cmax = 255.
%
%  The algorithm maps this domain onto a tree in which each node
%  represents a cube within that domain.  In the following discussion
%  these cubes are defined by the coordinate of two opposite vertices:
%  The vertex nearest the origin in RGB space and the vertex farthest
%  from the origin.
%
%  The tree's root node represents the the entire domain, (0,0,0) through
%  (cmax,cmax,cmax).  Each lower level in the tree is generated by
%  subdividing one node's cube into eight smaller cubes of equal size.
%  This corresponds to bisecting the parent cube with planes passing
%  through the midpoints of each edge.
%
%  The basic algorithm operates in three phases: Classification,
%  Reduction, and Assignment.  Classification builds a color
%  description tree for the image.  Reduction collapses the tree until
%  the number it represents, at most, the number of colors desired in the
%  output image.  Assignment defines the output image's color map and
%  sets each pixel's color by reclassification in the reduced tree.
%
%  Classification begins by initializing a color description tree of
%  sufficient depth to represent each possible input color in a leaf.
%  However, it is impractical to generate a fully-formed color
%  description tree in the classification phase for realistic values of
%  cmax.  If colors components in the input image are quantized to k-bit
%  precision, so that cmax= 2k-1, the tree would need k levels below the
%  root node to allow representing each possible input color in a leaf.
%  This becomes prohibitive because the tree's total number of nodes is
%  1 + sum(i=1,k,8k).
%
%  A complete tree would require 19,173,961 nodes for k = 8, cmax = 255.
%  Therefore, to avoid building a fully populated tree, QUANTIZE: (1)
%  Initializes data structures for nodes only as they are needed;  (2)
%  Chooses a maximum depth for the tree as a function of the desired
%  number of colors in the output image (currently log2(colormap size)).
%
%  For each pixel in the input image, classification scans downward from
%  the root of the color description tree.  At each level of the tree it
%  identifies the single node which represents a cube in RGB space
%  containing the pixel's color.  It updates the following data for each
%  such node:
%
%    n1 : Number of pixels whose color is contained in the RGB cube
%    which this node represents;
%
%    n2 : Number of pixels whose color is not represented in a node at
%    lower depth in the tree;  initially,  n2 = 0 for all nodes except
%    leaves of the tree.
%
%    Sr, Sg, Sb : Sums of the red, green, and blue component values for
%    all pixels not classified at a lower depth. The combination of
%    these sums and n2  will ultimately characterize the mean color of a
%    set of pixels represented by this node.
%
%  Reduction repeatedly prunes the tree until the number of nodes with
%  n2 > 0 is less than or equal to the maximum number of colors allowed
%  in the output image.  On any given iteration over the tree, it selects
%  those nodes whose n1  count is minimal for pruning and merges their
%  color statistics upward. It uses a pruning threshold, ns, to govern
%  node selection as follows:
%
%    ns = 0
%    while number of nodes with (n2 > 0) > required maximum number of colors
%      prune all nodes such that n1 <= ns
%      Set ns to minimum n1 in remaining nodes
%
%  When a node to be pruned has offspring, the pruning procedure invokes
%  itself recursively in order to prune the tree from the leaves upward.
%  n2,  Sr, Sg,  and  Sb in a node being pruned are always added to the
%  corresponding data in that node's parent.  This retains the pruned
%  node's color characteristics for later averaging.
%
%  For each node, n2 pixels exist for which that node represents the
%  smallest volume in RGB space containing those pixel's colors.  When n2
%  > 0 the node will uniquely define a color in the output image. At the
%  beginning of reduction,  n2 = 0  for all nodes except a the leaves of
%  the tree which represent colors present in the input image.
%
%  The other pixel count, n1, indicates the total number of colors
%  within the cubic volume which the node represents.  This includes n1 -
%  n2  pixels whose colors should be defined by nodes at a lower level in
%  the tree.
%
%  Assignment generates the output image from the pruned tree.  The
%  output image consists of two parts: (1)  A color map, which is an
%  array of color descriptions (RGB triples) for each color present in
%  the output image;  (2)  A pixel array, which represents each pixel as
%  an index into the color map array.
%
%  First, the assignment phase makes one pass over the pruned color
%  description tree to establish the image's color map.  For each node
%  with n2  > 0, it divides Sr, Sg, and Sb by n2 .  This produces the
%  mean color of all pixels that classify no lower than this node.  Each
%  of these colors becomes an entry in the color map.
%
%  Finally,  the assignment phase reclassifies each pixel in the pruned
%  tree to identify the deepest node containing the pixel's color.  The
%  pixel's value in the pixel array becomes the index of this node's mean
%  color in the color map.
%
%  For efficiency, QUANTIZE requires that the reference image be in a
%  run-length encoded format.
%
%  With the permission of USC Information Sciences Institute, 4676 Admiralty
%  Way, Marina del Rey, California  90292, this code was adapted from module
%  ALCOLS written by Paul Raveling.
%
%  The names of ISI and USC are not used in advertising or publicity
%  pertaining to distribution of the software without prior specific
%  written permission from ISI.
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"

/*
  Define declarations.
*/
#define color_number  number_colors
#define MaxNodes  266817
#define MaxShift  8
#define MaxTreeDepth  8  /* Log2(MaxRGB) */
#define NodesInAList  2048

/*
  Structures.
*/
typedef struct _Node
{
  struct _Node
    *parent,
    *child[8];

  unsigned char
    id,
    level,
    children,
    mid_red,
    mid_green,
    mid_blue;

  unsigned long int
    number_colors,
    number_unique,
    total_red,
    total_green,
    total_blue;
} Node;

typedef struct _Nodes
{
  Node
    nodes[NodesInAList];

  struct _Nodes
    *next;
} Nodes;

typedef struct _Cube
{
  Node
    *root;

  ColorPacket
    color,
    *colormap;

  unsigned int
    depth;

  unsigned long int
    colors,
    pruning_threshold,
    next_pruning_threshold,
    distance,
    shift[MaxTreeDepth+1],
    squares[MaxRGB+MaxRGB+1];

  unsigned int
    nodes,
    free_nodes,
    color_number;

  Node
    *next_node;

  Nodes
    *node_queue;
} Cube;

/*
  Global variables.
*/
static Cube
  cube;

/*
  External declarations.
*/
extern char
  *client_name;

/*
  Forward declarations.
*/
static Node
  *InitializeNode _Declare((unsigned int,unsigned int,Node *,unsigned int,
    unsigned int,unsigned int));

static unsigned int
  DitherImage _Declare((Image *));

static void
  ClosestColor _Declare((Node *)),
  Colormap _Declare((Node *)),
  PruneLevel _Declare((Node *));

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  A s s i g n m e n t                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Assignment generates the output image from the pruned tree.  The
%  output image consists of two parts: (1)  A color map, which is an
%  array of color descriptions (RGB triples) for each color present in
%  the output image;  (2)  A pixel array, which represents each pixel as
%  an index into the color map array.
%
%  First, the assignment phase makes one pass over the pruned color
%  description tree to establish the image's color map.  For each node
%  with n2  > 0, it divides Sr, Sg, and Sb by n2 .  This produces the
%  mean color of all pixels that classify no lower than this node.  Each
%  of these colors becomes an entry in the color map.
%
%  Finally,  the assignment phase reclassifies each pixel in the pruned
%  tree to identify the deepest node containing the pixel's color.  The
%  pixel's value in the pixel array becomes the index of this node's mean
%  color in the color map.
%
%  The format of the Assignment routine is:
%
%      Assignment(image,dither,optimal)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o dither: Set this integer value to something other than zero to
%      dither the quantized image.  The basic strategy of dithering is to
%      trade intensity resolution for spatial resolution by averaging the
%      intensities of several neighboring pixels.  Images which suffer
%      from severe contouring when quantized can be improved with the
%      technique of dithering.  Severe contouring generally occurs when
%      quantizing to very few colors, or to a poorly-chosen colormap.
%      Note, dithering is a computationally expensive process and will
%      increase processing time significantly.
%
%    o optimal: An unsigned integer value greater than zero indicates that
%      the optimal representation of the reference image should be returned.
%
%
*/
static void Assignment(image,dither,optimal)
Image
  *image;

unsigned int
  dither,
  optimal;
{
  register int
    i;

  register Node
    *node;

  register RunlengthPacket
    *p;

  register unsigned int
    id;

  /*
    Allocate image colormap.
  */
  image->alpha=False;
  image->class=PseudoClass;
  if (image->colormap != (ColorPacket *) NULL)
    (void) free((char *) image->colormap);
  image->colormap=(ColorPacket *)
    malloc((unsigned int) cube.colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      Warning("unable to quantize image","memory allocation failed");
      exit(1);
    }
  if (image->signature != (char *) NULL)
    (void) free((char *) image->signature);
  image->signature=(char *) NULL;
  cube.colormap=image->colormap;
  cube.colors=0;
  Colormap(cube.root);
  image->colors=(unsigned int) cube.colors;
  /*
    Create a reduced color image.  For the non-optimal case we trade
    accuracy for speed improvements.
  */
  if (dither)
    dither=!DitherImage(image);
  p=image->pixels;
  if (!dither)
    if (!optimal)
      for (i=0; i < image->packets; i++)
      {
        /*
          Identify the deepest node containing the pixel's color.
        */
        node=cube.root;
        for ( ; ; )
        {
          id=(p->red > node->mid_red ? 1 : 0) |
            (p->green > node->mid_green ? 1 : 0) << 1 |
            (p->blue > node->mid_blue ? 1 : 0) << 2;
          if ((node->children & (1 << id)) == 0)
            break;
          node=node->child[id];
        }
        p->index=(unsigned short) node->color_number;
        p++;
      }
    else
      for (i=0; i < image->packets; i++)
      {
        /*
          Identify the deepest node containing the pixel's color.
        */
        node=cube.root;
        for ( ; ; )
        {
          id=(p->red > node->mid_red ? 1 : 0) |
            (p->green > node->mid_green ? 1 : 0) << 1 |
            (p->blue > node->mid_blue ? 1 : 0) << 2;
          if ((node->children & (1 << id)) == 0)
            break;
          node=node->child[id];
        }
        /*
          Find closest color among siblings and their children.
        */
        cube.color.red=p->red;
        cube.color.green=p->green;
        cube.color.blue=p->blue;
        cube.distance=(unsigned long int) (~0);
        ClosestColor(node->parent);
        p->index=cube.color_number;
        p++;
      }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C l a s s i f i c a t i o n                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Classification begins by initializing a color description tree
%  of sufficient depth to represent each possible input color in a leaf.
%  However, it is impractical to generate a fully-formed color
%  description tree in the classification phase for realistic values of
%  cmax.  If colors components in the input image are quantized to k-bit
%  precision, so that cmax= 2k-1, the tree would need k levels below the
%  root node to allow representing each possible input color in a leaf.
%  This becomes prohibitive because the tree's total number of nodes is
%  1 + sum(i=1,k,8k).
%
%  A complete tree would require 19,173,961 nodes for k = 8, cmax = 255.
%  Therefore, to avoid building a fully populated tree, QUANTIZE: (1)
%  Initializes data structures for nodes only as they are needed;  (2)
%  Chooses a maximum depth for the tree as a function of the desired
%  number of colors in the output image (currently log2(colormap size)).
%
%  For each pixel in the input image, classification scans downward from
%  the root of the color description tree.  At each level of the tree it
%  identifies the single node which represents a cube in RGB space
%  containing It updates the following data for each such node:
%
%    n1 : Number of pixels whose color is contained in the RGB cube
%    which this node represents;
%
%    n2 : Number of pixels whose color is not represented in a node at
%    lower depth in the tree;  initially,  n2 = 0 for all nodes except
%    leaves of the tree.
%
%    Sr, Sg, Sb : Sums of the red, green, and blue component values for
%    all pixels not classified at a lower depth. The combination of
%    these sums and n2  will ultimately characterize the mean color of a
%    set of pixels represented by this node.
%
%  The format of the Classification routine is:
%
%      Classification(image)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static void Classification(image)
Image
  *image;
{
  register int
    i;

  register Node
    *node;

  register RunlengthPacket
    *p;

  register unsigned int
    bisect,
    count,
    id,
    level;

  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    if (cube.nodes > MaxNodes)
      {
        /*
          Prune one level if the color tree is too large.
        */
        PruneLevel(cube.root);
        cube.depth--;
      }
    /*
      Start at the root and descend the color cube tree.
    */
    count=p->length+1;
    node=cube.root;
    for (level=1; level < cube.depth; level++)
    {
      id=(p->red > node->mid_red ? 1 : 0) |
        (p->green > node->mid_green ? 1 : 0) << 1 |
        (p->blue > node->mid_blue ? 1 : 0) << 2;
      if (node->child[id] == (Node *) NULL)
        {
          /*
            Set colors of new node to contain pixel.
          */
          node->children|=1 << id;
          bisect=(unsigned int) (1 << (MaxTreeDepth-level)) >> 1;
          node->child[id]=InitializeNode(id,level,node,
            node->mid_red+(id & 1 ? bisect : -bisect),
            node->mid_green+(id & 2 ? bisect : -bisect),
            node->mid_blue+(id & 4 ? bisect : -bisect));
          if (node->child[id] == (Node *) NULL)
            {
              Warning("unable to quantize image","memory allocation failed");
              exit(1);
            }
          if (level == (cube.depth-1))
            cube.colors++;
        }
      /*
        Record the number of colors represented by this node.  Shift by level
        in the color description tree.
      */
      node=node->child[id];
      node->number_colors+=count << cube.shift[level];
    }
    /*
      Increment unique color count and sum RGB values for this leaf for later
      derivation of the mean cube color.
    */
    node->number_unique+=count;
    node->total_red+=p->red*count;
    node->total_green+=p->green*count;
    node->total_blue+=p->blue*count;
    p++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C l o s e s t C o l o r                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ClosestColor traverses the color cube tree at a particular node
%  and determines which colormap entry best represents the input color.
%
%  The format of the ClosestColor routine is:
%
%      ClosestColor(node)
%
%  A description of each parameter follows.
%
%    o node: The address of a structure of type Node which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void ClosestColor(node)
register Node
  *node;
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node->children > 0)
    for (id=0; id < 8; id++)
      if (node->children & (1 << id))
        ClosestColor(node->child[id]);
  if (node->number_unique > 0)
    {
      register ColorPacket
        *color;

      register unsigned int
        blue_distance,
        green_distance,
        red_distance;

      register unsigned long int
        distance;

      /*
        Determine if this color is "closest".
      */
      color=cube.colormap+node->color_number;
      red_distance=(int) color->red-(int) cube.color.red+MaxRGB;
      green_distance=(int) color->green-(int) cube.color.green+MaxRGB;
      blue_distance=(int) color->blue-(int) cube.color.blue+MaxRGB;
      distance=cube.squares[red_distance]+cube.squares[green_distance]+
        cube.squares[blue_distance];
      if (distance < cube.distance)
        {
          cube.distance=distance;
          cube.color_number=(unsigned short) node->color_number;
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C o l o r m a p                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure Colormap traverses the color cube tree and notes each colormap
%  entry.  A colormap entry is any node in the color cube tree where the
%  number of unique colors is not zero.
%
%  The format of the Colormap routine is:
%
%      Colormap(node)
%
%  A description of each parameter follows.
%
%    o node: The address of a structure of type Node which points to a
%      node in the color cube tree that is to be pruned.
%
%
*/
static void Colormap(node)
register Node
  *node;
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node->children > 0)
    for (id=0; id < 8; id++)
      if (node->children & (1 << id))
        Colormap(node->child[id]);
  if (node->number_unique > 0)
    {
      /*
        Colormap entry is defined by the mean color in this cube.        
      */
      cube.colormap[cube.colors].red=(unsigned char)
        ((node->total_red+(node->number_unique >> 1))/node->number_unique);
      cube.colormap[cube.colors].green=(unsigned char)
        ((node->total_green+(node->number_unique >> 1))/node->number_unique);
      cube.colormap[cube.colors].blue=(unsigned char)
        ((node->total_blue+(node->number_unique >> 1))/node->number_unique);
      node->color_number=cube.colors++;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  D i t h e r I m a g e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure DitherImage uses the Floyd-Steinberg algorithm to dither the
%  image.  Refer to "An Adaptive Algorithm for Spatial GreySscale", Robert W.
%  Floyd and Louis Steinberg, Proceedings of the S.I.D., Volume 17(2), 1976.
%
%  First find the closest representation to the reference pixel color in the
%  colormap, the node pixel is assigned this color.  Next, the colormap color
%  is subtracted from the reference pixels color, this represents the
%  quantization error.  Various amounts of this error are added to the pixels
%  ahead and below the node pixel to correct for this error.  The error
%  proportions are:
%
%            P     7/16
%      3/16  5/16  1/16
%
%  The error is distributed left-to-right for even scanlines and right-to-left
%  for odd scanlines.
%
%  The format of the DitherImage routine is:
%
%      DitherImage(image)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int DitherImage(image)
Image
  *image;
{
  typedef struct _ScaledColorPacket
  {
    int
      red,
      green,
      blue;
  } ScaledColorPacket;

  int
    *cache,
    odd_scanline;

  register int
    blue_error,
    green_error,
    red_error,
    step;

  register Node
    *node;

  register RunlengthPacket
    *p,
    *q;

  register ScaledColorPacket
    *cs,
    *ns;

  register unsigned char
    *range_limit;

  register unsigned int
    id;

  ScaledColorPacket
    *scanline;

  unsigned char
    blue,
    green,
    *range_table,
    red;

  unsigned int
    i,
    x,
    y;

  unsigned short
    index;

  /*
    Image must be uncompressed.
  */
  if (!UncompressImage(image))
    return(True);
  image->compression=NoCompression;
  /*
    Allocate the cache & scanline buffers to keep track of quantization error.
  */
  cache=(int *) malloc((1 << 18)*sizeof(int));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  scanline=(ScaledColorPacket *)
    malloc(2*(image->columns+2)*sizeof(ScaledColorPacket));
  if ((cache == (int *) NULL) || (range_table == (unsigned char *) NULL) ||
      (scanline == (ScaledColorPacket *) NULL))
    {
      Warning("unable to dither image","memory allocation failed");
      return(True);
    }
  /*
    Initialize tables.
  */
  for (i=0; i < (1 << 18); i++)
    cache[i]=(-1);
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  /*
    Preload first scanline.
  */
  p=image->pixels;
  cs=scanline+1;
  for (i=0; i < image->columns; i++)
  {
    cs->red=p->red;
    cs->green=p->green;
    cs->blue=p->blue;
    p++;
    cs++;
  }
  odd_scanline=False;
  for (y=0; y < image->rows; y++)
  {
    if (y < (image->rows-1))
      {
        /*
          Read another scanline.
        */
        ns=scanline+1;
        if (!odd_scanline)
          ns+=(image->columns+2);
        for (i=0; i < image->columns; i++)
        {
          ns->red=p->red;
          ns->green=p->green;
          ns->blue=p->blue;
          p++;
          ns++;
        }
      }
    if (!odd_scanline)
      {
        /*
          Distribute error left-to-right for even scanlines.
        */
        q=image->pixels+image->columns*y;
        cs=scanline+1;
        ns=scanline+(image->columns+2)+1;
        step=1;
      }
    else
      {
        /*
          Distribute error right-to-left for odd scanlines.
        */
        q=image->pixels+image->columns*y+(image->columns-1);
        cs=scanline+(image->columns+2)+(image->columns-1)+1;
        ns=scanline+(image->columns-1)+1;
        step=(-1);
      }
    for (x=0; x < image->columns; x++)
    {
      red=range_limit[cs->red];
      green=range_limit[cs->green];
      blue=range_limit[cs->blue];
      i=(red >> 2) << 12 | (green >> 2) << 6 | blue >> 2;
      if (cache[i] < 0)
        {
          /*
            Identify the deepest node containing the pixel's color.
          */
          node=cube.root;
          for ( ; ; )
          {
            id=(red > node->mid_red ? 1 : 0) |
              (green > node->mid_green ? 1 : 0) << 1 |
              (blue > node->mid_blue ? 1 : 0) << 2;
            if ((node->children & (1 << id)) == 0)
              break;
            node=node->child[id];
          }
          /*
            Find closest color among siblings and their children.
          */
          cube.color.red=red;
          cube.color.green=green;
          cube.color.blue=blue;
          cube.distance=(unsigned long int) (~0);
          ClosestColor(node->parent);
          cache[i]=cube.color_number;
        }
      index=(unsigned short) cache[i];
      red_error=(int) red-(int) cube.colormap[index].red;
      green_error=(int) green-(int) cube.colormap[index].green;
      blue_error=(int) blue-(int) cube.colormap[index].blue;
      q->index=index;
      q+=step;
      /*
        Propagate the error in these proportions:
                Q     7/16
          3/16  5/16  1/16
      */
      cs+=step;
      cs->red+=(red_error-((red_error*9+8)/16));
      cs->green+=(green_error-((green_error*9+8)/16));
      cs->blue+=(blue_error-((blue_error*9+8)/16));
      ns-=step;
      ns->red+=(red_error*3+8)/16;
      ns->green+=(green_error*3+8)/16;
      ns->blue+=(blue_error*3+8)/16;
      ns+=step;
      ns->red+=(red_error*5+8)/16;
      ns->green+=(green_error*5+8)/16;
      ns->blue+=(blue_error*5+8)/16;
      ns+=step;
      ns->red+=(red_error+8)/16;
      ns->green+=(green_error+8)/16;
      ns->blue+=(blue_error+8)/16;
    }
    odd_scanline=!odd_scanline;
  }
  /*
    Free up memory.
  */
  (void) free((char *) scanline);
  (void) free((char *) range_table);
  (void) free((char *) cache);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I n i t i a l i z e C u b e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function InitializeCube initialize the Cube data structure.
%
%  The format of the InitializeCube routine is:
%
%      InitializeCube(number_colors,tree_depth,number_pixels,optimal)
%
%  A description of each parameter follows.
%
%    o number_colors: This integer value indicates the maximum number of
%      colors in the quantized image or colormap.  Here we use this value
%      to determine the depth of the color description tree.
%
%    o tree_depth: Normally, this integer value is zero or one.  A zero or
%      one tells Quantize to choose a optimal tree depth of Log4(number_colors).
%      A tree of this depth generally allows the best representation of the
%      reference image with the least amount of memory and the fastest
%      computational speed.  In some cases, such as an image with low color
%      dispersion (a few number of colors), a value other than
%      Log4(number_colors) is required.  To expand the color tree completely,
%      use a value of 8.
%
%    o number_pixels: Specifies the number of individual pixels in the image.
%
%    o optimal: An unsigned integer value greater than zero indicates that
%      the optimal representation of the reference image should be returned.
%
*/
static void InitializeCube(number_colors,tree_depth,number_pixels,optimal)
unsigned int
  number_colors,
  tree_depth,
  number_pixels,
  optimal;
{
  register int
    i;

  static int
    log4[6] = {4, 16, 64, 256, 1024, ~0};

  unsigned int
    level;

  unsigned long int
    max_shift;

  /*
    Initialize tree to describe color cube.  Depth is: Log4(colormap size)+2;
  */
  cube.node_queue=(Nodes *) NULL;
  cube.nodes=0;
  cube.free_nodes=0;
  if (tree_depth > 1)
    cube.depth=Min(tree_depth,8);
  else
    {
      for (i=0; i < 6; i++)
        if (number_colors <= log4[i])
          break;
      cube.depth=i+3;
      if (!optimal)
        cube.depth--;
    }
  /*
    Initialize the shift values.
  */
  for (max_shift=0; number_pixels != 0; max_shift++)
    number_pixels<<=1;
  for (level=0; level < cube.depth; level++)
  {
    cube.shift[level]=(cube.depth-level-1) << 1;
    if (cube.shift[level] > max_shift)
      cube.shift[level]=max_shift;
  }
  /*
    Initialize the square values.
  */
  for (i=(-MaxRGB); i <= MaxRGB; i++)
    cube.squares[i+MaxRGB]=i*i;
  /*
    Initialize root node.
  */
  cube.root=InitializeNode(0,0,(Node *) NULL,(MaxRGB+1) >> 1,(MaxRGB+1) >> 1,
    (MaxRGB+1) >> 1);
  if (cube.root == (Node *) NULL)
    {
      Warning("unable to quantize image","memory allocation failed");
      exit(1);
    }
  cube.root->parent=cube.root;
  cube.root->number_colors=(unsigned long int) (~0);
  cube.colors=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I n i t i a l i z e N o d e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function InitializeNode allocates memory for a new node in the color cube
%  tree and presets all fields to zero.
%
%  The format of the InitializeNode routine is:
%
%      node=InitializeNode(node,id,level,mid_red,mid_green,mid_blue)
%
%  A description of each parameter follows.
%
%    o node: The InitializeNode routine returns this integer address.
%
%    o id: Specifies the child number of the node.
%
%    o level: Specifies the level in the classification the node resides.
%
%    o mid_red: Specifies the mid point of the red axis for this node.
%
%    o mid_green: Specifies the mid point of the green axis for this node.
%
%    o mid_blue: Specifies the mid point of the blue axis for this node.
%
%
*/
static Node *InitializeNode(id,level,parent,mid_red,mid_green,mid_blue)
unsigned int
  id,
  level;

Node
  *parent;

unsigned int
  mid_red,
  mid_green,
  mid_blue;
{
  register int
    i;

  register Node
    *node;

  if (cube.free_nodes == 0)
    {
      register Nodes
        *nodes;

      /*
        Allocate a new nodes of nodes.
      */
      nodes=(Nodes *) malloc(sizeof(Nodes));
      if (nodes == (Nodes *) NULL)
        return((Node *) NULL);
      nodes->next=cube.node_queue;
      cube.node_queue=nodes;
      cube.next_node=nodes->nodes;
      cube.free_nodes=NodesInAList;
    }
  cube.nodes++;
  cube.free_nodes--;
  node=cube.next_node++;
  node->parent=parent;
  for (i=0; i < 8; i++)
    node->child[i]=(Node *) NULL;
  node->id=id;
  node->level=level;
  node->children=0;
  node->mid_red=mid_red;
  node->mid_green=mid_green;
  node->mid_blue=mid_blue;
  node->number_colors=0;
  node->number_unique=0;
  node->total_red=0;
  node->total_green=0;
  node->total_blue=0;
  return(node);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  P r u n e C h i l d                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function PruneChild deletes the given node and merges its statistics into
%  its parent.
%
%  The format of the PruneSubtree routine is:
%
%      PruneChild(node)
%
%  A description of each parameter follows.
%
%    o node: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void PruneChild(node)
register Node
  *node;
{
  register Node
    *parent;

  /*
    Merge color statistics into parent.
  */
  parent=node->parent;
  parent->children&=~(1 << node->id);
  parent->number_unique+=node->number_unique;
  parent->total_red+=node->total_red;
  parent->total_green+=node->total_green;
  parent->total_blue+=node->total_blue;
  cube.nodes--;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  P r u n e L e v e l                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure PruneLevel deletes all nodes at the bottom level of the color
%  tree merging their color statistics into their parent node.
%
%  The format of the PruneLevel routine is:
%
%      PruneLevel(node)
%
%  A description of each parameter follows.
%
%    o node: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void PruneLevel(node)
register Node
  *node;
{
  register int
    id;

  /*
    Traverse any children.
  */
  if (node->children > 0)
    for (id=0; id < 8; id++)
      if (node->children & (1 << id))
        PruneLevel(node->child[id]);
  if (node->level == (cube.depth-1))
    PruneChild(node);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e d u c e                                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Reduce traverses the color cube tree and prunes any node whose
%  number of colors fall below a particular threshold.
%
%  The format of the Reduce routine is:
%
%      Reduce(node)
%
%  A description of each parameter follows.
%
%    o node: pointer to node in color cube tree that is to be pruned.
%
%
*/
static void Reduce(node)
register Node
  *node;
{
  register unsigned int
    id;

  /*
    Traverse any children.
  */
  if (node->children > 0)
    for (id=0; id < 8; id++)
      if (node->children & (1 << id))
        Reduce(node->child[id]);
  if (node->number_colors <= cube.pruning_threshold)
    {
      /*
        Node has a sub-threshold color count so prune it.  Node is an colormap
        entry if parent does not have any unique colors.
      */
      if (node->parent->number_unique == 0)
        cube.colors++;
      PruneChild(node);
      if (node->parent->number_colors < cube.next_pruning_threshold)
        cube.next_pruning_threshold=node->parent->number_colors;
    }
  else
    {
      /*
        Find minimum pruning threshold.  Node is a colormap entry if it has
        unique colors.
      */
      if (node->number_unique > 0)
        cube.colors++;
      if (node->number_colors < cube.next_pruning_threshold)
        cube.next_pruning_threshold=node->number_colors;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e d u c t i o n                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Reduction repeatedly prunes the tree until the number of nodes
%  with n2 > 0 is less than or equal to the maximum number of colors allowed
%  in the output image.  On any given iteration over the tree, it selects
%  those nodes whose n1  count is minimal for pruning and merges their
%  color statistics upward. It uses a pruning threshold, ns, to govern
%  node selection as follows:
%
%    ns = 0
%    while number of nodes with (n2 > 0) > required maximum number of colors
%      prune all nodes such that n1 <= ns
%      Set ns to minimum n1 in remaining nodes
%
%  When a node to be pruned has offspring, the pruning procedure invokes
%  itself recursively in order to prune the tree from the leaves upward.
%  n2,  Sr, Sg,  and  Sb in a node being pruned are always added to the
%  corresponding data in that node's parent.  This retains the pruned
%  node's color characteristics for later averaging.
%
%  For each node, n2 pixels exist for which that node represents the
%  smallest volume in RGB space containing those pixel's colors.  When n2
%  > 0 the node will uniquely define a color in the output image. At the
%  beginning of reduction,  n2 = 0  for all nodes except a the leaves of
%  the tree which represent colors present in the input image.
%
%  The other pixel count, n1, indicates the total number of colors
%  within the cubic volume which the node represents.  This includes n1 -
%  n2  pixels whose colors should be defined by nodes at a lower level in
%  the tree.
%
%  The format of the Reduction routine is:
%
%      Reduction(number_colors)
%
%  A description of each parameter follows.
%
%    o number_colors: This integer value indicates the maximum number of
%      colors in the quantized image or colormap.  The actual number of
%      colors allocated to the colormap may be less than this value, but
%      never more.  Note, the number of colors is restricted to a value
%      less than or equal to 65536 if the continuous_tone parameter has a
%      value of zero.
%
%
*/
static void Reduction(number_colors)
register unsigned int
  number_colors;
{
  cube.next_pruning_threshold=1;
  while (cube.colors > number_colors)
  {
    cube.pruning_threshold=cube.next_pruning_threshold;
    cube.next_pruning_threshold=cube.root->number_colors-1;
    cube.colors=0;
    Reduce(cube.root);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u a n t i z a t i o n E r r o r                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function QuantizationError measures the difference between the original
%  and quantized images.  This difference is the total quantization error.
%  The error is computed by summing over all pixels in an image the distance
%  squared in RGB space between each reference pixel value and its quantized
%  value.
%
%  The format of the QuantizationError routine is:
%
%      QuantizationError(image,mean_error_per_pixel,
%        normalized_mean_square_error,normalized_maximum_square_error)
%
%  A description of each parameter follows.
%
%    o image: The address of a byte (8 bits) array of run-length
%      encoded pixel data of your reference image.  The sum of the
%      run-length counts in the reference image must be equal to or exceed
%      the number of pixels.
%
%    o mean_error_per_pixel: The address of an integer value.  The value
%      returned is the mean error for any single pixel in the image.
%
%    o normalized_mean_square_error: The address of a real value.  The value
%      returned is the normalized mean quantization error for any single
%      pixel in the image.  This distance measure is normalized to a
%      range between 0 and 1.  It is independent of the range of red,
%      green, and blue values in your image.
%
%    o normalized_maximum_square_error: The address of a real value.  The value
%      returned is the normalized maximum quantization error for any
%      single pixel in the image.  This distance measure is normalized to
%      a range between 0 and 1.  It is independent of the range of red,
%      green, and blue values in your image.
%
%
*/
void QuantizationError(image,mean_error_per_pixel,normalized_mean_square_error,
  normalized_maximum_square_error)
Image
  *image;

unsigned int
  *mean_error_per_pixel;

double
  *normalized_mean_square_error,
  *normalized_maximum_square_error;
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned int
    blue_distance,
    green_distance,
    red_distance;

  unsigned long int
    distance,
    maximum_error_per_pixel,
    total_error;

  /*
    For each pixel, collect error statistics.
  */
  for (i=(-MaxRGB); i <= MaxRGB; i++)
    cube.squares[i+MaxRGB]=i*i;
  maximum_error_per_pixel=0;
  total_error=0;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    red_distance=(int) p->red-(int) image->colormap[p->index].red+MaxRGB;
    green_distance=(int) p->green-(int) image->colormap[p->index].green+MaxRGB;
    blue_distance=(int) p->blue-(int) image->colormap[p->index].blue+MaxRGB;
    distance=cube.squares[red_distance]+cube.squares[green_distance]+
      cube.squares[blue_distance];
    total_error+=(distance*(p->length+1));
    if (distance > maximum_error_per_pixel)
      maximum_error_per_pixel=distance;
    p++;
  }
  /*
    Compute final error statistics.
  */
  *mean_error_per_pixel=(unsigned int) total_error/(image->columns*image->rows);
  *normalized_mean_square_error=((double) *mean_error_per_pixel)/
    (3.0*(MaxRGB+1)*(MaxRGB+1));
  *normalized_maximum_square_error=((double) maximum_error_per_pixel)/
    (3.0*(MaxRGB+1)*(MaxRGB+1));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u a n t i z e I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function QuantizeImage analyzes the colors within a reference image and
%  chooses a fixed number of colors to represent the image.  The goal of the
%  algorithm is to minimize the difference between the input and output image
%  while minimizing the processing time.
%
%  The format of the Quantize routine is:
%
%      colors=QuantizeImage(image,number_colors,tree_depth,dither,optimal)
%
%  A description of each parameter follows.
%
%    o colors: The QuantizeImage function returns this integer
%      value.  It is the actual number of colors allocated in the
%      colormap.  Note, the actual number of colors allocated may be less
%      than the number of colors requested, but never more.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o number_colors: This integer value indicates the maximum number of
%      colors in the quantized image or colormap.  The actual number of
%      colors allocated to the colormap may be less than this value, but
%      never more.  Note, the number of colors is restricted to a value
%      less than or equal to 65536 if the continuous_tone parameter has a
%      value of zero.
%
%    o tree_depth: Normally, this integer value is zero or one.  A zero or
%      one tells Quantize to choose a optimal tree depth of Log4(number_colors).
%      A tree of this depth generally allows the best representation of the
%      reference image with the least amount of memory and the fastest
%      computational speed.  In some cases, such as an image with low color
%      dispersion (a few number of colors), a value other than
%      Log4(number_colors) is required.  To expand the color tree completely,
%      use a value of 8.
%
%    o dither: Set this integer value to something other than zero to
%      dither the quantized image.  The basic strategy of dithering is to
%      trade intensity resolution for spatial resolution by averaging the
%      intensities of several neighboring pixels.  Images which suffer
%      from severe contouring when quantized can be improved with the
%      technique of dithering.  Severe contouring generally occurs when
%      quantizing to very few colors, or to a poorly-chosen colormap.
%      Note, dithering is a computationally expensive process and will
%      increase processing time significantly.
%
%    o colorspace: An unsigned integer value that indicates the colorspace.
%      Empirical evidence suggests that distances in YUV or YIQ correspond to
%      perceptual color differences more closely than do distances in RGB
%      space.  The image is then returned to RGB colorspace after color
%      reduction.
%
%    o optimal: An unsigned integer value greater than zero indicates that
%      the optimal representation of the reference image should be returned.
%
%
*/
void QuantizeImage(image,number_colors,tree_depth,dither,colorspace,optimal)
Image
  *image;

unsigned int
  number_colors,
  tree_depth,
  dither,
  colorspace,
  optimal;
{
  Nodes
    *nodes;

  /*
    Reduce the number of colors in the continuous tone image.
  */
  if (number_colors > MaxColormapSize)
    number_colors=MaxColormapSize;
  InitializeCube(number_colors,tree_depth,image->columns*image->rows,optimal);
  if (!optimal)
    dither|=cube.depth > 3;
  if (!dither)
    if (image->packets == (image->columns*image->rows))
      CompressImage(image);
  if (colorspace != RGBColorspace)
    RGBTransformImage(image,colorspace);
  Classification(image);
  Reduction(number_colors);
  Assignment(image,dither,optimal);
  if (colorspace != RGBColorspace)
    TransformRGBImage(image,colorspace);
  /*
    Release color cube tree storage.
  */
  do
  {
    nodes=cube.node_queue->next;
    (void) free((char *) cube.node_queue);
    cube.node_queue=nodes;
  }
  while (cube.node_queue != (Nodes *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t i z e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantizeImages analyzes the colors within a set of reference images and
%  chooses a fixed number of colors to represent the set.  The goal of the
%  algorithm is to minimize the difference between the input and output images
%  while minimizing the processing time.
%
%  The format of the QuantizeImages routine is:
%
%      QuantizeImages(images,number_images,number_colors,tree_depth,dither,
%        optimal)
%
%  A description of each parameter follows:
%
%    o images: Specifies a pointer to a set of Image structures.
%
%    o number_images:  Specifies an unsigned integer representing the number
%      images in the image set.
%
%    o colormap_image: Specifies a pointer to a Image structure.  Reduce
%      images to a set of colors represented by this image.
%
%    o number_colors: This integer value indicates the maximum number of
%      colors in the quantized image or colormap.  The actual number of colors
%      allocated to the colormap may be less than this value, but never more.
%      Note, the number of colors is restricted to a value less than or equal
%      to 65536 if the quantized image is not DirectClass.
%
%    o tree_depth: Normally, this integer value is zero or one.  A zero or
%      one tells QUANTIZE to choose a optimal tree depth.  An optimal depth
%      generally allows the best representation of the reference image with the
%      fastest computational speed and the least amount of memory.  However,
%      the default depth is inappropriate for some images.  To assure the best
%      representation, try values between 2 and 8 for this parameter.
%
%    o dither: Set this integer value to something other than zero to
%      dither the quantized image.  The basic strategy of dithering is to
%      trade intensity resolution for spatial resolution by averaging the
%      intensities of several neighboring pixels.  Images which suffer
%      from severe contouring when quantized can be improved with the
%      technique of dithering.  Severe contouring generally occurs when
%      quantizing to very few colors, or to a poorly-chosen colormap.
%      Note, dithering is a computationally expensive process and will
%      increase processing time significantly.
%
%    o colorspace: An unsigned integer value that indicates the colorspace.
%      Empirical evidence suggests that distances in YUV or YIQ correspond to
%      perceptual color differences more closely than do distances in RGB
%      space.  The image is then returned to RGB colorspace after color
%      reduction.
%
%    o optimal: An unsigned integer value greater than zero indicates that
%      the optimal representation of the reference image should be returned.
%
%
*/
void QuantizeImages(images,number_images,colormap_image,number_colors,
  tree_depth,dither,colorspace,optimal)
Image
  **images;

unsigned int
  number_images;

Image
  *colormap_image;

unsigned int
  number_colors,
  tree_depth,
  dither,
  colorspace,
  optimal;
{
  Nodes
    *nodes;

  register unsigned int
    i;

  /*
    Reduce the number of colors in the continuous tone image sequence.
  */
  InitializeCube(number_colors,tree_depth,~0,optimal);
  if (!optimal)
    dither|=cube.depth > 3;
  if (colormap_image != (Image *) NULL)
    {
      /*
        Reduce images to a set of colors represented by the colormap image.
      */
      if (!dither)
        if (colormap_image->packets ==
             (colormap_image->columns*colormap_image->rows))
          CompressImage(colormap_image);
      if (colorspace != RGBColorspace)
        RGBTransformImage(colormap_image,colorspace);
      Classification(colormap_image);
      if (colorspace != RGBColorspace)
        TransformRGBImage(colormap_image,colorspace);
      optimal=True;
    }
  for (i=0; i < number_images; i++)
  {
    if (!dither)
      if (images[i]->packets == (images[i]->columns*images[i]->rows))
        CompressImage(images[i]);
    if (colorspace != RGBColorspace)
      RGBTransformImage(images[i],colorspace);
    if (colormap_image == (Image *) NULL)
      Classification(images[i]);
  }
  Reduction(number_colors);
  for (i=0; i < number_images; i++)
  {
    Assignment(images[i],dither,optimal);
    if (colorspace != RGBColorspace)
      TransformRGBImage(images[i],colorspace);
  }
  /*
    Release color cube tree storage.
  */
  do
  {
    nodes=cube.node_queue->next;
    (void) free((char *) cube.node_queue);
    cube.node_queue=nodes;
  }
  while (cube.node_queue != (Nodes *) NULL);
}
