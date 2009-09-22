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

#ifndef __OGLD_PLATFORM_GLUT_H__
#define __OGLD_PLATFORM_GLUT_H__

//
// OGLDPlatformGLUT.h
//
// Instead of including glut.h directly, include this file
//   to obtain glut.h declarations and definitions in a
//   platform-independent manner.
//


#if defined( __APPLE__ )


  #include <glut.h>


#else


  #include <GL/glut.h>


#endif


#endif
