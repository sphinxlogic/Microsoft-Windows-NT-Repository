VIM, UTILITIES, (Vi IMproved) Vi compatible text editor.
 
VIM version 6.1-18 
 
============================================================================== 
 
1. Getting started                                      *vms-started* 
 
Vim (Vi IMproved) is an almost 100% compatible version of the UNIX editor Vi.  
Vim runs on nearly every operating system known to humanity (almost any Unix  
flavor, MS-DOS, MS-Windows 3.1, MS-Windows 95/98/NT/2000, OS/2, Atari MiNT,  
BeOS, RISC OS, Macintosh, Amiga). Now use Vim on OpenVMS too, in character 
or X/Motif environment.  It is fully featured and absolutely compatible with 
Vim on other operating systems with multi level undo, syntax highlighting,  
command line history, filename completion, block operations, etc.  
 
Vim is especially recommended for editing programers who use to change 
development palatforms quite often. 
 
For more information, see http://www.vim.org and OS_VMS.TXT in this archive. 
You can download up to date executables from http://www.polarfox.com/vim/ 
 
============================================================================== 
 
2. This archive                                         *vms-archive* 
 
Vim archive contains all OpenVMS related files of Vim version 6.1 
 
   VIM  
    |- RUNTIME          runtime files, documentation, extra fonts etc. 
    |- SRC              complete source code 
    |- ALPHA            OpenVMS Alpha executables for gui and terminal  
    |- VAX              OpenVMS VAX executables for gui and terminal 
    |- GNU              Useful GNU tools that Vim might use 
    OS_VMS.TXT          OpenVMS users manual 
 
    Files description: 
    TEST.LOG		Output of <.src.testdir>make_vms.mms test file 
    INSTALL.LOG		Output main build 
    VERSION		Output of VIM --version 
 
============================================================================== 
 
3. Deploy                                               *vms-deploy* 
 
Copy all Vim runtime directory structure to the deployment position. 
Copy over all choosen executables to the VIM directory (GUI compiled can run 
in simple teminal mode as well, if you have MOTIF shared libraries installed) 
 
   vim  
    |-- doc 
    |-- syntax 
    |-- tools 
    |-- ... 
    vimrc    (system rc files) 
    gvimrc 
    VIM.EXE 
    XXD.EXE 
    CTAGS.EXE 
 
Add the following lines to your LOGIN.COM (in SYS$LOGIN directory). 
Set up logical $VIM to point to the VIM deployment directory as: 
 
>       $ define/nolog VIM device:<path> 
 
Set up some symbols: 
 
>       $ ! vi starts Vim in chr. mode. 
>       $ vi*m  :== mcr VIM:VIM.EXE 
 
>       $ !gvi starts Vim in GUI mode. 
>       $ gv*im :== spawn/nowait mcr VIM:VIM.EXE -g 
 
Create .vimrc and .gvimrc files (or _vimrc, _gvimrc) in your home directory 
(SYS$LOGIN). (optional) 
 
For more information about deployment, build from source code etc, please, 
check the OS_VMS.TXT file. 
 
============================================================================== 
 
Archive created by Zoltan Arpadffy <arpadffy@polarfox.com> 
 
