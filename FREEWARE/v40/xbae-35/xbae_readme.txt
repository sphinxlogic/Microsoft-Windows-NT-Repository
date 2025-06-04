Xbae Library Version 3.5 (see src/version.c)
Xbae 3.5 is the first public release.

The Xbae library is a set of two Motif widgets, XbaeMatrix and
XbaeCaption.  It was developed using X11R4/X11R5 and Motif 1.1.[0-4]
under SunOS 4.1.1.  It has also been built on an IBM RS/6000 running
AIX 3.2 and a Pyramid running OSx 5.1 (under the bsd universe).  It
should be pretty portable to other platforms.

To build the release:
  1) Edit Xbae.tmpl and change HaveWcl to NO if you don't have Wcl.
  2) xmkmf
  3) make World
  4) cd to 'examples' and "runtest" to see if things are working
  5) make install


What's in the release:
       src - the source code for XbaeMatrix and XbaeCaption
       doc - PostScript documents for XbaeMatrix and XbaeCaption
  examples - some simple examples using XbaeMatrix and XbaeCaption

What the widget's do:

 XbaeMatrix:

   XbaeMatrix is a Motif widget which presents an editable array of
   string data to the user in a scrollable table similar to a
   spreadsheet. The rows and columns of the Matrix may optionally be
   labeled. Also, a number of "fixed" leading rows or columns may be
   specified - these behave similarly to the labels. While XbaeMatrix
   looks and acts like a grid of XmTextField widgets, it actually
   contains only one XmTextField.  This means that XbaeMatrix widgets
   with hundreds or thousands of rows have much less overhead than they
   would if they used an XmTextField for each cell. XbaeMatrix has
   callbacks for doing field validation and customizing traversal. It
   allows cells to be assigned independent colors.  It allows rows,
   columns and regions of cells to be selected (highlighted).  The matrix
   can be dynamically grown or shrunk by adding and deleting rows and
   columns at any position.

 XbaeCaption:

   XbaeCaption is a simple Motif manager widget used to associate an
   XmLabel (caption) with it's single child. The label may be either an
   XmString or Pixmap and can be displayed in any one of twelve positions
   around the perimeter of the child. XbaeCaption's geometry management
   technique is to simply "shrink wrap" it's child and display the
   caption alongside it.  By using XbaeCaption with an XmFrame child,
   groups of related widgets can be labeled similarly to the IBM CUA
   "Group Box", but with more flexibility.  XbaeCaption is also useful
   for associating labels with individual XmTextField widgets.
