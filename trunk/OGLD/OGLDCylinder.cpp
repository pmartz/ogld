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

#include "OGLDPlatformGL.h"
#include "OGLDCylinder.h"
#include "OGLDif.h"
#include "OGLDMath.h"
#include <math.h>
#include <assert.h>



namespace ogld
{


// Static factory creation methods
Cylinder*
Cylinder::create( float radius, float length, int slices )
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
    case Ver15:
#ifdef GL_VERSION_1_5
        return new Cylinder15( radius, length, slices );
        break;
#endif

    case Ver14:
    case Ver13:
#ifdef GL_VERSION_1_3
        return new Cylinder13( radius, length, slices );
        break;
#endif

    case Ver12:
#ifdef GL_VERSION_1_2
        return new Cylinder12( radius, length, slices );
        break;
#endif

    case Ver11:
#ifdef GL_VERSION_1_1
        return new Cylinder( radius, length, slices );
        break;
#endif

    case Ver10:
    case VerUnknown:
    default:
        assert( false );
        return NULL;
        break;
    }
}

Cylinder::Cylinder( float radius, float length, int slices )
  : _radius( radius ),
    _length( length ),
    _slices( slices ),
    _drawCap1( false ),
    _drawCap2( false ),
    _vertices( NULL ),
    _normals( NULL ),
    _texCoords( NULL ),
    _indices( NULL ),
    _cap1Start( NULL ),
    _cap2Start( NULL ),
    _idxStart( 0 ),
    _cap1Idx( 0 ),
    _cap2Idx( 0 ),
    _idxEnd( 0 ),
    _valid( false ),
    _numVerts( 0 ),
    _numCapVerts( 0 ),
    _dListAllocated( false )
{
    assert( OGLDif::instance()->getVersion() >= Ver11 );
}

Cylinder::~Cylinder()
{
    if ( _valid )
    {
        if (_dListAllocated)
        {
            glDeleteLists( _dList, 1 );
            _dListAllocated = false;
        }

        if (_texCoords)
            delete[] _texCoords;
        if (_vertices)
            delete[] _vertices;
        if (_normals)
            delete[] _normals;
        if (_indices)
            delete[] _indices;
        _texCoords = _vertices = _normals = NULL;
        _indices = NULL;
        _valid = false;
    }
}

void
Cylinder::cap( bool posZ, bool negZ )
{
    _drawCap1 = posZ;
    _drawCap2 = negZ;
}

void
Cylinder::draw()
{
    if (!_valid)
    {
        if (!init())
            return;
    }

    glCallList( _dList );

    OGLDIF_CHECK_ERROR;
}

bool
Cylinder::init()
{
    if (!buildData())
        return false;

    if (!storeData())
        return false;

    return true;
}


