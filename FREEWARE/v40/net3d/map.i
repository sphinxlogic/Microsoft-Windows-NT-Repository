map 21 20  
 200.0  
 1200  
{
DSAGKADSKGLHDGKSHGDKA
DFBABCBAAAAABCBAAAKLA
KAAAAAAAAAAAAAAAAAAMA
NAAAAAAAAAAAAAAAAAAPA
LAAAAAAAAAAAAAAAAAANA
NAAAAAAAAAAAAAAAAAAPA
ZDBAAAAAAAAAAAAAAABQA
PAAAAAAAAAAAAAAAAAANA
NAAAAAAAAAAAAAAAAAAMA
OAAAAAAAAAAAAAAAAAALA
PAAAAAAAAGGAAAAAAAANA
NAAAAAAAAAAAAAAAAAAMA
AAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAA
AAJGJKGJKJKAAAAAAAAAA
AAAjjjkfuJLKAAAAAAAAA
AAAJHGJJGJJAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAA
}
 
lookout 815 915 130
fadeto 22000 22000 22000
ground 2 2
 

vehicle house def {
name "An average, boring house"
type static
hp 20
object {
at 0 0 0
 points {
 0 0 0 40 0 0 40 40 0 0 40 0
 0 0 30 40 0 30 40 40 30 0 40 30
 20 20 40
 }
 face 50 f { 0 4 5 1 }
 face 51 f { 1 5 6 2 }
 face 52 f { 2 6 7 3 }
 face 53 f { 3 7 4 0 }
 face 140 f { 4 8 5 }
 face 142 f { 5 8 6 }
 face 144 f { 6 8 7 }
 face 146 f { 7 8 4 }
 }
object {
 at 0 0 30
 points { 0 0 0 0 0 10 3 0 10 -3 0 10
 3 3 10 3 -3 10 -3 3 10 -3 -3 10 }
 face 32 l { 0 1 }
 face 32 l { 2 3 }
 face 32 l { 4 5 }
 face 32 l { 6 7 }
 }
}

 
vehicle pylon def {
name "A power pole"
type static
hp 6
object {
 at x y 0
 points { 0 0 0 3 0 0 3 3 0 0 3 0
 0 0 25 3 0 25 3 3 25 0 3 25
 }
 face 170 f { 0 4 5 1 }
 face 171 f { 1 5 6 2 }
 face 172 f { 2 6 7 3 }
 face 173 f { 3 7 4 0 }
 face 174 f { 7 6 5 4 }
 }
object {
 at x y 0
 points { 0 -4 22 0 7 22 }
 face 32 l { 0 1 }
 }
object {
 at x y 0
 points { 3 -4 22 3 7 22 }
 face 32 l { 0 1 }
 }
}

 

 

 
vehicle house2 def {
name "A fairly large house"
type static
hp 35
object {
 at x y 0
 points {
 0 0 0 80 0 0 80 40 0 0 40 0
 0 0 20 80 0 20 80 40 20 0 40 20
 20 20 30 60 20 30
 }
 face 140 f { 0 4 5 1 }
 face 141 f { 1 5 6 2 }
 face 142 f { 2 6 7 3 }
 face 143 f { 3 7 4 0 }
 face 50 f { 4 8 9 5 }
 face 51 f { 5 9 6 }
 face 52 f { 6 9 8 7 }
 face 53 f { 7 8 4 }
 }
}
 

 
vehicle tower def {
name "A 10 storey building"
type static
hp 66
object {  
 points {
 0 0 0 50 0 0 50 50 0 0 50 0
 0 0 80 50 0 80 50 50 80 0 50 80
 }
 face 50 f { 0 4 5 1 }
 face 51 f { 1 5 6 2 }
 face 52 f { 2 6 7 3 }
 face 53 f { 3 7 4 0 }
 face 54 f { 7 6 5 4 }
 }
object {  
 points {
 10 10 80 40 10 80 40 40 80 10 40 80
 10 10 200 40 10 200 40 40 200 10 40 200
 }
 face 55 f { 0 4 5 1 }
 face 56 f { 1 5 6 2 }
 face 57 f { 2 6 7 3 }
 face 58 f { 3 7 4 0 }
 face 59 f { 7 6 5 4 }
 }
object {  
 points {
 15 15 200 35 15 200 35 35 200 15 35 200 25 25 300
 }
 face 60 f { 0 4 1 }
 face 61 f { 1 4 2 }
 face 62 f { 2 4 3 }
 face 63 f { 3 4 0 }
 }
object {  
 points { 25 25 300 25 25 330 }
 face 255 l { 0 1 }
 }
}
 

 

 
 
