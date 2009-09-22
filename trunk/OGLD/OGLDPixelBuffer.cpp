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
#include "OGLDPixelBuffer.h"
#include "OGLDPixels.h"
#include "OGLDif.h"
#include <string>
#include <assert.h>



namespace ogld
{


PixelBuffer::PixelBuffer()
  : Pixels(),
    _pbo( 0 )
{
}


#ifdef PIXELBUFFER_BUILD_PBO

PixelBuffer::~PixelBuffer()
{
    if ( _valid )
    {
        assert( _pbo != 0 );
        glDeleteBuffers( 1, &_pbo );
    }
}

void
PixelBuffer::apply()
{
    if (!_valid)
    {
        if (!init())
            return;
    }

    assert( _pbo != 0 );
    glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, _pbo );
    glDrawPixels( _width, _height, _format, _type, bufferObjectPtr( 0 ) );

    OGLDIF_CHECK_ERROR;
}


bool
PixelBuffer::init()
{
    Pixels::init();

    if (!_valid)
        return false;


    glGenBuffers( 1, &_pbo );
    assert( _pbo != 0 );

    glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, _pbo );
    glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, size(), _pixels, GL_STATIC_DRAW );

    return _valid;
}

#endif // PIXELBUFFER_BUILD_PBO


}
