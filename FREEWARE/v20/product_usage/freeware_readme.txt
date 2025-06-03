PRODUSE010, SYSTEM MANAGEMENT, Product Usage by Imagename and Username 

Digital and third party vendors are providing many new licensing options in
addition to the standard full use system license. When building a new system,
it is quite easy to determine if system-wide, concurrent-use or user licenses
make the most sense. When upgrading an existing system, the decision is much
more difficult because the actual usage of products with system-wide licenses
is seldom known. This tool uses data collected by DECps or VPA to build an
array of Imagenames and Usernames run on different Cluster nodes over the time
specified (one month is the minimum suggested interval). The report produced
lists all images run on the systems sorted by product name (if known) and
imagename with the maximum number of concurrent users and the number of
different usernames running the product on each node. The /user=username
qualifier produces a report showing all images run by a single user and the
/image=imagename qualifier produces a report showing all users of an image.
The ability to save the array to avoid many interations of data collection is
provided (sorting through a months worth of DECps data for many nodes can take
a long time). User editable files of products to imagenames are provided.

This tool requires that either DECps or VPA are installed and running on the
system. 

This product is VMSINSTAL-able. See the Postscript User Guide for details.

Listing of save set(s)

Save set:          PRODUSE010.A (common files and fortran source)

BUILD_VMS_DECW_SKIP.COM		Builds a list of VMS and DECwindows images
IMAGE_TO_PRODUCT.DAT		List of known Product Images (do not edit)
KITINSTAL.COM			Vmsinstal required (this file does the install)
PRODUCTUSEIVP.COM		Installation verification procedure
PRODUCT_SKIP.DAT		User editable list of images to ignore
PRODUCT_SKIP_VMS_DECW.DAT	List of VMS and DECwindows images to ignore
PRODUCT_USE.CLD			Command language definition for the code
PRODUCT_USE.COM			Procedure to run the code
PRODUCT_USE.PS			Postscript documentation
PRODUCT_USE_DECPS.FOR		DECps version Fortran source code
PRODUCT_USE_VPA.FOR		VPA version Fortran source code
USER_IMAGE_TO_PRODUCT.DAT	User editable Product to imagename file

Save set:          PRODUSE010.B (DECps version of the code)

PRODUCT_USE_DECPS.OBJ	DECps object code used during install
PSPA$LIB.FOR		DECps Fortran library

Save set:          PRODUSE010.C (VPA version of the code)

PRODUCT_USE_VPA.OBJ	VPA object code used during install
VPA$LIB.FOR		VPA Fortran library
