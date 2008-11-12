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

#ifndef __OGLD_PIXELS_H__
#define __OGLD_PIXELS_H__


#include "OGLDPlatformGL.h"
#include <string>


namespace ogld
{

class CubeMap;


class Pixels
{
    // Allow CubeMap classes to access Pixels::init(), Pixels::size()
    friend class CubeMap;
#ifdef GL_VERSION_1_3
    friend class CubeMap13;
#endif
#ifdef GL_VERSION_1_4
    friend class CubeMap14;
#endif
#ifdef GL_VERSION_2_0
    friend class CubeMap20;
#endif

public:
    typedef enum {
        NoPBO, UsePBOIfAvailable, ForcePBO
    } PixelsProduct;

    static Pixels* create( PixelsProduct product=UsePBOIfAvailable );
    Pixels();
    virtual ~Pixels();

    virtual bool loadImage( const std::string& fileName );
    virtual bool loadDirect( GLsizei width, GLsizei height, GLubyte* pixels );

    virtual void unload();

    virtual void apply();

    bool valid() const { return _valid; }

    void getWidthHeight( GLsizei& width, GLsizei& height ) const;

    void setFormat( GLenum format );
    GLenum getFormat() const;

    void setType( GLenum type );
    GLenum getType() const;

    const unsigned char* getPixels() const { return _pixels; }

protected:
    virtual bool init();
    int size() const;

    unsigned char* _pixels;
    GLsizei _width;
    GLsizei _height;
    GLenum _format;
    GLenum _type;

    bool _valid;
};


}

#endif
