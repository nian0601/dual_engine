#define ASSERT(aCondition){ if(!(aCondition)) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "math.h"
#include "collision.h"
#include "collision.cpp"
#include "timer.cpp"
#include "input.cpp"
#include "gfx_interface.h"
#include "common_utils.cpp"
#include "array.cpp"
#include "heap.cpp"
#include "assets.cpp"
#include "renderer.cpp"
#include "FastNoise.cpp"

#include "entity.h"
#include "entity.cpp"

#include "game.h"
#include "mapgenerator.cpp"
#include "game.cpp"
#include "voxel.cpp"

#include "opengl.cpp"

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
    const char* windowTitle = "Voxel Engine";
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    Vector2f windowSize = {windowWidth, windowHeight};
    HWND windowHandle = Win32CreateWindow(windowTitle, windowWidth, windowHeight);
    gfx_Init(windowHandle, windowWidth, windowHeight);
    
    RECT clientArea;
    ASSERT(GetClientRect(windowHandle, &clientArea) != 0);
        
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

    //myCamera.myView = myCamera.myView * RotationMatrixX(pi * 0.25f);
    //myCamera.myView = myCamera.myView * RotationMatrixY(pi * 0.25f);
    //SetTranslation(myCamera.myView, {-30.f, 120.f, -55.f});
    
    SetTranslation(myCamera.myView, {-10.f, 10.f, -20.f});
    
    gfx_SetCamera(&myCamera);
    
    DE_Timer frameTimer = GetTimer();

    SetupAssetStorage();
    SetupRenderer();
    SetupGame();
    CreateWorld();
    
    AssetInfo texture0 = GetBitmap("container.jpg");
    //AssetInfo texture0 = GetBitmap("awesomeface.png");
    
    
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
        //UpdateAndRenderGame(deltaTime);
        
        
        gfx_Clear();
        
        gfx_Begin3D();
        UpdateWorld();
        RenderWorld();
        
        
        
        /*
        QueueQuad(texture0.myTextureID, {200.f, 200.f}, texture0.mySize);
       
        QueueText({100.f, 300.f}, "TestSomeMore");
        QueueText({100.f, 500.f}, "WithSomeMore");
        QueueText({100.f, 700.f}, "ABCDEFGHIJKLMNOP");
        QueueText({100.f, 900.f}, "abcdefghijklmnop");
        */
        
        PushRendererData();
        gfx_FinishFrame();    
    }
    
    FreeAssets();
    gfx_Shutdown();
    
    return 0;
}