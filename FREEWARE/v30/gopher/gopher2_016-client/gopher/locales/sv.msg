$
$ This Swedish Translation is made by Johan Svensson <Johan@ecsdg.lu.se>,
$ EkonomiCentrum Software Development Group, Lund University, Sweden.
$ Please send comments and suggestions to him about this file!
$ 1994-06-08
$ ---
$ This file may be used freely as long as the text above follows the work.
$

$
$ Welcome message...
$

$quote "

1 "V�lkommen till  Gophers underbara v�rld!\n\n"
2 "Gopher har begr�nsningar i sitt anv�ndningsomr�de och\n"
3 "har ingen garanti.  Titta v�nligen i filen 'Copyright'\n"
4 "som f�ljer med distributionen.\n\n"

5 "Copyright 1991,92,93,94 by the Regents of the University of Minnesota\n\n"
6 "Internet Gopher Information Client %s.%s patch%d\n"
7 "Tryck RETURN f�r att forts�tta\n"

$ 
$ Errors in .gopherrc
$
8 "Kan inte skriva till gophers inst�llningsfil!!"

$
$ Help for command line 
$

9 "Anv�ndning: %s [-sSbDr] [-T typ] [-p s�kv�g] [-t titel] [datorv�rd port]+\n"
10 "     -s        S�kert l�ge, anv�ndare utan eget konto\n"
11 "     -S        S�kert l�ge, anv�ndare med  eget konto\n"
12 "     -p s�kv�g S�kv�g till f�rsta objekt\n"
13 "     -T typ    F�rsta objektets typ\n"
14 "     -i        S�kkriterium (f�r -T 7)\n"
15 "     -b        Bokm�rken f�rst\n"
16 "     -r        Fj�rranv�ndare\n"
17 "     -D        Avlusningsl�ge\n"

$
$ Pager help..
$

18 "Bl�ddringshj�lp"
19 "u, ^G, v�nster  : �terg� till meny"
20 "mellanslag, ner : G� till n�sta sida"
21 "b, upp          : G� till f�reg�ende sida"
22 "/               : S�k text"
23 "m               : skicka aktuellt dokument med email"
24 "s               : spara aktuellt dokument"
25 "p               : skriv ut aktuellt dokument"
26 "D               : ladda ner aktuellt dokument"

$ 
$ New Application Menu
$ 
27 "Content-Type Namn"
28 "Applikation f�r visning"
29 "Applikation f�r utskrift"

$ 
$ Top Level Options
$ 
30 "Allm�nna inst�llningar"
31 "St�ll in applikationer f�r visning"
32 "St�ll in applikationer f�r utskrift"
33 "Definiera ny Content-Type"

$
$ Telnet connection
$
34 "Varning!!!!!, du h�ller p� att l�mna Internet"
35 "Gopher programmet och koppla upp dig mot en annan datorv�rd."
36 "Om du k�r fast kan du h�lla ner ctrl och"
37 "] tangenterna, och sedan skriva quit"
38 "^ tangenterna, och sedan skriva q."
39 "Kopplar upp %.40s, port %d med %s."
40 "Kopplar upp %.40s med %s."
41 "Anv�nd kontonamnet \"%.40s\" f�r att logga in"
$
$ HTML menu
$
42 "Varning!!!!!, du h�ller p� att l�mna Internet Gopher"
43 "Klienten och koppla upp dig mot en HTML Klient. Anv�nd"
44 "klientens Hj�lp om du inte kan programmets kommandon."
45 "N�r du l�mnar programmet, kommer du tillbaks till"
46 "Internet Gopher Klienten."
$
$ Other html msgs
$
47 "Tyv�rr, denna klient st�djer inte HTML"

$
$ gopher.c
$
48 "Kan tyv�rr inte hitta en administrat�r f�r detta objekt!"

