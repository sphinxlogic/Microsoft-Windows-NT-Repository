$
$ Translation courtesy of Fernando Garcia-Loygorri <fernando@ender.upc.es>
$

$
$ Welcome message...
$

$quote "

1 "�Bienvenido al maravilloso mundo de Gopher!\n\n"
2 "Hay limitaciones en el uso de Gopher y no est�\n"
3 "garantizado. Por favor, lea el fichero 'Copyright' inclu�do\n"
4 "en el paquete.\n\n"

5 "Copyright 1991,92,93,94 by the Regents of the Universidad of Minnesota\n\n"
6 "Cliente de Informaci�n de Gopher de Internet %s.%s parche%d\n"
7 "Pulse INTRO para continuar\n"

$ 
$ Errors in .gopherrc
$
8 "�No puedo escribir el fichero de configuraci�n de Gopher!"

$
$ Help for command line 
$

9 "Usage: %s [-sSbDr] [-T type] [-p path] [-t title] [hostname port]+\n"
10 "     -s      Modo seguro, usuarios sin cuenta propia\n"
11 "     -S      Modo seguro, usuarios con cuenta propia\n"
12 "     -p senda Especificaci�n de la senda al elemento inicial\n"
13 "     -T tipo Tipo de elemento inicial\n"
14 "     -i      Argumento de b�squeda (para -T 7)\n"
15 "     -b      Primero los recordatorios\n"
16 "     -r      Usuario remoto\n"
17 "     -D      Modo de depuraci�n\n"

$
$ Pager help..
$

18 "Ayuda sobre el compaginador"
19 "u, ^G, izquierda : Volver al men�"
20 "espacio, abajo   : Ir a la siguiente p�gina"
21 "b, arriba        : Ir a la p�gina anterior"
22 "/                : Buscar en el texto"
23 "m                : Enviar por correo el documento actual"
24 "s                : Guardar el documento actual"
25 "p                : Imprimir el documento actual"
26 "D                : Descargar el documento actual"

$ 
$ New Application Menu
$ 
27 "Nombre del tipo de contenido"
28 "Aplicaci�n de presentaci�n"
29 "Aplicaci�n de impresi�n"

$ 
$ Top Level Options
$ 
30 "Opciones generales"
31 "Configuraci�n de las aplicaciones de presentaci�n"
32 "Configuraci�n de las aplicaciones de impresi�n"
33 "Definici�n de un tipo de contenido nuevo"

$
$ Telnet connection
$
34 "���Aviso!!!, va a abandonar el programa Gopher"
35 "de Internet y a conectarse a otro ordenador. Si"
36 "se queda atascado, pulse la tecla de Control junto" 
37 "con la tecla ] y despu�s teclee quit"
38 "tecla ^ y despu�s teclee q."
39 "Conectando a %.40s, puerta %d usando %s."
40 "Conectando a %.40s usando %s."
41 "Use el nombre de cuenta \"%.40s\" para entrar"
$
$ HTML menu
$
42 "���Aviso!!!, va a abandonar el cliente Gopher de"
43 "Internet y a conectarse a un Lector de HTML. Use la Ayuda"
44 "del Lector si no est� familiarizado con sus �rdenes."
45 "Al salir del Lector, ser� devuelto al cliente Gopher"
46 "de Internet."
$
$ Other html msgs
$
47 "Lo sienta, este cliente no da soporte a HTML"

$
$ gopher.c
$
48 "Lo siento, no puedo encontrar un administrador para este elemento"

