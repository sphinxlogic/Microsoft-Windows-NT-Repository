//========================================================================
//
// PSOutputDev.cc
//
// Copyright 1996 Derek B. Noonburg
//
//========================================================================

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <signal.h>
#include <math.h>
#include "GString.h"
#include "config.h"
#include "Object.h"
#include "Error.h"
#include "GfxState.h"
#include "GfxFont.h"
#include "FontFile.h"
#include "Catalog.h"
#include "Page.h"
#include "Stream.h"
#include "FormWidget.h"
#include "PSOutputDev.h"

#if JAPANESE_SUPPORT
#include "Japan12ToRKSJ.h"
#endif

#ifdef MACOS
// needed for setting type/creator of MacOS files
#include "ICSupport.h"
#endif

//------------------------------------------------------------------------
// Parameters
//------------------------------------------------------------------------

// Generate Level 1 PostScript?
GBool psOutLevel1 = gFalse;

// Generate Encapsulated PostScript?
GBool psOutEPS = gFalse;

#if OPI_SUPPORT
// Generate OPI comments?
GBool psOutOPI = gFalse;
#endif

int paperWidth = defPaperWidth;
int paperHeight = defPaperHeight;

//------------------------------------------------------------------------
// PostScript prolog and setup
//------------------------------------------------------------------------

static char *prolog[] = {
  "/xpdf 75 dict def xpdf begin",
  "% PDF special state",
  "/pdfDictSize 14 def",
  "/pdfSetup {",
  "  2 array astore",
  "  /setpagedevice where {",
  "    pop 3 dict dup begin",
  "      exch /PageSize exch def",
  "      /ImagingBBox null def",
  "      /Policies 1 dict dup begin /PageSize 3 def end def",
  "    end setpagedevice",
  "  } {",
  "    pop",
  "  } ifelse",
  "} def",
  "/pdfStartPage {",
  "  pdfDictSize dict begin",
  "  /pdfFill [0] def",
  "  /pdfStroke [0] def",
  "  /pdfLastFill false def",
  "  /pdfLastStroke false def",
  "  /pdfTextMat [1 0 0 1 0 0] def",
  "  /pdfFontSize 0 def",
  "  /pdfCharSpacing 0 def",
  "  /pdfTextRender 0 def",
  "  /pdfTextRise 0 def",
  "  /pdfWordSpacing 0 def",
  "  /pdfHorizScaling 1 def",
  "} def",
  "/pdfEndPage { end } def",
  "/sCol { pdfLastStroke not {",
  "          pdfStroke aload length",
  "          1 eq { setgray } { setrgbcolor} ifelse",
  "          /pdfLastStroke true def /pdfLastFill false def",
  "        } if } def",
  "/fCol { pdfLastFill not {",
  "          pdfFill aload length",
  "          1 eq { setgray } { setrgbcolor } ifelse",
  "          /pdfLastFill true def /pdfLastStroke false def",
  "        } if } def",
  "% build a font",
  "/pdfMakeFont {",
  "  4 3 roll findfont",
  "  4 2 roll matrix scale makefont",
  "  dup length dict begin",
  "    { 1 index /FID ne { def } { pop pop } ifelse } forall",
  "    /Encoding exch def",
  "    currentdict",
  "  end",
  "  definefont pop",
  "} def",
  "/pdfMakeFont16 { findfont definefont pop } def",
  "% graphics state operators",
  "/q { gsave pdfDictSize dict begin } def",
  "/Q { end grestore } def",
  "/cm { concat } def",
  "/d { setdash } def",
  "/i { setflat } def",
  "/j { setlinejoin } def",
  "/J { setlinecap } def",
  "/M { setmiterlimit } def",
  "/w { setlinewidth } def",
  "% color operators",
  "/g { dup 1 array astore /pdfFill exch def setgray",
  "    /pdfLastFill true def /pdfLastStroke false def } def",
  "/G { dup 1 array astore /pdfStroke exch def setgray",
  "     /pdfLastStroke true def /pdfLastFill false def } def",
  "/rg { 3 copy 3 array astore /pdfFill exch def setrgbcolor",
  "     /pdfLastFill true def /pdfLastStroke false def } def",
  "/RG { 3 copy 3 array astore /pdfStroke exch def setrgbcolor",
  "     /pdfLastStroke true def /pdfLastFill false def } def",
  "% path segment operators",
  "/m { moveto } def",
  "/l { lineto } def",
  "/c { curveto } def",
  "/re { 4 2 roll moveto 1 index 0 rlineto 0 exch rlineto",
  "      neg 0 rlineto closepath } def",
  "/h { closepath } def",
  "% path painting operators",
  "/S { sCol stroke } def",
  "/f { fCol fill } def",
  "/f* { fCol eofill } def",
  "% clipping operators",
  "/W { clip newpath } def",
  "/W* { eoclip newpath } def",
  "% text state operators",
  "/Tc { /pdfCharSpacing exch def } def",
  "/Tf { dup /pdfFontSize exch def",
  "      dup pdfHorizScaling mul exch matrix scale",
  "      pdfTextMat matrix concatmatrix dup 4 0 put dup 5 0 put",
  "      exch findfont exch makefont setfont } def",
  "/Tr { /pdfTextRender exch def } def",
  "/Ts { /pdfTextRise exch def } def",
  "/Tw { /pdfWordSpacing exch def } def",
  "/Tz { /pdfHorizScaling exch def } def",
  "% text positioning operators",
  "/Td { pdfTextMat transform moveto } def",
  "/Tm { /pdfTextMat exch def } def",
  "% text string operators",
  "/Tj { pdfTextRender 1 and 0 eq { fCol } { sCol } ifelse",
  "      0 pdfTextRise pdfTextMat dtransform rmoveto",
  "      pdfFontSize mul pdfHorizScaling mul",
  "      1 index stringwidth pdfTextMat idtransform pop",
  "      sub 1 index length dup 0 ne { div } { pop pop 0 } ifelse",
  "      pdfWordSpacing 0 pdfTextMat dtransform 32",
  "      4 3 roll pdfCharSpacing add 0 pdfTextMat dtransform",
  "      6 5 roll awidthshow",
  "      0 pdfTextRise neg pdfTextMat dtransform rmoveto } def",
  "/TJm { pdfFontSize 0.001 mul mul neg 0",
  "       pdfTextMat dtransform rmoveto } def",
  "% Level 1 image operators",
  "/pdfIm1 {",
  "  /pdfImBuf1 4 index string def",
  "  { currentfile pdfImBuf1 readhexstring pop } image",
  "} def",
  "/pdfImM1 {",
  "  /pdfImBuf1 4 index 7 add 8 idiv string def",
  "  { currentfile pdfImBuf1 readhexstring pop } imagemask",
  "} def",
  "% Level 2 image operators",
  "/pdfImBuf 100 string def",
  "/pdfIm {",
  "  image",
  "  { currentfile pdfImBuf readline",
  "    not { pop exit } if",
  "    (%-EOD-) eq { exit } if } loop",
  "} def",
  "/pdfImM {",
  "  fCol imagemask",
  "  { currentfile pdfImBuf readline",
  "    not { pop exit } if",
  "    (%-EOD-) eq { exit } if } loop",
  "} def",
  "end",
  NULL
};

//------------------------------------------------------------------------
// Fonts
//------------------------------------------------------------------------

struct PSFont {
  char *name;			// PDF name
  char *psName;			// PostScript name
};

struct PSSubstFont {
  char *psName;			// PostScript name
  double mWidth;		// width of 'm' character
};

