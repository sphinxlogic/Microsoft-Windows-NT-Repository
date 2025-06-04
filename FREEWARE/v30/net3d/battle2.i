map 12 12 200 1000 {
AAAAAAAAAAAA
AZAAAAAAAAWA
AAAAANNAAAAA
AAAAAZZAAAAA
AAAAAnnAAAAA
AANZnppnZNAA
AANZnppnZNAA
AAAAAnnAAAAA
AAAAAZZAAAAA
AAAAANNAAAAA
AXAAAAAAAAYA
AAAAAAAAAAAA
}
 
ground 2 2
 
sky 140
stars 100
 
 

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


vehicle HOUSE { instance house offset 0 600 0 }
vehicle HOUSE { instance house offset 0 650 0 }
vehicle HOUSE { instance house offset -80 620 0 }
vehicle HOUSE { instance house offset 1000 0 0 }
vehicle HOUSE { instance house offset 2400 1000 0 }
vehicle HOUSE { instance house offset 1000 2200 0 }
 
 
vehicle trigpoint {
name "Hilltop marker"
type static
hp 10
offset 1200 1200 820
object {
 points { 10 0 0 0 10 0 -10 0 0 0 -10 0 0 0 20 0 0 40 }
 face 159 l { 0 4 }
 face 159 l { 1 4 }
 face 159 l { 2 4 }
 face 159 l { 3 4 }
 face 155 l { 4 5 }
 }
object {
 points { 0 0 32 0 0 40 8 0 40 8 0 32 }
 face 220 p { 0 1 2 3 }
 }
}
 
 
vehicle tree { instance tree offset 2500 200 0 }
vehicle tree { instance tree offset 2540 230 0 }
vehicle tree { instance tree offset 2570 210 0 }
vehicle tree { instance tree offset 2510 270 0 }
 
 

