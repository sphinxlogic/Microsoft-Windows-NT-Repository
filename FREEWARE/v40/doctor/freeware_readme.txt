DOCTOR, UTILITIES, Modify existing postscript files.

DOCTOR is a collection of seven tools under one umbrella:

       DOCTOR/GLOSSARY
       DOCTOR/MESSAGE
       DOCTOR/ONLINE
       DOCTOR/PS
       DOCTOR/SDML
       DOCTOR/TAG_COUNT
       DOCTOR/XREF

Each working on different files used or produced by VAX DOCUMENT.

DOCTOR is an unsupported product. 

__________________________________________________________________

1 DOCTOR's components

  The utility consists of several more or less independent
  parts, each working on some sort of file produced by the
  VAX DOCUMENT typesetting software. Only the part that
  works on PostScript files can also be used for other
  PostScript files, as long as they adhere to the Adobe
  specified minimal conformant file structure. 

  The types of files that can be handled by the DOCTOR are:

  o  PostScript files

  o  XREF cross reference files of VAX DOCUMENT

  o  SDML source files of VAX DOCUMENT

  o  MSG message files of the VMS Message utility to produce
     SDML files

__________________________________________________________________

2 DOCTORing SDML files: Sorting glossary entries

  Using the DOCTOR/GLOSSARY qualifier invokes the glossary item sort 
  utility. It will accept the input of any properly coded SDML source 
  file with <GTERM> entries and sorts these in ASCIIbetical order with some
  small corrections on its collating sequence. Under a properly coded file, 
  it is understood that only a single <GLOSSARY> - <ENDGLOSSARY> block exists 
  in the .SDML file. Multiple glossaries inside a single file are not
  supported by DOCTOR.

  The glossary does not need to be the only part of the file: it may be 
  embedded in a larger SDML file. Only the glossary part is sorted. Text 
  preceding or following the glossary section remains untouched.

__________________________________________________________________

3 DOCTORing VMS Message source files

  When a software product is coded and one uses the VAX/VMS error message 
  utility to produce error messages on SYS$ERROR when the software detects 
  an error (like the %DOCTOR messages you get from DOCTOR), the best place 
  to describe the reason of the error message occurring, and what the user 
  could possibly do to prevent it next time, is to write this information 
  immediately when the message is defined. That's when you know best which 
  message is added, why it was added and what you could do about it.

  The DOCTOR/MESSAGE utility will format a properly coded source file with 
  VAX/VMS message definitions and embedded comments in such a way that the 
  comments of the file are reformatted into a <MESSAGE_SECTION> section, 
  ready for inclusion into the User's Guide message appendix of the
  documentation that describes the software to which these error messages 
  belong.

  Note:
       If you use DOCTOR/MESSAGE/HELP then the produced .SDML file is 
       compatible for processing with the HELP.MESSAGE doctype.

  The produced SDML file can be <INCLUDE>d into a VAX DOCUMENT source 
  that is processed for the SOFTWARE family doctype.


__________________________________________________________________

4 DOCTORing SDML files for Bookreader output

  VAX DOCUMENT can be used to build books for printing on paper, but also 
  for using them as online books using the DECwindows Bookreader on either 
  VMS or ULTRIX.

  Unfortunately, the Bookreader product imposes a more strict use of some of 
  the tags, available in DOCUMENT, due to its unique features of "pop up" 
  elements (tables, figures, examples) that appear when you click on a "hot
  spot".

  Because any part of the document must be accessible from the table of 
  contents, a symbol must be attached to them so an implicit reference is 
  made from the table of contents to the section pointed at.

  DOCTOR/ONLINE will go through your SDML files and add symbols where there 
  are none, and also add references to pop up elements, where none is written 
  in the text.  If you specify a profile file, all elements will also be
  searched, as will any file referenced in a <INCLUDE> tag.  

__________________________________________________________________

