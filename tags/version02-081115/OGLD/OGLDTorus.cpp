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
#include "OGLDTorus.h"
#include "OGLDif.h"
#include "OGLDMath.h"
#include <math.h>
#include <assert.h>



namespace ogld
{


// Static factory creation methods
Torus*
Torus::create( float majRadius, float minRadius, int majApprox, int minApprox )
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
    case Ver15:
#ifdef GL_VERSION_1_5
        return new Torus15( majRadius, minRadius, majApprox, minApprox );
        break;
#endif

    case Ver14:
    case Ver13:
#ifdef GL_VERSION_1_3
        return new Torus13( majRadius, minRadius, majApprox, minApprox );
        break;
#endif

    case Ver12:
#ifdef GL_VERSION_1_2
        return new Torus12( majRadius, minRadius, majApprox, minApprox );
        break;
#endif

    case Ver11:
#ifdef GL_VERSION_1_1
        return new Torus( majRadius, minRadius, majApprox, minApprox );
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

Torus::Torus( float majRadius, float minRadius, int majApprox, int minApprox )
  : _majRadius( majRadius ),
    _minRadius( minRadius ),
    _majApprox( majApprox ),
    _minApprox( minApprox ),
    _vertices( NULL ),
    _normals( NULL ),
    _texCoords( NULL ),
    _indices( NULL ),
    _idxStart( 0 ),
    _idxEnd( 0 ),
    _valid( false ),
    _numVerts( 0 ),
    _numIndices( 0 ),
    _dListAllocated( false )
{
    assert( OGLDif::instance()->getVersion() >= Ver11 );

    if (_majApprox < 3)
        _majApprox = 3;
    if (_minApprox < 3)
        _minApprox = 3;
}

Torus::~Torus()
{
    if ( _valid )
    {
        if (_dListAllocated)
        {
            glDeleteLists( _dList, 1 );
            _dListAllocated = false;
        }

        if (_vertices)
            delete[] _vertices;
        if (_normals)
            delete[] _normals;
        if (_texCoords)
            delete[] _texCoords;
        if (_indices)
            delete[] _indices;
        _vertices = _normals = _texCoords = NULL;
        _indices = NULL;
        _valid = false;
    }
}

void
Torus::draw()
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
Torus::init()
{
    if (!buildData())
        return false;

    if (!storeData())
        return false;

    return true;
}

bool
Torus::buildData()
{
    _numVerts = (_majApprox + 1) * (_minApprox + 1);
    _numIndices = _majApprox * 2 * (_minApprox + 1);

    if (_numIndices > 65535)
        return false;

    double delta = _2PI / _minApprox;


    //
    // Make a circle in the Z=0 plane, with radius _minRadius,
    //   and translated in positive X by _majRadius. 
    _texCoords = new GLfloat[ _numVerts * 2 ];
    _normals = new GLfloat[ _numVerts * 3 ];
    _vertices = new GLfloat[ _numVerts * 3 ];

    GLfloat* texPtr = _texCoords;
    GLfloat* normPtr = _normals;
    GLfloat* vertPtr = _vertices;

    double theta = 0.;
    int i;
    for (i=_minApprox; i>=0; i--)
    {
        const float cosTheta = (float) cos( theta );
        const float sinTheta = (float) sin( theta );

        vertPtr[0] = cosTheta * _minRadius + _majRadius;
        vertPtr[1] = sinTheta * _minRadius;
        vertPtr[2] = 0.f;
        vertPtr += 3;

        normPtr[0] = cosTheta;
        normPtr[1] = sinTheta;
        normPtr[2] = 0.f;
        normPtr += 3;

        texPtr[0] = 0.f;
        texPtr[1] = (float)i / (float)_minApprox;
        texPtr += 2;

        theta += delta;
    }


    //
    // Rotate that circle around _maxApprox times.
    //   Store them in the vertex and normal buffers.

    delta = _2PI / _majApprox;
    theta = delta;

    for (i=1; i<_majApprox+1; i++)
    {
        GLfloat* srcNormPtr = _normals;
        GLfloat* srcTexPtr = _texCoords;
        GLfloat* srcVertPtr = _vertices;

        const float cosTheta = (float) cos( theta );
        const float sinTheta = (float) sin( theta );

        int j;
        for (j=_minApprox; j>=0; j--)
        {
            vertPtr[0] = cosTheta * srcVertPtr[0];
            vertPtr[1] = srcVertPtr[1];
            vertPtr[2] = -sinTheta * srcVertPtr[0];
            vertPtr += 3;
            srcVertPtr += 3;

            normPtr[0] = cosTheta * srcNormPtr[0];
            normPtr[1] = srcNormPtr[1];
            normPtr[2] = -sinTheta * srcNormPtr[0];
            normPtr += 3;
            srcNormPtr += 3;

            texPtr[0] = (float)i / (float)_majApprox;
            texPtr[1] = srcTexPtr[1];
            texPtr += 2;
            srcTexPtr += 2;
        }

        theta += delta;
    }


    // Finally, create indices into the arrays, making _majApprox
    //   quad strips, each with (_minApprox+1)*2 indices

    _indices = new GLushort[ _numIndices ];
    GLushort* indexPtr = _indices;

    _idxStart = 0;
    _idxEnd = _numVerts;

    for (i=0; i<_majApprox; i++)
    {
        int index;
        int j;
        for (j=0; j<_minApprox+1; j++)
        {
            index = (i*(_minApprox+1) + j) % _numVerts;
            assert( index < _numVerts );
            assert( index >= 0 );
            *indexPtr++ = index;

            index = (i*(_minApprox+1) + j + _minApprox+1) % _numVerts;
            assert( index < _numVerts );
            assert( index >= 0 );
            *indexPtr++ = index;
        }
    }

    return true;
}


bool
Torus::storeData()
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