vehicle eye def {
name "flying eye"
angle 90
flying
type bird
weapon tracer
max velocity 200

 

object {
 centre 0 -12 -18
 counter
 points {
 0 -5.0000 0
 0 5.0000 0
 0.0000 4.9240 0.8682
 0.5103 4.9240 0.7024
 0.8257 4.9240 0.2683
 0.8257 4.9240 -0.2683
 0.5103 4.9240 -0.7024
 0.0000 4.9240 -0.8682
 -0.5103 4.9240 -0.7024
 -0.8257 4.9240 -0.2683
 -0.8257 4.9240 0.2683
 -0.5103 4.9240 0.7024
 0.0000 3.2139 3.8302
 2.2513 3.2139 3.0987
 3.6428 3.2139 1.1836
 3.6428 3.2139 -1.1836
 2.2513 3.2139 -3.0987
 0.0000 3.2139 -3.8302
 -2.2513 3.2139 -3.0987
 -3.6428 3.2139 -1.1836
 -3.6428 3.2139 1.1836
 -2.2513 3.2139 3.0987
 0.0000 0.0000 5.0000
 2.9389 0.0000 4.0451
 4.7553 0.0000 1.5451
 4.7553 0.0000 -1.5451
 2.9389 0.0000 -4.0451
 0.0000 0.0000 -5.0000
 -2.9389 0.0000 -4.0451
 -4.7553 0.0000 -1.5451
 -4.7553 0.0000 1.5451
 -2.9389 0.0000 4.0451
 0.0000 -3.2139 3.8302
 2.2513 -3.2139 3.0987
 3.6428 -3.2139 1.1836
 3.6428 -3.2139 -1.1836
 2.2513 -3.2139 -3.0987
 0.0000 -3.2139 -3.8302
 -2.2513 -3.2139 -3.0987
 -3.6428 -3.2139 -1.1836
 -3.6428 -3.2139 1.1836
 -2.2513 -3.2139 3.0987
 0.0000 -4.9240 0.8682
 0.5103 -4.9240 0.7024
 0.8257 -4.9240 0.2683
 0.8257 -4.9240 -0.2683
 0.5103 -4.9240 -0.7024
 0.0000 -4.9240 -0.8682
 -0.5103 -4.9240 -0.7024
 -0.8257 -4.9240 -0.2683
 -0.8257 -4.9240 0.2683
 -0.5103 -4.9240 0.7024

 }

  

 face 117.0000 f { 3.0000 2.0000 12.0000 13.0000 }

 face 62.0000 f { 13.0000 12.0000 22.0000 23.0000 }

 face 61.0000 f { 23.0000 22.0000 32.0000 33.0000 }

 face 60.0000 f { 33.0000 32.0000 42.0000 43.0000 }
 face 55.0000 f { 43.0000 42.0000 0 }

 face 116.0000 f { 4.0000 3.0000 13.0000 14.0000 }

 face 61.0000 f { 14.0000 13.0000 23.0000 24.0000 }

 face 60.0000 f { 24.0000 23.0000 33.0000 34.0000 }

 face 59.0000 f { 34.0000 33.0000 43.0000 44.0000 }
 face 54.0000 f { 44.0000 43.0000 0 }

 face 115.0000 f { 5.0000 4.0000 14.0000 15.0000 }

 face 60.0000 f { 15.0000 14.0000 24.0000 25.0000 }

 face 59.0000 f { 25.0000 24.0000 34.0000 35.0000 }

 face 58.0000 f { 35.0000 34.0000 44.0000 45.0000 }
 face 53.0000 f { 45.0000 44.0000 0 }

 face 114.0000 f { 6.0000 5.0000 15.0000 16.0000 }

 face 59.0000 f { 16.0000 15.0000 25.0000 26.0000 }

 face 58.0000 f { 26.0000 25.0000 35.0000 36.0000 }

 face 57.0000 f { 36.0000 35.0000 45.0000 46.0000 }
 face 52.0000 f { 46.0000 45.0000 0 }

 face 113.0000 f { 7.0000 6.0000 16.0000 17.0000 }

 face 58.0000 f { 17.0000 16.0000 26.0000 27.0000 }

 face 57.0000 f { 27.0000 26.0000 36.0000 37.0000 }

 face 56.0000 f { 37.0000 36.0000 46.0000 47.0000 }
 face 51.0000 f { 47.0000 46.0000 0 }

 face 112.0000 f { 8.0000 7.0000 17.0000 18.0000 }

 face 57.0000 f { 18.0000 17.0000 27.0000 28.0000 }

 face 56.0000 f { 28.0000 27.0000 37.0000 38.0000 }

 face 55.0000 f { 38.0000 37.0000 47.0000 48.0000 }
 face 50.0000 f { 48.0000 47.0000 0 }

 face 113.0000 f { 9.0000 8.0000 18.0000 19.0000 }

 face 58.0000 f { 19.0000 18.0000 28.0000 29.0000 }

 face 57.0000 f { 29.0000 28.0000 38.0000 39.0000 }

 face 56.0000 f { 39.0000 38.0000 48.0000 49.0000 }
 face 51.0000 f { 49.0000 48.0000 0 }

 face 114.0000 f { 10.0000 9.0000 19.0000 20.0000 }

 face 59.0000 f { 20.0000 19.0000 29.0000 30.0000 }

 face 58.0000 f { 30.0000 29.0000 39.0000 40.0000 }

 face 57.0000 f { 40.0000 39.0000 49.0000 50.0000 }
 face 52.0000 f { 50.0000 49.0000 0 }

 face 115.0000 f { 11.0000 10.0000 20.0000 21.0000 }

 face 60.0000 f { 21.0000 20.0000 30.0000 31.0000 }

 face 59.0000 f { 31.0000 30.0000 40.0000 41.0000 }

 face 58.0000 f { 41.0000 40.0000 50.0000 51.0000 }
 face 53.0000 f { 51.0000 50.0000 0 }

 face 116.0000 f { 2.0000 11.0000 21.0000 12.0000 }

 face 61.0000 f { 12.0000 21.0000 31.0000 22.0000 }

 face 60.0000 f { 22.0000 31.0000 41.0000 32.0000 }

 face 59.0000 f { 32.0000 41.0000 51.0000 42.0000 }
 face 54.0000 f { 42.0000 51.0000 0 }

}

 


object {
 centre -10.6000 -3.8000 0.9000
 counter
 points { 
 -5.0000 0.0000 0.0000 -10.0000 3.0000 5.0000 -20.0000 -10.0000 -5.0000 -10.0000 -5.0000 3.0000 -8.0000 -7.0000 1.5000
 }

 face 43 p { 0 1 4 }
 face 43 p { 1 3 4 }
 face 45 p { 1 2 3 }
}

object {
 centre 10.6000 -3.8000 0.9000
 counter
 points { 
 5.0000 0.0000 0.0000 10.0000 3.0000 5.0000 20.0000 -10.0000 -5.0000 10.0000 -5.0000 3.0000 8.0000 -7.0000 1.5000
 }

 face 43 p { 0 1 4 }
 face 43 p { 1 3 4 }
 face 45 p { 1 2 3 }
}


object {
 centre 0 3 5
 counter
 points {
 -2 0 -5 2 0 -5
 -1.8 -2 -7 1.8 -2 -7
 -2 -3.5355 -3.5355 2 -3.5355 -3.5355
 -1.8 -5 -5 1.8 -5 -5
 }
 face 179 f { 0 1 3 2 }
 face 181 f { 6 7 5 4 }
 face 180 f { 0 2 6 4 }
 face 180 f { 5 7 3 1 }
}

object {
 centre 0 3 5
 counter
 points {
 -1.8 -2 -7 1.8 -2 -7
 -1.6 -3 -9 1.6 -3 -9
 -1.8 -5 -5 1.8 -5 -5
 -1.6 -6 -7 1.6 -6 -7
 }
 face 39 f { 0 1 3 2 }
 face 41 f { 6 7 5 4 }
 face 40 f { 0 2 6 4 }
 face 40 f { 5 7 3 1 }
}

object {
 centre 0 3 5
 counter
 points {
 -1.6 -3 -9 1.6 -3 -9
 -1.4 -4 -11 1.4 -4 -11
 -1.6 -6 -7 1.6 -6 -7
 -1.4 -7 -10 1.4 -7 -10
 }
 face 169 f { 0 1 3 2 }
 face 171 f { 6 7 5 4 }
 face 170 f { 0 2 6 4 }
 face 170 f { 5 7 3 1 }
}

object {
 centre 0 3 5
 counter
 points {
 -1.4 -4 -11 1.4 -4 -11
 -1.3 -6 -14 1.3 -6 -14
 -1.4 -7 -10 1.4 -7 -10
 -1.2 -8 -13 1.2 -8 -13
 }
 face 39 f { 0 1 3 2 }
 face 41 f { 6 7 5 4 }
 face 40 f { 0 2 6 4 }
 face 40 f { 5 7 3 1 }
}

object {
 centre 0 3 5
 counter
 points {
 -1.3 -6 -14 1.3 -6 -14
 -1 -8 -16 1 -8 -16
 -1.2 -8 -13 1.2 -8 -13
 -0.7 -9 -15 0.7 -9 -15
 }
 face 174 f { 0 1 3 2 }
 face 176 f { 6 7 5 4 }
 face 175 f { 0 2 6 4 }
 face 175 f { 5 7 3 1 }
}

object {
 centre 0 3 5
 counter
 points {
 -1 -8 -16 1 -8 -16
 -0.7 -9 -15 0.7 -9 -15
 0 -12 -18
 }
 face 44 f { 0 1 4 }
 face 46 f { 3 2 4 }
 face 45 f { 0 4 2 }
 face 45 f { 3 4 1 }
}

}

