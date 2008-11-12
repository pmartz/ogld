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
#include "OGLDPlatformGLUT.h"
#include "OGLDPlatformGLU.h"
#include "OGLDPixels.h"
#include <stdlib.h>
#include "OGLDPlane.h"
#include <assert.h>


static const int QUIT_VALUE( 99 );

ogld::Plane* plane = NULL;
GLuint texId( 0 );


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    gluLookAt( 0., 0., 2.,
            0., 0., 0.,
            0., 1., 0. );

    glBindTexture( GL_TEXTURE_2D, texId );

    // Draw the plane. ogld::Plane specifies vertices and texture
    //   coordinates. See OGLD/OGLDPlane.cpp for details.
    plane->draw();


    OGLDIF_CHECK_ERROR;


    glutSwapBuffers();
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 40., (double)w/(double)h, 1., 10. );

    // Leave us in modelview mode for our display routine.
    glMatrixMode( GL_MODELVIEW );
}

static void
mainMenuCB( int value )
{
    if (value == QUIT_VALUE)
    {
        if (plane)
            delete plane;
        plane = NULL;

        // Demonstrates how to delete a texture object.
        //   Unnecessary in this simple case, since the following
        //   call to exit() will destroy the rendering context
        //   and all associated texture objects.
        glDeleteTextures( 1, &texId );
        exit( 0 );
    }
}


static void
init()
{
    plane = ogld::Plane::create();

    // Use the ogld::Pixels class to load the image and obtain a pointer to the pixel data.
    ogld::Pixels image;
    const bool result = image.loadImage( std::string( "NewspaperRock.tif" ) );
    if ( result != true )
        exit( -1 );

    GLsizei width, height;
    image.getWidthHeight( width, height );

    // Note: OpenGL version 2.0 allows non power of 2 textuyre width
    //   and height. For run time version less than 2.0, only powers of
    //   2 are allowed. "NewspaperRock.tif" meets this criteria.
    //   However, use an assertion in case someone modifies the code
    //   later to display a different image.
    if (ogld::OGLDif::instance()->getVersion() < ogld::Ver20)
    {
        assert( (width & (width-1)) == 0 );
        assert( (height & (height-1)) == 0 );
    }

    // Obtain a texture ID and create/init the texture object
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, image.getFormat(), width, height,
        0, image.getFormat(), image.getType(), image.getPixels() );

    glEnable( GL_TEXTURE_2D );


    // Other default state.
    glClearColor( .9, .9, .9, 0. );
    glDisable( GL_DITHER );


    // Check for OpenGL errors.
    OGLDIF_CHECK_ERROR;


    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 300, 300 );
    glutCreateWindow( "Simple Texture Mapping" );

    init();

    glutMainLoop();

    return 0;
}

