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

#ifndef __OGLD_PIXEL_BUFFER_H__
#define __OGLD_PIXEL_BUFFER_H__


#include "OGLDPlatformGL.h"
#include "OGLDPixels.h"
#include <string>


#if defined( GL_ARB_pixel_buffer_object ) && defined( GL_VERSION_1_5 )
#  define PIXELBUFFER_BUILD_PBO 1
#endif 


namespace ogld
{

class CubeMap;


class PixelBuffer : public Pixels
{
    // Allow CubeMap::init() to access Pixels::init();
    friend class CubeMap;

public:
    PixelBuffer();

private:
    GLuint _pbo;


#ifdef PIXELBUFFER_BUILD_PBO

public:
    virtual ~PixelBuffer();

    virtual void apply();

protected:
    virtual bool init();

#endif // PIXELBUFFER_BUILD_PBO
};


}

#endif
