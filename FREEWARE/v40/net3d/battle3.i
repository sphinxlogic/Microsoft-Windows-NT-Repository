map 16 26 200.0 1600 {
AAAAAAAAAAAAAAAA
AAAGZfgfswefhsdA
AAABBCGJADEAGJKA
AAAAABBBBBDFJKKA
AAAAAAAAABBFGHEA
AAAAAAAAAABFFHHA
AAAAAAAAAABFFGIA
AAAAAAAAAABBFFJA
AAAAAAAAAAABFFMA
AAAAAAAAAAABFFLA
AAAAAAAAAABBFFKA
AAAAAAAAAABFFMNA
AAAAAAAAAABFFJMA
AAAAAAAAAABFFKDA
AAAAAAAAABBFFLGA
AAAAAAAAABFFGMPA
AAAAAJJJJGFGLMOA
AAAAAAAABCKMMMMA
AAAAAAAABDLNMMMA
AAAAAADDLMMMMMMA
AAAAAADDMMMMNMMA
AAAAAAADJMMMNMMA
AMMMAAAADKMMMMMA
AMMMAAAABDLMMNNA
AMMMAAAAAAABBBBA
AAAAAAAAAAAAAAAA
}
ground 2 3
fadeto 0 0 10000
lookout 2200 4000 550
 
 
vehicle lighthouse {
name "The Airey's Inlet lighthouse"
offset 2200 4000 343
hp 100
type static
object {  
 points {
  
 20 0 0 15 15 0 0 20 0 -15 15 0
 -20 0 0 -15 -15 0 0 -20 0 15 -15 0
  
 15 0 150 10 10 150 0 15 150 -10 10 150
 -15 0 150 -10 -10 150 0 -15 150 10 -10 150
 }
 face 60 f { 0 8 9 1 }
 face 61 f { 1 9 10 2 }
 face 62 f { 2 10 11 3 }
 face 63 f { 3 11 12 4 }
 face 60 f { 4 12 13 5 }
 face 61 f { 5 13 14 6 }
 face 62 f { 6 14 15 7 }
 face 63 f { 7 15 8 0 }
 }
object {  
 points {
 15 0 150 10 10 150 0 15 150 -10 10 150
 -15 0 150 -10 -10 150 0 -15 150 10 -10 150
 10 0 170 7 7 170 0 10 170 -7 7 170
 -10 0 170 -7 -7 170 0 -10 170 7 -7 170
 }
 face 150 f { 0 8 9 1 }
 face 151 f { 1 9 10 2 }
 face 152 f { 2 10 11 3 }
 face 153 f { 3 11 12 4 }
 face 154 f { 4 12 13 5 }
 face 155 f { 5 13 14 6 }
 face 156 f { 6 14 15 7 }
 face 157 f { 7 15 8 0 }
 face 158 f { 15 14 13 12 11 10 9 8 }
 }
object {  
 points { 5 -20 0 -5 -20 0 -5 -40 0 5 -40 0
 5 -20 9 -5 -20 9 -5 -40 9 5 -40 9
 0 -25 14 0 -35 14 }
 face 58 f { 0 4 5 1 }
 face 59 f { 1 5 6 2 }
 face 60 f { 2 6 7 3 }
 face 61 f { 3 7 4 0 }
 face 152 f { 7 9 8 4 }
 face 151 f { 4 8 5 }
 face 150 f { 6 5 8 9 }
 face 149 f { 6 9 7 }
 }
}
 
 

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

vehicle house { instance house offset 1900 1800 0 }
vehicle house { instance house offset 1900 1300 0 }
vehicle house { instance house offset 1900 2300 0 }
vehicle house { instance house offset 1400 3600 0 }
vehicle house { instance house offset 400 4600 343 }
 
 
vehicle ship {
name "A Battleship"
type static
hp 200
offset 800 1600 0
object {  
 points {
 -40 -140 0 40 -140 0 40 120 0 -40 120 0
 -60 -170 60 60 -170 60 60 170 60 -60 170 60
 }
 face 45 f { 0 4 5 1 }
 face 46 f { 1 5 6 2 }
 face 47 f { 2 6 7 3 }
 face 48 f { 3 7 4 0 }
 face 180 f { 7 6 5 4 }
 }
}
vehicle shipupper {  
name "A Battleship"
hp 100
type static
offset 800 1600 1
object {  
 points {
 -30 -80 60 30 -80 60 30 40 60 -30 40 60
 -30 -80 80 30 -80 80 30 40 80 -30 40 80
 }
 face 50 f { 0 4 5 1 }
 face 51 f { 1 5 6 2 }
 face 52 f { 2 6 7 3 }
 face 53 f { 3 7 4 0 }
 face 54 f { 7 6 5 4 }
 }
object {  
 points {
 -20 -40 80 20 -40 80 20 10 80 -20 10 80
 -20 -40 95 20 -40 95 20 10 95 -20 10 95
 }
 face 50 f { 0 4 5 1 }
 face 51 f { 1 5 6 2 }
 face 52 f { 2 6 7 3 }
 face 53 f { 3 7 4 0 }
 face 54 f { 7 6 5 4 }
 }
}

