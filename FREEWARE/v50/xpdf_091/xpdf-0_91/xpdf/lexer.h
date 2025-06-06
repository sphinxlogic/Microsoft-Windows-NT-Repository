//========================================================================
//
// Lexer.h
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifndef LEXER_H
#define LEXER_H

#ifdef __GNUC__
#pragma interface
#endif

#include "Object.h"
#include "Stream.h"

#define tokBufSize 128		// size of token buffer

//------------------------------------------------------------------------
// Lexer
//------------------------------------------------------------------------

class Lexer {
public:

  // Construct a lexer for a single stream.  Deletes the stream when
  // lexer is deleted.
  Lexer(Stream *str);

  // Construct a lexer for a stream or array of streams (assumes obj
  // is either a stream or array of streams).
  Lexer(Object *obj);

  // Destructor.
  ~Lexer();

  // Get the next object from the input stream.
  Object *getObj(Object *obj);

  // Skip to the beginning of the next line in the input stream.
  void skipToNextLine();

  // Skip over one character.
  void skipChar() { getChar(); }

  // Get stream.
  Stream *getStream()
    { return curStr.isNone() ? (Stream *)NULL : curStr.getStream(); }

  // Get current position in file.
  int getPos()
    { return curStr.isNone() ? -1 : curStr.streamGetPos(); }

  // Set position in file.
  void setPos(int pos)
    { if (!curStr.isNone()) curStr.streamSetPos(pos); }

private:

  int getChar();
  int lookChar();

  Array *streams;		// array of input streams
  int strPtr;			// index of current stream
  Object curStr;		// current stream
  GBool freeArray;		// should lexer free the streams array?
  char tokBuf[tokBufSize];	// temporary token buffer
};

#endif
