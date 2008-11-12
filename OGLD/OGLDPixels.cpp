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
#include "OGLDPixels.h"
#include "OGLDPixelBuffer.h"
#include "tiffio.h"
#include <string>
#include <assert.h>



namespace ogld
{



// static parameterized factory method
Pixels*
Pixels::create( PixelsProduct product )
{
    if (product == NoPBO)
        return new Pixels();

    const std::string pboStr( "GL_ARB_pixel_buffer_object" );
    const bool pboAvailable = ( (OGLDif::instance()->getVersion() >= Ver15) &&
            (OGLDif::instance()->isExtensionSupported(
            pboStr, glGetString( GL_EXTENSIONS ) )) );

    if (pboAvailable)
        return new PixelBuffer();

    if (product == UsePBOIfAvailable)
        // PBO not supported, use Pixels instead
        return new Pixels();
    
    // product == ForcePBO, but it's not supported
    return NULL;
}

Pixels::Pixels()
  : _pixels( NULL ),
    _width( 0 ),
    _height( 0 ),
    _format( GL_RGBA ),
    _type( GL_UNSIGNED_BYTE ),
    _valid( false )
{
}

Pixels::~Pixels()
{
    unload();
}

void
Pixels::unload()
{
    if (_pixels)
    {
        delete[] _pixels;
        _pixels = NULL;
    }

    _valid = false;
}

static void
copyRow( unsigned char* ptr, unsigned char* data, int n, int numSamples )
{
    while (n--)
    {
        int idx;
        for (idx = 0; idx < numSamples; idx++)
            *ptr++ = *data++;
        if (numSamples == 3)
            *ptr++ = 255;
    }
}

bool
Pixels::loadImage( const std::string& fileName )
{
    unload();

    TIFF* tif = NULL;
    int idx;
    for( idx = 0; idx < ogld::OGLDif::instance()->getDataPathSize(); idx++)
    {
        std::string fullPath = ogld::OGLDif::instance()->getDataPath( idx ) + fileName;
        tif = TIFFOpen( fullPath.c_str(), "r" );
        if (tif)
            break;
    }
    if (!tif)
        return false;

    uint32 w, h;
    size_t npixels;
    uint16 photometric;
    uint16 config;
    uint16 samples;

    TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &photometric );
    assert( photometric == PHOTOMETRIC_RGB );
    TIFFGetField( tif, TIFFTAG_PLANARCONFIG, &config );
    assert( config == PLANARCONFIG_CONTIG );

    TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, &samples );

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &w );
    TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &h );
    npixels = w * h;

    _width = w;
    _height = h;
    int sizeBytes = size();
    _pixels = new unsigned char[ sizeBytes ];
    unsigned char* ptr = _pixels + (h-1)*w*4;

    unsigned char* inBuf = new unsigned char[ TIFFScanlineSize( tif ) ];
    int row;
    for (row = 0; row < (int)h; row++)
    {
        if (TIFFReadScanline( tif, inBuf, row, 0 ) < 0)
        {
            assert( false );
            break;
        }
        copyRow( ptr, inBuf, w, samples );
        ptr -= 4*w;
    }
    delete[] inBuf;
    _valid = true;


    TIFFClose( tif );

    init();

    return _valid;
}

bool
Pixels::loadDirect( GLsizei width, GLsizei height, GLubyte* pixels )
{
    unload();

    _width = width;
    _height = height;

    int sizeBytes = size();

    _pixels = new unsigned char[ sizeBytes ];
    memcpy( _pixels, pixels, sizeBytes );

    _valid = true;

    return _valid;
}


void
Pixels::apply()
{
    if (!_valid)
    {
        if (!init())
            return;
    }

    glDrawPixels( _width, _height, _format, _type, (GLvoid*)_pixels );

    OGLDIF_CHECK_ERROR;
}

void
Pixels::getWidthHeight( GLsizei& width, GLsizei& height ) const
{
    width = _width;
    height = _height;
}

void
Pixels::setFormat( GLenum format )
{
    _format = format;
    if (_valid)
        unload();
}
GLenum
Pixels::getFormat() const
{
    return _format;
}

void
Pixels::setType( GLenum type )
{
    _type = type;
    if (_valid)
        unload();
}
GLenum
Pixels::getType() const
{
    return _type;
}


bool
Pixels::init()
{
    if (_valid)
        return true;

    _width = _height = 256;
    int tile = _width / 8;
    _format = GL_RGBA;
    _type = GL_UNSIGNED_BYTE;
    int sizeBytes = size();

    _pixels = new unsigned char[ sizeBytes ];

    unsigned char* texPtr = _pixels;
    int v;
    for (v=_height-1; v>=0; v--)
    {
        int u;
        for (u=_width-1; u>=0; u--)
        {
            if ( ((u/tile)&0x1) ^ ((v/tile)&0x1) )
            {
                texPtr[0] = 192;
                texPtr[1] = 192;
                texPtr[2] = 192;
            }
            else
            {
                texPtr[0] = 0;
                texPtr[1] = 0;
                texPtr[2] = 0;
            }
            texPtr[3] = 255;
            texPtr += 4;
        }
    }

    return (_valid = true);
}


int
Pixels::size() const
{
    int numComponents( 0 );
    switch (_format)
    {
    case GL_RGBA:
        numComponents = 4;
        break;
    case GL_RGB:
        numComponents = 3;
        break;
    case GL_DEPTH_COMPONENT:
    case GL_LUMINANCE:
        numComponents = 1;
        break;
    default:
        // Unsupported format
        assert( false );
        break;
    }

    int sizeofComponent( 0 );
    switch (_type)
    {
    case GL_UNSIGNED_BYTE:
        sizeofComponent = sizeof( GLubyte );
        break;
    case GL_UNSIGNED_INT:
        sizeofComponent = sizeof( GLuint );
        break;
    default:
        // Unsupported format
        assert( false );
        break;
    }

    return _width * _height * numComponents * sizeofComponent;
}



}
