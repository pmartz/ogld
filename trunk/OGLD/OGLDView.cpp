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
#include "OGLDView.h"
#include "OGLDMath.h"



namespace ogld
{


View::View()
  : _distance( 6. ),
    _center( 0., 0., 0. )
{
    setDirUp( Vec3d( 0., 0., -1. ), Vec3d( 0., 1., 0. ) );
}

void
View::getView( double* dir, double* up, double* cross, double* pos ) const
{
    Vec3d dV, uV, cV, pV;
    getView( dV, uV, cV, pV );

    dir[0] = dV[0];
    dir[1] = dV[1];
    dir[2] = dV[2];

    up[0] = uV[0];
    up[1] = uV[1];
    up[2] = uV[2];

    cross[0] = cV[0];
    cross[1] = cV[1];
    cross[2] = cV[2];

    pos[0] = pV[0];
    pos[1] = pV[1];
    pos[2] = pV[2];
}

void
View::getView( Vec3d& dir, Vec3d& up, Vec3d& cross, Vec3d& pos ) const
{
    dir = _dir;

    cross = Vec3d::cross( _dir, _up );
    cross.normalize();

    up = Vec3d::cross( cross, _dir );
    up.normalize();

    pos = _dir;
    pos.scale( -_distance );
    pos += _center;
}

void
View::loadMatrix() const
{
    glLoadIdentity();
    multMatrix();
}

void
View::multMatrix() const
{
    double dir[3], up[3], cross[3], pos[3];

    getView( dir, up, cross, pos );

    GLdouble m[16];
    m[0]=cross[0]; m[4]=cross[1]; m[8]=cross[2]; m[12] = 0.f;
    m[1]=up[0]; m[5]=up[1]; m[9]=up[2]; m[13] = 0.f;
    m[2]=-dir[0]; m[6]=-dir[1]; m[10]=-dir[2]; m[14] = 0.f;
    m[3]=0.f; m[7]=0.f; m[11]=0.f; m[15] = 1.f;

    glMultMatrixd( m );
    glTranslated( -pos[0], -pos[1], -pos[2] );
}

void
View::loadInverseAffine() const
{
    glLoadIdentity();
    multInverseAffine();
}

void
View::multInverseAffine() const
{
    double dir[3], up[3], cross[3], pos[3];

    getView( dir, up, cross, pos );

    GLdouble m[16];
    m[0]=cross[0]; m[1]=cross[1]; m[2]=cross[2]; m[3] = 0.f;
    m[4]=up[0]; m[5]=up[1]; m[6]=up[2]; m[7] = 0.f;
    m[8]=-dir[0]; m[9]=-dir[1]; m[10]=-dir[2]; m[11] = 0.f;
    m[12]=0.f; m[13]=0.f; m[14]=0.f; m[15] = 1.f;

    glMultMatrixd( m );
}


void
View::setDirUp( const Vec3d& dir, const Vec3d& up )
{
    _dir = dir;
    _dir.normalize();
    _up = up;
    _up.normalize();
}

void
View::getDirUp( Vec3d& dir, Vec3d& up )
{
    dir = _dir;
    up = _up;
}

void
View::setDistance( double distance )
{
    _distance = distance;
    if (_distance < 1.)
        _distance = 1.;
}

double
View::getDistance() const
{
    return _distance;
}

void
View::setCenter( const Vec3d& center )
{
    _center = center;
}

const Vec3d&
View::getCenter() const
{
    return _center;
}

void
View::mouseDown( int x, int y )
{
    _lastX = x;
    _lastY = y;
}

void
View::mouse( int x, int y )
{
    // No-op in View base class, but View is still useful as 
    //   a standalone class, so define this function rather than
    //   make it pure virtual.
}


}
