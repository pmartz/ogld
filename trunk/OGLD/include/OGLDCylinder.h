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

#ifndef __OGLD_CYLINDER_H__
#define __OGLD_CYLINDER_H__


#include "OGLDPlatformGL.h"
#include "OGLDif.h"
#include "OGLDShape.h"

namespace ogld
{


class Cylinder : public Shape
{
public:
    static Cylinder* create( float radius=1.f, float length=2.f, int slices=32 );

    virtual ~Cylinder();

    virtual void draw();

    void cap( bool posZ, bool negZ );

protected:
    Cylinder( float radius, float length, int slices );

    virtual bool init();

    virtual bool buildData();
    virtual bool storeData();

    float _radius;
    float _length;
    int _slices;
    bool _drawCap1, _drawCap2;

    GLfloat* _vertices;
    GLfloat* _normals;
    GLfloat* _texCoords;
    GLushort* _indices;
    GLushort* _cap1Start;
    GLushort* _cap2Start;

    GLuint _idxStart, _cap1Idx, _cap2Idx, _idxEnd;

    bool _valid;
    int _numVerts;
    int _numCapVerts;

    GLuint _dList;
    bool _dListAllocated;
};


#ifdef GL_VERSION_1_2

class Cylinder12 : public Cylinder
{
    friend class Cylinder;

public:
    virtual ~Cylinder12();

protected:
    Cylinder12( float radius, float length, int slices );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_3

class Cylinder13 : public Cylinder
{
    friend class Cylinder;

public:
    virtual ~Cylinder13();

protected:
    Cylinder13( float radius, float length, int slices );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_5

class Cylinder15 : public Cylinder
{
    friend class Cylinder;

public:
    virtual ~Cylinder15();

    virtual void draw();

protected:
    Cylinder15( float radius, float length, int slices );

    virtual bool storeData();

    static int _vertIdx, _normIdx, _texIdx, _idxIdx;
    GLuint _vbo[4];
};

#endif

}


#endif
