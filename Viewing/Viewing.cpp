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
#include "OGLDTrackballView.h"
#include "OGLDCylinder.h"
#include "OGLDSphere.h"
#include "OGLDTorus.h"
#include "OGLDPlane.h"
#include "OGLDif.h"
#include <stdlib.h>
#include <assert.h>

static const int ALT_AZ_VALUE( 1 );
static const int TRACKBALL_VALUE( 2 );
static const int ORTHO_VALUE( 3 );
static const int PERSP_VALUE( 4 );
static const int QUIT_VALUE( 99 );


ogld::AltAzView altAz;
ogld::TrackballView trackball;
ogld::View* view;

ogld::Cylinder* cyl = NULL;
ogld::Sphere* sphere = NULL;
ogld::Torus* torus = NULL;
ogld::Plane* plane = NULL;

// We need to track certain values to support viewing beyond those
//   values contained within the view classes themselves...
int lastY( 0 ); // Last mouse Y position, supports viewer distance:
bool viewMotion( false ); // True if the left mouse button is pressed.
bool shift( false ); // True if the shift key is held down.

bool perspProj( true );
int winWidth, winHeight;


static void
drawScene( bool wireframe )
{
    if (wireframe)
        glColor3f( 0.f, 0.f, 0.f );
    else
        glColor3f( 1.f, 1.f, 1.f );
    glPushMatrix();
    glTranslatef( -7.f, 4.5f, 3.f );
    cyl->draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef( -7.f, -4.5f, 3.f );
    cyl->draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 7.f, -4.5f, 3.f );
    cyl->draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 7.f, 4.5f, 3.f );
    cyl->draw();
    glPopMatrix();

    if (wireframe)
        glColor3f( .5f, 0.25f, 0.1f );
    else
        glColor3f( 1.f, .8f, .2f );
    glPushMatrix();
    glTranslatef( 0.f, 0.f, 5.f );
    sphere->draw();
    glPopMatrix();

    if (wireframe)
        glColor3f( 0.f, 0.f, .5f );
    else
        glColor3f( 0.1f, .7f, 1.f );
    glPushMatrix();
    glTranslatef( 0.f, 4.6f, 3.f );
    torus->draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 7.1f, 0.f, 3.f );
    glRotatef( -90.f, 0.f, 0.f, 1.f );
    torus->draw();
    glPopMatrix();

    if (wireframe)
        glColor3f( .2f, .2f, .2f );
    else
        glColor3f( .8f, .8f, .8f );
    plane->draw();
}

static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Note we're currently in model-view matrix mode (that is,
    //   glMatrixMode( GL_MODELVIEW ) was set; see the reshape()
    //   callback). This is the appropriate mode for specifying
    //   the view matrix.
    // 'view' points to the currently selected view (AltAz or
    //   Trackball).
    // Note we use the loadMatrix() method; the view matrix isn't
    //   multiplied onto the current top of stack; rather, it
    //   replaces it.
    view->loadMatrix();


    // Draw the scene, two passes: solid and wireframe.
    glPushAttrib( 0xffffffff );
    glEnable( GL_POLYGON_OFFSET_FILL );
    drawScene( false );
    glPopAttrib();

    glPushAttrib( 0xffffffff );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    drawScene( true );
    glPopAttrib();


    OGLDIF_CHECK_ERROR;


    glutSwapBuffers();
}

static void
setProjection()
{
    // We need to set the projection matrix in two circumstances
    //   (three, if using an orthographic view):
    //
    //   1. The window size changes. This could alter the aspect
    //      ratio.
    //   2. The user switches between perspective and orthographic.
    //
    // If an orthographic view is in effect, we also need to set the
    //   projection matrix if the user changes the viewing distance.
    //   (In orthographic mode, the viewer is effectively at infinity,
    //   so we change the dimensions of the orthographic view volume
    //   to produce an effect of moving closer or further away from
    //   the scene.)
    //
    // You might also want to change the projection matrix if your
    //   application allows the user to change the field of view.

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();


    // Note how field of view and eye distance are handled differently
    //   for perspective versus orthographic.
    //
    // Perspective: Eye distance is handled in the view matrix. FOV is
    //   handled by the projection matrix.
    // Orthographic: A change in eye distance in the model-view matrix
    //   alone would have no effect, so handle eye distance the same as
    //   FOV, by changing the dimensions of the view volume.

    const double fov = 50.;
    const double aspect = (double)winWidth/(double)winHeight;
    if (perspProj)
        gluPerspective( fov, aspect, 1., 100. );
    else
    {
        // For orthographic, approximate the field of view.
        const double y = view->getDistance() * fov/90.;
        const double x = y * aspect;
        glOrtho( -x, x, -y, y, 1., 100. );
    }

    // Leave us in model-view mode for our display routine
    glMatrixMode( GL_MODELVIEW );
}

