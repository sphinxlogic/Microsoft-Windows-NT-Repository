IMAGETREE, BUILD_TOOLS, Display the shareable image dependence tree of an image


$!
$!  Command procedure to display the shareable image dependence tree for
$!  and executable or shareable image. Works against VAX and Alpha images,
$!  also works on Alpha VESTed images.
$!
$!  Syntax:
$!    @IMAGETREE <imagename> [FULL]
$!
$!  <imagename> is the name of the shareable image, defaults to SYS$SHARE:.EXE
$!
$!  FULL is a flag which directs the procedure to perform a complete tree
$!	traversal. Since the procedure works by parsing ANALYZE/IMAGE output
$!	this can result in excessive processing. The default is to keep track
$!	of images already analysed and display them as "repeat image"
$!
$!  Note that all dependent images must be accessible. The procedure uses
$!  the same algorithm as the image activator to locate images.
$!
$!  Author: © John Gillings, Digital Customer Support Centre, 23-September-1994
$!
$!  THIS IS UNSUPPORTED SOFTWARE
$!
