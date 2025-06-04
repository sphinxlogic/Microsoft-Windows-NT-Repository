ground none
stars 200
lookout 0 700 -50
 

vehicle asteroid def {
name "An asteroid"
type thing
hp 40
flying
spin random 10 10 10
object {
 points {
  
 10 0 0 7 8 0 0 9 0 -8 6 0
 -11 0 0 -7 -6 0 0 -10 0 6 -6 0
  
 5 0 5 0 6 5 -5 0 4 0 -5 5
  
 0 0 8
 }
 face 170 f { 8 1 0 }  
 face 171 f { 9 2 1 }
 face 172 f { 9 3 2 }
 face 173 f { 10 4 3 }
 face 170 f { 10 5 4 }
 face 171 f { 11 6 5 }
 face 172 f { 11 7 6 }
 face 173 f { 8 0 7 }
 face 174 f { 8 9 1 }
 face 175 f { 10 3 9 }
 face 170 f { 11 5 10 }
 face 171 f { 8 7 11 }
 face 170 f { 12 9 8 }  
 face 172 f { 12 10 9 }
 face 174 f { 12 11 10 }
 face 176 f { 12 8 11 }
 }
object {
 counter
 points {
  
 10 0 0 7 8 0 0 9 0 -8 6 0
 -11 0 0 -7 -6 0 0 -10 0 6 -6 0
  
 5 0 -6 0 6 -5 -5 0 -4 0 -5 -5
  
 0 0 -9
 }
 face 170 f { 8 1 0 }  
 face 171 f { 9 2 1 }
 face 172 f { 9 3 2 }
 face 173 f { 10 4 3 }
 face 170 f { 10 5 4 }
 face 171 f { 11 6 5 }
 face 172 f { 11 7 6 }
 face 173 f { 8 0 7 }
 face 174 f { 8 9 1 }
 face 175 f { 10 3 9 }
 face 170 f { 11 5 10 }
 face 171 f { 8 7 11 }
 face 170 f { 12 9 8 }  
 face 172 f { 12 10 9 }
 face 174 f { 12 11 10 }
 face 176 f { 12 8 11 }
 }
}

vehicle ASTEROID { instance asteroid offset 500 -500 0 }
vehicle ASTEROID { instance asteroid offset 550 -520 20 }
vehicle ASTEROID { instance asteroid offset 520 -580 50 }
vehicle ASTEROID { instance asteroid offset 520 -600 0 }
vehicle ASTEROID { instance asteroid offset 590 -540 -30 }
vehicle ASTEROID { instance asteroid offset 570 -630 10 }
vehicle ASTEROID { instance asteroid offset 420 -420 0 }
vehicle ASTEROID { instance asteroid offset 660 -500 50 }
vehicle ASTEROID { instance asteroid offset 490 -760 0 }
vehicle ASTEROID { instance asteroid offset 670 -400 70 }
vehicle ASTEROID { instance asteroid offset 400 -630 90 }
vehicle ASTEROID { instance asteroid offset 420 -520 -40 }
 
vehicle ASTEROID {  
instance asteroid
offset 100 -500 0
velocity 20
}
vehicle ASTEROID {  
instance asteroid
offset -30 1100 0
hp 200
spin -10 -8 -3
scale 6 6 6
}
 

