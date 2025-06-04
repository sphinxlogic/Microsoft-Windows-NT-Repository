#include <iostream.h>
#include <math.h>
//
// Calcul de la hauteur au dessus du sol a une distance d  due a la
// courbure de la Terre:
//
// h = sqrt(R*R + d*d) - R;
//
int main()
{
  double x;
  double z;
  double y;
  double d2;
  double R=21000000;  // Rayon de la Terre en pieds
  double h;           // hauteur a calcluer
  double R2;          // rayon au carre
  char dir1[8];  // directive (GEOMETRY)
  char dir2[5];  // directive (POINT)

  R2 = R*R;      
  dir1[0] = 'G';
  while (dir1[0] == 'G') {
    cin >>dir1>>dir2>>x>>z>>y;
    d2 = (x*x+y*y);
    h  = z - (sqrt(R2 + d2) -R);
    if (dir1[0] == 'G') {
      cout <<"GEOMETRY POINT "<<x<<" "<<h<<" "<<y<<endl; }
   }
}
