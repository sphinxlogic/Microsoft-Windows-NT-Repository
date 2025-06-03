vehicle wall1 {  
name "A Wall Section"
type static
hp 10
object {
 points {
 -5 -1 0 -5 1 0 5 1 0 5 -1 0
 -5 -1 8 -5 1 8 5 1 8 5 -1 8
 }
 face 185 f { 0 1 5 4 }
 face 186 f { 1 2 6 5 }
 face 187 f { 2 3 7 6 }
 face 188 f { 3 0 4 7 }
 face 189 f { 4 5 6 7 }
 }
}

vehicle wall2 {  
name "A Wall Section"
type static
hp 10
object {
 points {
 -1 -5 0 1 -5 0 1 5 0 -1 5 0
 -1 -5 8 1 -5 8 1 5 8 -1 5 8
 }
 face 185 f { 0 4 5 1 }
 face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 }
 face 188 f { 3 7 4 0 }
 face 189 f { 7 6 5 4 }
 }
}

vehicle wall3 {  
name "A Wall section"
type static
hp 12
object {
 points { 1 -1 0 1 5 0 -1 5 0 -1 1 0 1 -1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 7 6 5 4 }
 rotate 180
 }
object {
 points { -5 -1 0 1 -1 0 -1 1 0 -5 1 0 -5 -1 8 1 -1 8 -1 1 8 -5 1 8 }
 face 191 f { 0 4 5 1 } face 190 f { 2 6 7 3 }
 face 189 f { 3 7 4 0 } face 188 f { 7 6 5 4 }
 rotate 180
 }
}

vehicle wall4 {  
name "A Wall section"
type static
hp 12
object {
 points { 1 -1 0 1 5 0 -1 5 0 -1 1 0 1 -1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 7 6 5 4 }
 rotate 270
 }
object {
 points { -5 -1 0 1 -1 0 -1 1 0 -5 1 0 -5 -1 8 1 -1 8 -1 1 8 -5 1 8 }
 face 191 f { 0 4 5 1 } face 190 f { 2 6 7 3 }
 face 189 f { 3 7 4 0 } face 188 f { 7 6 5 4 }
 rotate 270
 }
}

vehicle wall5 {  
name "A Wall section"
type static
hp 12
object {
 points { 1 -1 0 1 5 0 -1 5 0 -1 1 0 1 -1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 7 6 5 4 }
 }
object {
 points { -5 -1 0 1 -1 0 -1 1 0 -5 1 0 -5 -1 8 1 -1 8 -1 1 8 -5 1 8 }
 face 191 f { 0 4 5 1 } face 190 f { 2 6 7 3 }
 face 189 f { 3 7 4 0 } face 188 f { 7 6 5 4 }
 }
}

vehicle wall6 {  
name "A Wall section"
type static
hp 12
object {
 points { 1 -1 0 1 5 0 -1 5 0 -1 1 0 1 -1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 7 6 5 4 }
 rotate 90
 }
object {
 points { -5 -1 0 1 -1 0 -1 1 0 -5 1 0 -5 -1 8 1 -1 8 -1 1 8 -5 1 8 }
 face 191 f { 0 4 5 1 } face 190 f { 2 6 7 3 }
 face 189 f { 3 7 4 0 } face 188 f { 7 6 5 4 }
 rotate 90
 }
}

vehicle wall7 {  
name "A Wall section"
type static
hp 15
object {
 points { -1 -5 0 1 -5 0 1 5 0 -1 5 0 -1 -5 8 1 -5 8 1 5 8 -1 5 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 3 7 4 0 }
 face 189 f { 7 6 5 4 }
 rotate 270
 }
object {
 points { 1 -1 0 5 -1 0 5 1 0 1 1 0 1 -1 8 5 -1 8 5 1 8 1 1 8 }
 face 186 f { 0 4 5 1 } face 187 f { 1 5 6 2 }
 face 188 f { 2 6 7 3 } face 189 f { 7 6 5 4 }
 rotate 270
 }
}

vehicle wall8 {  
name "A Wall section"
type static
hp 15
object {
 points { -1 -5 0 1 -5 0 1 5 0 -1 5 0 -1 -5 8 1 -5 8 1 5 8 -1 5 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 3 7 4 0 }
 face 189 f { 7 6 5 4 }
 }
object {
 points { 1 -1 0 5 -1 0 5 1 0 1 1 0 1 -1 8 5 -1 8 5 1 8 1 1 8 }
 face 186 f { 0 4 5 1 } face 187 f { 1 5 6 2 }
 face 188 f { 2 6 7 3 } face 189 f { 7 6 5 4 }
 }
}

