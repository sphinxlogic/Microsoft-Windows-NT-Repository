/*
 *  xmark.c
 *
 *  This program implements the xmark benchmark.  It reads a x11perf output that
 *  has been generated by: xperf -rop "GXcopy" "GXxor" -all
 *
 *  It then turns these numbers into a benchmark number that measures the weighted
 *  averages against a SparcStation1.
 *
 *  This program is based on the version 1,15 dated 4/12/93.  The header from that
 *  script, including copyright notice is included below.
 *
 *  xmark.c edit history:
 *
 *  V1.15	Fred Kleinsorge, Digital Equipment Corporation, November 1994
 *
 * !############################################################
 * !# Xmark version $Revision: 1.15 $
 * !#
 * !# Usage: Xmark datafile
 * !#
 * !############################################################
 * !# CHANGE HISTORY:
 * !#
 * !# X11perfcompDR  --  Creates a Digital Review compatible breakdown of
 * !# x11perf v1.2 results as described in "Take Your Pick Of Graphics"
 * !# by John Montgomery, Digital Review, May 7th, 1990, Page 44.
 * !# 8/30/90  v1.0 program created by Lonnie Mandigo
 * !# 1/18/91  Modification by  Lonnie Mandigo
 * !#         Fixed a bug to include UCIRC in data.
 * !# 2/5/91   Modification by  Lonnie Mandigo
 * !#          Now removes sum.tmp file when finished.
 * !# 3/18/91  v1.01 Increased resolution to handle zero ratios.
 * !#          Minor aesethic cleanups and better error handling.
 * !#          Lonnie Mandigo and Jason Levitt (jason@cs.utexas.edu)
 * !# 3/4/93   Converted to Xmark by Bob Kuseski
 * !#          to produce a single number representing the ratio between
 * !#         the xpc weighted averages of the output from x11perf Rev 1.3
 * !#         for the server under test and a SparcStation 1
 * !# 3/10/93  Changed to produce THREE output numbers and added bounds
 * !#         checking of input data.
 * !# 3/15/93  Removed usage of '-F' in grep for compatibility reasons
 * !# 3/16/93  Corrected usage of substr() and used two greps versus fgrep
 * !# 4/12/93  Eliminated use of functions and \n in echo
 * !#
 * !############################################################
 * !# Copyright (c) 1993 by Hewlett-Packard Company
 * !#
 * !# Permission to use, copy, modify, and  distribute  this  software and its
 * !# documentation  for  any  purpose  and  without  fee is  hereby  granted,
 * !# provided that the above  copyright  notice appear in all copies and that
 * !# both  the  copyright  notice  and  this  permission   notice  appear  in
 * !# supporting  documentation, and that the name of  Hewlett-Packard  not be
 * !# used in  advertising  or publicity  pertaining  to  distribution  of the
 * !# software without specific, written prior permission.
 * !#
 * !############################################################
 * !# Instructions:
 * !#
 * !# Usage: Xmark datafile
 * !#
 * !# where: 'datafile' is created by running x11perf Rev 1.3
 * !# with the flags below:
 * !#
 * !#      x11perf -display mysystem:0.0 -rop GXcopy GXxor -all > datafile
 * !#   or
 * !#      x11perf -display mysystem:0.0 -rop GXcopy -all > datafile
 * !#      x11perf -display mysystem:0.0 -rop GXxor -all >> datafile
 * !#   or
 * !#      x11perf -display mysystem:0.0 -rop GXxor -all > datafile
 * !#      x11perf -display mysystem:0.0 -rop GXcopy -all >> datafile
 * !#
 * !# Xmark summarizes the results relative to the data contained in
 * !# the datafile.  Xmark writes to standard out, so if you want to
 * !# capture the output in a file, use:
 * !#
 * !#   Xmark datafile > output.comp
 * !#
 * !# The 'output.comp' contains THREE numbers:
 * !#          - Weighted x11perf number of the server under test
 * !#          - Weighted x11perf number of the SparcStation 1
 * !#          - Xmark = the ratio of the above two numbers
 * !#
 * !# Note the SparcStation 1 number comes from a:
 * !#   X11R5 Xsun
 * !#   Standard with SunOS 4.1.2
 * !#   SunOs 4.1.2
 * !#   CG3 dumb Color Frame Buffer
 * !#
 * !############################################################
 * 
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct weight_rec {
   int    valid;
   char   *name;
   int    weight;
   float  rate;
  } WEIGHT;

WEIGHT weights[] =
  {{1,"10x10 rectangle", 37, 0},
   {1,"Scroll 500x500 pixels", 33, 0},
   {1,"100x100 rectangle", 32, 0},
   {1,"10-pixel line segment", 31, 0},
   {1,"10-pixel line", 30, 0},
   {1,"PutImage 100x100 square", 30, 0},
   {1,"Scroll 100x100 pixels", 29, 0},
   {1,"PutImage 500x500 square", 29, 0},
   {1,"500x500 rectangle", 29, 0},
   {1,"100-pixel line", 28, 0},
   {1,"Char in 60-char line (9x15)", 28, 0},
   {1,"Char in 80-char image line (6x13)", 28, 0},
   {1,"Char in 80-char line (6x13)", 27, 0},
   {1,"Char in 80-char image line (TR 10)", 27, 0},
   {1,"100-pixel line segment", 27, 0},
   {1,"Char in 80-char line (TR 10)", 27, 0},
   {1,"Char in 60-char image line (9x15)", 26, 0},
   {1,"Copy 100x100 from pixmap to window", 26, 0},
   {1,"Copy 500x500 from pixmap to window", 26, 0},
   {1,"Char in 70-char line (8x13)", 24, 0},
   {1,"500-pixel line", 24, 0},
   {1,"Char in 20/40/20 line (6x13, TR 10)", 23, 0},
   {1,"Char in 70-char image line (8x13)", 23, 0},
   {1,"Change graphics context", 23, 0},
   {1,"Copy 100x100 from window to window", 23, 0},
   {1,"10-pixel horizontal line segment", 23, 0},
   {1,"10-pixel vertical line segment", 23, 0},
   {1,"Copy 500x500 from window to window", 22, 0},
   {1,"Char in 30-char line (TR 24)", 22, 0},
   {1,"Char16 in 40-char line (k14)", 22, 0},
   {1,"500-pixel line segment", 22, 0},
   {1,"Char in 30-char image line (TR 24)", 22, 0},
   {1,"100-pixel horizontal line segment", 22, 0},
   {1,"100-pixel vertical line segment", 22, 0},
   {1,"Scroll 10x10 pixels", 21, 0},
   {1,"Char16 in 40-char image line (k14)", 21, 0},
   {1,"GetImage 500x500 square", 21, 0},
   {1,"PutImage 10x10 square", 20, 0},
   {1,"GetImage 100x100 square", 20, 0},
   {1,"Move window (4 kids)", 20, 0},
   {1,"Copy 10x10 from pixmap to window", 20, 0},
   {1,"100x100 4x4 tiled rectangle", 20, 0},
   {1,"Copy 100x100 from window to pixmap", 19, 0},
   {1,"Hide/expose window via popup (4 kids)", 19, 0},
   {1,"Copy 500x500 from window to pixmap", 19, 0},
   {1,"500-pixel horizontal line segment", 19, 0},
   {1,"500-pixel vertical line segment", 19, 0},
   {1,"Fill 10x10 equivalent triangle", 19, 0},
   {1,"Char16 in 23-char line (k24)", 19, 0},
   {1,"1x1 rectangle", 19, 0},
   {1,"Char16 in 7/14/7 line (k14, k24)", 18, 0},
   {1,"10x10 4x4 tiled rectangle", 18, 0},
   {1,"100-pixel line segment (1 kid)", 18, 0},
   {1,"100x100 rectangle outline", 18, 0},
   {1,"Move window via parent (4 kids)", 18, 0},
   {1,"GetAtomName", 18, 0},
   {1,"GetProperty", 18, 0},
   {1,"Char16 in 23-char image line (k24)", 18, 0},
   {1,"100-pixel line segment (3 kids)", 17, 0},
   {1,"Resize window (4 kids)", 17, 0},
   {1,"Map window via parent (4 kids)", 17, 0},
   {1,"Copy 100x100 from pixmap to pixmap", 17, 0},
   {1,"Unmap window via parent (4 kids)", 17, 0},
   {1,"100-pixel line segment (2 kids)", 17, 0},
   {1,"10-pixel solid circle", 17, 0},
   {1,"1-pixel line", 17, 0},
   {1,"Move window (16 kids)", 16, 0},
   {1,"Fill 100x100 equivalent triangle", 16, 0},
   {1,"Hide/expose window via popup (16 kids)", 16, 0},
   {1,"Move window via parent (16 kids)", 16, 0},
   {1,"10x10 rectangle outline", 16, 0},
   {1,"Destroy window via parent (4 kids)", 16, 0},
   {1,"Create unmapped window (4 kids)", 16, 0},
   {1,"Copy 10x10 from window to window", 16, 0},
   {1,"Copy 100x100 1-bit deep plane", 16, 0},
   {1,"10-pixel circle", 16, 0},
   {1,"100x100 opaque stippled rectangle", 16, 0},
   {1,"500x500 4x4 tiled rectangle", 16, 0},
   {1,"100x100 stippled rectangle", 16, 0},
   {1,"Copy 500x500 from pixmap to pixmap", 16, 0},
   {1,"Create and map subwindows (16 kids)", 16, 0},
   {1,"10x10 stippled rectangle", 16, 0},
   {1,"Resize window (16 kids)", 16, 0},
   {1,"Map window via parent (16 kids)", 15, 0},
   {1,"100-pixel solid circle", 15, 0},
   {1,"Copy 500x500 1-bit deep plane", 15, 0},
   {1,"500x500 rectangle outline", 15, 0},
   {1,"Create and map subwindows (4 kids)", 15, 0},
   {1,"Destroy window via parent (16 kids)", 15, 0},
   {1,"10x1 wide horizontal line segment", 15, 0},
   {1,"10x1 wide vertical line segment", 15, 0},
   {1,"100x100 161x145 tiled rectangle", 15, 0},
   {1,"1-pixel line segment", 15, 0},
   {1,"Unmap window via parent (16 kids)", 15, 0},
   {1,"Fill 100x100 trapezoid", 14, 0},
   {1,"100-pixel circle", 14, 0},
   {1,"10x10 opaque stippled rectangle", 14, 0},
   {1,"100-pixel dashed line", 14, 0},
   {1,"500x500 161x145 tiled rectangle", 14, 0},
   {1,"Copy 10x10 from window to pixmap", 14, 0},
   {1,"100x10 wide horizontal line segment", 14, 0},
   {1,"100x10 wide vertical line segment", 14, 0},
   {1,"100x100 wide rectangle outline", 14, 0},
   {1,"100x100 216x208 tiled rectangle", 14, 0},
   {1,"Dot", 14, 0},
   {1,"10x10 161x145 tiled rectangle", 14, 0},
   {1,"Fill 100x100 equivalent complex polygons", 14, 0},
   {1,"Move window (50 kids)", 13, 0},
   {1,"Move window via parent (50 kids)", 13, 0},
   {1,"Circulate window (16 kids)", 13, 0},
   {1,"100-pixel dashed segment", 13, 0},
   {1,"Create and map subwindows (50 kids)", 13, 0},
   {1,"Create and map subwindows (25 kids)", 13, 0},
   {1,"10x10 wide rectangle outline", 13, 0},
   {1,"500x500 216x208 tiled rectangle", 13, 0},
   {1,"Hide/expose window via popup (25 kids)", 13, 0},
   {1,"Create and map subwindows (100 kids)", 13, 0},
   {1,"Map window via parent (50 kids)", 13, 0},
   {1,"Fill 10x10 trapezoid", 13, 0},
   {1,"100x10 wide line", 13, 0},
   {1,"10x10 216x208 tiled rectangle", 13, 0},
   {1,"10-pixel dashed line", 13, 0},
   {1,"Map window via parent (100 kids)", 13, 0},
   {1,"Copy 10x10 from pixmap to pixmap", 13, 0},
   {1,"Map window via parent (25 kids)", 13, 0},
   {1,"Create unmapped window (25 kids)", 13, 0},
   {1,"Move window (25 kids)", 13, 0},
   {1,"Circulate window (4 kids)", 12, 0},
   {1,"Move window via parent (25 kids)", 12, 0},
   {1,"Create unmapped window (16 kids)", 12, 0},
   {1,"Create and map subwindows (75 kids)", 12, 0},
   {1,"500x500 opaque stippled rectangle", 12, 0},
   {1,"Hide/expose window via popup (100 kids)", 12, 0},
   {1,"Create and map subwindows (200 kids)", 12, 0},
   {1,"Hide/expose window via popup (50 kids)", 12, 0},
   {1,"500x50 wide horizontal line segment", 12, 0},
   {1,"500x50 wide vertical line segment", 12, 0},
   {1,"100x100 161x145 opaque stippled rectangle", 12, 0},
   {1,"Resize window (25 kids)", 12, 0},
   {1,"Map window via parent (200 kids)", 12, 0},
   {1,"10x10 161x145 opaque stippled rectangle", 12, 0},
   {1,"X protocol NoOperation", 12, 0},
   {1,"10-pixel dashed segment", 12, 0},
   {1,"Resize unmapped window (16 kids)", 12, 0},
   {1,"Create unmapped window (100 kids)", 12, 0},
   {1,"Fill 10x10 equivalent complex polygon", 12, 0},
   {1,"Map window via parent (75 kids)", 12, 0},
   {1,"100x100 161x145 stippled rectangle", 12, 0},
   {1,"Moved unmapped window (16 kids)", 12, 0},
   {1,"Copy 10x10 1-bit deep plane", 12, 0},
   {1,"Unmap window via parent (50 kids)", 12, 0},
   {1,"GetImage 10x10 square", 12, 0},
   {1,"Fill 100x100 tiled trapezoid", 12, 0},
   {1,"500x500 wide rectangle outline", 12, 0},
   {1,"Fill 100x100 stippled trapezoid", 12, 0},
   {1,"500x500 stippled rectangle", 12, 0},
   {1,"Moved unmapped window (4 kids)", 12, 0},
   {1,"10x10 161x145 stippled rectangle", 12, 0},
   {1,"Unmap window via parent (25 kids)", 12, 0},
   {1,"500-pixel solid circle", 12, 0},
   {1,"Create unmapped window (50 kids)", 12, 0},
   {1,"Destroy window via parent (25 kids)", 11, 0},
   {1,"Resize window (200 kids)", 11, 0},
   {1,"100-pixel filled ellipse", 11, 0},
   {1,"Hide/expose window via popup (200 kids)", 11, 0},
   {1,"Unmap window via parent (100 kids)", 11, 0},
   {1,"Circulate window (25 kids)", 11, 0},
   {1,"Resize window (50 kids)", 11, 0},
   {1,"Hide/expose window via popup (75 kids)", 11, 0},
   {1,"Destroy window via parent (100 kids)", 11, 0},
   {1,"Destroy window via parent (50 kids)", 11, 0},
   {1,"Create unmapped window (200 kids)", 11, 0},
   {1,"Fill 100x100 opaque stippled trapezoid", 11, 0},
   {1,"Move window via parent (75 kids)", 11, 0},
   {1,"500-pixel filled ellipse", 11, 0},
   {1,"Resize unmapped window (4 kids)", 11, 0},
   {1,"Move window via parent (200 kids)", 11, 0},
   {1,"Move window (75 kids)", 11, 0},
   {1,"Fill 10x10 tiled trapezoid", 11, 0},
   {1,"Fill 100x100 161x145 stippled trapezoid", 11, 0},
   {1,"Move window (200 kids)", 11, 0},
   {1,"Fill 10x10 stippled trapezoid", 11, 0},
   {1,"Create unmapped window (75 kids)", 11, 0},
   {1,"100-pixel fill chord partial circle", 10, 0},
   {1,"Circulate Unmapped window (16 kids)", 10, 0},
   {1,"Circulate window (50 kids)", 10, 0},
   {1,"Move window (100 kids)", 10, 0},
   {1,"Circulate window (100 kids)", 10, 0},
   {1,"Move window via parent (100 kids)", 10, 0},
   {1,"500x50 wide line", 10, 0},
   {1,"500-pixel circle", 10, 0},
   {1,"100-pixel double-dashed line", 10, 0},
   {1,"Unmap window via parent (200 kids)", 10, 0},
   {1,"Moved unmapped window (50 kids)", 10, 0},
   {1,"10-pixel filled ellipse", 10, 0},
   {1,"Resize window (75 kids)", 10, 0},
   {1,"Fill 100x100 161x145 tiled trapezoid", 10, 0},
   {1,"500x500 161x145 opaque stippled rectangle", 10, 0},
   {1,"Fill 100x100 161x145 opaque stippled trapezoid", 10, 0},
   {1,"Unmap window via parent (75 kids)", 10, 0},
   {1,"Resize unmapped window (25 kids)", 10, 0},
   {1,"100-pixel ellipse", 10, 0},
   {1,"Destroy window via parent (75 kids)", 10, 0},
   {1,"Fill 1x1 equivalent triangle", 10, 0},
   {1,"500-pixel ellipse", 10, 0},
   {1,"Fill 10x10 opaque stippled trapezoid", 10, 0},
   {1,"Resize window (100 kids)", 10, 0},
   {1,"500x500 161x145 stippled rectangle", 10, 0},
   {1,"Destroy window via parent (200 kids)", 10, 0},
   {1,"100-pixel fill slice partial circle", 10, 0},
   {1,"Fill 100x100 216x208 tiled trapezoid", 9, 0},
   {1,"100-pixel wide circle", 9, 0},
   {1,"Moved unmapped window (25 kids)", 9, 0},
   {1,"10x1 wide line", 9, 0},
   {1,"Resize unmapped window (100 kids)", 9, 0},
   {1,"100-pixel double-dashed segment", 9, 0},
   {1,"10-pixel fill chord partial circle", 9, 0},
   {1,"Circulate window (75 kids)", 9, 0},
   {1,"100-pixel partial circle", 9, 0},
   {1,"Circulate window (200 kids)", 9, 0},
   {1,"Fill 10x10 161x145 stippled trapezoid", 9, 0},
   {1,"Circulate Unmapped window (4 kids)", 9, 0},
   {1,"100-pixel dashed circle", 9, 0},
   {1,"Circulate Unmapped window (25 kids)", 9, 0},
   {1,"10-pixel ellipse", 9, 0},
   {1,"Resize unmapped window (50 kids)", 9, 0},
   {1,"100-pixel partial ellipse", 9, 0},
   {1,"Fill 10x10 161x145 tiled trapezoid", 8, 0},
   {1,"100x10 wide dashed line", 8, 0},
   {1,"10-pixel partial circle", 8, 0},
   {1,"100-pixel wide ellipse", 8, 0},
   {1,"Fill 10x10 161x145 opaque stippled trapezoid", 8, 0},
   {1,"100-pixel wide partial circle", 8, 0},
   {1,"Moved unmapped window (75 kids)", 8, 0},
   {1,"Resize unmapped window (75 kids)", 8, 0},
   {1,"Circulate Unmapped window (100 kids)", 8, 0},
   {1,"100-pixel fill slice partial ellipse", 8, 0},
   {1,"100x10 wide double-dashed line", 8, 0},
   {1,"10-pixel fill slice partial circle", 8, 0},
   {1,"100-pixel dashed ellipse", 8, 0},
   {1,"100-pixel fill chord partial ellipse", 8, 0},
   {1,"100-pixel wide dashed circle", 8, 0},
   {1,"100-pixel double-dashed circle", 8, 0},
   {1,"Fill 10x10 216x208 tiled trapezoid", 7, 0},
   {1,"Moved unmapped window (100 kids)", 7, 0},
   {1,"10-pixel wide circle", 7, 0},
   {1,"Moved unmapped window (200 kids)", 7, 0},
   {1,"Resize unmapped window (200 kids)", 7, 0},
   {1,"Circulate Unmapped window (50 kids)", 7, 0},
   {1,"1-pixel circle", 7, 0},
   {1,"10-pixel partial ellipse", 7, 0},
   {1,"500-pixel wide circle", 7, 0},
   {1,"500-pixel wide ellipse", 7, 0},
   {1,"100-pixel wide partial ellipse", 7, 0},
   {1,"Circulate Unmapped window (75 kids)", 7, 0},
   {1,"100-pixel wide dashed ellipse", 7, 0},
   {1,"100-pixel double-dashed ellipse", 7, 0},
   {1,"10-pixel wide ellipse", 6, 0},
   {1,"10-pixel wide partial circle", 6, 0},
   {1,"1-pixel solid circle", 6, 0},
   {1,"100-pixel wide double-dashed circle", 6, 0},
   {1,"Circulate Unmapped window (200 kids)", 6, 0},
   {1,"1x1 4x4 tiled rectangle", 6, 0},
   {1,"10-pixel wide partial ellipse", 6, 0},
   {1,"10-pixel fill chord partial ellipse", 6, 0},
   {1,"10-pixel fill slice partial ellipse", 6, 0},
   {1,"100-pixel wide double-dashed ellipse", 5, 0},
   {1,"1x1 161x145 tiled rectangle", 5, 0},
   {1,"1x1 216x208 tiled rectangle", 5, 0},
   {1,"1x1 stippled rectangle", 5, 0},
   {1,"1x1 opaque stippled rectangle", 4, 0},
   {1,"1x1 161x145 opaque stippled rectangle", 4, 0},
   {1,"1x1 161x145 stippled rectangle", 4, 0},
   {1,"ShmPutImage 10x10 square", 0, 0},
   {1,"ShmPutImage 100x100 square", 0, 0},
   {1,"ShmPutImage 500x500 square", 0, 0},
   {1,"(xor) 100-pixel line", 16, 0},
   {1,"(xor) 10-pixel line segment", 14, 0},
   {1,"(xor) 10-pixel line", 13, 0},
   {1,"(xor) 100-pixel line segment", 13, 0},
   {1,"(xor) 500-pixel line segment", 13, 0},
   {1,"(xor) 500-pixel line", 12, 0},
   {1,"(xor) 10x10 rectangle", 12, 0},
   {1,"(xor) 100x100 rectangle", 12, 0},
   {1,"(xor) 100-pixel vertical line segment", 10, 0},
   {1,"(xor) 100x100 rectangle outline", 10, 0},
   {1,"(xor) 100-pixel horizontal line segment", 10, 0},
   {1,"(xor) 10-pixel vertical line segment", 10, 0},
   {1,"(xor) 100-pixel line segment (2 kids)", 10, 0},
   {1,"(xor) 10-pixel horizontal line segment", 10, 0},
   {1,"(xor) 100-pixel line segment (1 kid)", 9, 0},
   {1,"(xor) Char in 80-char line (6x13)", 9, 0},
   {1,"(xor) 500-pixel vertical line segment", 9, 0},
   {1,"(xor) Copy 100x100 from pixmap to window", 9, 0},
   {1,"(xor) 10x10 rectangle outline", 9, 0},
   {1,"(xor) 500-pixel horizontal line segment", 9, 0},
   {1,"(xor) 100-pixel line segment (3 kids)", 9, 0},
   {1,"(xor) PutImage 100x100 square", 9, 0},
   {1,"(xor) 500x500 rectangle outline", 9, 0},
   {1,"(xor) Char in 80-char line (TR 10)", 9, 0},
   {1,"(xor) 500x500 rectangle", 9, 0},
   {1,"(xor) Copy 500x500 from pixmap to window", 8, 0},
   {1,"(xor) Char in 20/40/20 line (6x13, TR 10)", 8, 0},
   {1,"(xor) 100-pixel dashed line", 8, 0},
   {1,"(xor) Copy 100x100 1-bit deep plane", 8, 0},
   {1,"(xor) Char in 60-char line (9x15)", 8, 0},
   {1,"(xor) Char16 in 40-char line (k14)", 7, 0},
   {1,"(xor) Fill 10x10 equivalent triangle", 7, 0},
   {1,"(xor) PutImage 500x500 square", 7, 0},
   {1,"(xor) Copy 100x100 from window to window", 7, 0},
   {1,"(xor) 100-pixel dashed segment", 7, 0},
   {1,"(xor) 100x100 wide rectangle outline", 6, 0},
   {1,"(xor) Char16 in 7/14/7 line (k14, k24)", 6, 0},
   {1,"(xor) Fill 100x100 trapezoid", 6, 0},
   {1,"(xor) 100-pixel solid circle", 6, 0},
   {1,"(xor) 10-pixel solid circle", 6, 0},
   {1,"(xor) 1-pixel line segment", 6, 0},
   {1,"(xor) 1-pixel line", 6, 0},
   {1,"(xor) 10x1 wide horizontal line segment", 6, 0},
   {1,"(xor) 10x1 wide vertical line segment", 6, 0},
   {1,"(xor) Copy 100x100 from pixmap to pixmap", 6, 0},
   {1,"(xor) 10-pixel dashed line", 6, 0},
   {1,"(xor) Char in 30-char line (TR 24)", 6, 0},
   {1,"(xor) Fill 100x100 equivalent triangle", 6, 0},
   {1,"(xor) Copy 10x10 from pixmap to window", 6, 0},
   {1,"(xor) Fill 10x10 trapezoid", 6, 0},
   {1,"(xor) Char in 70-char line (8x13)", 6, 0},
   {1,"(xor) 100-pixel circle", 6, 0},
   {1,"(xor) Copy 100x100 from window to pixmap", 6, 0},
   {1,"(xor) 10-pixel dashed segment", 5, 0},
   {1,"(xor) 10-pixel circle", 5, 0},
   {1,"(xor) 10x10 wide rectangle outline", 5, 0},
   {1,"(xor) 100x100 stippled rectangle", 5, 0},
   {1,"(xor) 100-pixel filled ellipse", 5, 0},
   {1,"(xor) 10x10 4x4 tiled rectangle", 5, 0},
   {1,"(xor) PutImage 10x10 square", 5, 0},
   {1,"(xor) Copy 500x500 from window to window", 5, 0},
   {1,"(xor) 500x500 wide rectangle outline", 5, 0},
   {1,"(xor) 10x10 stippled rectangle", 5, 0},
   {1,"(xor) 100x10 wide line", 5, 0},
   {1,"(xor) 100x10 wide horizontal line segment", 5, 0},
   {1,"(xor) 100x10 wide vertical line segment", 5, 0},
   {1,"(xor) Scroll 100x100 pixels", 5, 0},
   {1,"(xor) Char16 in 23-char line (k24)", 5, 0},
   {1,"(xor) Dot", 5, 0},
   {1,"(xor) Copy 500x500 1-bit deep plane", 5, 0},
   {1,"(xor) 100-pixel ellipse", 5, 0},
   {1,"(xor) 100x100 4x4 tiled rectangle", 5, 0},
   {1,"(xor) Copy 10x10 1-bit deep plane", 5, 0},
   {1,"(xor) 1x1 rectangle", 5, 0},
   {1,"(xor) 500-pixel solid circle", 4, 0},
   {1,"(xor) 500-pixel filled ellipse", 4, 0},
   {1,"(xor) 10-pixel filled ellipse", 4, 0},
   {1,"(xor) Fill 100x100 stippled trapezoid", 4, 0},
   {1,"(xor) 500x50 wide line", 4, 0},
   {1,"(xor) 500-pixel circle", 4, 0},
   {1,"(xor) 100-pixel double-dashed line", 4, 0},
   {1,"(xor) 500x50 wide horizontal line segment", 4, 0},
   {1,"(xor) 500x50 wide vertical line segment", 4, 0},
   {1,"(xor) 100-pixel dashed circle", 4, 0},
   {1,"(xor) 10x10 opaque stippled rectangle", 4, 0},
   {1,"(xor) 10-pixel ellipse", 4, 0},
   {1,"(xor) Copy 500x500 from pixmap to pixmap", 4, 0},
   {1,"(xor) 100x100 161x145 stippled rectangle", 4, 0},
   {1,"(xor) 10x10 161x145 tiled rectangle", 4, 0},
   {1,"(xor) Copy 10x10 from window to window", 4, 0},
   {1,"(xor) Copy 10x10 from pixmap to pixmap", 4, 0},
   {1,"(xor) 10x10 161x145 stippled rectangle", 4, 0},
   {1,"(xor) 100-pixel wide circle", 4, 0},
   {1,"(xor) 500x500 stippled rectangle", 4, 0},
   {1,"(xor) 500-pixel ellipse", 4, 0},
   {1,"(xor) 10x10 216x208 tiled rectangle", 4, 0},
   {1,"(xor) Fill 100x100 equivalent complex polygons", 4, 0},
   {1,"(xor) 100-pixel double-dashed segment", 3, 0},
   {1,"(xor) 10x10 161x145 opaque stippled rectangle", 3, 0},
   {1,"(xor) Fill 10x10 opaque stippled trapezoid", 3, 0},
   {1,"(xor) 100-pixel partial circle", 3, 0},
   {1,"(xor) Fill 10x10 161x145 stippled trapezoid", 3, 0},
   {1,"(xor) Fill 10x10 161x145 opaque stippled trapezoid", 3, 0},
   {1,"(xor) Scroll 500x500 pixels", 3, 0},
   {1,"(xor) 100x100 opaque stippled rectangle", 3, 0},
   {1,"(xor) Fill 10x10 stippled trapezoid", 3, 0},
   {1,"(xor) 100x10 wide dashed line", 3, 0},
   {1,"(xor) Copy 10x10 from window to pixmap", 3, 0},
   {1,"(xor) Copy 500x500 from window to pixmap", 3, 0},
   {1,"(xor) 500x500 opaque stippled rectangle", 3, 0},
   {1,"(xor) Fill 10x10 tiled trapezoid", 3, 0},
   {1,"(xor) 100x100 161x145 tiled rectangle", 3, 0},
   {1,"(xor) 100-pixel partial ellipse", 3, 0},
   {1,"(xor) 100-pixel fill slice partial circle", 3, 0},
   {1,"(xor) Fill 1x1 equivalent triangle", 3, 0},
   {1,"(xor) 100-pixel double-dashed circle", 3, 0},
   {1,"(xor) 500x500 4x4 tiled rectangle", 3, 0},
   {1,"(xor) 100-pixel wide ellipse", 3, 0},
   {1,"(xor) 100-pixel fill chord partial circle", 3, 0},
   {1,"(xor) 100x100 216x208 tiled rectangle", 3, 0},
   {1,"(xor) Fill 10x10 161x145 tiled trapezoid", 3, 0},
   {1,"(xor) Fill 100x100 216x208 tiled trapezoid", 3, 0},
   {1,"(xor) 500x500 161x145 stippled rectangle", 3, 0},
   {1,"(xor) 500x500 161x145 tiled rectangle", 3, 0},
   {1,"(xor) 100x100 161x145 opaque stippled rectangle", 3, 0},
   {1,"(xor) 500x500 161x145 opaque stippled rectangle", 3, 0},
   {1,"(xor) 10x1 wide line", 3, 0},
   {1,"(xor) 500x500 216x208 tiled rectangle", 3, 0},
   {1,"(xor) 100-pixel dashed ellipse", 3, 0},
   {1,"(xor) Fill 100x100 opaque stippled trapezoid", 3, 0},
   {1,"(xor) 10-pixel partial circle", 3, 0},
   {1,"(xor) 100x10 wide double-dashed line", 3, 0},
   {1,"(xor) Fill 100x100 161x145 stippled trapezoid", 3, 0},
   {1,"(xor) Fill 100x100 161x145 opaque stippled trapezoid", 3, 0},
   {1,"(xor) 100-pixel fill slice partial ellipse", 3, 0},
   {1,"(xor) 1-pixel circle", 3, 0},
   {1,"(xor) Fill 10x10 equivalent complex polygon", 3, 0},
   {1,"(xor) 100-pixel wide dashed circle", 2, 0},
   {1,"(xor) 100-pixel wide double-dashed circle", 2, 0},
   {1,"(xor) Scroll 10x10 pixels", 2, 0},
   {1,"(xor) 10-pixel wide circle", 2, 0},
   {1,"(xor) 100-pixel fill chord partial ellipse", 2, 0},
   {1,"(xor) Fill 100x100 tiled trapezoid", 2, 0},
   {1,"(xor) 100-pixel double-dashed ellipse", 2, 0},
   {1,"(xor) 100-pixel wide dashed ellipse", 2, 0},
   {1,"(xor) 100-pixel wide double-dashed ellipse", 2, 0},
   {1,"(xor) 10-pixel partial ellipse", 2, 0},
   {1,"(xor) 100-pixel wide partial circle", 2, 0},
   {1,"(xor) 100-pixel wide partial ellipse", 2, 0},
   {1,"(xor) 10-pixel fill slice partial circle", 2, 0},
   {1,"(xor) 10-pixel wide ellipse", 2, 0},
   {1,"(xor) Fill 100x100 161x145 tiled trapezoid", 2, 0},
   {1,"(xor) Fill 10x10 216x208 tiled trapezoid", 2, 0},
   {1,"(xor) 10-pixel fill chord partial circle", 2, 0},
   {1,"(xor) 500-pixel wide circle", 2, 0},
   {1,"(xor) 500-pixel wide ellipse", 2, 0},
   {1,"(xor) 1-pixel solid circle", 2, 0},
   {1,"(xor) 10-pixel fill chord partial ellipse", 2, 0},
   {1,"(xor) 10-pixel fill slice partial ellipse", 2, 0},
   {1,"(xor) 10-pixel wide partial circle", 1, 0},
   {1,"(xor) 10-pixel wide partial ellipse", 1, 0},
   {1,"(xor) 1x1 stippled rectangle", 1, 0},
   {1,"(xor) 1x1 161x145 stippled rectangle", 1, 0},
   {1,"(xor) 1x1 opaque stippled rectangle", 1, 0},
   {1,"(xor) 1x1 161x145 opaque stippled rectangle", 1, 0},
   {1,"(xor) 1x1 4x4 tiled rectangle", 1, 0},
   {1,"(xor) 1x1 161x145 tiled rectangle", 1, 0},
   {1,"(xor) 1x1 216x208 tiled rectangle", 1, 0},
   {1,"(xor) ShmPutImage 10x10 square", 0, 0},
   {1,"(xor) ShmPutImage 100x100 square", 0, 0},
   {1,"(xor) ShmPutImage 500x500 square", 0, 0},
   {-1, "END", 0, 0}
 };

int
find(char *sub, int sub_l, char *str, int str_l)
{
  int i, found;

  for (i = 0; i <= (str_l - sub_l); i += 1) {
    if (*str == *sub) {
      found = strncmp(sub, str, sub_l);
      if (found == 0) return (i);
      }
      *str++;
    }
  return (-1);
}

main(argc, argv)
  int argc;
  char **argv;
  {
    FILE *xperf_file;
    int status,record_length, position1, position2, match, i, j, SumOfWeights = 0, server_name = 0;
    float rate;
    double temp, temp2, logsum = 0, WeightedAverage, Sparc1 = 2118.51, divsum = 4566.0, xmark;
    char record[256], work[256], server_nbuf[256], *pRecord;
    char default_name[] = "*Unknown*";
    char *server = &default_name[0];

    xperf_file = fopen(argv[1], "r");

    if (!xperf_file) {
      printf("Error opening %s\n", argv[1]);
      return(1);
      }

    while (1) {

      pRecord = fgets(&record[0], 255, xperf_file);

      if (pRecord == 0) break;
      record_length = strlen(&record[0]) - 1;
      record[record_length] = 0;

      if (!server_name) {
        position1 = find("server", 4, &record[0], record_length);
        if (position1 >= 0) {
          strncpy(&server_nbuf[0], &record[0], position1);
	  server_nbuf[position1] = 0;
          server = &server_nbuf[0];
          server_name = 1;
          }
        }        

      position1 = find("trep", 4, &record[0], record_length);
      if (position1 >= 0) {
        position1 = find("(", 1, &record[0], record_length);
        position2 = find("/", 1, &record[0], record_length);
        if ((position1 < 0) || (position2 < 0) || (position2 <= position1)) {
          printf("Error: Could not find performance data in record: <%s>\n", &record[0]);
          return (1);
          }
        strncpy(&work[0], &record[position1+1], position2-(position1+1));
        work[position2-(position1+1)] = 0;
        rate = 0.0;
        sscanf(&work[0], "%f", &rate);
        position1 = find(":", 1, &record[0], record_length);
        if (position1 >= 0) {
          position1 += 1;
          while (record[position1] == 32) position1 += 1;
          j = 0;
          while (weights[j].valid != -1) {
            if (weights[j].valid == 1) {
	      match = -1;
              if (strlen(weights[j].name) == record_length-position1) {
                match = strncmp(&record[position1], weights[j].name, record_length-position1);
                }
              if (match == 0) {
                weights[j].rate  = rate;
                weights[j].valid = 0;
                break;
                } /* if (match == 0) */
              } /* if (weights[j].valid = 1) */
              j += 1;
            } /* while (weights[j].valid != -1) */
          } /* if (position1 >= 0) */
        } /* if (position1 >= 0) */
      } /* while (1) */

    j = 0;
    while (weights[j].valid != -1) {
      if (weights[j].valid == 0) {
        SumOfWeights += weights[j].weight;
        if ((weights[j].rate > 0.0) && (weights[j].weight > 0)) {
          logsum += log(weights[j].rate) * weights[j].weight;
          }
        }
      j += 1;
      }

    if (SumOfWeights != 4566) {
      printf("Sum of weights is not equal to 4566, computed: %d\n", SumOfWeights);
      printf("xmark value is suspect, check to make sure xperf file is valid.\n", SumOfWeights);
      }

    WeightedAverage = exp(logsum/divsum);
    printf ("Weighted x11perf of %s =%6.0f\n", server, WeightedAverage);
    printf ("Weighted x11perf of Sparcstation 1 server =%5.0f\n", Sparc1);
    xmark = WeightedAverage/Sparc1;
    printf ("Xmark =%8.4f\n", xmark);
    }
