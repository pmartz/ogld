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
#include "OGLDPlane.h"
#include "OGLDTorus.h"
#include "OGLDSphere.h"
#include "OGLDCylinder.h"
#include "OGLDAltAzView.h"
#include "OGLDTexture.h"
#include "OGLDMath.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>


static const int TOGGLE_LIGHT_VIEW_VALUE( 1 );
static const int QUIT_VALUE( 99 );


GLfloat lightPosition[4] = { 5.5f, -4.f, 9.f, 1.f };

ogld::Plane* planeHi = NULL;
ogld::Plane* planeLo = NULL;
ogld::Cylinder* cyl = NULL;
ogld::Torus* torus = NULL;
ogld::Sphere* sphere = NULL;
ogld::Sphere* lightSphere = NULL;
ogld::Texture* shadowTexture = NULL;
ogld::Texture* torusTex = NULL;
ogld::Texture* cylTex = NULL;
ogld::Texture* sphereTex = NULL;
ogld::Texture* petroglyphs = NULL;
ogld::AltAzView eyeView;
ogld::AltAzView lightView;
ogld::AltAzView makeTextureView;
ogld::View* view;

ogld::Vec3d viewCenter( 0.f, 0.f, 2.f );

int lastY( 0 );
bool viewMotion( false );
bool shift( false );


static void
createProjectedShadowMatrix( GLdouble m[16] )
{
    double lPos[4], peq[4];
    lPos[0] = lightPosition[0];
    lPos[1] = lightPosition[1];
    lPos[2] = lightPosition[2];
    lPos[3] = lightPosition[3];
    peq[0] = 0.f;
    peq[1] = 0.f;
    peq[2] = 1.f;
    peq[3] = 0.f;

    const double dp = lPos[0]*peq[0] + lPos[1]*peq[1] + lPos[2]*peq[2] + lPos[3]*peq[3];

    // Create a matrix that projects geometry as viewed from the position
    //   lPos (the light position) onto the plane specified by the plane
    //   equation peq. Since our plane equation is trivial, we could greatly
    //   simplify the math here, though I've left the full computation in
    //   place for education and possible later modification.
    //
    // For a recent discussion of this and other shadow algorithms, see
    //   Savchenko, Sergei. "Algorithms for Dynamic Shadows."
    //   Dr. Dobb’s Journal (February 2005).

    m[0] = dp - lPos[0]*peq[0];
    m[1] = -lPos[1]*peq[0];
    m[2] = -lPos[2]*peq[0];
    m[3] = -lPos[3]*peq[0];

    m[4] = -lPos[0]*peq[1];
    m[5] = dp - lPos[1]*peq[1];
    m[6] = -lPos[2]*peq[1];
    m[7] = -lPos[3]*peq[1];

    m[8] = -lPos[0]*peq[2];
    m[9] = -lPos[1]*peq[2];
    m[10] = dp - lPos[2]*peq[2];
    m[11] = -lPos[3]*peq[2];

    m[12] = -lPos[0]*peq[3];
    m[13] = -lPos[1]*peq[3];
    m[14] = -lPos[2]*peq[3];
    m[15] = dp - lPos[3]*peq[3];
}


