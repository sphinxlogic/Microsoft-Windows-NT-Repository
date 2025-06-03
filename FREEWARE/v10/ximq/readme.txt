
	    XIMQ - Display IMQ Images from NASA PDS CDs.


The XIMQ application was developed as a demo for ISO 9660 CDs.  The PDS
images are distributed as compressed greyscale data, and a volume descriptor
and image index file.

XIMQ reads the volume descriptor and image index, and allows the user to
select a image to display.  It then provides a display of the image, and
allows the user to change display parameters, and do a number of image
processing operations on the image.

There are a number of command line operators to control the operation of
the application:

  -title		Allows a title to be supplied for the main application
			window.

  -icon			Allows icon text to be supplied for XIMQ.

  -display		Allows the X11 display to be specified.

  -g <x> <y> <w> <h>	Specify the main window geometry.  Normally the
			window will be placed on the display using a
			"best fit" routine that looks at the current window
			population on the screen.

  -file	<dev:file>	Tells XIMQ to read and display a specific IMQ file.

  -index <dev:file>	Specifies an alternate index file.

  -vol <dev:file>	Specifies an alternate volume information file.

  -colors <n>		Specifies the number of colors to use (default = 128)

  -cmap			Force the use of a private colormap.

  -staticcolor		Force the use of a read-only colormap (uses screen
			default if not specified).

  -noequal		Disables equalization.  Normally, the image histogram
			is scanned to find the lowest and highest intensity
			levels, and the pixels are mapped from black to white
			within this range.  Specifying this switch will cause
			the pixels to be mapped from black to white without
			regard to the content of the image.

  -equal <p> <l> <h>	Specify parameters to automatic equalization.  To find
			the high/low levels, a threshold value is used.  This
			is because there is almost always "noise" at all levels.
			So a threshold <p> is used as a percentage of the
			highest pixel count.  The default is .015.  The input
			value will be multiplied by .001.  In addition, the
			black and white points can be adjusted by the <l> (for
			black) and <h> for white (0-255).

  -limits <b> <w>	Specifies the black and white levels for equalization.
			The range is from 0 to 255.  Forces manual equalization.


  -shift <l> <i>	Specifies the linear brightness <l> and the amount to
			shift the brightness from the menu.  The default is
			0 for the brightness, and 5 for the increment.

  -gamma <l> <i>	Specifies the gamma <l> adjustment, and the amount to
			change the gamma from the menu <i>.  The default gamma
			is 1.2.  The input values are multiplied by .01

  -nogamma		Turns off gamma correction.

  -flconvolve		There are two sets of convolution logic for some of
			the image processing selections.  One uses integer
			arithmetic, the other uses floating point.  This
			switch forces floating point.

  -inconvolve		There are two sets of convolution logic for some of
			the image processing selections.  One uses integer
			arithmetic, the other uses floating point.  This
			switch forces integer only.

  -zoomstyle <n>	Allows different zooming methods.

			0 = Pixel Replication (default)
			1 = Nearest neighbor
			2 = Area (3x3) average

  -nosqzoom		Allows non-square zooming.  Normally, the image will
			only be scaled with a 1:1 aspect.

  -norbguns		Sets the red and blue guns to zero.  This is useful
			when the screen is monochrome, but the display
			hardware is color.  All intensity values are put on
			the green gun.

  -debug		Prints debugging statements.

  -sync			This sets XSynchronization to TRUE.  Each X11 operation
			is completed on return from the call.  Useful mostly for
			debugging.


