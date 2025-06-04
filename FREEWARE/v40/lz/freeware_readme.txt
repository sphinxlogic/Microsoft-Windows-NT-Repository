COMPRESS_LZ, UTILITIES, Tool to compress and decompress files

  The COMPRESS command invokes a utility to copy a file, generating
  a file with (usually) fewer bytes.  Files compressed by COMPRESS
  are recovered by DECOMPRESS.

    COMPRESS Input-file-spec Output-file-spec

2 Parameter

 Input-file-spec

  Specifies the name of the file to be compressed.  If you have
  specified /EXPORT=VMS mode, the file must be stored on a disk.

 Output-file-spec

  Specifies the name of the file created by COMPRESS.
2 Qualifiers

  Indicate special actions to be performed by the COMPRESS utility
  or special properties of either the input or output files.
  Qualifiers apply to the entire process.

  The following list shows all the qualifiers available with the
  COMPRESS command:

   o /BITS=value

   o /EXPORT=(option,...)

   o /METHOD=option

   o /MODE=(option,...)

   o /SHOW=(option,...)

2 /BITS
 /BITS=value

  This specifies the maximum number of bits to be used in the
  compression. It implicitly controls both the "quality" of the
  compression (more bits means more compression) and the amount of
  memory needed for both compression and decompression (more bits
  requires more memory). If the compressed file is to be read by a
  computer with limited memory (such as a PDP-11), choose /BITS=12,
  else leave BITS at its default of 16.  The minimum value is 9
  and the maximum value is 16.

2 /EXPORT
 /EXPORT=(option, [,...])

  Export controls the format of the output file.  You can select
  the following:

  VMS (D)               Write a file that can only be read by VMS
                        COMPRESS.
  UNIX                  Write a format that can be read by programs
                        compatible with the Unix compress utility.
  [NO]ENDMARKER         Write a special file endmarker after the
                        data if specified.
  [NO]BLOCK             Monitor compression and reinitialize if the
                        quality decreases if specified.
  [NO]HEADER            Write a file header with information for
                        DECOMPRESS if specified.


  In general, use /EXPORT=VMS for compression where the result will
  be decompressed on a VMS system and /EXPORT=UNIX where the result
  will be decompressed on a Unix, RSX-11M, RSTS/E, or other non-VMS
  system.  If /EXPORT=UNIX is specified, BLOCK, HEADER, or ENDMARKER
  may be negated to further qualify the output file format.

3 VMS

  Specifies output in VMS ("private") mode.  In addition to the
  contents of the file itself, the "File definition block" is also
  compressed. The decompression utility can thus recreate the file
  exactly (including ISAM indexes).  This is the default, and the
  recommended mode for most uses.

3 UNIX

  Specifies an output format compatible with Unix compress v3.0.
  This allows transmitting sequential files to non-VMS systems that
  support a compress-compatible utility.  If you have specified
  /EXPORT=UNIX, the utility can be configured for variants of Unix
  compress by negating BLOCK, HEADER, and/or ENDMARKER as needed.
  Notice that file attributes are not preserved by /EXPORT=UNIX.

3 BLOCK

  Selects an algorithm whereby COMPRESS evaluates its performance
  and re-initializes the compression tables whenever performance
  degrades.  Older versions of Unix compress do not support this
  capability. If negated, ENDMARKER must also be negated.

3 HEADER

  If negated, COMPRESS does not write a header record.  This is
  for compatiblity with very old versions of Unix compress.  If
  negated, BLOCK and ENDMARKER must also be negated.

3 ENDMARKER

  If specified, a special "endmark" is written after the end of the
  file.  This is necessary if the file is to be decompressed on RT11
  or other systems that require the last block of a file to fill the
  last block.  On the other hand, some versions of Unix compress
  cannot understand the "extra" endmarker.  If you guess wrong, a
  few bytes of garbage may be appended to the decompressed file.  A
  version of Unix compress that handles endmarkers correctly is
  available.

2 /METHOD
 /METHOD=(option)

  This specifies the particular compression algorithm.  Currently,
  only /METHOD=LZW is supported.

3 LZW

  Use the Lempel-Ziv-Welch compression algorithm.

2 /MODE
 /MODE=(option)

  This allows specification of variations on the compression method.

3 BINARY

  This opens the file in "binary" mode, rather than "text" mode.
  It is ignored if /EXPORT=VMS is chosen.

