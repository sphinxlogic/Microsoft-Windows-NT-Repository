//========================================================================
//
// XRef.cc
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "gmem.h"
#include "Object.h"
#include "Stream.h"
#include "Lexer.h"
#include "Parser.h"
#include "Dict.h"
#ifndef NO_DECRYPTION
#include "Decrypt.h"
#endif
#include "Error.h"
#include "XRef.h"

//------------------------------------------------------------------------

#define xrefSearchSize 1024	// read this many bytes at end of file
				//   to look for 'startxref'

#ifndef NO_DECRYPTION
//------------------------------------------------------------------------
// Permission bits
//------------------------------------------------------------------------

#define permPrint    (1<<2)
#define permChange   (1<<3)
#define permCopy     (1<<4)
#define permNotes    (1<<5)
#define defPermFlags 0xfffc
#endif

//------------------------------------------------------------------------
// The global xref table
//------------------------------------------------------------------------

XRef *xref = NULL;

//------------------------------------------------------------------------
// XRef
//------------------------------------------------------------------------

XRef::XRef(BaseStream *str, GString *userPassword) {
  XRef *oldXref;
  int pos;
  int i;

  ok = gTrue;
  size = 0;
  entries = NULL;

  // get rid of old xref (otherwise it will try to fetch the Root object
  // in the new document, using the old xref)
  oldXref = xref;
  xref = NULL;

  // read the trailer
  this->str = str;
  start = str->getStart();
  pos = readTrailer();

  // if there was a problem with the trailer,
  // try to reconstruct the xref table
  if (pos == 0) {
    if (!(ok = constructXRef())) {
      xref = oldXref;
      return;
    }

  // trailer is ok - read the xref table
  } else {
    entries = (XRefEntry *)gmalloc(size * sizeof(XRefEntry));
    for (i = 0; i < size; ++i) {
      entries[i].offset = -1;
      entries[i].used = gFalse;
    }
    while (readXRef(&pos)) ;

    // if there was a problem with the xref table,
    // try to reconstruct it
    if (!ok) {
      gfree(entries);
      size = 0;
      entries = NULL;
      if (!(ok = constructXRef())) {
	xref = oldXref;
	return;
      }
    }
  }

  // set up new xref table
  xref = this;

  // check for encryption
#ifndef NO_DECRYPTION
  encrypted = gFalse;
#endif
  if (checkEncrypted(userPassword)) {
    ok = gFalse;
    xref = oldXref;
    return;
  }
}

XRef::~XRef() {
  gfree(entries);
  trailerDict.free();
}

// Read startxref position, xref table size, and root.  Returns
// first xref position.
int XRef::readTrailer() {
  Parser *parser;
  Object obj;
  char buf[xrefSearchSize+1];
  int n, pos, pos1;
  char *p;
  int c;
  int i;

  // read last xrefSearchSize bytes
  str->setPos(-xrefSearchSize);
  for (n = 0; n < xrefSearchSize; ++n) {
    if ((c = str->getChar()) == EOF)
      break;
    buf[n] = c;
  }
  buf[n] = '\0';

  // find startxref
  for (i = n - 9; i >= 0; --i) {
    if (!strncmp(&buf[i], "startxref", 9))
      break;
  }
  if (i < 0)
    return 0;
  for (p = &buf[i+9]; isspace(*p); ++p) ;
  pos = lastXRefPos = atoi(p);

  // find trailer dict by looking after first xref table
  // (NB: we can't just use the trailer dict at the end of the file --
  // this won't work for linearized files.)
  str->setPos(start + pos);
  for (i = 0; i < 4; ++i)
    buf[i] = str->getChar();
  if (strncmp(buf, "xref", 4))
    return 0;
  pos1 = pos + 4;
  while (1) {
    str->setPos(start + pos1);
    for (i = 0; i < 35; ++i) {
      if ((c = str->getChar()) == EOF)
	return 0;
      buf[i] = c;
    }
    if (!strncmp(buf, "trailer", 7))
      break;
    p = buf;
    while (isspace(*p)) ++p;
    while ('0' <= *p && *p <= '9') ++p;
    while (isspace(*p)) ++p;
    n = atoi(p);
    while ('0' <= *p && *p <= '9') ++p;
    while (isspace(*p)) ++p;
    if (p == buf)
      return 0;
    pos1 += (p - buf) + n * 20;
  }
  pos1 += 7;

  // read trailer dict
  obj.initNull();
  parser = new Parser(new Lexer(str->makeSubStream(start + pos1, -1, &obj)));
  parser->getObj(&trailerDict);
  if (trailerDict.isDict()) {
    trailerDict.dictLookupNF("Size", &obj);
    if (obj.isInt())
      size = obj.getInt();
    else
      pos = 0;
    obj.free();
    trailerDict.dictLookupNF("Root", &obj);
    if (obj.isRef()) {
      rootNum = obj.getRefNum();
      rootGen = obj.getRefGen();
    } else {
      pos = 0;
    }
    obj.free();
  } else {
    pos = 0;
  }
  delete parser;

  // return first xref position
  return pos;
}

