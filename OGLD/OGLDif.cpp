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
#include "OGLDPlatformGLU.h"

#ifdef OGLDIF_DECLARATION
  #undef OGLDIF_DECLARATION
#endif
#define OGLDIF_DECLARATION
#include "OGLDif.h"

#ifdef GLX
#include <dlfcn.h>
#endif

#include <exception>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <assert.h>



namespace ogld
{


OGLDif* OGLDif::_instance = 0;



OGLDif*
OGLDif::instance()
{
    if (!_instance)
        _instance = new OGLDif;

    if (!_instance->_valid)
        _instance->validate();

    return _instance;
}


OGLDif::OGLDif()
  : _valid( false ),
    _dlHandle( NULL ),
    _version( VerUnknown )
{
    //
    // Prime the data path with likely paths
    addToDataPath( "" );
    addToDataPath( "./" );
    addToDataPath( "Data/" );
    addToDataPath( "../Data/" );
    addToDataPath( "../../Data/" );
}

OpenGLVersion
OGLDif::getVersion()
{
    if (!_valid)
        validate();

    return _version;
}

void
OGLDif::validate()
{
    //
    // Obtain a version enum
    const GLubyte* version = NULL;
    version = glGetString( GL_VERSION );
    if (!version)
        // The only reason this would be NULL is if there
        //   is no current context. Return and validate later.
        return;

    std::string ver( (const char*) version );
    assert( !ver.empty() );
    std::istringstream verStream( ver );

    int major, minor;
    char dummySep;
    verStream >> major >> dummySep >> minor;

    int versionNum = major * 10 + minor;
    switch (versionNum)
    {
    case 10:
        _version = Ver10;
        break;
    case 11:
        _version = Ver11;
        break;
    case 12:
        _version = Ver12;
        break;
    case 13:
        _version = Ver13;
        break;
    case 14:
        _version = Ver14;
        break;
    case 15:
        _version = Ver15;
        break;
    case 20:
        _version = Ver20;
        break;
    default:
        _version = VerExtended;
        break;
    }

    assert( _version >= Ver11 );
    if (_version < Ver11)
        throw std::exception();

    _valid = true;


    //
    // Define function pointers for entry points declared
    //   in glext.h but not in gl.h.
#if defined( GL_VERSION_1_2 ) && defined( __OGLD_NEED_1_2 )
    if (_version >= Ver12)
    {
        glBlendColor = (PFNGLBLENDCOLORPROC) getProcAddress( "glBlendColor" );
        glBlendEquation = (PFNGLBLENDEQUATIONPROC) getProcAddress( "glBlendEquation" );
        glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) getProcAddress( "glDrawRangeElements" );
        glColorTable = (PFNGLCOLORTABLEPROC) getProcAddress( "glColorTable" );
        glColorTableParameterfv = (PFNGLCOLORTABLEPARAMETERFVPROC) getProcAddress( "glColorTableParameterfv" );
        glColorTableParameteriv = (PFNGLCOLORTABLEPARAMETERIVPROC) getProcAddress( "glColorTableParameteriv" );
        glCopyColorTable = (PFNGLCOPYCOLORTABLEPROC) getProcAddress( "glCopyColorTable" );
        glGetColorTable = (PFNGLGETCOLORTABLEPROC) getProcAddress( "glGetColorTable" );
        glGetColorTableParameterfv = (PFNGLGETCOLORTABLEPARAMETERFVPROC) getProcAddress( "glGetColorTableParameterfv" );
        glGetColorTableParameteriv = (PFNGLGETCOLORTABLEPARAMETERIVPROC) getProcAddress( "glGetColorTableParameteriv" );
        glColorSubTable = (PFNGLCOLORSUBTABLEPROC) getProcAddress( "glColorSubTable" );
        glCopyColorSubTable = (PFNGLCOPYCOLORSUBTABLEPROC) getProcAddress( "glCopyColorSubTable" );
        glConvolutionFilter1D = (PFNGLCONVOLUTIONFILTER1DPROC) getProcAddress( "glConvolutionFilter1D" );
        glConvolutionFilter2D = (PFNGLCONVOLUTIONFILTER2DPROC) getProcAddress( "glConvolutionFilter2D" );
        glConvolutionParameterf = (PFNGLCONVOLUTIONPARAMETERFPROC) getProcAddress( "glConvolutionParameterf" );
        glConvolutionParameterfv = (PFNGLCONVOLUTIONPARAMETERFVPROC) getProcAddress( "glConvolutionParameterfv" );
        glConvolutionParameteri = (PFNGLCONVOLUTIONPARAMETERIPROC) getProcAddress( "glConvolutionParameteri" );
        glConvolutionParameteriv = (PFNGLCONVOLUTIONPARAMETERIVPROC) getProcAddress( "glConvolutionParameteriv" );
        glCopyConvolutionFilter1D = (PFNGLCOPYCONVOLUTIONFILTER1DPROC) getProcAddress( "glCopyConvolutionFilter1D" );
        glCopyConvolutionFilter2D = (PFNGLCOPYCONVOLUTIONFILTER2DPROC) getProcAddress( "glCopyConvolutionFilter2D" );
        glGetConvolutionFilter = (PFNGLGETCONVOLUTIONFILTERPROC) getProcAddress( "glGetConvolutionFilter" );
        glGetConvolutionParameterfv = (PFNGLGETCONVOLUTIONPARAMETERFVPROC) getProcAddress( "glGetConvolutionParameterfv" );
        glGetConvolutionParameteriv = (PFNGLGETCONVOLUTIONPARAMETERIVPROC) getProcAddress( "glGetConvolutionParameteriv" );
        glGetSeparableFilter = (PFNGLGETSEPARABLEFILTERPROC) getProcAddress( "glGetSeparableFilter" );
        glSeparableFilter2D = (PFNGLSEPARABLEFILTER2DPROC) getProcAddress( "glSeparableFilter2D" );
        glGetHistogram = (PFNGLGETHISTOGRAMPROC) getProcAddress( "glGetHistogram" );
        glGetHistogramParameterfv = (PFNGLGETHISTOGRAMPARAMETERFVPROC) getProcAddress( "glGetHistogramParameterfv" );
        glGetHistogramParameteriv = (PFNGLGETHISTOGRAMPARAMETERIVPROC) getProcAddress( "glGetHistogramParameteriv" );
        glGetMinmax = (PFNGLGETMINMAXPROC) getProcAddress( "glGetMinmax" );
        glGetMinmaxParameterfv = (PFNGLGETMINMAXPARAMETERFVPROC) getProcAddress( "glGetMinmaxParameterfv" );
        glGetMinmaxParameteriv = (PFNGLGETMINMAXPARAMETERIVPROC) getProcAddress( "glGetMinmaxParameteriv" );
        glHistogram = (PFNGLHISTOGRAMPROC) getProcAddress( "glHistogram" );
        glMinmax = (PFNGLMINMAXPROC) getProcAddress( "glMinmax" );
        glResetHistogram = (PFNGLRESETHISTOGRAMPROC) getProcAddress( "glResetHistogram" );
        glResetMinmax = (PFNGLRESETMINMAXPROC) getProcAddress( "glResetMinmax" );
        glTexImage3D = (PFNGLTEXIMAGE3DPROC) getProcAddress( "glTexImage3D" );
        glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC) getProcAddress( "glTexSubImage3D" );
        glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC) getProcAddress( "glCopyTexSubImage3D" );
    }
