//
// http://www.opengldistilled.com
//
// This code was written by Paul Martz (pmartz@opengldistilled.com)
// (Apple-specific code was written by Brian Stempel.)
//
// This code comes with absolutely no warranty.
//
// You may freely use, distribute, modify, and publish this source
// code, provided that this comment block remains intact in all
// copies and derived works.
// 

#if defined( __APPLE__ )

  #include <Carbon/Carbon.h>
  #include <OpenGL/GLU.h>
  #include <AGL/agl.h>
// Uncomment this if you're compiling file as part of a carbon application
// in XCODE
//#define __XCODE__

#ifndef ____XCODE____
extern "C" {
	extern OSErr CPSEnableForegroundOperation(ProcessSerialNumber *psn);
}
#endif

#elif defined( GLX )

  #include <GL/glx.h>
  #include <GL/glu.h>
  #include <GL/gl.h>
  #include <X11/Xlib.h>
  #include <X11/Xatom.h>

#elif defined( WIN32 ) || defined( __CYGWIN__ )

  #include <windows.h>
  #include <GL/glu.h>
  #include <GL/gl.h>
  #include <GL/wglext.h>

#endif

#include "OGLDif.h"
#include "OGLDCylinder.h"
//#include <string>
#include <stdlib.h>
#include <assert.h>


// Global-scope variables
#if defined( __APPLE__ )

  AGLContext ctx = NULL;
  WindowRef win;

#elif defined( GLX )

#elif defined( WIN32 ) || defined( __CYGWIN__ )

  HGLRC hRC( NULL );

#endif

ogld::Cylinder* cyl = NULL;



//
// Platform-independent code

static void
display()
{
    // TBD need platform independent "startup" and
    //   "shutdown" routines, but they must get called
    //   once there is a current context.
    // This is currently a memory leak.
    if (!cyl)
        cyl = ogld::Cylinder::create();


    glClear( GL_COLOR_BUFFER_BIT );

    glLoadIdentity();
    glTranslatef( 0.f, 0.f, -4.f );

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    cyl->draw();

    OGLDIF_CHECK_ERROR;
}


static void
reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 50., (double)w/(double)h, 1., 100. );

    glMatrixMode( GL_MODELVIEW );

    OGLDIF_CHECK_ERROR;
}



//
// Platform-dependent code


#if defined( __APPLE__ )


void
initOGL()
{
    GLint attrib[] = { AGL_RGBA, AGL_DOUBLEBUFFER, AGL_NONE };

    AGLPixelFormat fmt;
    fmt = aglChoosePixelFormat( NULL, 0, attrib );

    ctx = aglCreateContext( fmt, NULL );
    SetPort( GetWindowPort( win ) );

    aglDestroyPixelFormat( fmt );

    aglSetDrawable( ctx, GetWindowPort( win ) );
    aglSetCurrentContext( ctx );
}

static pascal OSStatus
windowEvtHndlr (EventHandlerCallRef myHandler, EventRef event, void* userData)
{
    Rect rectPort = {0,0,0,0};
    UInt32 eventClass = GetEventClass( event );
    OSStatus result = eventNotHandledErr;
    UInt32 kind = GetEventKind( event );

    switch (eventClass)
    {
    case kEventClassWindow:
        GetEventParameter( event, kEventParamDirectObject, typeWindowRef,
                NULL, sizeof(WindowRef), NULL, &win );
        switch (kind)
        {
        case kEventWindowShown:
            initOGL();
            GetWindowPortBounds( win, &rectPort );
            reshape( rectPort.right-rectPort.left, rectPort.bottom-rectPort.top );

            display();
            aglSwapBuffers( ctx );

            return noErr;
            break;

        case kEventWindowDrawContent:
            if (ctx)
            {
                aglSetDrawable( ctx, GetWindowPort( win ) );
                aglSetCurrentContext( ctx );
                display();
                aglSwapBuffers( ctx );
            }
            return noErr;
            break;

        case kEventWindowClose:
            aglSetCurrentContext( NULL );
            aglDestroyContext( ctx );
            exit(0);
            return noErr;
            break;

        case kEventWindowBoundsChanged:
            aglSetCurrentContext( ctx );
            GetWindowPortBounds( win, &rectPort );
            reshape( rectPort.right-rectPort.left, rectPort.bottom-rectPort.top );
            InvalWindowRect( win,  &rectPort );
            return noErr;
            break;
        }
        break;
    }

    return result;
}

