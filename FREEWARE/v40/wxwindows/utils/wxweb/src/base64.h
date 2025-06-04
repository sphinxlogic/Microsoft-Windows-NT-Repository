#ifndef BASE64_H
#define BASE64_H

#ifndef string_H
#include "string.h"
#endif

class Base64Encoder
{
public:

	 Base64Encoder(ostream&, int line_breaks = 1);

	 void Put(const unsigned char* inString, unsigned int length);
	 void InputFinished();

private:

	 void Put(unsigned char inByte);
	 void LineBreak();
	 void EncodeQuantum();

	 int line_breaks;
	 int inBufSize;
	 int lineLength;
	 unsigned char inBuf[3];
	 ostream& ostr;
};

class Base64Decoder
{
public:

	 Base64Decoder(ostream&);

	 void Put(const unsigned char* inString, unsigned int length);
	 void InputFinished();

private:

	 static int ConvToNumber(unsigned char inByte);
	 void DecodeQuantum();
	 void Put(unsigned char inByte);

	 int inBufSize;
	 unsigned char inBuf[4];
	 ostream& ostr;
};

#endif

