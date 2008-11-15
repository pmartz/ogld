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

#include "OGLDShape.h"
#include "OGLDCylinder.h"
#include "OGLDSphere.h"
#include "OGLDTorus.h"
#include "OGLDif.h"
#include "Scene.h"
#include <assert.h>


// ShapeInfo is a simple class for storing two copies of a single piece of geometry,
//   one high-res for rendering with maximum detail, the other low-res for fast picking.
// The class also stores an object space translation vector for position.
ShapeInfo::ShapeInfo( ogld::Shape* draw, ogld::Shape* pick )
  : _draw( draw ),
    _pick( pick )
{
}

ShapeInfo::~ShapeInfo()
{
    if (_draw)
        delete _draw;
    if (_pick)
        delete _pick;
    _draw = _pick = NULL;
}




Scene::Scene()
  : _idx( -1 )
{
    // Create our thrww pieces of geometry. Create two copies of each for
    //   rendering and picking.
    ShapeInfo* sInfo = new ShapeInfo(
        ogld::Sphere::create( 1.f, 3 ), ogld::Sphere::create( 1.f, 1 ) );
    sInfo->_trans = ogld::Vec3d( -4.f, 0.f, 0.f );
    _shapes.push_back( sInfo );

    // (When creating the cylinders, specify they should be capped.)
    ogld::Cylinder* cylHi = ogld::Cylinder::create( .25f, 4.f, 16 );
    ogld::Cylinder* cylLo = ogld::Cylinder::create( .25f, 4.f, 4 );
    cylHi->cap( true, true );
    cylLo->cap( true, true );
    sInfo = new ShapeInfo( cylHi, cylLo );
    _shapes.push_back( sInfo );

    sInfo = new ShapeInfo(
        ogld::Torus::create( 1.5f, .25f, 64, 32 ), ogld::Torus::create( 1.5f, .25f, 16, 4 ) );
    sInfo->_trans = ogld::Vec3d( 4.f, 0.f, 0.f );
    _shapes.push_back( sInfo );
}

Scene::~Scene()
{
    int i;
    for (i=0; i<(int)(_shapes.size()); i++)
    {
        ShapeInfo* sInfo = _shapes[i];
        delete sInfo;
    }
}

void Scene::render()
{
    // Render each object in the scene.
    int i;
    for (i=0; i<(int)(_shapes.size()); i++)
    {
        ShapeInfo* sInfo = _shapes[i];

        // Position the object in object coordinate space. If the object was
        //   picked and dragged, the _trans vector sill store its dragged location.
        glPushMatrix();
        glTranslated( sInfo->_trans[0], sInfo->_trans[1], sInfo->_trans[2] );

        glPushAttrib( 0xffffffff );
        if (_idx != i)
        {
            // Use lighting to draw objects that aren't selected.
            glEnable( GL_LIGHTING );
            glEnable (GL_LIGHT0 );
        }
        glColor3f( 1.f, 1.f, 1.f );
        sInfo->_draw->draw();
        glPopAttrib();

        if (_idx == i)
        {
            // If the object is selected, use polygon offset to draw it in
            //   wireframe, thus "highlighting" the object of interest.
            glPushAttrib( 0xffffffff );
            glPolygonOffset( -1.f, -1.f );
            glEnable( GL_POLYGON_OFFSET_LINE );
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glColor3f( .2f, .2f, .2f );
            sInfo->_draw->draw();
            glPopAttrib();
        }

        glPopMatrix();
    }
}

void Scene::pick()
{
    // Like rendering, send all objects to OpenGL. However, use
    //   the name stack to identify each object, and use the simpler
    //   and lower-resolution version of the geometry to speed the
    //   selection pass.
    glInitNames();
    glPushName( 0xffffffff );
    int i;
    for (i=0; i<(int)(_shapes.size()); i++)
    {
        glLoadName( i );
        ShapeInfo* sInfo = _shapes[i];
        glPushMatrix();
        glTranslated( sInfo->_trans[0], sInfo->_trans[1], sInfo->_trans[2] );
        sInfo->_pick->draw();
        glPopMatrix();
    }
    glPopName();

    OGLDIF_CHECK_ERROR;
}

void Scene::reposition( const ogld::Vec3d& up, const ogld::Vec3d& right, int dX, int dY )
{
    // Move the selected object by the specified delta along the given 'up'
    //   and 'right' vectors.
    ShapeInfo* sInfo = _shapes[_idx];

    ogld::Vec3d u = right;
    ogld::Vec3d v = up;
    u.scale( dX * .01f );
    v.scale( dY * .01f );
    u += v;
    sInfo->_trans += u;
}

void Scene::reposition( const ogld::Vec3d& delta )
{
    // Move the selected objet by the given delta vector.
    ShapeInfo* sInfo = _shapes[_idx];

    sInfo->_trans += delta;
}

// Set and get the selected object index.
void Scene::select( int idx )
{
    _idx = idx;
}
int Scene::getSelect()
{
    return _idx;
}


