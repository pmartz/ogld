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
#include <string>
#include <sstream>
#include <assert.h>

// Define an ID for the "Quit" menu item.
static const int QUIT_VALUE( 99 );

// Global variable to hold display list ID
GLuint listID;


//
// GLUT display callback. Called by GLUT when the window
// needs redrawing.
static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT );

    // Modeling transform, move geometry 4 units back in Z.
    glLoadIdentity();
    glTranslatef( 0.f, 0.f, -4.f );

    // Draw the geometry
    glCallList( listID );

    // Swap buffers (display the rendered image).
    glutSwapBuffers();

    assert( glGetError() == GL_NO_ERROR );
}

//
// GLUT resize callback, called when window size changes.
static void
reshape( int w, int h )
{
    // Update the viewport to draw to the full window
    glViewport( 0, 0, w, h );

    // Update the projection matrix / aspect ratio
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 1., 10. );

    // Leave us in model-view mode for our display routine
    glMatrixMode( GL_MODELVIEW );

    assert( glGetError() == GL_NO_ERROR );
}

//
// GLUT menu callback, called when user selects a menu item.
static void
mainMenuCB( int value )
{
    if (value == QUIT_VALUE)
        exit( 0 );
}


static void
init()
{
    // Dither is on by default but not needed, disable it.
    glDisable( GL_DITHER );

    // Determine whether or not vertex arrays are available.
    // In other words, check for OpenGL v1.1...
    std::string ver( (const char*) glGetString( GL_VERSION ) );
    assert( !ver.empty() );
    std::istringstream verStream( ver );

    int major, minor;
    char dummySep;
    verStream >> major >> dummySep >> minor;

    const bool useVertexArrays = ( (major >= 1) && (minor >= 1) );

    const GLfloat data[] = {
        -1.f, -1.f, 0.f,
        1.f, -1.f, 0.f,
        0.f, 1.f, 0.f };

    if (useVertexArrays)
    {
        // Set up for using vertex arrays.
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, data );
    }

    // Create a new display list.
    listID = glGenLists( 1 );
    glNewList( listID, GL_COMPILE );

    if (useVertexArrays)
        // Vertex arrays are avilable in OpenGL 1.1, use them.
        glDrawArrays( GL_TRIANGLES, 0, 3 );

    else
    {
        // Use OpenGL 1.0 Begin/End interface.
        glBegin( GL_TRIANGLES );
        glVertex3fv( &data[0] );
        glVertex3fv( &data[3] );
        glVertex3fv( &data[6] );
        glEnd();
    }

    glEndList();

    assert( glGetError() == GL_NO_ERROR );

    // Register our display and resize callbacks with GLUT.
    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );

    // Create a right-mouse menu to allow users to exit.
    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );

    // Create a single GLUT window, 300x300 pixels, RGB mode,
    // and double-buffered. Call it "Simple Example".
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 300, 300 );
    glutCreateWindow( "Simple Example" );

    init();

    // Loop for events.
    glutMainLoop();

    return 0;
}

