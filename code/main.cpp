#define ASSERT(aCondition){ if(!(aCondition)) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "DE_Timer.cpp"
#include "DE_Math.h"
#include "gfx_interface.h"
#include "common_utils.cpp"

#define USE_DIRECTX

#ifdef USE_DIRECTX
#include "directx.cpp"
#else
#include "opengl.cpp"
#endif

unsigned int LoadTexture(bool aUseAlpha, const char* aFilePath)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(aFilePath, &width, &height, &nrChannels, 4);
    ASSERT(data != NULL);
    
    unsigned int texture = gfx_CreateTexture(width, height, aUseAlpha, data);
    
    stbi_image_free(data);
    return texture;
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
    
    ASSERT(windowHandle != NULL);
    
    ShowWindow(windowHandle, 10);
    UpdateWindow(windowHandle);
    
    return windowHandle;
}

int main(int argc, char** argv)
{
    const char* windowTitle = "Dual Engine";
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    HWND windowHandle = Win32CreateWindow(windowTitle, windowWidth, windowHeight);
    gfx_Init(windowHandle, windowWidth, windowHeight);
    
    unsigned int texture0 = LoadTexture(false, "container.jpg");
    unsigned int texture1  = LoadTexture(true, "awesomeface.png");
    
    gfx_Viewport(0, 0, windowWidth, windowHeight);
    gfx_ClearColor(0.8f, 0.2f, 0.f);
    
    MSG msg = {};
    bool isRunning = true;
    
    const float pi = 3.14159265f;
    Matrix projection = ProjectionMatrix(0.1f, 100.f, float(windowHeight) / windowWidth, pi * 0.5f);
    Matrix view = IdentityMatrix();
    Translate(view, {0.f, 5.f, -7.f});
    
    Matrix roadTransform = IdentityMatrix(); 
    roadTransform *= ScaleMatrix({8.f, 2.f, 100.f});
    Translate(roadTransform, {0.f, 0.f, 50.f});
    
    Matrix tankTransform = IdentityMatrix(); 
    Translate(tankTransform, {0.f, 2.f, 0.f});
    
    DE_Timer frameTimer = GetTimer();
    while(isRunning)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            UINT msgCode = msg.message;
            if(msgCode == WM_QUIT || msgCode == WM_DESTROY)
                isRunning = false;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        UpdateTimer(frameTimer);
        float deltaTime = GetDeltaTime(frameTimer);
        
        gfx_Clear();
        
#if 1
        
        gfx_Begin3D();
        gfx_SetProjection(projection);
        gfx_SetView(view);
        gfx_CommitConstantData();
        gfx_DrawColoredCube(roadTransform, {0.2f, 0.13f, 0.04f, 1.f});        
        gfx_DrawColoredCube(tankTransform, {0.5f, 0.8f, 0.57f, 1.f});        
        
#else
        
        gfx_Begin2D();
        gfx_BindTexture(texture0, 0, "texture0");
        gfx_BindTexture(texture1, 1, "texture1");
        gfx_DrawQuad();        
        
#endif
        
        
        gfx_FinishFrame();    
    }
    
    gfx_Shutdown();
    
    return 0;
}