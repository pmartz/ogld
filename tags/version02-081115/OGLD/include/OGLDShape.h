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

#ifndef __OGLD_SHAPE_H__
#define __OGLD_SHAPE_H__


namespace ogld
{


class Shape
{
public:
    Shape();
    virtual ~Shape();

    virtual void draw() = 0;

    void setTextures( int numTextures );
    int getTextures() const;

protected:
    int _numTextures;
};


}


#endif