// Read an xref table and the prev pointer from the trailer.
GBool XRef::readXRef(int *pos) {
  Parser *parser;
  Object obj, obj2;
  char s[20];
  GBool more;
  int first, n, i, j;
  int c;

  // seek to xref in stream
  str->setPos(start + *pos);

  // make sure it's an xref table
  while ((c = str->getChar()) != EOF && isspace(c)) ;
  s[0] = (char)c;
  s[1] = (char)str->getChar();
  s[2] = (char)str->getChar();
  s[3] = (char)str->getChar();
  if (!(s[0] == 'x' && s[1] == 'r' && s[2] == 'e' && s[3] == 'f'))
    goto err2;

  // read xref
  while (1) {
    while ((c = str->lookChar()) != EOF && isspace(c))
      str->getChar();
    if (c == 't')
      break;
    for (i = 0; (c = str->getChar()) != EOF && isdigit(c) && i < 20; ++i)
      s[i] = (char)c;
    if (i == 0)
      goto err2;
    s[i] = '\0';
    first = atoi(s);
    while ((c = str->lookChar()) != EOF && isspace(c))
      str->getChar();
    for (i = 0; (c = str->getChar()) != EOF && isdigit(c) && i < 20; ++i)
      s[i] = (char)c;
    if (i == 0)
      goto err2;
    s[i] = '\0';
    n = atoi(s);
    while ((c = str->lookChar()) != EOF && isspace(c))
      str->getChar();
    for (i = first; i < first + n; ++i) {
      for (j = 0; j < 20; ++j) {
	if ((c = str->getChar()) == EOF)
	  goto err2;
	s[j] = (char)c;
      }
      if (entries[i].offset < 0) {
	s[10] = '\0';
	entries[i].offset = atoi(s);
	s[16] = '\0';
	entries[i].gen = atoi(&s[11]);
	if (s[17] == 'n')
	  entries[i].used = gTrue;
	else if (s[17] == 'f')
	  entries[i].used = gFalse;
	else
	  goto err2;
#if 1 //~
	//~ PDF files of patents from the IBM Intellectual Property
	//~ Network have a bug: the xref table claims to start at 1
	//~ instead of 0.
	if (i == 1 && first == 1 &&
	    entries[1].offset == 0 && entries[1].gen == 65535 &&
	    !entries[1].used) {
	  i = first = 0;
	  entries[0] = entries[1];
	  entries[1].offset = -1;
	}
#endif
      }
    }
  }

  // read prev pointer from trailer dictionary
  obj.initNull();
  parser = new Parser(new Lexer(str->makeSubStream(str->getPos(), -1, &obj)));
  parser->getObj(&obj);
  if (!obj.isCmd("trailer"))
    goto err1;
  obj.free();
  parser->getObj(&obj);
  if (!obj.isDict())
    goto err1;
  obj.getDict()->lookupNF("Prev", &obj2);
  if (obj2.isInt()) {
    *pos = obj2.getInt();
    more = gTrue;
  } else {
    more = gFalse;
  }
  obj.free();
  obj2.free();

  delete parser;
  return more;

 err1:
  obj.free();
 err2:
  ok = gFalse;
  return gFalse;
}

// Attempt to construct an xref table for a damaged file.
GBool XRef::constructXRef() {
  Parser *parser;
  Object obj;
  char buf[256];
  int pos;
  int num, gen;
  int newSize;
  char *p;
  int i;
  GBool gotRoot;

  error(0, "PDF file is damaged - attempting to reconstruct xref table...");
  gotRoot = gFalse;

  str->reset();
  while (1) {
    pos = str->getPos();
    if (!str->getLine(buf, 256))
      break;
    p = buf;

    // got trailer dictionary
    if (!strncmp(p, "trailer", 7)) {
      obj.initNull();
      parser = new Parser(new Lexer(
		      str->makeSubStream(start + pos + 8, -1, &obj)));
      if (!trailerDict.isNone())
	trailerDict.free();
      parser->getObj(&trailerDict);
      if (trailerDict.isDict()) {
	trailerDict.dictLookupNF("Root", &obj);
	if (obj.isRef()) {
	  rootNum = obj.getRefNum();
	  rootGen = obj.getRefGen();
	  gotRoot = gTrue;
	}
	obj.free();
      } else {
	pos = 0;
      }
      delete parser;

    // look for object
    } else if (isdigit(*p)) {
      num = atoi(p);
      do {
	++p;
      } while (*p && isdigit(*p));
      if (isspace(*p)) {
	do {
	  ++p;
	} while (*p && isspace(*p));
	if (isdigit(*p)) {
	  gen = atoi(p);
	  do {
	    ++p;
	  } while (*p && isdigit(*p));
	  if (isspace(*p)) {
	    do {
	      ++p;
	    } while (*p && isspace(*p));
	    if (!strncmp(p, "obj", 3)) {
	      if (num >= size) {
		newSize = (num + 1 + 255) & ~255;
		entries = (XRefEntry *)
		            grealloc(entries, newSize * sizeof(XRefEntry));
		for (i = size; i < newSize; ++i) {
		  entries[i].offset = -1;
		  entries[i].used = gFalse;
		}
		size = newSize;
	      }
	      if (!entries[num].used || gen >= entries[num].gen) {
		entries[num].offset = pos - start;
		entries[num].gen = gen;
		entries[num].used = gTrue;
	      }
	    }
	  }
	}
      }
    }
  }

  if (gotRoot)
    return gTrue;

  error(-1, "Couldn't find trailer dictionary");
  return gFalse;
}