vehicle wall9 {  
name "A Wall section"
type static
hp 15
object {
 points { -1 -5 0 1 -5 0 1 5 0 -1 5 0 -1 -5 8 1 -5 8 1 5 8 -1 5 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 3 7 4 0 }
 face 189 f { 7 6 5 4 }
 rotate 90
 }
object {
 points { 1 -1 0 5 -1 0 5 1 0 1 1 0 1 -1 8 5 -1 8 5 1 8 1 1 8 }
 face 186 f { 0 4 5 1 } face 187 f { 1 5 6 2 }
 face 188 f { 2 6 7 3 } face 189 f { 7 6 5 4 }
 rotate 90
 }
}

vehicle wall10 {  
name "A Wall section"
type static
hp 15
object {
 points { -1 -5 0 1 -5 0 1 5 0 -1 5 0 -1 -5 8 1 -5 8 1 5 8 -1 5 8 }
 face 185 f { 0 4 5 1 } face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 } face 188 f { 3 7 4 0 }
 face 189 f { 7 6 5 4 }
 rotate 180
 }
object {
 points { 1 -1 0 5 -1 0 5 1 0 1 1 0 1 -1 8 5 -1 8 5 1 8 1 1 8 }
 face 186 f { 0 4 5 1 } face 187 f { 1 5 6 2 }
 face 188 f { 2 6 7 3 } face 189 f { 7 6 5 4 }
 rotate 180
 }
}

vehicle wall11 {  
name "A Wall section"
type static
hp 17
object {
 points { 0 0 0 1 1 0 1 5 0 -1 5 0 -1 1 0
 0 0 8 1 1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 1 6 7 2 } face 186 f { 2 7 8 3 }
 face 187 f { 3 8 9 4 } face 188 f { 9 8 7 6 5 }
 }
object {
 points { 0 0 0 1 1 0 1 5 0 -1 5 0 -1 1 0
 0 0 8 1 1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 1 6 7 2 } face 186 f { 2 7 8 3 }
 face 187 f { 3 8 9 4 } face 188 f { 9 8 7 6 5 }
 rotate 90
 }
object {
 points { 0 0 0 1 1 0 1 5 0 -1 5 0 -1 1 0
 0 0 8 1 1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 1 6 7 2 } face 186 f { 2 7 8 3 }
 face 187 f { 3 8 9 4 } face 188 f { 9 8 7 6 5 }
 rotate 180
 }
object {
 points { 0 0 0 1 1 0 1 5 0 -1 5 0 -1 1 0
 0 0 8 1 1 8 1 5 8 -1 5 8 -1 1 8 }
 face 185 f { 1 6 7 2 } face 186 f { 2 7 8 3 }
 face 187 f { 3 8 9 4 } face 188 f { 9 8 7 6 5 }
 rotate 270
 }
}

vehicle wall12 {  
name "Mine"
type mine
weapon none
hp 10
object {
 points {
 -3 -3 2 3 -3 2 3 3 2 -3 3 2
 -4 -3 0 -3 -4 0 3 -4 0 4 -3 0
 4 3 0 3 4 0 -3 4 0 -4 3 0
 }
 face 35 f { 3 2 1 0 }
 face 36 f { 4 0 5 }
 face 37 f { 6 1 7 }
 face 38 f { 8 2 9 }
 face 39 f { 10 3 11 }
 face 40 f { 0 1 6 5 }
 face 41 f { 1 2 8 7 }
 face 42 f { 2 3 10 9 }
 face 43 f { 3 0 4 11 }
 }
}

vehicle wall13 {  
name "Autocannon"
type gunsite
weapon tracer
ammo 50
angle 90
hp 15
object {  
 points { 
 4 6 8 3.5 6 7 4.5 6 7
 4 12 8 3.5 12 7 4.5 12 7
 }
 offset -4 -4 0
 centre 0 4 8
 face 55 f { 0 3 5 2 }
 face 56 f { 1 4 3 0 }
 face 57 f { 1 2 5 4 }
 face 30 f { 3 4 5 }
 }
object {  
 points {
 0 0 0 8 0 0 8 8 0 0 8 0
 2 2 8 6 2 8 6 6 8 2 6 8
 }
 offset -4 -4 0
 face 40 f { 0 4 5 1 }
 face 41 f { 1 5 6 2 }
 face 42 f { 2 6 7 3 }
 face 43 f { 3 7 4 0 }
 face 44 f { 7 6 5 4 }
 }
 
brain {
 state 0 {
 action left
 link 1 { threatfar }
 link 2 { ammolow }
 }
 state 1 {
 action shoot
 link 0 { !threatfar }
 link 2 { ammolow }
 }
 state 2 {
 action detonate
 }
 }
}

 
vehicle tracer {
name "a high velocity tracer round"
type bullet
flying
hp 3
object {
 points { 0 0 0 5 0 0 }
 face 63 l { 0 1 }
 }
}

