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
#include "OGLDif.h"
#include "OGLDTorus.h"
#include "OGLDTrackballView.h"
#include "OGLDTexture.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>


static const int TOGGLE_SECONDARY_VALUE( 1 );
static const int QUIT_VALUE( 99 );


GLfloat lightPosition[4] = { 10.f, 5.f, 10.f, 0.f };

ogld::Torus* torus = NULL;
ogld::Texture* texture = NULL;
ogld::TrackballView view;

const GLfloat specWhite[4] = { 1.f, 1.f, 1.f, 1.f };

int lastY( 0 );
bool viewMotion( false );
bool shift( false );

bool useSecondary( true );



static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the view matrix and light position.
    glLoadIdentity();
    view.multMatrix();

    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );


    // When glPopAttrib is issued, restore texture binding and matrix
    //   mode state.
    glPushAttrib( GL_TEXTURE_BIT | GL_TRANSFORM_BIT );

    texture->apply();
    // To correctly apply this texture, we need to transform the
    //   default texture coordinates provided by the Torus class.
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    //   Translate texture coordinates "up" 0.25 units in t.
    glTranslatef( 0.f, .25f, 0.f );
    //   Texture is inverted; negate t coordinates to resolve this.
    glScalef( 1.f, -1.f, 1.f );

    torus->draw();

    glPopAttrib();


    OGLDIF_CHECK_ERROR;

    
    glutSwapBuffers();
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 2., 50. );

    // Leave us in model-view mode for our display routine
    glMatrixMode( GL_MODELVIEW );

    // Trackball view needs to know the window size.
    view.setExtents( w, h );
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
specialKey( int key, int x, int y )
{
    // Allow the user the change the light position.
    switch (key)
    {
    case GLUT_KEY_LEFT:
        lightPosition[0] -= .5f;
        break;
    case GLUT_KEY_RIGHT:
        lightPosition[0] += .5f;
        break;
    case GLUT_KEY_DOWN:
        lightPosition[2] += .5f;
        break;
    case GLUT_KEY_UP:
        lightPosition[2] -= .5f;
        break;
    default:
        return;
        break;
    }


    glutPostRedisplay();
}

static void
setSecondary()
{
    // GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR, and
    //   GL_SINGLE_COLOR are only available if the compile time version
    //   is 1.4 or higher.
#ifdef GL_VERSION_1_4

    // Note that we should never get to this code if the OpenGL run
    //   time version is less than 1.4, because the init() function
    //   doesn't add the TOGGLE_SECONDARY_VALUE menu item unless the
    //   run time version is 1.4 or higher.

    if (useSecondary)
        glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );
    else
        glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR );
#endif
}

static void
mainMenuCB( int value )
{
    if (value == TOGGLE_SECONDARY_VALUE)
    {
        // This menu option should not be available if the run time
        //   version is less than 1.4.
        useSecondary = !useSecondary;
        setSecondary();
    }

    else if (value == QUIT_VALUE)
    {
        if (torus)
            delete torus;
        if (texture)
            delete texture;
        torus = NULL;
        texture = NULL;

        exit( 0 );
    }

    glutPostRedisplay();
}


static void
init()
{
    if (ogld::OGLDif::instance()->getVersion() < ogld::Ver14)
    {
        std::cout << "OpenGL run time version is less than 1.4. Secondary color is unavailable," << std::endl;
        std::cout << "  This example is not fully functional." << std::endl;
    }


    // The _useSecondary variable defaults to true. Set the initial
    //   secondary color state to reflect this.
    setSecondary();

    torus = ogld::Torus::create( 1.5, .6, 128, 64 );

    view.setDirUp( ogld::Vec3d( 0.f, -.2f, -1.f ), ogld::Vec3d( 0.f, 1.f, 0.f ) );
    view.setDistance( 8. );

    texture = ogld::Texture::create();

    //
    // Image credits:
    //
    //   AphrodesiaHeads.tif
    //     Image used courtesy of Dean Randazzo.
    //
    //   In ancient times, Aphrodisias Turkey was home to a sculpture school.
    //   This image shows the work of ancient sculpture students.
    //
    bool result;
    result = texture->loadImage( std::string( "AphrodesiaHeads.tif" ) );

    glEnable( GL_TEXTURE_2D );

    // Enable default lighting state. Set the specular material color
    //   to white, with a high shininess exponent.
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specWhite );
    glMaterialf( GL_FRONT, GL_SHININESS, 128.f );

    // Set other default state.
    glClearColor( .4, .4, .4, 1. );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );


    OGLDIF_CHECK_ERROR;


    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );
    glutSpecialFunc( specialKey );

    glutCreateMenu( mainMenuCB );
    if (ogld::OGLDif::instance()->getVersion() >= ogld::Ver14)
        // Only add this menu item if secondary color is available.
        glutAddMenuEntry( "Toggle secondary color", TOGGLE_SECONDARY_VALUE );
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 450, 450 );
    glutCreateWindow( "Secondary Color" );

    init();

    glutMainLoop();

    return 0;
}

