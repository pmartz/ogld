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

#ifndef __OGLD_VIEW_H__
#define __OGLD_VIEW_H__


#include "OGLDMath.h"


namespace ogld
{


class View
{
public:
    View();
    virtual ~View() {};

    virtual void getView( double* dir, double* up, double* cross, double* pos ) const;
    virtual void getView( Vec3d& dir, Vec3d& up, Vec3d& cross, Vec3d& pos ) const;

    virtual void loadMatrix() const;
    virtual void multMatrix() const;
    virtual void loadInverseAffine() const;
    virtual void multInverseAffine() const;

    virtual void setDirUp( const Vec3d& dir, const Vec3d& up );
    virtual void getDirUp( Vec3d& dir, Vec3d& up );

    virtual void setDistance( double distance );
    virtual double getDistance() const;

    virtual void setCenter( const Vec3d& center );
    virtual const Vec3d& getCenter() const;

    virtual void mouseDown( int x, int y );
    virtual void mouse( int x, int y );

protected:
    Vec3d _dir;
    Vec3d _up;
    double _distance;
    Vec3d _center;

    int _lastX, _lastY;
};


}


#endif
