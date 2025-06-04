WXWINDOWS, GRAPHICS, Free C++ GUI toolkit


   =====                  Announcing wxWindows 1.67                 =====
   =====  A Free C++ GUI toolkit for Motif, Open Look, Windows 3.1,  =====
   =====                  Windows 95 and Windows NT                  =====
   =====                           --o--                             =====
   =====          http://web.ukonline.co.uk/julian.smart/wxwin       =====

   wxWindows is a C++ framework for developing multi-platform, graphical
   applications from the same body of C++ code. Currently supporting
   Motif, Open Look and MS Windows, a Mac port is under development.
   A third-party port supports development using Xt.

 Main features

   +   Simple-to-use, object-oriented API
   +   Graphics calls include splines, polylines, rounded rectangles, etc.
   +   Simple programmatic panel/dialog item layout
   +   Support for menu bars and status lines
   +   Toolbar class
   +   Pens, brushes, fonts, icons, cursors, bitmaps
   +   Easy, object-oriented interprocess communication (DDE subset) under
       MS Windows and UNIX
   +   Encapsulated PostScript generation and MS Windows printing
   +   Print/preview framework
   +   Document/view/undo framework
   +   Memory-checking and debugging features
   +   ODBC subset support
   +   Virtually automatic MDI support under MS Windows
   +   Support for MS Windows printer and file selector common dialogs, with
       equivalents for UNIX
   +   Under MS Windows, support for copying metafiles to the clipboard
   +   Programmatic form facility for building form-like screens fast,
       with constraints on values
   +   Applications can access MS Windows Help or wxHelp help system
   +   All source, examples
   +   Reference manual in PostScript, wxHelp, WinHelp, HTML, RTF forms

 Additional features

   +   Simple-to-use Windows program installation utility for delivering
       your applications
   +   wxCLIPS: an interface to NASA's CLIPS, for rapid GUI prototyping
       and application extension
   +   wxPython: an IDE for rapid object-oriented GUI development
   +   Tex2RTF utility for maintaining online and printed manuals
   +   Tree layout, graph layout, charting and grid classes
   +   Contributed advanced editor/hypertext classes
   +   Dialog Editor

 Compilers supported

   +   Most Windows compilers: MS Visual C++ 1.x/4.x/5.x,
       Borland 3.1/4.x/5.x, Symantec C++, Watcom C++ (WIN32 only),
       GNU-WIN32
   +   Most UNIX compilers, including GCC

 Platforms supported

   +   All MS Windows variants
   +   Most if not all UNIX variants, including Linux, Solaris 1.x,
       Solaris 2.x, HP/UX, SGI IRIX, OSF/1, IBM AIX
   +   VAX/VMS (partial support)
   +   Mac (68K and PPC) port in progress, with alphas available

=============================================================================
This kit is preconfigured for building under OpenVMS VAX & Alpha with DEC CXX
and DEC C compilers. Extra libraries compiled are xpm, image and wxstring

The make processus is not able to include extra libraries into [.lib]wx.olb
(too long dcl line ...). I've put a small dcl script [.lib]extralibs.com
to complete the job. You can invoke it after the build if you want to have all
modules into wx.olb.

Please, take allok at Stefean Hammes' VMS FAQ into the kit.

Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr
