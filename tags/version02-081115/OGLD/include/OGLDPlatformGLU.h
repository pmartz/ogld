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

#ifndef __OGLD_PLATFORM_GLU_H__
#define __OGLD_PLATFORM_GLU_H__

//
// OGLDPlatformGLU.h
//
// Instead of including glu.h directly, include this file
//   to obtain glu.h declarations and definitions in a
//   platform-independent manner.
//


#if defined( __APPLE__ )


  #include <OpenGL/glu.h>


#elif defined( WIN32 ) || defined( __CYGWIN__ )


  #include <windows.h>
  #include <GL/glu.h>


#elif defined( GLX )


  #include <GL/glu.h>


#else


  // Unknown platform
  #include <GL/glu.h>


#endif


#endif