static PSFont psFonts[] = {
  {"Courier",               "Courier"},
  {"Courier-Bold",          "Courier-Bold"},
  {"Courier-Oblique",       "Courier-Bold"},
  {"Courier-BoldOblique",   "Courier-BoldOblique"},
  {"Helvetica",             "Helvetica"},
  {"Helvetica-Bold",        "Helvetica-Bold"},
  {"Helvetica-Oblique",     "Helvetica-Oblique"},
  {"Helvetica-BoldOblique", "Helvetica-BoldOblique"},
  {"Symbol",                "Symbol"},
  {"Times-Roman",           "Times-Roman"},
  {"Times-Bold",            "Times-Bold"},
  {"Times-Italic",          "Times-Italic"},
  {"Times-BoldItalic",      "Times-BoldItalic"},
  {"ZapfDingbats",          "ZapfDingbats"},
  {NULL}
};

static PSSubstFont psSubstFonts[] = {
  {"Helvetica",             0.833},
  {"Helvetica-Oblique",     0.833},
  {"Helvetica-Bold",        0.889},
  {"Helvetica-BoldOblique", 0.889},
  {"Times-Roman",           0.788},
  {"Times-Italic",          0.722},
  {"Times-Bold",            0.833},
  {"Times-BoldItalic",      0.778},
  {"Courier",               0.600},
  {"Courier-Oblique",       0.600},
  {"Courier-Bold",          0.600},
  {"Courier-BoldOblique",   0.600}
};

//------------------------------------------------------------------------
// PSOutputDev
//------------------------------------------------------------------------

PSOutputDev::PSOutputDev(char *fileName, Catalog *catalog,
			 int firstPage, int lastPage,
			 GBool embedType11, GBool doForm1) {
  Page *page;
  Dict *resDict;
  FormWidgets *formWidgets;
  char **p;
  int pg;
  Object obj1, obj2;
  int i;

  // initialize
  embedType1 = embedType11;
  doForm = doForm1;
  fontIDs = NULL;
  fontFileIDs = NULL;
  fontFileNames = NULL;
  embFontList = NULL;
  f = NULL;
  if (doForm)
    lastPage = firstPage;

  // open file or pipe
  ok = gTrue;
  if (!strcmp(fileName, "-")) {
    fileType = psStdout;
    f = stdout;
  } else if (fileName[0] == '|') {
    fileType = psPipe;
#ifdef HAVE_POPEN
#ifndef WIN32
    signal(SIGPIPE, (void (*)(int))SIG_IGN);
#endif
    if (!(f = popen(fileName + 1, "w"))) {
      error(-1, "Couldn't run print command '%s'", fileName);
      ok = gFalse;
      return;
    }
#else
    error(-1, "Print commands are not supported ('%s')", fileName);
    ok = gFalse;
    return;
#endif
  } else {
    fileType = psFile;
    if (!(f = fopen(fileName, "w"))) {
      error(-1, "Couldn't open PostScript file '%s'", fileName);
      ok = gFalse;
      return;
    }
  }

  // initialize fontIDs, fontFileIDs, and fontFileNames lists
  fontIDSize = 64;
  fontIDLen = 0;
  fontIDs = (Ref *)gmalloc(fontIDSize * sizeof(Ref));
  fontFileIDSize = 64;
  fontFileIDLen = 0;
  fontFileIDs = (Ref *)gmalloc(fontFileIDSize * sizeof(Ref));
  fontFileNameSize = 64;
  fontFileNameLen = 0;
  fontFileNames = (GString **)gmalloc(fontFileNameSize * sizeof(GString *));

  // initialize embedded font resource comment list
  embFontList = new GString();

  // write header
  if (doForm) {
    writePS("%%!PS-Adobe-3.0 Resource-Form\n");
    writePS("%%%%Creator: xpdf/pdftops %s\n", xpdfVersion);
    writePS("%%%%LanguageLevel: %d\n", psOutLevel1 ? 1 : 2);
    writePS("%%%%EndComments\n");
  } else if (psOutEPS) {
    writePS("%%!PS-Adobe-3.0 EPSF-3.0\n");
    writePS("%%%%Creator: xpdf/pdftops %s\n", xpdfVersion);
    writePS("%%%%LanguageLevel: %d\n", psOutLevel1 ? 1 : 2);
    page = catalog->getPage(firstPage);
    writePS("%%%%BoundingBox: %d %d %d %d\n",
	    (int)floor(page->getX1()), (int)floor(page->getY1()),
	    (int)ceil(page->getX2()), (int)ceil(page->getY2()));
    if (floor(page->getX1()) != ceil(page->getX1()) ||
	floor(page->getY1()) != ceil(page->getY1()) ||
	floor(page->getX2()) != ceil(page->getX2()) ||
	floor(page->getY2()) != ceil(page->getY2())) {
      writePS("%%%%HiResBoundingBox: %g %g %g %g\n",
	      page->getX1(), page->getY1(),
	      page->getX2(), page->getY2());
    }
    writePS("%%%%DocumentSuppliedResources:\n");
    writePS("%%%%+ font: (atend)\n");
    writePS("%%%%EndComments\n");
  } else {
    writePS("%%!PS-Adobe-3.0\n");
    writePS("%%%%Creator: xpdf/pdftops %s\n", xpdfVersion);
    writePS("%%%%LanguageLevel: %d\n", psOutLevel1 ? 1 : 2);
    writePS("%%%%DocumentMedia: plain %d %d 0 () ()\n",
	    paperWidth, paperHeight);
    writePS("%%%%Pages: %d\n", lastPage - firstPage + 1);
    writePS("%%%%EndComments\n");
    writePS("%%%%BeginDefaults\n");
    writePS("%%%%PageMedia: plain\n");
    writePS("%%%%EndDefaults\n");
  }

  // write prolog
  if (!doForm)
    writePS("%%%%BeginProlog\n");
  writePS("%%%%BeginResource: xpdf %s\n", xpdfVersion);
  for (p = prolog; *p; ++p)
    writePS("%s\n", *p);
  writePS("%%%%EndResource\n");
  if (!doForm)
    writePS("%%%%EndProlog\n");

  // set up fonts
  if (!doForm)
    writePS("%%%%BeginSetup\n");
  writePS("xpdf begin\n");
  for (pg = firstPage; pg <= lastPage; ++pg) {
    page = catalog->getPage(pg);
    if ((resDict = page->getResourceDict())) {
      setupFonts(resDict);
    }
    formWidgets = new FormWidgets(page->getAnnots(&obj1));
    obj1.free();
    for (i = 0; i < formWidgets->getNumWidgets(); ++i) {
      if (formWidgets->getWidget(i)->getAppearance(&obj1)->isStream()) {
	obj1.streamGetDict()->lookup("Resources", &obj2);
	if (obj2.isDict()) {
	  setupFonts(obj2.getDict());
	}
	obj2.free();
      }
      obj1.free();
    }
    delete formWidgets;
  }
  if (doForm) {
    writePS("end\n");
  } else {
#if OPI_SUPPORT
    if (psOutOPI) {
      writePS("/opiMatrix matrix currentmatrix def\n");
    }
#endif
    if (!psOutEPS) {
      writePS("%d %d pdfSetup\n", paperWidth, paperHeight);
    }
    writePS("%%%%EndSetup\n");
  }

  // write form header
  if (doForm) {
    page = catalog->getPage(firstPage);
    writePS("4 dict dup begin\n");
    writePS("/BBox [%d %d %d %d] def\n",
	    (int)page->getX1(), (int)page->getY1(),
	    (int)page->getX2(), (int)page->getY2());
    writePS("/FormType 1 def\n");
    writePS("/Matrix [1 0 0 1 0 0] def\n");
  }

  // initialize sequential page number
  seqPage = 1;

#if OPI_SUPPORT
  // initialize OPI nesting levels
  opi13Nest = 0;
  opi20Nest = 0;
#endif
}

