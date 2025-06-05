Perl, Languages, Powerful scripting language

Disclaimer
==========
While this Perl distribution for the hp OpenVMS freeware CD was packaged 
by Martin P.J. Zinser (zinser@decus.de) and I am certainly willing to help 
with questions and problems concerning its usage, I am a dwarf standing on the 
shoulders of giants here. A number of very talented individuals are working
hard to make sure OpenVMS support is well integrated into the Perl language.
This specifically includes Craig Berry, Peter Prymmer, Dan Sugalski, and 
Charles Bailey although this list is not exhaustive. If you are interested
in Perl on VMS consider joining the VMS Perl mailing list as described 
below and you will soon find out that there are many good people contributing
to this effort.

Versions on the freeware CD

Perl 5.8.1  

Modules
CGI           		2.98
Chart::Plot             0.11
Compress::Zlib          1.22
Date::Manip   		5.43
DBI                     1.38
GD            		2.06
Mail::Sender  		0.8.06
Math::SigFigs 		1.01
Math::VecStat 		1.08
MIME::Base64  		2.20
Parse::RecDescent       1.80
Spreadscheet-WriteExcel 0.41
Time::Modules           2003.0211
VMS::Device             0.08
VMS::FileFind           0.91
VMS::FileUtils          0.14
VMS::IndexedFile        0.02
VMS::Misc               1.01
VMS::Monitor            0.06
VMS::System             1.04
VMS::User               0.01

The version on this CD does contain a pre-build binary distribution for 
OpenVMS Alpha in the [.perl-bin] directory. This should work on 7.2-1H1
and later. For regular use backup this directory from the CD to disk and 
change perl_setup.com to point to the new location.

If you want to re-build from sources backup the [.perl-src] and [.mod]
directories from CD to disk. As a convenience there is a make.com provided
with this distribution which does build the perl interpreter as well as the 
additional/updated Modules. This procedure does accept the following 
parameters in arbitrary order:

MAKE=<MMS||MMK>
Enforce usage of MMS or MMK to build Perl over the build-in detection
by the procedure.

IDIR=<PATH>
Installation directory where the Perl binaries and libraries are to be 
moved after compilation

TARGET=<MODULE>
If invoked without this parameter bothe the perl interpreter and the 
add-on moudles will be build. If set to PERL only the perl interpreter
will be build and when given a value of MODULE only all the addon 
moudles will be build. To build a specific module use one of the following
as the target:

  CGI, CPLOT, czlib, dbi, datemanip, gdpm, mailsender, mimebase64, recdescent,
  sigfigs, timemod, vecstat, vmsdevice, vmsfileutils, vmsfindfile,
  vmsindexedfile, vmsmisc, vmsmonitor, vmsqueue, vmssystem, vmsuser,
  writeexcel

Below you can find a general discussion about building Perl with the procedures
that are part of the "standard" distribution. If you either want to change
particular settings to influence a particular aspect of Perl or want to 
build a new Perl version not on the CD it is highly recommended you get 
familiar with this

Note for users with experience on Perl prior to 5.8.0: The default type of 
floating point numbers has changed from D-Float to IEEE float as of 
5.8.0. If you do need D-Float support you need to recompile Perl and 
change the appropriate setting during execution of @configure.

================================================================================


Building


The installation of Perl has vastly improved during the past years. Nowadays 
it is almost as simple to install Perl on OpenVMS than it is on Unix ;-)
 
The build consists of two steps. First configure.com is used to determine the 
local configuration and then a "make" program is used to perform the actual 
compilation. 

configure.com

configure.com can be invoked with a number of different options. These include: 

"-d"
Use defaults for all answers.
 Handy if you have a normal installation and don't want to be bothered with 
 lots of questions by install. As far as I can tell the defaults used are 
 pretty reasonable. 
"-e"
Go on without questioning past the production of config.sh
 Together with the -d and -s options this suppresses many informational 
 messages during the build. This can be usefull for automated production 
 builds. 
"-f"
Specify an alternate default configuration file
 Configure.com by default looks for config.sh and [.vms]config.vms for 
 configuration information from previous invocations. If you want to override 
 settings in these files you can add your own configuration file with this 
 switch. 
"-h"
Prints a short help message and exits (with an informational status). 
"-m"
Skip the MANIFEST check to see that all files are present 
"-r"
Reuse C symbols value if possible (skips costly nm extraction)
 Seems not to be implemented on OpenVMS as of 5.8.0. 
