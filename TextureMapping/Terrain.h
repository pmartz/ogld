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

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "OGLDPlane.h"
#include <string>


// Forward declarations
namespace ogld
{
    class Texture;
}


class Terrain
{
public:
    Terrain();
    ~Terrain();

    void draw() const;

    bool init( const std::string& dataFile );

private:
    ogld::Plane* _hf;
    ogld::Texture* _texture;
};

#endif
