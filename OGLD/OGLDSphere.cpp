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

#if defined( WIN32 ) && !defined( __CYGWIN__ )
  // Disable harmless VC6 warnings
  #pragma warning (disable : 4305)
#endif

#include "OGLDPlatformGL.h"
#include "OGLDSphere.h"
#include "OGLDif.h"
#include "OGLDMath.h"
#include <math.h>
#include <map>
#include <assert.h>
#include <string.h>



namespace ogld
{


// Static factory creation methods
Sphere*
Sphere::create( float radius, int subdivisions )
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
    case Ver15:
#ifdef GL_VERSION_1_5
        return new Sphere15( radius, subdivisions );
        break;
#endif

    case Ver14:
    case Ver13:
#ifdef GL_VERSION_1_3
        return new Sphere13( radius, subdivisions );
        break;
#endif

    case Ver12:
#ifdef GL_VERSION_1_2
        return new Sphere12( radius, subdivisions );
        break;
#endif

    case Ver11:
#ifdef GL_VERSION_1_1
        return new Sphere( radius, subdivisions );
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

Sphere::Sphere( float radius, int subdivisions )
  : _radius( radius ),
    _subdivisions( subdivisions ),
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

    if (_subdivisions < 0)
        _subdivisions = 0;
}

Sphere::~Sphere()
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
Sphere::draw()
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
Sphere::init()
{
    if (!buildData())
        return false;

    if (!storeData())
        return false;

    return true;
}


