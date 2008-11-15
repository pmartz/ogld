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

#ifndef __OGLD_ALT_AZ_VIEW_H__
#define __OGLD_ALT_AZ_VIEW_H__


#include "OGLDView.h"
#include "OGLDMath.h"


namespace ogld
{


//
// OGLDAltAzView
//
// Interprets X and Y mouse motion as delta azimuth and altitude
//   angles, and changes the view direction accordingly. Clamps
//   changes in altitude to not exceed +/- PI/2 radians. As a
//   result, this view class is suitable for viewing a scene
//   from all angles, but always maintaining a constant up vector.
//
class AltAzView : public View
{
public:
    AltAzView();

    virtual void mouse( int x, int y );

    virtual void setDirUp( const Vec3d& dir, const Vec3d& up );

private:
    double _alt, _az;
    Vec3d _defDir, _defUp;
};


}


#endif