"-s"
Silent mode, only echoes questions and essential information, the rest will be 
sent to the Null-Device NLA0 ;-) 
-"D"
Define symbol to have some value 

  -"Dsymbol"               symbol gets the value 'define'
  -"Dsymbol=value"         symbol gets the value 'value' 

-E
Stop at the end of questions, after having produced config.sh.
 Doesn't do much yet, but prevents configure.com from going into silent mode 
 after the creation of config.sh. 
-K
Do not use unless you know what you are doing.
 Currently you might think you knowitall, but configure.com knows it better ;-).
 Setting this doesn't change the execution yet. 
-O
Let -D and -U override definitions from loaded configuration file.
 Seems not to be implemented on OpenVMS as of 5.8.0 
-S
Perform variable substitutions on all .SH files (can mix with -f)
 Seems not to be implemented on OpenVMS as of 5.8.0. 
-"U"
Undefine symbol: 

  -"Usymbol"    symbol gets the value 'undef'
  -"Usymbol="   symbol gets completely empty 

-V
Print version number and exit (with a zero status) 

My personal strategy is to perform one run with configure going through all 
the questions and options to get some idea about the new version and 
afterwards to use @configure "-des" to do the production build.
One of the essential things checked by configure.com is which "make"-tool is 
available on your system. It currently checks for the following: Imake, 
Gnumake, MGmake, Gmake, Make, MMS, and MMK. I personally always use MMK. MMS 
should work fine too, since it is pretty similar to MMK ;-) Gnumake might or 
might not work, at least it is the only of the other tools that I've seen a 
current version of. 


At the end of the execution of configure.com a line will be printed showing 
how you should invoke your "make"-tool to actually build Perl. Once you've 
typed in this command (with no changes compared to the example provided by 
configure.com) no further user intervention should be required. 


Setup


During the installation of Perl the DCL-procedure [.vms]perl_setup.com is 
created, which can be used as a starting point for the definiton of logicals 
and symbols. The procedure already defines everything which is undispensable 
for working seriously with Perl, but still you might wish to add some of the 
following: 

DEFINE/TRANS=(CONC,TERM) PERL_ROOT <path>
The path to the place where Perl is located on your system. Necessary to get 
access to library functions and documentation. 

PERL :== $<path>PERL.EXE
The symbol used to invoke Perl. 

PERLSHR :== <path>PERLSHR.EXE
Symbol pointing to the shareable image containing most of the routines 
belonging to Perl. 

PERLDOC :== "$<path>PERL PERL_ROOT:[LIB.POD]PERLDOC.COM -T"
Read documentation concerning perl available in POD format. 

DEFINE/NOLOG PERLDOC_PAGER <pager>
The file viewer used to display the documentation files. I personally prefer 
Look, but you're certainly entitled to have an own opinion about this ;-> 
(Yes, I'm really sick and tired to all the My xxx is better than your yyy 
flame wars). 

PERLBUG :== "$<path>PERL PERL_ROOT:[LIB]PERLBUG.COM"
Generates a template for a bug report providing all necessary details on your 
local perl installation. 

DEFINE/NOLOG POD2MAN PERL_ROOT:[LIB.POD]POD2MAN.COM
 POD2MAN :== "$<path>PERL POD2MAN"
Translates files in POD format to nroff. Some build procedures for perlmodules 
might complain if this symbol is not defined. 

DEFINE/NOLOG POD2TEXT PERL_ROOT:[LIB.POD]POD2TEXT.COM
 POD2TEXT :== "$<path>PERL POD2TEXT"
Translates files in POD format to plain text. This is also required for 
perldoc -f <internal function> to work properly. 

Basics introduction

I've once prepard a talk for our DECUS LUG covering the basics of Perl and awk. 
If you don't have an idea about Perl yet you might want to check this out 
starting with [.talk]ap_toc.html. 

(P.S.Yes, all of the stuff done there can be accomplished much easier with the 
VMS::User and Mail::Sender modules, but this particular presenataton is already
a few years old and I still think it gives a very basic idea about the things
one can do with Perl.)

Tips and Tricks

To retrieve the translations of a multi valued logical name use code like the 
following: 

$ define myname martin, zinser
$ show log myname
   "MYNAME" = "MARTIN" (LNM$PROCESS_TABLE)
        = "ZINSER"
$ perl -e "print $ENV{'myname;'.0}"
MARTIN
$ perl -e "print $ENV{'myname;'.1}"
ZINSER


