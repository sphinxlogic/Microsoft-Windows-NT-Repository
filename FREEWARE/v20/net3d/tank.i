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
offset 1300 920 0
 
 
brain {
 state 0 {  
 action left
 link 1 { threatfar }
 }
 state 1 {  
 action accel
 action shoot
 link 2 { collision }
 link 0 { !threatfar }
 }
 state 2 {  
 action starttimer
 action stop
 link 4 { always }
 }
 state 4 {  
 action decel
  
 link 6 { !counting }
 }
 state 5 {  
 action stop
 link 0 { always }
 }
 state 6 {  
 action stop
 action starttimer
 link 7 { always }
 }
 state 7 {
 action accel
 link 0 { !counting }
 link 8 { maybe }  
 link 9 { always }
 }
 state 8 {
 action left
 link 7 { always }
 }
 state 9 {
 action right
 link 7 { always }
 }
 }


}
 
