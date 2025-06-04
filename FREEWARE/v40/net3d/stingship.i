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
offset 100 100 400
}
 