PSOutputDev::~PSOutputDev() {
  int i;

  if (f) {
    if (doForm) {
      writePS("end\n");
      writePS("/Foo exch /Form defineresource pop\n");
    } else if (psOutEPS) {
      writePS("%%%%Trailer\n");
      writePS("end\n");
      writePS("%%%%DocumentSuppliedResources:\n");
      writePS("%s", embFontList->getCString());
      writePS("%%%%EOF\n");
    } else {
      writePS("%%%%Trailer\n");
      writePS("end\n");
      writePS("%%%%EOF\n");
    }
    if (fileType == psFile) {
#ifdef MACOS
      ICS_MapRefNumAndAssign((short)f->handle);
#endif
      fclose(f);
    }
#ifdef HAVE_POPEN
    else if (fileType == psPipe) {
      pclose(f);
#ifndef WIN32
      signal(SIGPIPE, (void (*)(int))SIG_DFL);
#endif
    }
#endif
  }
  if (embFontList) {
    delete embFontList;
  }
  if (fontIDs) {
    gfree(fontIDs);
  }
  if (fontFileIDs) {
    gfree(fontFileIDs);
  }
  if (fontFileNames) {
    for (i = 0; i < fontFileNameLen; ++i) {
      delete fontFileNames[i];
    }
    gfree(fontFileNames);
  }
}

void PSOutputDev::setupFonts(Dict *resDict) {
  Object fontDict, xObjDict, xObj, resObj;
  GfxFontDict *gfxFontDict;
  GfxFont *font;
  int i;

  resDict->lookup("Font", &fontDict);
  if (fontDict.isDict()) {
    gfxFontDict = new GfxFontDict(fontDict.getDict());
    for (i = 0; i < gfxFontDict->getNumFonts(); ++i) {
      font = gfxFontDict->getFont(i);
      setupFont(font);
    }
    delete gfxFontDict;
  }
  fontDict.free();

  resDict->lookup("XObject", &xObjDict);
  if (xObjDict.isDict()) {
    for (i = 0; i < xObjDict.dictGetLength(); ++i) {
      xObjDict.dictGetVal(i, &xObj);
      if (xObj.isStream()) {
	xObj.streamGetDict()->lookup("Resources", &resObj);
	if (resObj.isDict())
	  setupFonts(resObj.getDict());
	resObj.free();
      }
      xObj.free();
    }
  }
  xObjDict.free();
}

void PSOutputDev::setupFont(GfxFont *font) {
  Ref fontFileID;
  GString *name;
  char *psName;
  char *charName;
  double xs, ys;
  GBool do16Bit;
  int code;
  double w1, w2;
  double *fm;
  int i, j;

  // check if font is already set up
  for (i = 0; i < fontIDLen; ++i) {
    if (fontIDs[i].num == font->getID().num &&
	fontIDs[i].gen == font->getID().gen)
      return;
  }

  // add entry to fontIDs list
  if (fontIDLen >= fontIDSize) {
    fontIDSize += 64;
    fontIDs = (Ref *)grealloc(fontIDs, fontIDSize * sizeof(Ref));
  }
  fontIDs[fontIDLen++] = font->getID();

  xs = ys = 1;
  do16Bit = gFalse;

  // check for embedded Type 1 font
  if (embedType1 && font->getType() == fontType1 &&
      font->getEmbeddedFontID(&fontFileID)) {
    psName = font->getEmbeddedFontName();
    setupEmbeddedType1Font(&fontFileID, psName);

  // check for external Type 1 font file
  } else if (embedType1 && font->getType() == fontType1 &&
	     font->getExtFontFile()) {
    // this assumes that the PS font name matches the PDF font name
    psName = font->getName()->getCString();
    setupEmbeddedType1Font(font->getExtFontFile(), psName);

  // check for embedded Type 1C font
  } else if (embedType1 && font->getType() == fontType1C &&
	     font->getEmbeddedFontID(&fontFileID)) {
    psName = font->getEmbeddedFontName();
    setupEmbeddedType1CFont(font, &fontFileID, psName);

  } else if (font->is16Bit() && font->getCharSet16() == font16AdobeJapan12) {
    psName = "Ryumin-Light-RKSJ";
    do16Bit = gTrue;

  // do font substitution
  } else {
    name = font->getName();
    psName = NULL;
    if (name) {
      for (i = 0; psFonts[i].name; ++i) {
	if (name->cmp(psFonts[i].name) == 0) {
	  psName = psFonts[i].psName;
	  break;
	}
      }
    }
    if (!psName) {
      if (font->isFixedWidth())
	i = 8;
      else if (font->isSerif())
	i = 4;
      else
	i = 0;
      if (font->isBold())
	i += 2;
      if (font->isItalic())
	i += 1;
      psName = psSubstFonts[i].psName;
      if ((code = font->getCharCode("m")) >= 0) {
	w1 = font->getWidth(code);
      } else {
	w1 = 0;
      }
      w2 = psSubstFonts[i].mWidth;
      xs = w1 / w2;
      if (xs < 0.1) {
	xs = 1;
      }
      if (font->getType() == fontType3) {
	// This is a hack which makes it possible to substitute for some
	// Type 3 fonts.  The problem is that it's impossible to know what
	// the base coordinate system used in the font is without actually
	// rendering the font.
	ys = xs;
	fm = font->getFontMatrix();
	if (fm[0] != 0) {
	  ys *= fm[3] / fm[0];
	}
      } else {
	ys = 1;
      }
    }
  }

  // generate PostScript code to set up the font
  if (do16Bit) {
    writePS("/F%d_%d /%s pdfMakeFont16\n",
	    font->getID().num, font->getID().gen, psName);
  } else {
    writePS("/F%d_%d /%s %g %g\n",
	    font->getID().num, font->getID().gen, psName, xs, ys);
    for (i = 0; i < 256; i += 8) {
      writePS((i == 0) ? "[ " : "  ");
      for (j = 0; j < 8; ++j) {
	charName = font->getCharName(i+j);
	writePS("/%s", charName ? charName : ".notdef");
      }
      writePS((i == 256-8) ? "]\n" : "\n");
    }
    writePS("pdfMakeFont\n");
  }
}

