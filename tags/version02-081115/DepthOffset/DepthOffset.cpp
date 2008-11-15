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
#include "OGLDCylinder.h"
#include "OGLDif.h"
#include <stdlib.h>
#include <assert.h>

static const int TOGGLE_OFFSET_VALUE( 1 );
static const int QUIT_VALUE( 99 );

bool enableOffset;

ogld::Cylinder* cylBody = NULL;
ogld::Cylinder* cylStripe = NULL;

int lastTime( 0 );
float theta( 0.f );


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    glTranslatef( 0.f, 0.f, -4.f );
    glRotatef( theta, 0.f, 1.f, 0.f );

    if (enableOffset)
        glEnable( GL_POLYGON_OFFSET_FILL );
    else
        glDisable( GL_POLYGON_OFFSET_FILL );

    // Draw the large white cylinder
    glPolygonOffset( 2.f, 2.f );
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glColor3f( 1.f, 1.f, 1.f );
    cylBody->draw();

    // Draw the cylinder's center stripe
    glPolygonOffset( 1.f, 1.f );
    glColor3f( .6f, .6f, 1.f );
    cylStripe->draw();

    // Draw the cylinder in line mode
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f( 0.f, 0.f, 0.f );
    cylBody->draw();

    glutSwapBuffers();


    OGLDIF_CHECK_ERROR;
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 1., 10. );

    // Leave us in model-view mode for our display routine
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
    switch (value)
    {
    case TOGGLE_OFFSET_VALUE:
    {
        enableOffset = !enableOffset;
        break;
    }
    case QUIT_VALUE:
    {
        if (cylBody)
            delete cylBody;
        if (cylStripe)
            delete cylStripe;
        cylBody = cylStripe = NULL;

        exit( 0 );
    }

    default:
        break;
    }
}


static void
init()
{
    cylBody = ogld::Cylinder::create( 1.f, 2.f );
    cylBody->cap( false, true );

    cylStripe = ogld::Cylinder::create( 1.f, .5f );

    enableOffset = true;

    glClearColor( .4, .4, .4, 1. );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    OGLDIF_CHECK_ERROR;

    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    // GLUT calls the idle callback when there are no other events.
    glutIdleFunc( idle );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Toggle polygon offset", TOGGLE_OFFSET_VALUE );
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
    glutCreateWindow( "Depth Offset" );

    init();

    glutMainLoop();

    return 0;
}

