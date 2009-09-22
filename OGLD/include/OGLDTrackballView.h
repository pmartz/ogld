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

#ifndef __OGLD_TRACKBALL_VIEW_H__
#define __OGLD_TRACKBALL_VIEW_H__


#include "OGLDView.h"
#include "OGLDMath.h"


namespace ogld
{


class TrackballView : public View
{
public:
    TrackballView();
    TrackballView( int width, int height );

    virtual void mouseDown( int x, int y );
    virtual void mouse( int x, int y );

    void setExtents( int width, int height );

private:
    int _width, _height;
};


}


#endif
