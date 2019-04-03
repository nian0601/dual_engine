#define ASSERT(aCondition){ if(!(aCondition)) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "DE_Math.h"
#include "DE_Collision.h"
#include "DE_Collision.cpp"
#include "DE_Timer.cpp"
#include "DE_Input.cpp"
#include "gfx_interface.h"

#include "common_utils.cpp"
#include "array.cpp"
#include "heap.cpp"

#include "entity.h"
#include "entity.cpp"

#include "game.h"
#include "mapgenerator.cpp"
#include "game.cpp"

//#define USE_DIRECTX

#ifdef USE_DIRECTX
#include "directx.cpp"
#else
#include "opengl.cpp"
#endif

unsigned int LoadTexture(gfxTextureFormat aTextureFormat, const char* aFilePath)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(aFilePath, &width, &height, &nrChannels, 4);
    ASSERT(data != NULL);
    
    unsigned int texture = gfx_CreateTexture(width, height, aTextureFormat, data);
    
    stbi_image_free(data);
    return texture;
}

unsigned int CreateTextTexture()
{
    DE_File fontFile = DE_ReadEntireFile("c:/Windows/Fonts/arial.ttf");
    
    stbtt_fontinfo font;
    
    const unsigned char* fontBuffer = (unsigned char*)fontFile.myContents;
    stbtt_InitFont(&font, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));
    
    int width;
    int height;
    int xOffset;
    int yOffset;
    float fontHeight = 64.f;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &font,
        0,
        stbtt_ScaleForPixelHeight(&font, fontHeight), 
        'B',
        &width, 
        &height,
        &xOffset,
        &yOffset);

    unsigned int textureID = gfx_CreateTexture(width, height, SINGLE_CHANNEL, bitmap);
    stbtt_FreeBitmap(bitmap, NULL);
    
    return textureID;
}

LRESULT CALLBACK WndProc(HWND aHwnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
    switch (aMessage)
    {
		case WM_PAINT:
        { 
            PAINTSTRUCT ps;
            BeginPaint(aHwnd, &ps);
            EndPaint(aHwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_INPUT:
        {
            OnInputMessage(aWParam, aLParam);
            return 0;
        }
        case WM_SIZE:
        case WM_ACTIVATE:
		case WM_ENTERSIZEMOVE:
        case WM_EXITSIZEMOVE:
        {
            return 0;
        }
    }
    
    return DefWindowProcA(aHwnd, aMessage, aWParam, aLParam);
}

HWND Win32CreateWindow(const char* aTitle, int aWindowWidth, int aWindowHeight)
{
    WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
    
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(wcex.hInstance, NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = aTitle;
	wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);
    
    ASSERT(RegisterClassEx(&wcex) != FALSE);
    
    RECT rc = {0, 0, aWindowWidth, aWindowHeight};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    
    
    HWND windowHandle = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        aTitle,
        aTitle,
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    RECT windowSize;
    GetWindowRect(windowHandle, &windowSize);
    ASSERT(windowHandle != NULL);
    
    RegisterInputDevices(windowHandle);
    
    ShowWindow(windowHandle, 10);
    UpdateWindow(windowHandle);
    
    return windowHandle;
}

int main(int argc, char** argv)
{
    #ifdef USE_DIRECTX
    const char* windowTitle = "Dual Engine (DirectX)";
    #else
    const char* windowTitle = "Dual Engine (OpenGL)";
    #endif
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    Vector2f windowSize = {windowWidth, windowHeight};
    HWND windowHandle = Win32CreateWindow(windowTitle, windowWidth, windowHeight);
    gfx_Init(windowHandle, windowWidth, windowHeight);
    
    RECT clientArea;
    ASSERT(GetClientRect(windowHandle, &clientArea) != 0);
    
    
    unsigned int texture0 = LoadTexture(RGB, "container.jpg");
    unsigned int texture1  = LoadTexture(RGBA, "awesomeface.png");
    unsigned int fontTexture = CreateTextTexture();
    
    gfx_Viewport(0, 0, windowWidth, windowHeight);
    gfx_ClearColor(0.5f, 0.2f, 0.1f);
    
    MSG msg = {};
    bool isRunning = true;
    
    const float pi = 3.14159265f;
    
    gfx_camera myCamera;
    myCamera.myWindowSize.x = windowWidth;
    myCamera.myWindowSize.y = windowHeight;
    myCamera.myScreenSize.x = clientArea.right - clientArea.left;
    myCamera.myScreenSize.y = clientArea.bottom - clientArea.top;
    
    myCamera.myProjection = ProjectionMatrix(0.1f, 1000.f, float(windowHeight) / windowWidth, pi * 0.5f);
    myCamera.myView = IdentityMatrix();
    myCamera.myInvertedView = IdentityMatrix();

    SetTranslation(myCamera.myView, {30.f, 15.f, -35.f});
    myCamera.myView = myCamera.myView * RotationMatrixX(pi * 0.25f);
    
    gfx_SetCamera(&myCamera);
    
    DE_Timer frameTimer = GetTimer();

    SetupGame();
    
    while(isRunning)
    {
        UpdateInputState();
        UpdateTimer(frameTimer);
        float deltaTime = GetDeltaTime(frameTimer);
        
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            UINT msgCode = msg.message;
            if(msgCode == WM_QUIT || msgCode == WM_DESTROY)
                isRunning = false;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if(KeyDownThisFrame(DEK_ESCAPE))
            isRunning = false;
        
        
        UpdateCamera(deltaTime, myCamera);        
        UpdateAndRenderGame(deltaTime);
        
        
        gfx_Clear();
        
        gfx_DrawModels();
     
        gfx_DrawQuad(texture0, 50, 50, 300, 300); 
        gfx_DrawQuad(fontTexture, 350, 350, 400, 400); 
        gfx_DrawQuads();      
        
        
        
        gfx_FinishFrame();    
    }
    
    gfx_Shutdown();
    
    return 0;
}