/*
  Compress utility routines.
*/
extern unsigned int
  HuffmanEncodeImage _Declare((Image *)),
  LZWDecodeImage _Declare((Image *)),
  LZWEncodeImage _Declare((Image *,unsigned int)),
  QDecodeImage _Declare((unsigned char *,unsigned char *,unsigned int,
    unsigned int)),
  QEncodeImage _Declare((unsigned char *,unsigned char *,unsigned int,
    unsigned int));
