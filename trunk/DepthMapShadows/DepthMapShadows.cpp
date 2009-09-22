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
// Issue001, Issue002 (see issues.txt)
//
// This example is known to render incorrectly with the Mesa v6.4.1
// software renderer, and a NVIDIA GeForce 6800 with v81.98 drivers.
// NVIDIA workaround: revert to driver version 78.01.
//

#include "OGLDPlatformGL.h"
#include "OGLDif.h"

// This demo is designed for OpenGL version 1.4 or greater.
#ifdef GL_VERSION_1_4


#include "OGLDPlatformGLUT.h"
#include "OGLDPlane.h"
#include "OGLDTorus.h"
#include "OGLDSphere.h"
#include "OGLDCylinder.h"
#include "OGLDAltAzView.h"
#include "OGLDTexture.h"
#include "OGLDMath.h"
#include <math.h>
#include <iostream>
//#include <stdlib.h>
#include <assert.h>



static const int TOGGLE_LIGHT_VIEW_VALUE( 1 );
static const int QUIT_VALUE( 99 );


GLfloat lightPosition[4] = { 5.5f, -4.f, 9.f, 1.f };

ogld::Plane* basePlane = NULL;
ogld::Cylinder* cyl = NULL;
ogld::Torus* torus = NULL;
ogld::Sphere* sphere = NULL;
ogld::Sphere* lightSphere = NULL;
ogld::Texture* depthMapTexture = NULL;
ogld::Texture* torusTex = NULL;
ogld::Texture* cylTex = NULL;
ogld::Texture* sphereTex = NULL;
ogld::Texture* petroglyphs;
ogld::AltAzView eyeView;
ogld::AltAzView lightView;
ogld::View* view;

ogld::Vec3d viewCenter( 0.f, -1.f, 2.f );

int lastY( 0 );
bool viewMotion( false );
bool shift( false );



static void
drawScene( bool createShadows )
{
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glColor3f( 1.f, 1.f, 1.f );

    if (createShadows)
    {
        // When creating a depth map shadow, the goal is to obtain the
        //   contents of the depth biffer. As a result, lighting is
        //   irrelevant, and disabling it could improve performance.
        glDisable( GL_LIGHT0 );

        // Enabling polygon offset here minimizes aliasing artifacts
        //   by pushing the rendering back into the depth buffer.
        glEnable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( 2.f, 8.f );
    }
    else
    {
        // If not creating a depth map shadow, then this pass must be
        //   rendering the scene as seen from the eye. Position the
        //   light in case we're doing a lit pass (as opposed to an
        //   ambient pass).
        glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

        depthMapTexture->apply();
        glEnable( GL_TEXTURE_2D );
    }


    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Position and render the sphere.
    glPushMatrix();

    glTranslatef( 0.f, 0.f, 2.f );

    if (!createShadows)
    {
        // Only use texture mapping if not creating the depth map
        //   shadow.
        sphereTex->apply();
        glEnable( GL_TEXTURE_2D );
    }
    sphere->draw();

    glPopMatrix();


    // Position and render the torus.
    glPushMatrix();

    glTranslatef( 0.f, -4.f, 2.1f );
    glRotatef( 45.f, 0.f, 0.f, 1.f );

    if (!createShadows)
    {
        // Only use texture mapping if not creating the depth map
        //   shadow.
        torusTex->apply();
        glEnable( GL_TEXTURE_2D );
    }
    torus->draw();

    glPopMatrix();


    // Position and render the cylinder.
    glPushMatrix();

    glTranslatef( 2.5f, -.8f, 2.5f );

    if (!createShadows)
    {
        // Only use texture mapping if not creating the depth map
        //   shadow.
        cylTex->apply();
        glEnable( GL_TEXTURE_2D );
    }
    cyl->draw();

    glPopMatrix();
    glPopAttrib();


    // The two remaining primitives to draw are the ground plane and
    //   the light source sphere. When creating a depth map shadow,
    //   these objects don't need to be present in the depth buffer,
    //   so skip them in that case.
    if (!createShadows)
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );

        petroglyphs->apply();
        glEnable( GL_TEXTURE_2D );
        basePlane->draw();
        glDisable( GL_TEXTURE_2D );

    
        glPushMatrix();

        // The light source sphere is the only primitive in the
        //   final image that is unaffected by the depth map
        //   shadow, so disable the depth map when rendering the
        //   light source sphere.
        glActiveTexture( depthMapTexture->getUnit() );
        glDisable( GL_TEXTURE_2D );

        glTranslated( lightPosition[0], lightPosition[1], lightPosition[2] );
        glDisable( GL_LIGHTING );
        lightSphere->draw();

        glPopMatrix();
        glPopAttrib();
    }


    glPopAttrib();
}