vehicle ASTEROID { instance asteroid offset -112 658 -37 }
vehicle ASTEROID { instance asteroid offset -35 751 -123 }
vehicle ASTEROID { instance asteroid offset 60 719 -138 }
vehicle ASTEROID { instance asteroid offset 36 849 17 }
vehicle ASTEROID { instance asteroid offset -66 560 -125 }
vehicle ASTEROID { instance asteroid offset -7 789 33 }
vehicle ASTEROID { instance asteroid offset 87 866 16 }
vehicle ASTEROID { instance asteroid offset 28 595 -39 }
vehicle ASTEROID { instance asteroid offset 117 554 -119 }
vehicle ASTEROID { instance asteroid offset 95 782 -14 }
vehicle ASTEROID { instance asteroid offset 74 605 44 }
vehicle ASTEROID { instance asteroid offset 52 551 -83 }
vehicle ASTEROID { instance asteroid offset 4 553 11 }
vehicle ASTEROID { instance asteroid offset -54 528 38 }
vehicle ASTEROID { instance asteroid offset 135 643 17 }
vehicle ASTEROID { instance asteroid offset 56 665 -147 }
vehicle ASTEROID { instance asteroid offset -88 534 3 }
vehicle ASTEROID { instance asteroid offset -130 544 -147 }
vehicle ASTEROID { instance asteroid offset 12 618 -128 }
vehicle ASTEROID { instance asteroid offset -23 557 -5 }
vehicle ASTEROID { instance asteroid offset -71 883 1 }
vehicle ASTEROID { instance asteroid offset 144 770 -91 }
vehicle ASTEROID { instance asteroid offset -102 657 -121 }
vehicle ASTEROID { instance asteroid offset -44 706 40 }
vehicle ASTEROID { instance asteroid offset -112 616 -136 }
vehicle ASTEROID { instance asteroid offset 12 816 -125 }
vehicle ASTEROID { instance asteroid offset 31 834 -7 }
vehicle ASTEROID { instance asteroid offset 72 533 -14 }
vehicle ASTEROID { instance asteroid offset 10 879 -79 }
vehicle ASTEROID { instance asteroid offset -149 636 -89 }
vehicle ASTEROID { instance asteroid offset 110 505 -21 }
vehicle ASTEROID { instance asteroid offset -6 775 -57 }
vehicle ASTEROID { instance asteroid offset -136 639 -62 }
vehicle ASTEROID { instance asteroid offset -29 502 21 }
vehicle ASTEROID { instance asteroid offset 84 817 32 }
vehicle ASTEROID { instance asteroid offset 65 686 -97 }
vehicle ASTEROID { instance asteroid offset 56 874 -99 }
vehicle ASTEROID { instance asteroid offset -102 573 -116 }
vehicle ASTEROID { instance asteroid offset -57 610 -2 }
vehicle ASTEROID { instance asteroid offset -140 620 -101 }