vehicle HOUSE { instance house offset 700 700 0 }
vehicle HOUSE { instance house offset 760 700 0 }
vehicle HOUSE { instance house offset 820 700 0 }
vehicle HOUSE { instance house offset 880 700 0 }
vehicle HOUSE { instance house offset 700 780 0 }
vehicle HOUSE2 { instance house2 offset 760 780 0 }
 
 
vehicle road {
name "The road between villages"
type scenery
object { at 700 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 900 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 1100 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 1300 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 1500 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 1700 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 1900 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 2100 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 2300 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 2500 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 2700 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 2900 740 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object {  
 points { 3100 700 0 3220 700 0 3220 820 0 3100 820 0 }
 face 45 p { 0 1 2 3 }
 }
}
 
 
vehicle PYLON { instance pylon offset 960 737 0 }
vehicle PYLON { instance pylon offset 960 780 0 }
vehicle PYLON { instance pylon offset 1160 737 0 }
vehicle PYLON { instance pylon offset 1160 780 0 } 
vehicle PYLON { instance pylon offset 1360 737 0 }
vehicle PYLON { instance pylon offset 1360 780 0 } 
 
 
vehicle road2 {
name "The East-West Road"
type scenery
object { at 1700 500 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 1700 700 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 1700 900 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 1700 1100 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
}
 
 
vehicle HOUSE { instance house offset 1700 1300 0 }
 
 
vehicle HOUSE { instance house offset 1740 700 0 }
vehicle HOUSE2 { instance house2 offset 1740 780 0 }
vehicle HOUSE { instance house offset 1660 700 0 }
 
vehicle church {
name "A Church Building with tower"
type static
hp 100
offset 800 900 0
object {  
 at 0 0 0
 points {
 0 0 0 30 0 0 30 30 0 0 30 0
 0 0 90 30 0 90 30 30 90 0 30 90
 15 15 120
 }
 face 50 f { 0 4 5 1 }
 face 51 f { 1 5 6 2 }
 face 52 f { 2 6 7 3 }
 face 53 f { 3 7 4 0 }
 face 174 f { 4 8 5 }
 face 175 f { 5 8 6 }
 face 176 f { 6 8 7 }
 face 177 f { 7 8 4 }
 }
}
vehicle HOUSE2 { instance house2 offset 831 900 0 }
 
vehicle road3 {
name "The road from the village to tower"
type scenery
object { at 910 780 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
}
 
 
vehicle HOUSE { instance house offset 2800 3000 0 }
vehicle HOUSE { instance house offset 2800 3100 0 }
vehicle HOUSE { instance house offset 2880 3050 0 }
vehicle HOUSE2 { instance house2 offset 2880 3130 0 }
vehicle road4 {
name "A long long road"
type scenery
object { at 2840 3000 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 2800 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 2600 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 2400 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 2200 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 2000 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 1800 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 1600 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 1400 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 1200 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 1000 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 800 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2840 600 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
}
 
 
vehicle TREE { instance tree offset 1502 1002 0 }
vehicle TREE { instance tree offset 1515 1060 0 }
vehicle TREE { instance tree offset 2150 1486 0 }
vehicle TREE { instance tree offset 1081 1080 0 }
vehicle TREE { instance tree offset 1590 1801 0 }
vehicle TREE { instance tree offset 1933 1052 0 }
vehicle TREE { instance tree offset 1560 1010 0 }
vehicle TREE { instance tree offset 2581 1243 0 }
vehicle TREE { instance tree offset 1820 1030 0 }
vehicle TREE { instance tree offset 1201 1096 0 }
vehicle TREE { instance tree offset 2315 1638 0 }
vehicle TREE { instance tree offset 1500 1064 0 }
 
 
vehicle TOWER { instance tower offset 3200 3800 0 }
vehicle TOWER { instance tower offset 3290 3800 0 }
vehicle TOWER { instance tower offset 3200 3870 0 }
vehicle TOWER { instance tower offset 3290 3920 0 }
 
 
vehicle missilebonus {
name "A Missile Launcher"
flying
type weapon
weapon missile
hp 2
offset 3100 3700 2
object {
 points { 0 0 0 5 0 0 5 5 0 0 5 0
 0 0 5 5 0 5 5 5 5 0 5 5 }
 face 25 f { 0 4 5 1 }
 face 26 f { 1 5 6 2 }
 face 27 f { 2 6 7 3 }
 face 28 f { 3 7 4 0 }
 face 29 f { 0 1 2 3 }
 face 30 f { 7 6 5 4 }
 }
}
 
 
vehicle road {
name "The city road"
type scenery
object { at 3250 4000 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }  
object { at 3250 3800 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 3250 3600 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 3250 3400 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 3250 3200 0 points { 0 0 0 0 200 0 40 200 0 40 0 0 } face 45 p { 0 1 2 3 } }
object { at 2650 3200 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }  
object { at 2850 3200 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
object { at 3050 3200 0 points { 0 0 0 200 0 0 200 40 0 0 40 0 } face 45 p { 0 1 2 3 } }
}
 
 

vehicle pterodactyl def {
name "a pterodactyl or flying serpent"
angle 90
flying
type bird
max velocity 200

object {  
 at 100 100 60
 centre 0 4.5 0
 counter
 points {
 0 6 2 0 4 5 -0.5 3.5 3.5 0.5 3.5 3.5
 }
 face 188 f { 0 1 3 }
 face 186 f { 2 1 0 }
 face 180 f { 0 3 2 }
}

object {  
 at 100 100 60
 centre 0 4.5 0
 counter
 points {
 0 4 5 -0.5 3.5 3.5 0.5 3.5 3.5 0 -3 0 -0.5 -2 0 0.5 -2 0
 }
 face 48 f { 0 3 5 2 }
 face 50 f { 0 1 4 3 }
 face 45 f { 1 2 5 4 }
}


object {  
 at 100 100 60
 centre 2.4 -0.8 3.34
 counter
 points {
 0 3 4.286 3 1 8 3 -4 4 0 -2 0.714 
 }
 face 42 p { 0 1 2 3 }
 oscli 1 1 { - - -7 }
 oscli 2 1 { - - -7 }
}

object {  
 at 100 100 60
 centre 4 -1.67 3
 counter
 points {
 3 1 8 3 -4 4 6 -2 4
 }
 face 40 p { 0 1 2 }
 oscli 0 1 { - - -7 }
 oscli 1 1 { - - -7 }
 oscli 2 1 { - - -9 }
}

object {  
 at 100 100 60
 centre -2.4 -0.8 3.34
 counter
 points {
 0 3 4.286 -3 1 8 -3 -4 4 0 -2 0.714 
 }
 face 40 p { 0 1 2 3 }
 oscli 1 1 { - - -7 }
 oscli 2 1 { - - -7 }
}

object {  
 at 100 100 60
 centre -4 -1.67 3
 counter
 points {
 -3 1 8 -3 -4 4 -6 -2 4
 }
 face 42 p { 0 1 2 }
 oscli 0 1 { - - -7 }
 oscli 1 1 { - - -7 }
 oscli 2 1 { - - -9 }
}

object {  
 at 100 100 60
 centre 0 -3.67 -1.33
 counter
 points {
 0 -3 0 -0.5 -2 0 0.5 -2 0 0 -6 -3
 }
 face 184 f { 0 1 3 }
 face 182 f { 0 3 2 }
 face 178 f { 2 3 1 }
}

}


vehicle amigaball def {
name "an Amiga beachball"
flying
type thing

object {
 at 0 0 12
 centre 0 0 0
 points {
  
 0 0 10
 0 0 -10
 0 3.8268 -9.2388  
 2.2494 3.0960 -9.2388  
 3.6395 1.1826 -9.2388  
 3.6395 -1.1826 -9.2388  
 2.2494 -3.0960 -9.2388  
 0 -3.8268 -9.2388  
 -2.2494 -3.0960 -9.2388  
 -3.6395 -1.1826 -9.2388  
 -3.6395 1.1826 -9.2388  
 -2.2494 3.0960 -9.2388  
 0 7.0711 -7.0711  
 4.1563 5.7206 -7.0711  
 6.7250 2.1851 -7.0711  
 6.7250 -2.1851 -7.0711  
 4.1563 -5.7206 -7.0711  
 0 -7.0711 -7.0711  
 -4.1563 -5.7206 -7.0711  
 -6.7250 -2.1851 -7.0711  
 -6.7250 2.1851 -7.0711  
 -4.1563 5.7206 -7.0711  
 0 9.2388 -3.8268  
 5.4304 7.4743 -3.8268  
 8.7866 2.8549 -3.8268  
 8.7866 -2.8549 -3.8268  
 5.4304 -7.4743 -3.8268  
 0 -9.2388 -3.8268  
 -5.4304 -7.4743 -3.8268  
 -8.7866 -2.8549 -3.8268  
 -8.7866 2.8549 -3.8268  
 -5.4304 7.4743 -3.8268  
 0 10 0  
 5.8779 8.0902 0  
 9.5106 3.0902 0  
 9.5106 -3.0902 0  
 5.8779 -8.0902 0  
 0 -10 0  
 -5.8779 -8.0902 0  
 -9.5106 -3.0902 0  
 -9.5106 3.0902 0  
 -5.8778 8.0902 0  
 0 9.2388 3.8268  
 5.4304 7.4743 3.8268  
 8.7866 2.8549 3.8268  
 8.7866 -2.8549 3.8268  
 5.4304 -7.4743 3.8268  
 0 -9.2388 3.8268  
 -5.4304 -7.4743 3.8268  
 -8.7866 -2.8549 3.8268  
 -8.7866 2.8549 3.8268  
 -5.4304 7.4743 3.8268  
 0 7.0711 7.0711  
 4.1563 5.7206 7.0711  
 6.7250 2.1851 7.0711  
 6.7250 -2.1851 7.0711  
 4.1563 -5.7206 7.0711  
 0 -7.0711 7.0711  
 -4.1563 -5.7206 7.0711  
 -6.7250 -2.1851 7.0711  
 -6.7250 2.1851 7.0711  
 -4.1563 5.7206 7.0711  
 0 3.8268 9.2388  
 2.2494 3.0960 9.2388  
 3.6395 1.1826 9.2388  
 3.6395 -1.1826 9.2388  
 2.2494 -3.0960 9.2388  
 0 -3.8268 9.2388  
 -2.2494 -3.0960 9.2388  
 -3.6395 -1.1826 9.2388  
 -3.6395 1.1826 9.2388  
 -2.2494 3.0960 9.2388  

 }  

 face 152 f { 0 62 63 }
 face 60 f { 0 63 64 }

 face 60 f { 1 3 2 }
 face 152 f { 1 4 3 }
 face 152 f { 2 3 13 12 }
 face 60 f { 3 4 14 13 }
 face 152 f { 13 14 24 23 }
 face 60 f { 12 13 23 22 }
 face 152 f { 22 23 33 32 }
 face 60 f { 23 24 34 33 }
 face 152 f { 33 34 44 43 }
 face 60 f { 32 33 43 42 }
 face 152 f { 42 43 53 52 }
 face 60 f { 43 44 54 53 }
 face 152 f { 53 54 64 63 }
 face 60 f { 52 53 63 62 }

 face 152 f { 0 64 65 }
 face 60 f { 0 65 66 }

 face 60 f { 1 5 4 }
 face 152 f { 1 6 5 }
 face 152 f { 4 5 15 14 }
 face 60 f { 5 6 16 15 }
 face 152 f { 15 16 26 25 }
 face 60 f { 14 15 25 24 }
 face 152 f { 24 25 35 34 }
 face 60 f { 25 26 36 35 }
 face 152 f { 35 36 46 45 }
 face 60 f { 34 35 45 44 }
 face 152 f { 44 45 55 54 }
 face 60 f { 45 46 56 55 }
 face 152 f { 55 56 66 65 }
 face 60 f { 54 55 65 64 }

 face 152 f { 0 66 67 }
 face 60 f { 0 67 68 }

 face 60 f { 1 7 6 }
 face 152 f { 1 8 7 }
 face 152 f { 6 7 17 16 }
 face 60 f { 7 8 18 17 }
 face 152 f { 17 18 28 27 }
 face 60 f { 16 17 27 26 }
 face 152 f { 26 27 37 36 }
 face 60 f { 27 28 38 37 }
 face 152 f { 37 38 48 47 }
 face 60 f { 36 37 47 46 }
 face 152 f { 46 47 57 56 }
 face 60 f { 47 48 58 57 }
 face 152 f { 57 58 68 67 }
 face 60 f { 56 57 67 66 }

 face 152 f { 0 68 69 }
 face 60 f { 0 69 70 }

 face 60 f { 1 9 8 }
 face 152 f { 1 10 9 }
 face 152 f { 8 9 19 18 }
 face 60 f { 9 10 20 19 }
 face 152 f { 19 20 30 29 }
 face 60 f { 18 19 29 28 }
 face 152 f { 28 29 39 38 }
 face 60 f { 29 30 40 39 }
 face 152 f { 39 40 50 49 }
 face 60 f { 38 39 49 48 }
 face 152 f { 48 49 59 58 }
 face 60 f { 49 50 60 59 }
 face 152 f { 59 60 70 69 }
 face 60 f { 58 59 69 68 }

 face 152 f { 0 70 71 }
 face 60 f { 0 71 62 }

 face 60 f { 1 11 10 }
 face 152 f { 1 2 11 }
 face 152 f { 10 11 21 20 }
 face 60 f { 11 2 12 21 }
 face 152 f { 21 12 22 31 }
 face 60 f { 20 21 31 30 }
 face 152 f { 30 31 41 40 }
 face 60 f { 31 22 32 41 }
 face 152 f { 41 32 42 51 }
 face 60 f { 40 41 51 50 }
 face 152 f { 50 51 61 60 }
 face 60 f { 51 42 52 61 }
 face 152 f { 61 52 62 71 }
 face 60 f { 60 61 71 70 }

  
 bounce -1 1 { - - 30 }

 }
}


vehicle fish def {
name "a giant flying fish"
angle 90
flying
type fish
hp 15

object {  
 at 100 100 60
 centre 0 -5 0
 counter
 points {
  
 -10 0 0 -10 0 2 -8 0 6 -6 0 8 -2 0 10 
 2 0 10 6 0 8 8 0 6 10 0 2 
 10 0 0 10 0 -2 8 0 -6 6 0 -8 2 0 -10 
 -2 0 -10 -6 0 -8 -8 0 -6 -10 0 -2
   
 -8 6 2 -4 10 4 4 10 4 8 6 2 
 8 6 2 4 10 4 -4 10 4 -8 6 2
  
 -8 -6 0 -8 -6 2 -6 -6 6 -2 -6 8 2 -6 8 6 -6 6 8 -6 2 8 -6 0
  
 -2 -10 0 0 -10 4 2 -10 0 0 -10 -4
 }
 oscli 18 1 { - - -2 }
 oscli 19 1 { - - -4 }
 oscli 20 1 { - - -4 }
 oscli 21 1 { - - -2 }

  
 face 243 f { 0 1 18 }
 face 244 f { 1 2 18 }
 face 245 f { 2 3 19 18 }
 face 246 f { 3 4 19 }
 face 245 f { 4 5 20 19 }
 face 244 f { 5 6 20 }
 face 243 f { 6 7 21 20 }
 face 242 f { 7 8 21 }
 face 239 f { 8 9 21 }
  
 face 200 f { 0 18 19 20 21 9 }
  
 face 239 f { 26 27 1 0 }
 face 241 f { 27 28 2 1 }
 face 242 f { 28 3 2 }
 face 242 f { 28 29 4 3 }
 face 241 f { 29 30 5 4 }
 face 240 f { 30 31 6 5 }
 face 239 f { 31 7 6 }
 face 238 f { 31 32 8 7 }
 face 237 f { 32 33 9 8 }
  
 face 235 f { 26 34 27 }
 face 237 f { 27 34 28 }
 face 238 f { 34 35 28 }
 face 239 f { 28 35 29 }
 face 241 f { 29 35 30 }
 face 240 f { 30 35 31 }
 face 238 f { 31 35 36 }
 face 236 f { 32 31 36 }
 face 234 f { 32 36 33 }
}

object {  
 at 100 100 60
 centre 0 -5 0
 counter
 points {
  
 -10 0 0 -10 0 2 -8 0 6 -6 0 8 -2 0 10 
 2 0 10 6 0 8 8 0 6 10 0 2 
 10 0 0 10 0 -2 8 0 -6 6 0 -8 2 0 -10 
 -2 0 -10 -6 0 -8 -8 0 -6 -10 0 -2
   
 -8 6 -2 -4 10 -4 4 10 -4 8 6 -2 
 8 6 -2 4 10 -4 -4 10 -4 -8 6 -2
  
 8 -6 0 8 -6 -2 6 -6 -6 2 -6 -8 -2 -6 -8 -6 -6 -6 -8 -6 -2 -8 -6 0
  
 -2 -10 0 0 -10 4 2 -10 0 0 -10 -4
 }
 oscli 22 1 { - - 2 }
 oscli 23 1 { - - 4 }
 oscli 24 1 { - - 4 }
 oscli 25 1 { - - 2 }

  
 face 237 f { 9 10 22 }
 face 236 f { 10 11 22 }
 face 234 f { 11 12 23 22 }
 face 232 f { 12 13 23 }
 face 231 f { 13 14 24 23 }
 face 232 f { 14 15 24 }
 face 234 f { 15 16 25 24 }
 face 236 f { 16 17 25 }
 face 237 f { 17 0 25 }
  
 face 207 f { 0 9 22 23 24 25 }
  
 face 237 f { 9 26 27 10 }
 face 235 f { 10 27 28 11 }
 face 233 f { 11 28 12 }
 face 232 f { 12 28 29 13 }
 face 234 f { 13 29 30 14 }
 face 235 f { 14 30 31 15 }
 face 236 f { 15 31 16 }
 face 238 f { 16 31 32 17 }
 face 239 f { 17 32 33 0 }
  
 face 234 f { 26 36 27 }
 face 232 f { 27 36 28 }
 face 231 f { 28 36 37 }
 face 230 f { 28 37 29 }
 face 229 f { 29 37 30 }
 face 230 f { 30 37 31 }
 face 232 f { 37 34 31 }
 face 234 f { 34 32 31 }
 face 235 f { 34 33 32 }
}

object {  
 at 100 100 60
 centre -10.5 -4.5 0
 counter
 points {
 -10 0 0 -16 -6 0 -8 -6 2 -8 -6 -2
 }

 face 214 f { 0 1 2 }
 face 211 f { 3 1 0 }
 face 212 f { 3 2 0 }
}
object {  
 at 100 100 60
 centre 10.5 -4.5 0
 counter
 points {
 10 0 0 16 -6 0 8 -6 2 8 -6 -2
 }

 face 213 f { 2 1 0 }
 face 209 f { 0 1 3 }
 face 212 f { 0 2 3 }
}
object {  
 at 100 100 60
 centre 0 -4.5 10.5
 counter
 points {
 0 0 10 0 -6 16 -2 -6 8 2 -6 8 
 }

 face 215 f { 0 2 1 }
 face 211 f { 0 1 3 }
 face 212 f { 2 3 1 }
}

object {  
 at 100 100 60
 centre 0 -12 0
 counter
 points {
 -2 -10 0 0 -10 4 2 -10 0 0 -10 -4
 -4 -14 0 0 -14 8 4 -14 0 0 -14 -8
 }
 face 248 f { 4 5 1 0 }
 face 246 f { 5 6 2 1 }
 face 243 f { 6 7 3 2 }
 face 245 f { 7 4 0 3 }
 face 246 f { 7 6 5 4 }
}

}



vehicle fish {
instance fish
offset 600 600 200
}
 
vehicle amigaball {
instance amigaball
offset 1220 760 0
}
 
vehicle ptero2 {
instance pterodactyl
offset 1200 1100 160
}
 
vehicle ptero3 {
instance pterodactyl
offset 1000 1000 240
}
 