This may not work as expected on the Compaq supplied version of Perl 5.6.1. 


Mailing list


To stay informed about the latest developments concerning new versions of Perl 
and problems discovered with the current version you can subscribe to the 
VMSPerl mailing-list by sending a message to vmsperl-subscribe@perl.org . 


Further notes on Perl Modules

Building


Every module should contain a script named Makefile.PL. Executing this 
perl script should create a makefile (descrip.mms) which then can be used with 
make (MMS/MMK) to actually perform the installation. The normal procedure for 
installing a perl module therefore looks like this: 

perl Makefile.PL
mmk
mmk test
mmk install


Note: According to my experience this only works if you place the sources for 
the modules not inside the source tree of the normal Perl installation. 


Examples


Online Example


The following example shows a number of the mentioned packages in use. CGI is 
used to create an HTML document on the fly, which contains an input field, 
where one can enter some value. If this value can be interpreted as a date by 
datemanip it is echoed back and converted to a julian date using one of the 
Time-Modules. Vecstat is used to calculate the sum of year, month, and date 
(silly, I know ;-). In a second field you can enter your age, which will be 
echoed back rounded to one significant digit (usually decades ;-) using the 
SigFigs module. Finally some information about the VMS environment is shown. 
VMS::Queue is used to determine the batchjob running the webserver and the 
submission time of the job is printed on the page. VMS::User determines the 
last non-interactive login for the server account (which most of the time will 
correspond to the last script invocation on the server). VMS::System is used 
to retrive information on the hardware the server is executing on. 
VMS::Monitor then is used to determine how many processes currently execute on 
the system at all and VMS::Device gives space information for the disk housing 
the webserver. Which disk this is, is detemined using VMS::FileUtils::Root to 
translate the appropriate rooted logical name. GD is used to create a graphic 
showing free vs. used disk space on the device. In the next step your age is 
used to determine the starting letter for software to be pulled in from a list 
kept in an indexed file via VMS::IndexedFile. Subsequently VMS::FindFile 
generates a list of the currently stored disk usages graphics in the temp area 
of the webserver. Finally the Spreadsheet::WriteExcel module (which relies on 
Parse::RecDescent) is used to put some of the data into an Excel Spreadsheet, 
which you may download if you are inclined to do so.

#
# Define the Modules to be used
#
use CGI::Pretty qw(:all -no_xhtml);
use Date::Manip;
use GD;
use Math::SigFigs;
use Math::VecStat qw(max min maxabs minabs sum average);
use Spreadsheet::WriteExcel;
use Time::JulianDay;
use VMS::Device qw(device_list device_info);
use VMS::FileUtils::Root;
use VMS::FindFile;
use VMS::IndexedFile;
use VMS::Monitor;
use VMS::Queue qw(entry_list entry_info);
use VMS::System qw(get_one_sys_info_item);
use VMS::User;

#
# Base Perl modules
#
use File::Temp qw/tempfile/;
use File::Basename;

#
# Setup CGI object and print HTML form
#
$myform = new CGI();

print $myform->header();
print $myform->start_html("A Simple Example"),
      $myform->h1("A Simple Example"),
      $myform->start_form,
      $myform->p, "Please enter a date:",
      $myform->textfield(-name=>"date",-size=>"11"),
      $myform->p, "Please enter your age:",
      $myform->textfield(-name=>"age",-size=>"3"),
      $myform->p,
      $myform->submit,
      $myform->end_form,
      $myform->hr;
      
