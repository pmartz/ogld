//
// http://www.opengldistilled.com
//
// This code was written by Paul Martz (pmartz@opengldistilled.com)
//
// This code comes with absolutely no warranty.
//
// You may freely use, distribute, modify, and publish this source
// code, provided that this comment block remains intact in all
// copies and derived works.
// 

#ifndef __SIMPLE_CYLINDER_H__
#define __SIMPLE_CYLINDER_H__


#include "OGLDPlatformGL.h"
#include "OGLDShape.h"


class Cylinder : public ogld::Shape
{
public:
    Cylinder( float radius=1.f, float length=2.f, int slices=32 );
    virtual ~Cylinder();

    virtual void draw();

    void cap( bool posZ, bool negZ );

private:
    bool init();

    float _radius;
    float _length;
    int _slices;
    bool _drawCap1, _drawCap2;

    GLfloat* _vertices;
    GLfloat* _normals;
    GLushort* _indices;
    GLushort* _cap1Start;
    GLushort* _cap2Start;

    GLuint _idxStart, _cap1Idx, _cap2Idx, _idxEnd;

    bool _valid;
    int _numVerts;
    int _numCapVerts;
};


#endif