// Computes transformation parameters required both for rendering the
//   depth map texture, and texture coordinate generation.
static void
computeTransformParameters( double& fovy, double& aspect, int& w, int& h )
{
    // Obtain the window dimensions. Compute the highest power of 2
    //   width and height that fit within the window.
    w = glutGet( GLUT_WINDOW_WIDTH );
    h = glutGet( GLUT_WINDOW_HEIGHT );
    assert( w < 0x10000 );
    assert( h < 0x10000 );
    w = ogld::ceilPower2( (unsigned short)w ) >> 1;
    h = ogld::ceilPower2( (unsigned short)h ) >> 1;

    aspect = (double)w/(double)h;

    // Compute an optimal field of view based on the light distance.
    //   This ensures the generated texture has as much precision
    //   as possible given the window size we'll be rendering into.
    fovy = ogld::radiansToDegrees( atan( 10. / (lightView.getDistance() - 5.) ) );
}

static void
createDepthMapTexture()
{
    // Obtain the transformation parameters.
    int w, h;
    double fovy, aspect;
    computeTransformParameters( fovy, aspect, w, h );

    // Configure the depth map texture object and reserve space for
    //   the generated texure.
    depthMapTexture->setFormat( GL_DEPTH_COMPONENT );
    depthMapTexture->setType( GL_UNSIGNED_INT );
    depthMapTexture->reserve( w, h );


    // Not sure what the current matrix mode and viewport are, so set
	//   up to restore it when we're done.
    glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

    // Render the depth map texture using a standard projection matrix.
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluPerspective( fovy, aspect, 2., 200. );

    // Set up to view from the light position
    // The display() method will set the model-view matrix for each
    //   frame, so no need to push/pop it.
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    lightView.multMatrix();

    // Render
    drawScene( true );

    // Restore the projection matrix.
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    // Restore the matrix mode.
    glPopAttrib();

    // Copy the depth buffer into the depth map texture object.
    glBindTexture( GL_TEXTURE_2D, depthMapTexture->getObject() );
    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, w, h, 0 );
}

// We'll use eye linear texture coordinate generation, but we need
//   coordinates generated as if the light source were the eye point.
//   This function computes and sets the strq plane equations to
//   accomplish this.
static void eyeToLightSpace()
{
    // Obtain the transformation parameters.
    int w, h;
    double fovy, aspect;
    computeTransformParameters( fovy, aspect, w, h );


    // Not sure what the current matrix mode is, so set up to
    //   restore it when we're done.
    glPushAttrib( GL_TRANSFORM_BIT );

    // Temporarily use the model-view matrix to create
    //   the texture coordinate transform
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    glLoadIdentity();
    // Concatenate four matrices that transform from light space to
    //   depth map texture coordinates in the range 0 to 1. Use the
    //   same model-view and projection matrices as were used to
    //   generate the depth map, than transform the (-1, 1) NDC
    //   cube into (0, 1) texture coordinate range.
    glTranslated( .5, .5, .5 );
    glScaled( .5, .5, .5 );
    gluPerspective( fovy, aspect, 2., 200. );
    lightView.multMatrix();

    // Get the single matrix that performs this transformation.
    GLdouble m[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, m );

    glPopMatrix();

    glPopAttrib();

    // m now contains the plane equation values, but separate
    //   values in each equation aren't stored in contiguous
    //   memory. Transpose the matrix to remedy this.
    ogld::matrixTranspose( m );

    // Set the active texture unit.
    glActiveTexture( depthMapTexture->getUnit() );

    // Specify the texture coordinate plane equations.
    glTexGendv( GL_S, GL_EYE_PLANE, &(m[0]) );
    glTexGendv( GL_T, GL_EYE_PLANE, &(m[4]) );
    glTexGendv( GL_R, GL_EYE_PLANE, &(m[8]) );
    glTexGendv( GL_Q, GL_EYE_PLANE, &(m[12]) );
}