void PSOutputDev::setupEmbeddedType1Font(Ref *id, char *psName) {
  static char hexChar[17] = "0123456789abcdef";
  Object refObj, strObj, obj1, obj2;
  Dict *dict;
  int length1, length2;
  int c;
  int start[4];
  GBool binMode;
  int i;

  // check if font is already embedded
  for (i = 0; i < fontFileIDLen; ++i) {
    if (fontFileIDs[i].num == id->num &&
	fontFileIDs[i].gen == id->gen)
      return;
  }

  // add entry to fontFileIDs list
  if (fontFileIDLen >= fontFileIDSize) {
    fontFileIDSize += 64;
    fontFileIDs = (Ref *)grealloc(fontFileIDs, fontFileIDSize * sizeof(Ref));
  }
  fontFileIDs[fontFileIDLen++] = *id;

  // get the font stream and info
  refObj.initRef(id->num, id->gen);
  refObj.fetch(&strObj);
  refObj.free();
  if (!strObj.isStream()) {
    error(-1, "Embedded font file object is not a stream");
    goto err1;
  }
  if (!(dict = strObj.streamGetDict())) {
    error(-1, "Embedded font stream is missing its dictionary");
    goto err1;
  }
  dict->lookup("Length1", &obj1);
  dict->lookup("Length2", &obj2);
  if (!obj1.isInt() || !obj2.isInt()) {
    error(-1, "Missing length fields in embedded font stream dictionary");
    obj1.free();
    obj2.free();
    goto err1;
  }
  length1 = obj1.getInt();
  length2 = obj2.getInt();
  obj1.free();
  obj2.free();

  // beginning comment
  if (psOutEPS) {
    writePS("%%%%BeginResource: font %s\n", psName);
    embFontList->append("%%+ font ");
    embFontList->append(psName);
    embFontList->append("\n");
  }

  // copy ASCII portion of font
  strObj.streamReset();
  for (i = 0; i < length1 && (c = strObj.streamGetChar()) != EOF; ++i)
    fputc(c, f);

  // figure out if encrypted portion is binary or ASCII
  binMode = gFalse;
  for (i = 0; i < 4; ++i) {
    start[i] = strObj.streamGetChar();
    if (start[i] == EOF) {
      error(-1, "Unexpected end of file in embedded font stream");
      goto err1;
    }
    if (!((start[i] >= '0' && start[i] <= '9') ||
	  (start[i] >= 'A' && start[i] <= 'F') ||
	  (start[i] >= 'a' && start[i] <= 'f')))
      binMode = gTrue;
  }

  // convert binary data to ASCII
  if (binMode) {
    for (i = 0; i < 4; ++i) {
      fputc(hexChar[(start[i] >> 4) & 0x0f], f);
      fputc(hexChar[start[i] & 0x0f], f);
    }
    while (i < length2) {
      if ((c = strObj.streamGetChar()) == EOF)
	break;
      fputc(hexChar[(c >> 4) & 0x0f], f);
      fputc(hexChar[c & 0x0f], f);
      if (++i % 32 == 0)
	fputc('\n', f);
    }
    if (i % 32 > 0)
      fputc('\n', f);

  // already in ASCII format -- just copy it
  } else {
    for (i = 0; i < 4; ++i)
      fputc(start[i], f);
    for (i = 4; i < length2; ++i) {
      if ((c = strObj.streamGetChar()) == EOF)
	break;
      fputc(c, f);
    }
  }

  // write padding and "cleartomark"
  for (i = 0; i < 8; ++i)
    writePS("00000000000000000000000000000000"
	    "00000000000000000000000000000000\n");
  writePS("cleartomark\n");

  // ending comment
  if (psOutEPS) {
    writePS("%%%%EndResource\n");
  }

 err1:
  strObj.free();
}

//~ This doesn't handle .pfb files or binary eexec data (which only
//~ happens in pfb files?).
void PSOutputDev::setupEmbeddedType1Font(GString *fileName, char *psName) {
  FILE *fontFile;
  int c;
  int i;

  // check if font is already embedded
  for (i = 0; i < fontFileNameLen; ++i) {
    if (!fontFileNames[i]->cmp(fileName)) {
      return;
    }
  }

  // add entry to fontFileNames list
  if (fontFileNameLen >= fontFileNameSize) {
    fontFileNameSize += 64;
    fontFileNames = (GString **)grealloc(fontFileNames,
					 fontFileNameSize * sizeof(GString *));
  }
  fontFileNames[fontFileNameLen++] = fileName->copy();

  // beginning comment
  if (psOutEPS) {
    writePS("%%%%BeginResource: font %s\n", psName);
    embFontList->append("%%+ font ");
    embFontList->append(psName);
    embFontList->append("\n");
  }

  // copy the font file
  if (!(fontFile = fopen(fileName->getCString(), "rb"))) {
    error(-1, "Couldn't open external font file");
    return;
  }
  while ((c = fgetc(fontFile)) != EOF)
    fputc(c, f);
  fclose(fontFile);

  // ending comment
  if (psOutEPS) {
    writePS("%%%%EndResource\n");
  }
}

void PSOutputDev::setupEmbeddedType1CFont(GfxFont *font, Ref *id,
					  char *psName) {
  char *fontBuf;
  int fontLen;
  Type1CFontConverter *cvt;
  int i;

  // check if font is already embedded
  for (i = 0; i < fontFileIDLen; ++i) {
    if (fontFileIDs[i].num == id->num &&
	fontFileIDs[i].gen == id->gen)
      return;
  }

  // add entry to fontFileIDs list
  if (fontFileIDLen >= fontFileIDSize) {
    fontFileIDSize += 64;
    fontFileIDs = (Ref *)grealloc(fontFileIDs, fontFileIDSize * sizeof(Ref));
  }
  fontFileIDs[fontFileIDLen++] = *id;

  // beginning comment
  if (psOutEPS) {
    writePS("%%%%BeginResource: font %s\n", psName);
    embFontList->append("%%+ font ");
    embFontList->append(psName);
    embFontList->append("\n");
  }

  // convert it to a Type 1 font
  fontBuf = font->readEmbFontFile(&fontLen);
  cvt = new Type1CFontConverter(fontBuf, fontLen, f);
  cvt->convert();
  delete cvt;
  gfree(fontBuf);

  // ending comment
  if (psOutEPS) {
    writePS("%%%%EndResource\n");
  }
}

void PSOutputDev::startPage(int pageNum, GfxState *state) {
  int x1, y1, x2, y2, width, height, t;

  if (doForm) {

    writePS("/PaintProc {\n");
    writePS("begin xpdf begin\n");
    writePS("pdfStartPage\n");
    tx = ty = 0;
    xScale = yScale = 1;
    landscape = gFalse;

  } else if (psOutEPS) {

    writePS("pdfStartPage\n");
    tx = ty = 0;
    xScale = yScale = 1;
    landscape = gFalse;

  } else {

    writePS("%%%%Page: %d %d\n", pageNum, seqPage);
    writePS("%%%%BeginPageSetup\n");

    // rotate, translate, and scale page
    x1 = (int)(state->getX1() + 0.5);
    y1 = (int)(state->getY1() + 0.5);
    x2 = (int)(state->getX2() + 0.5);
    y2 = (int)(state->getY2() + 0.5);
    width = x2 - x1;
    height = y2 - y1;
    if (width > height && width > paperWidth) {
      landscape = gTrue;
      writePS("%%%%PageOrientation: Landscape\n");
      writePS("pdfStartPage\n");
      writePS("90 rotate\n");
      tx = -x1;
      ty = -(y1 + paperWidth);
      t = width;
      width = height;
      height = t;
    } else {
      landscape = gFalse;
      writePS("%%%%PageOrientation: Portrait\n");
      writePS("pdfStartPage\n");
      tx = -x1;
      ty = -y1;
    }
    if (width < paperWidth) {
      tx += (paperWidth - width) / 2;
    }
    if (height < paperHeight) {
      ty += (paperHeight - height) / 2;
    }
    if (tx != 0 || ty != 0) {
      writePS("%g %g translate\n", tx, ty);
    }
    if (width > paperWidth || height > paperHeight) {
      xScale = (double)paperWidth / (double)width;
      yScale = (double)paperHeight / (double)height;
      if (yScale < xScale) {
	xScale = yScale;
      }
      writePS("%0.4f %0.4f scale\n", xScale, xScale);
    } else {
      xScale = yScale = 1;
    }

    writePS("%%%%EndPageSetup\n");
    ++seqPage;
  }
}

void PSOutputDev::endPage() {
  if (doForm) {
    writePS("pdfEndPage\n");
    writePS("end end\n");
    writePS("} def\n");
  } else {
    writePS("showpage\n");
    writePS("%%%%PageTrailer\n");
    writePS("pdfEndPage\n");
  }
}

