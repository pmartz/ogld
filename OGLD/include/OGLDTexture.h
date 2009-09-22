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

#ifndef __OGLD_TEXTURE_H__
#define __OGLD_TEXTURE_H__


#include "OGLDPlatformGL.h"
#include "OGLDPixels.h"
#include <string>


namespace ogld
{


class Texture : public Pixels
{
public:
    static Texture* create();

    virtual ~Texture();

    virtual bool loadImage( const std::string& fileName );
    virtual bool loadDirect( GLsizei width, GLsizei height, GLubyte* pixels );
    virtual bool reserve( GLsizei width, GLsizei height );

    virtual void unload();

    virtual void apply();

    void setUnit( GLenum unit );
    GLenum getUnit() const;

    void setEnvMode( GLint mode );
    GLint getEnvMode() const;

    GLuint getObject() const;

protected:
    Texture();

    virtual bool init();
    virtual void initTextureObject();

    GLuint _texId;
    bool _textureObjectAllocated;
    GLenum _unit;

    GLint _mode;
};


#ifdef GL_VERSION_1_3

class Texture13 : public Texture
{
    friend class Texture;

public:
    ~Texture13();

    virtual void apply();

protected:
    Texture13();
};

#endif


#ifdef GL_VERSION_1_4

class Texture14 : public Texture13
{
    friend class Texture;

public:
    ~Texture14();

    virtual void initTextureObject();

protected:
    Texture14();
};

#endif


#ifdef GL_VERSION_2_0

class Texture20 : public Texture13
{
    friend class Texture;

public:
    ~Texture20();

    virtual void initTextureObject();

protected:
    Texture20();
};

#endif

}

#endif