bool
Sphere::buildData()
{
    GLfloat vertData[] = {
        0.000000, 0.850651, 0.525731,
        0.000000, 0.850651, -0.525731,
        0.000000, -0.850651, -0.525731,
        0.000000, -0.850651, 0.525731,
        0.525731, 0.000000, 0.850651,
        0.525731, 0.000000, -0.850651,
        -0.525731, 0.000000, -0.850651,
        -0.525731, 0.000000, 0.850651,
        0.850651, 0.525731, 0.000000,
        0.850651, -0.525731, 0.000000,
        -0.850651, -0.525731, 0.000000,
        -0.850651, 0.525731, 0.000000 };

    int faces = 20;
    _numVerts = 12;
    _numIndices = faces * 3;

    if (_numIndices > 65535)
        return false;


    int vertsSize = _numVerts * 3;
    _vertices = new GLfloat[ vertsSize ];
    memcpy( _vertices, vertData, sizeof( vertData ) );

    _indices = new GLushort[ _numIndices ];
    GLushort* indexPtr = _indices;
    *indexPtr++ = 0;
    *indexPtr++ = 7;
    *indexPtr++ = 4;
    *indexPtr++ = 0;
    *indexPtr++ = 4;
    *indexPtr++ = 8;
    *indexPtr++ = 0;
    *indexPtr++ = 8;
    *indexPtr++ = 1;
    *indexPtr++ = 0;
    *indexPtr++ = 1;
    *indexPtr++ = 11;
    *indexPtr++ = 0;
    *indexPtr++ = 11;
    *indexPtr++ = 7;
    *indexPtr++ = 2;
    *indexPtr++ = 6;
    *indexPtr++ = 5;
    *indexPtr++ = 2;
    *indexPtr++ = 5;
    *indexPtr++ = 9;
    *indexPtr++ = 2;
    *indexPtr++ = 9;
    *indexPtr++ = 3;
    *indexPtr++ = 2;
    *indexPtr++ = 3;
    *indexPtr++ = 10;
    *indexPtr++ = 2;
    *indexPtr++ = 10;
    *indexPtr++ = 6;
    *indexPtr++ = 7;
    *indexPtr++ = 3;
    *indexPtr++ = 4;
    *indexPtr++ = 4;
    *indexPtr++ = 3;
    *indexPtr++ = 9;
    *indexPtr++ = 4;
    *indexPtr++ = 9;
    *indexPtr++ = 8;
    *indexPtr++ = 8;
    *indexPtr++ = 9;
    *indexPtr++ = 5;
    *indexPtr++ = 8;
    *indexPtr++ = 5;
    *indexPtr++ = 1;
    *indexPtr++ = 1;
    *indexPtr++ = 5;
    *indexPtr++ = 6;
    *indexPtr++ = 1;
    *indexPtr++ = 6;
    *indexPtr++ = 11;
    *indexPtr++ = 11;
    *indexPtr++ = 6;
    *indexPtr++ = 10;
    *indexPtr++ = 11;
    *indexPtr++ = 10;
    *indexPtr++ = 7;
    *indexPtr++ = 7;
    *indexPtr++ = 10;
    *indexPtr++ = 3;

    _idxStart = 0;
    _idxEnd = 11;


    // Subdivide as requested
    int i;
    for (i=_subdivisions; i; i--)
    {
        // Make a map of edges
        typedef std::map< unsigned int, GLushort> EdgeMapType;
        EdgeMapType edgeMap;
        indexPtr = _indices;
        int f;
        for (f=faces; f; f--)
        {
            unsigned int key = makeKey(indexPtr[0], indexPtr[1]);
            if (edgeMap.find( key ) == edgeMap.end())
                edgeMap[key] = ++_idxEnd;

            key = makeKey(indexPtr[1], indexPtr[2]);
            if (edgeMap.find( key ) == edgeMap.end())
                edgeMap[key] = ++_idxEnd;
            
            key = makeKey(indexPtr[2], indexPtr[0]);
            if (edgeMap.find( key ) == edgeMap.end())
                edgeMap[key] = ++_idxEnd;

            indexPtr += 3;
        }

        GLfloat* oldVerts = _vertices;
        GLushort* oldIndices = _indices;

        _numVerts += (int)(faces * 1.5f);
        int newFaces = faces * 4;
        _numIndices = newFaces * 3;

        // Create new indices
        _indices = new GLushort[ _numIndices ];
        GLushort* oldIdxPtr = oldIndices;
        indexPtr = _indices;
        for (f=faces; f; f--)
        {
            GLushort vertA = *oldIdxPtr++;
            GLushort vertB = *oldIdxPtr++;
            GLushort vertC = *oldIdxPtr++;
            GLushort edgeAB = edgeMap[ makeKey(vertA,vertB) ];
            GLushort edgeBC = edgeMap[ makeKey(vertB,vertC) ];
            GLushort edgeCA = edgeMap[ makeKey(vertC,vertA) ];

            *indexPtr++ = vertA;
            *indexPtr++ = edgeAB;
            *indexPtr++ = edgeCA;
            *indexPtr++ = edgeAB;
            *indexPtr++ = vertB;
            *indexPtr++ = edgeBC;
            *indexPtr++ = edgeAB;
            *indexPtr++ = edgeBC;
            *indexPtr++ = edgeCA;
            *indexPtr++ = edgeCA;
            *indexPtr++ = edgeBC;
            *indexPtr++ = vertC;
        }

        // Copy old vertices into new vertices
        _vertices = new GLfloat[ _numVerts * 3 ];
        memcpy( _vertices, oldVerts, vertsSize * sizeof( GLfloat ) );

        // Create new vertices at midpoint of each edge
        EdgeMapType::const_iterator it = edgeMap.begin();
        while (it != edgeMap.end())
        {
            GLushort idxA, idxB;
            idxA = ((*it).first) >> 16;
            idxB = ((*it).first) & 0xffff;

            GLfloat* dest = &(_vertices[ ((*it).second * 3) ]);
            GLfloat* srcA = &(_vertices[idxA*3]);
            GLfloat* srcB = &(_vertices[idxB*3]);
            average3fv( dest, srcA, srcB );

            it++;
        }


        faces = newFaces;
        vertsSize = _numVerts * 3;
        delete[] oldVerts;
        delete[] oldIndices;
    }


    //
    // Create normal array by making vertices unit length
    _normals = new GLfloat[ _numVerts * 3 ];
    GLfloat* vertPtr = _vertices;
    GLfloat* normPtr = _normals;
    for (i=_numVerts; i; i--)
    {
        Vec3d v( vertPtr[0], vertPtr[1], vertPtr[2] );
        float lengthInv = (float)( v.invLen() );
        *normPtr++ = *vertPtr++ * lengthInv;
        *normPtr++ = *vertPtr++ * lengthInv;
        *normPtr++ = *vertPtr++ * lengthInv;
    }

    //
    // Scale vertices out to the specified radius
    vertPtr = _vertices;
    normPtr = _normals;
    for (i=_numVerts*3; i; i--)
        *vertPtr++ = *normPtr++ * _radius;

    //
    // Texture coordinates are identical to normals for cube mapping
    _texCoords = new GLfloat[ _numVerts * 3 ];
    memcpy( _texCoords, _normals, _numVerts * 3 * sizeof( GLfloat) );

    return true;
}

