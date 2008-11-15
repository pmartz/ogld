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

#ifndef __OGLD_TORUS_H__
#define __OGLD_TORUS_H__


#include "OGLDPlatformGL.h"
#include "OGLDif.h"
#include "OGLDShape.h"

namespace ogld
{


class Torus : public Shape
{
public:
    static Torus* create( float majRadius=1.f, float minRadius=.25f, int majApprox=32, int minApprox=32 );

    virtual ~Torus();

    virtual void draw();

protected:
    Torus( float majRadius, float minRadius, int majApprox, int minApprox );

    virtual bool init();

    virtual bool buildData();
    virtual bool storeData();

    float _majRadius, _minRadius;
    int _majApprox, _minApprox;

    GLfloat* _vertices;
    GLfloat* _normals;
    GLfloat* _texCoords;
    GLushort* _indices;

    GLuint _idxStart, _idxEnd;

    bool _valid;
    int _numVerts, _numIndices;

    GLuint _dList;
    bool _dListAllocated;
};


#ifdef GL_VERSION_1_2

class Torus12 : public Torus
{
    friend class Torus;

public:
    virtual ~Torus12();

protected:
    Torus12( float majRadius, float minRadius, int majApprox, int minApprox );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_3

class Torus13 : public Torus
{
    friend class Torus;

public:
    virtual ~Torus13();

protected:
    Torus13( float majRadius, float minRadius, int majApprox, int minApprox );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_5

class Torus15 : public Torus
{
    friend class Torus;

public:
    virtual ~Torus15();

    virtual void draw();

protected:
    Torus15( float majRadius, float minRadius, int majApprox, int minApprox );

    virtual bool storeData();

    static int _vertIdx, _normIdx, _texIdx, _idxIdx;
    GLuint _vbo[4];
};

#endif


}


#endif
