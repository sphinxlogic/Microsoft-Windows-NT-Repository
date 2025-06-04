wxImage: A Platform Independent Image Class Library

Alejandro Aguilar-Sierra

You can display and stretch out images and read/write them from several
image file formats. The "favorite" formats will be JPEG and PNG, but
code are provided to manage BMP and GIF. Also, a wxImage object can be
constructed from a wxBitmap object.


Main features
=============

1. Display grayscaled, paletted and fullcolor images on a wxDC.

2. Easy bitmap manipulations regardless the platform.
   You don't access directly the image memory, instead
   use portable member functions. It is possible to sweep over
   an image efficiently using iterators.

3. High performance because it uses an internal system dependent format.

4. Gamma correction (jpeg & png) and alpha channel (png).


Classes
-------

wbImage		Basic methods for managing images. It is the only
		unportable class because it defines the internal format.

wxImage		Provides file management for all the file formats
		supported. It could use external portable libraries
		like libjpeg and libpng.

imaIter		The basic iterator. 

wxImaIter	More complex iterators.


Applications
------------

This library provides a basis for

o Digital Image Processing

o Warping/Morphing

o Raytracing, radiosity.

o Animation, games.

o And more... !


Status 
======

Third alpha version.

To get a real beta, still is needed:

o X version (would you like to help? :-)

  wbImage should be based on a simplified version of
  Julian Smart's wxImage.
  
o Good documentation.

			     
Bugs & technical problems
------------------------
	    
o Better integration into wxWidows.

o DIB orientation problem under Win16.


wxWindows dependences
---------------------

At this stage an image can be displayed only on a CanvasDC, but
when this stuff gets more stable and gets part of the wxWindows
distribution, it will support the other DC's (PS, printer, etc), since
Draw and Stretch will be methods of wxDC class.

The internal formats are defined to be compatible with the display
system (X or Windows). If your application don't need this to display
the image, you can derive a class from the abstract class ImaBase.
With wbImage all the palette operations are managed using wxColourMap,
but you could change this in your new derived class, and then all the
other classes could derive from your class.




Please let me know your suggestions, comments etc.

--
Alejandro Aguilar-Sierra
asierra@servidor.unam.mx
