{1:}{10:}{:10}program PATGEN;label{7:}22,9999;{:7}const{18:}
triesize=55000;triecsize=26000;maxops=4080;maxval=7;maxdot=15;maxlen=50;
{:18}type{11:}textchar=char;asciicode=0..127;{:11}{19:}qindex=0..128;
valtype=0..maxval;dottype=0..maxdot;optype=0..maxops;
wordindex=0..maxlen;triepointer=0..triesize;triecpointer=0..triecsize;
trienode=record ch:asciicode;lh,rh:triepointer end;
opword=packed record dot:dottype;val:valtype;op:optype end;{:19}var{2:}
patstart,patfinish:dottype;hyphstart,hyphfinish:valtype;
goodwt,badwt,thresh:integer;{:2}{4:}
dictionary,patterns,pattmp,outfile:file of textchar;
fname:packed array[1..FILENAMESIZE]of char;{:4}{12:}
xord:array[textchar]of asciicode;xchr:array[asciicode]of textchar;{:12}
{20:}triec:packed array[triepointer]of asciicode;
triel,trier:packed array[triepointer]of triepointer;
trietaken:packed array[triepointer]of boolean;
triecc:packed array[triecpointer]of asciicode;
triecl,triecr:packed array[triecpointer]of triecpointer;
triectaken:packed array[triecpointer]of boolean;
ops:array[optype]of opword;{:20}{21:}trieq:array[1..128]of trienode;
qmax:qindex;qmaxthresh:qindex;{:21}{23:}triemax:triepointer;
triebmax:triepointer;triecount:triepointer;opcount:optype;{:23}{30:}
pat:array[dottype]of asciicode;patlen:dottype;{:30}{33:}
triecmax,triecbmax,trieccount:triecpointer;trieckmax:triecpointer;
patcount:integer;{:33}{43:}goodpatcount,badpatcount:integer;
goodcount,badcount,misscount:integer;levelpatterncount:integer;
moretocome:boolean;{:43}{51:}word:array[wordindex]of asciicode;
dots:array[wordindex]of asciicode;dotw:array[wordindex]of integer;
hval:array[wordindex]of valtype;nomore:array[wordindex]of boolean;
wlen:wordindex;wordwt:integer;wtchg:boolean;buf:array[1..80]of textchar;
bufptr:1..80;{:51}{59:}gooddot,baddot:asciicode;{:59}{61:}
procesp,hyphp:boolean;patdot:dottype;hyphlevel:valtype;
filnam:packed array[1..8]of char;{:61}{70:}dot1,dot2:dottype;
levelnomore:array[dottype]of boolean;{:70}procedure initialize;var{13:}
i:textchar;j:asciicode;{:13}begin{73:}
if argc<4 then begin begin writeln(stdout,
'Usage: patgen <dictionary file> <pattern file> <output file>');
uexit(1);end;end;argv(3,fname);rewrite(outfile,fname);{:73}{14:}
for i:=chr(0)to chr(127)do xord[i]:=127;xord['*']:=42;xord['-']:=45;
xord['.']:=46;xord['0']:=48;xord['1']:=49;xord['2']:=50;xord['3']:=51;
xord['4']:=52;xord['5']:=53;xord['6']:=54;xord['7']:=55;xord['8']:=56;
xord['9']:=57;xord['A']:=65;xord['B']:=66;xord['C']:=67;xord['D']:=68;
xord['E']:=69;xord['F']:=70;xord['G']:=71;xord['H']:=72;xord['I']:=73;
xord['J']:=74;xord['K']:=75;xord['L']:=76;xord['M']:=77;xord['N']:=78;
xord['O']:=79;xord['P']:=80;xord['Q']:=81;xord['R']:=82;xord['S']:=83;
xord['T']:=84;xord['U']:=85;xord['V']:=86;xord['W']:=87;xord['X']:=88;
xord['Y']:=89;xord['Z']:=90;xord['a']:=65;xord['b']:=66;xord['c']:=67;
xord['d']:=68;xord['e']:=69;xord['f']:=70;xord['g']:=71;xord['h']:=72;
xord['i']:=73;xord['j']:=74;xord['k']:=75;xord['l']:=76;xord['m']:=77;
xord['n']:=78;xord['o']:=79;xord['p']:=80;xord['q']:=81;xord['r']:=82;
xord['s']:=83;xord['t']:=84;xord['u']:=85;xord['v']:=86;xord['w']:=87;
xord['x']:=88;xord['y']:=89;xord['z']:=90;{:14}{15:}
for j:=0 to 127 do xchr[j]:=' ';xchr[42]:='*';xchr[45]:='-';
xchr[46]:='.';xchr[48]:='0';xchr[49]:='1';xchr[50]:='2';xchr[51]:='3';
xchr[52]:='4';xchr[53]:='5';xchr[54]:='6';xchr[55]:='7';xchr[56]:='8';
xchr[57]:='9';xchr[65]:='a';xchr[66]:='b';xchr[67]:='c';xchr[68]:='d';
xchr[69]:='e';xchr[70]:='f';xchr[71]:='g';xchr[72]:='h';xchr[73]:='i';
xchr[74]:='j';xchr[75]:='k';xchr[76]:='l';xchr[77]:='m';xchr[78]:='n';
xchr[79]:='o';xchr[80]:='p';xchr[81]:='q';xchr[82]:='r';xchr[83]:='s';
xchr[84]:='t';xchr[85]:='u';xchr[86]:='v';xchr[87]:='w';xchr[88]:='x';
xchr[89]:='y';xchr[90]:='z';{:15}end;{:1}{24:}procedure initpatterntrie;
var c:asciicode;h:optype;begin for c:=0 to 127 do begin triec[1+c]:=c;
triel[1+c]:=0;trier[1+c]:=0;end;trietaken[1]:=true;triebmax:=1;
triemax:=128;triecount:=128;qmaxthresh:=5;triel[0]:=triemax+1;
trier[triemax+1]:=0;for h:=1 to maxops do ops[h].val:=0;opcount:=0;end;
{:24}{25:}function firstfit:triepointer;label 40,41;var s,t:triepointer;
q:qindex;begin{26:}if qmax>qmaxthresh then t:=trier[triemax+1]else t:=0;
while true do begin t:=triel[t];s:=t-trieq[1].ch;{27:}
if s+128>triesize then begin writeln(stdout,'Pattern trie too full!');
uexit(1);end;;while triebmax<s do begin triebmax:=triebmax+1;
trietaken[triebmax]:=false;triec[triebmax+127]:=0;
triel[triebmax+127]:=triebmax+128;trier[triebmax+128]:=triebmax+127;
end{:27};if trietaken[s]then goto 41;
for q:=qmax downto 2 do if triec[s+trieq[q].ch]>0 then goto 41;goto 40;
41:end;40:{:26};for q:=1 to qmax do begin t:=s+trieq[q].ch;
triel[trier[t]]:=triel[t];trier[triel[t]]:=trier[t];
triec[t]:=trieq[q].ch;triel[t]:=trieq[q].rh;trier[t]:=trieq[q].lh;
if t>triemax then triemax:=t;end;trietaken[s]:=true;firstfit:=s end;
{:25}{28:}procedure unpack(s:triepointer);var c:asciicode;t:triepointer;
begin qmax:=0;for c:=64 to 90 do begin t:=s+c;
if triec[t]=c then begin qmax:=qmax+1;trieq[qmax].ch:=c;
trieq[qmax].rh:=triel[t];trieq[qmax].lh:=trier[t];trier[triel[0]]:=t;
triel[t]:=triel[0];triel[0]:=t;trier[t]:=0;triec[t]:=0;end;end;
trietaken[s]:=false;end;{:28}{29:}function newtrieop(v:valtype;
d:dottype;n:optype):optype;label 10;var h:optype;
begin h:=((n+313*d+361*v)mod maxops)+1;
while true do begin if ops[h].val=0 then begin opcount:=opcount+1;
if opcount=maxops then begin writeln(stdout,'Too many outputs!');
uexit(1);end;;ops[h].val:=v;ops[h].dot:=d;ops[h].op:=n;newtrieop:=h;
goto 10;end;
if(ops[h].val=v)and(ops[h].dot=d)and(ops[h].op=n)then begin newtrieop:=h
;goto 10;end;if h>1 then h:=h-1 else h:=maxops;end;10:end;{:29}{31:}
procedure insertpattern(val:valtype;dot:dottype);var i:dottype;
s,t:triepointer;begin i:=1;s:=1+pat[i];t:=triel[s];
while(t>0)and(i<patlen)do begin i:=i+1;t:=t+pat[i];
if triec[t]<>pat[i]then{32:}
begin if triec[t]=0 then begin triel[trier[t]]:=triel[t];
trier[triel[t]]:=trier[t];triec[t]:=pat[i];triel[t]:=0;trier[t]:=0;
if t>triemax then triemax:=t;end else begin t:=t-pat[i];unpack(t);
qmax:=qmax+1;trieq[qmax].ch:=pat[i];trieq[qmax].rh:=0;trieq[qmax].lh:=0;
t:=firstfit;triel[s]:=t;t:=t+pat[i];end;triecount:=triecount+1;end;{:32}
;s:=t;t:=triel[s];end;trieq[1].rh:=0;trieq[1].lh:=0;qmax:=1;
while i<patlen do begin i:=i+1;trieq[1].ch:=pat[i];t:=firstfit;
triel[s]:=t;s:=t+pat[i];triecount:=triecount+1;end;
trier[s]:=newtrieop(val,dot,trier[s]);end;{:31}{34:}
procedure initcounttrie;var c:asciicode;
begin for c:=0 to 127 do begin triecc[1+c]:=c;triecl[1+c]:=0;
triecr[1+c]:=0;end;triectaken[1]:=true;triecbmax:=1;triecmax:=128;
trieccount:=128;trieckmax:=4096;triecl[0]:=triecmax+1;
triecr[triecmax+1]:=0;patcount:=0;end;{:34}{35:}
function firstcfit:triecpointer;label 40,41;var a,b:triecpointer;
q:qindex;begin{36:}if qmax>3 then a:=triecr[triecmax+1]else a:=0;
while true do begin a:=triecl[a];b:=a-trieq[1].ch;{37:}
if b+128>trieckmax then begin if trieckmax=triecsize then begin writeln(
stdout,'Count trie too full!');uexit(1);end;;
write(stdout,trieckmax div 1024:1,'K ');
if trieckmax+4096>triecsize then trieckmax:=triecsize else trieckmax:=
trieckmax+4096;end;while triecbmax<b do begin triecbmax:=triecbmax+1;
triectaken[triecbmax]:=false;triecc[triecbmax+127]:=0;
triecl[triecbmax+127]:=triecbmax+128;
triecr[triecbmax+128]:=triecbmax+127;end{:37};
if triectaken[b]then goto 41;
for q:=qmax downto 2 do if triecc[b+trieq[q].ch]>0 then goto 41;goto 40;
41:end;40:{:36};for q:=1 to qmax do begin a:=b+trieq[q].ch;
triecl[triecr[a]]:=triecl[a];triecr[triecl[a]]:=triecr[a];
triecc[a]:=trieq[q].ch;triecl[a]:=trieq[q].rh;triecr[a]:=trieq[q].lh;
if a>triecmax then triecmax:=a;end;triectaken[b]:=true;firstcfit:=b end;
{:35}{38:}procedure unpackc(b:triecpointer);var c:asciicode;
a:triecpointer;begin qmax:=0;for c:=64 to 90 do begin a:=b+c;
if triecc[a]=c then begin qmax:=qmax+1;trieq[qmax].ch:=c;
trieq[qmax].rh:=triecl[a];trieq[qmax].lh:=triecr[a];
triecr[triecl[0]]:=a;triecl[a]:=triecl[0];triecl[0]:=a;triecr[a]:=0;
triecc[a]:=0;end;end;triectaken[b]:=false;end;{:38}{39:}
function insertcpat(fpos:wordindex):triecpointer;var spos:wordindex;
a,b:triecpointer;begin spos:=fpos-patlen;spos:=spos+1;b:=1+word[spos];
a:=triecl[b];while(a>0)and(spos<fpos)do begin spos:=spos+1;
a:=a+word[spos];if triecc[a]<>word[spos]then{40:}
begin if triecc[a]=0 then begin triecl[triecr[a]]:=triecl[a];
triecr[triecl[a]]:=triecr[a];triecc[a]:=word[spos];triecl[a]:=0;
triecr[a]:=0;if a>triecmax then triecmax:=a;
end else begin a:=a-word[spos];unpackc(a);qmax:=qmax+1;
trieq[qmax].ch:=word[spos];trieq[qmax].rh:=0;trieq[qmax].lh:=0;
a:=firstcfit;triecl[b]:=a;a:=a+word[spos];end;trieccount:=trieccount+1;
end;{:40};b:=a;a:=triecl[a];end;trieq[1].rh:=0;trieq[1].lh:=0;qmax:=1;
while spos<fpos do begin spos:=spos+1;trieq[1].ch:=word[spos];
a:=firstcfit;triecl[b]:=a;b:=a+word[spos];trieccount:=trieccount+1;end;
insertcpat:=b;patcount:=patcount+1;end;{:39}{41:}
procedure traversecounttrie(b:triecpointer;i:dottype);var c:asciicode;
a:triecpointer;begin i:=i+1;for c:=64 to 90 do begin a:=b+c;
if triecc[a]=c then begin pat[i]:=c;
if i<patlen then traversecounttrie(triecl[a],i)else{42:}
if goodwt*triecl[a]<thresh then begin insertpattern(maxval,patdot);
badpatcount:=badpatcount+1 end else if goodwt*triecl[a]-badwt*triecr[a]
>=thresh then begin insertpattern(hyphlevel,patdot);
goodpatcount:=goodpatcount+1;goodcount:=goodcount+triecl[a];
badcount:=badcount+triecr[a];end else moretocome:=true{:42};end;end;end;
{:41}{44:}procedure collectcounttrie;begin goodpatcount:=0;
badpatcount:=0;goodcount:=0;badcount:=0;moretocome:=false;
traversecounttrie(1,0);
write(stdout,goodpatcount:1,' good and ',badpatcount:1,
' bad patterns added');
levelpatterncount:=levelpatterncount+goodpatcount;
if moretocome then writeln(stdout,' (more to come)')else writeln(stdout,
' ');write(stdout,'finding ',goodcount:1,' good and ',badcount:1,
' bad hyphens');
if goodpatcount>0 then begin write(stdout,', efficiency = ');
printreal(goodcount/(goodpatcount+badcount/(thresh/goodwt)),1,2);
writeln(stdout);end else writeln(stdout,' ');
writeln(stdout,'pattern trie has ',triecount:1,' nodes, ','trie_max = ',
triemax:1,', ',opcount:1,' outputs');end;{:44}{45:}
function deletepatterns(s:triepointer):triepointer;var c:asciicode;
t:triepointer;allfreed:boolean;h,n:optype;begin allfreed:=true;
for c:=64 to 90 do begin t:=s+c;if triec[t]=c then begin{46:}begin h:=0;
ops[0].op:=trier[t];n:=ops[0].op;
while n>0 do begin if ops[n].val=maxval then ops[h].op:=ops[n].op else h
:=n;n:=ops[h].op;end;trier[t]:=ops[0].op;end{:46};
if triel[t]>0 then triel[t]:=deletepatterns(triel[t]);
if(triel[t]>0)or(trier[t]>0)or((t<=128)and(t>=1))then allfreed:=false
else{47:}begin triel[trier[triemax+1]]:=t;trier[t]:=trier[triemax+1];
triel[t]:=triemax+1;trier[triemax+1]:=t;triec[t]:=0;
triecount:=triecount-1;end{:47};end;end;
if allfreed then begin trietaken[s]:=false;s:=0;end;deletepatterns:=s;
end;{:45}{48:}procedure deletebadpatterns;var oldopcount:optype;
oldtriecount:triepointer;t:triepointer;h:optype;
begin oldopcount:=opcount;oldtriecount:=triecount;t:=deletepatterns(1);
for h:=1 to maxops do if ops[h].val=maxval then begin ops[h].val:=0;
opcount:=opcount-1;end;
writeln(stdout,oldtriecount-triecount:1,' nodes and ',oldopcount-opcount
:1,' outputs deleted');qmaxthresh:=7;end;{:48}{49:}
procedure outputpatterns(s:triepointer;patlen:dottype);var c:asciicode;
t:triepointer;h:optype;d:dottype;begin patlen:=patlen+1;
for c:=64 to 90 do begin t:=s+c;if triec[t]=c then begin pat[patlen]:=c;
h:=trier[t];if h>0 then{50:}begin for d:=0 to patlen do hval[d]:=0;
repeat d:=ops[h].dot;if hval[d]<ops[h].val then hval[d]:=ops[h].val;
h:=ops[h].op;until h=0;if hval[0]>0 then write(outfile,hval[0]:1);
for d:=1 to patlen do begin if pat[d]=64 then write(outfile,'.')else
write(outfile,xchr[pat[d]]);if hval[d]>0 then write(outfile,hval[d]:1);
end;writeln(outfile);end{:50};
if triel[t]>0 then outputpatterns(triel[t],patlen);end;end;end;{:49}
{52:}procedure readword;var c:asciicode;bptr:integer;begin bufptr:=1;
while not eoln(dictionary)and(bufptr<80)do begin buf[bufptr]:=getc(
dictionary);bufptr:=bufptr+1;end;buf[bufptr]:=' ';readln(dictionary);
word[1]:=64;wlen:=1;bufptr:=1;c:=xord[buf[bufptr]];
if(c<=57)and(c>=48)then begin wordwt:=(c-48);wtchg:=true;
bufptr:=bufptr+1;end;repeat c:=xord[buf[bufptr]];
if c>=65 then begin wlen:=wlen+1;word[wlen]:=c;dots[wlen]:=0;
dotw[wlen]:=wordwt;
end else if c>=48 then dotw[wlen]:=(c-48)else dots[wlen]:=c;
bufptr:=bufptr+1;until buf[bufptr]=' ';wlen:=wlen+1;word[wlen]:=64;
dotw[wlen-3]:=1;end;{:52}{53:}procedure hyphenate;label 30;
var spos,dpos,fpos:wordindex;t:triepointer;h:optype;v:valtype;
begin for spos:=wlen-3 downto 0 do begin nomore[spos]:=false;
hval[spos]:=0;fpos:=spos+1;t:=1+word[fpos];repeat h:=trier[t];
while h>0 do{54:}begin dpos:=spos+ops[h].dot;v:=ops[h].val;
if(v<maxval)and(hval[dpos]<v)then hval[dpos]:=v;
if(v>=hyphlevel)then if((fpos-patlen)<=(dpos-patdot))and((dpos-patdot)<=
spos)then nomore[dpos]:=true;h:=ops[h].op;end{:54};t:=triel[t];
if t=0 then goto 30;fpos:=fpos+1;t:=t+word[fpos];
until triec[t]<>word[fpos];30:end;end;{:53}{55:}procedure changedots;
var dpos:wordindex;
begin for dpos:=wlen-3 downto 3 do begin if hval[dpos]>0 then if odd(
hval[dpos])then begin if dots[dpos]=45 then dots[dpos]:=42 else if dots[
dpos]=0 then dots[dpos]:=46;
end else begin if dots[dpos]=46 then dots[dpos]:=0 else if dots[dpos]=42
then dots[dpos]:=45;end;
if dots[dpos]=42 then goodcount:=goodcount+dotw[dpos]else if dots[dpos]=
46 then badcount:=badcount+dotw[dpos]else if dots[dpos]=45 then
misscount:=misscount+dotw[dpos];end;end;{:55}{56:}
procedure outputhyphenatedword;var dpos:wordindex;
begin if wtchg then begin write(pattmp,wordwt:1);wtchg:=false end;
if wlen<6 then{57:}
begin for dpos:=2 to wlen-1 do write(pattmp,xchr[word[dpos]]);
writeln(pattmp);end{:57}else begin write(pattmp,xchr[word[2]]);
for dpos:=3 to wlen-3 do begin write(pattmp,xchr[word[dpos]]);
if dots[dpos]>0 then write(pattmp,xchr[dots[dpos]]);
if(dotw[dpos]<>wordwt)and(dpos<wlen-3)then write(pattmp,dotw[dpos]:1);
end;writeln(pattmp,xchr[word[wlen-2]],xchr[word[wlen-1]]);end;end;{:56}
{58:}procedure doword;label 22,30;var spos,dpos,fpos:wordindex;
a:triecpointer;goodp:boolean;
begin for dpos:=wlen-3 downto 3 do begin spos:=dpos-patdot;
fpos:=spos+patlen;{60:}
if(spos<0)or(fpos>wlen)or nomore[dpos]then goto 22;
if dots[dpos]=gooddot then goodp:=true else if dots[dpos]=baddot then
goodp:=false else goto 22;{:60};spos:=spos+1;a:=1+word[spos];
while spos<fpos do begin spos:=spos+1;a:=triecl[a]+word[spos];
if triecc[a]<>word[spos]then begin a:=insertcpat(fpos);goto 30;end;end;
30:if goodp then triecl[a]:=triecl[a]+dotw[dpos]else triecr[a]:=triecr[a
]+dotw[dpos];22:end;end;{:58}{62:}procedure dodictionary;
begin goodcount:=0;badcount:=0;misscount:=0;wordwt:=1;wtchg:=false;
argv(1,fname);reset(dictionary,fname);{63:}
if odd(hyphlevel)then begin gooddot:=45;baddot:=0;
end else begin gooddot:=46;baddot:=42;end{:63};
if procesp then begin initcounttrie;
writeln(stdout,'processing dictionary with pat_len = ',patlen:1,
', pat_dot = ',patdot:1);end;if hyphp then begin{64:}
begin filnam[1]:='p';filnam[2]:='a';filnam[3]:='t';filnam[4]:='t';
filnam[5]:='m';filnam[6]:='p';filnam[7]:='.';end{:64};
filnam[8]:=chr(hyphlevel+ord('0'));rewrite(pattmp,filnam);
writeln(stdout,'writing ',filnam);end;{65:}
while not eof(dictionary)do begin readword;hyphenate;changedots;
if hyphp then outputhyphenatedword;if procesp then doword;end{:65};
writeln(stdout,' ');
writeln(stdout,goodcount:1,' good, ',badcount:1,' bad, ',misscount:1,
' missed');printreal((100*goodcount/(goodcount+misscount)),1,2);
write(stdout,' %, ');
printreal((100*badcount/(goodcount+misscount)),1,2);
write(stdout,' %, ');
printreal((100*misscount/(goodcount+misscount)),1,2);
writeln(stdout,' %');
if procesp then writeln(stdout,patcount:1,' patterns, ',trieccount:1,
' nodes in count trie, ','triec_max = ',triecmax:1);end;{:62}{66:}
procedure readpatterns;var c:asciicode;begin levelpatterncount:=0;
argv(2,fname);reset(patterns,fname);
while not eof(patterns)do begin bufptr:=1;
while not eoln(patterns)and(bufptr<80)do begin buf[bufptr]:=getc(
patterns);bufptr:=bufptr+1;end;buf[bufptr]:=' ';readln(patterns);
levelpatterncount:=levelpatterncount+1;{67:}patlen:=0;bufptr:=1;
while buf[bufptr]<>' 'do begin c:=xord[buf[bufptr]];
if c>=65 then begin patlen:=patlen+1;pat[patlen]:=c;
end else if c=46 then begin patlen:=patlen+1;pat[patlen]:=64;end;
bufptr:=bufptr+1;end{:67};{68:}patdot:=0;bufptr:=1;
while buf[bufptr]<>' 'do begin c:=xord[buf[bufptr]];
if(c<=57)and(c>=48)then insertpattern(c-48,patdot)else patdot:=patdot+1;
bufptr:=bufptr+1;end{:68};end;
writeln(stdout,levelpatterncount:1,' patterns read in');
writeln(stdout,'pattern trie has ',triecount:1,' nodes, ','trie_max = ',
triemax:1,', ',opcount:1,' outputs');end;{:66}{69:}begin initialize;
initpatterntrie;readpatterns;procesp:=true;hyphp:=false;
write(stdout,'hyph_start = ');hyphstart:=inputint(stdin);
write(stdout,'hyph_finish = ');hyphfinish:=inputint(stdin);
for hyphlevel:=hyphstart to hyphfinish do begin levelpatterncount:=0;
for patdot:=0 to maxdot do levelnomore[patdot]:=false;
if hyphlevel>hyphstart then writeln(stdout,' ');
write(stdout,'pat_start = ');patstart:=inputint(stdin);
write(stdout,'pat_finish = ');patfinish:=inputint(stdin);
write(stdout,'good weight, bad weight, threshold: ');
input3ints(goodwt,badwt,thresh);{71:}
for patlen:=patstart to patfinish do begin patdot:=patlen div 2;
dot1:=patdot*2;dot2:=patlen*2-1;repeat patdot:=dot1-patdot;
dot1:=dot2-dot1;if levelnomore[patdot]then goto 22;dodictionary;
collectcounttrie;if not moretocome then levelnomore[patdot]:=true;
22:until patdot=patlen;
for patdot:=patlen downto 0 do if levelnomore[patdot]then levelnomore[
patdot+1]:=true;end;{:71};deletebadpatterns;
writeln(stdout,'total of ',levelpatterncount:1,
' patterns at hyph_level ',hyphlevel:1);end;outputpatterns(1,0);{72:}
procesp:=false;hyphp:=true;hyphlevel:=hyphfinish;
write(stdout,'hyphenate word list? ');begin buf[1]:=getc(stdin);
readln(stdin);end;if(buf[1]='Y')or(buf[1]='y')then dodictionary{:72};
9999:end.{:69}
