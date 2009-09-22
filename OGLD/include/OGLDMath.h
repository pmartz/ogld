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

#ifndef __OGLD_MATH_H__
#define __OGLD_MATH_H__


namespace ogld
{


// Constants
#define _PI (3.1415926535897932384626433832795)
#define _2PI (6.283185307179586476925286766559)
#define _PIover2 (1.5707963267948966192313216916398)
#define _PIover4 (0.78539816339744830961566084581988)


//
// Conversion functions
inline double degreesToRadians( const double d )
{
	return ( d * _PI / 180. );
}
inline double radiansToDegrees( const double r )
{
	return ( r * 180. / _PI );
}


//
// Functions
unsigned short ceilPower2( unsigned short x );

void matrixTranspose( double m[16] );



//
// 3D vector class
class Vec3d
{
public:
    Vec3d();
    Vec3d( const Vec3d& a );
    Vec3d( const double x, const double y, const double z );
    ~Vec3d();

    void scale( const double s );
    void normalize();
    void rotate( const Vec3d& axis, double angleRads );
    double dot( const Vec3d& a ) const;

    double sqLen() const;
    double len() const;
    double invLen() const;

    Vec3d& operator=( const Vec3d& a );
    bool operator==( const Vec3d& a ) const;
    Vec3d operator+( const Vec3d& a ) const;
    Vec3d& operator+=( const Vec3d& a );
    Vec3d operator-( const Vec3d& a ) const;
    Vec3d& operator-=( const Vec3d& a );
    double& operator[]( int idx );

    static Vec3d average( const Vec3d& a, const Vec3d& b );
    static Vec3d cross( const Vec3d& a, const Vec3d& b );

private:
    double _v[3];
};


}

#endif