void PSOutputDev::saveState(GfxState *state) {
  writePS("q\n");
}

void PSOutputDev::restoreState(GfxState *state) {
  writePS("Q\n");
}

void PSOutputDev::updateCTM(GfxState *state, double m11, double m12,
			    double m21, double m22, double m31, double m32) {
  writePS("[%g %g %g %g %g %g] cm\n", m11, m12, m21, m22, m31, m32);
}

void PSOutputDev::updateLineDash(GfxState *state) {
  double *dash;
  double start;
  int length, i;

  state->getLineDash(&dash, &length, &start);
  writePS("[");
  for (i = 0; i < length; ++i)
    writePS("%g%s", dash[i], (i == length-1) ? "" : " ");
  writePS("] %g d\n", start);
}

void PSOutputDev::updateFlatness(GfxState *state) {
  writePS("%d i\n", state->getFlatness());
}

void PSOutputDev::updateLineJoin(GfxState *state) {
  writePS("%d j\n", state->getLineJoin());
}

void PSOutputDev::updateLineCap(GfxState *state) {
  writePS("%d J\n", state->getLineCap());
}

void PSOutputDev::updateMiterLimit(GfxState *state) {
  writePS("%g M\n", state->getMiterLimit());
}

void PSOutputDev::updateLineWidth(GfxState *state) {
  writePS("%g w\n", state->getLineWidth());
}

void PSOutputDev::updateFillColor(GfxState *state) {
  GfxColor *color;
  double r, g, b;

  color = state->getFillColor();
  r = color->getR();
  g = color->getG();
  b = color->getB();
  if (r == g && g == b)
    writePS("%g g\n", r);
  else
    writePS("%g %g %g rg\n", r, g, b);
}

void PSOutputDev::updateStrokeColor(GfxState *state) {
  GfxColor *color;
  double r, g, b;

  color = state->getStrokeColor();
  r = color->getR();
  g = color->getG();
  b = color->getB();
  if (r == g && g == b)
    writePS("%g G\n", r);
  else
    writePS("%g %g %g RG\n", r, g, b);
}

void PSOutputDev::updateFont(GfxState *state) {
  if (state->getFont()) {
    writePS("/F%d_%d %g Tf\n",
	    state->getFont()->getID().num, state->getFont()->getID().gen,
	    state->getFontSize());
  }
}

void PSOutputDev::updateTextMat(GfxState *state) {
  double *mat;

  mat = state->getTextMat();
  writePS("[%g %g %g %g %g %g] Tm\n",
	  mat[0], mat[1], mat[2], mat[3], mat[4], mat[5]);
}

void PSOutputDev::updateCharSpace(GfxState *state) {
  writePS("%g Tc\n", state->getCharSpace());
}

void PSOutputDev::updateRender(GfxState *state) {
  writePS("%d Tr\n", state->getRender());
}

void PSOutputDev::updateRise(GfxState *state) {
  writePS("%g Ts\n", state->getRise());
}

void PSOutputDev::updateWordSpace(GfxState *state) {
  writePS("%g Tw\n", state->getWordSpace());
}

void PSOutputDev::updateHorizScaling(GfxState *state) {
  writePS("%g Tz\n", state->getHorizScaling());
}

void PSOutputDev::updateTextPos(GfxState *state) {
  writePS("%g %g Td\n", state->getLineX(), state->getLineY());
}

void PSOutputDev::updateTextShift(GfxState *state, double shift) {
  writePS("%g TJm\n", shift);
}

void PSOutputDev::stroke(GfxState *state) {
  doPath(state->getPath());
  writePS("S\n");
}

void PSOutputDev::fill(GfxState *state) {
  doPath(state->getPath());
  writePS("f\n");
}

void PSOutputDev::eoFill(GfxState *state) {
  doPath(state->getPath());
  writePS("f*\n");
}

void PSOutputDev::clip(GfxState *state) {
  doPath(state->getPath());
  writePS("W\n");
}

void PSOutputDev::eoClip(GfxState *state) {
  doPath(state->getPath());
  writePS("W*\n");
}

void PSOutputDev::doPath(GfxPath *path) {
  GfxSubpath *subpath;
  double x0, y0, x1, y1, x2, y2, x3, y3, x4, y4;
  int n, m, i, j;

  n = path->getNumSubpaths();

  if (n == 1 && path->getSubpath(0)->getNumPoints() == 5) {
    subpath = path->getSubpath(0);
    x0 = subpath->getX(0);
    y0 = subpath->getY(0);
    x4 = subpath->getX(4);
    y4 = subpath->getY(4);
    if (x4 == x0 && y4 == y0) {
      x1 = subpath->getX(1);
      y1 = subpath->getY(1);
      x2 = subpath->getX(2);
      y2 = subpath->getY(2);
      x3 = subpath->getX(3);
      y3 = subpath->getY(3);
      if (x0 == x1 && x2 == x3 && y0 == y3 && y1 == y2) {
	writePS("%g %g %g %g re\n",
		x0 < x2 ? x0 : x2, y0 < y1 ? y0 : y1,
		fabs(x2 - x0), fabs(y1 - y0));
	return;
      } else if (x0 == x3 && x1 == x2 && y0 == y1 && y2 == y3) {
	writePS("%g %g %g %g re\n",
		x0 < x1 ? x0 : x1, y0 < y2 ? y0 : y2,
		fabs(x1 - x0), fabs(y2 - y0));
	return;
      }
    }
  }

  for (i = 0; i < n; ++i) {
    subpath = path->getSubpath(i);
    m = subpath->getNumPoints();
    writePS("%g %g m\n", subpath->getX(0), subpath->getY(0));
    j = 1;
    while (j < m) {
      if (subpath->getCurve(j)) {
	writePS("%g %g %g %g %g %g c\n", subpath->getX(j), subpath->getY(j),
		subpath->getX(j+1), subpath->getY(j+1),
		subpath->getX(j+2), subpath->getY(j+2));
	j += 3;
      } else {
	writePS("%g %g l\n", subpath->getX(j), subpath->getY(j));
	++j;
      }
    }
    if (subpath->isClosed()) {
      writePS("h\n");
    }
  }
}

void PSOutputDev::drawString(GfxState *state, GString *s) {
  // check for invisible text -- this is used by Acrobat Capture
  if ((state->getRender() & 3) == 3)
    return;

  writePSString(s);
  writePS(" %g Tj\n", state->getFont()->getWidth(s));
}

void PSOutputDev::drawString16(GfxState *state, GString *s) {
  int c1, c2;
  double w;
  int i;

  // check for invisible text -- this is used by Acrobat Capture
  if ((state->getRender() & 3) == 3)
    return;

  switch (state->getFont()->getCharSet16()) {

  case font16AdobeJapan12:
#if JAPANESE_SUPPORT
    writePS("<");
    w = 0;
    for (i = 0; i < s->getLength(); i += 2) {
      c1 = ((s->getChar(i) & 0xff) << 8) + (s->getChar(i+1) & 0xff);
      if (c1 <= 8285) {
	c2 = japan12ToRKSJ[c1];
      } else {
	c2 = 0x20;
      }
      if (c2 <= 0xff) {
	writePS("%02x", c2);
      } else {
	writePS("%02x%02x", c2 >> 8, c2 & 0xff);
      }
      w += state->getFont()->getWidth16(c1);
    }
    writePS("> %g Tj\n", w);
#endif
    break;

  case font16AdobeGB12:
    break;
  }
}

