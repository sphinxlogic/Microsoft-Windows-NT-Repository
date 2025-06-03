SDL, LANGUAGES, Data Structure/Interface Definition Language

SDL is both a data structure and interface definition
language and a utility/compiler to convert those definitions
into language output for a number of different languages.
This allows a single source definition of a data structure,
for example, while supporting multiple language representations
of that data structure.

SDL consists of a front-end and numerous language
back-ends. The front-end generates an intermediate language
which is interpreted by the back-ends. It is straightforward to
create back-ends for new languages, following the example of an
existing back end. When the SDL/LANGUAGE qualifier is used, the
specified language name is used to search for the back-end image.
Note that SDL sources are provided in the [.SRC] directory.

Since the OpenVMS operating system itself is written
in multiple languages which share many of the same data structures,
SDL is used to define the data structure once and then produce
output for the respective languages which require access to that
structure definition.

The most recent "formal" documentation describes VAX
SDL V3.2. That documentation is available on this kit. There have
been a number of enhancements to SDL for AXP, and this kit contains
both VAX and translated images with those enhancements (which are
undocumented). These include:

o Improved conditional support in the language
o New datatypes
o Bitfields up to 64 bits
o Structure member alignment controls
o Structure base alignment controls
o Automatic generation of STARLET header files according to established
  conventions
o Automatic generation of LIB header files according to established
  conventions
o Full function prototypes which include parameter types
o Upper and lower case names for function prototype routine names
o C++ structure prototypes
o Support "old" and "new" STARLET header file definitions (the new are
  consistent with LIB-style definitions)

To use this SDL, copy all the images into a particular directory,
such as DAD$FREEWARE:[SDL], and create a local command file such as the
following (to use the "native" SDL images):

$ AXP_NATIVE == 1
$ AXP_CROSS  == 0
$ DEFINE EVMS$BUILD_TOOLS DAD$FREEWARE:[SDL], SYS$LIBRARY
$ @EVMS$BUILD_TOOLS:SDL ""
