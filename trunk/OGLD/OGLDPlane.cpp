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
#include "OGLDPlane.h"
#include "OGLDif.h"
#include "OGLDMath.h"
#include <assert.h>
#include <string.h>



namespace ogld
{


// Static factory creation methods
Plane*
Plane::create( float xLen, float yLen, int xUnits, int yUnits, float z )
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
    case Ver15:
#ifdef GL_VERSION_1_5
        //
        // Issue000 (see issues.txt)
        //
        // The Plane15 class is known to produce incorrect results
        // using an ATI Radeon 9600, driver version 1.4.18, on a G4
        // Power Macintosh with Mac OS X 10.4.4. Workaround:
        // Comment out the next two lines, allowing the switch to fall
        // through and create a Plane14 instance.
        //
        return new Plane15( xLen, yLen, xUnits, yUnits, z );
        break;
#endif

    case Ver14:
#ifdef GL_VERSION_1_4
        return new Plane14( xLen, yLen, xUnits, yUnits, z );
        break;
#endif

    case Ver13:
#ifdef GL_VERSION_1_3
        return new Plane13( xLen, yLen, xUnits, yUnits, z );
        break;
#endif

    case Ver12:
    case Ver11:
#ifdef GL_VERSION_1_1
        return new Plane( xLen, yLen, xUnits, yUnits, z );
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

Plane*
Plane::create( float xLen, float yLen, int xUnits, int yUnits, float* zArray )
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
    case Ver15:
#ifdef GL_VERSION_1_5
        //
        // Issue000 (see issues.txt)
        //
        // The Plane15 class is known to produce incorrect results
        // using an ATI Radeon 9600, driver version 1.4.18, on a G4
        // Power Macintosh with Mac OS X 10.4.4. Workaround:
        // Comment out the next two lines, allowing the switch to fall
        // through and create a Plane14 instance.
        //
        return new Plane15( xLen, yLen, xUnits, yUnits, zArray );
        break;
#endif

    case Ver14:
#ifdef GL_VERSION_1_4
        return new Plane14( xLen, yLen, xUnits, yUnits, zArray );
        break;
#endif

    case Ver13:
#ifdef GL_VERSION_1_3
        return new Plane13( xLen, yLen, xUnits, yUnits, zArray );
        break;
#endif

    case Ver12:
    case Ver11:
#ifdef GL_VERSION_1_1
        return new Plane( xLen, yLen, xUnits, yUnits, zArray );
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


Plane::Plane( float xLen, float yLen, int xUnits, int yUnits, float z )
  : _xLen( xLen ),
    _yLen( yLen ),
    _xUnits( xUnits ),
    _yUnits( yUnits ),
    _z( z ),
    _zArray( NULL ),
    _vertices( NULL ),
    _normals( NULL ),
    _texCoords( NULL ),
    _indices( NULL ),
    _idxAddrs( NULL ),
    _idxCounts( NULL ),
    _valid( false ),
    _numVerts( 0 ),
    _numIndices( 0 ),
    _dListAllocated( false )
{
    assert( OGLDif::instance()->getVersion() >= Ver11 );

    if (_xUnits < 1)
        _xUnits = 1;
    if (_yUnits < 1)
        _yUnits = 1;
}

Plane::Plane( float xLen, float yLen, int xUnits, int yUnits, float* zArray )
  : _xLen( xLen ),
    _yLen( yLen ),
    _xUnits( xUnits ),
    _yUnits( yUnits ),
    _z( 0.f ),
    _zArray( NULL ),
    _vertices( NULL ),
    _normals( NULL ),
    _texCoords( NULL ),
    _indices( NULL ),
    _idxAddrs( NULL ),
    _idxCounts( NULL ),
    _valid( false ),
    _numVerts( 0 ),
    _numIndices( 0 ),
    _dListAllocated( false )
{
    assert( OGLDif::instance()->getVersion() >= Ver11 );

    if (_xUnits < 1)
        _xUnits = 1;
    if (_yUnits < 1)
        _yUnits = 1;

    int sz = (_xUnits+1) * (_yUnits+1);
    _zArray = new float[ sz ];
    memcpy( _zArray, zArray, sz * sizeof( float ) );
}

Plane::~Plane()
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
        if (_idxAddrs)
            delete[] _idxAddrs;
        if (_idxCounts)
            delete[] _idxCounts;
        if (_zArray)
            delete[] _zArray;
        _vertices = _normals = _texCoords = NULL;
        _indices = NULL;
        _idxAddrs = NULL;
        _idxCounts = NULL;
        _zArray = NULL;
        _valid = false;
    }
}