3 DELTA
  Compress the difference between successive bytes, rather than the
  bytes themselves.  For certain file formats, such as bit-mapped
  graphics, this may yield a 10-15% improvement in compressibility.
  This is not compatible with some implementations of Unix compress.

2 /SHOW
 /SHOW=(option, [,...])

  Display information about the compression.  If omitted, COMPRESS
  operates silently (except for error messages).

3 ALL
  Equivalent to /SHOW=(PROGRESS,STATISTICS,FDL)

3 PROGRESS
  Print status messages at intervals, showing the operation of the
  program.  The report shows the current compression ratio (the
  ratio of input to output bytes).  If this decreases, COMPRESS
  decides that the characteristics of the file have changed, and
  resets its internal parameters.  The "gap" is the number of
  input codes used to compute the ratio.

3 STATISTICS
  Print a report at the end of the process.  Note that COMPRESS reports
  the number of bytes it compresses, which includes the file as well as
  the information that COMPRESS records about the file (the File Definition
  Language block and some internal codes), and will therefore be several
  hundred characters greater than the actual size of the file.

3 FDL
  Dump the File Definition Language block that describes a VMS input
  file.

3 DEBUG
  Print internal debugging information.

3 DEBUG_SERIOUS
  Print more internal debugging information.

3 DEBUG_IO
  Dump the output file, too.

2 LZW_Overview
  LZW stands for a compression method described in

    "A technique for High Performance Data Compression."
    Terry A. Welch. IEEE Computer, Vol 17, No. 6 (June 1984)
    pp. 8-19.

  This section is abstracted from Terry Welch's article referenced
  below.  The algorithm builds a string translation table that maps
  substrings in the input into fixed-length codes.  The compress
  algorithm may be described as follows:

  1. Initialize table to contain single-character strings.
  2. Read the first character.  Set <w> (the prefix string) to that
     character.
  3. (step): Read next input character, K.
  4. If at end of file, output code(<w>); exit.
  5. If <w>K is in the string table:
     Set <w> to <w>K; goto step 3.
  6. Else <w>K is not in the string table.
       Output code(<w>);
       Put <w>K into the string table;
       Set <w> to K; Goto step 3.

  "At each execution of the basic step an acceptable input string
  <w> has been parsed off.  The next character K is read and the
  extended string <w>K is tested to see if it exists in the string
  table.  If it is there, then the extended string becomes the
  parsed string <w> and the step is repeated.  If <w>K is not in the
  string table, then it is entered, the code for the successfully
  parsed string <w> is put out as compressed data, the character K
  becomes the beginning of the next string, and the step is
  repeated."

  The decompression algorithm translates each received code into a
  prefix string and extension [suffix] character. The extension
  character is stored (in a push-down stack), and the prefix
  translated again, until the prefix is a single character, which
  completes decompression of this code.  The entire code is then
  output by popping the stack.  I.e., the last code put into the
  stack was the first code in the original file.

  "An update to the string table is made for each code received
  (except the first one).  When a code has been translated, its
  final character is used as the extension character, combined with
  the prior string, to add a new string to the string table.  This
  new string is assigned a unique code value, which is the same
  code that the compressor assigned to that string.  In this way,
  the decompressor incrementally reconstructs the same string table
  that the decompressor used.... Unfortunately ... [the algorithm]
  does not work for an abnormal case.

  The abnormal case occurs whenever an input character string
  contains the sequence K<w>K<w>K, where K<w> already appears in
  the compressor string table."

  The decompression algorithm, augmented to handle the abnormal
  case, is as follows:

  1. Read first input code;
     Store in CODE and OLDcode;
     With CODE = code(K), output(K); FINchar = K;
  2. Read next code to CODE; INcode = CODE;
     If at end of file, exit;
  3. If CODE not in string table (special case) then
       Output(FINchar);
       CODE = OLDcode;
       INcode = code(OLDcode, FINchar);
  4. If CODE == code(<w>K) then
       Push K onto the stack;
       CODE == code(<w>);
       Goto 4.

  5. If CODE == code(K) then
       Output K;
       FINchar = K;

  6. While stack not empty
       Output top of stack;
       Pop stack;

  7. Put OLDcode,K into the string table.
       OLDcode = INcode;
       Goto 2.

  The algorithm as implemented here introduces two additional
  complications.

  The actual codes are transmitted using a variable-length
  encoding.  The lowest-level routines increase the number of
  bits in the code when the largest possible code is transmitted.

  Periodically, the algorithm checks that compression is still
  increasing.  If the ratio of input bytes to output bytes
  decreases, the entire process is reset.  This can happen if the
  characteristics of the input file change. (This can be supressed
  by /EXPORT=(UNIX, NOBLOCK)).
