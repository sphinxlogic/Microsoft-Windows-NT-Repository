TeX95, SOFTWARE, TeX typesetting system

DECUS TeX95 is a new compilation of the popular TeX typesetting system
This compilation is a joined work of Ch.Spieler and R.Gaertner, starting
in 1994. The compilation has a much clearer structure and consists of the
programs:
  TeX 3.14159 (Juni '95), Metafont 2.718 (Juni '95), XDVI , DVIPS 5.58,
  DVITOLN03 4.1c, BibTeX 0.99c, Makeindex, Bibclean, GloTeX, IdxTeX,
  and DECUS SPELL

All programs appear in source form with prebuild objects and images for
both Alpha and VAX. The programs have been compiled on VMS 6.0 systems,
so running them on VMS 5.x may require a recompilation. Beside the pro-
grams the most used macro packages are available as prebuild formats:
  Plain , LaTeX , AMSTeX 2.1 , AMSLaTeX 1.2 , Eplain, TeXinfo, TeXsis,
  and some other formats.
For sites which can not immediatly upgrade, the (now obsolete) formats
for LaTeX 2.09 and AMSLaTeX 1.1 are supplied.

The following fonts for a printer with 300 dpi are available: 
  CM , LaTeX , AMS , DC/EC 1.1 , Concrete Fonts , DVIPS virtual fonts
Besides these precomputed fonts some other fonts are supplied in source
form. Missing bitmaps can be computed with MAKETEXPK, if the Metafont
source is available.

The on-line help has been rewritten and a new guide, VMS_TEX_MGR.GUIDE,
describing the relations/roots of the various components has been added.
For all components (programs/packages) DCL procedures to recreate these
components are supplied. DECUS TeX95 can be installed on a system-wide
or process-only basis.

An installation will create about 7800 files and will need appr. 200MB of 
disk space.

For the future ist is planned to reorganize the collection in the direction
of TUG directory standard, to add more DVI drivers and to increase the
PostScript support.