int
main( int argc, char *argv[] )
{
    IBNibRef nibRef = NULL;

    EventTypeSpec list[] = { { kEventClassWindow, kEventWindowClose },
            { kEventClassWindow, kEventWindowShown },
            { kEventClassWindow, kEventWindowDrawContent },
            { kEventClassWindow, kEventWindowBoundsChanged } };

#ifndef __XCODE__ /* omit this if using XCode */    
    ProcessSerialNumber psn;

    GetCurrentProcess(&psn);
    // This function is not documented by Apple.
    CPSEnableForegroundOperation(&psn);
    SetFrontProcess(&psn);
#endif

    WindowAttributes  windowAttrs;
    Rect              contentRect; 
    CFStringRef       titleKey;
    CFStringRef       windowTitle; 
    OSStatus          result; 
    
    windowAttrs = kWindowStandardDocumentAttributes // 1
        | kWindowStandardHandlerAttribute 
        | kWindowInWindowMenuAttribute; 
    SetRect (&contentRect, 100, 100, // 2
             400, 400);
    
    CreateNewWindow (kDocumentWindowClass, windowAttrs,// 3
                     &contentRect, &win);
    
    titleKey    = CFSTR("Platform"); // 4
    windowTitle = CFCopyLocalizedString(titleKey, NULL); // 5
    result = SetWindowTitleWithCFString (win, windowTitle); // 6                                                    
    CFRelease (titleKey); // 7
    CFRelease (windowTitle); 
    
    RepositionWindow (win, NULL, kWindowCascadeOnMainScreen); 
    
    EventTargetRef theTarget; 
    theTarget = GetWindowEventTarget( win );

    EventHandlerUPP gWinEvtHandler = NewEventHandlerUPP( windowEvtHndlr );
    InstallWindowEventHandler( win, gWinEvtHandler, GetEventTypeCount( list ), list, NULL, NULL );
    
    ShowWindow( win );
    
    RunApplicationEventLoop();

    return 0;
}



#elif defined( GLX )



void
glx13Init( Display* dpy, GLXWindow* win, GLXContext* ctx )
{
    int nelements;

    // Find a FBConfig that uses RGBA.  Note that no attribute list is
    //   needed since GLX_RGBA_BIT is a default attribute.
    GLXFBConfig* fbc = glXChooseFBConfig( dpy, DefaultScreen(dpy), 0, &nelements );
    XVisualInfo* vi = glXGetVisualFromFBConfig( dpy, fbc[0] );

    // Create a GLX context using the first FBConfig in the list.
    *ctx = glXCreateNewContext( dpy, fbc[0], GLX_RGBA_TYPE, 0, GL_TRUE );

    const Window root = RootWindow( dpy, vi->screen );

    // Create a window
    XSetWindowAttributes swa;
    swa.colormap = XCreateColormap( dpy, root, vi->visual, AllocNone );
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;
    Window xwin = XCreateWindow( dpy, root, 0, 0, 300, 300, 0,
            vi->depth, InputOutput, vi->visual,
            CWBorderPixel | CWColormap | CWEventMask, &swa );

    // Set the title in the window manager
    const char* title = "Simple GLX Example";
    XTextProperty xText;
    xText.value = (unsigned char *) title;
    xText.encoding = XA_STRING;
    xText.format = 8;
    xText.nitems = strlen( title );

    XWMHints* wmHints = XAllocWMHints();
    wmHints->initial_state = NormalState;
    wmHints->flags = StateHint;
    XSetWMProperties( dpy, xwin, &xText, &xText,
        NULL, 0, NULL, wmHints, NULL );
    XFree( wmHints );
    Atom atom;
    atom = XInternAtom( dpy, "WM_DELETE_WINDOW", False );
    XSetWMProtocols( dpy, xwin, &atom, 1 );

    // Show the window
    XMapWindow( dpy, xwin );


    // Create a GLX window using the same FBConfig that we used for the
    //   the GLX context.
    *win = glXCreateWindow( dpy, fbc[0], xwin, 0 );

    XFree( fbc );
}
 
