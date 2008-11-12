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
#include "OGLDPlatformGLU.h"
#include "OGLDif.h"
#include "OGLDPixels.h"
#include "OGLDTexture.h"
#include "OGLDMath.h"
#include <string>
#include <assert.h>



namespace ogld
{


// Static factory creation method
Texture*
Texture::create()
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
#ifdef GL_VERSION_2_0
        return new Texture20();
        break;
#endif

    case Ver15:
    case Ver14:
#ifdef GL_VERSION_1_4
        return new Texture14();
        break;
#endif

    case Ver13:
#ifdef GL_VERSION_1_3
        return new Texture13();
        break;
#endif

    case Ver12:
    case Ver11:
#ifdef GL_VERSION_1_1
        return new Texture();
        break;
#endif

    case Ver10:
    case VerUnknown:
    default:
        assert( false );
        return NULL;
        break;
    }
}

Texture::Texture()
  : _texId( 0 ),
    _textureObjectAllocated( false ),
    _mode( GL_MODULATE )
{
    assert( OGLDif::instance()->getVersion() >= Ver11 );
}

Texture::~Texture()
{
    unload();
}

bool
Texture::loadImage( const std::string& fileName )
{
    Pixels::loadImage( fileName );

    if (_valid)
        initTextureObject();

    return _valid;
}

bool
Texture::loadDirect( GLsizei width, GLsizei height, GLubyte* pixels )
{
    Pixels::loadDirect( width, height, pixels );

    if (_valid)
        initTextureObject();

    return _valid;
}

bool
Texture::reserve( GLsizei width, GLsizei height )
{
    unload();

    _width = width;
    _height = height;

    _valid = true;

    initTextureObject();

    return _valid;
}


void
Texture::unload()
{
    if (_textureObjectAllocated)
        glDeleteTextures( 1, &_texId );
    _texId = 0;
    _textureObjectAllocated = false;

    Pixels::unload();
}

void
Texture::apply()
{
    if (!_valid)
    {
        if (!init())
            return;
    }

    glBindTexture( GL_TEXTURE_2D, _texId );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, _mode );

    OGLDIF_CHECK_ERROR;
}

bool
Texture::init()
{
    Pixels::init();

    if (_valid)
        initTextureObject();

    return _valid;
}

void Texture::initTextureObject()
{
    glGenTextures( 1, &_texId );
    _textureObjectAllocated = true;

    glBindTexture( GL_TEXTURE_2D, _texId );

    bool useMipmaps = (_pixels != NULL);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );


    // Non-power-of-2 texture dimensions not supported. Resample if necessary.
    bool wNPOT = ( (_width & (_width-1)) != 0);
    bool hNPOT = ( (_height & (_height-1)) != 0);
    if (wNPOT || hNPOT)
    {
        GLsizei oldW = _width;
        GLsizei oldH = _height;
        _width = ceilPower2( _width );
        _height = ceilPower2( _height );
        int sizeBytes = size();
        unsigned char* newData = new unsigned char[ sizeBytes ];

        gluScaleImage( _format, oldW, oldH, _type, _pixels,
            _width, _height, _type, (GLvoid*) newData );

        delete[] _pixels;
        _pixels = newData;
    }

    if (_pixels != NULL)
        gluBuild2DMipmaps( GL_TEXTURE_2D, _format, _width, _height,
                _format, _type, _pixels );
    else
        glTexImage2D( GL_TEXTURE_2D, 0, _format, _width, _height,
                0, _format, _type, _pixels );


    OGLDIF_CHECK_ERROR;
}


void
Texture::setUnit( GLenum unit )
{
    _unit = unit;
}
GLenum
Texture::getUnit() const
{
    return _unit;
}

void
Texture::setEnvMode( GLint mode )
{
    _mode = mode;
}
GLint
Texture::getEnvMode() const
{
    return _mode;
}

GLuint
Texture::getObject() const
{
    if (!_textureObjectAllocated)
        return 0;
    return _texId;
}




#ifdef GL_VERSION_1_3


Texture13::Texture13()
  : Texture()
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );

    _unit = GL_TEXTURE0;
}

Texture13::~Texture13()
{
}

void
Texture13::apply()
{
    if (!_valid)
    {
        if (!init())
            return;
    }

    glActiveTexture( _unit );

    glBindTexture( GL_TEXTURE_2D, _texId );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, _mode );

    OGLDIF_CHECK_ERROR;
}


#endif




#ifdef GL_VERSION_1_4


Texture14::Texture14()
  : Texture13()
{
    assert( OGLDif::instance()->getVersion() >= Ver14 );
}

Texture14::~Texture14()
{
}


void Texture14::initTextureObject()
{
    glGenTextures( 1, &_texId );
    _textureObjectAllocated = true;

    glBindTexture( GL_TEXTURE_2D, _texId );

    bool useMipmaps;
    if (_format == GL_DEPTH_COMPONENT)
    {
        useMipmaps = false;

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
        glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else
    {
        useMipmaps = (_pixels != NULL);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }

    // Non-power-of-2 texture dimensions not supported. Resample if necessary.
    bool wNPOT = ( (_width & (_width-1)) != 0);
    bool hNPOT = ( (_height & (_height-1)) != 0);
    if (wNPOT || hNPOT)
    {
        GLsizei oldW = _width;
        GLsizei oldH = _height;
        _width = ceilPower2( _width );
        _height = ceilPower2( _height );
        int sizeBytes = size();
        unsigned char* newData = new unsigned char[ sizeBytes ];

        gluScaleImage( _format, oldW, oldH, _type, _pixels,
            _width, _height, _type, (GLvoid*) newData );

        delete[] _pixels;
        _pixels = newData;
    }

    if (useMipmaps)
        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
    glTexImage2D( GL_TEXTURE_2D, 0, _format, _width, _height,
            0, _format, _type, _pixels );


    OGLDIF_CHECK_ERROR;
}


#endif




#ifdef GL_VERSION_2_0


Texture20::Texture20()
  : Texture13()
{
}

Texture20::~Texture20()
{
}


void Texture20::initTextureObject()
{
    glGenTextures( 1, &_texId );
    _textureObjectAllocated = true;

    glBindTexture( GL_TEXTURE_2D, _texId );

    bool useMipmaps;
    if (_format == GL_DEPTH_COMPONENT)
    {
        useMipmaps = false;

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
        glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }
    else
    {
        useMipmaps = (_pixels != NULL);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    }

    if (useMipmaps)
        glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
    glTexImage2D( GL_TEXTURE_2D, 0, _format, _width, _height,
            0, _format, _type, _pixels );


    OGLDIF_CHECK_ERROR;
}


#endif


}
