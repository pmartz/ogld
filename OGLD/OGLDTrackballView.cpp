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

#include "OGLDTrackballView.h"
#include "OGLDMath.h"
#include <math.h>



namespace ogld
{


TrackballView::TrackballView()
  : View()
{
    // Assume an effective trackball surface area of 100x100 pixels.
    //   The owning application will typically set this to the extent
    //   of the window, and reset it if the window size changes.
    setExtents( 100, 100 );
}

TrackballView::TrackballView( int width, int height )
  : View()
{
    setExtents( width, height );
}


void
TrackballView::mouseDown( int x, int y )
{
    // Record the initial x and y mouse positions.
    //   Save the y value in OpenGL space (origin at bottom).
    _lastX = x;
    _lastY = _height - y;
}

void
TrackballView::mouse( int x, int y )
{
    // Prepare to operate in a pixel coordinate system with origin at
    //   the center of the trackball active surface.
    const int halfWidth = _width >> 1;
    const int halfHeight = _height >> 1;

    // Get the old mouse xy position as a vector in this coordinate
    //   system.
    int oldX = _lastX - halfWidth;
    int oldY = _lastY - halfHeight;
    //   Save current xy as (lastX,lastY).
    mouseDown( x, y );

    // Get the new mouse xy position as a vector in this coordinate
    //   system.
    y = _height - y;
    int newX = x - halfWidth;
    int newY = y - halfHeight;

    Vec3d origin( oldX, oldY, 0. );

    // Compute the delta motion and absolute values.
    double deltaX = newX - oldX;
    double deltaY = newY - oldY;
    double len = sqrt( deltaX*deltaX + deltaY*deltaY );
    if (len == 0.)
        return;
    double absX = (deltaX < 0.) ? -deltaX : deltaX;
    double absY = (deltaY < 0.) ? -deltaY : deltaY;

    // Derive an axis of rotation to mimic a trackball, that is, the
    //   user can rotate around an axis that is closer to the view
    //   direction by positioning the mouse away from the window
    //   center. Mouse position closer to the center uses an axis of
    //   rotation nearly orthogonal to the view direction. Axis of
    //   rotation is always orthogonal to the delta mouse motion.

    // First, compute 'ballAxis', which is coincident with the mouse
    //   delta vector, but in the view's current coordinate space.
    const double percentX = deltaX / (absX + absY);
    const double percentY = deltaY / (absX + absY);
    Vec3d crossB = Vec3d::cross( _dir, _up );
    crossB.scale( percentX );
    Vec3d upB( _up );
    upB.scale( percentY );
    Vec3d ballAxis = Vec3d::average( crossB, upB );

    // Determine the distance the mouse is away from the center, use
    //   that distance to derive an angle to rotate a vector around
    //   ballAxis.
    Vec3d axis = ballAxis;
    axis.rotate( _dir, -_PIover2 );
    axis.normalize();
    Vec3d screenAxis( -deltaY, deltaX, 0. );
    screenAxis.normalize();
    double dp = screenAxis.dot( origin );
    double angle = -dp / halfWidth * _PIover4;

    // 'axis' will be our desired axis of rotation.
    axis.rotate( ballAxis, angle );
    axis.normalize();

    // Finally, rotate the _dir and _up vectors around the computed
    //   tackball axis of rotation. Angle is in radians, so 314 pixels
    //   of motion rotates ~pi radians or ~180 degrees.
    _dir.rotate( axis, (-len * .01) );
    _up.rotate( axis, (-len * .01) );
}

void
TrackballView::setExtents( int width, int height )
{
    _width = width;
    _height = height;
}


}
