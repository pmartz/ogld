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
#include "OGLDAltAzView.h"
#include "Orrery.h"
#include "tiffio.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <assert.h>

static const int SCREENSHOT_VALUE( 1 );
static const int QUIT_VALUE( 99 );


Orrery* orrery;

ogld::AltAzView view;

int lastY( 0 );
bool viewMotion( false );
bool shift( false );

bool capture;



static void
screenCapture()
{
    TIFF* tif = TIFFOpen( "screen.tif", "w" );
    if (tif)
    {
        glReadBuffer( GL_BACK );

        int w = glutGet( GLUT_WINDOW_WIDTH );
        int h = glutGet( GLUT_WINDOW_WIDTH );

        unsigned char** pixels = new unsigned char*[ h ];
        int idx;
        for (idx=0; idx<h; idx++)
        {
            pixels[ idx ] = new unsigned char[ w*3 ];
            glReadPixels( 0, idx, w, 1, GL_RGB, GL_UNSIGNED_BYTE, pixels[ idx ] );
        }

        TIFFSetField( tif, TIFFTAG_IMAGEWIDTH, w );
        TIFFSetField( tif, TIFFTAG_IMAGELENGTH, h );

        TIFFSetField( tif, TIFFTAG_BITSPERSAMPLE, 8 );
        TIFFSetField( tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW );
        TIFFSetField( tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
        TIFFSetField( tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB );
        TIFFSetField( tif, TIFFTAG_SAMPLESPERPIXEL, 3 );
        int rowsPerStrip = (8 * 1024) / (3 * w);
        if (rowsPerStrip == 0)
            rowsPerStrip = 1;
        TIFFSetField( tif, TIFFTAG_ROWSPERSTRIP, rowsPerStrip );
        TIFFSetField( tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );

        for (idx=0; idx<h; idx++)
        {
            if (TIFFWriteScanline( tif, pixels[ h-idx-1], idx, 0 ) < 0)
            {
                assert( false );
                break;
            }
        }

        for (idx=0; idx<h; idx++)
            delete[] pixels[ idx ];
        delete[] pixels;

        TIFFFlushData( tif );
        TIFFClose( tif );
    }
}

static void
display()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    view.loadMatrix();

    // Pass in the current time for a steady animation rate.
    orrery->draw( glutGet( GLUT_ELAPSED_TIME ) );

    if (capture)
    {
        screenCapture();
        capture = false;
    }

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
}

static void
idle()
{
    glutPostRedisplay();
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
    case SCREENSHOT_VALUE:
        capture = true;
        glutPostRedisplay();
        break;

    case QUIT_VALUE:
        exit( 0 );

    default:
        break;
    }
}


static void
init()
{
    std::string pboStr( "GL_ARB_pixel_buffer_object" );
    bool hasPBO = ogld::OGLDif::instance()->isExtensionSupported(
            pboStr, glGetString( GL_EXTENSIONS ) );
    std::cout << pboStr << (hasPBO ? " is" : " is not") << " available." << std::endl;

    orrery = new Orrery;

    capture = false;

    view.setDirUp( ogld::Vec3d( 0.f, 1.f, -.1f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    view.setDistance( 25. );

    glClearColor( 0., 0., 0., 1. );
    glPolygonOffset( 1.f, 1.f );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    OGLDIF_CHECK_ERROR;

    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutIdleFunc( idle );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Capture to file", SCREENSHOT_VALUE );
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 500, 300 );
    glutCreateWindow( "Extensions" );

    init();

    glutMainLoop();

    return 0;
}

