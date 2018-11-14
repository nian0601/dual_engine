#define ASSERT(aCondition){ if(!aCondition) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gfx_interface.h"
#include "common_utils.cpp"

struct Vector3f
{
    float x;
    float y;
    float z;
};

struct Vector2f
{
    float x;
    float y;
};

//#define USE_DIRECTX
//#define CORBET

#ifdef CORBET
#include "corbet.cpp"
#include "opengl.cpp"
#else
#ifdef USE_DIRECTX
#include "directx.cpp"
#else
#include "opengl.cpp"
#endif
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

#ifdef CORBET
int main(int argc, char** argv)
{
    const char* windowTitle = "Dual Engine";
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    HWND windowHandle = Win32CreateWindow(windowTitle, windowWidth, windowHeight);
    
    MSG msg = {};
    bool isRunning = true;
    
    corbetInit(windowHandle);
    corbetSetWindowSize(windowWidth, windowHeight);
    
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
        
        corbetUpdate();
        corbetRender();
    }
    
    return 0;
}
#else
int main(int argc, char** argv)
{
    const char* windowTitle = "Dual Engine";
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    HWND windowHandle = Win32CreateWindow(windowTitle, windowWidth, windowHeight);
    gfx_Init(windowHandle, windowWidth, windowHeight);
   
    //unsigned int texture1 = LoadTexture(false, "container.jpg");
    unsigned int texture1  = LoadTexture(true, "awesomeface.png");
    
    unsigned int shaderProgram = gfx_CreateHardcodedShader();
    
    gfx_Viewport(0, 0, windowWidth, windowHeight);
    gfx_ClearColor(0.8f, 0.2f, 0.f);
    
    gfx_BindShader(shaderProgram);

    gfx_ShaderConstanti(shaderProgram, "ourTexture1", 0);
    gfx_ShaderConstanti(shaderProgram, "ourTexture2", 1);
    
    MSG msg = {};
    bool isRunning = true;
    
    
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
 
        gfx_Clear();
        
        gfx_BindTexture(texture1, 0);
        //gfx_BindTexture(texture2, 1);
        gfx_DrawQuad();
    
        gfx_FinishFrame();    
    }
    
    gfx_Shutdown();
    
    return 0;
}
#endif