/*  DEC/CMS REPLACEMENT HISTORY, Element STDCMAP.H */
/*  *1     5-APR-1989 10:24:59 FISHER "Standard Colormap Code" */
/*  DEC/CMS REPLACEMENT HISTORY, Element STDCMAP.H */
/* Defines for standard colormap routines */

#define ROUND(x)  ((unsigned short)((x) + 0.5))
#define COLORFIX(x) (((x) > 1.0) ? maxColor : ROUND((x) * maxColor))

typedef struct _fixedColor 
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
} fixedColor;

typedef struct _RGB
{
	float red;
	float green;
	float blue;
} RGB;

#define XMapRGBtoPixel(rgb,pixel,cmap) \
	pixel = cmap.base_pixel  \
                + (ROUND(rgb.red * cmap.red_max) * cmap.red_mult) \
                + (ROUND(rgb.green * cmap.green_max) * cmap.green_mult) \
                + (ROUND(rgb.blue * cmap.blue_max) * cmap.blue_mult) 

/*
#define XMapRGBtoPixel(rgb,pixel,cmap) \
	*pixel = cmap->base_pixel  \
                + (ROUND(rgb.red * cmap->red_max) * cmap->red_mult) \
                + (ROUND(rgb.green * cmap->green_max) * cmap->green_mult) \
                + (ROUND(rgb.blue * cmap->blue_max) * cmap->blue_mult)) 

*/
#define XMapColortoPixel(rgb,pixel,cmap) \
	{ \
	*pixel = cmap->base_pixel  \
                + (ROUND(rgb.red / 65535.0 * cmap->red_max) \
			* cmap->red_mult) \
                + (ROUND(rgb.green / 65535.0 * cmap->green_max) \
			* cmap->green_mult) \
                + (ROUND(rgb.blue / 65535.0 * cmap->blue_max) \
			* cmap->blue_mult)) \
	}

#ifdef STDCMAP
Atom XA_GRAY_DEFAULT_MAP;
Atom XA_GRAY_BEST_MAP;
int XSetUpStandardColormap();
#else
extern Atom XA_GRAY_DEFAULT_MAP;
extern Atom XA_GRAY_BEST_MAP;
extern int XSetUpStandardColormap();
#endif

