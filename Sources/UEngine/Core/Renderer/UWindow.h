#pragma once
#include "..\Utils\ULogger.h"
#include "URendererHelper.h"

struct UKeys
{

    UINPUT_KEY_STATE keyState[256];
    UINPUT_KEY_STATE buttonState[3];
    int     cursor_position[2];
};

class UInput
{
public:

    static bool IsKeyDown(unsigned short int key);
    static bool IsKeyPressed(unsigned short int key);
    static bool IsButtonDown(unsigned short int button);
    static bool IsButtonClick(unsigned short int button);

    static void GetCursorPos(int *x, int *y);
    static void SetCursorPos(int x, int y);
    static void ShowCursor(bool visible);

};

class UWindow
{
    HINSTANCE g_hInstance;
    HGLRC     g_hRC;

    static HWND      g_hWnd;
    static HDC       g_hDC;

    static const string UWND_CLASS_NAME;

    int width;
    int height;

    // States
    static bool fullscreen;
    static bool active;
    static bool running;

public:

    UWindow();
    ~UWindow();

    //Getting states functions
    static bool IsActive(){ return active; }
    static bool IsRunning(){ return running; }
    static bool IsFullscreen(){ return fullscreen; }

    //Setting states functions
    static void SetActive(bool value){ active = value; }
    static void SetRunning(bool value){ running = value; }
    static void SetFullscreen(bool value){ fullscreen = value; }

    static void SetWindowTitle(LPCWSTR lString){ SetWindowText(g_hWnd, lString); }
    // Getters for handles
    static HWND GetHWND(){ return g_hWnd; }
    static HDC GetHDC(){ return g_hDC; }

    //Getters for parameters
    int GetWidth(){ return width; }
    int GetHeight(){ return height; }

    // Create window
    bool Create(LPCWSTR title = L"UEngine", int width = 1024, int height = 768, bool isFullScreen = false);

    // Destroy window
    void Destroy();

    // Set window size
    void SetSize(int width, int height, bool isFullScreen = false);
};