#endif

#if defined( GL_VERSION_1_3 ) && defined( __OGLD_NEED_1_3 )
    if (_version >= Ver13)
    {
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) getProcAddress( "glActiveTexture" );
        glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) getProcAddress( "glClientActiveTexture" );
        glMultiTexCoord1d = (PFNGLMULTITEXCOORD1DPROC) getProcAddress( "glMultiTexCoord1d" );
        glMultiTexCoord1dv = (PFNGLMULTITEXCOORD1DVPROC) getProcAddress( "glMultiTexCoord1dv" );
        glMultiTexCoord1f = (PFNGLMULTITEXCOORD1FPROC) getProcAddress( "glMultiTexCoord1f" );
        glMultiTexCoord1fv = (PFNGLMULTITEXCOORD1FVPROC) getProcAddress( "glMultiTexCoord1fv" );
        glMultiTexCoord1i = (PFNGLMULTITEXCOORD1IPROC) getProcAddress( "glMultiTexCoord1i" );
        glMultiTexCoord1iv = (PFNGLMULTITEXCOORD1IVPROC) getProcAddress( "glMultiTexCoord1iv" );
        glMultiTexCoord1s = (PFNGLMULTITEXCOORD1SPROC) getProcAddress( "glMultiTexCoord1s" );
        glMultiTexCoord1sv = (PFNGLMULTITEXCOORD1SVPROC) getProcAddress( "glMultiTexCoord1sv" );
        glMultiTexCoord2d = (PFNGLMULTITEXCOORD2DPROC) getProcAddress( "glMultiTexCoord2d" );
        glMultiTexCoord2dv = (PFNGLMULTITEXCOORD2DVPROC) getProcAddress( "glMultiTexCoord2dv" );
        glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) getProcAddress( "glMultiTexCoord2f" );
        glMultiTexCoord2fv = (PFNGLMULTITEXCOORD2FVPROC) getProcAddress( "glMultiTexCoord2fv" );
        glMultiTexCoord2i = (PFNGLMULTITEXCOORD2IPROC) getProcAddress( "glMultiTexCoord2i" );
        glMultiTexCoord2iv = (PFNGLMULTITEXCOORD2IVPROC) getProcAddress( "glMultiTexCoord2iv" );
        glMultiTexCoord2s = (PFNGLMULTITEXCOORD2SPROC) getProcAddress( "glMultiTexCoord2s" );
        glMultiTexCoord2sv = (PFNGLMULTITEXCOORD2SVPROC) getProcAddress( "glMultiTexCoord2sv" );
        glMultiTexCoord3d = (PFNGLMULTITEXCOORD3DPROC) getProcAddress( "glMultiTexCoord3d" );
        glMultiTexCoord3dv = (PFNGLMULTITEXCOORD3DVPROC) getProcAddress( "glMultiTexCoord3dv" );
        glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FPROC) getProcAddress( "glMultiTexCoord3f" );
        glMultiTexCoord3fv = (PFNGLMULTITEXCOORD3FVPROC) getProcAddress( "glMultiTexCoord3fv" );
        glMultiTexCoord3i = (PFNGLMULTITEXCOORD3IPROC) getProcAddress( "glMultiTexCoord3i" );
        glMultiTexCoord3iv = (PFNGLMULTITEXCOORD3IVPROC) getProcAddress( "glMultiTexCoord3iv" );
        glMultiTexCoord3s = (PFNGLMULTITEXCOORD3SPROC) getProcAddress( "glMultiTexCoord3s" );
        glMultiTexCoord3sv = (PFNGLMULTITEXCOORD3SVPROC) getProcAddress( "glMultiTexCoord3sv" );
        glMultiTexCoord4d = (PFNGLMULTITEXCOORD4DPROC) getProcAddress( "glMultiTexCoord4d" );
        glMultiTexCoord4dv = (PFNGLMULTITEXCOORD4DVPROC) getProcAddress( "glMultiTexCoord4dv" );
        glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC) getProcAddress( "glMultiTexCoord4f" );
        glMultiTexCoord4fv = (PFNGLMULTITEXCOORD4FVPROC) getProcAddress( "glMultiTexCoord4fv" );
        glMultiTexCoord4i = (PFNGLMULTITEXCOORD4IPROC) getProcAddress( "glMultiTexCoord4i" );
        glMultiTexCoord4iv = (PFNGLMULTITEXCOORD4IVPROC) getProcAddress( "glMultiTexCoord4iv" );
        glMultiTexCoord4s = (PFNGLMULTITEXCOORD4SPROC) getProcAddress( "glMultiTexCoord4s" );
        glMultiTexCoord4sv = (PFNGLMULTITEXCOORD4SVPROC) getProcAddress( "glMultiTexCoord4sv" );
        glLoadTransposeMatrixf = (PFNGLLOADTRANSPOSEMATRIXFPROC) getProcAddress( "glLoadTransposeMatrixf" );
        glLoadTransposeMatrixd = (PFNGLLOADTRANSPOSEMATRIXDPROC) getProcAddress( "glLoadTransposeMatrixd" );
        glMultTransposeMatrixf = (PFNGLMULTTRANSPOSEMATRIXFPROC) getProcAddress( "glMultTransposeMatrixf" );
        glMultTransposeMatrixd = (PFNGLMULTTRANSPOSEMATRIXDPROC) getProcAddress( "glMultTransposeMatrixd" );
        glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC) getProcAddress( "glSampleCoverage" );
        glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) getProcAddress( "glCompressedTexImage3D" );
        glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) getProcAddress( "glCompressedTexImage2D" );
        glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) getProcAddress( "glCompressedTexImage1D" );
        glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) getProcAddress( "glCompressedTexSubImage3D" );
        glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) getProcAddress( "glCompressedTexSubImage2D" );
        glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) getProcAddress( "glCompressedTexSubImage1D" );
        glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) getProcAddress( "glGetCompressedTexImage" );
    }