vehicle shell {
name "a high explosive tank shell"
type bullet
flying
hp 10
object {
 points { 3 0 0 0 0 1 0 1 0 0 0 -1 0 -1 0 }
 face 40 f { 4 3 2 1 }
 face 41 f { 1 2 0 }
 face 42 f { 2 3 0 }
 face 43 f { 3 4 0 }
 face 44 f { 4 1 0 }
 }
}

vehicle bomb {
name "a free-falling bomb"
type bullet
hp 10
object {
 points { 0 0 0 0 0 2 }
 face 32 c { 0 1 }
 }
}

vehicle bullet {
name "a light 9mm bullet"
type bullet
flying
hp 1
object {
 points { 0 0 0 }
 face 63 d { 0 }
 }
}

vehicle flame {
name "a cloud of flame"
type bullet
flying
hp 7
object {
 points {
 4 0 0 4 0 2
 -4 0 0 -4 0 2
 0 4 0 0 4 2
 0 -4 0 0 -4 2
 0 0 4 0 0 6
 0 0 -4 0 0 -2
 }
 face 25 c { 0 1 }
 face 26 c { 2 3 }
 face 27 c { 4 5 }
 face 28 c { 6 7 }
 face 29 c { 8 9 }
 face 30 c { 10 11 }
 }
spin random 20 20 20
}

vehicle missile {
name "an air-to-air missile"
type missile
flying
hp 15
object {
 points { 6 0 0 0 0 0 0 0 1 0 1 0 0 0 -1 0 -1 0 }
 face 150 p { 0 1 2 }
 face 151 p { 0 1 3 }
 face 151 p { 0 1 4 }
 face 151 p { 0 1 5 }
 }
}

vehicle chicken {
name "an armour-peircing chicken"
flying
type bullet
hp 6

object {  
 centre 0 4.5 0
 counter
 points { 0 5 0 -0.5 4 0 0 4 0.5 0.5 4 0 0 4 -0.5 }
 rotate -90
 face 191 f { 1 2 0 }
 face 190 f { 2 3 0 }
 face 188 f { 4 1 0 }
 face 189 f { 3 4 0 }
}
object {  
 centre 0 2.5 0
 counter
 points { 
   -0.5 4 0.5 0.5 4 0.5 0.5 4 -0.5 -0.5 4 -0.5
   -1 3.5 0.5 -0.5 3.5 1 0.5 3.5 1 1 3.5 0.5 
 1 3.5 -0.5 0.5 3.5 -1 -0.5 3.5 -1 -1 3.5 -0.5
   -1 2.5 0.5 -0.5 2.5 1 0.5 2.5 1 1 2.5 0.5 
 1 2.5 -0.5 0.5 2.5 -1 -0.5 2.5 -1 -1 2.5 -0.5
   -0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 -0.5 -0.5 0.5 -0.5
 }
 rotate -90
 face 60 f { 0 1 2 3 }

 face 63 f { 0 4 5 }
 face 62 f { 0 5 6 1 }
 face 61 f { 1 6 7 }
 face 60 f { 1 7 8 2 }
 face 59 f { 2 8 9 }
 face 60 f { 2 9 10 3 }
 face 61 f { 3 10 11 }
 face 62 f { 3 11 4 0 }

 face 62 f { 5 4 12 13 }
 face 61 f { 6 5 13 14 }
 face 60 f { 7 6 14 15 } 
 face 59 f { 8 7 15 16 }
 face 58 f { 9 8 16 17 }
 face 59 f { 10 9 17 18 }
 face 60 f { 11 10 18 19 } 
 face 61 f { 4 11 19 12 }

 face 61 f { 13 12 20 }
 face 60 f { 14 13 20 21 }
 face 59 f { 15 14 21 }
 face 58 f { 16 15 21 22 }
 face 57 f { 17 16 22 }
 face 58 f { 18 17 22 23 }
 face 59 f { 19 18 23 }
 face 60 f { 12 19 23 20 }

}
object {  
 centre 0 -3 0
 counter
 points { 
 -0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 -0.5 -0.5 0.5 -0.5

 -1.5 0 0.5 -1 0 1 1 0 1 1.5 0 0.5
 1.5 0 -0.5 0.5 0 -1.5 -0.5 0 -1.5 -1.5 0 -0.5

 -1.5 -6 0.5 -1 -5.5 1 1 -5.5 1 1.5 -6 0.5
 1.5 -5 -0.5 0.5 -4.5 -1.5 -0.5 -4.5 -1.5 -1.5 -5 -0.5
 }
 rotate -90
 face 60 f { 2 1 7 8 }
 face 59 f { 2 8 9 }
 face 60 f { 3 2 9 10 }
 face 61 f { 3 10 11 }
 face 62 f { 0 3 11 4 }
 face 63 f { 0 4 5 }
 face 62 f { 1 0 5 6 }
 face 61 f { 1 6 7 }

 face 62 f { 5 4 12 13 }
 face 61 f { 6 5 13 14 }
 face 60 f { 7 6 14 15 } 
 face 59 f { 8 7 15 16 }
 face 58 f { 9 8 16 17 }
 face 59 f { 10 9 17 18 }
 face 60 f { 11 10 18 19 } 
 face 61 f { 4 11 19 12 }

 face 60 f { 15 14 13 12 }
 face 58 f { 12 19 16 15 }
 face 57 f { 19 18 17 16 }

}
object {  
 centre 0 -3 1.25
 counter
 points {
   0 -4 1
 -1 -5.5 1 0 -6.5 2 1 -5.5 1
 }
 rotate -90
 face 63 f { 1 2 0 }
 face 61 f { 2 3 0 }
 face 59 f { 3 2 1 }
}

object {  
 centre 0 3 2.25
 counter
 points { 
 0 3.5 1
 -0.5 2.5 1
 0 2.5 1.25
 0.5 2.5 1
 }
 rotate -90
 face 152 f { 0 1 2 }
 face 151 f { 2 3 0 }
 face 151 f { 3 2 1 }

}
}
vehicle torpedo {
type missile
name "A lethal ship to ship guided missile"
flying
hp 25
object {
 points { 0 0 1 0 1 0 0 0 -1 0 -1 0
 5 0 1 5 1 0 5 0 -1 5 -1 0
 10 0 0 }
 face 110 f { 0 1 5 4 }
 face 111 f { 1 2 6 5 }
 face 112 f { 2 3 7 6 }
 face 113 f { 3 0 4 7 }
 face 114 f { 4 5 8 }
 face 115 f { 5 6 8 }
 face 116 f { 6 7 8 }
 face 117 f { 7 4 8 }
 face 29 f { 3 2 1 0 }
 }
}
vehicle plasmaballs {
type bullet
name "A chain of plasma balls"
flying
hp 10
object { points { 0 0 0 0 0 .5 } face 63 c { 0 1 } }
object { points { 1 0 0 1 0 .5 } face 127 c { 0 1 } }
object { points { 2 0 0 2 0 .5 } face 63 c { 0 1 } }
object { points { 3 0 0 3 0 .5 } face 127 c { 0 1 } }
object { points { 4 0 0 4 0 .5 } face 63 c { 0 1 } }
}


 
 
