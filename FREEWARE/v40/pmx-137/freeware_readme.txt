PMX, MISCELLANEOUS, Pre-Processor for MusixTeX score macros

        ***  PMX - 1.3.7 (Unix/C version)  ***

This is the Unix/C version of PMX-1.3.7 . It was generated from the
fortran source code with Netlib's F2C. This package includes the special
library routines required to compile the resulting C source.
(please read the file "permission" in the present directory, which
lists conditions for the use of Netlib's library code)
Most of the original PMX distribution is contained in the pmx-orig/
subdirectory. Be sure to read the PMX manual there (pmx134.ps). It is
probably useful to print the quick reference card (ref134.ps).

  System Requirements
  ===================


* any current mainstream Unix flavour
* standard C compiler (as always, GCC is the safest bet)
* TeX/LaTeX (a recent version, as required by musixtex)
* MusiXTeX T.86 or newer

PMX 1.3.7 for OpenVMS                                        May 1998
=====================

You can install PMX 1.3.7 on OpenVMS providong TeX (DECUS distribution) and
MusiXTeX packages are already installed. Set up your privileges to be able to
write into TEX_ROOT directory tree and invoque:

$ @INSTALL_VMS

The DECus TeX and MetaFont distribution is available on the OpenVMS Freeware
CDROM V 3.0 (size about 90 Mo).

To run PMX, you can use PMX.COM script (there are 2 passes of PMX and 3 passes
of MusiXTeX needed).

Enjoy !!

Patrick Moreau
<pmoreau@cena.dgac.fr>
<moreau_p@decus.fr>
http://www2.cenaath.cena.dgac.fr/~pmoreau/
