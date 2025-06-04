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
Note that SDL sources are provided in the [.SRC] directory,
including build and option files as well as needed include files.
	Since the OpenVMS operating system itself is written
in multiple languages which share many of the same data structures,
SDL is used to define the data structure once and then produce
output for the respective languages which require access to that
structure definition.
	The most recent "formal" documentation describes VAX
SDL V3.2. That documentation is available on this kit. There have been a
number of bug fixes made to SDL for Alpha which are included in this kit.
	To use SDL, invoke the [SDL]SDL.COM procedure.

	Typical use of SDL:

	The following example starts with a standard OpenVMS message
definiion file, and ends up with an include file (.H) for C:

    $ MESSAGE/NOOBJECT/SDL='NAME'.SDL 'NAME'.MSG
    $ SDL/VAX -   ! or /ALPHA 
        /LANGUAGE=(C)
        /COPYRIGHT 'NAME'.SDL

	The GEN_MSG tool can be used to have a common source file for
both the application message file and the SDML documentation.

