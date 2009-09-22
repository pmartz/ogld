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
#include "OGLDif.h"
#include "Orrery.h"
#include <list>
#include <assert.h>



class SortPixelsHelper
{
public:
	bool operator<( const SortPixelsHelper& a ) const
	{
		return (_d < a._d);
	}

	double _d;
	double _m[16];
	ogld::Pixels* _pixels;
};



Orrery::Orrery()
  : _earthDegrees( 0.f ),
	_moonDegrees( 0.f ),
    _lastTime( 0 )
{
    //
    // Image credits:
    //
    //   earth.tif
    //     This image is owned by NASA and was developed by the Earth
    //     Observatory team (http://earthobservatory.nasa.gov/).
    //
    //   moon.tif
    //     Courtesy NASA/JPL-Caltech.
    //
    //   sun.tif
    //     Courtesy of SOHO/Extreme Ultraviolet Imaging Telescope
    //     (EIT) consortium. SOHO is a project of international
    //     cooperation between ESA and NASA. 
    //

	bool result;
    _sunImage = ogld::Pixels::create( ogld::Pixels::UsePBOIfAvailable );
	result = _sunImage->loadImage( std::string("sun.tif") );

	_earthImage = ogld::Pixels::create( ogld::Pixels::UsePBOIfAvailable );
	result = _earthImage->loadImage( std::string("earth.tif") );

	_moonImage = ogld::Pixels::create( ogld::Pixels::UsePBOIfAvailable );
	result = _moonImage->loadImage( std::string("moon.tif") );
}

Orrery::~Orrery()
{
	if (_sunImage)
		delete _sunImage;
	if (_earthImage)
		delete _earthImage;
	if (_moonImage)
		delete _moonImage;
}

void Orrery::draw( int currentTime )
{
    const int elapsedTime = currentTime - _lastTime;
    _lastTime = currentTime;

    // Make the moon orbit once per second, and the earth once per
    //   twelve seconds.
    _moonDegrees += (elapsedTime / 1000.f * 360.f);
    if (_moonDegrees > 360.f)
        _moonDegrees -= 360.f;
    _earthDegrees += (elapsedTime / 1000.f * 30.f);
    if (_earthDegrees > 360.f)
        _earthDegrees -= 360.f;


	std::list<SortPixelsHelper> pixelsList;

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_ALPHA_TEST );
	glAlphaFunc( GL_GREATER, 0.f );


    // We don't actually draw anything here, we just compute
    //   The matrices and eye space distance to each image and
    //   store them in a std:: list for later sorting and rendering.
    // Note the 15th element of the model-view matrix contains the
    //   (negated) eye space Z translation. Negate this value to
    //   get the absolute eye space Z distance.
	SortPixelsHelper sph;
	glGetDoublev( GL_MODELVIEW_MATRIX, &(sph._m[0]) );
	sph._d = -sph._m[14]; // Eye space distance to Sun.
	sph._pixels = _sunImage;
	pixelsList.push_back( sph );


	glRotatef( _earthDegrees, 0.f, 0.f, 1.f );
	glTranslatef( 12.f, 0.f, 0.f );

	glGetDoublev( GL_MODELVIEW_MATRIX, &(sph._m[0]) );
	sph._d = -sph._m[14]; // Eye space distance to Earth.
	sph._pixels = _earthImage;
	pixelsList.push_back( sph );


	glRotatef( _moonDegrees, 0.f, 0.f, 1.f );
	glTranslatef( 3.f, 0.f, 0.f );

	glGetDoublev( GL_MODELVIEW_MATRIX, &(sph._m[0]) );
	sph._d = -sph._m[14]; // Eye space distance to Moon.
	sph._pixels = _moonImage;
	pixelsList.push_back( sph );


    // Sorts images in order of ascending distance. Furthest
    //   away are at the back of the list.
	pixelsList.sort();
	while (!pixelsList.empty())
	{
        // Render images in back-to-front order.
		sph = pixelsList.back();
		pixelsList.pop_back();

		GLsizei w, h;
		sph._pixels->getWidthHeight( w, h );

		glLoadMatrixd( sph._m );
		glRasterPos3f( 0.f, 0.f, 0.f );
		glBitmap( 0, 0, 0.f, 0.f,
				(float)( -(w>>1) ), (float)( -(h>>1) ), NULL );
		sph._pixels->apply();
	}


	glPopMatrix();
	glPopAttrib();


    OGLDIF_CHECK_ERROR;
}

