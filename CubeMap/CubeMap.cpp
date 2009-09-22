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
#include "OGLDCylinder.h"
#include "OGLDTorus.h"
#include "OGLDSphere.h"
#include "OGLDTrackballView.h"
#include "OGLDCubeMap.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>



static const int QUIT_VALUE( 99 );


GLfloat lightPosition[4] = { 5.f, 15.f, 4.f, 1.f };

ogld::CubeMap* cubeMapTexture = NULL;
ogld::Cylinder* cyl = NULL;
ogld::Torus* torus = NULL;
ogld::Sphere* sphere = NULL;
ogld::TrackballView view;


int lastY( 0 );
bool viewMotion( false );
bool shift( false );


static void
setCubeMapTextureMatrix()
{
    // The cube map default places a light source at the positive X axis.
    //   This routine creates a texture matrix that transforms the eye
    //   coordinate reflection vector into world coordinates, and further
    //   transforms those world coordinates so that the X axis aligns with
    //   the actual light position.

    ogld::Vec3d worldX( 1., 0., 0. );

    // New X axis is vector to the light.
    ogld::Vec3d x( lightPosition[0], lightPosition[1], lightPosition[2] );
    x.normalize();

    // Create the Y axis, careful to handle the case where the 
    //   world X axis and the light vector are nearly coincident.
    double dot = worldX.dot( x );
    if (dot < 0.) dot = -dot;
    ogld::Vec3d y;
    if (dot > .98)
        y = ogld::Vec3d( 0., 1., 0. );
    else
        y = ogld::Vec3d::cross( x, worldX );
    y.normalize();

    // Z axis
    ogld::Vec3d z = ogld::Vec3d::cross( x, y );
    z.normalize();

    // Fill in the matrix
    GLdouble m[16] = { x[0], y[0], z[0], 0.,
        x[1], y[1], z[1], 0.,
        x[2], y[2], z[2], 0.,
        0., 0., 0., 1. };

    // save and restore current matrix mode.
    GLenum matrixMode;
    glGetIntegerv( GL_MATRIX_MODE, (GLint*)( &matrixMode ) );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMultMatrixd( m );
    view.multInverseAffine();

    glMatrixMode( matrixMode );
}

static void
drawScene()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    // Bind the cube map texture object. No need to check OpenGL
    //   version here; ogld::CubeMap class handles that.
    cubeMapTexture->apply();
    setCubeMapTextureMatrix();


    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glPushMatrix();

    glTranslatef( 0.f, 0.f, 0.f );

    glColor3f( 0.f, .5f, 0.f );
    sphere->draw();

    glPopMatrix();
    glPopAttrib();

    
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glPushMatrix();

    glTranslatef( -4.f, 0.f, 0.f );

    glColor3f( .5f, 0.f, 1.f );
    torus->draw();

    glPopMatrix();
    glPopAttrib();


    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glPushMatrix();

    glTranslatef( 4.f, 0.f, 0.f );

    glColor3f( 1.f, 0.f, 0.f );
    cyl->draw();

    glPopMatrix();
    glPopAttrib();


    glPopAttrib();


    OGLDIF_CHECK_ERROR;
}


static void
display()
{
    glLoadIdentity();
    view.multMatrix();

    drawScene();

    glutSwapBuffers();
}

static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 2., 250. );

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
mainMenuCB( int value )
{
    if (value == QUIT_VALUE)
    {
        if (cyl)
            delete cyl;
        if (sphere)
            delete sphere;
        if (torus)
            delete torus;
        if (cubeMapTexture)
            delete cubeMapTexture;
        torus = NULL;
        cyl = NULL;
        sphere = NULL;
        torus = NULL;
        cubeMapTexture = NULL;

        exit( 0 );
    }

    glutPostRedisplay();
}


static void
init()
{
    if (ogld::OGLDif::instance()->getVersion() < ogld::Ver13)
    {
        std::cout << "OpenGL run time version is less than 1.3. Cube mapping is unavailable," << std::endl;
        std::cout << "  This example is not fully functional." << std::endl;
    }


    cubeMapTexture = ogld::CubeMap::create();

    cyl = ogld::Cylinder::create( .6f, 5.f, 16 );
    cyl->cap( true, true );

    sphere = ogld::Sphere::create( 1.5, 2 );

    torus = ogld::Torus::create( 1.5, .6, 32, 16 );

    view.setDirUp( ogld::Vec3d( 0.f, 0.f, -1.f ), ogld::Vec3d( 0.f, 1.f, 0.f ) );
    view.setDistance( 15. );


#ifdef GL_VERSION_1_3
    if (ogld::OGLDif::instance()->getVersion() >= ogld::Ver13)
    {
        // While the ogld::CubeMap class won't produce correct results unless
        //   using OpenGL version 1.3 or later, we can still instantiate and
        //   use it regardless. This means the only version-specific code
        //   we need is right here at init time.
        // This code uses the multitexturing, GL_ADD texture environment, cube
        //   mapping, and texture coordinate reflection features, all of which
        //   are available in OpenGL version 1.3 or later.
        cubeMapTexture->setUnit( GL_TEXTURE0 );

        cubeMapTexture->apply(); // Sets glActiveTexture()
        glEnable( GL_TEXTURE_CUBE_MAP );
        cubeMapTexture->setEnvMode( GL_ADD );
        glEnable( GL_TEXTURE_GEN_S );
        glEnable( GL_TEXTURE_GEN_T );
        glEnable( GL_TEXTURE_GEN_R );
        glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP );
        glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP );
        glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP );
    }
#endif

    glEnable( GL_LIGHTING );
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT, GL_DIFFUSE );
    glEnable( GL_LIGHT0 );

    glClearColor( .4, .4, 1., 1. );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    OGLDIF_CHECK_ERROR;

    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );
    glutSpecialFunc( specialKey );

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
    glutInitWindowSize( 450, 450 );
    glutCreateWindow( "Cube Map Textures" );

    init();

    glutMainLoop();

    return 0;
}

