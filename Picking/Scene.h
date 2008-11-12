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

#ifndef __SCENE_H__
#define __SCENE_H__

#include "OGLDMath.h"
#include <vector>


namespace ogld
{
    class Shape;
}


class ShapeInfo
{
public:
    ShapeInfo( ogld::Shape* draw, ogld::Shape* pick );
    ~ShapeInfo();

    ogld::Vec3d _trans;
    ogld::Shape* _draw;
    ogld::Shape* _pick;
};

class Scene
{
public:
    Scene();
    ~Scene();

    void render();
    void pick();

    void reposition( const ogld::Vec3d& up, const ogld::Vec3d& right, int dX, int dY );
    void reposition( const ogld::Vec3d& delta );

    void select( int idx );
    int getSelect();

private:
    typedef std::vector<ShapeInfo*> ShapeList;
    ShapeList _shapes;

    int _idx;
};


#endif
