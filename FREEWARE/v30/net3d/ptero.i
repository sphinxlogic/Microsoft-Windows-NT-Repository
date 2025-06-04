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


 
vehicle pterodactyl {
instance pterodactyl
offset 1000 1000 200
}
 
