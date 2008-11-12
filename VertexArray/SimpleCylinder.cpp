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
#include "SimpleCylinder.h"
#include "OGLDif.h"
#include "OGLDMath.h"
#include <math.h>
#include <assert.h>

using ogld::OGLDif;



Cylinder::Cylinder( float radius, float length, int slices )
  : _radius( radius ),
	_length( length ),
	_slices( slices ),
	_drawCap1( false ),
	_drawCap2( false ),
	_vertices( NULL ),
	_normals( NULL ),
	_indices( NULL ),
	_cap1Start( NULL ),
	_cap2Start( NULL ),
	_idxStart( 0 ),
	_cap1Idx( 0 ),
	_cap2Idx( 0 ),
	_idxEnd( 0 ),
    _valid( false ),
	_numVerts( 0 ),
	_numCapVerts( 0 )
{
}

Cylinder::~Cylinder()
{
	if ( _valid )
	{
		if (_vertices)
			delete[] _vertices;
		if (_normals)
			delete[] _normals;
		if (_indices)
			delete[] _indices;
		_vertices = _normals = NULL;
		_indices = NULL;
		_valid = false;
	}
}

void Cylinder::cap( bool posZ, bool negZ )
{
	_drawCap1 = posZ;
	_drawCap2 = negZ;
}

void Cylinder::draw()
{
	if (!_valid)
	{
		if (!init())
			return;
	}

    // We're going to modify client state, so save and restore it on
    //   the client sttribute stack.

    // This will restore the previous client enable state for
    //   GL_VERTEX_ARRAY and GL_NORMAL_ARRAY when OpenGL receives the
    //   corresponding glPopClientAttrib() call, below.
	glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );

    // Enable vertex arrays for the data we're sending: vertices and
    //   normals. Note this example doesn't (presently) use normal
    //   data (lighting isn't covered until chapter 4).
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

    // Specify the data to be used for rendering, along with its
    //   format. OpenGL immediately copies the data into client
    //   memory.
	glVertexPointer( 3, GL_FLOAT, 0, _vertices );
	glNormalPointer( GL_FLOAT, 0, _normals );

    // Draw the cylinder body.
	glDrawElements( GL_QUAD_STRIP, _numVerts, GL_UNSIGNED_SHORT, _indices );
    // Draw end caps, if enabled.
	if (_drawCap1)
		glDrawElements( GL_TRIANGLE_FAN, _numCapVerts, GL_UNSIGNED_SHORT, _cap1Start );
	if (_drawCap2)
		glDrawElements( GL_TRIANGLE_FAN, _numCapVerts, GL_UNSIGNED_SHORT, _cap2Start );

	glPopClientAttrib();

    OGLDIF_CHECK_ERROR;
}

bool Cylinder::init()
{
	_numVerts = (_slices + 1) * 2;
	_numCapVerts = _slices + 2;

	const int totalVerts = _numVerts + (_numCapVerts * 2);

    // Allocate memory to hold the vertex array data. We'll pass these
    //   addresses to OpenGL when we need to draw the geometry. Also
    //   need to delete this memory in the class destructor.
	_vertices = new GLfloat[ totalVerts * 3 ];
	_normals = new GLfloat[ totalVerts * 3 ];
	_indices = new GLushort[ totalVerts ];

	const float halfLength = _length * .5f;
	const double delta = _2PI / _slices;

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

		*indexPtr++ = idx++;

		vertPtr[0] = cosTheta * _radius;
		vertPtr[1] = sinTheta * _radius;
		vertPtr[2] = -halfLength;
		vertPtr += 3;

		normPtr[0] = cosTheta;
		normPtr[1] = sinTheta;
		normPtr[2] = 0.f;
		normPtr += 3;

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

	*indexPtr++ = idx++;

	vertPtr[0] = _radius;
	vertPtr[1] = 0.f;
	vertPtr[2] = -halfLength;
	vertPtr += 3;

	normPtr[0] = 1.f;
	normPtr[1] = 0.f;
	normPtr[2] = 0.f;
	normPtr += 3;

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

	*indexPtr++ = idx++;
	_idxEnd = idx;


    OGLDIF_CHECK_ERROR;

	return( _valid = true );
}
