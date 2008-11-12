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

#include "OGLDMath.h"
#include <math.h>



namespace ogld
{


//
// ceilPower2
//
// Return next highest power of 2 greater than x
//   if x is a power of 2, return the -next- highest power of 2.
unsigned short ceilPower2( unsigned short x )
{
    if (x == 0)
        return 1;

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    return x+1;
}



//
// matrixTranspose
//
// Transposes a 4x4 matrix.
void
matrixTranspose( double m[16] )
{
    double t;

#define SWAP(_a,_b) \
{ t = _a; _a = _b; _b = t; }

    SWAP( m[1], m[4] );
    SWAP( m[2], m[8] );
    SWAP( m[3], m[12] );
    SWAP( m[6], m[9] );
    SWAP( m[7], m[13] );
    SWAP( m[11], m[14] );
}


Vec3d::Vec3d()
{
    _v[0] = _v[1] = _v[2] = 0.f;
}

Vec3d::Vec3d( const Vec3d& a )
{
    _v[0] = a._v[0];
    _v[1] = a._v[1];
    _v[2] = a._v[2];
}

Vec3d::Vec3d( const double x, const double y, const double z )
{
    _v[0] = x;
    _v[1] = y;
    _v[2] = z;
}

Vec3d::~Vec3d()
{
}

void
Vec3d::scale( const double s )
{
    _v[0] *= s;
    _v[1] *= s;
    _v[2] *= s;
}

void
Vec3d::normalize()
{
    scale( invLen() );
}

void
Vec3d::rotate( const Vec3d& axis, double angleRads )
{
    const double cosAngle = cos( angleRads );
    const double sinAngle = sin( angleRads );

    Vec3d normAxis( axis );
    normAxis.normalize();

    Vec3d cross = Vec3d::cross( *this, normAxis );
    Vec3d cross2 = Vec3d::cross( cross, normAxis );
    cross2.scale( (double)(1. - cosAngle) );
    cross.scale( (double)sinAngle );

    *this += cross2 - cross;
}

double
Vec3d::dot( const Vec3d& a ) const
{
    return _v[0]*a._v[0] + _v[1]*a._v[1] + _v[2]*a._v[2];
}

double
Vec3d::sqLen() const
{
    return dot( *this );
}

double
Vec3d::len() const
{
    return (double)( sqrt( sqLen() ) );
}

double
Vec3d::invLen() const
{
    return (double)( 1. / sqrt( sqLen() ) );
}

Vec3d&
Vec3d::operator=( const Vec3d& a )
{
    _v[0] = a._v[0];
    _v[1] = a._v[1];
    _v[2] = a._v[2];
    return *this;
}

bool
Vec3d::operator==( const Vec3d& a ) const
{
    return( (_v[0] == a._v[0]) && (_v[1] == a._v[1]) && (_v[2] == a._v[2]) );
}

Vec3d
Vec3d::operator+( const Vec3d& a ) const
{
    Vec3d r( *this );
    r += a;
    return r;
}

Vec3d&
Vec3d::operator+=( const Vec3d& a )
{
    _v[0] += a._v[0];
    _v[1] += a._v[1];
    _v[2] += a._v[2];
    return *this;
}

Vec3d
Vec3d::operator-( const Vec3d& a ) const
{
    Vec3d r( *this );
    r -= a;
    return r;
}

Vec3d&
Vec3d::operator-=( const Vec3d& a )
{
    _v[0] -= a._v[0];
    _v[1] -= a._v[1];
    _v[2] -= a._v[2];
    return *this;
}

double&
Vec3d::operator[]( int idx )
{
    return _v[ idx ];
}

Vec3d
Vec3d::average( const Vec3d& a, const Vec3d& b )
{
    Vec3d result( (a._v[0] + b._v[0]) * .5f,
            (a._v[1] + b._v[1]) * .5f,
            (a._v[2] + b._v[2]) * .5f );
    return result;
}

Vec3d
Vec3d::cross( const Vec3d& a, const Vec3d& b )
{
    Vec3d result( a._v[1]*b._v[2] - a._v[2]*b._v[1],
            a._v[2]*b._v[0] - a._v[0]*b._v[2],
            a._v[0]*b._v[1] - a._v[1]*b._v[0] );
    return result;
};


}