vehicle fireball25 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 25 c { 0 1 } }
}
vehicle fireball26 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 26 c { 0 1 } }
}
vehicle fireball27 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 27 c { 0 1 } }
}
vehicle fireball28 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 28 c { 0 1 } }
}
vehicle fireball29 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 29 c { 0 1 } }
}
vehicle fireball30 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 30 c { 0 1 } }
}
vehicle fireball31 {
type shrapnel
name "a flaming ball of plasma"
hp 1
object { points { 0 0 0 0 0 3 } face 31 c { 0 1 } }
}


 
vehicle player {
name "A player without a vehicle"
type fish
weapon bullet
hp 3
ammo 30
object {  
 centre 3 0 0
 points { 0 0 0 3 1 0 3 0 1 3 -1 0 3 0 -1 6 0 0 }
 offset 0 0 2
 rotate 180
 face 150 f { 0 1 2 }
 face 151 f { 0 2 3 }
 face 152 f { 0 3 4 }
 face 153 f { 0 4 1 }
 face 154 f { 1 5 2 }
 face 155 f { 2 5 3 }
 face 156 f { 3 5 4 }
 face 157 f { 4 5 1 }
 }
object {  
 centre 7 0 0
 points { 6 0 0 8 0 2 8 0 -2 }
 offset 0 0 2
 rotate 180
 face 158 p { 0 1 2 }
 }
}

vehicle tree {
name "A useful tree"
type tree
hp 0
max treeheight 40
object {  
 at 0 0 0
 points {
 0 0 0 1 0 0 1 1 0 0 1 0
 0 0 10 1 0 10 1 1 10 0 1 10
 }
 face 185 f { 0 4 5 1 }
 face 186 f { 1 5 6 2 }
 face 187 f { 2 6 7 3 }
 face 188 f { 3 7 4 0 }
 }
object {  
 at 0 0 0
 points {
 -3 -3 10 4 -3 10 4 4 10 -3 4 10 .5 .5 22
 }
 face 90 f { 0 4 1 }
 face 91 f { 1 4 2 }
 face 92 f { 2 4 3 }
 face 93 f { 3 4 0 }
 face 80 f { 0 1 2 3 }
 }
}