void PSOutputDev::drawImageMask(GfxState *state, Stream *str,
				int width, int height, GBool invert,
				GBool inlineImg) {
  int len;

  len = height * ((width + 7) / 8);
  if (psOutLevel1)
    doImageL1(NULL, invert, inlineImg, str, width, height, len);
  else
    doImage(NULL, invert, inlineImg, str, width, height, len);
}

void PSOutputDev::drawImage(GfxState *state, Stream *str, int width,
			    int height, GfxImageColorMap *colorMap,
			    GBool inlineImg) {
  int len;

  len = height * ((width * colorMap->getNumPixelComps() *
		   colorMap->getBits() + 7) / 8);
  if (psOutLevel1)
    doImageL1(colorMap, gFalse, inlineImg, str, width, height, len);
  else
    doImage(colorMap, gFalse, inlineImg, str, width, height, len);
}

void PSOutputDev::doImageL1(GfxImageColorMap *colorMap,
			    GBool invert, GBool inlineImg,
			    Stream *str, int width, int height, int len) {
  ImageStream *imgStr;
  Guchar pixBuf[4];
  GfxColor color;
  int x, y, i;

  // width, height, matrix, bits per component
  if (colorMap) {
    writePS("%d %d 8 [%d 0 0 %d 0 %d] pdfIm1\n",
	    width, height,
	    width, -height, height);
  } else {
    writePS("%d %d %s [%d 0 0 %d 0 %d] pdfImM1\n",
	    width, height, invert ? "true" : "false",
	    width, -height, height);
  }

  // image
  if (colorMap) {

    // set up to process the data stream
    imgStr = new ImageStream(str, width, colorMap->getNumPixelComps(),
			     colorMap->getBits());
    imgStr->reset();

    // process the data stream
    i = 0;
    for (y = 0; y < height; ++y) {

      // write the line
      for (x = 0; x < width; ++x) {
	imgStr->getPixel(pixBuf);
	colorMap->getColor(pixBuf, &color);
	fprintf(f, "%02x", (int)(color.getGray() * 255 + 0.5));
	if (++i == 32) {
	  fputc('\n', f);
	  i = 0;
	}
      }
    }
    if (i != 0)
      fputc('\n', f);
    delete imgStr;

  // imagemask
  } else {
    str->reset();
    i = 0;
    for (y = 0; y < height; ++y) {
      for (x = 0; x < width; x += 8) {
	fprintf(f, "%02x", str->getChar() & 0xff);
	if (++i == 32) {
	  fputc('\n', f);
	  i = 0;
	}
      }
    }
    if (i != 0)
      fputc('\n', f);
  }
}

void PSOutputDev::doImage(GfxImageColorMap *colorMap,
			  GBool invert, GBool inlineImg,
			  Stream *str, int width, int height, int len) {
  GfxColorSpace *colorSpace;
  LabParams *labParams;
  GString *s;
  int n, numComps;
  Guchar *color;
  Guchar x[4];
  GfxColor rgb;
  GBool useRLE, useA85;
  int c;
  int i, j, k;

  // color space
  if (colorMap) {
    colorSpace = colorMap->getColorSpace();
    if (colorSpace->isSeparation()) {
      //~ this is a kludge -- the correct thing would be to output
      //~ a separation color space
      n = (1 << colorMap->getBits()) - 1;
      writePS("[/Indexed /DeviceRGB %d <", n);
      for (i = 0; i <= n; i += 8) {
	writePS("  ");
	for (j = i; j < i+8 && j <= n; ++j) {
	  x[0] = j;
	  colorMap->getColor(x, &rgb);
	  writePS("%02x%02x%02x",
		  (int)(255 * rgb.getR() + 0.5),
		  (int)(255 * rgb.getG() + 0.5),
		  (int)(255 * rgb.getB() + 0.5));
	}
	writePS("\n");
      }
      writePS("> ] setcolorspace\n");
    } else {
      if (colorSpace->isIndexed()) {
	writePS("[/Indexed ");
      }
      switch (colorSpace->getMode()) {
      case colorGray:
	writePS("/DeviceGray ");
	break;
      case colorCMYK:
	writePS("/DeviceCMYK ");
	break;
      case colorRGB:
	writePS("/DeviceRGB ");
	break;
      case colorLab:
	labParams = colorSpace->getLabParams();
	writePS("[/CIEBasedABC <<\n");
	writePS(" /RangeABC [0 100 %g %g %g %g]\n",
		labParams->aMin, labParams->aMax,
		labParams->bMin, labParams->bMax);
	writePS(" /DecodeABC [{16 add 116 div} bind {500 div} bind {200 div} bind]\n");
	writePS(" /MatrixABC [1 1 1 1 0 0 0 0 -1]\n");
	writePS(" /DecodeLMN\n");
	writePS("   [{dup 6 29 div ge {dup dup mul mul}\n");
	writePS("     {4 29 div sub 108 841 div mul } ifelse %g mul} bind\n",
		labParams->whiteX);
	writePS("    {dup 6 29 div ge {dup dup mul mul}\n");
	writePS("     {4 29 div sub 108 841 div mul } ifelse %g mul} bind\n",
		labParams->whiteY);
	writePS("    {dup 6 29 div ge {dup dup mul mul}\n");
	writePS("     {4 29 div sub 108 841 div mul } ifelse %g mul} bind]\n",
		labParams->whiteZ);
	writePS(" /WhitePoint [%g %g %g]\n",
		labParams->whiteX, labParams->whiteY, labParams->whiteZ);
	writePS(">>] ");
	break;
      }
      if (colorSpace->isIndexed()) {
	n = colorSpace->getIndexHigh();
	numComps = colorSpace->getNumColorComps();
	writePS("%d <\n", n);
	for (i = 0; i <= n; i += 8) {
	  writePS("  ");
	  for (j = i; j < i+8 && j <= n; ++j) {
	    color = colorSpace->getLookupVal(j);
	    for (k = 0; k < numComps; ++k) {
	      writePS("%02x", color[k]);
	    }
	  }
	  writePS("\n");
	}
	writePS("> ] setcolorspace\n");
      } else {
	writePS("setcolorspace\n");
      }
    }
  }

  // image dictionary
  writePS("<<\n  /ImageType 1\n");

  // width, height, matrix, bits per component
  writePS("  /Width %d\n", width);
  writePS("  /Height %d\n", height);
  writePS("  /ImageMatrix [%d 0 0 %d 0 %d]\n", width, -height, height);
  writePS("  /BitsPerComponent %d\n",
	  colorMap ? colorMap->getBits() : 1);

  // decode 
  if (colorMap) {
    writePS("  /Decode [");
    if (colorMap->getColorSpace()->isSeparation()) {
      //~ this is a kludge -- the correct thing would be to output
      //~ a separation color space
      n = (1 << colorMap->getBits()) - 1;
      writePS("%g %g", colorMap->getDecodeLow(0) * n,
	      colorMap->getDecodeHigh(0) * n);
    } else {
      numComps = colorMap->getNumPixelComps();
      for (i = 0; i < numComps; ++i) {
	if (i > 0) {
	  writePS(" ");
	}
	writePS("%g %g", colorMap->getDecodeLow(i),
		colorMap->getDecodeHigh(i));
      }
    }
    writePS("]\n");
  } else {
    writePS("  /Decode [%d %d]\n", invert ? 1 : 0, invert ? 0 : 1);
  }

  if (doForm) {

    // data source
    writePS("  /DataSource <~\n");

    // write image data stream, using ASCII85 encode filter
    if (inlineImg) {
      str = new FixedLengthEncoder(str, len);
    }
    str = new ASCII85Encoder(str);
    str->reset();
    while ((c = str->getChar()) != EOF)
      fputc(c, f);
    fputc('\n', f);
    delete str;

    // end of image dictionary
    writePS(">>\n%s\n", colorMap ? "image" : "imagemask");

  } else {

    // data source
    writePS("  /DataSource currentfile\n");
    s = str->getPSFilter("    ");
    if (inlineImg || !s) {
      useRLE = gTrue;
      useA85 = gTrue;
    } else {
      useRLE = gFalse;
      useA85 = str->isBinary();
    }
    if (useA85)
      writePS("    /ASCII85Decode filter\n");
    if (useRLE)
      writePS("    /RunLengthDecode filter\n");
    else
      writePS("%s", s->getCString());
    if (s)
      delete s;

    // cut off inline image streams at appropriate length
    if (inlineImg)
      str = new FixedLengthEncoder(str, len);
    else if (!useRLE)
      str = str->getBaseStream();

    // add RunLengthEncode and ASCII85 encode filters
    if (useRLE)
      str = new RunLengthEncoder(str);
    if (useA85)
      str = new ASCII85Encoder(str);

    // end of image dictionary
    writePS(">>\n");
#if OPI_SUPPORT
    if (opi13Nest) {
      if (inlineImg) {
	// this can't happen -- OPI dictionaries are in XObjects
	error(-1, "Internal: OPI in inline image");
	n = 0;
      } else {
	// need to read the stream to count characters -- the length
	// is data-dependent (because of A85 and RLE filters)
	str->reset();
	n = 0;
	while ((c = str->getChar()) != EOF) {
	  ++n;
	}
      }
      // +6/7 for "pdfIm\n" / "pdfImM\n"
      // +8 for newline + trailer
      n += colorMap ? 14 : 15;
      writePS("%%%%BeginData: %d Hex Bytes\n", n);
    }
#endif
    writePS("%s\n", colorMap ? "pdfIm" : "pdfImM");

    // copy the stream data
    str->reset();
    while ((c = str->getChar()) != EOF)
      fputc(c, f);

    // add newline and trailer to the end
    fputc('\n', f);
    fputs("%-EOD-\n", f);
#if OPI_SUPPORT
    if (opi13Nest) {
      writePS("%%%%EndData\n");
    }
#endif

    // delete encoders
    if (useRLE || useA85)
      delete str;
  }
}