static void
display()
{
    // Update the depth map only when the light position changes.
    //   (Changing the light position invalidates the depth map.)
    if ( !depthMapTexture->valid() )
    {
        glClear( GL_DEPTH_BUFFER_BIT );
        createDepthMapTexture();
    }


    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    view->multMatrix();
    eyeToLightSpace();


    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0. );

    // Render unshadowed / lit fragments
    depthMapTexture->apply();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS );
    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA );
    drawScene( false );

    // Disable the light and render shadowed / unlit fragments
    depthMapTexture->apply();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL );
    glDisable( GL_LIGHT0 );

    drawScene( false );

    glPopAttrib();


    glutSwapBuffers();


    OGLDIF_CHECK_ERROR;
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

    // Invalidate the shadow texture
    depthMapTexture->unload();
}

static void
motion( int x, int y )
{
    // viewMotion will never be true if we're viewing from the
    //   light source.
    if (viewMotion)
    {
        if (shift)
        {
            const int deltaY = y-lastY;
            view->setDistance( view->getDistance() * ( 1. - (deltaY / 150.) ) );
        }
        else
            view->mouse( x, y );

        lastY = y;
    }

    glutPostRedisplay();
}

static void
mouse( int button, int state, int x, int y )
{
    // If we're viewing from the light source, return. Don't allow user
    //   to interact with the view in this case.
    if (view == &lightView)
        return;

    shift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;

    if ( viewMotion = (button == GLUT_LEFT) )
    {
        view->mouseDown( x, y );
        lastY = y;
    }
}

static void
specialKey( int key, int x, int y )
{
    // Push the light around using the arrow keys.
    switch (key)
    {
    case GLUT_KEY_LEFT:
        lightPosition[0] -= .2f;
        break;
    case GLUT_KEY_RIGHT:
        lightPosition[0] += .2f;
        break;
    case GLUT_KEY_DOWN:
        lightPosition[1] -= .2f;
        break;
    case GLUT_KEY_UP:
        lightPosition[1] += .2f;
        break;
    default:
        return;
        break;
    }

    // Create a view matrix as if the eye were at the light source.
    ogld::Vec3d light( -lightPosition[0], -lightPosition[1], -lightPosition[2] );
    light += viewCenter;
    lightView.setDirUp( light, ogld::Vec3d( 0.f, 0.f, 1.f ) );
    lightView.setDistance( (float) light.len() );

    // The light source changed. Unload and invalidate the depth map
    //   texture. This will cause it to be regenerated in the next
    //   display() callback.
    depthMapTexture->unload();

    glutPostRedisplay();
}

static void
mainMenuCB( int value )
{
    if (value == TOGGLE_LIGHT_VIEW_VALUE)
    {
        if (view == &lightView)
            view = &eyeView;
        else
        {
            view = &lightView;
            viewMotion = false;
        }
    }

    else if (value == QUIT_VALUE)
    {
        if (basePlane)
            delete basePlane;
        if (cyl)
            delete cyl;
        if (sphere)
            delete sphere;
        if (lightSphere)
            delete lightSphere;
        if (torus)
            delete torus;
        if (depthMapTexture)
            delete depthMapTexture;
        if (torusTex)
            delete torusTex;
        if (cylTex)
            delete cylTex;
        if (sphereTex)
            delete sphereTex;
        if (petroglyphs)
            delete petroglyphs;
        basePlane = NULL;
        cyl = NULL;
        sphere = lightSphere = NULL;
        torus = NULL;
        depthMapTexture = torusTex = cylTex =
            sphereTex = petroglyphs = NULL;

        exit( 0 );
    }

    glutPostRedisplay();
}


