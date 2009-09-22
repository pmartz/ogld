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
#include "OGLDTexture.h"
#include "OGLDif.h"
#include "Terrain.h"
#include <assert.h>

// Header files for ntohl and ntohs vary by platform
#if defined( __CYGWIN__ ) || defined( GLX )
  #include <netinet/in.h>
#elif defined( __APPLE__ )
  #include <arpa/inet.h>
#endif

using namespace ogld;


Terrain::Terrain()
  : _hf( NULL ),
    _texture( NULL )
{
}

Terrain::~Terrain()
{
    if (_hf)
        delete _hf;
    if (_texture)
        delete _texture;
    _hf = NULL;
    _texture = NULL;
}

void Terrain::draw() const
{
    // Save and restore texture binding and texture enable state.
    glPushAttrib( GL_TEXTURE_BIT );

    glEnable( GL_TEXTURE_2D );
    _texture->apply();

    _hf->draw();

    glPopAttrib();


    OGLDIF_CHECK_ERROR;
}

bool Terrain::init( const std::string& dataFile )
{
    //
    // Data credits
    //
    //   teton.dat
    //     Elevation and imagery data used courtesy of SimAuthor, Inc.
    //

    if (_hf)
        delete _hf;
    if (_texture)
        delete _texture;
    _hf = NULL;
    _texture = NULL;


    // Intentionally not using streams. I prefer to use the same IO
    //   routines as was used to initially create the data file. This
    //   is personal preference, and an input stream could be made to
    //   work.
    FILE* f = NULL;

    // The OGLDif class maintains a list of possible data paths.
    //   Iterate over this list until dataFile is found.
    int idx;
    for( idx = 0; idx < ogld::OGLDif::instance()->getDataPathSize(); idx++)
    {
        std::string fullPath = ogld::OGLDif::instance()->getDataPath( idx ) + dataFile;
        f= fopen( fullPath.c_str(), "rb" );
        if (f)
            // Found (and opened) the file.
            break;
    }
    if ( !f )
        // File not found; return false.
        return false;

    // First two fullwords are the elevation data width and height.
    //   These are stored in network byte order; convert them to
    //   host byte order.
    int elevW, elevH, elevWNBO, elevHNBO;
    unsigned short* elev;
    fread( &elevWNBO, sizeof( int ), 1, f );
    fread( &elevHNBO, sizeof( int ), 1, f );
    elevW = ntohl( elevWNBO );
    elevH = ntohl( elevHNBO );
    assert( elevW > 0 );
    assert( elevH > 0 );

    // Following the width and height are a number of elevation sample
    //   points. Each is an unsigned short. The number of sample points
    //   is equal to elevW times elevH.

    // Allocate memory to hold the elevation data.
    elev = new unsigned short[ elevW*elevH ];
    unsigned short* ptrE = elev;
    for( idx=0; idx<elevW*elevH; idx++)
    {
        // Read each unsigned short elevation data sample, and convert
        //   from network byte order to host byte order.
        unsigned short us;
        fread( &us, sizeof( unsigned short ), 1, f );
        *ptrE++ = ntohs( us );
    }


    // The ogld::Plane class expects a y-major array of float
    //   z values, but our elevation data is East-West or
    //   x-major shorts, so we need to transpose it as we
    //   convert to float.
    float* zArray = new float[ elevW*elevH ];
    int xIdx;
    for( xIdx=0; xIdx<elevW; xIdx++)
    {
        int yIdx;
        for( yIdx=0; yIdx<elevH; yIdx++)
            zArray[ xIdx + yIdx*elevW ] = (float)( elev[ yIdx + xIdx*elevH ] );
    }

    // Create the Plane. Note Plane has two create() factory methods,
    //   This code uses the one that allows us to pass an array of z
    //   values to create a height field rather than a plane.
    _hf = ogld::Plane::create( 11852.f * .7f, 11852.f, elevW-1, elevH-1, zArray );
    delete[] elev;
    delete[] zArray;

    // Following the elevation sample points comes the texture map
    //   width and height, each are fullwords in network byte order.
    //   Read them and convert to host byte order.
    int imageW, imageH, imageWNBO, imageHNBO;
    unsigned char* image;
    fread( &imageWNBO, sizeof( int ), 1, f );
    fread( &imageHNBO, sizeof( int ), 1, f );
    imageW = ntohl( imageWNBO );
    imageH = ntohl( imageHNBO );
    assert( imageW > 0 );
    assert( imageH > 0 );

    // After the width and height are the texture map RGBA components.
    //   These are stored as RGBA packed bytes, and the number of RGBA
    //   values is imageW times imageH. Multiply that by 4 to get the
    //   amount of memory we need.

    // Allocate memory to hold the texture map.
    image = new unsigned char[ imageW*imageH*4 ];
    int sz = fread( image, sizeof( unsigned char), imageW*imageH*4, f );
    assert( sz == imageW*imageH*4 );

    // Create the Texture, load the data. Specify GL_REPLACE texture
    //   environment mode, since the image contains all lighting and
    //   shadow effects.
    _texture = ogld::Texture::create();
    _texture->loadDirect( (GLsizei)imageW, (GLsizei)imageH, (GLubyte*)image );
    _texture->setEnvMode( GL_REPLACE );
    delete[] image;

    fclose( f );

    return true;
}