void
Plane::draw()
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
Plane::init()
{
    if (!buildData())
        return false;

    if (!storeData())
        return false;

    return true;
}


bool
Plane::buildData()
{
    _numVerts = (_xUnits+1)*(_yUnits+1);
    _numIndices = (_yUnits+1)*2*_xUnits;
    float halfXLen = _xLen * .5f;
    float halfYLen = _yLen * .5f;

    if (_numIndices > 65535)
        return false;


    // Plane normal points in positive Z as long as sign of xLen and
    //   yLen are the same; negative Z otherwise.
    Vec3d normal( 0., 0., 1. );
    if (_xLen < 0.f)
        normal.scale( -1.f );
    if (_yLen < 0.f)
        normal.scale( -1.f );

    //
    // Create vertex, normal, and texCoord arrays
    _vertices = new GLfloat[ _numVerts * 3 ];
    _normals = new GLfloat[ _numVerts * 3 ];
    _texCoords = new GLfloat[ _numVerts * 2 ];
    GLfloat* vertPtr = _vertices;
    GLfloat* normPtr = _normals;
    GLfloat* texPtr = _texCoords;

    float* zPtr = _zArray;
    float xStep = _xLen / (float)_xUnits;
    float yStep = _yLen / (float)_yUnits;
    float x=-halfXLen;
    int xIdx;
    for (xIdx=0; xIdx<=_xUnits; xIdx++)
    {
        float y=-halfYLen;
        int yIdx;
        for (yIdx=0; yIdx<=_yUnits; yIdx++)
        {
            *vertPtr++ = x;
            *vertPtr++ = y;
            if (zPtr)
                *vertPtr++ = *zPtr++;
            else
                *vertPtr++ = _z;

            *normPtr++ = (float) normal[0];
            *normPtr++ = (float) normal[1];
            *normPtr++ = (float) normal[2];

            *texPtr++ = (float)xIdx / (float)_xUnits;
            *texPtr++ = (float)yIdx / (float)_yUnits;

            y += yStep;
        }
        x += xStep;
    }


    //
    // Create indices. We'll have _xUnits quad strips with _yUnits quads
    //   in each strip.
    _indices = new GLushort[ _numIndices ];
    GLushort* indexPtr = _indices;

    int iterations = (_yUnits+1)*_xUnits;
    for (xIdx=0; xIdx<iterations; xIdx++)
    {
        *indexPtr++ = xIdx;
        *indexPtr++ = xIdx + _yUnits + 1;
    }

    _idxAddrs = new GLushort*[ _xUnits ];
    _idxCounts = new GLsizei[ _xUnits ];
    int count = 0;
    for (xIdx=0; xIdx<_xUnits; xIdx++)
    {
        _idxAddrs[xIdx] = indexAddress( count );
        _idxCounts[xIdx] = (_yUnits+1)*2;
        count += _idxCounts[xIdx];
    }

    return true;
}


bool
Plane::storeData()
{
    _dList = glGenLists( 1 );
    assert( _dList != 0 );
    _dListAllocated = true;
    glNewList( _dList, GL_COMPILE );


    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 3, GL_FLOAT, 0, _vertices );
    glNormalPointer( GL_FLOAT, 0, _normals );
    glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );

    int idx;
    for (idx=0; idx<_xUnits; idx++)
        glDrawElements( GL_TRIANGLE_STRIP, _idxCounts[idx], GL_UNSIGNED_SHORT, _idxAddrs[idx] );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}

GLushort*
Plane::indexAddress( int count )
{
    return ( &_indices[ count ] );
}





