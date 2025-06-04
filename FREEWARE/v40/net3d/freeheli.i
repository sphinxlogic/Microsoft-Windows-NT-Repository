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

 
vehicle freeheli {
instance heli
offset 3160 760 0
owner none
}
 
 
