$ define X11xaw SUB3:[DECW.GHOSTVIEW23.xaw3d]
$ define X11xmu SUB3:[DECW.GHOSTVIEW23.xmu]
$ define XAW_DIRECTORY SUB3:[DECW.GHOSTVIEW23.xaw3d]
$ define XMU_DIRECTORY SUB3:[DECW.GHOSTVIEW23.xmu]
$ define X11_LIBRARY SUB3:[DECW.GHOSTVIEW23.exe]
$ define X11_DIRECTORY decw$include:
$ define sys sys$library:
$ CC/DEFINE=VMS/INCLUDE=[] ARCOP.C
$ CC/DEFINE=VMS/INCLUDE=[] BLOBOP.C
$ CC/DEFINE=VMS/INCLUDE=[] BOXOP.C
$ CC/DEFINE=VMS/INCLUDE=[] BRUSHOP.C
$ CC/DEFINE=VMS/INCLUDE=[] CHROMA.C
$ CC/DEFINE=VMS/INCLUDE=[] CIRCLEOP.C
$ CC/DEFINE=VMS/INCLUDE=[] COLOR.C
$ CC/DEFINE=VMS/INCLUDE=[] COLOREDIT.C
$ CC/DEFINE=VMS/INCLUDE=[] COLORMAP.C
$ CC/DEFINE=VMS/INCLUDE=[] CUTCOPYPASTE.C
$ CC/DEFINE=VMS/INCLUDE=[] DIALOG.C
$ CC/DEFINE=VMS/INCLUDE=[] FATBITSEDIT.C
$ CC/DEFINE=VMS/INCLUDE=[] FILENAME.C
$ CC/DEFINE=VMS/INCLUDE=[] FILLOP.C
$ CC/DEFINE=VMS/INCLUDE=[] FONTOP.C
$ CC/DEFINE=VMS/INCLUDE=[] FONTSELECT.C
$ CC/DEFINE=VMS/INCLUDE=[] GRAB.C
$ CC/DEFINE=VMS/INCLUDE=[] GRAPHIC.C
$ CC/DEFINE=VMS/INCLUDE=[] HASH.C
$ CC/DEFINE=VMS/INCLUDE=[] HELP.C
$ CC/DEFINE=VMS/INCLUDE=[] IMAGE.C
$ CC/DEFINE=VMS/INCLUDE=[] IMAGECOMP.C
$ CC/DEFINE=VMS/INCLUDE=[] IPROCESS.C
$ CC/DEFINE=VMS/INCLUDE=[] LINEOP.C
$ CC/DEFINE=VMS/INCLUDE=[] MAIN.C
$ CC/DEFINE=VMS/INCLUDE=[] MENU.C
$ CC/DEFINE=VMS/INCLUDE=[] MISC.C
$ CC/DEFINE=VMS/INCLUDE=([],[.BITMAPS]) OPERATION.C
$ CC/DEFINE=VMS/INCLUDE=[] PAINT.C
$ CC/DEFINE=VMS/INCLUDE=[] PAINTEVENT.C
$ CC/DEFINE=VMS/INCLUDE=[] PAINTREGION.C
$ CC/DEFINE=VMS/INCLUDE=[] PAINTUNDO.C
$ CC/DEFINE=VMS/INCLUDE=[] PALETTE.C
$ CC/DEFINE=VMS/INCLUDE=[] PATTERN.C
$ CC/DEFINE=VMS/INCLUDE=[] PENCILOP.C
$ CC/DEFINE=VMS/INCLUDE=[] POLYOP.C
$ CC/DEFINE=VMS/INCLUDE=[] PROTOCOL.C
$ CC/DEFINE=VMS/INCLUDE=[] READRC.C
$ CC/DEFINE=VMS/INCLUDE=[] SELECTOP.C
$ CC/DEFINE=VMS/INCLUDE=[] SIZE.C
$ CC/DEFINE=VMS/INCLUDE=[] SPRAYOP.C
$ CC/DEFINE=VMS/INCLUDE=[] TEXT.C
$ CC/DEFINE=VMS/INCLUDE=[] TEXTURE.C
$ CC/DEFINE=VMS/INCLUDE=[] TYPECONVERT.C