#
# Second invocation of the form - Analyze parameters
#      
if ($myform->param()) {

  $formdate = $myform->param("date");
#
# Decode date input (Datemanip)
#
  $pdate = &ParseDate($formdate);
  print $myform->p;
  if ($pdate eq ""){
    print "Date $formdate can not be parsed";
  }
  else{  
    $datecomp[0] = &UnixDate($pdate, "%Y");
    $datecomp[1] = &UnixDate($pdate, "%m");
    $datecomp[2] = &UnixDate($pdate, "%d");
#
#   Transform to Julian date (Time modules)
#
    $jd = &julian_day($datecomp[0], $datecomp[1], $datecomp[2]);
#
#   Calculate sum Year + Month + Date (Vecstat)
# 
    $sum = &sum(@datecomp);
    print $myform->p, 
          "The (gregorian) Date is ", &UnixDate($pdate, "%A, %Y%m%d"),
	  " which translates to a julian Date of ", $jd, ".",
	  $myform->br,
	  "The sum Year+Month+Day is ", $sum;
  }
  $myage = $myform->param("age");
  $sage = &FormatSigFigs($myage, 1);
  print $myform->p, "Your significant age is ", $sage;
#
#  Information about the currently executing Web-server job
#

  %entry_cond = {jobname=>"BATSER"};
  @entries = entry_list(\%entry_cond);
  $entryprop = entry_info($entries[0]);
  print $myform->hr, $myform->p, "Webserver process active since ", 
        $$entryprop{"SUBMISSION_TIME"}, $myform->br, "\n";   
#
#  Information about the last noninteractive login of the server account
#

  $UserName = getlogin();
  $uairef = VMS::User::user_info($UserName);
  print $myform->br, "Last non-interactive login at ", 
        $uairef->{"LASTLOGIN_N"}, $myform->br, "\n"; 

#
# ... on the Webserver system
#
#

  $hwtyp = get_one_sys_info_item("HW_NAME");
  print $myform->br, "Websever running on a ", $hwtyp, "\n";
  
#
# ... accomodating x processes
#  
  $procs = VMS::Monitor::one_monitor_piece("PROCS");
  print $myform->br, "with currently $procs active processes", "\n";
  
# ... and on the disk used for the Web-server
#
  $r = new VMS::FileUtils::Root;
  $path = $r->unrooted('www_root:[000000]');
  @words = split(/\//,$path);
  $pubdev = uc("_" . (($ENV{qq($words[1])} ne "") ?  $ENV{qq($words[1])} :
                                                     $words[1] . ":"));
  foreach my $devname (device_list($pubdev, "DISK")) {
    $DevHash = device_info($devname);
    $FreeBlocks = $DevHash->{FREEBLOCKS};
    $MaxBlocks = $DevHash->{MAXBLOCK};
    print $myform->p, "Disk-Status: Free/Max $FreeBlocks / $MaxBlocks";
  }
#
# ... create a small graph showing disk utilization using GD
#
  $split = ($FreeBlocks/$MaxBlocks)*200;
  $im = new GD::Image(200,25);
  $red   = $im->colorAllocate(255,0,0);
  $green = $im->colorAllocate(0,255,51);
  $white = $im->colorAllocate(255,255,255);
  $im->transparent($white);
  $im->interlaced('true');
  $im->filledRectangle(0,0,$split,25,$green);
  $im->filledRectangle($split,0,200,25,$red);
  
  system ("delete/before=yesterday public\$root:[www.tmp]DISK*.png;*");
  $path = File::Temp::tempnam("public\$root:[www.tmp]","DISK") . ".png";
  ($name,$dum1,$dum2) = fileparse($path);
  $wname = "/www/tmp/" . $name;			    
  open(MYIMG,">$path");
  binmode MYIMG;
  print MYIMG $im->png;
  close(MYIMG);
  print $myform->p, img({-src=>$wname});
#
# Silly use of IndexedFile 
#
  $letter = chr(65 + ($myage % 26));
  tie(%sw,VMS::IndexedFile,'public$root:[mgr]sw.dat',0);
  @rows = $myform->th("Program") . $myform->th("Version");
  foreach (keys(%sw)){
    if (/^$letter/){
      @words = split(/\s+/,$sw{$_});
      push(@rows, $myform->td($words[0]) . $myform->td($words[1]));
    }
  }
  print $myform->table({-border=>"3"},
                       $myform->caption("Programs recommended for your age"),
		       $myform->Tr(\@rows)
		      ); 
#
# Show earlier versions of Disk usage graphic using VMS::FindFile
#
  print $myform->h2("Old diskgraphs as per VMS::FindFile");
  print "<ul>\n";
  my $ff = VMS::FindFile->new("public\$root:[www.tmp]DISK*.png");
  while (my $filename = $ff->search()){    
    ($name,$dum1,$dum2) = fileparse($filename,';.*');
    print $myform->li($myform->a({-href=>"/www/tmp/$name"},$name));
   }
  print "</ul>\n";
#
# Finally turn some of the stuff into an Excel-Sheet for optional downloading
#
  system ("delete/before=yesterday public\$root:[www.tmp]PM*.xls;*");
  $path = File::Temp::tempnam("public\$root:[www.tmp]","PM") . ".xls";
  ($name,$dum1,$dum2) = fileparse($path);			    
  
  my $workbook = Spreadsheet::WriteExcel->new($path);
  my $sheet = $workbook->addworksheet("From Perl");
  my $psheet = $workbook->addworksheet("Programs");
  $sheet->set_column(0,0,20);
  $sheet->set_column(0,1,25);
  $sheet->write(0,0, "Date entered");	
  $sheet->write(0,1, $formdate);
  $sheet->write(1,0, "Weekday");
  $sheet->write(1,1, &UnixDate($pdate, "%A"));
  $sheet->write(3,0, "Up since");
  $sheet->write(3,1, $$entryprop{"SUBMISSION_TIME"});
  $sheet->write(4,0, "Last script exec");
  $sheet->write(4,1, $uairef->{"LASTLOGIN_N"});
  $sheet->write(5,0, "System type");
  $sheet->write(5,1, $hwtyp);
  $sheet->write(6,0, "# of procs");
  $sheet->write(6,1, $procs);
  $sheet->write(7,0, "Space used");
  $sheet->write(7,1, $MaxBlocks - $FreeBlocks);
  $psheet->set_column(0,0,20);
  $psheet->set_column(0,1,15);
  $i = 0;
  foreach (keys(%sw)){
    if (/^$letter/){
      @words = split(/\s+/,$sw{$_});
      $psheet->write($i,0, $words[0]);
      $psheet->write($i,1, $words[1]);
      $i++;
    }
  }
  print $myform->p,     
        $myform->a({-href=>"/www/tmp/$name"},"Excel sheet"),
	" with output data (will be removed periodically)";
}
print $myform->end_html;


 For the curious, this example can be seen in action at 
 http://zinser.no-ip.info:8080/htbin/perl_mod.pl. 


Mail Example


As might be expected in the days of SPAM I do not provide the following 
example on-line. It uses Mail::Sender (and implicitly MIME::Base64) to 
distribute a mail to a number of addreses read from a file. Obviously any 
number of improvements can be made to this sketch, but it does show how to use 
the module(s). 
use Mail::Sender;
use Getopt::Std;

getopts('s:f:');

if (not $opt_s){
  die "-s \"Subject\" Missing ... exiting\n";
}

&build_add_list;
 
&check_body;

ref ($sender = new Mail::Sender({from => 'foo@bar.de',
                                 smtp => 'mail.foobar.net'})
    ) ;
printf "with Subject %s ...\n", $opt_s;    
for($i=0;$i<@add;$i++){
  &send_body;
}    
exit;

sub build_add_list {
  open(ADD,"email.txt");
  my $i = 0;
  while(<ADD>){
    chop();
    s/\s+//g;
    if ($_ ne ""){
      $add[$i++]=$_;
    }
  }
  close(ADD);
}		       				 

sub check_body {
  die "-f file with Mail contents missing ... exiting\n" if (not $opt_f);
  die "File $opt_f can not be read ... exiting\n"  if (not -r $opt_f); 
  if (-T $opt_f) {
    printf "Text message found in $opt_f will be sent\n";
    open(IN, $opt_f);
    @text = <IN>;
    close(IN);
  }
  else {
    printf "Binary contents in $opt_f will be sent as attachment\n";
  }  
}

sub send_body {
  if (-t $opt_f) {
    (ref ($sender->Open({to => $add[$i], subject => $opt_s}))
     and print "Start Mail to $add[$i] ok\n"
    )
    or die "$Mail::Sender::Error\n";
    $sender->SendEx(@text);
    $sender->Close();  
  }
  else {
    (ref ($sender->MailFile({to => $add[$i], subject => $opt_s,
                             msg => "Please check attachment\n Me\n",
			     file => $opt_f}))
     and print "Mail to $add[$i] sent ok\n"
    )
    or die "$Mail::Sender::Error\n"; 
     			     
  }
}


Perl reference guide


Since the Perl reference guide is not really a module the installation process 
is a bit different than described above.
You'll need MMS/MMK to build the file. The process is described in detail in 
the readme file of the package. In short 

mmk testpage.ps
 Print the page, find the horizontal offset by folding the resulting page and 
 change makefile accordingly. Repeat this step until the printout is centered 
 on the page. 
mmk refguide.ps
 Printing the resulting file refguide.ps should give you a copy of the 
 reference guide which can be cut and stapeled to a readily useable paper 
 version of the reference guide. 

The version from this server contains an adapted copy of makefile 
(makefile.lps17_vms) for use with an DEC LPS17 printserver, using A4 paper and 
duplex printing. As you can see from this the changes needed are really easy. 
