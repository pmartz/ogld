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

#include "OGLDAltAzView.h"
#include "OGLDMath.h"
#include <math.h>



namespace ogld
{


AltAzView::AltAzView()
  : View(),
    _alt( 0. ),
    _az( 0. )
{
    getDirUp( _defDir, _defUp );
}


void
AltAzView::mouse( int x, int y )
{
    const int deltaX = x - _lastX;
    const int deltaY = y - _lastY;
    mouseDown( x, y );

    // Turn the deltas into radians. Multiplying by 0.01 means,
    //   for example, that moving the mouse 314 pixels will
    //   rotate by (approximately) PI radisna.
    const double deltaAlt = -deltaY * 0.01;
    const double deltaAz = -deltaX * 0.01;

    // Compute the new altitude angle. Clamp to avoid wrapping
    //   around the up vector (or its negation).
    _alt += deltaAlt;
    if (_alt > _PIover2)
        _alt = _PIover2;
    else if (_alt < -_PIover2)
        _alt = -_PIover2;

    // Compute the new azimuth angle in the range 0 < az < 2*PI.
    _az += deltaAz;
    while (_az >= _2PI)
        _az -= _2PI;
    while (_az < 0.)
        _az += _2PI;

    // Compute the view direction vector by starting with the
    //   original direction vector, and rotating it by our
    //   updated altitude and azimuth angles.
    Vec3d cross = Vec3d::cross( _defDir, _defUp );
    _dir = _defDir;
    _dir.rotate( cross, _alt );
    _dir.rotate( _defUp, _az );
    _dir.normalize();
}

void
AltAzView::setDirUp( const Vec3d& dir, const Vec3d& up )
{
    // Get normalized up and view direction vectors
    View::setDirUp( dir, up );
    getDirUp( _defDir, _defUp );

    // Our initial altitude angle is computed as:
    //   1. Take the dot product of the up and view dir vectors.
    //      This yields the cosine of the angle between them.
    //   2. The arccosine of the dot product produces the angle
    //      in radians of the view dir away from the "pole".
    //   3. To get angle away from the "equator", subtract the angle
    //      computed in step 2 from PI/2.
    _alt = _PIover2 - acos( _defUp.dot( _defDir ) );

    // Next, compute a default direction vector that is at the
    //   "equator" (PI/2 radians away from the "pole").
    Vec3d cross = Vec3d::cross( _defDir, _defUp );
    _defDir = Vec3d::cross( _defUp, cross );

    // Our default azimuth angle is initially zero.
    _az = 0.;
}


}