static void
reshape( int w, int h )
{
    winWidth = w;
    winHeight = h;

    // Update the viewport to render to the full window. Note this is
    //   required for most window systems that allow the user to change
    //   the window size.
    glViewport( 0, 0, w, h );

    // Update the new projection matrix (account for change in aspect).
    setProjection();

    // Trackball view needs to know the window size.
    trackball.setExtents( w, h );
}

static void
motion( int x, int y )
{
    if (viewMotion)
    {
        // User is dragging the mouse with the left mouse button down.

        if (shift)
        {
            // Shift key down. Support changing the eye distance.

            // Get the distance moved in Y.
            const int deltaY = y-lastY;

            // Change the view distance as a function of the current
            //   distance. This creates larger movement further away,
            //   and less movement as the eye distance shrinks.
            view->setDistance( view->getDistance() * ( 1. - (deltaY / 150.) ) );

            if (!perspProj)
                // Orthographic view. To give the appearance of a
                //   changing eye distance, change the size of the view
                //   volume using the projection matrix.
                setProjection();
        }
        else
            // Change the view direction.
            view->mouse( x, y );

        lastY = y;
    }

    glutPostRedisplay();
}

static void
mouse( int button, int state, int x, int y )
{
    // Note if the user is pressing the shift key.
    shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    // If the left mouse is down, the user is changing the view.
    if ( viewMotion = (button == GLUT_LEFT) )
    {
        // Tell the view object that the mouse button was pressed.
        view->mouseDown( x, y );
        lastY = y;
    }
}

static void
mainMenuCB( int value )
{
    switch (value)
    {
    case ALT_AZ_VALUE:
    {
        view = &altAz;
        break;
    }
    case TRACKBALL_VALUE:
    {
        view = &trackball;
        break;
    }
    case ORTHO_VALUE:
    {
        perspProj = false;
        setProjection();
        break;
    }
    case PERSP_VALUE:
    {
        perspProj = true;
        setProjection();
        break;
    }
    case QUIT_VALUE:
    {
        if (torus)
            delete torus;
        if (plane)
            delete plane;
        if (cyl)
            delete cyl;
        if (sphere)
            delete sphere;
        torus = NULL;
        plane = NULL;
        cyl = NULL;
        sphere = NULL;

        exit( 0 );
    }

    default:
        break;
    }

    glutPostRedisplay();
}


static void
init()
{
    // Create geometry.
    torus = ogld::Torus::create( 2.6f, .4f, 24, 8 );
    plane = ogld::Plane::create( 15., 10., 15, 10 );
    cyl = ogld::Cylinder::create( .5f, 6.f, 16 );
    cyl->cap( true, false );
    sphere = ogld::Sphere::create( 1.f, 2 );

    // Create view objects.
    altAz.setDirUp( ogld::Vec3d( 0.f, 1.f, -.1f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    altAz.setDistance( 25. );
    altAz.setCenter( ogld::Vec3d( 0.f, 0.f, 3.f ) );

    trackball.setDirUp( ogld::Vec3d( 0.f, 1.f, -.1f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    trackball.setDistance( 25. );
    trackball.setCenter( ogld::Vec3d( 0.f, 0.f, 3.f ) );

    view = &altAz;


    // Set other default state.
    glClearColor( .4, .4, .4, 1. );
    glPolygonOffset( 1.f, 1.f );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );


    OGLDIF_CHECK_ERROR;


    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Alt-Az", ALT_AZ_VALUE );
    glutAddMenuEntry( "Trackball", TRACKBALL_VALUE );
    glutAddMenuEntry( "Orthographic", ORTHO_VALUE );
    glutAddMenuEntry( "Perspective", PERSP_VALUE );
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 300, 300 );
    glutCreateWindow( "Viewing" );

    init();

    glutMainLoop();

    return 0;
}

