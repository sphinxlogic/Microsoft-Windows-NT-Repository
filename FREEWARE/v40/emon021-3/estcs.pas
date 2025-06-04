(****************************************************************)
(*								*)
(*		ESSAIS PASCAL DE TCS, AG2T ET AG2C		*)
(*								*)
(****************************************************************)
[inherit ('exttcs.pen','extag2t.pen','extag2c.pen')] program estcs;
var
x : array[1..5] of real := ( 4.0,  6.0, 12.0,  8.0, 22.0);
y : array[1..7] of real := ( 6.0, 12.0, 24.0, 34.0, 43.0, 55.0, 70.0 );
i : integer;
begin
initt(960,3);
movabs(100,100);
drwabs(300,100);
drwabs(200,187);
drwabs(100,100);
pntabs(200,140);
movrel(200,-40);
drwrel(200,0);
drwrel(-100,87);
drwrel(-100,-87);
pntrel(100,40);
dwindo(10.0,100.0,-10.0,50.0);
vwindo(10.0,90.0,-10.0,50.0);
movea(20.0,20.0);
drawa(80.0,20.0);
drawa(50.0,40.0);
drawa(20.0,20.0);
pointa(50.0,30.0);
mover(-20.0,-5.0);
drawr(40.0,0.0);
drawr(-20.0,10.0);
drawr(-20.0,-10.0);
pointr(5.0,5.0);
twindo(500,768,500,768);
swindo(500,268,500,268);
movea(20.0,20.0);
drawa(80.0,20.0);
drawa(50.0,40.0);
drawa(20.0,20.0);
pointa(50.0,30.0);
movabs(0,0);
dshabs(800,500,1234);
dshrel(-600,200,9);
swindo(0,1023,0,768);
vwindo(0.0,100.0,0.0,100.0);
movea(20.0,80.0);
dasha(80.0,10.0,5414);
dashr(-80.0,100.0,9);
tinput(i);
newpag;
binitt;
slimx(200,800);
slimy(100,700);
check(x,y);
dsplay(x,y);
finitt(0,0);
end.
