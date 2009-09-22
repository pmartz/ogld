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
#include "OGLDif.h"
#include "OGLDShape.h"
#include <algorithm>



namespace ogld
{


Shape::Shape()
  : _numTextures( 1 )
{
}

Shape::~Shape()
{
}


void
Shape::setTextures( int numTextures )
{
    _numTextures = numTextures;
}

int
Shape::getTextures() const
{
    return _numTextures;
}


}
