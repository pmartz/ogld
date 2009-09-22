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
// VertexArray
//
// This demonstrates basic use of the OpenGL version 1.1 vertex array
//   feature. This demonstrates vertex arrays without buffer objects.
//   For source code that uses buffer objects, see the Plane, Sphere,
//   Cylinder, and Torus classes in the OGLD directory.
//

#include "OGLDPlatformGL.h"
#include "OGLDPlatformGLUT.h"
#include "SimpleCylinder.h"
#include <stdlib.h>
#include <assert.h>

static const int QUIT_VALUE( 99 );

Cylinder cyl;


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Viewing not covered until chapter 3; set up a simple model-view
    //   matrix to place the cylinder just in front of the viewer.
    glLoadIdentity();
    glTranslatef( 0.f, 0.f, -4.f );

    // First time through, initialize and draw the cylinder.
    //   Subsequently, vertex arrays are already created, so just
    //   render the cylinder
    cyl.draw();

    glutSwapBuffers();
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
mainMenuCB( int value )
{
    if (value == QUIT_VALUE)
        exit( 0 );
}


static void
init()
{
    // Disable unnecessary state.
    glDisable( GL_DITHER );

    // Draw in wireframe.
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

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
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 300, 300 );
    glutCreateWindow( "Vertex Arrays" );

    init();

    glutMainLoop();

    return 0;
}