bool
Sphere::storeData()
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
    glTexCoordPointer( 3, GL_FLOAT, 0, _normals );


    glDrawElements( GL_TRIANGLES, _numIndices, GL_UNSIGNED_SHORT, _indices );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}




#ifdef GL_VERSION_1_2


Sphere12::Sphere12( float radius, int subdivisions )
  : Sphere( radius, subdivisions )
{
    assert( OGLDif::instance()->getVersion() >= Ver12 );
}

Sphere12::~Sphere12()
{
}

bool
Sphere12::storeData()
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
    glTexCoordPointer( 3, GL_FLOAT, 0, _texCoords );


    glDrawRangeElements( GL_TRIANGLES, _idxStart, _idxEnd, _numIndices, GL_UNSIGNED_SHORT, _indices );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_3


Sphere13::Sphere13( float radius, int subdivisions )
  : Sphere( radius, subdivisions )
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );
}

Sphere13::~Sphere13()
{
}

bool
Sphere13::storeData()
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
        glTexCoordPointer( 3, GL_FLOAT, 0, _texCoords );
    }


    glDrawRangeElements( GL_TRIANGLES, _idxStart, _idxEnd, _numIndices, GL_UNSIGNED_SHORT, _indices );

    glPopClientAttrib();


    glEndList();


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif




#ifdef GL_VERSION_1_5


// Static indices into the _vbo vertex buffer array
int Sphere15::_vertIdx( 0 );
int Sphere15::_normIdx( 1 );
int Sphere15::_texIdx( 2 );
int Sphere15::_idxIdx( 3 );


Sphere15::Sphere15( float radius, int subdivisions )
  : Sphere( radius, subdivisions )
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );
}

Sphere15::~Sphere15()
{
    if ( _valid )
        glDeleteBuffers( 4, _vbo );
}

void
Sphere15::draw()
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
        glTexCoordPointer( 3, GL_FLOAT, 0, bufferObjectPtr( 0 ) );
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[_idxIdx] );
    glDrawRangeElements( GL_TRIANGLES, _idxStart, _idxEnd, _numIndices, GL_UNSIGNED_SHORT, 0 );


    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glPopClientAttrib();


    OGLDIF_CHECK_ERROR;
}


bool
Sphere15::storeData()
{
    glGenBuffers( 4, _vbo );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_normIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _normals, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_vertIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _vertices, GL_STATIC_DRAW );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo[_texIdx] );
    glBufferData( GL_ARRAY_BUFFER, _numVerts*3*sizeof(GLfloat), _normals, GL_STATIC_DRAW );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo[_idxIdx] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numIndices*sizeof(GLushort), _indices, GL_STATIC_DRAW );


    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );


    OGLDIF_CHECK_ERROR;

    return( _valid = true );
}


#endif


}
