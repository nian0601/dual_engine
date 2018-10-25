#define ASSERT(aCondition){ if(!aCondition) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gfx_interface.h"

//#define USE_DIRECTX

#ifdef USE_DIRECTX
#include "directx.cpp"
#else
#include "opengl.cpp"
#endif

#define CORBET
#ifdef CORBET
#include "corbet.cpp"
#endif

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec3 vertexColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vertexColor = aColor;\n"
"   texCoord = aTexCoord;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 vertexColor;\n"
"in vec2 texCoord;\n"
"uniform sampler2D ourTexture1;\n"
"uniform sampler2D ourTexture2;\n"
"void main()\n"
"{\n"
"   FragColor = mix(texture(ourTexture1, texCoord), texture(ourTexture2, texCoord), 0.2);\n"
//"   FragColor = texture(ourTexture2, texCoord);\n"
//"   FragColor = vec4(vertexColor, 1.f); \n"
"}\n\0";

unsigned int LoadTexture(bool aUseAlpha, const char* aFilePath)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(aFilePath, &width, &height, &nrChannels, 0);
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
    gfx_Init(windowHandle);
    
    MSG msg = {};
    bool isRunning = true;
    
    corbetSetWindowSize(windowWidth, windowHeight);
    corbetInit();
    
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
    gfx_Init(windowHandle);
   
    //stbi_set_flip_vertically_on_load(true); 
    //unsigned int texture1 = LoadTexture(false, "container.jpg");
    //unsigned int texture2  = LoadTexture(true, "awesomeface.png");
    
    //unsigned int shaderProgram = gfx_CreateShader(vertexShaderSource, fragmentShaderSource);
    
    //gfx_Viewport(0, 0, windowWidth, windowHeight);
    //gfx_ClearColor(0.8f, 0.2f, 0.f);
    
    //gfx_BindShader(shaderProgram);

    //gfx_ShaderConstanti(shaderProgram, "ourTexture1", 0);
    //gfx_ShaderConstanti(shaderProgram, "ourTexture2", 1);
    
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
 
        //gfx_Clear();
        
        //gfx_BindTexture(texture1, 0);
        //gfx_BindTexture(texture2, 1);
        //gfx_DrawQuad();
    
        //gfx_FinishFrame();
      
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);      
        
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.0, windowWidth/(double)windowHeight, 0.1, 100.0);
        
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // This sets the position and direction of the camera
        // Place the camera at: 0, 0, 0
        // Point the camera towards: 0, 0, 10 (so make it point forward)
        // and we use Y as our up-axis
        gluLookAt(0, 0, 0,
                  0, 0, 10,
                  0, 1, 0);
        
      
        // Start the translations that are specific to the Quad
        glPushMatrix();
      
        // Move it away from the camera
        float distanceFromCamera = 2.f;
        glTranslatef(0.f, 0.f, distanceFromCamera);
        
        // Rotate it away from the camera
        glRotatef(30, 1, 0, 0);
        
        glBegin(GL_QUADS);
        
        glColor3f(1.0f, 0.0f,0.0f);
        float size = 0.5f;
        
        glVertex3f(-size, -size, 0.f);
        glVertex3f( size, -size, 0.f);
        glVertex3f( size,  size, 0.f);
        glVertex3f(-size,  size, 0.f);
        
        glEnd();
        
        // Stop the translations for the Quad
        glPopMatrix();
        
        
        // Push all the commands to the GPU
        glFinish();
        
    }
    
    return 0;
}
#endif