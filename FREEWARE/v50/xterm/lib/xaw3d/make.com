$ if (f$trnlnm("XMU_DIRECTORY") .eqs. "")
$ then
$   define x11_directory decw$include
$   thisfile = f$environment("PROCEDURE")
$   thisis   = f$parse(thisfile,,,"DEVICE") + f$parse(thisfile,,,"DIRECTORY")
$   define xaw_directory 'thisis'
$   xmuis = thisis - "XAW3D]" + "XMU]"
$   define xmu_directory 'xmuis'
$   bitis = thisis - "XAW3D]" + "BITMAPS]"
$   define bitmap_directory 'bitis'
$ endif
$!
$ mycc :== cc/include=(decw$include)/define=VMS
$ mycc ALLWIDGETS.C     
$ mycc ASCIISINK.C      
$ mycc ASCIISRC.C       
$ mycc ASCIITEXT.C      
$ mycc BOX.C            
$ mycc CLOCK.C          
$ mycc COMMAND.C        
$ mycc DIALOG.C         
$ mycc FORM.C           
$ mycc FORM3D.C         
$ mycc GRIP.C           
$ mycc LABEL.C          
$ mycc LAYOUT.C         
$ mycc LIST.C           
$ mycc LOGO.C           
$ mycc MAILBOX.C        
$ mycc MENUBUTTON.C     
$ mycc PANED.C          
$ mycc PANNER.C         
$ mycc PORTHOLE.C       
$ mycc REPEATER.C       
$ mycc SCROLLBAR.C      
$!
$! SUN specific stuff
$! mycc SHAREDLIB.C      
$ mycc SIMPLE.C         
$ mycc SIMPLEMENU.C     
$ mycc SME.C            
$ mycc SMEBSB.C         
$ mycc SMELINE.C        
$ mycc SMETHREED.C      
$ mycc STRIPCHART.C     
$!
$! won't compile without error
$! mycc TEMPLATE.C       
$ mycc TEXT.C           
$ mycc TEXTACTION.C     
$ mycc TEXTPOP.C        
$ mycc TEXTSINK.C       
$ mycc TEXTSRC.C        
$ mycc TEXTTR.C         
$ mycc THREED.C         
$ mycc TOGGLE.C         
$ mycc TREE.C           
$! mycc VENDOR.C         
$ mycc VIEWPORT.C       
$ mycc XAWINIT.C        
$!
$ library/object/create xaw3dlib.olb *.obj
$ del *.obj.*