5 DOCTORing PostScript files

  PostScript files are produced by a wide variety of products, such as 
  VAX DOCUMENT, DECwrite, DECpaint or MS-Word. With the arrival of laser 
  printers lots of documentation is produced in the PostScript format and
  printed. Several problems can then occur:

  o  A file print is halfway ready when the system crashes or the job gets 
     aborted for all the right and wrong reasons.

  o  Only a section of a document is interesting to print.

  o  Some products, like VAX DOCUMENT, do not produce blank pages when a 
     section ends on an odd page. This causes double sided printers to skip 
     a page and print the following section on the wrong side of the page 
     (recto pages on verso pages and vice versa).

  o  The same file is needed, but one would like a different standard page 
     layout (e.g. with DRAFT written over the page) as defined in the prologue 
     of the PostScript file.

  o  The PostScript file contains figures that were inserted as encapsulated 
     PostScript and you would like to remove those from the file or to create 
     them as separate files to re-use those figures.

  o  A PostScript document should be re-ordered in page output to allow for 
     saddle stitching of double sided printed sheets (4 pages/sheet, like a 
     magazine with page sheets folded in the middle).

  The DOCTOR utility does all this in an easy manner, provided the 
  PostScript file that must be inspected adheres to Adobe's minimal 
  conformant coding style. This is assumed to be true if the first line of 
  the PostScript file contains %!PS-Adobe, possible followed by a version 
  number.

Replacing the prologue:

  The prologue contains definitions and a general page layout applicable to all 
  pages. Hence, you can add or replace certain features to give the printed 
  pages a different look, without modifying any of the text that is part of 
  the document itself. You can create your own prologue, either based on the 
  original one, or entirely homewritten, to replace the prologue that comes 
  with the input file. This way you can produce effects like writing "DRAFT" 
  diagonally across all pages, or print a faint logo on each page or border 
  the text in a frame.

  To allow some simple modification, the qualifier /CHANGE_PROLOG has been 
  implemented. You can specify three items on each page:

  o  A bold printed text at the top of each page (a header)

  o  A bold printed text at the bottom of each page (a footer)

  o  A faintly grey printed text diagonally across the page.
     The grey scale can be set manually.


Extracting ranges:

  Using the /EXTRACT  qualifier you can specify to copy only a range of 
  pages from the input file into the output file. 


Removing or re-using figures:

  A PostScript file contains text and possibly figures. Those figures are 
  very often made separately (using a drawing package) and then inserted 
  into the text body by the text formatting tool such as VAX DOCUMENT's 
  <FIGURE_FILE> tag or DECwrite's "LINK TO PICTURE" option.

  Occasionally there is a need to either remove the figures from the 
  PostScript file (to allow the remainder to be converted back to plain 
  ASCII text file) or to re-use those figures in other documents.

  Removing figures:

    For some time now several tools are available to convert a PostScript 
    file back into its plain ASCII text file.  This can come in handy if the 
    sources to produce the PostScript file are lost or part of the document 
    could be used in another file. These PostScript-to-ASCII converters
    sometimes cannot handle embedded figures. DOCTOR enables you to remove
    that code before conversion it attempted.


  Extracting figures:

    When you want to extract figures from a document into individual figure 
    files, this function will result in DOCTOR scanning the PostScript source 
    file and to extracting each included figure to become a separate .EPS 
    file. This file can then be used in other documents or presentations.


Saddle stitch printing:

  When one wants to use a printer that allows for printing on both sides of a 
  sheet of paper, it may be advantageous to print the pages on half format 
  (allowing two pages on a single sheet side) and then fold the pages in the
  middle to make a signature (like a magazine), ready for saddle stitching.

  When the book is very thick, one may decide to divide the book into several 
  of these signatures and then stitch them together.

  In both cases this requires the output order of the pages in the PostScript 
  file to be modified. Rather than the usual sequential order of page 1, 2, 3 
  etc, we now need the first and the last page to be printed after each other 
  (and on the same sheet side if printing is setup for two pages/sheet). And 
  then page 2 and the one-but-last page on the backside of the sheet.

   Saddle stitched printing
   __________________________________________________________________

+-------+-------+    +-------+-------+    +-------+-------+    +-------+-------+
|       |       |    |       |       |    |       |       |    |       |       |
|       |       |    |       |       |    |       |       |    |       |       |
|       |       |    |       |       |    |       |       |    |       |       |
|       |       |    |       |       |    |       |       |    |       |       |
|       |       |    |       |       |    |       |       |    |       |       |
|8      |      1|    |2      |      7|    |6      |      3|    |4      |      5|
+-------+-------+    +-------+-------+    +-------+-------+    +-------+-------+

__front_sheet_1_________back_sheet_1_________front_sheet_2__      back sheet 2

  SADDLE only works for printing 4 pages on a sheet (two on each side). There 
  is no support for 4 or more pages printed on a single side of a sheet of 
  paper.

