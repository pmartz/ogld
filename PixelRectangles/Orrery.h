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
    class Pixels;
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

    ogld::Pixels* _sunImage;
    ogld::Pixels* _earthImage;
    ogld::Pixels* _moonImage;
};

#endif
