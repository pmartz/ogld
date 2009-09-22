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

#ifndef __OGLD_PLANE_H__
#define __OGLD_PLANE_H__


#include "OGLDPlatformGL.h"
#include "OGLDif.h"
#include "OGLDShape.h"

namespace ogld
{


class Plane : public Shape
{
public:
    static Plane* create( float xLen=1.f, float yLen=1.f, int xUnits=1, int yUnits=1, float z=0.f );
    static Plane* create( float xLen, float yLen, int xUnits, int yUnits, float* zArray );

    virtual ~Plane();

    virtual void draw();

protected:
    Plane( float xLen, float yLen, int xUnits, int yUnits, float z );
    Plane( float xLen, float yLen, int xUnits, int yUnits, float* zArray );

    virtual bool init();

    virtual bool buildData();
    virtual bool storeData();

    virtual GLushort* indexAddress( int count );

    float _xLen, _yLen;
    int _xUnits, _yUnits;
    float _z;
    float* _zArray;

    GLfloat* _vertices;
    GLfloat* _normals;
    GLfloat* _texCoords;
    GLushort* _indices;
    GLushort** _idxAddrs;
    GLsizei* _idxCounts;

    bool _valid;
    int _numVerts, _numIndices;

    GLuint _dList;
    bool _dListAllocated;
};


#ifdef GL_VERSION_1_3

class Plane13 : public Plane
{
    friend class Plane;

public:
    virtual ~Plane13();

protected:
    Plane13( float xLen, float yLen, int xUnits, int yUnits, float z );
    Plane13( float xLen, float yLen, int xUnits, int yUnits, float* zArray );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_4

class Plane14 : public Plane
{
    friend class Plane;

public:
    virtual ~Plane14();

protected:
    Plane14( float xLen, float yLen, int xUnits, int yUnits, float z );
    Plane14( float xLen, float yLen, int xUnits, int yUnits, float* zArray );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_5

class Plane15 : public Plane
{
    friend class Plane;

public:
    virtual ~Plane15();

    virtual void draw();

protected:
    Plane15( float xLen, float yLen, int xUnits, int yUnits, float z );
    Plane15( float xLen, float yLen, int xUnits, int yUnits, float* zArray );

    virtual bool storeData();

    virtual GLushort* indexAddress( int count );

    static int _vertIdx, _normIdx, _texIdx, _idxIdx;
    GLuint _vbo[4];
};

#endif

}


#endif