bool
Cylinder::buildData()
{
    _numVerts = (_slices + 1) * 2;
    _numCapVerts = _slices + 2;

    const int totalVerts = _numVerts + (_numCapVerts * 2);

    _texCoords = new GLfloat[ totalVerts * 2 ];
    _vertices = new GLfloat[ totalVerts * 3 ];
    _normals = new GLfloat[ totalVerts * 3 ];
    _indices = new GLushort[ totalVerts ];

    const float halfLength = _length * .5f;
    const double delta = _2PI / _slices;

    GLfloat* texPtr = _texCoords;
    GLfloat* vertPtr = _vertices;
    GLfloat* normPtr = _normals;
    GLushort* indexPtr = _indices;
    int idx = 0;


    //
    // Cylinder body
    _idxStart = idx;
    double theta = 0.f;
    int i;
    for (i=0; i<_slices; i++)
    {
        const float cosTheta = (float) cos( theta );
        const float sinTheta = (float) sin( theta );

        vertPtr[0] = cosTheta * _radius;
        vertPtr[1] = sinTheta * _radius;
        vertPtr[2] = halfLength;
        vertPtr += 3;

        normPtr[0] = cosTheta;
        normPtr[1] = sinTheta;
        normPtr[2] = 0.f;
        normPtr += 3;

        texPtr[0] = (float)i / (float)_slices;
        texPtr[1] = 1.f;
        texPtr += 2;

        *indexPtr++ = idx++;

        vertPtr[0] = cosTheta * _radius;
        vertPtr[1] = sinTheta * _radius;
        vertPtr[2] = -halfLength;
        vertPtr += 3;

        normPtr[0] = cosTheta;
        normPtr[1] = sinTheta;
        normPtr[2] = 0.f;
        normPtr += 3;

        texPtr[0] = (float)i / (float)_slices;
        texPtr[1] = 0.f;
        texPtr += 2;

        *indexPtr++ = idx++;

        theta += delta;
    }

    vertPtr[0] = _radius;
    vertPtr[1] = 0.f;
    vertPtr[2] = halfLength;
    vertPtr += 3;

    normPtr[0] = 1.f;
    normPtr[1] = 0.f;
    normPtr[2] = 0.f;
    normPtr += 3;

    texPtr[0] = 1.f;
    texPtr[1] = 1.f;
    texPtr += 2;

    *indexPtr++ = idx++;

    vertPtr[0] = _radius;
    vertPtr[1] = 0.f;
    vertPtr[2] = -halfLength;
    vertPtr += 3;

    normPtr[0] = 1.f;
    normPtr[1] = 0.f;
    normPtr[2] = 0.f;
    normPtr += 3;

    texPtr[0] = 1.f;
    texPtr[1] = 0.f;
    texPtr += 2;

    *indexPtr++ = idx++;


    //
    // Cap 1 (positive Z)
    _cap1Idx = idx;
    _cap1Start = indexPtr;

    vertPtr[0] = 0.f;
    vertPtr[1] = 0.f;
    vertPtr[2] = halfLength;
    vertPtr += 3;

    normPtr[0] = 0.f;
    normPtr[1] = 0.f;
    normPtr[2] = 1.f;
    normPtr += 3;

    texPtr[0] = .5f;
    texPtr[1] = .5f;
    texPtr += 2;

    *indexPtr++ = idx++;

    theta = 0.;
    for (i=0; i<_slices; i++)
    {
        const float cosTheta = (float) cos( theta );
        const float sinTheta = (float) sin( theta );

        vertPtr[0] = cosTheta * _radius;
        vertPtr[1] = sinTheta * _radius;
        vertPtr[2] = halfLength;
        vertPtr += 3;

        normPtr[0] = 0.f;
        normPtr[1] = 0.f;
        normPtr[2] = 1.f;
        normPtr += 3;

        texPtr[0] = cosTheta * .5f + .5f;
        texPtr[1] = sinTheta * .5f + .5f;
        texPtr += 2;

        *indexPtr++ = idx++;

        theta += delta;
    }

    vertPtr[0] = _radius;
    vertPtr[1] = 0.f;
    vertPtr[2] = halfLength;
    vertPtr += 3;

    normPtr[0] = 0.f;
    normPtr[1] = 0.f;
    normPtr[2] = 1.f;
    normPtr += 3;

    texPtr[0] = 1.f;
    texPtr[1] = .5f;
    texPtr += 2;

    *indexPtr++ = idx++;


    //
    // Cap 2 (negative Z)
    _cap2Idx = idx;
    _cap2Start = indexPtr;

    vertPtr[0] = 0.f;
    vertPtr[1] = 0.f;
    vertPtr[2] = -halfLength;
    vertPtr += 3;

    normPtr[0] = 0.f;
    normPtr[1] = 0.f;
    normPtr[2] = -1.f;
    normPtr += 3;

    texPtr[0] = .5f;
    texPtr[1] = .5f;
    texPtr += 2;

    *indexPtr++ = idx++;

    theta = _2PI;
    for (i=0; i<_slices; i++)
    {
        const float cosTheta = (float) cos( theta );
        const float sinTheta = (float) sin( theta );

        vertPtr[0] = cosTheta * _radius;
        vertPtr[1] = sinTheta * _radius;
        vertPtr[2] = -halfLength;
        vertPtr += 3;

        normPtr[0] = 0.f;
        normPtr[1] = 0.f;
        normPtr[2] = -1.f;
        normPtr += 3;

        texPtr[0] = cosTheta * .5f + .5f;
        texPtr[1] = sinTheta * .5f + .5f;
        texPtr += 2;

        *indexPtr++ = idx++;

        theta -= delta;
    }

    vertPtr[0] = _radius;
    vertPtr[1] = 0.f;
    vertPtr[2] = -halfLength;
    vertPtr += 3;

    normPtr[0] = 0.f;
    normPtr[1] = 0.f;
    normPtr[2] = -1.f;
    normPtr += 3;

    texPtr[0] = 1.f;
    texPtr[1] = .5f;
    texPtr += 2;

    *indexPtr++ = idx++;
    assert( idx == totalVerts );
    _idxEnd = idx;

    return true;
}


bool
Cylinder::storeData()
{
    _dList = glGenLists( 1 );
    assert( _dList != 0 );
    _dListAllocated = true;
    glNewList( _dList, GL_COMPILE );


    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, _vertices );

    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_FLOAT, 0, _normals );

    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );


    glDrawElements( GL_QUAD_STRIP, _numVerts, GL_UNSIGNED_SHORT, _indices );
    if (_drawCap1)
        glDrawElements( GL_TRIANGLE_FAN, _numCapVerts, GL_UNSIGNED_SHORT, _cap1Start );
    if (_drawCap2)
        glDrawElements( GL_TRIANGLE_FAN, _numCapVerts, GL_UNSIGNED_SHORT, _cap2Start );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}




#ifdef GL_VERSION_1_2


Cylinder12::Cylinder12( float radius, float length, int slices )
  : Cylinder( radius, length, slices )
{
    assert( OGLDif::instance()->getVersion() >= Ver12 );

}

Cylinder12::~Cylinder12()
{
}