Minimal conformant files:

  Adobe Inc. defined a "Document Structuring Conventions Specification" for 
  PostScript files that utilities such as VAX DOCUMENT and DECwrite should 
  adhere to. Only then other utilities such as DOCTOR can inspect those files
  and manipulate them. DOCTOR's output is also conformant to these styles, so 
  one could use DOCTOR on its own files recursively.

__________________________________________________________________

6 DOCTORing SDML files: hierarchy of files within a book

  A complex document written for VAX DOCUMENT processing can consist of many 
  elements. The text may all be written in a single source file, or 
  distributed over many others, that are all <INCLUDE>d into the final 
  printable document. Besides text, other tools and utilities can provide
  additional graphical or tabular data. Often a complex document is 
  built using a profile file where this profile specifies the names of the 
  other SDML files that are part of the complete document.

  When several people work on such a compound document, it is easy to loose 
  track of the number of files that are referenced from the main, the root, 
  source file.

  Here, the DOCTOR/SDML utility, can assist you in the process. Given any
  top level root file that is coded in VAX DOCUMENT, it will produce one or
  several of:

  o  An organizational hierarchy of how the document is composed of individual 
     elements (that in turn may also be composed of smaller elements).

  o  A DEC/MMS description file that lists all the dependencies between the 
     individual components in order to rebuild the final printable document 
     in any of the supported destinations: LN03, PostScript, Bookreader,
     Line_printer or Mail.

  o  A list of all occurrences of the <X> and <Y> tags that are written in 
     those VAX DOCUMENT source files, annotated with the exact line number 
     of the file in which they were found.


Listing all included files:

  When a document is processed to the final output, the VAX DOCUMENT command 
  line indicates the top level source SDML file to be processed.  When one of 
  VAX DOCUMENT's three components (Tag Translator, Text Processor and
  Device Converter) encounter any of the tags (or their equivalents),
  the specified files will be opened and included into the final printable 
  output.

Producing MMS description (rebuild) files:

  Because once the hierarchy list is made, the DOCTOR also knows which files 
  depend on which other ones, it is an easy task to produce a description file 
  that can be read by DEC/MMS to rebuild the document if one of the elements
  is modified.

  Because DEC/MMS does not know how to build an LN03 or PostScript file from an
  SDML file, the DOCTOR/SDML also inserts all the required MMS rules and 
  suffixes to allow DEC/MMS to retrieve the sources from a CMS library and to
  rebuild it using a proper VAX DOCUMENT command.

Using DOCUMENT/GRAPHICS:

  A special mention must be made for the use of DOCUMENT /GRAPHICS produced 
  graphics. This utility, that is delivered with VAX DOCUMENT V2.0 onwards, 
  is a DECwindows oriented graphics editor, that will produce one or several 
  different output graphics: PostScript, sixel or BRF-bookreader files. In 
  addition it can output another .SDML file that only contains <FIGURE_FILE>
  tags. This enables the author to completely decouple text and graphics 
  components of the document. The only thing written in the text file will be

       <FIGURE>(figure caption\figure_symbol)
       <INCLUDE>(figurefile.SDML)
       <ENDFIGURE>

  whereas the figurefile.SDML will be created by the DOCUMENT/GRAPHICS editor 
  and contains the required <FIGURE_FILE> tags with the correct size of the 
  figure, which may differ for each destination.

  When DOCTOR/SDML/MMS is required to produce an MMS file, it will also add 
  the action rule to invoke the graphics editor DOCUMENT/GRAPHICS=RENDER to 
  recreate any such .SDML file from a graphics meta-file with file type .GRA.
  However, this will only work, if the MMS description file is invoked by 
  MMS while running on a DECwindows display (terminal or workstation), as the
  DOCUMENT/GRAPHICS utility will only work when it has this environment
  available - even if it will not display a single window. 