$ 
$ Gripe form msgs
$ 
49 "Tryck p� Enter f�r att skicka ditt meddelande."
50 "Tryck p� Tab tangenten vid slutet av varje rad."
51 "�mne"
52 "Problem"
53 "Kan inte skicka datorpost..."
54 "Skickar datorpost till %s..."
55 "Ord att leta efter"
56 "  Sida: %d/%d"
57 " f�r Hj�lp, "
58 " f�r att Avsluta"
59 " f�r att g� upp en meny"
60 "%s: Erh�ll inget f�r huvudmenyn, kan inte forts�tta\n"
61 "%sm%s f�r datorpost, %sD%s f�r att ladda ner, %ss%s f�r att spara, eller %sp%s f�r utskrift:"
62 ", %sm%s f�r datorpost, %sD%s f�r att ladda ner:"
63 ", <m> f�r datorpost, <D> f�r att ladda ner:"
64 ",\n   <m> f�r datorpost, <D> f�r att ladda ner, <s> f�r att spara, eller <p> f�r utskrift:"
65 "Adress syntaxen felaktig..."
66 "Feta s�kord i den inbyggda bl�ddraren"
67 "Bokm�rken"
68 "Kan inte �ppna gammal fil.."
69 "Kan inte skriva till SYS$SCRATCH!"
70 "Kan inte skriva till /tmp biblioteket!"
71 "Kan inte �ppna beg�rd fil.."
72 "Kan inte koppla upp datorv�rden %.40s, port %d."
73 "V�lj en dokumenttyp"
74 "V�lj en metod att ladda ner"
75 "Koppla upp en ny Gopher Server"
76 "Koppla upp anonymous FTP Server �ver en Gopher gateway"
77 "Kopplar upp..."
78 "Uppkoppling misslyckades: %s."
79 "Uppkoppling mot datorv�rd misslyckades."
80 "Kunde inte skapa '%s'"
81 "Kunde inte exekvera %s\n"
82 "Kunde inte hitta text"
83 "Kunde inte skapa en tempor�r fil!\n"
84 "Det blev fel vid utskrift, ledsen..."
85 "Fel vid anrop av TPU som bl�ddrare!!!"
86 "Avsluta: u"
87 "H�mtar f�lt..."
88 "Filen �verf�rdes inte korrekt"
89 "Gopher Biblioteks Information"
90 "Gopher Fel"
91 "Gopher Fil"
92 "Gopher Hj�lp Fil"
93 "Gopher Objekt Information"
94 "Gopher Inst�llningar"
95 "Gopher �verf�ringsfel"
96 "Hj�lp: ?"
97 "Hem Gopher server: %.59s"
98 "Datorv�rd att fr�ga:"
99 "Datornamn"
100 "Datornamn �r ok�nt."
101 "Jag kan inte f�rst� din terminal typ\n"
102 "Internet Gopher Information Klient v%s.%s.%d"
103 "L�nk Information"
104 "Lokalt Datum och Tid"
105 "Skicka aktuellt dokument med datorpost:"
106 "Skickar fil..."
107 "Skickar fil till %s..."
108 "Att titta p� video �r inte implementerat"
109 "Namn p� detta bokm�rke:"
110 "N�tverksfel"
111 "Nej"
112 "Inga bokm�rken definierade"
113 "Inget kommando f�r att titta p� detta �r definierat!"
114 "Anv�ndare '%s' finns ej"
115 "SidaNer: <MELLANSLAG>"
116 "SidaNer: Mellanslag"
117 "SidaUpp: b"
118 "Port"
119 "Tryck "
120 "Tryck <RETURN> f�r att forts�tta"
121 "Tryck <RETURN> n�r du �r klar\n"
122 "Skriv ut aktuellt dokument"
123 "Problem vid Skrivning"
124 "Verkligen Avsluta (y/n) ?"
125 "Tar emot Bibliotek..."
126 "Tar emot Fil..."
127 "Tar emot HTML sida..."
128 "Tar emot Information..."
129 "Tar emot MIME Fil..."
130 "Tar emot Video..."
131 "Tar emot Ljud..."
132 "Tar emot Bild..."
133 "H�mtar Bibliotek..."
134 "�terg� till Meny: u"
135 "Huvudmeny: "
136 "Spara till fil:"
137 "Spara lista av objekt till fil:"
138 "Sparar Fil..."
139 "S�k i bibliotekstitlar:"
140 "S�kningen misslyckades..."
141 "S�k i text:"
142 "S�ker i text..."
143 "S�ker..."
144 "Avgr�nsare (valfritt)"
145 "Visa datorv�rdens lokala datum och tid"
146 "Storlek    Spr�k         Dokumenttyp\n"
147 "---------- ------------- ----------------------------\n"
148 "Tyv�rr, kan inte visa denna fil"
149 "Tyv�rr, kan inte radera �versta biblioteksniv�n."
150 "Tyv�rr, kan inte ladda ner det d�r!"
151 "Tyv�rr, kan inte g�ra ett bokm�rke av bokm�rken"
152 "Tyv�rr, kan inte spara filer med detta konto"
153 "Tyv�rr, kan inte k�ra med detta konto"
154 "Tyv�rr, kan inte spara objektet till en fil!"
155 "Tyv�rr, kan inte skriva ut detta dokument"
156 "Tyv�rr, det finns ingen metod att skriva ut detta dokument"
157 "Tyv�rr, telnet �r inte tillg�ngligt."
158 "Tyv�rr, denna maskin st�djer inte bilder"
159 "Tyv�rr, denna maskin st�djer inte ljud"
160 "Tyv�rr, tn3270 �r inte tillg�ngligt."
161 "Tyv�rr, du har inte beh�righet att skriva ut filer"
162 "Tyv�rr, du har inte beh�righet att spara filer"
163 "Tyv�rr, du har inte beh�righet att g�ra inst�llningar i S�kert L�ge."
164 "Tyv�rr, du har inte beh�righet att g�ra detta"
165 "Tyv�rr, denna maskin st�djer inte videos"
166 "Tyv�rr, finns inget program f�r denna typ av fil...."
167 "Start av DCL misslyckades!"
168 "Start av standard kommandotolk (shell) misslyckades!"
169 "Startar DCL delprocess.  Logga ut f�r att �terv�nda till Gopher.\n"
170 "Startar din standard kommandotolk (shell). Skriv 'exit' f�r att �terv�nda till Gopher.\n\n"
171 "Starta din \"f�ngst\" nu...\n\n"
172 "Starta nedladdningen nu...\n"
173 "Problem med systemet, kan inte skicka datorpost"
174 "Filnamnet �r:"
175 "Kan inte skapa en n�tverkskanal (socket)."
176 "Unix \"r�rkommando\" (pipe) �r beg�rt.  Kontrollera dina inst�llningar!"
177 "Ok�nt fel."
178 "Anv�nd '/' f�r att definiera en s�kning f�rst..."
179 "Titta p� objekt nummer: "
180 "Ja"
181 "Ditt val"
182 "\nNedladdning klar. %d bytes totalt, %d bytes/s\n"
183 "\nNedladdning kunde inte slutf�ras, ledsen... \n"
184 "\nTryck %sRETURN%s f�r att forts�tta"
185 "\nTryck <RETURN> f�r att forts�tta"
186 "\n\nServer Information\n"
187 "anonymous FTP datorv�rd"
188 "gopher: Erh�ll inget f�r huvudmenyn, kan inte forts�tta\n"
$ 
$ Default language/region gopher+ view to choose
$
218 "Sv_SE"
$
$ Add to post-pager option list in SecureMode and NoShellMode
$
219 ", %sD%s f�r att ladda ner:"
220 ", <D> f�r att ladda ner:"
$
$ For determining the user's desired language
$
$221 "Sorry, only Swedish is available"
222 "Shall we speak Swedish?"
$
$  Multilingual HELP files are now possible
$			If CONF.H defines GOPHERHELP_SECURE, make sure you
$			have a separate file for secure users' help that 
$			doesn't document features they're not allowed to use.
$  For VMS:
$
223 "GopherP_Dir:gopher.hlp"
224 "GopherP_Dir:gopher.hlp_secure"
$
$  For Unix:
$
225 "/usr/local/lib/gopher.hlp"
226 "/usr/local/lib/gopher.hlp_secure"
$
$ Gripe Enhancements
$
227 "Sorry, can't submit gripes in secure mode"
228 "Cannot send mail to null address..."
229 "Regarding the following Gopher item:\n"
230 "To"
231 "<Last Line>"
