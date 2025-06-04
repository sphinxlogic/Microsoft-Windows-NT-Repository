XTIFF, MOTIF_TOYS, View a TIFF file in an X window (mono and multi-pages)

    xtiff
    is a TIFF viewer for X based on the libtiff (3) package.
    It displays a TIFF file in an X window that can be resized and panned.
    On appropriate display hardware it can handle 24-bit RGB color TIFF files,
     8, 4 and 2-bit palette color TIFF files
    and 8, 4, 2 and 1-bit grayscale TIFF files.
    The Next and Previous
    buttons allow the user to view different pages
    in a multi-page TIFF file.

    If the image is larger than the window,
    you can pan around the image with the mouse or arrow keys.
    Grab the image by pressing down
    and holding the left mouse button
    and then drag the mouse
    to expose a different region of the image.
    The arrow keys provide another method for moving
    a large image inside a smaller window.

    xtiff
    manages the negotiation between the needs of an image
    and the visual capabilities made available by an X server.
    If necessary,
    it will promote an image to a deeper visual,
    but it will not demote an image by quantizing and/or dithering.
    In that case it will fail to display the image.
 
    xtiff
    reads the "Gray Response Curve", "Gray Response Unit"
    and "Color Response Curve" tags in a TIFF file.
    The data in these tags describe gamma compensation or image companding.
    Together with the -gamma
    option or the NTSC default gamma value of 2.2,
    the image will be gamma corrected and displayed.

    For example, if a TIFF file has been prepared for a typical display,
    it has a gamma compensation of 2.2 built into either the image
    or preferably the "Color Response Curve" tag.
    This is a device-dependent image and,
    in this case, the value for the -gamma
    command line option should be 1.0.
    If the "Color Response Curve"
    tag describes a companded but otherwise device-independent image
    then the command line gamma should be set according to the monitor.

    Unfortunately there is no way of knowing a priori whether or not
    an image is device-independent without knowing its ancestry.
    If the image conforms to the TIFF 5.0 specification
    it should be device-independent;
    but many scanner and image processing programs
    do not adhere rigorously to the standard.