#if OPI_SUPPORT
void PSOutputDev::opiBegin(GfxState *state, Dict *opiDict) {
  Object dict;

  if (psOutOPI) {
    opiDict->lookup("2.0", &dict);
    if (dict.isDict()) {
      opiBegin20(state, dict.getDict());
      dict.free();
    } else {
      dict.free();
      opiDict->lookup("1.3", &dict);
      if (dict.isDict()) {
	opiBegin13(state, dict.getDict());
      }
      dict.free();
    }
  }
}

void PSOutputDev::opiBegin20(GfxState *state, Dict *dict) {
  Object obj1, obj2, obj3, obj4;
  double width, height, left, right, top, bottom;
  int w, h;
  int i;

  writePS("%%%%BeginOPI: 2.0\n");
  writePS("%%%%Distilled\n");

  dict->lookup("F", &obj1);
  if (getFileSpec(&obj1, &obj2)) {
    writePS("%%%%ImageFileName: %s\n",
	    obj2.getString()->getCString());
    obj2.free();
  }
  obj1.free();

  dict->lookup("MainImage", &obj1);
  if (obj1.isString()) {
    writePS("%%%%MainImage: %s\n", obj1.getString()->getCString());
  }
  obj1.free();

  //~ ignoring 'Tags' entry
  //~ need to use writePSString() and deal with >255-char lines

  dict->lookup("Size", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 2) {
    obj1.arrayGet(0, &obj2);
    width = obj2.getNum();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    height = obj2.getNum();
    obj2.free();
    writePS("%%%%ImageDimensions: %g %g\n", width, height);
  }
  obj1.free();

  dict->lookup("CropRect", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 4) {
    obj1.arrayGet(0, &obj2);
    left = obj2.getNum();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    top = obj2.getNum();
    obj2.free();
    obj1.arrayGet(2, &obj2);
    right = obj2.getNum();
    obj2.free();
    obj1.arrayGet(3, &obj2);
    bottom = obj2.getNum();
    obj2.free();
    writePS("%%%%ImageCropRect: %g %g %g %g\n", left, top, right, bottom);
  }
  obj1.free();

  dict->lookup("Overprint", &obj1);
  if (obj1.isBool()) {
    writePS("%%%%ImageOverprint: %s\n", obj1.getBool() ? "true" : "false");
  }
  obj1.free();

  dict->lookup("Inks", &obj1);
  if (obj1.isName()) {
    writePS("%%%%ImageInks: %s\n", obj1.getName());
  } else if (obj1.isArray() && obj1.arrayGetLength() >= 1) {
    obj1.arrayGet(0, &obj2);
    if (obj2.isName()) {
      writePS("%%%%ImageInks: %s %d",
	      obj2.getName(), (obj1.arrayGetLength() - 1) / 2);
      for (i = 1; i+1 < obj1.arrayGetLength(); i += 2) {
	obj1.arrayGet(i, &obj3);
	obj1.arrayGet(i+1, &obj4);
	if (obj3.isString() && obj4.isNum()) {
	  writePS(" ");
	  writePSString(obj3.getString());
	  writePS(" %g", obj4.getNum());
	}
	obj3.free();
	obj4.free();
      }
      writePS("\n");
    }
    obj2.free();
  }
  obj1.free();

  writePS("gsave\n");

  writePS("%%%%BeginIncludedImage\n");

  dict->lookup("IncludedImageDimensions", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 2) {
    obj1.arrayGet(0, &obj2);
    w = obj2.getInt();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    h = obj2.getInt();
    obj2.free();
    writePS("%%%%IncludedImageDimensions: %d %d\n", w, h);
  }
  obj1.free();

  dict->lookup("IncludedImageQuality", &obj1);
  if (obj1.isNum()) {
    writePS("%%%%IncludedImageQuality: %g\n", obj1.getNum());
  }
  obj1.free();

  ++opi20Nest;
}