#endif

#if defined( GL_VERSION_1_4 ) && defined( __OGLD_NEED_1_4 )
    if (_version >= Ver14)
    {
        glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) getProcAddress( "glBlendFuncSeparate" );
        glFogCoordf = (PFNGLFOGCOORDFPROC) getProcAddress( "glFogCoordf" );
        glFogCoordfv = (PFNGLFOGCOORDFVPROC) getProcAddress( "glFogCoordfv" );
        glFogCoordd = (PFNGLFOGCOORDDPROC) getProcAddress( "glFogCoordd" );
        glFogCoorddv = (PFNGLFOGCOORDDVPROC) getProcAddress( "glFogCoorddv" );
        glFogCoordPointer = (PFNGLFOGCOORDPOINTERPROC) getProcAddress( "glFogCoordPointer" );
        glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC) getProcAddress( "glMultiDrawArrays" );
        glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC) getProcAddress( "glMultiDrawElements" );
        glPointParameterf = (PFNGLPOINTPARAMETERFPROC) getProcAddress( "glPointParameterf" );
        glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC) getProcAddress( "glPointParameterfv" );
        glPointParameteri = (PFNGLPOINTPARAMETERIPROC) getProcAddress( "glPointParameteri" );
        glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC) getProcAddress( "glPointParameteriv" );
        glSecondaryColor3b = (PFNGLSECONDARYCOLOR3BPROC) getProcAddress( "glSecondaryColor3b" );
        glSecondaryColor3bv = (PFNGLSECONDARYCOLOR3BVPROC) getProcAddress( "glSecondaryColor3bv" );
        glSecondaryColor3d = (PFNGLSECONDARYCOLOR3DPROC) getProcAddress( "glSecondaryColor3d" );
        glSecondaryColor3dv = (PFNGLSECONDARYCOLOR3DVPROC) getProcAddress( "glSecondaryColor3dv" );
        glSecondaryColor3f = (PFNGLSECONDARYCOLOR3FPROC) getProcAddress( "glSecondaryColor3f" );
        glSecondaryColor3fv = (PFNGLSECONDARYCOLOR3FVPROC) getProcAddress( "glSecondaryColor3fv" );
        glSecondaryColor3i = (PFNGLSECONDARYCOLOR3IPROC) getProcAddress( "glSecondaryColor3i" );
        glSecondaryColor3iv = (PFNGLSECONDARYCOLOR3IVPROC) getProcAddress( "glSecondaryColor3iv" );
        glSecondaryColor3s = (PFNGLSECONDARYCOLOR3SPROC) getProcAddress( "glSecondaryColor3s" );
        glSecondaryColor3sv = (PFNGLSECONDARYCOLOR3SVPROC) getProcAddress( "glSecondaryColor3sv" );
        glSecondaryColor3ub = (PFNGLSECONDARYCOLOR3UBPROC) getProcAddress( "glSecondaryColor3ub" );
        glSecondaryColor3ubv = (PFNGLSECONDARYCOLOR3UBVPROC) getProcAddress( "glSecondaryColor3ubv" );
        glSecondaryColor3ui = (PFNGLSECONDARYCOLOR3UIPROC) getProcAddress( "glSecondaryColor3ui" );
        glSecondaryColor3uiv = (PFNGLSECONDARYCOLOR3UIVPROC) getProcAddress( "glSecondaryColor3uiv" );
        glSecondaryColor3us = (PFNGLSECONDARYCOLOR3USPROC) getProcAddress( "glSecondaryColor3us" );
        glSecondaryColor3usv = (PFNGLSECONDARYCOLOR3USVPROC) getProcAddress( "glSecondaryColor3usv" );
        glSecondaryColorPointer = (PFNGLSECONDARYCOLORPOINTERPROC) getProcAddress( "glSecondaryColorPointer" );
        glWindowPos2d = (PFNGLWINDOWPOS2DPROC) getProcAddress( "glWindowPos2d" );
        glWindowPos2dv = (PFNGLWINDOWPOS2DVPROC) getProcAddress( "glWindowPos2dv" );
        glWindowPos2f = (PFNGLWINDOWPOS2FPROC) getProcAddress( "glWindowPos2f" );
        glWindowPos2fv = (PFNGLWINDOWPOS2FVPROC) getProcAddress( "glWindowPos2fv" );
        glWindowPos2i = (PFNGLWINDOWPOS2IPROC) getProcAddress( "glWindowPos2i" );
        glWindowPos2iv = (PFNGLWINDOWPOS2IVPROC) getProcAddress( "glWindowPos2iv" );
        glWindowPos2s = (PFNGLWINDOWPOS2SPROC) getProcAddress( "glWindowPos2s" );
        glWindowPos2sv = (PFNGLWINDOWPOS2SVPROC) getProcAddress( "glWindowPos2sv" );
        glWindowPos3d = (PFNGLWINDOWPOS3DPROC) getProcAddress( "glWindowPos3d" );
        glWindowPos3dv = (PFNGLWINDOWPOS3DVPROC) getProcAddress( "glWindowPos3dv" );
        glWindowPos3f = (PFNGLWINDOWPOS3FPROC) getProcAddress( "glWindowPos3f" );
        glWindowPos3fv = (PFNGLWINDOWPOS3FVPROC) getProcAddress( "glWindowPos3fv" );
        glWindowPos3i = (PFNGLWINDOWPOS3IPROC) getProcAddress( "glWindowPos3i" );
        glWindowPos3iv = (PFNGLWINDOWPOS3IVPROC) getProcAddress( "glWindowPos3iv" );
        glWindowPos3s = (PFNGLWINDOWPOS3SPROC) getProcAddress( "glWindowPos3s" );
        glWindowPos3sv = (PFNGLWINDOWPOS3SVPROC) getProcAddress( "glWindowPos3sv" );
    }