2 Unix

  Is a trademark of AT&T Bell Laboratories.

!
! decompress
!
1 DECOMPRESS
  The DECOMPRESS command invokes a utility to restore copy a file
  that had been compressed by COMPRESS.

    DECOMPRESS Input-file-spec [Output-file-spec]

2 Command_Parameters

 Input-file-spec

  Specifies the name of the compressed input file.

 Output-file-spec

  Specifies the name of the file created by COMPRESS.  If the file
  was compressed by /EXPORT=VMS, the original file name will be
  used if the Output file spec. is omitted.  If the file was
  compressed by /EXPORT=UNIX and no Output file spec. is provided,
  the file will be written to SYS$OUTPUT:

2 Command_Qualifiers

  Indicate special actions to be performed by the COMPRESS utility
  or special properties of either the input or output files.
  Qualifiers apply to the entire process.

  The following list shows all the qualifiers available with the
  DECOMPRESS command:

   o /BITS=value

   o /EXPORT=(option,...)

   o /METHOD=option

   o /MODE=(option,...)

   o /SHOW=(option,...)

2 /BITS
 /BITS=value

  If a header was not provided, this specifies the maximum number
  of bits that were used in the compression.  This parameter is
  ignored if the compressed file contains a header.

2 /EXPORT
 /EXPORT=(option, [,...])

  Export describes the format of the input file.  You can select
  the following:

  VMS (D)               The file was created by VMS COMPRESS.
  UNIX                  The file was created by Unix compress
			or a compatible program.
                        compatible with the Unix compress utility.
  [NO]ENDMARKER         A special file endmarker follows the data.
  [NO]BLOCK             The compress program may have reinitialized
			compression.
  [NO]HEADER            The compress program wrote its parameters
			into a file header.

  In general, the program can determine the proper value of these
  flags by reading the first few bytes of the file.  If valid,
  the file header overrides the command line specification.
  Generally, this option is needed only if you are trying to read
  a file generated by a version of Unix compress that did not
  write a header.  See the description of COMPRESS for details.

2 /METHOD
 /METHOD=(option)

  This specifies the particular compression algorithm.  Currently,
  only /METHOD=LZW is supported.

3 LZW

  Use the Lempel-Ziv-Welch compression algorithm.

2 /MODE
 /MODE=(option)

  This allows specification of variations on the output file
  format.  These values will be taken from the source file
  description if /EXPORT=VMS is chosen.

3 BINARY

  This creates the file in "binary" mode, rather than "text" mode.
  It is ignored if COMPRESS created the file in /EXPORT=VMS mode.
  The output file will be created in RMS "Stream-LF" format.

3 DELTA
  Compress used the difference between successive bytes, rather
  than the bytes themselves.  For certain file formats, such as
  bit-mapped graphics, this may yield a 10-15% improvement in
  compressibility. This is not compatible with some implementations
  of Unix compress.  This value is normally read from the file
  header, and generally need not be specified by DECOMPRESS.

3 FIXED

  Create the file in "fixed-length-record" format.
  This is probably the best format to use for decompressing
  binary files (such as tar archives)  created on Unix.

  The record size can be specified with this keyword, as in

  	/MODE=(FIXED=recsiz)

  If no size is specified, then a value of 512 is assumed.

3 TEXT

  Create the file in "variable-length carriage-control" format.
  This is appropriate for decompressing readable text files
  created by Unix compress.

2 /SHOW
 /SHOW=(option, [,...])

  Display information about the compression.  If omitted, DECOMPRESS
  operates silently (except for error messages).

3 ALL
  Equivalent to /SHOW=(PROGRESS,STATISTICS,FDL)

3 PROGRESS
  Print status messages at intervals, showing the operation of the
  program.

3 STATISTICS
  Print a report at the end of the process.

3 FDL
  Dump the File Definition Language block that describes the output
  file.

3 DEBUG
  Print internal debugging information.

3 DEBUG_SERIOUS
  Print more internal debugging information.

3 DEBUG_IO
  Dump the input file, too.

2 Unix
  Is a trademark of AT&T Bell Laboratories.