$ 
$ Gripe form msgs
$ 
49 "Pulse la tecla Intro para enviar su mensaje."
50 "Pulse la tecla de tabulaci�n al final de cada l�nea que escriba."
51 "Sujecto"
52 "Problema"
53 "Cannot send mail..."
54 "Enviando una queja a %s..."
55 "Palabras a buscar"
56 "  P�gina: %d/%d"
57 " para la Ayuda, "
58 " para Salir"
59 " para subir un men�" 
60 "%s: No se ha recibido el men� principal, no es posible seguir\n"
61 "%sm%s para enviar por correo, %sD%s para descargar, %ss%s para guardar o %sp%s para imprimir:"
62 ", %sm%s para enviar por correo, %sD%s para descargar:"
63 ", <m> para enviar por correo, <D> para descargar:"
64 "\n <m> para enviar por correo, <D> para descargar, <s> para guardar o <p> para imprimir:"
65 "La sintaxis de la direcci�n ha sido rechazada..."
66 "Palabras buscadas en negrita en el compaginador interno"
67 "Recordatorios"
68 "No puedo abrir el antiguo fichero.."
69 "�No puedo escribir en SYS$SCRATCH!"
70 "�No puedo escribir en el directorio /tmp!"
71 "No puedo abrir el fichero solicitado.."
72 "No puedo conectar al ordenador %.40s, puerta %d."
73 "Elija un tipo de documento"
74 "Elija un m�todo de descarga"
75 "Conexi�n a otro servidor de Gopher"
76 "Conexi�n a un servidor de FTP an�nimo v�a la pasarela Gopher"
77 "Conectando..."
78 "Fallo en la conexi�n: %s."
79 "Fallo en la conexi�n al ordenador remoto."
80 "No he podido crear '%s'"
81 "No he podido ejecutar %s\n"
82 "No he podido encontrar el texto"
83 "�No he podido crear un fichero temporal!\n" 
84 "Lo siento, ha habido un problema al imprimir..."
85 "�Error al invocar una TPU como compaginador!"
86 "Salir: u"
87 "Extrayendo campos..." 
88 "El fichero no fue transferido con �xito"
89 "Informaci�n sobre el directorio de Gopher"
90 "Error de Gopher"
91 "Fichero de Gopher"
92 "Fichero de ayuda de Gopher"
93 "Informaci�n sobre elemento de Gopher"
94 "Opciones de Gopher"
95 "Error de transmisi�n de Gopher"
96 "Ayuda: ?"
97 "Servidor de Gopher inicial: %.59s"
98 "Ordenador al que interrogar:"
99 "Nombre de ordenador"
100 "Nombre de ordenador desconocido."
101 "No puedo establecer el tipo de su terminal\n"
102 "Cliente de Informaci�n de Gopher de Internet v%s.%s.%d"
103 "Informaci�n sobre enlaces"
104 "Fecha y hora locales"
105 "Enviar por correo el documento actual a:"
106 "Enviando el fichero..."
107 "Enviando el fichero a %s..."
108 "A�n no se ha implementado el pase de pel�culas"
109 "Nombre de este recordatorio:"
110 "Error en la red"
111 "No"
112 "No hay recordatorios definidos"
113 "�No hay una orden de presentaci�n asignada a esta vista!"
114 "El usuario '%s' no existe"
115 "Siguiente p�gina: <ESPACIO>"
116 "Siguiente p�gina: Espacio"
117 "P�gina anterior: b"
118 "Puerta"
119 "Pulse "
120 "Pulse <INTRO> para continuar"
121 "Pulse <INTRO> cuando est� listo\n"
122 "Imprimir el documento actual"
123 "Hay problemas escribiendo"
124 "�Quiere realmente salir (y=s�/n=no) ?"
125 "Recibiendo directorio..."
126 "Recibiendo fichero..."
127 "Recibiendo p�gina de HTML..."
128 "Recibiendo informaci�n..."
129 "Recibiendo fichero MIME..."
130 "Recibiendo pel�cula..."
131 "Recibiendo sonido..."
132 "Receiving Image..."
133 "Extrayendo directorio..."
134 "Volver al Men�: u"
135 "Men� ra�z: "
136 "Guardar en fichero:"
137 "Guardar lista de elementos en fichero:"
138 "Guardando fichero..."
139 "B�squeda en t�tulos de directorios de:"
140 "B�squeda sin �xito..."
141 "B�squeda en texto de:"
142 "Buscando texto..."
143 "Buscando..."
144 "Seleccionador (Opcional)"
145 "Mostrar la fecha y hora locales en el ordenador remoto"
146 "Tama�o     Lenguaje      Tipo de Documento\n"
147 "---------- ------------- ----------------------------\n"
148 "Lo siento, no puedo mostrar este fichero"
149 "Lo siento, no puedo borrar el directorio de m�s alto nivel."
150 "�Lo siento, no puedo descargar eso!"
151 "Lo siento, no puedo hacer un recordatorio de recordatorios"
152 "Lo siento, con esta cuenta no es posible guardar ficheros"
153 "Lo siento, con esta cuenta no es posible iniciar otro proceso"
154 "�Lo siento, no puedo guardar ese elemento en un fichero!"
155 "Lo siento, no puedo imprimir este documento"
156 "Lo siento, no hay mecanismo para imprimir este documneto"
157 "Lo siento, telnet no est� disponible."
158 "Lo siento, esta m�quina no da soporte a im�genes"
159 "Lo siento, esta m�quina no da soporte a sonidos"
160 "Lo siento, tn3270 no est� disponible."
161 "Lo siento, Ud. no est� autorizado a imprimir ficheros"
162 "Lo siento, Ud. no est� autorizado a guardar ficheros"
163 "Sorry, you are not allowed to set options in secure mode."
164 "Lo siento, Ud. no est� autorizado a hacer esto"
165 "Sorry, this machine doesn't support movies"
166 "Sorry, no viewer for the file...."
167 "�Ha fallado el inicio del subproceso DCL!"
168 "�Ha fallado el inicio del int�rprete de �rdenes habitual!"
169 "Iniciando el subproceso DCL. Salga para volver a Gopher.\n"
170 "Iniciando su int�rprete de �rdenes habitual. Teclee 'exit' para volver\na Gopher.\n\n"
171 "Comience ahora su captura de datos...\n\n"
172 "Comience ahora su descarga de datos...\n"
173 "Problema en el sistema, no es posible enviar correo"
174 "El nombre del fichero es"
175 "No es posible reservar un z�calo."
176 "Se ha solicitado un empalme de Unix. �Compruebe su configuraci�n!"
177 "Error desconocido."
178 "Use '/' para primero definir una b�squeda..."
179 "Ver elemento n�mero:"
180 "S�"
181 "Elija"
182 "\nCompletada la descarga. %d bytes en total, %d bytes/sec.\n"
183 "\nLo siento, no ha sido posible completar la descarga... \n"
184 "\nPulse %sINTRO%s para continuar"
185 "\nPulse <INTRO> para continuar"
186 "\n\nInformaci�n del servidor\n"
187 "Ordenador para FTP an�nimo"
188 "gopher: No se ha recibido el men� principal, no es posible seguir\n"
189 "The 'd'elete command is only for bookmarks"

$ 
$ Default language/region gopher+ view to choose
$
218 "Es_ES"
$
$ Add to post-pager option list in SecureMode and NoShellMode
$
219 ", %sD%s para descargar:"
220 ", <D> para descargar:"
$
$ For determining the user's desired language
$
$221 "Sorry, only Spanish is available"
222 "�Quiere los mensajes en espa�ol?"
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
