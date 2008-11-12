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

#ifndef __OGLD_SPHERE_H__
#define __OGLD_SPHERE_H__


#include "OGLDPlatformGL.h"
#include "OGLDif.h"
#include "OGLDShape.h"

namespace ogld
{


class Sphere : public Shape
{
public:
    static Sphere* create( float radius=1.f, int subdivisions=2 );

    virtual ~Sphere();

    virtual void draw();

protected:
    Sphere( float radius, int subdivisions );

    bool init();

    virtual bool buildData();
    virtual bool storeData();

	//
	// Inline utility functions used by buildData().
	inline unsigned int makeKey( const unsigned short a, const unsigned short b ) const
	{
		if ( a < b )
			return (( a << 16) | b);
		else
			return (( b << 16) | a);
	};
	inline void average3fv( float* r, const float* a, const float* b ) const
	{
		r[0] = (a[0] + b[0]) * .5f;
		r[1] = (a[1] + b[1]) * .5f;
		r[2] = (a[2] + b[2]) * .5f;
	};


    float _radius;
    int _subdivisions;

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

class Sphere12 : public Sphere
{
    friend class Sphere;

public:
    virtual ~Sphere12();

protected:
    Sphere12( float radius, int subdivisions );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_3

class Sphere13 : public Sphere
{
    friend class Sphere;

public:
    virtual ~Sphere13();

protected:
    Sphere13( float radius, int subdivisions );

    virtual bool storeData();
};

#endif


#ifdef GL_VERSION_1_5

class Sphere15 : public Sphere
{
    friend class Sphere;

public:
    virtual ~Sphere15();

    virtual void draw();

protected:
    Sphere15( float radius, int subdivisions );

    virtual bool storeData();

    static int _vertIdx, _normIdx, _texIdx, _idxIdx;
    GLuint _vbo[4];
};

#endif

}


#endif
