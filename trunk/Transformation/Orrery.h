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

#ifndef __ORRERY_H__
#define __ORRERY_H__


// Forward declarations
namespace ogld
{
    class Cylinder;
    class Sphere;
};


class Orrery
{
public:
    Orrery();
    ~Orrery();

    void draw( int currentTime );

private:
    float _earthDegrees;
    float _moonDegrees;
    int _lastTime;

    ogld::Sphere* _sun;
    ogld::Sphere* _earth;
    ogld::Sphere* _moon;

    ogld::Cylinder* _sunStem;
    ogld::Cylinder* _earthStem;
    ogld::Cylinder* _moonStem;

    ogld::Cylinder* _earthOrbit;
    ogld::Cylinder* _moonOrbit;
};

#endif
