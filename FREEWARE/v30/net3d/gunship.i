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

 
vehicle gunship {
instance gunship
offset 0 0 300
}
 