int
main( int argc, char **argv )
{
    char* dpyName = getenv( "DISPLAY" );
    Display* dpy = XOpenDisplay( dpyName );
    if (!dpy)
        exit( 1 );

    int major, minor;
    if (!glXQueryVersion( dpy, &major, &minor ))
        exit( 1 );

    if (major != 1)
        exit( 1 );
    if (minor < 3)
        exit( 1 );

    GLXWindow win;
    GLXContext ctx;
    glx13Init( dpy, &win, &ctx );
    glXMakeContextCurrent( dpy, win, win, ctx );


    XSelectInput( dpy, win, ExposureMask |
            KeyPressMask | StructureNotifyMask );

    bool done( false );
    XEvent event;
    while (!done)
    {
        XNextEvent( dpy, &event );
        switch (event.type)
        {
        case ConfigureNotify:
            reshape( event.xconfigure.width, event.xconfigure.height );
            break;

        case KeyPress:
            done = true;
            break;

        default:
            break;
        }

        display();
        glXSwapBuffers( dpy, win );
    }

    XCloseDisplay( dpy );
}



#elif defined( WIN32 ) || defined( __CYGWIN__ )



void
initOGL( HWND hWnd )
{
    PIXELFORMATDESCRIPTOR pfd;
    memset( &pfd, 0, sizeof(PIXELFORMATDESCRIPTOR) );
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC hDC = GetDC( hWnd );
    int pfID = ChoosePixelFormat( hDC, &pfd );

    SetPixelFormat( hDC, pfID, &pfd );

    HGLRC initRC = wglCreateContext( hDC );
    assert( initRC );

    wglMakeCurrent( hDC, initRC );
    hRC = initRC;

#ifdef WGL_ARB_extensions_string

    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
    wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
            wglGetProcAddress( "wglGetExtensionsStringARB" );
    if (wglGetExtensionsStringARB != NULL)
    {
        const GLubyte* extStr = (const GLubyte*) wglGetExtensionsStringARB( hDC );
        bool usePixelFormat =
                ogld::OGLDif::instance()->isExtensionSupported(
                    "WGL_ARB_pixel_format", extStr );
#ifdef WGL_ARB_pixel_format
        if (usePixelFormat)
        {
            PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
            wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
                    wglGetProcAddress( "wglChoosePixelFormatARB" );
            assert( wglChoosePixelFormatARB != NULL );

            const int attribs[] = { WGL_COLOR_BITS_ARB, 24,
                    WGL_DEPTH_BITS_ARB, 16, WGL_DOUBLE_BUFFER_ARB, TRUE,
                    WGL_DRAW_TO_WINDOW_ARB, TRUE,
                    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 0 };
            int formats[10];
            UINT numFormats;
            BOOL result = wglChoosePixelFormatARB( hDC, attribs, NULL,
                    10, formats, &numFormats );
            assert( result );
            assert( numFormats > 0 );

            int ret;
            ret = SetPixelFormat( hDC, formats[1], &pfd );
            // TBD can't re-set the pixel format.
            // Remove this attempt to setPixelFormat.
            //   For now, just ignore the fact that it fails.
            //assert( ret == TRUE );

            hRC = wglCreateContext( hDC );
            assert( hRC );

            wglMakeCurrent( hDC, hRC );
        }
#endif
    }

#endif

    wglMakeCurrent( NULL, NULL );
    ReleaseDC( hWnd, hDC );
}


LONG WINAPI
wndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg)
    {
    case WM_CREATE:
    {
        initOGL( hWnd );
        return 0;
    }

    case WM_SIZE:
    {
        HDC hDC = GetDC (hWnd);
        wglMakeCurrent( hDC, hRC );

        reshape( (int)LOWORD( lParam ), (int)HIWORD( lParam ) );

        wglMakeCurrent( NULL, NULL );
        ReleaseDC( hWnd, hDC );

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint( hWnd, &ps );
        wglMakeCurrent( hDC, hRC );

        display();
        SwapBuffers( hDC );

        wglMakeCurrent( NULL, NULL );
        EndPaint( hWnd, &ps );
        return 0;
    }
        
    case WM_DESTROY:
    {
        assert( hRC );
        wglDeleteContext( hRC );
        hRC = NULL;

        PostQuitMessage( 0 );
        return 0;
    }
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

int
WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow )
{
    const char szAppName[] = "PlatformWGL";
    const char szTitle[] = "Simple WGL Example";

    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = 0;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;
    RegisterClass( &wc );

    HWND hWnd = CreateWindow( szAppName, szTitle,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 300,
        NULL, NULL, hInstance, NULL );

    if ( !hWnd )
        return 0;

    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    MSG msg;
    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return msg.wParam;
}


#endif
