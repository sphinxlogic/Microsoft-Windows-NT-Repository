HTDIG, WEB_TOOLS, WWW indexing and searching system

The ht://Dig system is a complete world wide web indexing and
searching system for a small domain or intranet. This system is
*not* meant to replace the need for powerful internet-wide search
systems like Lycos, Infoseek, Webcrawler and AltaVista. Instead
it is meant to cover the search needs for a single company, campus,
or even a particular sub section of a web site.

As opposed to some WAIS or web server based search engines, ht://Dig
can span several web servers at a site. The type of these different
web servers doesn't matter as long as they understand the HTTP 1.0
protocol.

This software is distributed under the GNU General Public License (GPL) v2.

This OpenVMS port of ht://Dig 3.1.6 will install and run under OpenVMS
Alpha V7.2 or later. The most recent version can be found at
http://www.pdv-systeme.de/users/martinv/htdig/ and by anonymous FTP
to ftp.pdv-systeme.de in the directory vms/ .

To set up the software:

- Unpack the archive:

    $ RUN CDROM:[HTDIG]GNU-AXPVMS-HTDIG-V0301-6R2-1.PCSI-ZIP_AXPEXE

  where CDROM is the drive that contains the OpenVMS Freeware CD-ROM.

- Install the software:

    $ PRODUCT INSTALL HTDIG

  Please see the PRODUCT command's help for available qualifiers.

  The Installation Verification Procedure (IVP) isn't yet perfect,
  so you may try and continue with the installation even if it fails.

- Complete the base installation by executing

    $ @SYS$STARTUP:HTDIG_STARTUP CONFIG
    
  This will create SYS$MANAGER:HTDIG_CONFIG.DAT .
  
- Set up the software by executing

    $ @SYS$STARTUP:HTDIG_STARTUP START

  The "START" parameter is optional, if SYS$MANAGER:HTDIG_CONFIG.DAT
  exists. This command should also be included in the system startup.

- Make HTDIG_ROOT:[CGI-BIN]HTSEARCH.EXE accesible to the web server
  as a CGI program.
  
  The procedure depends on your web server software; e.g. for CSWS:
  $ SET SECURITY /OWNER=APACHE$WWW HTDIG_ROOT:[CGI-BIN]HTSEARCH.EXE

- Make sure that the directories HTDIG_ROOT:[COMMON], [CONF],
  and [DB], all files in these directories, and the shareable image
  HTDIG_ROOT:[BIN]LIBDB_SHR.EXE are accessible to HTSEARCH.EXE
  when run by the account the web server runs under.

  The procedure depends on your web server software; e.g. for CSWS:

    $ SET FILE /OWNER=APACHE$WWW -
    _$ HTDIG_ROOT:[000000]COMMON.DIR, CONF.DIR, DB.DIR, -
    _$ [COMMON]*.*;*, [CONF]*.*;*, [DB]*.*;*, -
    _$ [BIN]LIBDB_SHR.EXE

- Edit HTDIG_ROOT:[CONF]HTDIG.CONF to suit your needs. Complete
  documentation of the product including a description of all
  configuration attributes is installed by default, and can be
  found in HTDIG_ROOT:[DOCUMENTATION].

  Especially modify the line containing the "start_url" attribute.
  Without being changed, the online ht://Dig documentation at
  http://www.htdig.org/ will be indexed!

  Path and file specification attributes (e.g. common_dir, url_list)
  can be written in either VMS or Unix syntax, i.e. DEVICE:[DIR1.DIR2]
  can also be written as /device/dir1/dir2.

  When using variables in file specifications, a directory variable
  must be separated from the filename by a slash, e.g. ${database_dir}/db,
  regardless of the notation of the directory. This syntax is consistent
  with the ht://Dig documentation.

  As the dollar sign is used as a variable designator in attribute
  values, all dollar characters in device and directory names must
  be escaped (i.e. prepended with a backslash). That is, USER$DISK:[DIR]
  must be written as USER\$DISK:[DIR] or /user\$disk/dir.

  In the "external_parsers" and "pdf_parser" attributes' values, the
  external converter or parser programs must be valid DCL commands.

  The default values for attributes that are defined at compile time
  (refered to in the configuration file documentation) are:

    BIN_DIR           /htdig_root/bin
    COMMON_DIR        /htdig_root/common
    CONFIG_DIR        /htdig_root/conf
    DATABASE_DIR      /htdig_root/db
    IMAGE_URL_PREFIX  /htdig_image
    PDF_PARSER        pdftops
    VERSION           3.1.6-2/VMS

  The file specification of the default configuration file is found
  by the logical name HTDIG_DEFAULT_CONFIG_FILE.

- Before you can start searching, you have to create a search
  database. A sample command procedure to do this is provided
  in HTDIG_ROOT:[BIN]RUNDIG.COM.

- HTDIG_ROOT:[SEARCH]SEARCH.HTML contains a sample search form.
  The <FORM ACTION> attribute value must be changed to point to
  HTSEARCH.EXE. Also pay attention to the <INPUT TYPE=HIDDEN>
  fields. The provided SEARCH.HTML uses image files from
  HTDIG_ROOT:[IMAGE], which must then too be accessible to
  the web server, of course.

- Tools to index PDF, Microsoft Word, Excel, and Powerpoint files
  are available for download at the URL mentioned above.

Enjoy,

  Martin Vorlaender
  <martin@radiogaga.harz.de>
  <mv@pdv-systeme.de>

01-Sep-2003
