#define solidr_width 32
#define solidr_height 32
#define solidr_depth 1
static unsigned char solidr_bits[] =
{
  0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0xfc, 0xff,
  0xff, 0x3f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,
  0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff,
  0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0x01, 0x80, 0x7f,
  0xfe, 0x01, 0x80, 0x7f, 0xfe, 0x01, 0x80, 0x7f, 0xfe, 0xff,
  0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,
  0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff,
  0xff, 0x7f, 0xfe, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x1f,
  0xfe, 0x01, 0xff, 0x01, 0xfe, 0x01, 0xfe, 0x03, 0xfe, 0x01,
  0xfc, 0x07, 0xfe, 0x01, 0xf8, 0x0f, 0xfe, 0x01, 0xf0, 0x1f,
  0xfe, 0x01, 0xe0, 0x3f, 0xfe, 0x01, 0xc0, 0x7f, 0xfe, 0x01,
  0x80, 0x7f, 0xfe, 0x01, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3e,
  0x78, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00,
};
static unsigned long solidr_noswap[] =
{
  0x2,
  COLOR_LETTER_GOT,
};
static unsigned long solidr_swap[] =
{
  0x0,
  COLOR_LETTER_GOT,
};
