LYNX283, Utilities, A Terminal-based Web Browser

Lynx V2.8.3 (a terminal driven Web browser)

Updated
  Tue May 2 20:04:01 2000

Author
  Lou Montulli et al.

Help
  http://lynx.browser.org/

Comment
  This re-packaged version includes Slang 1.4.1. Current distribution 
  includes an OpenVMS 7.2 Alpha executable linked with Slang and Multinet 
  TPIP. Usage of -color will (at last) make colors available for VMS Lynx.


--

Also provided is a version of Lynx V2.8.2 built by Dick Picard, with the
updated files and the file DIFFs for OpenVMS, in the OSU subdirectory.

"Enhancing LYNX's Web Crawling

Project

     I have modified LYNX 2.8.2 for more effective use with the VMSINDEX 
     software for creating keyword searchable indexes of Web sites, when 
     used with command lines such as: 

         $ CRAWL :== "$DISK9:[LYNX]LYNX -NOPAUSE -CRAWL -TRAVERSAL -REALM"
         $ CRAWL http://www.ohiou.edu/

     The changes include the following: 

       1.Enhancing the parsing of the META tags in the HEAD to use the 
         "keywords" and "description". 

       2.Writing the META-tagged keywords and description to the output 
         file immediately after the URL and TITLE when crawling. 

       3.Provided an environment variable for the number of subdirectories 
         to use for the output files when crawling. VMS exhibits seriously 
         degraded performance when more than about 1,100 files are in the 
         same directory, because the directory file exceeds 127 blocks. 

       4.Overloading the "-nopause" option to have it also prevent querying 
         for passwords. 

       5.The compiler complained because an unsigned int was being subjected 
         to a comparison "<= 0", so I changed it to "== 0" to avoid the 
         error message."

