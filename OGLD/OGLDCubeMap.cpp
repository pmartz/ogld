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
#include "OGLDCubeMap.h"
#include "OGLDMath.h"
#include <string>
#include <exception>
#include <assert.h>



namespace ogld
{


// Static factory creation method
CubeMap*
CubeMap::create()
{
    switch (OGLDif::instance()->getVersion())
    {

    case VerExtended:
    case Ver20:
    case Ver15:
    case Ver14:
#ifdef GL_VERSION_1_4
        return new CubeMap14();
        break;
#endif

    case Ver13:
#ifdef GL_VERSION_1_3
        return new CubeMap13();
        break;
#endif

    case Ver12:
    case Ver11:
#ifdef GL_VERSION_1_1
        return new CubeMap();
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


CubeMap::CubeMap()
  : _texId( 0 ),
    _textureObjectAllocated( false ),
    _mode( GL_MODULATE )
{
    assert( OGLDif::instance()->getVersion() >= Ver11 );

    _pixels.resize( 6 );
    int i;
    for (i=0; i<6; i++)
        _pixels[i] = new ogld::Pixels;
}

CubeMap::~CubeMap()
{
    unload();

    int i;
    for (i=0; i<6; i++)
        delete _pixels[i];
}

void
CubeMap::unload( GLenum target )
{
}

void
CubeMap::unload()
{
}

bool
CubeMap::loadImage( GLenum target, const std::string& fileName )
{
    return false;
}

bool
CubeMap::loadDirect( GLenum target, GLsizei width, GLsizei height, GLubyte* pixels )
{
    return false;
}


void
CubeMap::apply()
{
}


bool
CubeMap::valid( GLenum target ) const
{
    return false;
}

bool
CubeMap::valid() const
{
    int i;
    for( i=0; i<6; i++ )
    {
        if (!_pixels[i]->valid())
            return false;
    }

    return true;
}


void
CubeMap::setFormat( GLenum format )
{
    int i;
    for( i=0; i<6; i++ )
        _pixels[i]->setFormat( format );
}

GLenum
CubeMap::getFormat() const
{
    return _pixels[0]->getFormat();
}


void
CubeMap::setType( GLenum type )
{
    int i;
    for( i=0; i<6; i++ )
        _pixels[i]->setType( type );
}

GLenum
CubeMap::getType() const
{
    return _pixels[0]->getType();
}


void
CubeMap::setUnit( GLenum unit )
{
    _unit = unit;
}
GLenum
CubeMap::getUnit() const
{
    return _unit;
}


void
CubeMap::setEnvMode( GLint mode )
{
    _mode = mode;
}
GLint
CubeMap::getEnvMode() const
{
    return _mode;
}


GLuint
CubeMap::getObject() const
{
    if (!_textureObjectAllocated)
        return 0;
    return _texId;
}


bool
CubeMap::init()
{
    return false;
}

int
CubeMap::size() const
{
    return _pixels[0]->size();
}

void
CubeMap::initTextureObject()
{
}




#ifdef GL_VERSION_1_3


CubeMap13::CubeMap13()
  : CubeMap()
{
    assert( OGLDif::instance()->getVersion() >= Ver13 );
}

CubeMap13::~CubeMap13()
{
}

void
CubeMap13::unload( GLenum target )
{
    _pixels[ target - GL_TEXTURE_CUBE_MAP_POSITIVE_X ]->unload();
}

void
CubeMap13::unload()
{
    int i;
    for (i=0; i<6; i++)
        unload( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i );
}

bool
CubeMap13::loadImage( GLenum target, const std::string& fileName )
{
    ogld::Pixels* pixels = _pixels[ target - GL_TEXTURE_CUBE_MAP_POSITIVE_X ];

    pixels->loadImage( fileName );
    if (CubeMap::valid())
        initTextureObject();

    return pixels->valid();
}

bool
CubeMap13::loadDirect( GLenum target, GLsizei width, GLsizei height, GLubyte* pixels )
{
    ogld::Pixels* pix = _pixels[ target - GL_TEXTURE_CUBE_MAP_POSITIVE_X ];

    pix->loadDirect( width, height, pixels );
    if (CubeMap::valid())
        initTextureObject();

    return pix->valid();
}


void
CubeMap13::apply()
{
    if (!CubeMap::valid())
    {
        if (!init())
            return;
    }

    glActiveTexture( _unit );

    glBindTexture( GL_TEXTURE_CUBE_MAP, _texId );

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, _mode );

    OGLDIF_CHECK_ERROR;
}


bool
CubeMap13::valid( GLenum target ) const
{
    return _pixels[ target - GL_TEXTURE_CUBE_MAP_POSITIVE_X ]->valid();
}

bool
CubeMap13::init()
{
    int i;
    for( i=0; i<6; i++ )
    {
        if (!_pixels[i]->valid())
        {
            // Create default environment map for specular highlight at +X.
            int size( 64 );
            GLubyte* face = new GLubyte[size*size];
            memset( face, 0, size*size );

            if (i==0)
            {
                // This is the face for the positive X axis.
                //   Add a round white circle / light source
                //   in the middle of this face.
                int c = size>>1;
                int sqRad = 32;
                int xIdx;
                for (xIdx=0; xIdx<size; xIdx++)
                {
                    int yIdx;
                    for (yIdx=0; yIdx<size; yIdx++)
                    {
                        int xD( xIdx - c );
                        int yD( yIdx - c );
                        if (xD*xD + yD*yD <= sqRad)
                            face[ xIdx*size + yIdx ] = 255;
                    }
                }
            }

            _pixels[i]->setFormat( GL_LUMINANCE );
            _pixels[i]->setType( GL_UNSIGNED_BYTE );
            _pixels[i]->loadDirect( size, size, face );
            delete[] face;
        }
    }

    if (CubeMap::valid())
        initTextureObject();

    return CubeMap::valid();
}

void
CubeMap13::initTextureObject()
{
    glGenTextures( 1, &_texId );
    _textureObjectAllocated = true;

    glBindTexture( GL_TEXTURE_CUBE_MAP, _texId );

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    int i;
    for( i=0; i<6; i++)
    {
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

        GLsizei width, height;
        _pixels[i]->getWidthHeight( width, height );
        GLenum format = _pixels[i]->getFormat();
        GLenum type = _pixels[i]->getType();

        // Non-power-of-2 texture dimensions not supported. Resample if necessary.
        bool wNPOT = ( (width & (width-1)) != 0);
        bool hNPOT = ( (height & (height-1)) != 0);
        if (wNPOT || hNPOT)
        {
            GLsizei oldW = width;
            GLsizei oldH = height;
            width = ceilPower2( width );
            height = ceilPower2( height );
            int sizeBytes = _pixels[i]->size();
            unsigned char* newData = new unsigned char[ sizeBytes ];

            gluScaleImage( format, oldW, oldH, type, _pixels[i]->getPixels(),
                width, height, type, (GLvoid*) newData );

            _pixels[i]->loadDirect( width, height, newData );
            delete[] newData;
        }

        gluBuild2DMipmaps( target, format, width, height,
                format, type, _pixels[i]->getPixels() );
    }

    OGLDIF_CHECK_ERROR;
}


#endif




#ifdef GL_VERSION_1_4


CubeMap14::CubeMap14()
  : CubeMap13()
{
    assert( OGLDif::instance()->getVersion() >= Ver14 );
}

CubeMap14::~CubeMap14()
{
}

void
CubeMap14::initTextureObject()
{
    glGenTextures( 1, &_texId );
    _textureObjectAllocated = true;

    glBindTexture( GL_TEXTURE_CUBE_MAP, _texId );

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    int i;
    for( i=0; i<6; i++)
    {
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

        GLsizei width, height;
        _pixels[i]->getWidthHeight( width, height );
        GLenum format = _pixels[i]->getFormat();
        GLenum type = _pixels[i]->getType();

        // Non-power-of-2 texture dimensions not supported. Resample if necessary.
        bool wNPOT = ( (width & (width-1)) != 0);
        bool hNPOT = ( (height & (height-1)) != 0);
        if (wNPOT || hNPOT)
        {
            GLsizei oldW = width;
            GLsizei oldH = height;
            width = ceilPower2( width );
            height = ceilPower2( height );
            int sizeBytes = _pixels[i]->size();
            unsigned char* newData = new unsigned char[ sizeBytes ];

            gluScaleImage( format, oldW, oldH, type, _pixels[i]->getPixels(),
                width, height, type, (GLvoid*) newData );

            _pixels[i]->loadDirect( width, height, newData );
            delete[] newData;
        }

        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE );
        glTexImage2D( target, 0, format, width, height,
                0, format, type, _pixels[i]->getPixels() );
    }

    OGLDIF_CHECK_ERROR;
}


#endif




#ifdef GL_VERSION_2_0


CubeMap20::CubeMap20()
  : CubeMap13()
{
}

CubeMap20::~CubeMap20()
{
}

void
CubeMap20::initTextureObject()
{
    glGenTextures( 1, &_texId );
    _textureObjectAllocated = true;

    glBindTexture( GL_TEXTURE_CUBE_MAP, _texId );

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    int i;
    for( i=0; i<6; i++)
    {
        GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

        GLsizei width, height;
        _pixels[i]->getWidthHeight( width, height );
        GLenum format = _pixels[i]->getFormat();
        GLenum type = _pixels[i]->getType();

        glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE );
        glTexImage2D( target, 0, format, width, height,
                0, format, type, _pixels[i]->getPixels() );
    }

    OGLDIF_CHECK_ERROR;
}


#endif

}
