BAT V1.9, TOOLS, Easily submit commands to batch queues
                                       
   [9-AUG-1994]
   
   BAT lets you submit multiple commands to a batch queue easily and
   quickly. For example, you can use:

    $ bat cc := cc/vaxc|mmk
   
   or

    $ bat
    _Command: cc := cc/vax
    _Command: mmk
    _Command: ^Z
    $
   
   to define CC for that batch job and then run MMK in the current
   directory. BAT will automatically create the a temporary .COM file
   that sets the default to the current directory and includes the
   commands to be executed. The .COM file is then submitted to a batch
   queue for processing.
   
   BAT is written in BLISS. Complete sources are provided.
   
     _________________________________________________________________
   
  CONTACTING THE AUTHOR
  
   BAT was written by Hunter Goatley. To report bugs, please send mail to
   MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing lists are
   available as well.
