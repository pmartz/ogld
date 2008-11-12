# Microsoft Developer Studio Project File - Name="OGLD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OGLD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OGLD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OGLD.mak" CFG="OGLD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OGLD - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OGLD - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OGLD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\OGLD\Release"
# PROP BASE Intermediate_Dir "..\OGLD\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\OGLD\lib"
# PROP Intermediate_Dir "..\OGLD\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\OGLD\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "OGLD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\OGLD\Debug"
# PROP BASE Intermediate_Dir "..\OGLD\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\OGLD\lib"
# PROP Intermediate_Dir "..\OGLD\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\OGLD\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\OGLD\lib\OGLDd.lib"

!ENDIF 

# Begin Target

# Name "OGLD - Win32 Release"
# Name "OGLD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\OGLD\OGLDAltAzView.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDCubeMap.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDif.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDMath.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDPixelBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDPixels.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDShape.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDTorus.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDTrackballView.cpp
# End Source File
# Begin Source File

SOURCE=..\OGLD\OGLDView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\OGLD\include\OGLDAltAzView.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDCubeMap.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDCylinder.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDif.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDMath.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDPixelBuffer.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDPixels.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDPlane.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDPlatformGL.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDPlatformGLU.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDPlatformGLUT.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDShape.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDSphere.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDTexture.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDTorus.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDTrackballView.h
# End Source File
# Begin Source File

SOURCE=..\OGLD\include\OGLDView.h
# End Source File
# End Group
# End Target
# End Project
