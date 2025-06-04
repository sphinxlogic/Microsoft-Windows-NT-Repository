France V 3.3 for DEC's Flight V 3.x              January 1997
====================================

France world for Flight is the new name of Orly world.

Orly International Airport, in the near south of Paris (France) is one of the
great French international airports.  

Orly Airport is composed of 4 runways, the first is an old one and is no longer
used. Orly has two main terminals, South Terminal mainly for interational 
traffic and West Terminal for French domestic traffic.

Another airports are provided in this world with their radio-navigation 
aids (NDB, VOR & ILS):

+ Melun airport, a small airport in the South-East of Orly
+ Le Bourget Airport 
+ Cerny/La-Ferté-Alais airfield
+ Roissy CDG International Airport
+ Bretigny/Orge Airport
+ Toussus-le-Noble Airport
+ Velizy-Villacoublay Airport
+ Toulouse-Blagnac Airport
+ Clermont-Ferrand-Aulnat Airport
+ Biarritz Airport
+ Biscarosse Airfield
+ Cazaux Airport
+ Montalivet Airfield
+ Royan Airport
+ Bordeaux International Airport
+ St Pierre d'Oleron airfield
+ Rochefort Airport
+ La Rochelle Airport
+ Les Sables d'Olonne airfield
+ Ile d'Yeu airfield

And a lot of goodies with manu entry points for vehicules or ships 
(harbours, beaches, islands, bridges, rivers, ...):

+ La Sainte Assise Radio Center
+ Paris and la Seine from Melun to Conflans with Islands
+ Rambouillet forest
+ La Chaine des Puys (old volcanos near Clermmont-Ferrand)
+ Toulouse, Colomiers & Blagnac cities
+ La Garonne from Muret to Merville
+ St Jean de Luz harbour + beach 
+ Biarritz beach
+ Hendaye beach
+ La Rhune
+ L'Adour
+ Les Landes
+ Le Bassin d'Arcachon (with le Banc d'Arguin & l'Ile aux Oiseaux)
+ l'Etang de Lacanau
+ la Pointe de Grave
+ Le Plateau de Cordouan
+ La Gironde
+ La Garonne
+ La Dordogne
+ Oleron Island (with the bridge)
+ Aix & Madame Islands
+ Fort Boyard
+ Ré Island
+ Les Sables d'Olonne harbour & beach
+ St Gilles Croix de Vie beach
+ Yeu Island
+ St Jean de Monts beach
+ Noirmoutier Island

etc ...

COMPILE.COM compiles the world from sources.

INSTALL.COM copies MONDE_FRANCE.WORLD file into SYS$LIBRARY:FLT$FRANCE23.WORLD
file).

This world is now very complex, but you can create sub worlds for better
performances:

COMPILE_CERNY.COM compiles a sub world with only the south of Paris. You
install it with INSTALL_CERNY.COM (world name is FLT$CERNY.WORLD).

COMPILE_BASQUE.COM compiles a sub world with Hendaye, St jean de Luz, Biarritz
etc ... You install it with INSTALL_BASQUE.COM  (world name is
FLT$EUSKADI.WORLD).

COMPILE_PARIS.COM compiles a sub world with only Paris and objects around Paris
(all airports). Cerny world is a subset of Paris world. You install Paris
world with INSTALL_PARIS.COM (world name is ... FLT$PARIS.WORLD)

COMPILE_SW.COM compiles a sub world with only the South West part of France
world (from Spain to Gironde). You install it with INSTALL_SW.COM (world name
is FLT$FRANCE_SW.WORLD).

COMPILE_CHARENTE.COM compiles a sub world with Gironde + Charentes.
You install it via INSTALL_CHARENTE.COM procedure.

COMPILE_VENDEE.COM compiles a sub world with Vendé + Charentes.
You install it via INSTALL_VENDEE.COM procedure.

CHANGELOG. file list the changes.

The CENA is near Orly, and you can see it in this world !! 

A lot of entry points are available !!

A new FLT$KEYMAP.DAT, providing a tenth viewpoint bound on PF2-KP0 is 
provided. You need to install it under sys$library to enjoy the Puy de Dome
viewpoint. Other viewpoints are defined into this new keymap file:

