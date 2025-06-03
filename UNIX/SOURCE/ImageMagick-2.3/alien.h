
/*
  Typedef declarations.
*/
typedef struct _AlienInfo
{
  char
    filename[2048];

  char
    *server_name,
    *font,
    *geometry,
    *density;

  unsigned int
    verbose;
} AlienInfo;

/*
  Alien image format routines.
*/
extern Image
  *ReadAlienImage _Declare((AlienInfo *));

extern unsigned int
  WriteAlienImage _Declare((Image *));

extern void
  GetAlienInfo _Declare((AlienInfo *));
