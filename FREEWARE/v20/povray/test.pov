#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"
#include "chars.inc"

// Camera definition
camera { 
  location <0, 0, -500> 
  direction <0, 0, 10>
  look_at z*0
} 

plane {z,3 texture{Red_Marble}}

light_source { <200, 200, -500> color White }

light_source { <-200, 200, -500> color White }

light_source { <0, -200, -500> color Blue }

#default {texture{Gold_Metal}}

#declare ColWide=5
#declare Col1=-18
#declare Col2=Col1+ColWide
#declare Col3=Col2+ColWide
#declare Col4=Col3+ColWide
#declare Col5=Col4+ColWide
#declare Col6=Col5+ColWide
#declare Col7=Col6+ColWide
#declare Col8=Col7+ColWide
#declare Col9=Col8+ColWide
#declare Col10=Col9+ColWide

#declare Row1=7
#declare Row2=0
#declare Row3= 4
#declare Row4=-3
#declare Row5=-10
#declare Row6=-17

object {char_O translate <Col1,Row1,-80>}
object {char_P translate <Col2,Row1,-80>}
object {char_E translate <Col3,Row1,-80>}
object {char_N translate <Col4,Row1,-80>}
object {char_V translate <Col6,Row1,-80>}
object {char_M translate <Col7,Row1,-80>}
object {char_S translate <Col8,Row1,-80>}

object {char_P translate <Col2,Row2,-80>}
object {char_O translate <Col3,Row2,-80>}
object {char_V translate <Col4,Row2,-80>}
object {char_R translate <Col5,Row2,-80>}
object {char_A translate <Col6,Row2,-80>}
object {char_Y translate <Col7,Row2,-80>}

object { 
  sphere { <0, -15, -30> 30 } 
  texture { Mirror} 
} 
