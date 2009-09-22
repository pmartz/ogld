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
#include "OGLDTrackballView.h"
#include "OGLDif.h"
#include "Scene.h"
#include <stdlib.h>
#include <assert.h>

static const int QUIT_VALUE( 99 );


ogld::TrackballView view;
Scene* scene = NULL;

int lastX( 0 ), lastY( 0 );
GLuint lastZ( 0 );
bool viewMotion( false );
bool shift( false );



// Process hit records returned by OpenGL after a selection pass.
//   Return the name (from the name stack) of the selected item
//   closest to the viewer, and record it's Z value in lastZ.
// Returns -1 if nothing was selected.
static int
processHits( int hits, GLuint* pickBuf )
{
    int select = -1;
    lastZ = 0xffffffff;

    GLuint* bufPtr = pickBuf;
    int i;
    for (i=hits; i; i--)
    {
        // Examine each hit record, which contains:
        //   GLuint - number of names in the name stack when this hit occurred.
        //   GLuint - Minimum Z value for the hit.
        //   GLuint - maximum Z value for the hit.
        //   GLuint... - contents of the name stack.

        // Get the number of names. For this simple example, it must be 1, since
        //   our code never pushes the name stack beyond depth 1.
        GLuint numNames = *bufPtr++;
        assert( numNames == 1);

        // Get the minimum Z value and skip the maximum Z value.
        GLuint minZ = *bufPtr++; bufPtr++;
        // Get the one name from the name stack.
        GLuint name = *bufPtr++;
        if (minZ < lastZ)
        {
            // This hit record is either the first one we've encountered,
            //   or the geometry picked was closer to the viewer.
            //   Save the name and Z value from the current hit record.
            select = name;
            lastZ = minZ;
        }
    }

    return select;
}


static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    view.loadMatrix();

    scene->render();

    glutSwapBuffers();

    OGLDIF_CHECK_ERROR;
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 1., 100. );

    // Leave us in model-view mode for our display routine
    glMatrixMode( GL_MODELVIEW );

    // Trackball view needs to know the window size.
    view.setExtents( w, h );
}


GLdouble projMat[16];
GLdouble mvMat[16];
GLint vp[4];

static void
motion( int x, int y )
{
    if (viewMotion)
    {
        // viewMotion is true if the user is holding down the left mouse button.

        if (scene->getSelect() != -1)
        {
            // An object in the scene was selected when the user pushed the
            //   mouse button. The following code allows the user to drag
            //   the object in such a way that it always follows the mouse.
            //
            // Imagine a plane containing the selected object that is orthogonal
            //   (face on) to the viewer. We need to determine the delta mouse
            //   motion in terms of an object space vector lying on that plane.
            //   Do this by unprojecting the window space mouse vector back into
            //   object coordinates.
            GLdouble oX, oY, oZ;
            GLint projected;

            // lastX,lastY are the previous mouse position. lastZ is the
            //   Z value of the selected object. Together, they form the window
            //   coordinate position of the mouse in 3D window coordinate space.
            //   Vack-transform them into object coordinates.
            projected = gluUnProject( lastX, vp[3]-lastY, lastZ/(double)0xffffffff,
                mvMat, projMat, vp, &oX, &oY, &oZ );
            assert( projected == GL_TRUE );
            ogld::Vec3d a( oX, oY, oZ );

            // So the same with the current window coordnate mouse position.
            projected = gluUnProject( x, vp[3]-y, lastZ/(double)0xffffffff,
                mvMat, projMat, vp, &oX, &oY, &oZ );
            assert( projected == GL_TRUE );
            ogld::Vec3d b( oX, oY, oZ );

            // (b-a) is the object coordinate delta motion of the mouse.
            //   Set this in the scene to reposition the selected object.
            scene->reposition( b-a );
        }
        else
        {
            // Nothing selected; handle normal viewing.
            if (shift)
            {
                const int deltaY = y-lastY;
                view.setDistance( view.getDistance() * ( 1. - (deltaY / 150.) ) );
            }
            else
                view.mouse( x, y );
        }

        // Save current mouse position as previous mouse position.
        lastX = x;
        lastY = y;
    }

    glutPostRedisplay();
}

static void
mouse( int button, int state, int x, int y )
{
    if (state != GLUT_DOWN)
    {
        // This isn't a mouse button press, so make sure
        //   no objects in the scene are selected.
        scene->select( -1 );

        glutPostRedisplay();
        return;
    }
    // At this point in the code, we know we received a mouse
    //   button down event.

    if ( !( viewMotion = (button == GLUT_LEFT) ) )
        // The user didn't press the left mouse button,
        //   so there's nothing for us to do.
        return;


    // At this point in the code, we know the left mouse
    //   button was pressed.
    //
    // Check to see if the user selected an object in
    //   the scene. Perform a selection pass.
    glPushAttrib( 0xffffffff );

    // Create and set the selection buffer.
    const int size( 1024 );
    GLuint pickBuf[ size ];
    glSelectBuffer( size, pickBuf );
    glRenderMode( GL_SELECT );

    // We need to save the Projection and Model-View matrices, and viewport.
    //   If the user picks and drags an object, the motion() callback
    //   uses this data to ensure the object follows the mouse.
    glGetDoublev( GL_PROJECTION_MATRIX, projMat );
    glGetIntegerv( GL_VIEWPORT, vp );

    // Set up the Projection matrix for picking. Use gluPickMatrix
    //   to restrict the field of view to a 5x5 pixel area.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPickMatrix( x, vp[3]-y, 5, 5, vp );
    glMultMatrixd( projMat );

    glMatrixMode( GL_MODELVIEW );
    view.loadMatrix();
    glGetDoublev( GL_MODELVIEW_MATRIX, mvMat );


    // Perform the actual selection pass. This passes everything
    //   in the scene to OpenGL in selection (rather than render) mode.
    scene->pick();


    // Obtain the number of picked objects (could be zero).
    int hits = glRenderMode( GL_RENDER );

    // Process the hit records. Returned value is the selected object
    //   index (or -1 if nothing was selected).
    int select = processHits( hits, pickBuf );

    // Set the selected object.
    scene->select( select );

    // Restore state
    glMatrixMode( GL_PROJECTION );
    glLoadMatrixd( projMat );

    glPopAttrib();

    OGLDIF_CHECK_ERROR;


    shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    view.mouseDown( x, y );
    // Save current mouse position as previous mouse position.
    lastX = x;
    lastY = y;

    glutPostRedisplay();
}

static void
mainMenuCB( int value )
{
    switch (value)
    {
    case QUIT_VALUE:
    {
        if (scene)
            delete scene;
        scene = NULL;

        exit( 0 );
    }

    default:
        break;
    }
}


static void
init()
{
    scene = new Scene;

    view.setDirUp( ogld::Vec3d( 0.f, 1.f, 0.f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    view.setDistance( 13. );

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
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 450, 300 );
    glutCreateWindow( "Picking" );

    init();

    glutMainLoop();

    return 0;
}