    glDrawElements( GL_QUAD_STRIP, _numIndices, GL_UNSIGNED_SHORT, _indices );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}




#ifdef GL_VERSION_1_2


Torus12::Torus12( float majRadius, float minRadius, int majApprox, int minApprox )
  : Torus( majRadius, minRadius, majApprox, minApprox )
{
    assert( OGLDif::instance()->getVersion() >= Ver12 );
}

Torus12::~Torus12()
{
}


bool
Torus12::storeData()
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


    glDrawRangeElements( GL_QUAD_STRIP, _idxStart, _idxEnd, _numIndices, GL_UNSIGNED_SHORT, _indices );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_3


Torus13::Torus13( float majRadius, float minRadius, int majApprox, int minApprox )
  : Torus( majRadius, minRadius, majApprox, minApprox )
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );
}

Torus13::~Torus13()
{
}


bool
Torus13::storeData()
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


    glDrawRangeElements( GL_QUAD_STRIP, _idxStart, _idxEnd, _numIndices, GL_UNSIGNED_SHORT, _indices );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_5


// Static indices into the _vbo vertex buffer array
int Torus15::_vertIdx( 0 );
int Torus15::_normIdx( 1 );
int Torus15::_texIdx( 2 );
int Torus15::_idxIdx( 3 );


Torus15::Torus15( float majRadius, float minRadius, int majApprox, int minApprox )
  : Torus( majRadius, minRadius, majApprox, minApprox )
{
    assert( OGLDif::instance()->getVersion() >= Ver15 );
}

Torus15::~Torus15()
{
    if ( _valid )
        glDeleteBuffers( 3, _vbo );
}


void
Torus15::draw()
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
    glDrawRangeElements( GL_QUAD_STRIP, _idxStart, _idxEnd, _numIndices, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glPopClientAttrib();


    OGLDIF_CHECK_ERROR;
}

bool
Torus15::storeData()
{
    glGenBuffers( 4, _vbo );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_texIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*2*sizeof(GLfloat), _texCoords, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_normIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _normals, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_vertIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _vertices, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[_idxIdx] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numIndices*sizeof(GLushort), _indices, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif

}
