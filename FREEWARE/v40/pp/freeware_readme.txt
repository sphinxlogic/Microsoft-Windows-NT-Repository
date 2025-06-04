PP v3.0, LANGUAGES, Pascal Pre-processor (or Pretty Pascal)

PP - the Pascal Pre-processor (or "Pretty Pascal")

PP takes a  DEC  Pascal  source  file  and  performs  a  number  of text
standardization tasks on it.

PP identifies four different types of symbols (or "tokens"):

   - tokens that are identified as "Pascal reserved words";
       (these are specified in a vocabulary file)
   - tokens that contain a dollar sign ($);
   - tokens that are considered as user-declared identifiers;
   - all characters not included above.

Three contexts are identified:
   - in a comment;
   - in a quoted string;
   - not in either a comment nor a quoted string.

Tokens are only identified when they do not occur in a comment nor
a quoted string.

The Pascal Pre-processor is capable of performing the following tasks:

   - each of the token types (reserved symbol, identifier and dollar
     symbol) can be converted to one of the following lexical styles:
           . all letters uppercased;
           . all letters lowercased;
           . no change from current style;
           . the first letter and each first letter following an
             underscore or dollar sign are uppercased, all other
             letters are lowercased.
   - lines which have white noise beyond the last significant
     character of a line are unequivocally truncated to the
     last significant character;
   - lines may be re-formatted as necessary to maximize the use
     of TAB characters to eliminate SPACES where possible;
   - lines which are longer than the working screen-width (either
     80 or 132 characters) are identified;

  Progress of the processing can be watched with Control-W.

To install PP:

  1. Re-link PP (NO Traceback) as directed in the build procedure.
  2. Define EXECUTIVE mode SYSTEM table logical name as required (see help).
  3. Define a foreign command: $ PP :== $device:[directory]PP
  4. Pretty Pascal!

The author of PP is

    Jonathan Ridler,
    Information Technology Services,
    The University of Melbourne,
    Parkville, Victoria, AUSTRALIA, 3052.

    Email: jonathan@unimelb.edu.au

PP is written entirely in DEC  Pascal  and  has been built and tested on
OpenVMS  VAX v6.2 (DEC Pascal v5.4) and OpenVMS Alpha v6.2 and v7.1 (DEC
Pascal v5.5).

Suggestions, comments, criticisms, bug reports, bug fixes ... all welcome.

The current version of PP is v3.0 (13-Aug-1998).
