zlib 0.95 is a beta version of a general purpose compression library.
This should be the last version before the first official
version (1.0), so please test it now. (At least compile and run it with
"make test"). This version has no known bugs.

The data format used by the zlib library is described in the
files zlib-3.1.doc, deflate-1.1.doc and gzip-4.1.doc, available
in ftp.uu.net:/pub/archiving/zip/doc.

All functions of the compression library are documented in the file
zlib.h. A usage example of the library is given in the file example.c
which also tests that the library is working correctly. Another
example is given in the file minigzip.c.

To compile all files and run the test program, just type: make test
(For MSDOS, use one of the special makefiles such as Makefile.msc;
for VMS, use Make_vms.com or descrip.mms.)
To install the zlib library (libz.a) in /usr/local/lib, type: make install
To install in a different directory, use for example:
  make install prefix=$HOME
This will install in $HOME/lib instead of /usr/local/lib.

The changes made in version 0.95 are documented in the file ChangeLog.
The main changes since 0.94 are:
- fix MSDOS small and medium model (now easier to adapt to any compiler)
- inlined send_bits
- fix the final (:-) bug for deflate with flush (output was correct but
  not completely flushed in rare occasions).

For MSDOS, the small and medium models have been tested only with Microsoft C.
(This should now work for Borland C also, but I don't have a recent Borland
compiler to test with.) The small model was tested with Turbo C but only with
reduced performance to avoid any far allocation; it was tested with
 -DMAX_WBITS=11 -DMAX_MEM_LEVEL=3


 (C) 1995 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  gzip@prep.ai.mit.edu    madler@alumni.caltech.edu

If you use the zlib library in a product, we would appreciate *not*
receiving lengthy legal documents to sign. The sources are provided
for free but without warranty of any kind.  The library has been
entirely written by Jean-loup Gailly and Mark Adler; it does not
include third-party code.

If you redistribute modified sources, we would appreciate that you include
in the file ChangeLog history information documenting your changes.
