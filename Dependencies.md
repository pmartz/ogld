# GNU Make 3.80 #

The example code uses a multi-platform makefile pattern that requires GNU Make v3.80 or later.

> Apple, Linux, and Cygwin—If your system doesn't have GNU Make 3.80, download the full source and follow the instructions to build and install it.
> > [GNU Make source code](http://ftp.gnu.org/pub/gnu/make/make-3.80.tar.gz)

> Windows—You don't need GNU Make to build using Visual C++ v6.0 or Visual Studio .NET. The example code package comes with Visual C++ v6.0 project and workspace files (also compatible with Visual Studio .NET).

# libTIFF #

> All platforms—Distributions in gzipped tar and Windows zip format are available from the libTIFF master download site.
> > [libTIFF—TIFF Library and Utilities Web site](http://www.libtiff.org/)

> Microsoft Windows—Developer files and binaries specifically for Microsoft Windows are available from the TIFF for Windows Web site.
> > [TIFF for Windows](http://gnuwin32.sourceforge.net/packages/tiff.htm)

# glext.h #

If building the examples on Microsoft Windows or Linux, you'll need the glext.h header file, which provides definitions and declarations for OpenGL since version 1.1. (If you are using Mesa on Linux, you probably already have this file.) This file is available from the OpenGL Extension Registry:


> [glext.h](http://oss.sgi.com/projects/ogl-sample/ABI/glext.h)

glext.h is not required to build the examples on Apple Mac OS X.

# wglext.h #

If building the examples on Microsoft Windows, you'll need the wglext.h header file, which provides definitions and declarations for WGL extensions. This file is available from the OpenGL Extension Registry:

> [wglext.h](http://oss.sgi.com/projects/ogl-sample/ABI/wglext.h)

wglext.h is not required to build the examples on Linux and Apple Mac OS X.