Retrieve all index entries:

  When a document is written and it contains many index entries, your chances 
  are good that some index entries that were supposed to be identical, but on 
  different pages, are actually printed as two entries in the index.  Usually 
  because you mistyped an entry of the <X> or <Y> tag.

  Finding those typo's is tedious: you need to get the book printed, then look 
  in the index, find the errors, look at that page, determine which file it 
  comes from and finally go into the editor and find the entry.

  The DOCTOR allows all this to be done much easier, without the overhead of 
  rebuilding the entire book each time you think you found all the errors. 
  (Usually the fixing of index entries take several repeated loops of
  correcting the tag entries).

  By specifying DOCTOR/SDML/INDEX the DOCTOR produces the hierarchy list, but, 
  while scanning through all the SDML source files, it also copies each and 
  every occurrence of the <X> and <Y> tags into a separate file, specified by
  the /INDEX= qualifier.

  The result looks something like the following example. This may look odd, 
  but it is very useful.

       Example Output of DOCTOR/SDML/INDEX
       __________________________________________________________________

        <X>(DOCTOR<XS>installation)       <comment>(    34 DOCTOR.SDML)
        <X>(DOCTOR<xs>invocation)       <comment>(    58 DOCTOR.SDML)
        <X>(CONFORM)       <comment>(    16 DOCTOR_MSG.SDML)
        <X>(CREATED)       <comment>(    25 DOCTOR_MSG.SDML)
        <Y>(IDENT - See DOCTOR)       <comment>(    34 DOCTOR_MSG.SDML)

       __________________________________________________________________

  The fact that all lines start with tag <X> or <Y>, allows you to use the 
  DCL command SORT to sort the file into alphabetical order. (It is produced 
  in order of the files referenced by the DOCTOR).

       $ SORT  input_file   output_file

  This will position typo's near each other and easy to spot when the sorted 
  file is printed. It is also easy to load all the required SDML source files 
  into your editor buffers once, and then move from buffer to buffer[1] to 
  correct the errors. Since you do not add any lines, but simply re-type an 
  entry, the line numbers that precede the file specification that is given 
  in the <COMMENT> on the same line, allows you to use a goto line command[2] 
  to quickly move to the indicated line to find the incorrect tag entry.

  Once all these entries have been corrected in the SDML files, you can run 
  DOCTOR/SDML/INDEX once more on it and inspect the new output to spot any 
  typo's you missed. If there are none, you can use the produced index file
  to make a dummy book by processing the index file by itself. Of course the 
  final output will contain non-sense page numbers, but at least it is 
  processed and built in substantially less time than redoing the entire 
  document.  When the processed and printed index also looks correct,
  then you reprocess the entire book. If you're still not satisfied, you 
  should make additional changes to the incorrect tag entries.

            [1] "GOTO BUFFER name" for LSEDIT

            [2] LINE for LSEDIT

__________________________________________________________________

7 DOCTORing SDML files: Counting tags used

  This is a very simple utility that counts all the tags encountered in an 
  SDML file. A tag is defined as any contiguous text string consisting of 
  alphabetic, numeric or underscore characters, surrounded by the opening 
  and closing angle brackets: <LIKE_THIS>. For this reason, the utility 
  also detects and counts all user defined tags.

  The utility is invoked through DOCTOR/TAG_COUNT, and takes a single SDML 
  file as an input parameter. It automatically also searches through all files 
  referenced by the input file specified: if a profile is given, all elements 
  of the book are also searched, as are the <INCLUDE> files.

  Note:
     This utility counts any tag-like construct, as it does not 
     interpret or validate them.  This also means it counts all 
     tags within <COMMENT> blocks and/or <LITERAL> blocks. When
     within these blocks <INCLUDE> or <ELEMENT> or <INCLUDES_FILE> 
     tags are encountered, DOCTOR/TAG will attempt to lookup the 
     files referenced by these tags. It will give a warning if 
     those files do not exist, but continues processing.

     __________________________________________________________________
           Example   Sample DOCTOR/TAG_COUNT output file
     __________________________________________________________

                Generated by DOCTOR/TAG_COUNT.  Digital Internal Use Only

  Tags (and their frequency) found in SDML document rooted in DOCTOR.SDML:

                             2 <ABSTRACT>
                             4 <APPENDIX>
                            10 <CALLOUT>
                             3 <CALLOUTS>
                             2 <CENTER_LINE>
                            10 <CHAPTER>
                           412 <CODE_EXAMPLE>
                             7 <COMMAND>
                             1 <COMMAND_SECTION>
                            23 <COMMENT>
                                :
                                :
                           177 <TAG>
                             6 <TEX>
                             3 <TITLE>
                             2 <TITLE_PAGE>
                             4 <U>
                             3 <VALID_BREAK>
                           369 <X>
                           104 <XS>
                       _____13_<Y>_________________________________________

__________________________________________________________________

