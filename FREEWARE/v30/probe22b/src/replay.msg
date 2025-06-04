.TITLE          PLAYBACK/Probe Playback Utility Messages
.IDENT          'Version 2.2'
.FACILITY       PLAY,23/PREFIX=RPL_

.SEVERITY	FATAL
BADDATA		<unknown record type encountered in playback stream>
BADREC		<playback file is an invalid or corrupted PROBE recording>
CYCOOB		<all requested cycles are beyond actual recorded range>
INCREC		<incomplete playback file record encountered... cannot pursue>
NEWLVL		<playback file recording format (!UB.!UB-!UW) is unknown>/FAO=3
NOACHAN		<error opening analysis output channel>
NODISP		<interactive playback not implemented... please specify /ANALYZE>
NOSAMPLE	<error opening playback file !AZ for input>/FAO=1
NOTAVAX		<currently unable to interpret recording not made on VAX>
OUTOFSEQ	<out of sequence frame data block encountered>

.SEVERITY       ERROR
BADNADR		<specified node value refused - definition ignored>
BADPVAL		<specified protocol value refused - definition ignored>
EXTABLE		<error opening specified table file for input - ignored>

.SEVERITY	WARNING
BIGCYC		<ignoring cycle identifer !UL beyond actual recorded range>/FAO=1
DATALESS	<recordings before V2.2 contain no data... ignoring data class>
DUPCID		<ignoring duplicated cycle identifier !UL from extraction list>/FAO=1
EMPTY		<this recording contains no frame data... ignoring data class>
INVCID		<ignoring cycle identifier !UL outside valid range [1..32767]>/FAO=1
NULNNAM		<specified node name refused - defaulting to null>
NULPNAM		<specified protocol name refused - defaulting to null>
OVRPID		<specified protocol ID refused - defaulting to null>
TBLFLT		<ignoring invalid external table definition at line !UL>/FAO=1

.SEVERITY	INFORMATION
ALLCYC		<no valid cycle subset specified - processing all cycles>
ANAPROG		<performing analysis to output file... please wait>
LOOKATN		<node !XB-!XB-!XB-!XB-!XB-!XB known as !AZ with !UL reference>/FAO=8
LOOKATP		<protocol !XB-!XB known as !AZ (ID !UW) with !UL reference>/FAO=5
UPDNDEF		<redefined node !XB-!XB-!XB-!XB-!XB-!XB from !AZ to !AZ>/FAO=8
UPDPDEF		<redefined protocol !XB-!XB from !AZ (ID !UW) to !AZ (ID !UW)>/FAO=6
