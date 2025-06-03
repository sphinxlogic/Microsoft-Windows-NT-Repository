#define ball_width 7
#define ball_height 14
#define ball_depth 2
static unsigned char ball_bits[] =
{
  0x00, 0x1c, 0x3e, 0x3e, 0x3e, 0x1c, 0x00, 0x1c, 0x22, 0x41,
  0x41, 0x41, 0x22, 0x1c,
};
static unsigned long ball_noswap[] =
{
  0x4,
  COLOR_BALL,
  COLOR_BLACK,
  ~(unsigned long)0,
};
static unsigned long ball_swap[] =
{
  0x0,
  COLOR_BALL,
  COLOR_BALL,
  ~(unsigned long)0,
};
