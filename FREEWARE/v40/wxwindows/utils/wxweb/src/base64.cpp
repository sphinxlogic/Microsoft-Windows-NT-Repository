// base64.cpp - written and placed in the public domain by Wei Dai

#include <iostream.h>

#include "astring.h"
#include "base64.h"

static const int MAX_LINE_LENGTH = 72;

static const unsigned char vec[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char padding = '=';

Base64Encoder::Base64Encoder(ostream& str, int lb) :
	ostr(str)
{
	line_breaks = lb;
	inBufSize=0;
	lineLength=0;
}

void Base64Encoder::LineBreak()
{
	ostr << '\n';
	lineLength=0;
}

void Base64Encoder::EncodeQuantum()
{
	unsigned char out;

	out=(unsigned char)((inBuf[0] & 0xFC) >> 2);
	ostr << vec[out];

	out=(unsigned char)(((inBuf[0] & 0x03) << 4) | (inBuf[1] >> 4));
	ostr << vec[out];

	out=(unsigned char)(((inBuf[1] & 0x0F) << 2) | (inBuf[2] >> 6));
	ostr << (inBufSize > 1 ? vec[out] : padding);

	out=(unsigned char)(inBuf[2] & 0x3F);
	ostr << (inBufSize > 2 ? vec[out] : padding);

	inBufSize=0;
	lineLength+=4;

	if (line_breaks && (lineLength>=MAX_LINE_LENGTH))
		LineBreak();
}

void Base64Encoder::Put(unsigned char inByte)
 {
	 inBuf[inBufSize++]=inByte;
	 if (inBufSize==3)
		EncodeQuantum();
 }

void Base64Encoder::Put(const unsigned char* inString, unsigned int len)
{
	while (len--)
		Base64Encoder::Put(*inString++);
}

void Base64Encoder::InputFinished()
{
	if (inBufSize)
	{
		for (int i=inBufSize;i<3;i++)
			inBuf[i]=0;

		EncodeQuantum();
	 }

	if (lineLength) // force a line break unless the current line is empty
		LineBreak();
}

Base64Decoder::Base64Decoder(ostream& str) :
	ostr(str)
{
	inBufSize=0;
}

void Base64Decoder::DecodeQuantum()
{
	unsigned char out;

	out = (unsigned char)((inBuf[0] << 2) | (inBuf[1] >> 4));
	ostr << out;

	out = (unsigned char)((inBuf[1] << 4) | (inBuf[2] >> 2));
	if (inBufSize > 2) ostr << out;

	out = (unsigned char)((inBuf[2] << 6) | inBuf[3]);
	if (inBufSize > 3) ostr << out;

	inBufSize=0;
}

int Base64Decoder::ConvToNumber(unsigned char inByte)
{
	if (inByte >= 'A' && inByte <= 'Z')
		return (inByte - 'A');

	if (inByte >= 'a' && inByte <= 'z')
		return (inByte - 'a' + 26);

	if (inByte >= '0' && inByte <= '9')
		return (inByte - '0' + 52);

	if (inByte == '+')
		return (62);

	if (inByte == '/')
		return (63);

	return (-1);
}

void Base64Decoder::Put(unsigned char inByte)
{
	int i=ConvToNumber(inByte);
	if (i >= 0)
		inBuf[inBufSize++]=(unsigned char) i;
	if (inBufSize==4)
		DecodeQuantum();
}

void Base64Decoder::Put(const unsigned char* inString, unsigned int len)
{
	while (len--)
		Base64Decoder::Put(*inString++);
}

void Base64Decoder::InputFinished()
{
	if (inBufSize)
	{
		for (int i=inBufSize;i<4;i++)
			inBuf[i]=0;

		DecodeQuantum();
	}
}