static void
drawScene( bool drawShadows )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if (drawShadows)
    {
        // To create the projected shadow, first create a matrix to
        //   perform the projection, then multiply it onto the
        //   model-view matrix.
        double m[16];
        createProjectedShadowMatrix( m );

        glPushMatrix();
        glMultMatrixd( m );

        // Draw the hi-resolution plane with lighting enabled, to
        //   capture lighting effects.
        planeHi->draw();

        // Next, render the sphere, torus, and cylinder with lighting
        //   and depth test disabled.
        glDisable( GL_LIGHT0 );
        glDisable( GL_DEPTH_TEST );
    }



    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glPushMatrix();

    glTranslatef( 0.f, 0.f, 2.f );

    if (!drawShadows)
    {
        // If not creating the shadow, apply the texture and enable
        //   texture mapping. Not necessary when creating the shadow.
        sphereTex->apply();
        glEnable( GL_TEXTURE_2D );
    }
    sphere->draw();

    glPopMatrix();


    glPushMatrix();

    glTranslatef( 0.f, -4.f, 2.1f );
    glRotatef( 45.f, 0.f, 0.f, 1.f );

    if (!drawShadows)
        torusTex->apply();
    torus->draw();

    glPopMatrix();


    glPushMatrix();

    glTranslatef( 2.5f, -.8f, 2.5f );

    if (!drawShadows)
        cylTex->apply();
    cyl->draw();

    glPopMatrix();
    glPopAttrib();



    if (!drawShadows)
    {
        glPushAttrib( GL_ALL_ATTRIB_BITS );

        // Lighting isn't needed for drawing the plane.
        glDisable( GL_LIGHTING );

        if (ogld::OGLDif::instance()->getVersion() >= ogld::Ver13)
        {
            // Multitexturing is enabled, so apply the petroglyph
            //   texture before drawing the plane.
            petroglyphs->apply();
            glEnable( GL_TEXTURE_2D );
        }
        // Also apply the shadow texture. If multitexturing is
        //   unavailable, this will be the only texture on the plane.
        shadowTexture->apply();
        glEnable( GL_TEXTURE_2D );

        // Lo-res plane; gets all its lighting effects from
        //   shadowTexture.
        planeLo->draw();

        glPopAttrib();

    
        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glPushMatrix();

        // Lighting isn't needed for drawing the light source.
        glDisable( GL_LIGHTING );

        glTranslated( lightPosition[0], lightPosition[1], lightPosition[2] );
        lightSphere->draw();

        glPopAttrib();
        glPopMatrix();
    }

    else
        // Pop the projected shadow matrix
        glPopMatrix();

    OGLDIF_CHECK_ERROR;
}


void
createShadowTexture()
{
    int w = glutGet( GLUT_WINDOW_WIDTH );
    int h = glutGet( GLUT_WINDOW_HEIGHT );
    if (ogld::OGLDif::instance()->getVersion() < ogld::Ver20)
    {
        // OpenGL version is less than 20. Make sure we only render to
        //   the highest power of 2 that fits within the window. If
        //   the version is 2.0 or greater, non power of 2 textures
        //   are supported, so render to the full window.
        assert( w < 0x10000 );
        assert( h < 0x10000 );
        w = ogld::ceilPower2( (unsigned short)w ) >> 1;
        h = ogld::ceilPower2( (unsigned short)h ) >> 1;
    }
    // The shadow map is a single-channel luminance texture.
    shadowTexture->setFormat( GL_LUMINANCE );
    // We're going to copy the contents of the framebuffer into this
    //   texture object, so reserve space to hold it.
    shadowTexture->reserve( w, h );

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Set an orthograph projection just large enough to contain
    //   the base plane.
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( -10., 10., -10., 10., -3.5, 3.5 );

    // Set a bird's eye view of the base plane.
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    makeTextureView.multMatrix();

    // Position the light, given the new model-view matrix. (The light
    //   position is transformed into eye space, so always re-specify
    //   it after a change in view or change in light position.)
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    drawScene( true );
    // The framebuffer now contains:
    //  * Hi-resolution base plane with lighting effects.
    //  * Ambient-only shadows on that plane from the sphere, torus,
    //    and cylinder.

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
    glPopAttrib();

    // Copy the contents of the back buffer into the shadowTexture
    //   texture object.
    shadowTexture->apply();
    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, w, h, 0 );
}


static void
display()
{
    // The shadowTexture is invalidated when the window is resized or
    //   the user repositions the light. If it's invalid, re-create it.
    if (!shadowTexture->valid())
        createShadowTexture();
        // Note that if the code were to swap the buffers here and
        //   return, the shadow map shown in figure 6-8 (page 161)
        //   would be displayed.


    // Specify the view matrix and re-specify the light position.
    glLoadIdentity();
    view->multMatrix();
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    drawScene( false );

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
    shadowTexture->unload();
}

