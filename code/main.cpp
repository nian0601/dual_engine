#define ASSERT(aCondition){ if(!(aCondition)) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "glad/include/gl.h"
#include "glad/gl.c"
#include "glfw/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3native.h"

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

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void glfw_key_callback(GLFWwindow* aWindow, int aKey, int aScancode, int aAction, int someMods)
{
    OnGLFWKeyboardCallback(aKey, aAction);
}

void glfw_mousebutton_callback(GLFWwindow* aWindow, int aButton, int aAction, int someMods)
{
    OnGLFWMouseButtonCallback(aButton, aAction);
}

int main()
{
    const char* windowTitle = "Voxel Engine";
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetMouseButtonCallback(window, glfw_mousebutton_callback);
    glfwMakeContextCurrent(window);
    
    if(!gladLoaderLoadGL())
    {
        glfwTerminate();
        return -1;
    }
   
    HWND windowHandle = glfwGetWin32Window(window);
    gfx_Init(windowHandle, windowWidth, windowHeight);
    gfx_Viewport(0, 0, windowWidth, windowHeight);
    gfx_ClearColor(0.5f, 0.2f, 0.1f);
    
    const float pi = 3.14159265f;
    
    gfx_camera myCamera;
    myCamera.myWindowSize.x = windowWidth;
    myCamera.myWindowSize.y = windowHeight;
    myCamera.myScreenSize.x = windowWidth;
    myCamera.myScreenSize.y = windowHeight;
    
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
    
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();
 
        UpdateInputState(window);
        UpdateTimer(frameTimer);
        float deltaTime = GetDeltaTime(frameTimer);
        
        UpdateCamera(deltaTime, myCamera);        
        
        gfx_Clear();
        
        gfx_Begin3D();
        UpdateWorld();
        RenderWorld();
        
        PushRendererData();
        gfx_FinishFrame();    
        
        glfwSwapBuffers(window);
    }
    
    FreeAssets();
    gfx_Shutdown();

    glfwTerminate();
    return 0;
}