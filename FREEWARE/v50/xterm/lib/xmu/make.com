$ if( f$trnlnm("XMU_DIRECTORY") .eqs. "")
$ then
$   define x11_directory decw$include
$   thisfile = f$environment("PROCEDURE")
$   thisis   = f$parse(thisfile,,,"DEVICE") + f$parse(thisfile,,,"DIRECTORY")
$   define xmu_directory 'thisis'
$ endif
$!
$ mycc :== cc/include=(decw$include)/define=VMS
$ mycc ALLCMAP.C        
$ mycc ATOMS.C          
$ mycc CLIENTWIN.C      
$ mycc CLOSEHOOK.C      
$ mycc CMAPALLOC.C      
$ mycc CRCMAP.C         
$ mycc CRPIXFBIT.C      
$ mycc CURSORNAME.C     
$ mycc CVTCACHE.C       
$ mycc CVTSTDSEL.C      
$ mycc DEFERRMSG.C      
$ mycc DELCMAP.C        
$ mycc DISPLAYQUE.C     
$ mycc DISTINCT.C       
$ mycc DRAWLOGO.C       
$ mycc DRRNDRECT.C      
$ mycc EDITRESCOM.C     
$ mycc FTOCBACK.C       
$ mycc GETHOST.C
$ mycc GRAYPIXMAP.C     
$ mycc INITER.C         
$ mycc LOCBITMAP.C      
$ mycc LOOKUP.C         
$ mycc LOOKUPCMAP.C     
$ mycc LOWER.C          
$ mycc RDBITF.C         
$ mycc SCROFWIN.C       
$ mycc SHAPEWIDG.C      
$!
$! SUN bit
$! mycc SHAREDLIB.C      
$ mycc STDCMAP.C        
$ mycc STRTOBMAP.C      
$ mycc STRTOBS.C        
$ mycc STRTOCURS.C      
$ mycc STRTOGRAV.C      
$ mycc STRTOJUST.C      
$ mycc STRTOLONG.C      
$ mycc STRTOORNT.C      
$ mycc STRTOSHAP.C      
$ mycc STRTOWIDG.C      
$ mycc UPDMAPHINT.C     
$ mycc VISCMAP.C        
$ mycc WIDGETNODE.C
$ mycc XCT.C            
$ mycc _STRTOPMAP_VMS.C 
$!
$ library/object/create xmulib.olb *.obj
$ delete *.obj.*