static void
motion( int x, int y )
{
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
    // Use arrow keys to drive the light position.
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

    // Recompute the "view from light"; view direction is the negation
    //   of the light position vector.
    ogld::Vec3d light( -lightPosition[0], -lightPosition[1], -lightPosition[2] );
    light += viewCenter;
    lightView.setDirUp( light, ogld::Vec3d( 0.f, 0.f, 1.f ) );
    lightView.setDistance( (float) light.len() );

    // Invalidate the shadow texture so that it will be recomputed to
    //   account for the change in light position.
    shadowTexture->unload();

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
        if (planeHi)
            delete planeHi;
        if (planeLo)
            delete planeLo;
        if (cyl)
            delete cyl;
        if (sphere)
            delete sphere;
        if (lightSphere)
            delete lightSphere;
        if (torus)
            delete torus;
        if (shadowTexture)
            delete shadowTexture;
        if (torusTex)
            delete torusTex;
        if (cylTex)
            delete cylTex;
        if (sphereTex)
            delete sphereTex;
        if (petroglyphs)
            delete petroglyphs;
        planeHi = planeLo = NULL;
        cyl = NULL;
        sphere = lightSphere = NULL;
        torus = NULL;
        shadowTexture = torusTex = cylTex =
            sphereTex = petroglyphs = NULL;

        exit( 0 );
    }

    glutPostRedisplay();
}


static void
init()
{
    // Hi-resolution plane (64 triangle strips with 128 triangles
    //   per strip) is used to render the shadow map with lighting
    //   effects.
    planeHi = ogld::Plane::create( 20.f, 20.f, 64, 64 );
    // Low-resolution plane is used to render the final scene, using
    //   the shadow map texture for lighting effects.
    planeLo = ogld::Plane::create( 20.f, 20.f, 1, 1 );

    // Create and configure the other objects in the scene: a cylinder,
    //   torus, sphere, and a light source sphere.
    cyl = ogld::Cylinder::create( .6f, 5.f, 32 );
    cyl->cap( true, false );
    sphere = ogld::Sphere::create( 1.5, 3 );
    lightSphere = ogld::Sphere::create( .25, 2 );
    torus = ogld::Torus::create( 1.5, .6, 128, 64 );

    // Create the main scene view.
    eyeView.setDirUp( ogld::Vec3d( 0.f, 1.f, -.2f ), ogld::Vec3d( 0.f, 0.f, 1.f ) );
    eyeView.setDistance( 15. );
    eyeView.setCenter( viewCenter );

    // This example allows the user to also view the scene from the
    //   light source location. Create a view for that.
    ogld::Vec3d light( -lightPosition[0], -lightPosition[1], -lightPosition[2] );
    light += viewCenter;
    lightView.setDirUp( light, ogld::Vec3d( 0.f, 0.f, 1.f ) );
    lightView.setDistance( (float) light.len() );
    lightView.setCenter( viewCenter );
    view = &eyeView;

    // Create a third view used when rendering the shadow map. This is
    //   a bird's eye view directly above the scene.
    makeTextureView.setDirUp( ogld::Vec3d( 0.f, 0.f, -1.f ), ogld::Vec3d( 0.f, 1.f, 0.f ) );
    makeTextureView.setDistance( 1. );
    makeTextureView.setCenter( viewCenter );


    // Create and load texture maps
    shadowTexture = ogld::Texture::create();
    torusTex = ogld::Texture::create();
    cylTex = ogld::Texture::create();
    sphereTex = ogld::Texture::create();
    petroglyphs = ogld::Texture::create();

    bool result;
    result = torusTex->loadImage( std::string( "texture0.tif" ) );
    assert( result );

    result = cylTex->loadImage( std::string( "texture2.tif" ) );
    assert( result );

    result = sphereTex->loadImage( std::string( "texture1.tif" ) );
    assert( result );

    if (ogld::OGLDif::instance()->getVersion() >= ogld::Ver13)
    {
        // Multitexturing is enabled. Create a petroglyph texture for
        //   the base plane. Use GL_REPLACE texture environment mode,
        //   as we will modulate the lighting effects over this
        //   texture with the shadowTexture texture map.
        result = petroglyphs->loadImage( std::string( "NewspaperRock.tif" ) );
        assert( result );
        petroglyphs->setEnvMode( GL_REPLACE );

#ifdef GL_VERSION_1_3
        // Set texture unit to 1 for multitexturing.
        shadowTexture->setUnit( GL_TEXTURE1 );
#endif

        // Tell the low-resolution plane object that, when rendering,
        //   it needs to issue texture coordinate sets for both unit
        //   1 and unit 2.
        planeLo->setTextures( 2 );
    }


    glEnable( GL_LIGHTING );
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT, GL_DIFFUSE );
    glEnable( GL_LIGHT0 );
    glColor3f( 1.f, 1.f, 1.f );

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
    glutInitWindowSize( 500, 500 );
    glutCreateWindow( "Projected Shadows" );

    init();

    glutMainLoop();

    return 0;
}
