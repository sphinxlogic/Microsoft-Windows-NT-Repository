TrueType developer information files in this directory:

1. The TrueType specifications (in two formats):

   The TrueType Font File specifications detail how to construct
   a TrueType font from scratch (or build a tool to do so), the 
   TrueType programming language, and the complete format of each
   sub-table contained in the .TTF file complete with illustrations.

   a. In Windows 3.1 help format:
      The "ttf_spec.hlp" file is a hyper-text based version of the
      specifications which makes an ideal online reference
      guide to in internals of a TrueType font.  If you have Windows 
      3.1, you have all of the tools necessary to view or print this 
      online guide (no other programs are required).  You can even make 
      this into an icon for your desktop by doing a "File:New" from 
      the Windows 3.1 Program Manager and just pointing the command 
      line argument to the ttf_spec.hlp file.  Double-clicking on 
      that icon will automatically launch winhelp.exe and open the
      ttf_spec.hlp file.

   b. In Word for Windows 2.0 format:
	ttspec1.zip
	ttspec2.zip
	ttspec3.zip
      These three compressed files contain the exact same information
      which is present in the online help file mentioned above.
      This version would allow you to easily print out the entire
      400 page book.  These documents are stored in Word for Windows 
      2.0 format and require Windows 3.1 for printing.  See the 
      "readme.doc" (in ttspec1.zip) for printing instructions.  
      Requires 2.5MB after uncompression.

2. ttfdump.zip  - An MS-DOS executable which will dump the contents
   of a TrueType font out in a human-readable fashion.  It allows 
   you to dump the entire font, or just specific sub-tables.  This
   tool, combined with the specifications above, allows very effective
   debugging or exploration of any TrueType font.  For example, to
   dump the contents of the 'cmap' (character code to glyph index
   mapping) table, enter:

	ttfdump fontname.ttf -tcmap -nx

   Entering "ttfdump" with no options will give you a help message.

3. ttfname.zip  - Example C source code on how to parse the contents
   of a TrueType font.  Although this particular example will open up 
   the file and locate the font name contained within the 'name' table,
   it could be readily adapted to parse any other structure in the 
   file.  This compressed zip file also contains many useful include 
   files which have pre-defined structures set up for the internal 
   tables of a TrueType font file.  This code may be useful for 
   developers who wish to parse the TrueType data stream returned
   by the GetFontData() API in Windows 3.1.

4. tt-win.zip   - A 31 page Word for Windows 2.0 document which is
   targeted for the Windows developer who is interested in learning
   about some of the capabilities TrueType adds to Windows 3.1.
   Contains many illustrations.

5. embeddin.zip - A text file which describes all of the information
   necessary for a Windows developer to add TrueType font embedding
   capabilities to their application.  Font embedding allows the 
   application to bundle the TrueType fonts that were used in that
   document and transport it to another platform where the document
   can be viewed or printed correctly.

6. tt-talk.zip  - The TrueType Technical Talks 1 and 2.  These
   text files describe some of the things that are happening
   with TrueType behind the scenes in Windows 3.1.  The first
   document walks the reader through all of the steps that occur
   from when the user first presses the key on the keyboard until
   that character appears on the screen (scaling, hinting, drop
   out control, caching and blitting).  The second talk describes
   one of the unique features of TrueType called non-linear scaling
   which allows the font vendor to overcome some of the physical
   limitations of low resolution output devices.

7. lucida.zip  - This text file contains useful typographic 
   information on the 22 Lucida fonts which are contained in the
   Microsoft TrueType Font Pack for Windows.  It gives pointers
   on line-layout, mixing and matching fonts in the family and
   a little history on each typeface.  This information was
   written by the font's designers, Chuck Bigelow & Kris Holmes.