bool
Cylinder12::storeData()
{
    _dList = glGenLists( 1 );
    assert( _dList != 0 );
    _dListAllocated = true;
    glNewList( _dList, GL_COMPILE );

    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, _vertices );

    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_FLOAT, 0, _normals );

    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );

    glDrawRangeElements( GL_QUAD_STRIP, _idxStart, _cap1Idx-1, _numVerts,
            GL_UNSIGNED_SHORT, _indices );
    if (_drawCap1)
        glDrawRangeElements( GL_TRIANGLE_FAN, _cap1Idx, _cap2Idx-1, _numCapVerts,
                GL_UNSIGNED_SHORT, _cap1Start );
    if (_drawCap2)
        glDrawRangeElements( GL_TRIANGLE_FAN, _cap2Idx, _idxEnd-1, _numCapVerts,
                GL_UNSIGNED_SHORT, _cap2Start );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_3


Cylinder13::Cylinder13( float radius, float length, int slices )
  : Cylinder( radius, length, slices )
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );

}

Cylinder13::~Cylinder13()
{
}


bool
Cylinder13::storeData()
{
    _dList = glGenLists( 1 );
    assert( _dList != 0 );
    _dListAllocated = true;
    glNewList( _dList, GL_COMPILE );

    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, _vertices );

    glEnableClientState( GL_NORMAL_ARRAY );
    glNormalPointer( GL_FLOAT, 0, _normals );

    int tIdx;
    for (tIdx=0; tIdx<_numTextures; tIdx++)
    {
        glClientActiveTexture( GL_TEXTURE0 + tIdx );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );
    }


    glDrawRangeElements( GL_QUAD_STRIP, _idxStart, _cap1Idx-1, _numVerts,
            GL_UNSIGNED_SHORT, _indices );
    if (_drawCap1)
        glDrawRangeElements( GL_TRIANGLE_FAN, _cap1Idx, _cap2Idx-1, _numCapVerts,
                GL_UNSIGNED_SHORT, _cap1Start );
    if (_drawCap2)
        glDrawRangeElements( GL_TRIANGLE_FAN, _cap2Idx, _idxEnd-1, _numCapVerts,
                GL_UNSIGNED_SHORT, _cap2Start );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_5


// Static indices into the _vbo vertex buffer array
int Cylinder15::_vertIdx( 0 );
int Cylinder15::_normIdx( 1 );
int Cylinder15::_texIdx( 2 );
int Cylinder15::_idxIdx( 3 );


Cylinder15::Cylinder15( float radius, float length, int slices )
  : Cylinder( radius, length, slices )
{
    assert( OGLDif::instance()->getVersion() >= Ver15 );
}

Cylinder15::~Cylinder15()
{
    if ( _valid )
        glDeleteBuffers( 4, _vbo );
}

void
Cylinder15::draw()
{
    if (!_valid)
    {
        if (!init())
            return;
    }

    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_vertIdx] );
    glVertexPointer( 3, GL_FLOAT, 0, bufferObjectPtr( 0 ) );

    glEnableClientState( GL_NORMAL_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_normIdx] );
    glNormalPointer( GL_FLOAT, 0, bufferObjectPtr( 0 ) );

    int tIdx;
    for (tIdx=0; tIdx<_numTextures; tIdx++)
    {
        glClientActiveTexture( GL_TEXTURE0 + tIdx );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glBindBuffer( GL_ARRAY_BUFFER, _vbo[_texIdx] );
        glTexCoordPointer( 2, GL_FLOAT, 0, bufferObjectPtr( 0 ) );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[_idxIdx] );
    glDrawRangeElements( GL_QUAD_STRIP, _idxStart, _cap1Idx-1, _numVerts,
            GL_UNSIGNED_SHORT, bufferObjectPtr( 0 ) );
    if (_drawCap1)
        glDrawRangeElements( GL_TRIANGLE_FAN, _cap1Idx, _cap2Idx-1, _numCapVerts,
                GL_UNSIGNED_SHORT, bufferObjectPtr((_cap1Start-_indices)*sizeof(GLushort)) );
    if (_drawCap2)
        glDrawRangeElements( GL_TRIANGLE_FAN, _cap2Idx, _idxEnd-1, _numCapVerts,
                GL_UNSIGNED_SHORT, bufferObjectPtr((_cap2Start-_indices)*sizeof(GLushort)) );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glPopClientAttrib();


    OGLDIF_CHECK_ERROR;
}

bool
Cylinder15::storeData()
{
    const int totalVerts = _numVerts + (_numCapVerts * 2);

    glGenBuffers( 4, _vbo );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_texIdx] );
    glBufferData( GL_ARRAY_BUFFER, totalVerts*2*sizeof(GLfloat), _texCoords, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_normIdx] );
    glBufferData( GL_ARRAY_BUFFER, totalVerts*3*sizeof(GLfloat), _normals, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_vertIdx] );
    glBufferData( GL_ARRAY_BUFFER, totalVerts*3*sizeof(GLfloat), _vertices, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[_idxIdx] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, totalVerts*sizeof(GLushort), _indices, GL_STATIC_DRAW );


    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif

}
