#include "UWindow.h"
#include "URenderer.h"

// Static members definition
bool UWindow::active;
bool UWindow::fullscreen;
bool UWindow::running;
HWND UWindow::g_hWnd = NULL;
HDC UWindow::g_hDC = NULL;

LRESULT CALLBACK UWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// input
static UKeys    g_input;

UWindow::UWindow()
{
    width = 1024;
    height = 768;
    fullscreen = false;
}

UWindow::~UWindow()
{
}

bool UWindow:: Create(LPCWSTR title, int width, int height, bool fullScreen)
{
    UWindow::width = width;
    UWindow::height = height;
    UWindow::fullscreen = fullScreen;

    WNDCLASSEX            wcx;
    PIXELFORMATDESCRIPTOR pfd;
    RECT                  rect;
    HGLRC                 hRCTemp;
    DWORD                 style, exStyle;
    int                   x, y, format;

    // Pointer to create context OpenGL function
    //PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

#ifdef UE_DEBUG
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB ,
        0
    };
#endif

#ifdef UE_RELEASE
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
#endif

    g_hInstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));

    //NULL to all keys
    memset(&g_input, 0, sizeof(g_input));

    // Window class registration
    memset(&wcx, 0, sizeof(wcx));
    wcx.cbSize        = sizeof(wcx);
    wcx.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcx.lpfnWndProc   = reinterpret_cast<WNDPROC>(UWindowProc);
    wcx.hInstance     = g_hInstance;
    wcx.lpszClassName = (LPCWSTR)UWND_CLASS_NAME.c_str();
    wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassEx(&wcx))
    {
        char message[MAXCHAR];
        sprintf_s(message,"RegisterClassEx fail (%d)", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        return false;
    }

    // Window styles
    style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    exStyle = WS_EX_APPWINDOW;

    // Centrate window
    x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    rect.left   = x; 
    rect.right  = x + width;
    rect.top    = y;
    rect.bottom = y + height;

    // Setup window size with styles
    AdjustWindowRectEx (&rect, style, FALSE, exStyle);

    // Create window
    g_hWnd = CreateWindowEx(exStyle, (LPCWSTR)UWND_CLASS_NAME.c_str(), title, style, rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, g_hInstance, NULL);

    if (!g_hWnd)
    {
        char message[MAXCHAR];
        sprintf_s(message,"CreateWindowEx fail (%d)", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        return false;
    }

    // ïGet window descriptor
    g_hDC = GetDC(g_hWnd);

    if (!g_hDC)
    {
        char message[MAXCHAR];
        sprintf_s(message,"GetDC fail (%d)", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        return false;
    }

    // Pixel format description
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    // Get pixel format for format which is described above
    format = ChoosePixelFormat(g_hDC, &pfd);
    if (!format || !SetPixelFormat(g_hDC, format, &pfd))
    {
        char message[MAXCHAR];
        sprintf_s(message,"Setting pixel format fail (%d)", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        return false;
    }

    // Create temp rendering context
    // It is neccessary for getting wglCreateContextAttribsARB function
    hRCTemp = wglCreateContext(g_hDC);
    if (!hRCTemp || !wglMakeCurrent(g_hDC, hRCTemp))
    {
        char message[MAXCHAR];
        sprintf_s(message,"Ñreating temp render context fail (%d)", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        return false;
    }

    GLenum err = glewInit();
    OPENGL_CHECK_FOR_ERRORS();
    // Get function address
    //wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
    //wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    //OPENGL_GET_PROC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
    
    // Delete temp context
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRCTemp);

    if (!wglCreateContextAttribsARB)
    {
        char message[MAXCHAR];
        sprintf_s(message,"wglCreateContextAttribsARB fail (%d), try to create context with wglCreateContext", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        //return false;
        
        // Create context with OpenGL 2 support
        g_hRC = wglCreateContext(g_hDC);
    }
    else
    {
        // Create extended context with OpenGL 3 support
        g_hRC = wglCreateContextAttribsARB(g_hDC, 0, attribs);
    }

    if (!g_hRC || !wglMakeCurrent(g_hDC, g_hRC))
    {
        char message[MAXCHAR];
        sprintf_s(message,"Creating render context fail (%d)", GetLastError());
        ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
        return false;
    }
    
    // Set up window size
    SetSize(width, height, fullScreen);

    ULogger::GetInstance()->Message("UWindow: Window is created succesfully");

    return true;
}

void UWindow::SetSize(int width, int height, bool isFullScreen)
{
    RECT    rect;
    DWORD   style, exStyle;
    DEVMODE devMode;
    LONG    result;
    int     x, y;

    // If we change from fullscreen mode
    if (fullscreen && !isFullScreen)
    {
        ChangeDisplaySettings(NULL, CDS_RESET);
        ShowCursor(TRUE);
    }

    fullscreen = isFullScreen;

    // Fullscreen mode
    if (fullscreen)
    {
        memset(&devMode, 0, sizeof(devMode));
        devMode.dmSize       = sizeof(devMode);
        devMode.dmPelsWidth  = width;
        devMode.dmPelsHeight = height;
        devMode.dmBitsPerPel = GetDeviceCaps(g_hDC, BITSPIXEL);
        devMode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Try to set fullscreen mode
        result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
        if (result != DISP_CHANGE_SUCCESSFUL)
        {
            char message[MAXCHAR];
            sprintf_s(message,"ChangeDisplaySettings fail %dx%d (%d)", width, height, result);
            ULogger::GetInstance()->Message(message, ULOG_MSG_ERROR);
            fullscreen = false;
        }
    }

    // If fullscreen mode setuped succesfully
    if (fullscreen)
    {
        ShowCursor(FALSE);
        style   = WS_POPUP;
        exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;

        x = y = 0;
    } 
    else 
    { //Window
        style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        exStyle = WS_EX_APPWINDOW;

        // Centralize window
        x = (GetSystemMetrics(SM_CXSCREEN) - width)  / 2;
        y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    }

    rect.left   = x;
    rect.right  = x + width;
    rect.top    = y;
    rect.bottom = y + height;

    //Setup styles
    AdjustWindowRectEx (&rect, style, FALSE, exStyle);

    SetWindowLong(g_hWnd, GWL_STYLE,   style);
    SetWindowLong(g_hWnd, GWL_EXSTYLE, exStyle);

    // Refresh window position
    SetWindowPos(g_hWnd, HWND_TOP, rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_FRAMECHANGED);

    // Show
    ShowWindow(g_hWnd, SW_SHOW);
    SetForegroundWindow(g_hWnd);
    SetFocus(g_hWnd);
    UpdateWindow(g_hWnd);

    // Get sizes of the window
    GetClientRect(g_hWnd, &rect);
    width  = rect.right - rect.left;
    height = rect.bottom - rect.top;

    // Setup viewport on the window
    OPENGL_CALL(glViewport(0, 0, width, height));

    // Centralize cursor in the center of the screen
    SetCursorPos(x + width / 2, y + height / 2);

    OPENGL_CHECK_FOR_ERRORS();
}

void UWindow:: Destroy()
{
    // Restore window size
    if (fullscreen)
    {
        ChangeDisplaySettings(NULL, CDS_RESET);
        ShowCursor(TRUE);
    }

    // Release renderer`s context 
    if (g_hRC)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(g_hRC);
    }

    // Release window context
    if (g_hDC)
        ReleaseDC(g_hWnd, g_hDC);

    // Destroy window
    if (g_hWnd)
        DestroyWindow(g_hWnd);

    // Release window class
    if (g_hInstance)
        UnregisterClassA((LPCSTR)UWND_CLASS_NAME.c_str(), g_hInstance);
}

LRESULT CALLBACK UWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        {
            g_input.cursorPos[0] = (int)LOWORD(lParam);
            g_input.cursorPos[1] = (int)HIWORD(lParam);

            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
                g_input.buttonState[0] = (msg == WM_LBUTTONDOWN ? UINPUT_PRESSED : UINPUT_UP);

            if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
                g_input.buttonState[1] = (msg == WM_RBUTTONDOWN ? UINPUT_PRESSED : UINPUT_UP);

            if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
                g_input.buttonState[2] = (msg == WM_MBUTTONDOWN ? UINPUT_PRESSED : UINPUT_UP);

            return FALSE;
        }

        case WM_MOUSEMOVE:
        {
            g_input.cursorPos[0] = (int)LOWORD(lParam);
            g_input.cursorPos[1] = (int)HIWORD(lParam);

            return FALSE;
        }
                
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            if (wParam < 256 && (lParam & 0x40000000) == 0)
                g_input.keyState[wParam] = UINPUT_PRESSED;

            return FALSE;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            if (wParam < 256)
                g_input.keyState[wParam] = UINPUT_UP;

            return FALSE;
        }

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            UWindow::SetActive(msg == WM_SETFOCUS);
            return FALSE;

        case WM_ACTIVATE:
            UWindow::SetActive(LOWORD(wParam) == WA_INACTIVE);
            return FALSE;

        case WM_CLOSE:
        {
            UWindow::SetActive(false);
            UWindow::SetRunning(false);

            PostQuitMessage(0);
            return FALSE;
        }
        case WM_SYSCOMMAND:
        {    switch (wParam & 0xFFF0)
            {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    if (UWindow::IsFullscreen())
                        return FALSE;
                    break;

                case SC_KEYMENU:
                    return FALSE;
            }
            break;
        }
        case WM_ERASEBKGND:
            return FALSE;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool UInput:: IsKeyDown(unsigned short int  key)
{
    return (g_input.keyState[key] != UINPUT_UP );
}

bool UInput::IsKeyPressed(unsigned short int  key)
{
    bool pressed = (g_input.keyState[key] == UINPUT_PRESSED);
    g_input.keyState[key] = UINPUT_DOWN;
    return pressed;
}

bool UInput:: IsButtonDown(unsigned short int  button)
{
    return (g_input.buttonState[button] != UINPUT_UP);
}

bool UInput:: IsButtonClick(unsigned short int  button)
{    
    bool pressed = (g_input.buttonState[button] == UINPUT_PRESSED);
    g_input.buttonState[button] = UINPUT_DOWN;
    return pressed;
}

void UInput:: GetCursorPos(int *x, int *y)
{
    *x = g_input.cursorPos[0];
    *y = g_input.cursorPos[1];
}

void UInput:: SetCursorPos(int x, int y)
{
    POINT pos = {x, y};
    ClientToScreen(URenderer::GetInstance()->GetHWND(), &pos);

    // bvahaha, it was bug, because I did not use "::" and there was a recursion //_-
    ::SetCursorPos(pos.x, pos.y);

    g_input.cursorPos[0] = x;
    g_input.cursorPos[1] = y;
}

void UInput:: ShowCursor(bool visible)
{
    ::ShowCursor(visible ? TRUE : FALSE);
}