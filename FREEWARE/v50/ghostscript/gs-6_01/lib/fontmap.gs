% Copyright (C) 1996, 1999 Aladdin Enterprises.  All rights reserved.
% 
% This file is part of Aladdin Ghostscript.
% 
% Aladdin Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author
% or distributor accepts any responsibility for the consequences of using it,
% or for whether it serves any particular purpose or works at all, unless he
% or she says so in writing.  Refer to the Aladdin Ghostscript Free Public
% License (the "License") for full details.
% 
% Every copy of Aladdin Ghostscript must include a copy of the License,
% normally in a plain ASCII text file named PUBLIC.  The License grants you
% the right to copy, modify and redistribute Aladdin Ghostscript, but only
% under certain conditions described in the License.  Among other things, the
% License requires that the copyright notice and this notice be preserved on
% all copies.

% $Id: Fontmap.GS,v 1.1 2000/03/09 08:40:39 lpd Exp $
% Fontmap - standard font catalog for Ghostscript.

% ----------------------------------------------------------------

% This file is a catalog of fonts known to Ghostscript.  Any font
% that is to be loaded automatically when named must be in this catalog,
% except for fonts that Ghostscript finds automatically in directories
% named in the GS_FONTPATH environment variable.

% Each font has an entry consisting of three items:
%
%	- The name by which the font is known inside Ghostscript
%	(a Ghostscript name preceded by a `/', or a string enclosed
%	in parentheses).  This is used to find the file from which
%	a font of a given name should be loaded.
%
%	- Information depending on whether this is a real font or a
%	font alias:
%
%		- For real fonts, the name of the Ghostscript font
%		file (a Ghostscript string, enclosed in parentheses).
%		The filename should include the extension, which (by
%		convention) is `.gsf'.  `.pfa' and `.pfb' files are
%		also usable as fonts for Ghostscript.
%
%		- For font aliases, the name of the font which should
%		be used when this one is requested, preceded by a
%		`/'.  See the entry for Charter below for an example.
%		Note that an alias name cannot be enclosed in parentheses.
%
%	- At least one space or tab, and a terminating semicolon.

% Because of limitations in the MS-DOS environment, Ghostscript font
% file names must be no more than 8 characters long, must consist only
% of LOWER CASE letters, digits, and underscores, and must start with a
% letter.  Font names, on the other hand, need only obey the syntax of
% names in the Ghostscript language, which is much more liberal.

% The following table is actually a Ghostscript data structure.
% If you add new entries, be sure to copy the punctuation accurately;
% in particular, you must leave at least one space or tab between each
% field in the entry.  Also, please read fonts.doc for important information
% about font names.

% Note that .pfa and .pfb fonts are compatible with Adobe Type Manager
% and other programs that don't include full PostScript interpreters,
% as well as with PostScript interpreters; .gsf fonts are compatible with
% PostScript interpreters, but not with ATM or similar programs.

% 
%
% Fonts contributed by:
%	URW++ Design and Development Incorporated
%	Poppenbuetteler Bogen 29A
%	D-22399 Hamburg
%	Germany
%	tel. +49 (40) 60 60 50
%	fax +49 (40) 60 60 51 11
%	http://www.urwpp.de
% for distribution under the GNU License and Aladdin Free Public License.
% See the notice at the head of this Fontmap file for licensing terms.
% Each of these fonts is individually covered by the license:
% for licensing purposes, they are not "part of" any larger entity.
% The following notice applies to these fonts:
%
%	Copyright URW Software, Copyright 1994 by URW.
%

% Actual fonts

/URWBookmanL-DemiBold	(b018015l.pfb)	;
/URWBookmanL-DemiBoldItal	(b018035l.pfb)	;
/URWBookmanL-Ligh	(b018012l.pfb)	;
/URWBookmanL-LighItal	(b018032l.pfb)	;

/NimbusMonL-Regu	(n022003l.pfb)	;
/NimbusMonL-ReguObli	(n022023l.pfb)	;
/NimbusMonL-Bold	(n022004l.pfb)	;
/NimbusMonL-BoldObli	(n022024l.pfb)	;

/URWGothicL-Book	(a010013l.pfb)	;
/URWGothicL-BookObli	(a010033l.pfb)	;
/URWGothicL-Demi	(a010015l.pfb)	;
/URWGothicL-DemiObli	(a010035l.pfb)	;

/NimbusSanL-Regu	(n019003l.pfb)	;
/NimbusSanL-ReguItal	(n019023l.pfb)	;
/NimbusSanL-Bold	(n019004l.pfb)	;
/NimbusSanL-BoldItal	(n019024l.pfb)	;

/NimbusSanL-ReguCond	(n019043l.pfb)	;
/NimbusSanL-ReguCondItal	(n019063l.pfb)	;
/NimbusSanL-BoldCond	(n019044l.pfb)	;
/NimbusSanL-BoldCondItal	(n019064l.pfb)	;

/URWPalladioL-Roma	(p052003l.pfb)	;
/URWPalladioL-Ital	(p052023l.pfb)	;
/URWPalladioL-Bold	(p052004l.pfb)	;
/URWPalladioL-BoldItal	(p052024l.pfb)	;

/CenturySchL-Roma	(c059013l.pfb)	;
/CenturySchL-Ital	(c059033l.pfb)	;
/CenturySchL-Bold	(c059016l.pfb)	;
/CenturySchL-BoldItal	(c059036l.pfb)	;

/NimbusRomNo9L-Regu	(n021003l.pfb)	;
/NimbusRomNo9L-ReguItal	(n021023l.pfb)	;
/NimbusRomNo9L-Medi	(n021004l.pfb)	;
/NimbusRomNo9L-MediItal	(n021024l.pfb)	;

/StandardSymL	(s050000l.pfb)	;

/URWChanceryL-MediItal	(z003034l.pfb)	;

/Dingbats	(d050000l.pfb)	;

% Aliases

/Bookman-Demi			/URWBookmanL-DemiBold	;
/Bookman-DemiItalic		/URWBookmanL-DemiBoldItal	;
/Bookman-Light			/URWBookmanL-Ligh	;
/Bookman-LightItalic		/URWBookmanL-LighItal	;

/Courier			/NimbusMonL-Regu	;
/Courier-Oblique		/NimbusMonL-ReguObli	;
/Courier-Bold			/NimbusMonL-Bold	;
/Courier-BoldOblique		/NimbusMonL-BoldObli	;

/AvantGarde-Book		/URWGothicL-Book	;
/AvantGarde-BookOblique		/URWGothicL-BookObli	;
/AvantGarde-Demi		/URWGothicL-Demi	;
/AvantGarde-DemiOblique		/URWGothicL-DemiObli	;

/Helvetica			/NimbusSanL-Regu	;
/Helvetica-Oblique		/NimbusSanL-ReguItal	;
/Helvetica-Bold			/NimbusSanL-Bold	;
/Helvetica-BoldOblique		/NimbusSanL-BoldItal	;

/Helvetica-Narrow		/NimbusSanL-ReguCond	;
/Helvetica-Narrow-Oblique	/NimbusSanL-ReguCondItal	;
/Helvetica-Narrow-Bold		/NimbusSanL-BoldCond	;
/Helvetica-Narrow-BoldOblique	/NimbusSanL-BoldCondItal	;

/Palatino-Roman			/URWPalladioL-Roma	;
/Palatino-Italic		/URWPalladioL-Ital	;
/Palatino-Bold			/URWPalladioL-Bold	;
/Palatino-BoldItalic		/URWPalladioL-BoldItal	;

/NewCenturySchlbk-Roman		/CenturySchL-Roma	;
/NewCenturySchlbk-Italic	/CenturySchL-Ital	;
/NewCenturySchlbk-Bold		/CenturySchL-Bold	;
/NewCenturySchlbk-BoldItalic	/CenturySchL-BoldItal	;

/Times-Roman			/NimbusRomNo9L-Regu	;
/Times-Italic			/NimbusRomNo9L-ReguItal	;
/Times-Bold			/NimbusRomNo9L-Medi	;
/Times-BoldItalic		/NimbusRomNo9L-MediItal	;

/Symbol				/StandardSymL	;

/ZapfChancery-MediumItalic	/URWChanceryL-MediItal	;

/ZapfDingbats			/Dingbats	;

% 
%
% Type 1 fonts contributed to the X11R5 distribution.
%

% The following notice accompanied the Charter fonts.
%
% (c) Copyright 1989-1992, Bitstream Inc., Cambridge, MA.
%
% You are hereby granted permission under all Bitstream propriety rights
% to use, copy, modify, sublicense, sell, and redistribute the 4 Bitstream
% Charter (r) Type 1 outline fonts and the 4 Courier Type 1 outline fonts
% for any purpose and without restriction; provided, that this notice is
% left intact on all copies of such fonts and that Bitstream's trademark
% is acknowledged as shown below on all unmodified copies of the 4 Charter
% Type 1 fonts.
%
% BITSTREAM CHARTER is a registered trademark of Bitstream Inc.

/CharterBT-Roman		(bchr.pfa)	;
/CharterBT-Italic		(bchri.pfa)	;
/CharterBT-Bold			(bchb.pfa)	;
/CharterBT-BoldItalic		(bchbi.pfa)	;

% Aliases

/Charter-Roman			/CharterBT-Roman	;
/Charter-Italic			/CharterBT-Italic	;
/Charter-Bold			/CharterBT-Bold		;
/Charter-BoldItalic		/CharterBT-BoldItalic	;

% The following notice accompanied the Utopia font:
%
%   Permission to use, reproduce, display and distribute the listed
%   typefaces is hereby granted, provided that the Adobe Copyright notice
%   appears in all whole and partial copies of the software and that the
%   following trademark symbol and attribution appear in all unmodified
%   copies of the software:
%
%           Copyright (c) 1989 Adobe Systems Incorporated
%           Utopia (R)
%           Utopia is a registered trademark of Adobe Systems Incorporated
%
%   The Adobe typefaces (Type 1 font program, bitmaps and Adobe Font
%   Metric files) donated are:
%
%           Utopia Regular
%           Utopia Italic
%           Utopia Bold
%           Utopia Bold Italic

/Utopia-Regular		(putr.pfa)	;
/Utopia-Italic		(putri.pfa)	;
/Utopia-Bold		(putb.pfa)	;
/Utopia-BoldItalic	(putbi.pfa)	;

% 
%
% Fonts contributed by URW GmbH for distribution under the GNU License.
% The following notice accompanied these fonts:
%
% U004006T URW Grotesk 2031 Bold PostScript Type 1 Font Program
% U003043T URW Antiqua 2051 Regular Condensed PostScript Type 1 Font Program
%
% Copyright (c) 1992 URW GmbH, Hamburg, Germany
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; wihtout even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
% See the GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 U.S.A.
%
% Address:
% URW GmbH
% PC Support
% Harksheider Strasse 102
% 2000 Hamburg 65
% Germany
% Phone: +49 40 60 60 50       (Reception)
% Phone: +49 40 60 60 52 30   (PC Support)
% Fax    : +49 40 60 60 52 52
%

/URWAntiquaT-RegularCondensed	(u003043t.gsf)	;
/URWGroteskT-Bold		(u004006t.gsf)	;

% 
%
% Shareware Kana fonts.  These are subject to the following notice:
%
% These copyrighted fonts were developed by Kevin Hartig. Permission is
% granted to freely distribute them in entirety along with this statement.
% This is shareware. If you decide to use these fonts please contribute
% $10 US to help support further freeware and shareware software development.
% Questions and comments may be sent to:
%
% hartig@fsl.noaa.gov
% khartig@nyx.cs.du.edu
%
% Kevin Hartig
% 1126 Collyer Street
% Longmont, CO 80501 USA
% 
% copyright 1993.

% Hiragana and Katakana fonts.  The character names are inappropriate,
% and the encoding is probably not related to any known standard.

/Calligraphic-Hiragana		(fhirw.gsf)	;
/Calligraphic-Katakana		(fkarw.gsf)	;

% 
%
% Public-domain fonts.  These have no copyright, and are of unknown quality.

% Cyrillic fonts.  The character names are inappropriate,
% and the encoding is probably not related to any known standard.

/Shareware-Cyrillic-Regular	(fcyr.gsf)	;
/Shareware-Cyrillic-Italic	(fcyri.gsf)	;

% Aliases
/Cyrillic			/Cyrillic-Regular	;
/Cyrillic-Regular		/Shareware-Cyrillic-Regular	;
/Cyrillic-Italic		/Shareware-Cyrillic-Italic	;

% 
%
% Fonts converted from Hershey outlines.  These are constructed and
% maintained manually.  These are also in the public domain.
%
% The suggested UniqueID's and filenames are constructed differently for
% these than for the ones above, because of the strange way that the Hershey
% fonts were constructed.  The scheme for these looks like:
%
% 42TTXY0
%
% TT = typeface, X = ``class'', Y = variation
%
% The typeface names and numbers are listed in fonts.mak.
%
% class:
% 0 = normal			= r
% 1 = simplex			= s
% 2 = complex			= c
% 3 = triplex			= t
% 4 = duplex			= d
%
% variation:
% 0 = normal			(omitted)
% 1 = oblique			= o
% 2 = italic			= i
% 3 = bold			= b
% 4 = bold oblique		= bo
% 5 = bold italic		= bi
%

% Fonts created by Thomas Wolff <wolff@inf.fu-berlin.de>, by adding
% accents, accented characters, and various other non-alphabetics
% to the original Hershey fonts.  These are "freeware", not to be sold.

/Hershey-Gothic-English		(hrger.pfa)	;	% 5066533
/Hershey-Gothic-German		(hrgrr.pfa)	;
/Hershey-Gothic-Italian		(hritr.pfa)	; 

/Hershey-Plain-Duplex		(hrpld.pfa)	;
/Hershey-Plain-Duplex-Italic	(hrpldi.pfa)	;
/Hershey-Plain-Triplex		(hrplt.pfa)	;
/Hershey-Plain-Triplex-Italic	(hrplti.pfa)	;

/Hershey-Script-Complex		(hrscc.pfa)	;
/Hershey-Script-Simplex		(hrscs.pfa)	;	% 5066541

% Fonts created algorithmically from the above.

/Hershey-Gothic-English-Bold		(hrgerb.gsf)	;	% 5066542
/Hershey-Gothic-English-Oblique		(hrgero.gsf)	;
/Hershey-Gothic-English-SemiBold	(hrgerd.gsf)	;
/Hershey-Gothic-German-Bold		(hrgrrb.gsf)	;
/Hershey-Gothic-German-Oblique		(hrgrro.gsf)	;
/Hershey-Gothic-Italian-Bold		(hritrb.gsf)	;
/Hershey-Gothic-Italian-Oblique		(hritro.gsf)	;

/Hershey-Plain-Duplex-Bold		(hrpldb.gsf)	;
/Hershey-Plain-Duplex-Bold-Italic	(hrpldbi.gsf)	;
/Hershey-Plain-Triplex-Bold		(hrpltb.gsf)	;
/Hershey-Plain-Triplex-Bold-Italic	(hrpltbi.gsf)	;

/Hershey-Script-Complex-Bold		(hrsccb.gsf)	;
/Hershey-Script-Complex-Oblique		(hrscco.gsf)	;
/Hershey-Script-Simplex-Bold		(hrscsb.gsf)	;
/Hershey-Script-Simplex-Oblique		(hrscso.gsf)	;	% 5066556

% Fonts consisting only of characters from the original Hershey
% distribution.  These are Type 3 fonts.

/Hershey-Greek-Complex			(hrgkc.gsf)	;	% 5066557
/Hershey-Greek-Simplex			(hrgks.gsf)	;

/Hershey-Plain				(hrplr.gsf)	;
/Hershey-Plain-Simplex			(hrpls.gsf)	;	% 5066560

% Fonts created algorithmically from the above.

/Hershey-Plain-Bold			(hrplrb.gsf)	;	% 5066561
/Hershey-Plain-Bold-Oblique		(hrplrbo.gsf)	;
/Hershey-Plain-Oblique			(hrplro.gsf)	;
/Hershey-Plain-Simplex-Bold		(hrplsb.gsf)	;
/Hershey-Plain-Simplex-Bold-Oblique	(hrplsbo.gsf)	;
/Hershey-Plain-Simplex-Oblique		(hrplso.gsf)	;	% 5066566

% This font, and only this font among the Hershey fonts, uses
% the SymbolEncoding.

/Hershey-Symbol				(hrsyr.gsf)	;	% 5066567