Notes:

  Where do I get the images?

   NASA sells images from various missions on multiple CD sets.  They are
   distributed by Planetary Data Systems (PDS).

  Why does XIMQ not use colormap twiddling?

   The application was written to run on any X11 display, and to work on
   TrueColor 24-plane displays, as well as other non-pseudoColor devices.
   To do this, a number of functions that could be implemented as colormap
   operations - such as brightness and gamma correction, are implemented as
   pixel operations that require the image to be repainted.

  Why doesn't XIMQ use the X Toolkit?

   No particular reason, except that the author is not a Motif application
   programmer, and because some of the standard Motif widgets would be a bit
   slow to load up and modify (for instance the file selection window).

  What is "Equalization"?

   The image is supplied with a histogram, which is simply a 256 element
   array that contains a count of how many pixels in the image there are
   for each of the 256 possible intensities.

   The simplest way to display the image would be to map each image pixel
   from 0-256 to a colormap that contains a greyscale ramp from black to
   white.  However, most images have their intensities mostly concentrated
   in a much smaller range, and when the image is mapped in this fashion
   it appears "washed out" and does not have a lot of true black and white
   areas.

   The equalization that XIMQ provides is to scan the histogram and find
   the area where most of the intensity data exists, and use this pixel
   range as the bottom and top of the pixel ramp from black to white.  Any
   of the pixels outside this range are then clamped to either black, or
   white.  The result is that the image is less washed out, and has better
   contrast.

   There are 3 values of interest in this analysis:  The computed black and
   white thresholds, and a "threshold" value that is used to find them.
   Most non-synthetic images have some amount of noise outside of the range
   where most of the pixel intensities are.  So to just look for zero counts
   in the histogram will usually fail.  Instead, a value (the deault is .015)
   is multiplied by the highest pixel count to derive a "threshold" below
   which a count is considered "noise".  The histogram is then scanned from
   0 to find the first run of two pixel values that have a count higher than
   this threshold (this helps eliminate noise spikes) - and this is the black
   threshold.  The same process is done for white, scanning backwards from 255.

  What is "Calibration"?

   This is something that was hacked into the code.  Essentially, the PDS
   contains "Dark Current" images, which is what the camera sees when it
   is covered.  By scanning for a dark current image prior to the current
   image, that has the same characteristics as the image, and then subtracting
   the dark current image from the base image, the final image has any CCD
   anomolies removed.  This works *really* well most of the time.  On
   occasion, the dark current image can't be found, and things get a little
   bogus.  You can turn off calibration in the secondary menu.

  What is "Gamma Correction"?

   This is a non-linear function that is applied to the greyscale ramp to
   provide a better contrast lightening and darkening of the image.  The
   default is 1.2 which gives a modest darkening (contrast) to the image.
   A value of 1.0 will provide a linear ramp.

   To derive the gamma correction, the normal intensity for a pixel is
   converted to a value between 0 and 1, and then raised to the gamma
   value (that is, intensity ** gamma).  As part of the process that
   derives the final pixel value, the pixel intensity is looked up in
   a gamma correction table.

   Think of gamma as a contrast adjustment.

   I've also found that if you leave equalization on (the default) and then
   crank the gamma adjustment to 0.1 (gamma brighten) that image data that is
   not normally visible jumps into the picture.  A good example is image
   number 34698.58 and a number of the other S_RINGS pictures from the
   VOYAGER 1 series.  Outer rings will appear when this is done.

 What is "Linear Brightness"?

   This simply "shifts" the final pixel value by some amount.  So each
   pixel appears lighter (or if the shift is negative, darker).  It is
   a linear function, as it effects each pixel intensity in the same way.
   The actual pixel data isn't altered, just the relative brightness.

 What is the "Noise Filter"?

   This filter takes a pixel, plus all it's neighbors in a 3x3 matrix, and
   sorts them to find the median value.  The pixel is then replaced by the
   median value.  The end result is that very small areas of noise, that
   show up as bright dots, or lines - are replaced by a pixel value that
   blends into the rest of the image.

 What does "Enhance Edges" do?

   It applies a laplace filter to the data.  The result is to amplify any
   edges in intensity.  This logic probably needs some refinement, but it
   can show some neat stuff.

 What does "Blur Image" do?

   It does the same thing as the noise filter, only instead of the median
   value - it replaces the pixel with the average value.

 What are the zoom style selections?

   The simplest, and default, selection is a pixel replication.  Each pixel is
   simply multiplied up in size.  The result is simply very large square
   picture elements.  The nearest neighbor, and area average zooms will result
   in zoom images that are prettier... however, the image data is altered and
   is no longer exact.

