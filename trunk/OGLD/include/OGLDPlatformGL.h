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

#ifndef __OGLD_PLATFORM_GL_H__
#define __OGLD_PLATFORM_GL_H__

//
// OGLDPlatformGL.h
//
// Instead of including gl.h directly, include this file
//   to obtain gl.h declarations and definitions in a
//   platform-independent manner.
//


#if defined( __APPLE__ )


  #include <OpenGL/gl.h>


#elif defined( WIN32 ) || defined( __CYGWIN__ )


  // MS Windows operating systems gl.h requires definitions from
  //   windows.h, or compile errors will ensure.
  #include <windows.h>

  #include <GL/gl.h>


#elif defined( GLX )


  // Mesa gl.h automatically #includes glext.h. Defining
  //   GL_GLEXT_LEGACY gets us function declarations from glext.h
  //   so that we don't need to define them ourselves later.
  #define GL_GLEXT_LEGACY

  #include <GL/gl.h>

  // Linux applications will also need GLX declarations.
  #include <GL/glx.h>


#else


  // Unknown platform
  #include <GL/gl.h>


#endif


#endif