vehicle cannon def {
name "A high-power, long range cannon"
type gunsite
weapon shell
ammo 25
hp 40
object {  
 points {
 16 1 14 16 1 12 16 -1 12 16 -1 14
 36 1 14 36 1 12 36 -1 12 36 -1 14
 }
 centre 36 0 13
 face 35 f { 0 4 5 1 }
 face 36 f { 1 5 6 2 }
 face 37 f { 2 6 7 3 }
 face 38 f { 3 7 4 0 }
 face 39 f { 7 6 5 4 }
 }
object {  
 points {
 -20 -20 0 20 -20 0 20 20 0 -20 20 0
 -16 -16 14 16 -16 14 16 16 14 -16 16 14
 }
 face 45 f { 0 4 5 1 }
 face 46 f { 1 5 6 2 }
 face 47 f { 2 6 7 3 }
 face 48 f { 3 7 4 0 }
 face 49 f { 7 6 5 4 }
 }
object {  
 points {
 -3 -3 14 3 -3 14 3 3 14 -3 3 14
 -3 -3 18 -3 3 18
 }
 face 140 f { 0 4 1 }
 face 141 f { 1 4 5 2 }
 face 142 f { 2 5 3 }
 face 143 f { 3 5 4 0 }
 }
}

vehicle cannon1 {
instance cannon
offset 800 1710 61
flying
}
vehicle cannon2 {
instance cannon
offset 800 1460 61
flying
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


vehicle ptero1 {
instance pterodactyl
type thing
offset 1200 1200 50
angle_velocity 25
max velocity 100
velocity 40
}
vehicle ptero2 {
instance pterodactyl
type thing
offset 1200 2700 400
angle_velocity -25
max velocity 100
velocity 50
}
 
vehicle heli def {
name "a helicopter"
flying
angle 180
type hover
weapon tracer
ammo 200
hp 20

 
object {  
 points { 10 0 0 0 0 7 10 0 15 20 0 15 20 0 0 }
 offset -15 -5 0
 face 170 f { 0 1 2 3 4 }
 }
object {  
 points { 10 10 0 0 10 7 10 10 15 20 10 15 20 10 0 }
 offset -15 -5 0
 face 170 f { 4 3 2 1 0 }
 }
object {  
 points { 10 0 0 0 0 7 0 10 7 10 10 0 }
 offset -15 -5 0
 face 171 f { 3 2 1 0 }
 }
object {  
 points { 0 0 7 10 0 15 10 10 15 0 10 7 }
 offset -15 -5 0
 face 172 f { 3 2 1 0 }
 }
object {  
 points { 10 0 15 20 0 15 20 10 15 10 10 15 }
 offset -15 -5 0
 face 173 f { 3 2 1 0 }
 }
object {  
 points { 20 0 15 20 0 0 20 10 0 20 10 15 }
 offset -15 -5 0
 face 174 f { 3 2 1 0 }
 }
object {  
 points { 20 0 0 10 0 0 10 10 0 20 10 0 }
 offset -15 -5 0
 face 175 f { 3 2 1 0 }
 }

object {  
 points { 20 5 5 20 5 10 30 5 10 35 5 15 35 5 5 }
 offset -15 -5 0
 face 100 p { 0 1 2 3 4 }
 }
object {  
 points { 35 5 10 35 0 10 30 5 10 }
 offset -15 -5 0
 face 103 p { 0 1 2 }
 }
object {  
 points { 35 5 10 35 10 10 30 5 10 }
 offset -15 -5 0
 face 103 p { 0 1 2 }
 }

object {  
 angle_velocity 200
 points { 0 0 0 20 0 0 20 5 0 }
 offset 0 0 17
 face 50 s { 0 1 2 }
 }
object {  
 angle_velocity 200
 points { 0 0 0 0 20 0 -5 20 0 }
 offset 0 0 17
 face 50 s { 0 1 2 }
 }
object {  
 angle_velocity 200
 points { 0 0 0 -20 0 0 -20 -5 0 }
 offset 0 0 17
 face 50 s { 0 1 2 }
 }
object {  
 angle_velocity 200
 points { 0 0 0 0 -20 0 5 -20 0 }
 offset 0 0 17
 face 50 s { 0 1 2 }
 }
object {  
 turret
 centre 0 0 -2
 points { -2 -2 0 2 -2 0 2 2 0 -2 2 0
 -2 -2 -3 2 -2 -3 2 2 -3 -2 2 -3
 }
 face 50 f { 0 1 5 4 }
 face 51 f { 1 2 6 5 }
 face 52 f { 2 3 7 6 }
 face 53 f { 3 0 4 7 }
 face 54 f { 4 5 6 7 }
 }
object {  
 turret
 centre 0 0 -2
 points { -2 0 -2 -6 0 -2 }
 face 32 l { 0 1 }
 }
}

vehicle gunship def {
name "A heavily armed helicopter"
type hover
hp 40
weapon shell
ammo 50
flying

object {  
 points {
 -15 -5 0 20 -5 0 20 5 0 -15 5 0
 -20 -5 10 20 -5 10 20 5 10 -20 5 10
 -20 -5 20 15 -5 20 15 5 20 -20 5 20
 }
 face 45 f { 0 4 8 9 5 1 }
 face 46 f { 1 5 6 2 }
 face 47 f { 2 6 10 11 7 3 }
 face 48 f { 3 7 4 0 }
 face 49 f { 7 11 8 4 }
 face 120 f { 5 9 10 6 }  
 face 50 f { 0 1 2 3 }
 face 51 f { 11 10 9 8 }
 centre 22 0 10  
 }
object {  
 points { 0 0 0 30 0 0 30 5 0 }
 offset 0 0 23 face 60 s { 0 1 2 }
 angle_velocity 200 }
object {  
 points { 0 0 0 0 30 0 -5 30 0 }
 offset 0 0 23 face 60 s { 0 1 2 }
 angle_velocity 200 }
object {  
 points { 0 0 0 -30 0 0 -30 -5 0 }
 offset 0 0 23 face 60 s { 0 1 2 }
 angle_velocity 200 }
object {  
 points { 0 0 0 0 -30 0 5 -30 0 }
 offset 0 0 23 face 60 s { 0 1 2 }
 angle_velocity 200 }
object {  
 points { 0 0 20 0 0 23 }
 face 60 l { 0 1 }
 }
object {  
 points { -50 -2 17 -20 -3 17 -20 3 17 -50 2 17
 -50 -2 20 -20 -3 20 -20 3 20 -50 2 20 }
 face 50 f { 0 4 5 1 }
 face 51 f { 2 6 7 3 }
 face 52 f { 3 7 4 0 }
 face 53 f { 0 1 2 3 }
 face 54 f { 7 6 5 4 }
 }
object {  
 points { -50 0 20 -50 0 27 -43 0 20 }
 face 60 p { 0 1 2 }
 }


}

vehicle boat def {
name "A speedboat"
type tank
weapon tracer
ammo 150
hp 20

object {  
 points { -10 -4 0 10 -4 0 10 4 0 -10 4 0
 -10 -5 5 10 -5 5 10 5 5 -10 5 5 18 0 5 }
 face 140 f { 0 4 5 1 }
 face 141 f { 1 5 8 }
 face 142 f { 1 8 2 }
 face 143 f { 8 6 2 }
 face 144 f { 2 6 7 3 }
 face 145 f { 3 7 4 0 }
 face 146 f { 7 6 8 5 4 }
 }
object {  
 points { -3 -3 5 3 -3 5 3 3 5 -3 3 5
 -3 -3 8 3 -3 8 3 3 8 -3 3 8 }
 face 50 f { 0 4 5 1 }
 face 51 f { 1 5 6 2 }
 face 52 f { 2 6 7 3 }
 face 53 f { 3 7 4 0 }
 face 54 f { 7 6 5 4 }
 }
object {  
 points { 1 0 9 4 0 9 }
 face 60 l { 0 1 }
 centre 4 0 9
 turret
 }
object {  
 points { -1 -1 8 1 -1 8 1 1 8 -1 1 8
 -1 -1 9 1 -1 9 1 1 9 -1 1 9 }
 face 110 f { 0 4 5 1 }
 face 111 f { 1 5 6 2 }
 face 112 f { 2 6 7 3 }
 face 113 f { 3 7 4 0 }
 face 114 f { 7 6 5 4 }
 turret
 }
}

 
vehicle helicopter1 {
instance heli
offset 1800 1150 5
weapon tracer
}
 
vehicle helicopter2 {
instance heli
offset 2400 4950 5
weapon chicken
}
 
vehicle speedboat {
instance boat
offset 200 1500 0
}
 
vehicle gunship {
instance gunship
offset 1200 1200 300
}
 
 
