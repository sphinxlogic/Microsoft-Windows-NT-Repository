                        OpenVMS Freeware CD-ROM Readme
                        ------------------------------

Welcome to Compaq Computer Corporation's OpenVMS Freeware CD-ROM!!

All the software included on this CD-ROM is also available at the OpenVMS
web site:

   http://www.openvms.compaq.com/

This document contains a general introduction to the OpenVMS Freeware, 
as well as notes specific to the V5.0 distribution.

                             General Introduction
                             --------------------

The OpenVMS Freeware CD-ROM contains free software tools and utilities to aid
software developers in creating applications and managing and using OpenVMS
systems.  Contained on this CD-ROM are dozens of packages for VAX and Alpha
Systems in such categories as:

         AFFINITY             Interoperation with Microsoft Windows systems
         BUILD_TOOLS          Build tools
         WEB_TOOLS            CGI and HTML tools
         GAMES                Games (also see Motif Toys)
         GRAPHICS             Graphics tools
         LANGUAGES            Compilers, assemblers, and language-related
         MISCELLANEOUS        Or ask for a new category
         MOTIF_TOYS           X Windows, CDE, and Motif toys
         NETWORKING           Networking and network-related tools
         PROGRAMMING          Source Code Examples, APIs, etc
         SYSTEM_MANAGEMENT    Tools related to system management, security
         STORAGE_MANAGEMENT   Tools related to storage management
         UTILITIES            Random Utilities not related to other categories

Many of these tools are popular packages already widely known and in use, 
while others are internally developed Compaq tools our engineers are making
available to our OpenVMS customers. For example, the OpenVMS Freeware CD-ROM
includes Bliss-32 for OpenVMS VAX, Bliss-32 for OpenVMS Alpha, and Bliss-64 
for OpenVMS Alpha.

The Freeware CD-ROM includes the following features:

	o  Includes an easy-to-use 4GL-based menu system for
	   examining the contents of the CD-ROM.

  	o  Is in Files-11 format to make it readable on OpenVMS
           systems.

	o  Contains binaries, source code (except for the Bliss
           compilers), and documentation.

	o  Represents some of the most popular, most often
           requested free packages, plus new items never before
           made public.

Disclaimer on Testing, Quality, and Licensing

The OpenVMS Freeware for VAX and Alpha Systems CD-ROM set is provided "AS IS" 
without warranty.  Compaq imposes no restrictions on its distribution, nor 
on the redistribution of anything on it. Be aware, however, that some of 
the packages on the CD-ROM may carry restrictions on their use imposed by the
original authors. Therefore, you should carefully read the documentation
accompanying the products.

Compaq Computer Corporation is providing this software free of charge and
without warranty. Included in the [FREEWARE] directory on this CD are the GNU
software licenses. Unless otherwise stated in sources or source packages, the
GNU licenses cover all software supplied on this CD.

Compaq makes no claims about the quality of this software.  It is provided 
to the customer as a free service.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Remember, the packages contained herein are supplied AS IS.  They are
without any warranty, expressed or implied!!
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


                     General Freeware V5.0 release notes
                     -----------------------------------

  o Please take the time to review the information in the [21264] directory.
    Evaluate the packages using that information -- there are OpenVMS Alpha
    executable images present on the Freeware CD-ROM set that are flagged by 
    SRM_CHECK.

  o There were several problems building the second volume of the CD-ROM set,
    so you will see changes to the first lines of various FREEWARE_README.TXT
    files.  Several categories were also added after the submission deadline.
    A future Freeware distribution may add additional categories.  Apologies 
    for these changes, and for the last-minute changes.

  o Several submissions were unpacked only with difficulty, due to problems
    with ZIP incompatibilities.  Various file attributes were reset to match
    OpenVMS expectations, but there may well be additional files that do
    not have the expected file attributes.  One tool that may be useful is
    [000TOOLS]RESET_BACKUP_SAVESET_ATTRIBUTES.COM.

  o The menu system is deliberately not requiring all CD-ROMs in the Freeware
    set to be mounted at the same time -- this avoids the requirement to have
    multiple CD-ROM drives.  This also unfortunately makes it somewhat more
    difficult to rummage around on the various disks.

  o Please take the time to examine the following files:

      [000000]AAAREADME.V5
      [000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
      [000000]00FREEWARE_ABSTRACT.TXT

   These files are the same on all Freeware disks.

