/* flag structure for banner routine */

typedef struct {
unsigned int	italic:1,	/* true - letters tilted right */
		dblwidth:1,	/* true - letters 2x as wide */
		halfspace:1;	/* true - add leading half space (centering) */
int		height;		/* number of lines per 'pixel' */
				/* (2 gives double height, 1 gives normal) */
char		bannerch;	/* char. to use for 'pixel.'  0 - print each */
				/* letter with the letter itself. */
} banflags;

extern banflags banFlagDefault;
