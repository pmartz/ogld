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

//
// SimpleLighting
//
// This example demonstrates the minimal code needed to produce
//   lighting effects with the OpenGL lighting feature, as described
//   in section 4.1.3, "Minimal Lighting Code," on page 103, 
// It uses the ogld::Cylinder class to produce appropriate surface
//   normals; see OGLD/OGLDCylinder.cpp for how to compute and
//   specify normal data.
//

#include "OGLDPlatformGL.h"
#include "OGLDPlatformGLUT.h"
#include "OGLDif.h"
#include <stdlib.h>
#include "OGLDCylinder.h"


static const int QUIT_VALUE( 99 );

ogld::Cylinder* cyl = NULL;

int lastTime( 0 );
float theta( 0.f );


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    gluLookAt( 0., 0., 4.,
            0., 0., 0.,
            0., 1., 0. );

    // Animate / rotate the cylinder
    glRotatef( theta, 0., 1., 0. );

    // The Cylinder::draw() method specifies vertices and unit-
    //   length normals. The normals are required for correct
    //   lighting results.
    cyl->draw();


    OGLDIF_CHECK_ERROR;


    glutSwapBuffers();
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 1., 10. );

    // Leave us in modelview mode for our display routine
    glMatrixMode( GL_MODELVIEW );
}

static void
idle()
{
    const int currentTime = glutGet( GLUT_ELAPSED_TIME );
    const int elapsedTime = currentTime - lastTime;
    lastTime = currentTime;

    // elapsedTime is in milliseconds (thousandths of a second).
    //   To rotate once every 4 seconds, divide by 1000 and multiply
    //   by 90 degrees. This generates the delta rotation for the
    //   next frame.
    theta += (elapsedTime / 1000.f) * 90.f;

    if (theta > 360.f)
        // Wrap at 360 degrees; otherwise, floating point precision
        //   will eventually cause no rotation to occur.
        theta -= 360.f;


    glutPostRedisplay();
}

static void
mainMenuCB( int value )
{
    if (value == QUIT_VALUE)
    {
        if (cyl)
            delete cyl;
        cyl = NULL;

        exit( 0 );
    }
}


static void
init()
{
    cyl = ogld::Cylinder::create();
    cyl->cap( true, true );

    glClearColor(.3, .3, .5, 0. );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    // Minimum code to enable lighting effects:
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );


    OGLDIF_CHECK_ERROR;


    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    // GLUT calls the idle callback when there are no other events.
    glutIdleFunc( idle );

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
    glutCreateWindow( "Simple Lighting" );

    init();

    glutMainLoop();

    return 0;
}