void PSOutputDev::opiBegin13(GfxState *state, Dict *dict) {
  Object obj1, obj2;
  int left, right, top, bottom, samples, bits, width, height;
  double c, m, y, k;
  double llx, lly, ulx, uly, urx, ury, lrx, lry;
  double tllx, tlly, tulx, tuly, turx, tury, tlrx, tlry;
  double horiz, vert;
  int i, j;

  writePS("save\n");
  writePS("/opiMatrix2 matrix currentmatrix def\n");
  writePS("opiMatrix setmatrix\n");

  dict->lookup("F", &obj1);
  if (getFileSpec(&obj1, &obj2)) {
    writePS("%%ALDImageFileName: %s\n",
	    obj2.getString()->getCString());
    obj2.free();
  }
  obj1.free();

  dict->lookup("CropRect", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 4) {
    obj1.arrayGet(0, &obj2);
    left = obj2.getInt();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    top = obj2.getInt();
    obj2.free();
    obj1.arrayGet(2, &obj2);
    right = obj2.getInt();
    obj2.free();
    obj1.arrayGet(3, &obj2);
    bottom = obj2.getInt();
    obj2.free();
    writePS("%%ALDImageCropRect: %d %d %d %d\n", left, top, right, bottom);
  }
  obj1.free();

  dict->lookup("Color", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 5) {
    obj1.arrayGet(0, &obj2);
    c = obj2.getNum();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    m = obj2.getNum();
    obj2.free();
    obj1.arrayGet(2, &obj2);
    y = obj2.getNum();
    obj2.free();
    obj1.arrayGet(3, &obj2);
    k = obj2.getNum();
    obj2.free();
    obj1.arrayGet(4, &obj2);
    if (obj2.isString()) {
      writePS("%%ALDImageColor: %g %g %g %g ", c, m, y, k);
      writePSString(obj2.getString());
      writePS("\n");
    }
    obj2.free();
  }
  obj1.free();

  dict->lookup("ColorType", &obj1);
  if (obj1.isName()) {
    writePS("%%ALDImageColorType: %s\n", obj1.getName());
  }
  obj1.free();

  //~ ignores 'Comments' entry
  //~ need to handle multiple lines

  dict->lookup("CropFixed", &obj1);
  if (obj1.isArray()) {
    obj1.arrayGet(0, &obj2);
    ulx = obj2.getNum();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    uly = obj2.getNum();
    obj2.free();
    obj1.arrayGet(2, &obj2);
    lrx = obj2.getNum();
    obj2.free();
    obj1.arrayGet(3, &obj2);
    lry = obj2.getNum();
    obj2.free();
    writePS("%%ALDImageCropFixed: %g %g %g %g\n", ulx, uly, lrx, lry);
  }
  obj1.free();

  dict->lookup("GrayMap", &obj1);
  if (obj1.isArray()) {
    writePS("%%ALDImageGrayMap:");
    for (i = 0; i < obj1.arrayGetLength(); i += 16) {
      if (i > 0) {
	writePS("\n%%%%+");
      }
      for (j = 0; j < 16 && i+j < obj1.arrayGetLength(); ++j) {
	obj1.arrayGet(i+j, &obj2);
	writePS(" %d", obj2.getInt());
	obj2.free();
      }
    }
    writePS("\n");
  }
  obj1.free();

  dict->lookup("ID", &obj1);
  if (obj1.isString()) {
    writePS("%%ALDImageID: %s\n", obj1.getString()->getCString());
  }
  obj1.free();

  dict->lookup("ImageType", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 2) {
    obj1.arrayGet(0, &obj2);
    samples = obj2.getInt();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    bits = obj2.getInt();
    obj2.free();
    writePS("%%ALDImageType: %d %d\n", samples, bits);
  }
  obj1.free();

  dict->lookup("Overprint", &obj1);
  if (obj1.isBool()) {
    writePS("%%ALDImageOverprint: %s\n", obj1.getBool() ? "true" : "false");
  }
  obj1.free();

  dict->lookup("Position", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 8) {
    obj1.arrayGet(0, &obj2);
    llx = obj2.getNum();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    lly = obj2.getNum();
    obj2.free();
    obj1.arrayGet(2, &obj2);
    ulx = obj2.getNum();
    obj2.free();
    obj1.arrayGet(3, &obj2);
    uly = obj2.getNum();
    obj2.free();
    obj1.arrayGet(4, &obj2);
    urx = obj2.getNum();
    obj2.free();
    obj1.arrayGet(5, &obj2);
    ury = obj2.getNum();
    obj2.free();
    obj1.arrayGet(6, &obj2);
    lrx = obj2.getNum();
    obj2.free();
    obj1.arrayGet(7, &obj2);
    lry = obj2.getNum();
    obj2.free();
    opiTransform(state, llx, lly, &tllx, &tlly);
    opiTransform(state, ulx, uly, &tulx, &tuly);
    opiTransform(state, urx, ury, &turx, &tury);
    opiTransform(state, lrx, lry, &tlrx, &tlry);
    writePS("%%ALDImagePosition: %g %g %g %g %g %g %g %g\n",
	    tllx, tlly, tulx, tuly, turx, tury, tlrx, tlry);
    obj2.free();
  }
  obj1.free();

  dict->lookup("Resolution", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 2) {
    obj1.arrayGet(0, &obj2);
    horiz = obj2.getNum();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    vert = obj2.getNum();
    obj2.free();
    writePS("%%ALDImageResoution: %g %g\n", horiz, vert);
    obj2.free();
  }
  obj1.free();

  dict->lookup("Size", &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 2) {
    obj1.arrayGet(0, &obj2);
    width = obj2.getInt();
    obj2.free();
    obj1.arrayGet(1, &obj2);
    height = obj2.getInt();
    obj2.free();
    writePS("%%ALDImageSize: %d %d\n", width, height);
  }
  obj1.free();

  //~ ignoring 'Tags' entry
  //~ need to use writePSString() and deal with >255-char lines

  dict->lookup("Tint", &obj1);
  if (obj1.isNum()) {
    writePS("%%ALDImageTint: %g\n", obj1.getNum());
  }
  obj1.free();

  dict->lookup("Transparency", &obj1);
  if (obj1.isBool()) {
    writePS("%%ALDImageTransparency: %s\n", obj1.getBool() ? "true" : "false");
  }
  obj1.free();

  writePS("%%%%BeginObject: image\n");
  writePS("opiMatrix2 setmatrix\n");
  ++opi13Nest;
}

// Convert PDF user space coordinates to PostScript default user space
// coordinates.  This has to account for both the PDF CTM and the
// PSOutputDev page-fitting transform.
void PSOutputDev::opiTransform(GfxState *state, double x0, double y0,
			       double *x1, double *y1) {
  double t;

  state->transform(x0, y0, x1, y1);
  *x1 += tx;
  *y1 += ty;
  if (landscape) {
    t = *x1;
    *x1 = -*y1;
    *y1 = t;
  }
  *x1 *= xScale;
  *y1 *= yScale;
}

void PSOutputDev::opiEnd(GfxState *state, Dict *opiDict) {
  Object dict;

  if (psOutOPI) {
    opiDict->lookup("2.0", &dict);
    if (dict.isDict()) {
      writePS("%%%%EndIncludedImage\n");
      writePS("%%%%EndOPI\n");
      writePS("grestore\n");
      --opi20Nest;
      dict.free();
    } else {
      dict.free();
      opiDict->lookup("1.3", &dict);
      if (dict.isDict()) {
	writePS("%%%%EndObject\n");
	writePS("restore\n");
	--opi13Nest;
      }
      dict.free();
    }
  }
}

GBool PSOutputDev::getFileSpec(Object *fileSpec, Object *fileName) {
  if (fileSpec->isString()) {
    fileSpec->copy(fileName);
    return gTrue;
  }
  if (fileSpec->isDict()) {
    fileSpec->dictLookup("DOS", fileName);
    if (fileName->isString()) {
      return gTrue;
    }
    fileName->free();
    fileSpec->dictLookup("Mac", fileName);
    if (fileName->isString()) {
      return gTrue;
    }
    fileName->free();
    fileSpec->dictLookup("Unix", fileName);
    if (fileName->isString()) {
      return gTrue;
    }
    fileName->free();
    fileSpec->dictLookup("F", fileName);
    if (fileName->isString()) {
      return gTrue;
    }
    fileName->free();
  }
  return gFalse;
}
#endif // OPI_SUPPORT

void PSOutputDev::writePS(const char *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  vfprintf(f, fmt, args);
  va_end(args);
}

void PSOutputDev::writePSString(GString *s) {
  Guchar *p;
  int n;

  fputc('(', f);
  for (p = (Guchar *)s->getCString(), n = s->getLength(); n; ++p, --n) {
    if (*p == '(' || *p == ')' || *p == '\\')
      fprintf(f, "\\%c", *p);
    else if (*p < 0x20 || *p >= 0x80)
      fprintf(f, "\\%03o", *p);
    else
      fputc(*p, f);
  }
  fputc(')', f);
}