#ifndef NO_DECRYPTION
GBool XRef::checkEncrypted(GString *userPassword) {
  Object encrypt, ownerKey, userKey, permissions, fileID, fileID1;
  GBool encrypted1;
  GBool ret;

  ret = gFalse;

  permFlags = defPermFlags;
  trailerDict.dictLookup("Encrypt", &encrypt);
  if ((encrypted1 = encrypt.isDict())) {
    ret = gTrue;
    encrypt.dictLookup("O", &ownerKey);
    encrypt.dictLookup("U", &userKey);
    encrypt.dictLookup("P", &permissions);
    trailerDict.dictLookup("ID", &fileID);
    if (ownerKey.isString() && ownerKey.getString()->getLength() == 32 &&
	userKey.isString() && userKey.getString()->getLength() == 32 &&
	permissions.isInt() &&
	fileID.isArray()) {
      permFlags = permissions.getInt();
      fileID.arrayGet(0, &fileID1);
      if (fileID1.isString()) {
	if (Decrypt::makeFileKey(ownerKey.getString(), userKey.getString(),
				 permFlags, fileID1.getString(),
				 userPassword, fileKey)) {
	  ret = gFalse;
	} else {
	  error(-1, "Incorrect user password");
	}
      } else {
	error(-1, "Weird encryption info");
      }
      fileID1.free();
    } else {
      error(-1, "Weird encryption info");
    }
    ownerKey.free();
    userKey.free();
    permissions.free();
    fileID.free();
  }
  encrypt.free();

  // this flag has to be set *after* we read the O/U/P strings
  encrypted = encrypted1;

  return ret;
}
#else
GBool XRef::checkEncrypted(GString *userPassword) {
  Object obj;
  GBool encrypted;

  trailerDict.dictLookup("Encrypt", &obj);
  if ((encrypted = !obj.isNull())) {
    error(-1, "PDF file is encrypted and cannot be displayed");
    error(-1, "* Decryption support is currently not included in xpdf");
    error(-1, "* due to legal restrictions: the U.S.A. still has bogus");
    error(-1, "* export controls on cryptography software.");
  }
  obj.free();
  return encrypted;
}
#endif

GBool XRef::okToPrint() {
#ifndef NO_DECRYPTION
  if (!(permFlags & permPrint)) {
    return gFalse;
  }
#endif
  return gTrue;
}

GBool XRef::okToChange() {
#ifndef NO_DECRYPTION
  if (!(permFlags & permChange)) {
    return gFalse;
  }
#endif
  return gTrue;
}

GBool XRef::okToCopy() {
#ifndef NO_DECRYPTION
  if (!(permFlags & permCopy)) {
    return gFalse;
  }
#endif
  return gTrue;
}

GBool XRef::okToAddNotes() {
#ifndef NO_DECRYPTION
  if (!(permFlags & permNotes)) {
    return gFalse;
  }
#endif
  return gTrue;
}

Object *XRef::fetch(int num, int gen, Object *obj) {
  XRefEntry *e;
  Parser *parser;
  Object obj1, obj2, obj3;

  // check for bogus ref - this can happen in corrupted PDF files
  if (num < 0 || num >= size) {
    obj->initNull();
    return obj;
  }

  e = &entries[num];
  if (e->gen == gen && e->offset >= 0) {
    obj1.initNull();
    parser = new Parser(new Lexer(
	           str->makeSubStream(start + e->offset, -1, &obj1)));
    parser->getObj(&obj1);
    parser->getObj(&obj2);
    parser->getObj(&obj3);
    if (obj1.isInt() && obj1.getInt() == num &&
	obj2.isInt() && obj2.getInt() == gen &&
	obj3.isCmd("obj")) {
#ifndef NO_DECRYPTION
      parser->getObj(obj, encrypted ? fileKey : (Guchar *)NULL, num, gen);
#else
      parser->getObj(obj);
#endif
    } else {
      obj->initNull();
    }
    obj1.free();
    obj2.free();
    obj3.free();
    delete parser;
  } else {
    obj->initNull();
  }
  return obj;
}

Object *XRef::getDocInfo(Object *obj) {
  return trailerDict.dictLookup("Info", obj);
}