PF2-PF1 (View 11), PF2-PF2 (view 12), PF2-PF3 (view 13) and PF2-PF4 (view 14)

NDB, VOR & ILS frequencies:
---------------------------
Orly:
----
Orly VOR  "OL"  : 111.20 Mhz
ILS 02L   "OLN" : 110.30 Mhz
ILS 07    "ORE" : 108.50 Mhz
ILS 26    "OLW" : 109.50 Mhz
ILS 25    "OLO" : 110.90 Mhz
ILS 08    "OLE" : 108.10 Mhz 

Melun:
-----
Melun VOR "MEL" : 109.80 Mhz
Bray VOR  "BRY" : 114.10 Mhz
ILS 29    "MV"  : 110.50 Mhz
NDB Melun "MV"  : 434.00 Khz  
NBD Bray  "BRY" : 277.00 Khz  

Le Bourget:
----------
Le Bourget VOR "BT"  : 108.80 Mhz
ILS 07 "LBE"         : 109.90 Mhz
ILS 27 "BGE"         : 110.50 Mhz
NDB    "BGW"         : 334.00 Khz

Roissy CDG:
-----------
ILS 09 "CGE"        : 110.10 Mhz
ILS 27 "CGW"        : 110.70 Mhz
ILS 10 "GLE"        : 108.70 Mhz
ILS 28 "GAU"        : 109.10 Mhz
VOR    "CGN"        : 115.3  Mhz
VOR    "PGS"        : 117.0  Mhz
NDB    "RSO"        : 364.00 Khz
NDB    "CGO"        : 343.00 Khz

Bretigny:
--------
ILS 05 "BY"         : 108.9 Mhz
NDB    "EM"         : 295.00 Khz  (Etampes)

Toussus-le-Noble:
----------------
VOR  "TSU" Toussus    : 110.8 Mhz
VOR  "RBT" Rambouillet: 114.7 Mhz
ILS 26R "TNO"         : 109.3 Mhz

Velizy-Villacoublay:
-------------------
ILS   "VV"        : 111.7  Mhz
NDB   "HOL"       : 315.00 Khz
NDB   "TA"        : 286.00 Khz
NDB   "TH"        : 302.00 Khz  

Divers Nord de la France:
------------------------

VOR Pithiviers "PTV" : 116.5 Mhz
VOR Chartres   "CHW" : 115.2 Mhz
VOR Montdidier "MTD" : 113.6 Mhz
VOR Evreux     "EVX" : 112.4 Mhz
VOR Rouen      "ROU" : 116.8 Mhz
VOR Reims      "REM" : 112.3 Mhz

Toulouse-Blagnac:
----------------
ILS 15R  "TBS"    : 110.7 Mhz
ILS 15L  "TG"     : 108.9 Mhz
ILS 33L  "TBN"    : 109.3 Mhz
VOR      "TOU"    : 117.7 Mhz
VOR      "AGN"    : 114.8 Mhz  (Agen)
VOR      "GAI"    : 115.8 Mhz  (Gaillac)
NDB      "TS"     : 370.0 Khz
NDB      "TOL"    : 415.0 Khz 
NDB      "TLE"    : 564.0 Khz
NDB      "TN"     : 378.0 Khz
NDB      "MUT"    : 361.0 Khz (Muret)

Biarritz
--------

VOR      "BTZ"    : 114.1 Mhz
ILS      "BZ"     : 111.3 Mhz

Bordeaux
--------

VOR      "BDX"    : 114.1 Mhz
ILS      "BD"     : 110.3 Mhz

Charente
--------

NDB      "ROC"    : 396   Khz  (Rochefort)
NDB      "RL"     : 322   Khz  (La Rochelle)
ILS      "ROC"    : 110.5 Mhz  (Rochefort)
ILS      "RL"     : 109.5 Mhz  (La Rochelle)

Enjoy flying in France !!

--
Patrick Moreau - CENA/Athis-Mons - FRANCE
pmoreau@cena.dgac.fr