static void
init()
{
    if (ogld::OGLDif::instance()->getVersion() < ogld::Ver14)
    {
        std::cout << "Can't execute the DepthMapShadows example:" << std::endl;
        std::cout << "  OpenGL version 1.4 or greater required at run time." << std::endl;

        exit( 1 );
    }


    // Create and configure the geometry objects
    basePlane = ogld::Plane::create( 20.f, 20.f, 32, 32 );
    basePlane->setTextures( 1 );

    cyl = ogld::Cylinder::create( .6f, 5.f, 32 );
    cyl->cap( true, false );

    sphere = ogld::Sphere::create( 1.5, 3 );
    lightSphere = ogld::Sphere::create( .25, 2 );

    torus = ogld::Torus::create( 1.5, .6, 128, 64 );

    // Create the view
    eyeView.setDirUp( ogld::Vec3d( 0.f, 1.f, -.2f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    eyeView.setDistance( 15. );
    eyeView.setCenter( viewCenter );

    // Store the initial light position and create a view from the light source.
    ogld::Vec3d light( -lightPosition[0], -lightPosition[1], -lightPosition[2] );
    light += viewCenter;
    lightView.setDirUp( light, ogld::Vec3d( 0.f, 0.f, 1.f ) );
    lightView.setDistance( (float) light.len() );
    lightView.setCenter( viewCenter );
    view = &eyeView;


    // Create and initialize textures
    torusTex = ogld::Texture::create();
    cylTex = ogld::Texture::create();
    sphereTex = ogld::Texture::create();
    petroglyphs = ogld::Texture::create();

    bool result;
    petroglyphs->setUnit( GL_TEXTURE0 );
    result = petroglyphs->loadImage( std::string( "NewspaperRock.tif" ) );
    assert( result );

    torusTex->setUnit( GL_TEXTURE0 );
    result = torusTex->loadImage( std::string( "texture0.tif" ) );
    assert( result );

    cylTex->setUnit( GL_TEXTURE0 );
    result = cylTex->loadImage( std::string( "texture2.tif" ) );
    assert( result );

    sphereTex->setUnit( GL_TEXTURE0 );
    result = sphereTex->loadImage( std::string( "texture1.tif" ) );
    assert( result );


    // Create and initialize the depth map texture.
    depthMapTexture = ogld::Texture::create();
    depthMapTexture->setUnit( GL_TEXTURE1 );
    glActiveTexture( depthMapTexture->getUnit() );

    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );
    glEnable( GL_TEXTURE_GEN_R );
    glEnable( GL_TEXTURE_GEN_Q );
    glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
    glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
    glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
    glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );


    glEnable( GL_LIGHTING );
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT, GL_DIFFUSE );
    glEnable( GL_LIGHT0 );

    glClearColor( .4, .4, .4, 1. );
    glDisable( GL_DITHER );
    glEnable( GL_DEPTH_TEST );

    OGLDIF_CHECK_ERROR;

    glutDisplayFunc( display ); 
    glutReshapeFunc( reshape );
    glutMotionFunc( motion );
    glutMouseFunc( mouse );
    glutSpecialFunc( specialKey );

    glutCreateMenu( mainMenuCB );
    glutAddMenuEntry( "Toggle view from light", TOGGLE_LIGHT_VIEW_VALUE );
    glutAddMenuEntry( "Quit", QUIT_VALUE );
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

int
main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Depth Map Shadows" );

    init();

    glutMainLoop();

    return 0;
}



#else
// OpenGL version 1.4 not supported at compile time.

#include <iostream>

int
main( int argc, char** argv )
{
    std::cout << "Can't execute the DepthMapShadows example:" << std::endl;
    std::cout << "  OpenGL version 1.4 or greater required at compile time." << std::endl;

    return 1;
}

#endif
