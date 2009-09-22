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
#include "Orrery.h"
#include "OGLDCylinder.h"
#include "OGLDSphere.h"


Orrery::Orrery()
  : _earthDegrees( 0.f ),
    _moonDegrees( 0.f ),
    _lastTime( 0 )
{
    _sun = ogld::Sphere::create( 3.f, 3 );
    _earth = ogld::Sphere::create( 1.f, 3 );
    _moon = ogld::Sphere::create( .5f, 3 );

    _sunStem = ogld::Cylinder::create( .1f, 4.f, 8 );
    _earthStem = ogld::Cylinder::create( .1f, 4.f, 8 );
    _moonStem = ogld::Cylinder::create( .1f, 4.f, 8 );

    _earthOrbit = ogld::Cylinder::create( 12.1f, .1f, 64 );
    _moonOrbit = ogld::Cylinder::create( 3.1f, .1f, 64 );

    _earthOrbit->cap( true, false );
    _moonOrbit->cap( true, false );
}

Orrery::~Orrery()
{
    if (_sun)
        delete _sun;
    if (_earth)
        delete _earth;
    if (_moon)
        delete _moon;
    _sun = _earth = _moon = NULL;

    if (_sunStem)
        delete _sunStem;
    if (_earthStem)
        delete _earthStem;
    if (_moonStem)
        delete _moonStem;
    _sunStem = _earthStem = _moonStem = NULL;

    if (_earthOrbit)
        delete _earthOrbit;
    if (_moonOrbit)
        delete _moonOrbit;
    _earthOrbit = _moonOrbit = NULL;
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


    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    // First, draw the Sun and the stem supporting it.
    glColor3f( 1.f, 1.f, .8f );
    _sun->draw();

    glPushMatrix();
    glTranslatef( 0.f, 0.f, -2.f );
    glColor3f( .9f, .9f, .9f );
    _sunStem->draw();
    glPopMatrix();


    // The Earth and Moon both follow the Earth's rotation.
    //   Specify that rotation here to establish the new coordinate
    //   system for subsequent geometry.
    glRotatef( _earthDegrees, 0.f, 0.f, 1.f );

    glPushMatrix();
    glTranslatef( 0.f, 0.f, -4.f );
    glColor3f( .55f, .55f, .55f );
    _earthOrbit->draw();
    glPopMatrix();


    // Draw the Earth and the stem supporting it.
    glTranslatef( 12.f, 0.f, 0.f );
    glColor3f( .3f, .5f, 1.f );
    _earth->draw();

    glPushMatrix();
    glTranslatef( 0.f, 0.f, -2.f );
    glColor3f( .9f, .9f, .9f );
    _earthStem->draw();
    glPopMatrix();


    // The Moon geometry obeys the Moon's orbit. Set that rotation
    //   here, within the coordinate system already established for
    //   the Earth orbit, to create the new coordinate system for
    //   the Moon.
    glRotatef( _moonDegrees, 0.f, 0.f, 1.f );

    glPushMatrix();
    glTranslatef( 0.f, 0.f, -3.9f );
    glColor3f( .5f, .5f, .5f );
    _moonOrbit->draw();
    glPopMatrix();


    // Draw the Moon and the stem supporting it.
    glTranslatef( 3.f, 0.f, 0.f );
    glColor3f( .5f, .5f, .5f );
    _moon->draw();

    glPushMatrix();
    glTranslatef( 0.f, 0.f, -2.f );
    glColor3f( .9f, .9f, .9f );
    _moonStem->draw();
    glPopMatrix();


    glPopMatrix();
    glPopAttrib();


    OGLDIF_CHECK_ERROR;
}

