#define hnormf_width 32
#define hnormf_height 24
#define hnormf_depth 3
static unsigned char hnormf_bits[] =
{
  0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x00, 0xbe, 0xaa,
  0x86, 0x3f, 0xf8, 0x55, 0xfd, 0x7d, 0xfe, 0xaf, 0xaa, 0x0e,
  0x5e, 0x55, 0xd5, 0x7f, 0xf8, 0xff, 0xff, 0x79, 0x80, 0x7f,
  0xf8, 0x03, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0x00,
  0x3e, 0x00, 0xfc, 0x3f, 0xf8, 0x01, 0xfc, 0x78, 0xfe, 0x0f,
  0x00, 0x0c, 0x1e, 0x00, 0x80, 0x7f, 0xf8, 0xff, 0xff, 0x79,
  0x80, 0x7f, 0xf8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x40, 0x55, 0x01, 0x00, 0x00, 0xaa, 0x02, 0x02,
  0x00, 0x50, 0x55, 0x01, 0xa0, 0xaa, 0x2a, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static unsigned long hnormf_noswap[] =
{
  0xa,
  COLOR_NORMAL,
  COLOR_WHITE,
  COLOR_BLACK,
  COLOR_NORMAL,
  ~(unsigned long)0,
  ~(unsigned long)0,
  ~(unsigned long)0,
};
static unsigned long hnormf_swap[] =
{
  0x14,
  COLOR_NORMAL,
  COLOR_BLACK,
  COLOR_WHITE,
  COLOR_NORMAL,
  ~(unsigned long)0,
  ~(unsigned long)0,
  ~(unsigned long)0,
};
