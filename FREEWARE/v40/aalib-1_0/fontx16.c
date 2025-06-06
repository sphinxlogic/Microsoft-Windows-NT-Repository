
#include "config.h"
#include "aalib.h"
/* binary image of font8x16 follows */

unsigned char fontX16data[] =
{

    0x01,
    0x03, 0x00, 0x02, 0x01, 0x03, 0x00, 0x02, 0x01,
    0x03, 0x00, 0x0b, 0x05, 0x02, 0x01, 0x10, 0x10,
    0x38, 0x38, 0x7c, 0x7c, 0xfe, 0xfe, 0x7c, 0x7c,
    0x38, 0x38, 0x10, 0x10, 0x00, 0x00, 0x92, 0x92,
    0x44, 0x44, 0x92, 0x92, 0x44, 0x44, 0x92, 0x92,
    0x44, 0x44, 0x92, 0x92, 0x00, 0x00, 0x00, 0x88,
    0x88, 0x88, 0xf8, 0x88, 0x88, 0x88, 0x00, 0x3e,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0xf8,
    0x80, 0x80, 0xf0, 0x80, 0x80, 0x80, 0x3e, 0x20,
    0x20, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00, 0x70,
    0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, 0x3c,
    0x22, 0x22, 0x3c, 0x28, 0x24, 0x22, 0x00, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x00, 0x3e,
    0x20, 0x20, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x10, 0x10, 0x10, 0xfe, 0x10, 0x10, 0x10,
    0x10, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x84,
    0xc4, 0xa4, 0xa4, 0x94, 0x94, 0x8c, 0x84, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x3e, 0x00, 0x00,
    0x88, 0x88, 0x88, 0x50, 0x50, 0x20, 0x00, 0x3e,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xf0, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x0c, 0x30, 0xc0, 0x30, 0x0c,
    0x02, 0xfe, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x60, 0x18, 0x06, 0x18, 0x60,
    0x80, 0xfe, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x24, 0x24,
    0x24, 0x24, 0x44, 0x84, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x04, 0x08, 0xfe, 0x10, 0xfe,
    0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0c, 0x12, 0x10, 0x10, 0x10, 0x7c,
    0x10, 0x10, 0x3c, 0x52, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38,
    0x38, 0x38, 0x38, 0x38, 0x38, 0x10, 0x10, 0x10,
    0x10, 0x00, 0x00, 0x10, 0x38, 0x10, 0x6c, 0x6c,
    0x24, 0x24, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12,
    0x12, 0x12, 0x7f, 0x24, 0x24, 0x24, 0x24, 0x24,
    0xfe, 0x48, 0x48, 0x48, 0x48, 0x00, 0x10, 0x38,
    0x54, 0x92, 0x96, 0x90, 0x50, 0x38, 0x14, 0x12,
    0xd2, 0x92, 0x94, 0x78, 0x10, 0x10, 0x02, 0x62,
    0x94, 0x94, 0x94, 0x98, 0x68, 0x10, 0x10, 0x2c,
    0x32, 0x52, 0x52, 0x52, 0x8c, 0x80, 0x00, 0x30,
    0x48, 0x48, 0x48, 0x50, 0x20, 0x2e, 0x54, 0x54,
    0x94, 0x88, 0x8c, 0x72, 0x00, 0x00, 0xe0, 0xe0,
    0x20, 0x20, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04,
    0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x08, 0x08, 0x04, 0x02, 0x80, 0x40,
    0x20, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x20, 0x20, 0x40, 0x80, 0x00, 0x00,
    0x00, 0x10, 0x38, 0x92, 0xd6, 0x38, 0xd6, 0x92,
    0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x10, 0x10, 0x10, 0xfe, 0x10, 0x10,
    0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe0, 0xe0, 0x20, 0x20, 0xc0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x40, 0xe0, 0xe0, 0x40, 0x00, 0x02, 0x02,
    0x04, 0x04, 0x08, 0x08, 0x08, 0x10, 0x10, 0x10,
    0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x00, 0x18,
    0x24, 0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x24, 0x24, 0x18, 0x00, 0x00, 0x10,
    0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x42, 0x62, 0x02, 0x04, 0x08, 0x08, 0x10,
    0x20, 0x22, 0x42, 0x7e, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x82, 0x82, 0x02, 0x04, 0x38, 0x04, 0x02,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x08,
    0x18, 0x28, 0x28, 0x48, 0x48, 0x88, 0x88, 0xfe,
    0x08, 0x08, 0x08, 0x3c, 0x00, 0x00, 0x00, 0xfc,
    0x80, 0x80, 0x80, 0xb8, 0xc4, 0x82, 0x02, 0x02,
    0xc2, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x3c,
    0x42, 0x46, 0x80, 0x80, 0xb8, 0xc4, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0xfe,
    0x82, 0x82, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08,
    0x08, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x38,
    0x44, 0x82, 0x82, 0x82, 0x44, 0x38, 0x44, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x82, 0x82, 0x82, 0x82, 0x46, 0x3a, 0x02,
    0x02, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00,
    0x00, 0x38, 0x38, 0x18, 0x10, 0x30, 0x02, 0x04,
    0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x10,
    0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0xfe,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40,
    0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x10,
    0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x00, 0x38,
    0x44, 0x82, 0xc2, 0x02, 0x04, 0x04, 0x08, 0x10,
    0x10, 0x00, 0x00, 0x10, 0x38, 0x10, 0x00, 0x3c,
    0x42, 0x82, 0x9a, 0xa6, 0xa2, 0xa2, 0xa2, 0xa6,
    0x9a, 0x80, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x10,
    0x28, 0x28, 0x28, 0x44, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x00, 0xf8,
    0x44, 0x42, 0x42, 0x42, 0x44, 0x78, 0x44, 0x42,
    0x42, 0x42, 0x42, 0xfc, 0x00, 0x00, 0x00, 0x3a,
    0x46, 0x42, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x82, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x00, 0xf8,
    0x44, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x44, 0x44, 0xf8, 0x00, 0x00, 0x00, 0xfe,
    0x42, 0x42, 0x40, 0x48, 0x48, 0x78, 0x48, 0x48,
    0x42, 0x42, 0x42, 0xfe, 0x00, 0x00, 0x00, 0xfe,
    0x42, 0x42, 0x40, 0x48, 0x48, 0x78, 0x48, 0x48,
    0x40, 0x40, 0x40, 0xf0, 0x00, 0x00, 0x00, 0x1a,
    0x26, 0x42, 0x40, 0x80, 0x80, 0x8f, 0x82, 0x82,
    0x82, 0x42, 0x66, 0x1a, 0x00, 0x00, 0x00, 0xe7,
    0x42, 0x42, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0xe7, 0x00, 0x00, 0x00, 0xfe,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0xfe, 0x00, 0x00, 0x00, 0x1f,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0xe6,
    0x44, 0x44, 0x48, 0x48, 0x70, 0x50, 0x48, 0x48,
    0x44, 0x44, 0x42, 0xe3, 0x00, 0x00, 0x00, 0xf0,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x42, 0x42, 0x42, 0xfe, 0x00, 0x00, 0x00, 0x82,
    0xc6, 0xaa, 0xaa, 0xaa, 0x92, 0x92, 0x92, 0x82,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x00, 0x87,
    0xc2, 0xa2, 0xa2, 0xa2, 0x92, 0x92, 0x92, 0x8a,
    0x8a, 0x8a, 0x86, 0xc2, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0xf8,
    0x44, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78, 0x40,
    0x40, 0x40, 0x40, 0xf0, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82,
    0xba, 0x44, 0x44, 0x38, 0x08, 0x06, 0x00, 0xf8,
    0x44, 0x42, 0x42, 0x42, 0x44, 0x78, 0x48, 0x44,
    0x44, 0x44, 0x42, 0xe3, 0x00, 0x00, 0x00, 0x34,
    0x4c, 0x84, 0x80, 0x80, 0x60, 0x18, 0x04, 0x82,
    0x82, 0x82, 0xc4, 0xb8, 0x00, 0x00, 0x00, 0xfe,
    0x92, 0x92, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0xe7,
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x00, 0xc6,
    0x82, 0x82, 0x82, 0x82, 0x44, 0x44, 0x44, 0x44,
    0x28, 0x28, 0x10, 0x10, 0x00, 0x00, 0x00, 0xc6,
    0x82, 0x82, 0x82, 0x92, 0x92, 0x92, 0xaa, 0xaa,
    0xaa, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0xee,
    0x44, 0x44, 0x28, 0x28, 0x10, 0x28, 0x28, 0x28,
    0x44, 0x44, 0x82, 0xc6, 0x00, 0x00, 0x00, 0xc6,
    0x82, 0x44, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0xfe,
    0x84, 0x88, 0x08, 0x10, 0x10, 0x10, 0x20, 0x20,
    0x42, 0x42, 0x82, 0xfe, 0x00, 0x00, 0x1e, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x1e, 0x00, 0x80,
    0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08,
    0x08, 0x04, 0x04, 0x02, 0x02, 0x00, 0xf0, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0xf0, 0x10, 0x28,
    0x44, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x30, 0x30,
    0x20, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0xc0,
    0x40, 0x40, 0x40, 0x78, 0x44, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x44, 0x78, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3a, 0x46, 0x82, 0x80, 0x80,
    0x80, 0x82, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x06,
    0x04, 0x04, 0x04, 0x3c, 0x44, 0x84, 0x84, 0x84,
    0x84, 0x84, 0x44, 0x3e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0xfe, 0x80,
    0x80, 0x82, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x0e,
    0x11, 0x10, 0x10, 0xfe, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3b, 0x44, 0x44, 0x44, 0x38,
    0x40, 0x78, 0x84, 0x82, 0x82, 0x7c, 0x00, 0xc0,
    0x40, 0x40, 0x40, 0x5c, 0x62, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0xe7, 0x00, 0x00, 0x18, 0x18,
    0x00, 0x00, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x06, 0x06,
    0x00, 0x00, 0x00, 0x3e, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x82, 0x82, 0x44, 0x38, 0x00, 0xc0,
    0x40, 0x40, 0x40, 0x42, 0x44, 0x48, 0x58, 0x64,
    0x44, 0x42, 0x42, 0xe3, 0x00, 0x00, 0x00, 0x78,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x6c, 0x92, 0x92, 0x92, 0x92,
    0x92, 0x92, 0x92, 0xdb, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xdc, 0x62, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0xe7, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x38, 0x44, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xf8, 0x44, 0x42, 0x42, 0x42,
    0x42, 0x44, 0x78, 0x40, 0x40, 0xf0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3e, 0x44, 0x84, 0x84, 0x84,
    0x84, 0x44, 0x3c, 0x04, 0x04, 0x1e, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xec, 0x32, 0x22, 0x20, 0x20,
    0x20, 0x20, 0x20, 0xfc, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3a, 0x46, 0x42, 0x40, 0x3c,
    0x02, 0x82, 0xc2, 0xbc, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x20, 0x20, 0xfc, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc6, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x46, 0x39, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xc6, 0x82, 0x82, 0x44, 0x44,
    0x44, 0x28, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x92, 0x92, 0x92, 0x92, 0xaa,
    0xaa, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xee, 0x44, 0x28, 0x28, 0x10,
    0x28, 0x28, 0x44, 0xee, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xe7, 0x42, 0x22, 0x24, 0x14,
    0x08, 0x08, 0x10, 0x90, 0xa0, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x7e, 0x44, 0x08, 0x08, 0x10,
    0x10, 0x22, 0x42, 0xfe, 0x00, 0x00, 0x06, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x10, 0x20, 0x10, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x06, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xc0, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x10, 0x08, 0x10, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0xc0, 0x60, 0x92,
    0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x38, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10,
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x00, 0x00,
    0x10, 0x3c, 0x52, 0x92, 0x90, 0x90, 0x90, 0x90,
    0x90, 0x92, 0x52, 0x3c, 0x10, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0c, 0x12, 0x10, 0x10, 0x10, 0x7c,
    0x10, 0x10, 0x3c, 0x52, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x42, 0x3c, 0x24, 0x24, 0x3c,
    0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x82,
    0x44, 0x44, 0xfe, 0x28, 0x10, 0xfe, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x3c,
    0x42, 0x40, 0x20, 0x3c, 0x42, 0x42, 0x42, 0x3c,
    0x04, 0x02, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x24,
    0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,
    0x42, 0x99, 0xa5, 0xc3, 0xc1, 0xc1, 0xc1, 0xc3,
    0xa5, 0x99, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x1c,
    0x22, 0x1e, 0x22, 0x26, 0x1a, 0x00, 0x3e, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x09, 0x12, 0x24, 0x48, 0x90, 0x90, 0x48,
    0x24, 0x12, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x06,
    0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,
    0x42, 0x81, 0xf9, 0xc5, 0xc5, 0xf9, 0xc5, 0xc5,
    0xc5, 0x81, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x10, 0x10, 0xfe, 0x10, 0x10,
    0x10, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x04, 0x08, 0x10, 0x20, 0x3c, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x04, 0x18, 0x04, 0x24, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c,
    0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22,
    0x44, 0x44, 0x6c, 0x54, 0x82, 0x80, 0x00, 0x7f,
    0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0x7a, 0x0a, 0x0a,
    0x0a, 0x0a, 0x0a, 0x1b, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0x08, 0x10, 0x00, 0x08,
    0x18, 0x28, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x24, 0x24, 0x24, 0x18, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x90, 0x48, 0x24, 0x12, 0x09, 0x09,
    0x12, 0x24, 0x48, 0x90, 0x00, 0x00, 0x00, 0x40,
    0xc0, 0x40, 0x40, 0x41, 0x46, 0x08, 0x32, 0xc6,
    0x0a, 0x12, 0x1f, 0x02, 0x00, 0x00, 0x00, 0x40,
    0xc0, 0x40, 0x40, 0x41, 0x46, 0x08, 0x36, 0xc9,
    0x01, 0x06, 0x08, 0x0f, 0x00, 0x00, 0x00, 0x60,
    0x90, 0x10, 0x60, 0x11, 0x96, 0x68, 0x32, 0xc6,
    0x0a, 0x12, 0x1f, 0x02, 0x00, 0x00, 0x08, 0x1c,
    0x08, 0x00, 0x08, 0x08, 0x08, 0x10, 0x20, 0x20,
    0x40, 0x43, 0x41, 0x22, 0x1c, 0x00, 0x30, 0x10,
    0x08, 0x10, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x18, 0x10,
    0x20, 0x10, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x10, 0x28,
    0x44, 0x10, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x24, 0x58,
    0x00, 0x10, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x44, 0x44,
    0x00, 0x10, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x38, 0x44,
    0x38, 0x10, 0x28, 0x28, 0x44, 0x44, 0x44, 0x7c,
    0x82, 0x82, 0x82, 0xc6, 0x00, 0x00, 0x00, 0x00,
    0x1f, 0x28, 0x28, 0x48, 0x48, 0x48, 0x4e, 0xf8,
    0x88, 0x88, 0x88, 0x8f, 0x00, 0x00, 0x00, 0x3c,
    0x42, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x40, 0x42, 0x3c, 0x08, 0x08, 0x10, 0x30, 0x10,
    0x08, 0xfe, 0x42, 0x42, 0x40, 0x48, 0x78, 0x48,
    0x40, 0x42, 0x42, 0xfe, 0x00, 0x00, 0x18, 0x10,
    0x20, 0xfe, 0x42, 0x42, 0x40, 0x48, 0x78, 0x48,
    0x40, 0x42, 0x42, 0xfe, 0x00, 0x00, 0x30, 0x48,
    0x00, 0xfe, 0x42, 0x42, 0x40, 0x48, 0x78, 0x48,
    0x40, 0x42, 0x42, 0xfe, 0x00, 0x00, 0x44, 0x44,
    0x00, 0xfe, 0x42, 0x42, 0x40, 0x48, 0x78, 0x48,
    0x40, 0x42, 0x42, 0xfe, 0x00, 0x00, 0x30, 0x10,
    0x08, 0xfe, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0xfe, 0x00, 0x00, 0x18, 0x10,
    0x20, 0xfe, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0xfe, 0x00, 0x00, 0x10, 0x28,
    0x44, 0xfe, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0xfe, 0x00, 0x00, 0x44, 0x44,
    0x00, 0xfe, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0xfe, 0x00, 0x00, 0x00, 0xf8,
    0x44, 0x44, 0x42, 0x42, 0x42, 0xf2, 0x42, 0x42,
    0x42, 0x44, 0x44, 0xf8, 0x00, 0x00, 0x24, 0x58,
    0x00, 0x87, 0xc2, 0xa2, 0xa2, 0x92, 0x92, 0x92,
    0x8a, 0x8a, 0x86, 0xc2, 0x00, 0x00, 0x30, 0x10,
    0x08, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x18, 0x10,
    0x20, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x38, 0x44,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x24, 0x58,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x44, 0x44,
    0x00, 0x38, 0x44, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x81, 0x42, 0x24, 0x18, 0x18, 0x24,
    0x42, 0x81, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02,
    0x3c, 0x44, 0x86, 0x8a, 0x8a, 0x92, 0x92, 0xa2,
    0xa2, 0xc2, 0x44, 0x78, 0x80, 0x80, 0x30, 0x10,
    0x08, 0xe7, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x0c, 0x08,
    0x10, 0xe7, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x18, 0x24,
    0x00, 0xe7, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x24, 0x24,
    0x00, 0xe7, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x3c, 0x00, 0x00, 0x0c, 0x08,
    0x10, 0xc6, 0x82, 0x44, 0x44, 0x28, 0x28, 0x10,
    0x10, 0x10, 0x10, 0x7c, 0x00, 0x00, 0xe0, 0x40,
    0x78, 0x44, 0x42, 0x42, 0x42, 0x44, 0x78, 0x40,
    0x40, 0x40, 0x40, 0xf0, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x82, 0x82, 0x82, 0x84, 0x98, 0x84, 0x82,
    0x82, 0x82, 0x82, 0x9c, 0x00, 0x00, 0x00, 0x30,
    0x10, 0x08, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0x18,
    0x10, 0x20, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x42, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0x32,
    0x4c, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0x00,
    0x24, 0x24, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x18, 0x00, 0x3c, 0x42, 0x02, 0x3e, 0x42,
    0x82, 0x82, 0x86, 0x7b, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x6c, 0x92, 0x12, 0x3e, 0x50,
    0x90, 0x92, 0x92, 0x6c, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3a, 0x46, 0x82, 0x80, 0x80, 0x80,
    0x82, 0x42, 0x3c, 0x10, 0x10, 0x20, 0x00, 0x30,
    0x10, 0x08, 0x00, 0x38, 0x44, 0x82, 0xfe, 0x80,
    0x80, 0x82, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x18,
    0x10, 0x20, 0x00, 0x38, 0x44, 0x82, 0xfe, 0x80,
    0x80, 0x82, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x38,
    0x44, 0x82, 0x00, 0x38, 0x44, 0x82, 0xfe, 0x80,
    0x80, 0x82, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x44,
    0x44, 0x00, 0x00, 0x38, 0x44, 0x82, 0xfe, 0x80,
    0x80, 0x80, 0x44, 0x38, 0x00, 0x00, 0x00, 0x30,
    0x10, 0x08, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x00, 0x18,
    0x10, 0x20, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x00, 0x18,
    0x24, 0x42, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x00, 0x24,
    0x24, 0x00, 0x00, 0x78, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0xff, 0x00, 0x00, 0x00, 0x64,
    0x18, 0x18, 0x24, 0x3c, 0x42, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x32,
    0x4c, 0x00, 0x00, 0xdc, 0x62, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0xe7, 0x00, 0x00, 0x00,
};
struct aa_font fontX16 =
{fontX16data, 16, "Standard X 8x16 font", "X8x16"};
