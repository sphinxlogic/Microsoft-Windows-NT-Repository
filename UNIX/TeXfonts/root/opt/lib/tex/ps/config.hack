* Configuration file for our LaserJet IIP.

*   We have 2 2MB expansion cards.
*
m 2750000


*   Send output by default to our printer.
*
* o !lpr


*   Default resolution.
*
D 1270

Z


*   Reverse the pages, since we usually print on the face-up
*   tray.  But psrev already does this for us, so forget it.
* r


* Mode is for Canon engine.
*
M CanonCX


* Also look for this list of resolutions.
*
R 1270


* The printer offsets the output by this much.
*
O 0pt,-6pt


*   Paper size information.  First definition is the default.
*
*   If your default is a4 uncomment the following definition
*   and comment out the letterSize definition.
*
*   There is some redundancy in the following comments; that is to
*   get things to work both on the NeXT and on other printers.
*
@ a4size 210mm 297mm
@+ %%PaperSize: a4

@ letterSize 8.5in 11in

@ letter 8.5in 11in
@+ %%PaperSize: Letter
@+ %%BeginPaperSize: Letter
@+ letter
@+ %%EndPaperSize

@ legal 8.5in 14in
@+ ! %%DocumentPaperSizes: Legal
@+ %%PaperSize: Legal
@+ %%BeginPaperSize: Legal
@+ legal
@+ %%EndPaperSize

@ ledger 17in 11in
@+ ! %%DocumentPaperSizes: Ledger
@+ %%PaperSize: Ledger
@+ %%BeginPaperSize: Ledger
@+ ledger
@+ %%EndPaperSize

@ tabloid 11in 17in
@+ ! %%DocumentPaperSizes: Tabloid
@+ %%PaperSize: Tabloid
@+ %%BeginPaperSize: Tabloid
@+ 11x17
@+ %%EndPaperSize

@ a4 210mm 297mm
@+ ! %%DocumentPaperSizes: a4
@+ %%PaperSize: a4
@+ %%BeginPaperSize: a4
@+ a4
@+ %%EndPaperSize

@ a3 297mm 420mm
@+ ! %%DocumentPaperSizes: a3
@+ %%PaperSize: a3
@+ %%BeginPaperSize: a3
@+ a3
@+ %%EndPaperSize