vehicle eyething {
instance eye
type thing
offset 2200 2200 500
angle_velocity -5
velocity 10
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

vehicle tank def {
name "A simple tank"
type tank
weapon shell
hp 50
ammo 60

object {  
 at 0 0 0
 centre 0 0 2
 points {
 -10 -4 0 10 -4 0 10 4 0 -10 4 0
 -10 -4 4 6 -4 4 6 4 4 -10 4 4
 }
 scale 2 2 2
 face 32 f { 0 1 2 3 }
 face 50 f { 7 6 5 4 }
 face 45 f { 0 4 5 1 }
 face 45 f { 1 5 6 2 }
 face 45 f { 2 6 7 3 }
 face 45 f { 3 7 4 0 }
 }
object {  
 turret
 at 0 0 8
 centre 0 0 1.5
 points {
 -4 -4 0 4 -4 0 4 4 0 -4 4 0
 -4 -4 3 4 -4 3 4 4 3 -4 4 3
 }
 scale 2 2 2
 face 64 f { 0 1 2 3 }
 face 85 f { 7 6 5 4 }
 face 77 f { 0 4 5 1 }
 face 77 f { 1 5 6 2 }
 face 77 f { 2 6 7 3 }
 face 77 f { 3 7 4 0 }
 }
object {  
 turret
 at 0 0 10
 centre 7 0 0
 points {
 4 -0.5 0 10 -0.5 0 10 0.5 0 4 0.5 0
 4 -0.5 1 10 -0.5 1 10 0.5 1 4 0.5 1
 }
 scale 2 2 2
 face 40 f { 0 1 2 3 }
 face 50 f { 7 6 5 4 }
 face 45 f { 0 4 5 1 }
 face 32 f { 1 5 6 2 }
 face 45 f { 2 6 7 3 }
 face 32 f { 3 7 4 0 }
 } 
}

 
vehicle tank {
instance tank
offset 1700 1700 0
}
 
vehicle helicopter1 {
instance heli
weapon tracer
offset 200 200 500
}
 
vehicle helicopter2 {
instance heli
offset 2000 200 600
}
 
