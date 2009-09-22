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
#include "OGLDAltAzView.h"
#include "OGLDif.h"
#include "Terrain.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <assert.h>

static const int WIRE_SOLID_VALUE( 1 );
static const int QUIT_VALUE( 99 );


Terrain terrain;
ogld::AltAzView view;

bool wireSolid( true );

int lastY( 0 );
bool viewMotion( false );
bool shift( false );


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    view.loadMatrix();

    terrain.draw();

    glutSwapBuffers();

    OGLDIF_CHECK_ERROR;
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 10., 30000. );

    // Leave us in model-view mode for our display routine
    glMatrixMode( GL_MODELVIEW );
}

static void
motion( int x, int y )
{
    if (viewMotion)
    {
        if (shift)
        {
            const int deltaY = y-lastY;
            view.setDistance( view.getDistance() * ( 1. - (deltaY / 150.) ) );
        }
        else
            view.mouse( x, y );

        lastY = y;
    }

    glutPostRedisplay();
}

static void
mouse( int button, int state, int x, int y )
{
    shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    if ( viewMotion = (button == GLUT_LEFT) )
    {
        view.mouseDown( x, y );
        lastY = y;
    }
}

static void
mainMenuCB( int value )
{
    switch (value)
    {
    case WIRE_SOLID_VALUE:
        wireSolid = !wireSolid;
        glPolygonMode( GL_FRONT_AND_BACK, wireSolid ? GL_FILL : GL_LINE );
        break;

    case QUIT_VALUE:
        exit( 0 );

    default:
        break;
    }

    glutPostRedisplay();
}


static void
init()
{
    bool terrainOK = terrain.init( "teton.dat" );
    if (!terrainOK)
    {
        std::cout << "Failed to load teton.dat terrain data file." << std::endl;
        exit( 1 );
    }

    view.setDirUp( ogld::Vec3d( -1.f, .8f, -.4f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    view.setDistance( 13000. );
    view.setCenter( ogld::Vec3d( 0., 0., 3000. ) );

    glClearColor( 0., 0., 0., 1. );
    glPolygonOffset( 1.f, 1.f );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );


    OGLDIF_CHECK_ERROR;


    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Toggle wireframe/solid", WIRE_SOLID_VALUE );
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
    glutCreateWindow( "Texture Mapping" );

    init();

    glutMainLoop();

    return 0;
}

