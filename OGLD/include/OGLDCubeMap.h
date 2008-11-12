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

#ifndef __OGLD_CUBE_MAP_H__
#define __OGLD_CUBE_MAP_H__


#include "OGLDPlatformGL.h"
#include "OGLDPixels.h"
#include <string>
#include <vector>


namespace ogld
{


class CubeMap
{
public:
    static CubeMap* create();

    virtual ~CubeMap();

    virtual bool loadImage( GLenum target, const std::string& fileName );
    virtual bool loadDirect( GLenum target, GLsizei width, GLsizei height, GLubyte* pixels );

    virtual void unload( GLenum target );
    virtual void unload();

    virtual void apply();

    virtual bool valid( GLenum target ) const;
    bool valid() const;

    void setFormat( GLenum format );
    GLenum getFormat() const;

    void setType( GLenum type );
    GLenum getType() const;

    void setUnit( GLenum unit );
    GLenum getUnit() const;

    void setEnvMode( GLint mode );
    GLint getEnvMode() const;

    GLuint getObject() const;

protected:
    CubeMap();

    virtual bool init();
    int size() const;
    virtual void initTextureObject();

    GLuint _texId;
    bool _textureObjectAllocated;
    GLenum _unit;

    std::vector<ogld::Pixels*> _pixels;
    GLint _mode;

    bool _valid;
};


#ifdef GL_VERSION_1_3

class CubeMap13 : public CubeMap
{
    friend class CubeMap;

public:
    virtual ~CubeMap13();

    virtual bool loadImage( GLenum target, const std::string& fileName );
    virtual bool loadDirect( GLenum target, GLsizei width, GLsizei height, GLubyte* pixels );

    virtual void unload( GLenum target );
    virtual void unload();

    virtual void apply();

    virtual bool valid( GLenum target ) const;

protected:
    CubeMap13();

    virtual bool init();
    virtual void initTextureObject();
};

#endif


#ifdef GL_VERSION_1_4

class CubeMap14 : public CubeMap13
{
    friend class CubeMap;

public:
    virtual ~CubeMap14();

protected:
    CubeMap14();

    virtual void initTextureObject();
};

#endif


#ifdef GL_VERSION_2_0

class CubeMap20 : public CubeMap13
{
    friend class CubeMap;

public:
    virtual ~CubeMap20();

protected:
    CubeMap20();

    virtual void initTextureObject();
};

#endif

}

#endif
