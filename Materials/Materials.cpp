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
#include "OGLDTorus.h"
#include "OGLDCylinder.h"
#include "OGLDSphere.h"
#include "OGLDTrackballView.h"
#include <stdlib.h>
#include <assert.h>

static const int SCENE_LIGHT_VALUE( 1 );
static const int HEAD_LIGHT_VALUE( 2 );
static const int QUIT_VALUE( 99 );

ogld::Torus* torus = NULL;
ogld::Cylinder* cyl = NULL;
ogld::Sphere* sphere = NULL;
ogld::TrackballView view;

const GLfloat pink[4] = { 1.f, .6f, .6f, 1.f };
const GLfloat dkYellow[4] = { .17f, .17f, 0.f, 1.f };
const GLfloat white[4] = { 1.f, 1.f, 1.f, 1.f };
const GLfloat dkGray[4] = { .25f, .25f, .25f, 1.f };
const GLfloat black[4] = { 0.f, 0.f, 0.f, 1.f };
const GLfloat brass[4] = { 1.f, .7f, .2f, 1.f };

GLfloat sceneLightPosition[4] = { 10.f, 5.f, 10.f, 0.f };

int lastY( 0 );
bool viewMotion( false );
bool shift( false );
int lightType;


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();

    if (lightType == HEAD_LIGHT_VALUE)
    {
        GLfloat pos[4] = { 0.f, .5f, .5f, 1.f };
        glLightfv( GL_LIGHT0, GL_POSITION, pos );
    }

    view.multMatrix();

    if (lightType == SCENE_LIGHT_VALUE)
        glLightfv( GL_LIGHT0, GL_POSITION, sceneLightPosition );



    glPushMatrix();

    glTranslatef( -6.f, -6.f, 0.f );

    // Soft dull red plastic
    glMaterialfv( GL_FRONT, GL_SPECULAR, pink );
    glMaterialf( GL_FRONT, GL_SHININESS, 15.f );
    glColor3f( .9, 0., 0. );
    torus->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( 0.f, -6.f, 0.f );

    // Polished wood
    glMaterialfv( GL_FRONT, GL_SPECULAR, white );
    glMaterialf( GL_FRONT, GL_SHININESS, 128.f );
    glColor3f( .75, .6, .1 );
    torus->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( 6.f, -6.f, 0.f );

    // Tire rubber
    glMaterialfv( GL_FRONT, GL_SPECULAR, dkGray );
    glMaterialf( GL_FRONT, GL_SHININESS, 3.f );
    glColor3f( .15, .15, .15 );
    torus->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( -6.f, 0.f, 0.f );
    glRotatef( 75.f, 1.f, 0.f, 0.f );

    // Hard shiny blue plastic
    glMaterialfv( GL_FRONT, GL_SPECULAR, white );
    glMaterialf( GL_FRONT, GL_SHININESS, 20.f );
    glColor3f( .1, .1, 1. );
    cyl->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( 0.f, 0.f, 0.f );
    glRotatef( 75.f, 1.f, 0.f, 0.f );

    // Ebony
    glMaterialfv( GL_FRONT, GL_SPECULAR, white );
    glMaterialf( GL_FRONT, GL_SHININESS, 100.f );
    glColor3f( 0., 0., 0. );
    cyl->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( 6.f, 0.f, 0.f );
    glRotatef( 75.f, 1.f, 0.f, 0.f );

    // Brass
    glMaterialfv( GL_FRONT, GL_SPECULAR, brass );
    glMaterialf( GL_FRONT, GL_SHININESS, 5.f );
    glColor3f( .3, .3, .3 );
    cyl->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( -6.f, 6.f, 0.f );

    // Plaster
    glMaterialfv( GL_FRONT, GL_SPECULAR, black );
    glColor3f( .7, .7, .7 );
    sphere->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( 0.f, 6.f, 0.f );

    // China / glazed alabaster
    glMaterialfv( GL_FRONT, GL_SPECULAR, white );
    glMaterialf( GL_FRONT, GL_SHININESS, 128.f );
    glColor3f( .7, .7, .7 );
    sphere->draw();

    glPopMatrix();



    glPushMatrix();

    glTranslatef( 6.f, 6.f, 0.f );

    // Lemon
    glMaterialfv( GL_FRONT, GL_SPECULAR, dkYellow );
    glMaterialf( GL_FRONT, GL_SHININESS, 32.f );
    glColor3f( .85, .68, .02 );
    sphere->draw();

    glPopMatrix();


    
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
    switch (key)
    {
    case GLUT_KEY_LEFT:
        sceneLightPosition[0] -= .5f;
        break;
    case GLUT_KEY_RIGHT:
        sceneLightPosition[0] += .5f;
        break;
    case GLUT_KEY_DOWN:
        sceneLightPosition[2] += .5f;
        break;
    case GLUT_KEY_UP:
        sceneLightPosition[2] -= .5f;
        break;
    default:
        return;
        break;
    }


    glutPostRedisplay();
}

static void
mainMenuCB( int value )
{
    if ( (value == SCENE_LIGHT_VALUE) || (value == HEAD_LIGHT_VALUE) )
        lightType = value;

    else if (value == QUIT_VALUE)
	{
		if (torus)
			delete torus;
		if (cyl)
			delete cyl;
		if (sphere)
			delete sphere;
		torus = NULL;
		cyl = NULL;
		sphere = NULL;

        exit( 0 );
	}

    glutPostRedisplay();
}


static void
init()
{
	torus = ogld::Torus::create( 1.5, .6, 128, 64 );

	cyl = ogld::Cylinder::create( 1.f, 4.f, 64 );
    cyl->cap( true, true );

	sphere = ogld::Sphere::create( 1.6f, 4 );

    lightType = SCENE_LIGHT_VALUE;


    view.setDistance( 20. );

    glEnable( GL_LIGHTING );
    glEnable( GL_COLOR_MATERIAL );
    glEnable( GL_LIGHT0 );

    glClearColor( .4, .4, .4, 1. );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    glEnable( GL_CULL_FACE );

    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );
    glutSpecialFunc( specialKey );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Scene light", SCENE_LIGHT_VALUE );
    glutAddMenuEntry( "Head light", HEAD_LIGHT_VALUE );
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
    glutCreateWindow( "Materials" );

    init();

    glutMainLoop();

    return 0;
}