#endif

#if defined( GL_VERSION_1_5 ) && defined( __OGLD_NEED_1_5 )
    if (_version >= Ver15)
    {
        glGenQueries = (PFNGLGENQUERIESPROC) getProcAddress( "glGenQueries" );
        glDeleteQueries = (PFNGLDELETEQUERIESPROC) getProcAddress( "glDeleteQueries" );
        glIsQuery = (PFNGLISQUERYPROC) getProcAddress( "glIsQuery" );
        glBeginQuery = (PFNGLBEGINQUERYPROC) getProcAddress( "glBeginQuery" );
        glEndQuery = (PFNGLENDQUERYPROC) getProcAddress( "glEndQuery" );
        glGetQueryiv = (PFNGLGETQUERYIVPROC) getProcAddress( "glGetQueryiv" );
        glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC) getProcAddress( "glGetQueryObjectiv" );
        glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC) getProcAddress( "glGetQueryObjectuiv" );
        glBindBuffer = (PFNGLBINDBUFFERPROC) getProcAddress( "glBindBuffer" );
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) getProcAddress( "glDeleteBuffers" );
        glGenBuffers = (PFNGLGENBUFFERSPROC) getProcAddress( "glGenBuffers" );
        glIsBuffer = (PFNGLISBUFFERPROC) getProcAddress( "glIsBuffer" );
        glBufferData = (PFNGLBUFFERDATAPROC) getProcAddress( "glBufferData" );
        glBufferSubData = (PFNGLBUFFERSUBDATAPROC) getProcAddress( "glBufferSubData" );
        glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC) getProcAddress( "glGetBufferSubData" );
        glMapBuffer = (PFNGLMAPBUFFERPROC) getProcAddress( "glMapBuffer" );
        glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) getProcAddress( "glUnmapBuffer" );
        glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC) getProcAddress( "glGetBufferParameteriv" );
        glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC) getProcAddress( "glGetBufferPointerv" );
    }