#ifdef GL_VERSION_1_3


Plane13::Plane13( float xLen, float yLen, int xUnits, int yUnits, float z )
  : Plane( xLen, yLen, xUnits, yUnits, z )
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );
}

Plane13::Plane13( float xLen, float yLen, int xUnits, int yUnits, float* zArray )
  : Plane( xLen, yLen, xUnits, yUnits, zArray )
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );
}

Plane13::~Plane13()
{
}


bool
Plane13::storeData()
{
    _dList = glGenLists( 1 );
    assert( _dList != 0 );
    _dListAllocated = true;
    glNewList( _dList, GL_COMPILE );


    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glVertexPointer( 3, GL_FLOAT, 0, _vertices );
    glNormalPointer( GL_FLOAT, 0, _normals );

    int idx;
    for (idx=0; idx<_numTextures; idx++)
    {
        glClientActiveTexture( GL_TEXTURE0 + idx );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );
    }

    for (idx=0; idx<_xUnits; idx++)
        glDrawElements( GL_TRIANGLE_STRIP, _idxCounts[idx], GL_UNSIGNED_SHORT, _idxAddrs[idx] );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}

#endif




#ifdef GL_VERSION_1_4 


Plane14::Plane14( float xLen, float yLen, int xUnits, int yUnits, float z )
  : Plane( xLen, yLen, xUnits, yUnits, z )
{
    assert( OGLDif::instance()->getVersion() >= Ver14 );
}

Plane14::Plane14( float xLen, float yLen, int xUnits, int yUnits, float* zArray )
  : Plane( xLen, yLen, xUnits, yUnits, zArray )
{
    assert( OGLDif::instance()->getVersion() >= Ver14 );
}

Plane14::~Plane14()
{
}


bool
Plane14::storeData()
{
    _dList = glGenLists( 1 );
    assert( _dList != 0 );
    _dListAllocated = true;
    glNewList( _dList, GL_COMPILE );

    glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glVertexPointer( 3, GL_FLOAT, 0, _vertices );
    glNormalPointer( GL_FLOAT, 0, _normals );

    glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );

    int idx;
    for (idx=0; idx<_numTextures; idx++)
    {
        glClientActiveTexture( GL_TEXTURE0 + idx );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glTexCoordPointer( 2, GL_FLOAT, 0, _texCoords );
    }

    glMultiDrawElements( GL_TRIANGLE_STRIP, _idxCounts, GL_UNSIGNED_SHORT, (const void**) _idxAddrs, _xUnits );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_5


// Static indices into the _vbo vertex buffer array
int Plane15::_vertIdx( 0 );
int Plane15::_normIdx( 1 );
int Plane15::_texIdx( 2 );
int Plane15::_idxIdx( 3 );


Plane15::Plane15( float xLen, float yLen, int xUnits, int yUnits, float z )
  : Plane( xLen, yLen, xUnits, yUnits, z )
{
    assert( OGLDif::instance()->getVersion() >= Ver15 );
}

Plane15::Plane15( float xLen, float yLen, int xUnits, int yUnits, float* zArray )
  : Plane( xLen, yLen, xUnits, yUnits, zArray )
{
    assert( OGLDif::instance()->getVersion() >= Ver15 );
}

Plane15::~Plane15()
{
    if ( _valid )
        glDeleteBuffers( 4, _vbo );
}

void
Plane15::draw()
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
    glMultiDrawElements( GL_TRIANGLE_STRIP, _idxCounts, GL_UNSIGNED_SHORT, (const void**) _idxAddrs, _xUnits );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glPopClientAttrib();


    OGLDIF_CHECK_ERROR;
}


GLushort*
Plane15::indexAddress( int count )
{
    return (GLushort*) bufferObjectPtr( count * sizeof( GLushort ) );
}


bool
Plane15::storeData()
{
    glGenBuffers( 4, _vbo );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_vertIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _vertices, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_normIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _normals, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_texIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*2*sizeof(GLfloat), _texCoords, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[_idxIdx] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numIndices*sizeof(GLushort), _indices, GL_STATIC_DRAW );
    
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif

}
