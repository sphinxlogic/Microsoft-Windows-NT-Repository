FREEWARE, FREEWARE, The Master OpenVMS Freeware README File

                        OpenVMS Freeware CD-ROM ReadMe
                        ------------------------------

Welcome to the HP OpenVMS Freeware V6.0 CD-ROM.

All the software included on this CD-ROM is also available for (free)
download at the OpenVMS web site:

   http://www.hp.com/go/openvms/freeware/

This document contains a general introduction to the OpenVMS Freeware as
well as notes specific to the Freeware V6.0 distribution.

                             General Introduction
                             --------------------

The OpenVMS Freeware CD-ROM contains free software tools and utilities to aid
software developers, partners and OpenVMS Hobbyists in creating applications,
and in managing and using OpenVMS systems, as well as packages that can serve
as programming source code examples.

Contained on this CD-ROM are of packages for OpenVMS VAX, OpenVMS Alpha, and
OpenVMS I64, divided into general categories including the following:

         AFFINITY             Interoperation with Microsoft Windows systems
         BUILD_TOOLS          Build tools
         WEB_TOOLS            CGI and HTML tools
         GAMES                Games (also see Motif Toys)
         GRAPHICS             Graphics tools
         LANGUAGES            Compilers, assemblers, and language-related
         MISCELLANEOUS        Or ask for the creation of a new category
         MOTIF_TOYS           X Windows, CDE, and Motif toys (see Games)
         NETWORKING           Networking and network-related tools
         PROGRAMMING          Source Code Examples, APIs, etc
         SYSTEM_MANAGEMENT    Tools related to system management, security
         STORAGE_MANAGEMENT   Tools related to storage management
         UTILITIES            Random Utilities unrelated to other categories
         FREEWARE             Reserved for use by Freeware CD-ROM constructs

Many of these tools are popular packages already widely known and in use, 
while others are internally developed HP OpenVMS tools our engineers are 
making available to you, our OpenVMS customers. For example, the OpenVMS 
Freeware CD-ROM includes Bliss compilers for various OpenVMS platforms.

The Freeware CD-ROM includes the following features:

	o  Includes an easy-to-use 4GL-based menu system for
	   examining the contents of the CD-ROM.

  	o  Is in Files-11 format to make it readable on OpenVMS
           systems.

	o  Contains binaries, source code (except for the Bliss
           compilers and specific other HP-provided packages),
           and documentation.

	o  Represents some of the most popular, most often
           requested free packages, plus new items never before
           made public.

Disclaimer on Testing, Quality, and Licensing

The OpenVMS Freeware CD-ROM set is provided "AS IS" without warranty.  HP
adds no Freeware-specific restrictions on the distribution or redistribution
of the contents of these disks, nor adds any specific restrictions onto the 
redistribution of any packages on it. Be aware, however, that various of the 
individual packages on the CD-ROM may carry restrictions on their use or on
their redistribution, as imposed by the original author(s) or owner(s) of 
the package(s). Therefore, you should carefully read the documentation 
accompanying any products of interest. 

HP is providing this software free of charge and without warranty -- copies
of packages (and various package updates that might become available) can 
usually be downloaded from http://www.hp.com/go/openvms/freeware, and 
(additional) copies of this CD-ROM set can be ordered from HP for a small
media replication, handling, and distribution charge.

Included in the [FREEWARE] directory on this CD are various GNU software 
licenses.

HP makes no claims concerning this software, and provides this CD-ROM 
distribution to the OpenVMS community and to HP customers as a free 
service.


  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

         Remember, all packages contained herein are supplied AS IS.

         All packages are without any warranty, expressed or implied.

      HP has performed no verification, no code reviews, and no testing.

     You are solely responsible for support, debug, test and remediation.

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



                     General Freeware V6.0 release notes
                     -----------------------------------

  o There were the usual problems building the distributions due to errors
    found in various FREEWARE_README.TXT files, and due to submissions that
    were missing the (required0 FREEWARE_README.TXT file.  The Keeper of the
    Freeware (KoF) apologizes for these changes, and for the last-minute 
    changes.

  o Several BACKUP saveset submissions were unpacked only with difficulty, due
    to problems with the ZIP archive files.  Various file attributes were reset
    to match OpenVMS expectations, but there may well be additional files that
    do not have the expected file attributes.  One tool that may be useful for
    this [000TOOLS]RESET_BACKUP_SAVESET_ATTRIBUTES.COM.

  o Most of the submissions in this kit were zipped solely to try to fit the 
    total volume of Freeware submissions -- and thank you for your response 
    to the Freeware collection effort! -- onto the available CD-ROM storage.
    Kits providing PCSI or VMSINSTAL installation kits were zipped in multiple
    parts, and the installation kits were left available outside the main zip
    archive for the submission.  For the unzip tool, please see [000TOOLS] 
    and (if you cannot use the pre-built version here for some reason) please 
    see the various sites offering pre-built Zip images referenced in the
    OpenVMS Frequently Asked Questions (FAQ).

  o Kits that were submitted as zip archives were unzipped first, and then
    re-zipped into a consistent format and settings.  This means various of 
    the submissions will appear different than what was originally submitted.
    For the unzip tool, please see [000TOOLS] and also please consider the 
    unzip download pointers in the OpenVMS Frequently Asked Questions (FAQ).

  o The menu system is deliberately not requiring all CD-ROMs in the Freeware
    set to be mounted at the same time -- this avoids the requirement to have
    multiple CD-ROM drives.  This also unfortunately makes it somewhat more
    difficult to rummage around on the various disks.

  o Please take the time to examine the following files and directories:

      [000000]AAAREADME.*
      [000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
      [000000]00FREEWARE_ABSTRACT.TXT
      [000TOOLS]*.*
      [FREEWARE]*.*
      [VMSFAQ]*.*

    These files are replicated on all Freeware CD-ROM disks.

  o Current copies of the OpenVMS Frequently Asked Questions (FAQ) document
    are available at:

      http://www.hp.com/go/openvms/faq/


  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Thank you,

Stephen Hoffman
Keeper of the Freeware
HP OpenVMS Engineering
October, 2003