8       DOCTORing XREF files


  When a large document is built using DOCUMENT, often references are used 
  to point to other parts of the document. After a while, and certainly if 
  several authors work on a document, you start losing track of what 
  symbols are defined for what tables, sections and other referable items. 
  Even if a naming convention is used.

  For this reason DOCTOR/XREF has been built. It will work on any document 
  of which the cross reference .XREF file exists. This is always true for 
  documents built through a profile. Single file documents do not have an 
  .XREF file - the Tag Translator keeps all data gathered in Pass 1 in 
  memory and uses it to resolve references during Pass 2: no need for a 
  file. You must break up this file into parts and add a profile file to 
  it, in order to use DOCTOR/XREF.

  Using DOCTOR/XREF on a .XREF file will allow you to do several things:

  o  Build an SDML file with all symbols listed in a <TABLE> structure. By 
     processing this SDML file (e.g. using the REPORT doctype) you get a 
     nicely printed listing of all symbols and their textual contents. Those 
     lists can be alphabetic on symbol name or sorted numerically by type
     (e.g. all table symbols sorted by table number). 

  o  Build an SDML symbol file containing <DEFINE_SYMBOL> tags of all symbols 
     defined in the .XREF file. This allows you to <INCLUDE> that file into 
     some other book and thereby enables you to cross reference between books.

Build symbol listings:

  DOCTOR/XREF allows you to produce a formatted listing of all symbols 
  defined in a document. The listings are made in <TABLE> format and can be 
  processed with VAX DOCUMENT using any doctype available that supports table 
  tags.  The listings can be made in alphabetical order on the symbol names 
  but also in numerical order of the section or object numbers.

  By default all symbols are then sorted alphabetically per type. However, in 
  many cases it may be more useful if they are sorted by their numeric value 
  (if applicable) so that for example, you can quickly look up the symbol for
  Table 5-6. 
  
Build cross reference symbol file:

  To build a file with <DEFINE_SYMBOL> tags that define the symbols plus 
  their reference translation, you need to specify the .XREF file to scan 
  and the /SYMBOL_FILE=filespec for the resulting output file.

__________________________________________________________________

In Summary, the various tools under DOCTOR are invoked by the following
commands:

DOCTOR/GLOSSARY

  DOCTOR/GLOSSARY-Sort glossary items in SDML file

  The DOCTOR/GLOSSARY utility will sort all <GTERM> tags and associated 
  <GDEF> tags within an SDML source file. The glossary section may be part 
  of a larger file. There may only be a single glossary.


DOCTOR/MESSAGE

  DOCTOR/MESSAGE-Create SDML file from VMS Message definitions

  The DOCTOR/MESSAGE utility can create a VAX DOCUMENT SDML file with 
  explanation of error messages, from a properly coded VAX/VMS Message
  source file.


DOCTOR/ONLINE 

  DOCTOR/ONLINE-Modify SDML files for Bookreader

  The DOCTOR/ONLINE scans through the specified SDML source file and the 
  files it references, for correct syntax to be processed for the 
  BOOKREADER destination. This implies that symbols are added to all 
  unsymboled sections, tables, examples and figures.


DOCTOR/PS

  DOCTOR/PS-Modify PostScript files

  This utility adds blank pages to PostScript files when required to have 
  a balanced set of odd/even pages. It also replaces the prologue part of 
  a PostScript file or produces an alternative output file that consists 
  of a subset of pages. It can extract or remove encapsulated PostScript 
  figures from a document, or re-order the page sequence to allow for 
  saddle stitching the printed output.


DOCTOR/SDML

  DOCTOR/SDML-Markup Files Included

  The DOCTOR/SDML scans through the VAX DOCUMENT source files for other 
  sources included by them, up to a nesting level of 20. These included 
  files can be VMS files or CMS elements, if DEC/CMS is installed on the 
  system. These files can be any of the ones normally accepted through 
  standard tags.

  It reports the document structure as a comment block in the source file 
  and can produce an MMS build file as well as a list of all the 
  occurrences of index tags.


DOCTOR/TAG_COUNT 

  DOCTOR/TAG_COUNT-Count all tags in SDML file

  The DOCTOR/TAG_COUNT utility counts all tag constructs inside an SDML 
  document. This document may be a complete book (profile plus elements).
  It reports the counted tags in the produced output file.


DOCTOR/XREF

  DOCTOR/XREF-List cross reference symbols

  The DOCTOR/XREF utility enables you to produce a file with 
  <DEFINE_SYMBOL> tags of all symbols defined within a book, derived from 
  its .XREF file.

  It also enables you to produce a listing of all symbols, sorted 
  alphabetically or by symbol type (chapter, table, example etc)
