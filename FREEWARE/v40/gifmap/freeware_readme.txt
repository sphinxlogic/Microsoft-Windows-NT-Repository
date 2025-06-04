GIFMAP, MISCELLANEOUS, GIF files Web indexer (clickable contact sheets)

Gifmap provides a means of easily putting image collections on the Web.
It recurses through directory trees, building HTML pages and imagemap
(GIF or JPEG) files to allow the user to navigate through collections of
thumbnail images (somewhat similar to 'xv') and select the images to
view with a mouse click.

Gifmap is written by Bob Friesenhahn (bfriesen@simple.dallas.tx.us). I
would be happy to hear about your trials and tribulations with gifmap.

Gifmap 1.54 port under OpenVMS                             March 1998
==============================

You need perl and ImageMagick installed on your VMS system to run Gifmap (and
his brother gifmapsel). Gifmapsel is another version of gifmap with a new
option:

 --selectiontype image_type

to select only one image file type (for ex gif, jpg, etc ...).

gifmap try to translate a VMS path into a Unix syntax before writing an URL.
You generally need to define a rooted logical to correctly map your VMS path to
your WWW path.

Example:

The WWW /toto path is located to disk2:[tata.toto]
Before running gifmap, you need to define toto logigal name:

$ def/trans=conc toto disk2:[tata.toto.]

and

$ set def toto:[000000]

Now you can run gifmap or gifmapsel without need to edit the html files created
by gifmap.

Nota: Comments written by gifmap into html index files are in French.

You can invoque SETUP.COM procedure to define symbols and use gifmap
directly from this distribution.

Enjoy !

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
