#define clock_width 32
#define clock_height 96
#define clock_depth 3
static unsigned char clock_bits[] =
{
  0x00, 0x00, 0x00, 0x00, 0x90, 0x03, 0xc0, 0x09, 0xe0, 0x07,
  0xe7, 0x07, 0xf0, 0x0f, 0xf7, 0x0f, 0xf8, 0x07, 0xe2, 0x1f,
  0xf8, 0x01, 0x80, 0x1f, 0xf8, 0x01, 0x80, 0x1f, 0x38, 0xf0,
  0x0f, 0x1c, 0x10, 0x7c, 0x3e, 0x08, 0x00, 0x76, 0x6e, 0x00,
  0x00, 0xff, 0xe7, 0x00, 0x80, 0xff, 0xe3, 0x01, 0x80, 0xfe,
  0x73, 0x01, 0xc0, 0xff, 0xf1, 0x03, 0xc0, 0xe1, 0xf9, 0x03,
  0xc0, 0xc3, 0xfd, 0x03, 0x40, 0x06, 0x7e, 0x02, 0x40, 0x7e,
  0x7e, 0x02, 0xc0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x03,
  0xc0, 0xff, 0xff, 0x03, 0x80, 0xfe, 0x7f, 0x01, 0x80, 0xff,
  0xff, 0x01, 0x00, 0xff, 0xff, 0x00, 0x00, 0x76, 0x6e, 0x00,
  0x00, 0x7c, 0x3e, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0xc0, 0x01,
  0xe0, 0x07, 0xe2, 0x07, 0xf0, 0x01, 0x80, 0x0f, 0xf0, 0xf0,
  0x0d, 0x0f, 0x30, 0x0c, 0x30, 0x0c, 0x10, 0xf3, 0xcf, 0x08,
  0x80, 0x8c, 0x31, 0x01, 0x40, 0x8a, 0x51, 0x02, 0x40, 0x01,
  0x98, 0x02, 0xa0, 0x00, 0x1c, 0x05, 0xa0, 0x01, 0x8c, 0x05,
  0x50, 0x00, 0x0e, 0x0a, 0x50, 0x1e, 0x06, 0x0a, 0x50, 0x3c,
  0x02, 0x0a, 0xd0, 0xf9, 0x81, 0x0b, 0xd0, 0x81, 0x81, 0x0b,
  0x50, 0x00, 0x00, 0x0a, 0x50, 0x00, 0x00, 0x0a, 0x50, 0x00,
  0x00, 0x0a, 0xa0, 0x01, 0x80, 0x05, 0xa0, 0x00, 0x00, 0x05,
  0x40, 0x01, 0x80, 0x02, 0x40, 0x8a, 0x51, 0x02, 0x80, 0x8c,
  0x31, 0x01, 0x00, 0xf1, 0x8f, 0x00, 0x00, 0x0d, 0xb0, 0x00,
  0x00, 0x73, 0xce, 0x00, 0x80, 0x81, 0x81, 0x01, 0x80, 0x80,
  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x90, 0x03, 0xc0, 0x09, 0x60, 0x04, 0x27, 0x06, 0x10, 0x08,
  0x15, 0x08, 0x08, 0x06, 0x62, 0x10, 0x08, 0xf1, 0x8f, 0x10,
  0xc8, 0xfd, 0xbf, 0x13, 0x28, 0xff, 0xff, 0x14, 0x90, 0x0f,
  0xf0, 0x09, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x01, 0x80, 0x03,
  0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0x70, 0x00,
  0x00, 0x0e, 0x70, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x0e,
  0x70, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x0e, 0x70, 0x00,
  0x00, 0x0e, 0x70, 0x00, 0x00, 0x0e, 0x70, 0x00, 0x00, 0x0e,
  0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xc0, 0x01,
  0x80, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0x80, 0x0f, 0xf0, 0x01,
  0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xf3,
  0xcf, 0x00, 0x80, 0x81, 0x81, 0x01, 0x80, 0x80, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00,
};
static unsigned long clock_noswap[] =
{
  0x7c,
  COLOR_CLOCK_FACE,
  COLOR_BLACK,
  COLOR_CLOCK_BELL,
  COLOR_CLOCK_EDGE,
  COLOR_BLACK,
  COLOR_BLACK,
  COLOR_CLOCK_EDGE,
};
static unsigned long clock_swap[] =
{
  0x7c,
  COLOR_CLOCK_FACE,
  COLOR_BLACK,
  COLOR_CLOCK_BELL,
  COLOR_CLOCK_EDGE,
  COLOR_CLOCK_BELL,
  COLOR_CLOCK_EDGE,
  COLOR_CLOCK_EDGE,
};