vehicle cosmosphere def {
name "A CO$MO$PHERE"
type hover
weapon chicken
ammo 40
hp 70
flying

object {
 points {
  
 
 -10 3 -4 -8 3 -8 -4 3 -10 4 3 -10 8 3 -8 10 3 -4
 10 3 4 8 3 8 4 3 10 -4 3 10 -8 3 8 -10 3 4
 
 -10 -3 -4 -8 -3 -8 -4 -3 -10 4 -3 -10 8 -3 -8 10 -3 -4
 10 -3 4 8 -3 8 4 -3 10 -4 -3 10 -8 -3 8 -10 -3 4

  
 -8 8 3 -4 10 3 4 10 3 8 8 3
 8 -8 3 4 -10 3 -4 -10 3 -8 -8 3

 -8 8 -3 -4 10 -3 4 10 -3 8 8 -3
 8 -8 -3 4 -10 -3 -4 -10 -3 -8 -8 -3

  
 3 8 -8 3 8 8 3 -8 8 3 -8 -8
 -3 8 -8 -3 8 8 -3 -8 8 -3 -8 -8
 }
  
  
 face 150 f { 9 8 20 21 }
 
 face 175 f { 10 9 21 22 }
 face 147 f { 11 10 22 23 }
 face 174 f { 8 7 19 20 }
 face 146 f { 7 6 18 19 }
 face 145 f { 6 5 17 18 }
 face 142 f { 5 4 16 17 }
 face 141 f { 4 3 15 16 }
 face 143 f { 3 2 14 15 }
 face 145 f { 2 1 13 14 }
 face 147 f { 1 0 12 13 }
 face 149 f { 0 11 23 12 }
  
 face 150 f { 32 24 11 0 }
 face 151 f { 33 25 24 32 }
 face 150 f { 34 26 25 33 }
 face 149 f { 35 27 26 34 }
 face 147 f { 5 6 27 35 }
 face 144 f { 36 28 18 17 }
 face 142 f { 37 29 28 36 }
 face 143 f { 38 30 29 37 }
 face 145 f { 39 31 30 38 }
 face 147 f { 12 23 31 39 }
  
 face 146 f { 2 3 40 44 }
 face 149 f { 44 40 34 33 }
 face 151 f { 25 26 41 45 }
 face 150 f { 45 41 8 9 }
 face 149 f { 21 20 42 46 }
 face 146 f { 46 42 29 30 }
 face 141 f { 38 37 43 47 }
 face 142 f { 47 43 15 14 }
  
 face 169 f { 1 2 44 }
 face 171 f { 0 1 32 }
 face 174 f { 32 44 33 }
 face 172 f { 1 44 32 }

 face 168 f { 3 4 40 }
 face 167 f { 4 5 35 }
 face 171 f { 35 34 40 }
 face 169 f { 40 4 35 }

 face 174 f { 9 10 45 }
 face 172 f { 10 11 24 }
 face 175 f { 24 25 45 }
 face 174 f { 24 45 10 }

 face 173 f { 7 8 41 }
 face 171 f { 6 7 27 }
 face 173 f { 26 27 41 }
 face 172 f { 27 7 41 }

 face 168 f { 14 13 47 }
 face 170 f { 13 12 39 }
 face 173 f { 47 39 38 }
 face 171 f { 13 39 47 }

 face 167 f { 43 16 15 }
 face 166 f { 36 17 16 }
 face 170 f { 37 36 43 }
 face 168 f { 43 36 16 }

 face 174 f { 46 22 21 }
 face 172 f { 31 23 22 }
 face 175 f { 30 31 46 }
 face 174 f { 31 22 46 }

 face 173 f { 20 19 42 }
 face 171 f { 19 18 28 }
 face 173 f { 42 28 29 }
 face 172 f { 42 19 28 }

 scale 5 5 5 
}

object {  
 centre 10 0 -50.5
 turret

 points {
 -1 -2 -10 1 -2 -10 2 -1 -10 2 1 -10 1 2 -10 -1 2 -10 -2 1 -10
 -2 -1 -10

 -1 -1 -11 1 -1 -11 1 1 -11 -1 1 -11
 }

 face 43 f { 0 1 9 8 }
 face 44 f { 0 8 7 }
 face 44 f { 1 2 9 }
 face 45 f { 7 8 11 6 }
 face 30 f { 9 2 3 10 }
 face 44 f { 8 9 10 11 }
 face 46 f { 6 11 5 }
 face 46 f { 3 4 10 }
 face 47 f { 11 10 4 5 }

 scale 5 5 5 
}

}




vehicle cosmosphere {
instance cosmosphere
offset 0 -600 0
}
 

vehicle stingship def {
name "A lightly armed starfighter"
type hover
weapon plasmaballs
ammo 50
hp 25
flying

object {  
 centre 40 0 0
 points {
 40 0 0 20 10 0 0 15 0 0 -15 0 20 -10 0
 20 0 7 0 10 8 0 -10 8
 }
 face 140 f { 5 1 0 }
 face 141 f { 6 1 5 }
 face 142 f { 6 2 1 }
 face 143 f { 6 7 5 }
 face 144 f { 7 5 4 }
 face 145 f { 3 7 4 }
 face 146 f { 4 5 0 }
 face 147 f { 0 1 2 3 4 }
 }
object {  
 points { 20 0 7 0 10 8 0 -10 8 }
 face 148 f { 2 1 0 }
 }
object {  
 points { 0 15 0 0 -15 0 0 -10 8 0 10 8 }
 face 30 f { 3 2 1 0 }
 }
object {  
 points { 0 15 0 0 20 0 20 10 0 }
 face 110 p { 0 1 2 }
 }
object {  
 points { 0 -15 0 0 -20 0 20 -10 0 }
 face 110 p { 0 1 2 }
 }
}

vehicle stingship {
instance stingship
offset 50 50 0
weapon tracer
}
vehicle stingship {
instance stingship
weapon torpedo
offset -900 900 0
}
vehicle stingship {
instance stingship
offset -500 -400 0
}
 