#endif

#if defined( GL_VERSION_2_0 ) && defined( __OGLD_NEED_2_0 )
    if (_version >= Ver20)
    {
        glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC) getProcAddress( "glBlendEquationSeparate" );
        glDrawBuffers = (PFNGLDRAWBUFFERSPROC) getProcAddress( "glDrawBuffers" );
        glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC) getProcAddress( "glStencilOpSeparate" );
        glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC) getProcAddress( "glStencilFuncSeparate" );
        glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC) getProcAddress( "glStencilMaskSeparate" );
        glAttachShader = (PFNGLATTACHSHADERPROC) getProcAddress( "glAttachShader" );
        glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) getProcAddress( "glBindAttribLocation" );
        glCompileShader = (PFNGLCOMPILESHADERPROC) getProcAddress( "glCompileShader" );
        glCreateProgram = (PFNGLCREATEPROGRAMPROC) getProcAddress( "glCreateProgram" );
        glCreateShader = (PFNGLCREATESHADERPROC) getProcAddress( "glCreateShader" );
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC) getProcAddress( "glDeleteProgram" );
        glDeleteShader = (PFNGLDELETESHADERPROC) getProcAddress( "glDeleteShader" );
        glDetachShader = (PFNGLDETACHSHADERPROC) getProcAddress( "glDetachShader" );
        glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) getProcAddress( "glDisableVertexAttribArray" );
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) getProcAddress( "glEnableVertexAttribArray" );
        glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) getProcAddress( "glGetActiveAttrib" );
        glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) getProcAddress( "glGetActiveUniform" );
        glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) getProcAddress( "glGetAttachedShaders" );
        glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) getProcAddress( "glGetAttribLocation" );
        glGetProgramiv = (PFNGLGETPROGRAMIVPROC) getProcAddress( "glGetProgramiv" );
        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) getProcAddress( "glGetProgramInfoLog" );
        glGetShaderiv = (PFNGLGETSHADERIVPROC) getProcAddress( "glGetShaderiv" );
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) getProcAddress( "glGetShaderInfoLog" );
        glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) getProcAddress( "glGetShaderSource" );
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) getProcAddress( "glGetUniformLocation" );
        glGetUniformfv = (PFNGLGETUNIFORMFVPROC) getProcAddress( "glGetUniformfv" );
        glGetUniformiv = (PFNGLGETUNIFORMIVPROC) getProcAddress( "glGetUniformiv" );
        glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC) getProcAddress( "glGetVertexAttribdv" );
        glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC) getProcAddress( "glGetVertexAttribfv" );
        glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC) getProcAddress( "glGetVertexAttribiv" );
        glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC) getProcAddress( "glGetVertexAttribPointerv" );
        glIsProgram = (PFNGLISPROGRAMPROC) getProcAddress( "glIsProgram" );
        glIsShader = (PFNGLISSHADERPROC) getProcAddress( "glIsShader" );
        glLinkProgram = (PFNGLLINKPROGRAMPROC) getProcAddress( "glLinkProgram" );
        glShaderSource = (PFNGLSHADERSOURCEPROC) getProcAddress( "glShaderSource" );
        glUseProgram = (PFNGLUSEPROGRAMPROC) getProcAddress( "glUseProgram" );
        glUniform1f = (PFNGLUNIFORM1FPROC) getProcAddress( "glUniform1f" );
        glUniform2f = (PFNGLUNIFORM2FPROC) getProcAddress( "glUniform2f" );
        glUniform3f = (PFNGLUNIFORM3FPROC) getProcAddress( "glUniform3f" );
        glUniform4f = (PFNGLUNIFORM4FPROC) getProcAddress( "glUniform4f" );
        glUniform1i = (PFNGLUNIFORM1IPROC) getProcAddress( "glUniform1i" );
        glUniform2i = (PFNGLUNIFORM2IPROC) getProcAddress( "glUniform2i" );
        glUniform3i = (PFNGLUNIFORM3IPROC) getProcAddress( "glUniform3i" );
        glUniform4i = (PFNGLUNIFORM4IPROC) getProcAddress( "glUniform4i" );
        glUniform1fv = (PFNGLUNIFORM1FVPROC) getProcAddress( "glUniform1fv" );
        glUniform2fv = (PFNGLUNIFORM2FVPROC) getProcAddress( "glUniform2fv" );
        glUniform3fv = (PFNGLUNIFORM3FVPROC) getProcAddress( "glUniform3fv" );
        glUniform4fv = (PFNGLUNIFORM4FVPROC) getProcAddress( "glUniform4fv" );
        glUniform1iv = (PFNGLUNIFORM1IVPROC) getProcAddress( "glUniform1iv" );
        glUniform2iv = (PFNGLUNIFORM2IVPROC) getProcAddress( "glUniform2iv" );
        glUniform3iv = (PFNGLUNIFORM3IVPROC) getProcAddress( "glUniform3iv" );
        glUniform4iv = (PFNGLUNIFORM4IVPROC) getProcAddress( "glUniform4iv" );
        glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC) getProcAddress( "glUniformMatrix2fv" );
        glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC) getProcAddress( "glUniformMatrix3fv" );
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) getProcAddress( "glUniformMatrix4fv" );
        glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) getProcAddress( "glValidateProgram" );
        glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC) getProcAddress( "glVertexAttrib1d" );
        glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC) getProcAddress( "glVertexAttrib1dv" );
        glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC) getProcAddress( "glVertexAttrib1f" );
        glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC) getProcAddress( "glVertexAttrib1fv" );
        glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC) getProcAddress( "glVertexAttrib1s" );
        glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC) getProcAddress( "glVertexAttrib1sv" );
        glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC) getProcAddress( "glVertexAttrib2d" );
        glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC) getProcAddress( "glVertexAttrib2dv" );
        glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC) getProcAddress( "glVertexAttrib2f" );
        glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC) getProcAddress( "glVertexAttrib2fv" );
        glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC) getProcAddress( "glVertexAttrib2s" );
        glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC) getProcAddress( "glVertexAttrib2sv" );
        glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC) getProcAddress( "glVertexAttrib3d" );
        glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC) getProcAddress( "glVertexAttrib3dv" );
        glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC) getProcAddress( "glVertexAttrib3f" );
        glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC) getProcAddress( "glVertexAttrib3fv" );
        glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC) getProcAddress( "glVertexAttrib3s" );
        glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC) getProcAddress( "glVertexAttrib3sv" );
        glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC) getProcAddress( "glVertexAttrib4Nbv" );
        glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC) getProcAddress( "glVertexAttrib4Niv" );
        glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC) getProcAddress( "glVertexAttrib4Nsv" );
        glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC) getProcAddress( "glVertexAttrib4Nub" );
        glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC) getProcAddress( "glVertexAttrib4Nubv" );
        glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC) getProcAddress( "glVertexAttrib4Nuiv" );
        glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC) getProcAddress( "glVertexAttrib4Nusv" );
        glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC) getProcAddress( "glVertexAttrib4bv" );
        glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC) getProcAddress( "glVertexAttrib4d" );
        glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC) getProcAddress( "glVertexAttrib4dv" );
        glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) getProcAddress( "glVertexAttrib4f" );
        glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC) getProcAddress( "glVertexAttrib4fv" );
        glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC) getProcAddress( "glVertexAttrib4iv" );
        glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC) getProcAddress( "glVertexAttrib4s" );
        glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC) getProcAddress( "glVertexAttrib4sv" );
        glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC) getProcAddress( "glVertexAttrib4ubv" );
        glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC) getProcAddress( "glVertexAttrib4uiv" );
        glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC) getProcAddress( "glVertexAttrib4usv" );
        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) getProcAddress( "glVertexAttribPointer" );
    }
