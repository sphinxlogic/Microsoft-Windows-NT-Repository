vehicle glassfish def {
name "a flying glass fish"
flying
max velocity 200
angle 90
object {  
 counter
 points {
  
 -10 0 0 -10 0 2 -8 0 6 -6 0 8 -2 0 10 
 2 0 10 6 0 8 8 0 6 10 0 2 
 10 0 0 10 0 -2 8 0 -6 6 0 -8 2 0 -10 
 -2 0 -10 -6 0 -8 -8 0 -6 -10 0 -2
   
 -8 6 2 -4 10 4 4 10 4 8 6 2 
 8 6 -2 4 10 -4 -4 10 -4 -8 6 -2
  
 -8 -6 0 -8 -6 2 -6 -6 6 -2 -6 8 2 -6 8 6 -6 6 8 -6 2 8 -6 0
  
 -2 -10 0 0 -10 4 2 -10 0 0 -10 -4
 }

  
 face 4 g { 0 1 18 }
 face 4 g { 1 2 18 }
 face 4 g { 2 3 19 18 }
 face 4 g { 3 4 19 }
 face 4 g { 4 5 20 19 }
 face 4 g { 5 6 20 }
 face 4 g { 6 7 21 20 }
 face 4 g { 7 8 21 }
 face 4 g { 8 9 21 }
  
 face 4 g { 0 18 19 20 21 9 }
  
 face 4 g { 26 27 1 0 }
 face 4 g { 27 28 2 1 }
 face 4 g { 28 3 2 }
 face 4 g { 28 29 4 3 }
 face 4 g { 29 30 5 4 }
 face 4 g { 30 31 6 5 }
 face 4 g { 31 7 6 }
 face 4 g { 31 32 8 7 }
 face 4 g { 32 33 9 8 }
  
 face 4 g { 26 34 27 }
 face 4 g { 27 34 28 }
 face 4 g { 34 35 28 }
 face 4 g { 28 35 29 }
 face 4 g { 29 35 30 }
 face 4 g { 30 35 31 }
 face 4 g { 31 35 36 }
 face 4 g { 32 31 36 }
 face 4 g { 32 36 33 }
}

object {  
 counter
 points {
  
 -10 0 0 -10 0 2 -8 0 6 -6 0 8 -2 0 10 
 2 0 10 6 0 8 8 0 6 10 0 2 
 10 0 0 10 0 -2 8 0 -6 6 0 -8 2 0 -10 
 -2 0 -10 -6 0 -8 -8 0 -6 -10 0 -2
   
 -8 6 2 -4 10 4 4 10 4 8 6 2 
 8 6 -2 4 10 -4 -4 10 -4 -8 6 -2
  
 8 -6 0 8 -6 -2 6 -6 -6 2 -6 -8 -2 -6 -8 -6 -6 -6 -8 -6 -2 -8 -6 0
  
 -2 -10 0 0 -10 4 2 -10 0 0 -10 -4
 }

  
 face 3 g { 9 10 22 }
 face 3 g { 10 11 22 }
 face 3 g { 11 12 23 22 }
 face 3 g { 12 13 23 }
 face 3 g { 13 14 24 23 }
 face 3 g { 14 15 24 }
 face 3 g { 15 16 25 24 }
 face 3 g { 16 17 25 }
 face 3 g { 17 0 25 }
  
 face 3 g { 0 9 22 23 24 25 }
  
 face 3 g { 9 26 27 10 }
 face 3 g { 10 27 28 11 }
 face 3 g { 11 28 12 }
 face 3 g { 12 28 29 13 }
 face 3 g { 13 29 30 14 }
 face 3 g { 14 30 31 15 }
 face 3 g { 15 31 16 }
 face 3 g { 16 31 32 17 }
 face 3 g { 17 32 33 0 }
  
 face 3 g { 26 36 27 }
 face 3 g { 27 36 28 }
 face 3 g { 28 36 37 }
 face 3 g { 28 37 29 }
 face 3 g { 29 37 30 }
 face 3 g { 30 37 31 }
 face 3 g { 37 34 31 }
 face 3 g { 34 32 31 }
 face 3 g { 34 33 32 }
}

object {  
 counter
 points {
 -10 0 0 -16 -6 0 -8 -6 2 -8 -6 -2
 }

 face 2 g { 0 1 2 }
 face 2 g { 3 1 0 }
 face 2 g { 3 2 0 }
}
object {  
 counter
 points {
 10 0 0 16 -6 0 8 -6 2 8 -6 -2
 }

 face 2 g { 2 1 0 }
 face 2 g { 0 1 3 }
 face 2 g { 0 2 3 }
}
object {  
 counter
 points {
 0 0 10 0 -6 16 -2 -6 8 2 -6 8 
 }

 face 1 g { 0 2 1 }
 face 1 g { 0 1 3 }
 face 1 g { 2 3 1 }
}

object {  
 counter
 points {
 -2 -10 0 0 -10 4 2 -10 0 0 -10 -4
 -4 -14 0 0 -14 8 4 -14 0 0 -14 -8
 }
 face 6 g { 4 5 1 0 }
 face 6 g { 5 6 2 1 }
 face 6 g { 6 7 3 2 }
 face 6 g { 7 4 0 3 }
 face 6 g { 7 6 5 4 }
}

}



 
vehicle glassfish {
instance glassfish
offset 0 0 300
}
 
