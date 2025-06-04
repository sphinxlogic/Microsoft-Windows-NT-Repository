/*
 * File:        tool3d.h
 * Purpose:     class definitions for projecting 3D objects
 * Author:      Markus Franz Meisinger
 * e-mail:      Markus.Meisinger@risc.uni-linz.ac.at
 * Created:     June 1994
 * Updated:     
 * Copyright:   
 */

#ifdef __GNUG__
#pragma interface
#endif

class object3D
{ public:
    struct point3D {
      int x, y, z;
    };
    struct longPoint3D {
      long x, y, z;
    };
    struct point2D {
      int x, y;
    };
    struct faceStruct {
      int noPts;
      int visible;
      struct point3D *origPoint;
      struct point3D *point;
      struct point2D *projected;
      struct longPoint3D origNormal;
      struct longPoint3D normal;
    };

    int sinTable[360];    
    int alpha;
    int beta;
    int gamma;
    struct facesStruct {
      int no;
      struct faceStruct *points;
    } faces;

    object3D(void);
    virtual ~object3D(void);

    long Sinus(int grad);
    long Cosinus(int grad);
    void RotateX(void);
    void RotateY(void);
    void RotateZ(void);
    virtual void Draw(void) {};
    void Project(int xOfs, int yOfs, int xEye, int yEye, int zEye);
    void Rotate(int xRot, int yRot, int zRot);
    void Translate(int x, int y, int z);
};