#endif
}


void*
OGLDif::getProcAddress( const char* function )
{
#if defined( WIN32 ) || defined( __CYGWIN__ )

    return (void*) wglGetProcAddress( function );

#elif defined( GLX )

#ifdef GLX_VERSION_1_4
    return (void*) glXGetProcAddress( (const GLubyte*)function );
#else
    return (void*) dlsym( getDLHandle(), function );
#endif

#elif defined( __APPLE__ )

    // There are ways to get OpenGL entry point addresses on Apple,
    //   but this is usually not necessary and this example code
    //   doesn't support it.
    assert( false );

#else

    // Unsupported development platform
    assert( false );

#endif
}

bool
OGLDif::isExtensionSupported( const std::string& name, const GLubyte* extensions )
{
    std::string extStr( (const char*) extensions );

    while ( !extStr.empty() )
    {
        int idx = extStr.find_first_of( ' ' );
        std::string ext = extStr.substr( 0, idx );
        if (name == ext)
            return true;
        extStr.erase( 0, idx+1 );
    }

    return false;
}

int
OGLDif::getDataPathSize() const
{
    return _dataPath.size();
}

const std::string&
OGLDif::getDataPath( int idx ) const
{
    return _dataPath[ idx ];
}

void
OGLDif::addToDataPath( std::string path )
{
    int idx = _dataPath.size();
    _dataPath.resize( idx + 1 );
    _dataPath[ idx ] = path;
}

void
OGLDif::clearDataPath()
{
    _dataPath.empty();
}


void
OGLDif::checkError()
{
    const GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        const GLubyte* errorStr = gluErrorString( error );
        std::cerr << "Error: " << std::string( (char*)errorStr ) << std::endl;
    }
    assert( error == GL_NO_ERROR );
}


void*
OGLDif::getDLHandle()
{
#ifdef GLX

    if (!_dlHandle)
    {
        std::string libName( "/usr/lib/libGL.so" );
        _dlHandle = dlopen( libName.c_str(), RTLD_LAZY | RTLD_LOCAL );
    }

#endif

    return _dlHandle;
}


}
